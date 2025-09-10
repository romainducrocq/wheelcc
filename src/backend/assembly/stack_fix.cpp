#include "util/c_std.h"
#include "util/throw.h"

#include "ast/back_ast.h"
#include "ast/back_symt.h"
#include "ast_t.h" // ast

#include "backend/assembly/registers.h"
#include "backend/assembly/stack_fix.h"

PairKeyValue(TIdentifier, TLong);

typedef struct StackFixContext {
    BackEndContext* backend;
    // Pseudo register replacement
    TLong stack_bytes;
    hashmap_t(TIdentifier, TLong) pseudo_stack_map;
    // Instruction fix up
    vector_t(unique_ptr_t(AsmInstruction)) * p_fix_instrs;
} StackFixContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

typedef StackFixContext* Ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement

static void pseudo_data(AsmPseudo* node, shared_ptr_t(AsmOperand) * pseudo_op) {
    TIdentifier name = node->name;
    free_AsmOperand(pseudo_op);
    *pseudo_op = make_AsmData(name, 0l);
}

static void pseudo_mem_data(AsmPseudoMem* node, shared_ptr_t(AsmOperand) * pseudo_op) {
    TIdentifier name = node->name;
    TLong offset = node->offset;
    free_AsmOperand(pseudo_op);
    *pseudo_op = make_AsmData(name, offset);
}

static void pseudo_memory(Ctx ctx, AsmPseudo* node, shared_ptr_t(AsmOperand) * pseudo_op) {
    TLong value = -1l * map_get(ctx->pseudo_stack_map, node->name);
    free_AsmOperand(pseudo_op);
    *pseudo_op = gen_memory(REG_Bp, value);
}

static void pseudo_mem_memory(Ctx ctx, AsmPseudoMem* node, shared_ptr_t(AsmOperand) * pseudo_op) {
    TLong value = -1l * (map_get(ctx->pseudo_stack_map, node->name) - node->offset);
    free_AsmOperand(pseudo_op);
    *pseudo_op = gen_memory(REG_Bp, value);
}

static void align_offset_stack_bytes(Ctx ctx, TInt alignment) {
    TLong offset = ctx->stack_bytes % alignment;
    if (offset != 0l) {
        ctx->stack_bytes += alignment - offset;
    }
}

static void align_offset_pseudo(Ctx ctx, TLong size, TInt alignment) {
    ctx->stack_bytes += size;
    align_offset_stack_bytes(ctx, alignment);
}

static void alloc_offset_pseudo(Ctx ctx, AssemblyType* asm_type) {
    switch (asm_type->type) {
        case AST_Byte_t:
            align_offset_pseudo(ctx, 1l, 1);
            break;
        case AST_LongWord_t:
            align_offset_pseudo(ctx, 4l, 4);
            break;
        case AST_QuadWord_t:
        case AST_BackendDouble_t:
            align_offset_pseudo(ctx, 8l, 8);
            break;
        default:
            THROW_ABORT;
    }
}

static void alloc_offset_pseudo_mem(Ctx ctx, AssemblyType* asm_type) {
    if (asm_type->type == AST_ByteArray_t) {
        ByteArray* p_asm_type = &asm_type->get._ByteArray;
        align_offset_pseudo(ctx, p_asm_type->size, p_asm_type->alignment);
    }
    else {
        THROW_ABORT;
    }
}

static void repl_pseudo_op(Ctx ctx, AsmPseudo* node, shared_ptr_t(AsmOperand) * pseudo_op) {
    if (map_find(ctx->pseudo_stack_map, node->name) == map_end()) {

        BackendObj* backend_obj = &map_get(ctx->backend->symbol_table, node->name)->get._BackendObj;
        if (backend_obj->is_static) {
            pseudo_data(node, pseudo_op);
            return;
        }
        else {
            alloc_offset_pseudo(ctx, backend_obj->asm_type);
            map_add(ctx->pseudo_stack_map, node->name, ctx->stack_bytes);
        }
    }

    pseudo_memory(ctx, node, pseudo_op);
}

static void repl_pseudo_mem_op(Ctx ctx, AsmPseudoMem* node, shared_ptr_t(AsmOperand) * pseudo_op) {
    if (map_find(ctx->pseudo_stack_map, node->name) == map_end()) {

        BackendObj* backend_obj = &map_get(ctx->backend->symbol_table, node->name)->get._BackendObj;
        if (backend_obj->is_static) {
            pseudo_mem_data(node, pseudo_op);
            return;
        }
        else {
            alloc_offset_pseudo_mem(ctx, backend_obj->asm_type);
            map_add(ctx->pseudo_stack_map, node->name, ctx->stack_bytes);
        }
    }

    pseudo_mem_memory(ctx, node, pseudo_op);
}

static void repl_mov_pseudo(Ctx ctx, AsmMov* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_mov_sx_pseudo(Ctx ctx, AsmMovSx* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_zero_extend_pseudo(Ctx ctx, AsmMovZeroExtend* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_lea_pseudo(Ctx ctx, AsmLea* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_cvttsd2si_pseudo(Ctx ctx, AsmCvttsd2si* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_cvtsi2sd_pseudo(Ctx ctx, AsmCvtsi2sd* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_unary(Ctx ctx, AsmUnary* node) {
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_binary_pseudo(Ctx ctx, AsmBinary* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_cmp_pseudo(Ctx ctx, AsmCmp* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_idiv_pseudo(Ctx ctx, AsmIdiv* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
}

static void repl_div_pseudo(Ctx ctx, AsmDiv* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
}

static void repl_set_cc_pseudo(Ctx ctx, AsmSetCC* node) {
    switch (node->dst->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->dst->get._AsmPseudo, &node->dst);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->dst->get._AsmPseudoMem, &node->dst);
            break;
        default:
            break;
    }
}

static void repl_push_pseudo(Ctx ctx, AsmPush* node) {
    switch (node->src->type) {
        case AST_AsmPseudo_t:
            repl_pseudo_op(ctx, &node->src->get._AsmPseudo, &node->src);
            break;
        case AST_AsmPseudoMem_t:
            repl_pseudo_mem_op(ctx, &node->src->get._AsmPseudoMem, &node->src);
            break;
        default:
            break;
    }
}

static void repl_pseudo_regs(Ctx ctx, AsmInstruction* node) {
    switch (node->type) {
        case AST_AsmMov_t:
            repl_mov_pseudo(ctx, &node->get._AsmMov);
            break;
        case AST_AsmMovSx_t:
            repl_mov_sx_pseudo(ctx, &node->get._AsmMovSx);
            break;
        case AST_AsmMovZeroExtend_t:
            repl_zero_extend_pseudo(ctx, &node->get._AsmMovZeroExtend);
            break;
        case AST_AsmLea_t:
            repl_lea_pseudo(ctx, &node->get._AsmLea);
            break;
        case AST_AsmCvttsd2si_t:
            repl_cvttsd2si_pseudo(ctx, &node->get._AsmCvttsd2si);
            break;
        case AST_AsmCvtsi2sd_t:
            repl_cvtsi2sd_pseudo(ctx, &node->get._AsmCvtsi2sd);
            break;
        case AST_AsmUnary_t:
            repl_unary(ctx, &node->get._AsmUnary);
            break;
        case AST_AsmBinary_t:
            repl_binary_pseudo(ctx, &node->get._AsmBinary);
            break;
        case AST_AsmCmp_t:
            repl_cmp_pseudo(ctx, &node->get._AsmCmp);
            break;
        case AST_AsmIdiv_t:
            repl_idiv_pseudo(ctx, &node->get._AsmIdiv);
            break;
        case AST_AsmDiv_t:
            repl_div_pseudo(ctx, &node->get._AsmDiv);
            break;
        case AST_AsmSetCC_t:
            repl_set_cc_pseudo(ctx, &node->get._AsmSetCC);
            break;
        case AST_AsmPush_t:
            repl_push_pseudo(ctx, &node->get._AsmPush);
            break;
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Instruction fix up

unique_ptr_t(AsmInstruction) alloc_stack_bytes(TLong byte) {
    unique_ptr_t(AsmBinaryOp) binop = make_AsmSub();
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    shared_ptr_t(AsmOperand) src = sptr_new();
    {
        TULong value = (TULong)byte;
        bool is_byte = byte <= 127l && byte >= -128l;
        bool is_quad = byte > 2147483647l || byte < -2147483648l;
        bool is_neg = byte < 0l;
        src = make_AsmImm(value, is_byte, is_quad, is_neg);
    }
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Sp);
    return make_AsmBinary(&binop, &asm_type, &src, &dst);
}

static void push_fix_instr(Ctx ctx, unique_ptr_t(AsmInstruction) instr) { vec_move_back(*ctx->p_fix_instrs, instr); }

static void swap_fix_instr_back(Ctx ctx) {
    unique_ptr_t(AsmInstruction) swap_instr = uptr_new();
    unique_ptr_t(AsmInstruction)* instr_back_1 = &vec_back(*ctx->p_fix_instrs);
    unique_ptr_t(AsmInstruction)* instr_back_2 = &(*ctx->p_fix_instrs)[vec_size(*ctx->p_fix_instrs) - 2];
    uptr_move(AsmInstruction, *instr_back_1, swap_instr);
    uptr_move(AsmInstruction, *instr_back_2, *instr_back_1);
    uptr_move(AsmInstruction, swap_instr, *instr_back_2);
}

static void fix_alloc_stack_bytes(Ctx ctx, TLong callee_saved_size) {
    TLong callee_saved_bytes = callee_saved_size * 8l;
    ctx->stack_bytes += callee_saved_bytes;
    if (ctx->stack_bytes > 0l) {
        align_offset_stack_bytes(ctx, 16);
        ctx->stack_bytes -= callee_saved_bytes;
        (*ctx->p_fix_instrs)[0] = alloc_stack_bytes(ctx->stack_bytes);
    }
}

static void push_callee_saved_regs(Ctx ctx, vector_t(shared_ptr_t(AsmOperand)) callee_saved_regs) {
    for (size_t i = 0; i < vec_size(callee_saved_regs); ++i) {
        shared_ptr_t(AsmOperand) src = sptr_new();
        sptr_copy(AsmOperand, callee_saved_regs[i], src);
        push_fix_instr(ctx, make_AsmPush(&src));
    }
}

static void pop_callee_saved_regs(Ctx ctx, vector_t(shared_ptr_t(AsmOperand)) callee_saved_regs) {
    for (size_t i = vec_size(callee_saved_regs); i-- > 0;) {
        THROW_ABORT_IF(callee_saved_regs[i]->type != AST_AsmRegister_t);
        REGISTER_KIND reg_kind = register_mask_kind(callee_saved_regs[i]->get._AsmRegister.reg);
        unique_ptr_t(AsmReg) reg = uptr_new();
        switch (reg_kind) {
            case REG_Bx: {
                reg = make_AsmBx();
                break;
            }
            case REG_R12: {
                reg = make_AsmR12();
                break;
            }
            case REG_R13: {
                reg = make_AsmR13();
                break;
            }
            case REG_R14: {
                reg = make_AsmR14();
                break;
            }
            case REG_R15: {
                reg = make_AsmR15();
                break;
            }
            default:
                THROW_ABORT;
        }
        push_fix_instr(ctx, make_AsmPop(&reg));
    }
}

static bool is_op_addr(AsmOperand* node) {
    switch (node->type) {
        case AST_AsmMemory_t:
        case AST_AsmData_t:
        case AST_AsmIndexed_t:
            return true;
        default:
            return false;
    }
}

static void mov_dbl_from_addr_to_addr(Ctx ctx, AsmMov* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Xmm14);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void mov_from_quad_imm(Ctx ctx, AsmMov* node) {
    if (node->dst->type == AST_AsmRegister_t) {
        if (node->asm_type->type != AST_QuadWord_t) {
            free_AssemblyType(&node->asm_type);
            node->asm_type = make_QuadWord();
        }
        return;
    }
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void mov_from_addr_to_addr(Ctx ctx, AsmMov* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void fix_mov_instr(Ctx ctx, AsmMov* node) {
    if (node->asm_type->type == AST_BackendDouble_t) {
        if (is_op_addr(node->src) && is_op_addr(node->dst)) {
            mov_dbl_from_addr_to_addr(ctx, node);
        }
    }
    else {
        if (node->src->type == AST_AsmImm_t && node->src->get._AsmImm.is_quad) {
            mov_from_quad_imm(ctx, node);
        }
        if (is_op_addr(node->src) && is_op_addr(node->dst)) {
            mov_from_addr_to_addr(ctx, node);
        }
    }
}

static void mov_sx_from_imm(Ctx ctx, AsmMovSx* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type_src, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void mov_sx_to_addr(Ctx ctx, AsmMovSx* node) {
    shared_ptr_t(AsmOperand) src = gen_register(REG_R11);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    sptr_move(AsmOperand, node->dst, dst);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type_dst, asm_type);
    sptr_copy(AsmOperand, src, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
}

static void fix_mov_sx_instr(Ctx ctx, AsmMovSx* node) {
    if (node->src->type == AST_AsmImm_t) {
        mov_sx_from_imm(ctx, node);
    }
    if (is_op_addr(node->dst)) {
        mov_sx_to_addr(ctx, node);
    }
}

static void byte_zero_extend_from_imm(Ctx ctx, AsmMovZeroExtend* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = make_Byte();
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void byte_zero_extend_to_addr(Ctx ctx, AsmMovZeroExtend* node) {
    shared_ptr_t(AsmOperand) src = gen_register(REG_R11);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    sptr_move(AsmOperand, node->dst, dst);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type_dst, asm_type);
    sptr_copy(AsmOperand, src, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
}

static AsmMov* zero_extend_as_mov(Ctx ctx, AsmMovZeroExtend* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    sptr_move(AsmOperand, node->dst, dst);
    shared_ptr_t(AssemblyType) asm_type = make_LongWord();
    unique_ptr_t(AsmInstruction)* instr_back = &vec_back(*ctx->p_fix_instrs);
    free_AsmInstruction(instr_back);
    *instr_back = make_AsmMov(&asm_type, &src, &dst);
    return &(*instr_back)->get._AsmMov;
}

static void zero_extend_to_addr(Ctx ctx, AsmMov* node) {
    shared_ptr_t(AsmOperand) src = gen_register(REG_R11);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    sptr_move(AsmOperand, node->dst, dst);
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    sptr_copy(AsmOperand, src, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
}

static void fix_zero_extend_instr(Ctx ctx, AsmMovZeroExtend* node) {
    if (node->asm_type_src->type == AST_Byte_t) {
        if (node->src->type == AST_AsmImm_t) {
            byte_zero_extend_from_imm(ctx, node);
        }
        if (is_op_addr(node->dst)) {
            byte_zero_extend_to_addr(ctx, node);
        }
    }
    else {
        AsmMov* mov = zero_extend_as_mov(ctx, node);
        if (is_op_addr(mov->dst)) {
            zero_extend_to_addr(ctx, mov);
        }
    }
}

static void lea_to_addr(Ctx ctx, AsmLea* node) {
    shared_ptr_t(AsmOperand) src = gen_register(REG_R11);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    sptr_move(AsmOperand, node->dst, dst);
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    sptr_copy(AsmOperand, src, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
}

static void fix_lea_instr(Ctx ctx, AsmLea* node) {
    if (is_op_addr(node->dst)) {
        lea_to_addr(ctx, node);
    }
}

static void cvttsd2si_to_addr(Ctx ctx, AsmCvttsd2si* node) {
    shared_ptr_t(AsmOperand) src = gen_register(REG_R11);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    sptr_move(AsmOperand, node->dst, dst);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, src, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
}

static void fix_cvttsd2si_instr(Ctx ctx, AsmCvttsd2si* node) {
    if (is_op_addr(node->dst)) {
        cvttsd2si_to_addr(ctx, node);
    }
}

static void cvtsi2sd_from_imm(Ctx ctx, AsmCvtsi2sd* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void cvtsi2sd_to_addr(Ctx ctx, AsmCvtsi2sd* node) {
    shared_ptr_t(AsmOperand) src = gen_register(REG_Xmm15);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    sptr_move(AsmOperand, node->dst, dst);
    shared_ptr_t(AssemblyType) asm_type = make_BackendDouble();
    sptr_copy(AsmOperand, src, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
}

static void fix_cvtsi2sd_instr(Ctx ctx, AsmCvtsi2sd* node) {
    if (node->src->type == AST_AsmImm_t) {
        cvtsi2sd_from_imm(ctx, node);
    }
    if (is_op_addr(node->dst)) {
        cvtsi2sd_to_addr(ctx, node);
    }
}

static void binary_dbl_to_addr(Ctx ctx, AsmBinary* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->dst, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Xmm15);
    shared_ptr_t(AssemblyType) asm_type = make_BackendDouble();
    sptr_copy(AsmOperand, dst, node->dst);
    {
        shared_ptr_t(AsmOperand) src_cp = sptr_new();
        sptr_copy(AsmOperand, src, src_cp);
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        shared_ptr_t(AssemblyType) asm_type_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type, asm_type_cp);
        push_fix_instr(ctx, make_AsmMov(&asm_type_cp, &src_cp, &dst_cp));
    }
    swap_fix_instr_back(ctx);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &dst, &src));
}

static void binary_from_quad_imm(Ctx ctx, AsmBinary* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void binary_from_addr_to_addr(Ctx ctx, AsmBinary* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void binary_imul_to_addr(Ctx ctx, AsmBinary* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->dst, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R11);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->dst);
    {
        shared_ptr_t(AsmOperand) src_cp = sptr_new();
        sptr_copy(AsmOperand, src, src_cp);
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        shared_ptr_t(AssemblyType) asm_type_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type, asm_type_cp);
        push_fix_instr(ctx, make_AsmMov(&asm_type_cp, &src_cp, &dst_cp));
    }
    swap_fix_instr_back(ctx);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &dst, &src));
}

static void binary_shx_from_not_imm(Ctx ctx, AsmBinary* node) {
    if (node->src->type == AST_AsmRegister_t && register_mask_kind(node->src->get._AsmRegister.reg) == REG_Cx) {
        return;
    }
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Cx);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void fix_binary_instr(Ctx ctx, AsmBinary* node) {
    if (node->asm_type->type == AST_BackendDouble_t) {
        if (is_op_addr(node->dst)) {
            binary_dbl_to_addr(ctx, node);
        }
    }
    else {
        switch (node->binop->type) {
            case AST_AsmAdd_t:
            case AST_AsmSub_t:
            case AST_AsmBitAnd_t:
            case AST_AsmBitOr_t:
            case AST_AsmBitXor_t: {
                if (node->src->type == AST_AsmImm_t && node->src->get._AsmImm.is_quad) {
                    binary_from_quad_imm(ctx, node);
                }
                if (is_op_addr(node->src) && is_op_addr(node->dst)) {
                    binary_from_addr_to_addr(ctx, node);
                }
                break;
            }
            case AST_AsmMult_t: {
                if (node->src->type == AST_AsmImm_t && node->src->get._AsmImm.is_quad) {
                    binary_from_quad_imm(ctx, node);
                }
                if (is_op_addr(node->dst)) {
                    binary_imul_to_addr(ctx, node);
                }
                break;
            }
            case AST_AsmBitShiftLeft_t:
            case AST_AsmBitShiftRight_t:
            case AST_AsmBitShrArithmetic_t: {
                if (node->src->type == AST_AsmImm_t && node->src->get._AsmImm.is_quad) {
                    binary_from_quad_imm(ctx, node);
                }
                if (node->src->type != AST_AsmImm_t) {
                    binary_shx_from_not_imm(ctx, node);
                }
                break;
            }
            default:
                break;
        }
    }
}

static void cmp_dbl_to_addr(Ctx ctx, AsmCmp* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->dst, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Xmm15);
    shared_ptr_t(AssemblyType) asm_type = make_BackendDouble();
    sptr_copy(AsmOperand, dst, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void cmp_from_quad_imm(Ctx ctx, AsmCmp* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void cmp_from_addr_to_addr(Ctx ctx, AsmCmp* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void cmp_to_imm(Ctx ctx, AsmCmp* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->dst, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R11);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->dst);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void fix_cmp_instr(Ctx ctx, AsmCmp* node) {
    if (node->asm_type->type == AST_BackendDouble_t) {
        if (is_op_addr(node->dst)) {
            cmp_dbl_to_addr(ctx, node);
        }
    }
    else {
        if (node->src->type == AST_AsmImm_t && node->src->get._AsmImm.is_quad) {
            cmp_from_quad_imm(ctx, node);
        }
        if (is_op_addr(node->src) && is_op_addr(node->dst)) {
            cmp_from_addr_to_addr(ctx, node);
        }
        else if (node->dst->type == AST_AsmImm_t) {
            cmp_to_imm(ctx, node);
        }
    }
}

static void idiv_from_imm(Ctx ctx, AsmIdiv* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void fix_idiv_instr(Ctx ctx, AsmIdiv* node) {
    if (node->src->type == AST_AsmImm_t) {
        idiv_from_imm(ctx, node);
    }
}

static void div_from_imm(Ctx ctx, AsmDiv* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = sptr_new();
    sptr_copy(AssemblyType, node->asm_type, asm_type);
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void fix_div_instr(Ctx ctx, AsmDiv* node) {
    if (node->src->type == AST_AsmImm_t) {
        div_from_imm(ctx, node);
    }
}

static void push_dbl_from_xmm_reg(Ctx ctx, AsmPush* node) {
    shared_ptr_t(AsmOperand) src_reg = sptr_new();
    sptr_move(AsmOperand, node->src, src_reg);
    shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
    {
        unique_ptr_t(AsmBinaryOp) binop = make_AsmSub();
        shared_ptr_t(AsmOperand) src = make_AsmImm(8ul, true, false, false);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Sp);
        shared_ptr_t(AssemblyType) asm_type_src_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src, asm_type_src_cp);
        unique_ptr_t(AsmInstruction)* instr_back = &vec_back(*ctx->p_fix_instrs);
        free_AsmInstruction(instr_back);
        *instr_back = make_AsmBinary(&binop, &asm_type_src_cp, &src, &dst);
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_memory(REG_Sp, 0l);
        push_fix_instr(ctx, make_AsmMov(&asm_type_src, &src_reg, &dst));
    }
}

static void push_from_quad_imm(Ctx ctx, AsmPush* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    sptr_move(AsmOperand, node->src, src);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_R10);
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    sptr_copy(AsmOperand, dst, node->src);
    push_fix_instr(ctx, make_AsmMov(&asm_type, &src, &dst));
    swap_fix_instr_back(ctx);
}

static void fix_push_instr(Ctx ctx, AsmPush* node) {
    if (node->src->type == AST_AsmRegister_t) {
        REGISTER_KIND reg_kind = register_mask_kind(node->src->get._AsmRegister.reg);
        if (reg_kind != REG_Sp && register_mask_bit(reg_kind) > 11) {
            push_dbl_from_xmm_reg(ctx, node);
        }
    }
    else if (node->src->type == AST_AsmImm_t && node->src->get._AsmImm.is_quad) {
        push_from_quad_imm(ctx, node);
    }
}

static void fix_instr(Ctx ctx, AsmInstruction* node) {
    switch (node->type) {
        case AST_AsmMov_t:
            fix_mov_instr(ctx, &node->get._AsmMov);
            break;
        case AST_AsmMovSx_t:
            fix_mov_sx_instr(ctx, &node->get._AsmMovSx);
            break;
        case AST_AsmMovZeroExtend_t:
            fix_zero_extend_instr(ctx, &node->get._AsmMovZeroExtend);
            break;
        case AST_AsmLea_t:
            fix_lea_instr(ctx, &node->get._AsmLea);
            break;
        case AST_AsmCvttsd2si_t:
            fix_cvttsd2si_instr(ctx, &node->get._AsmCvttsd2si);
            break;
        case AST_AsmCvtsi2sd_t:
            fix_cvtsi2sd_instr(ctx, &node->get._AsmCvtsi2sd);
            break;
        case AST_AsmBinary_t:
            fix_binary_instr(ctx, &node->get._AsmBinary);
            break;
        case AST_AsmCmp_t:
            fix_cmp_instr(ctx, &node->get._AsmCmp);
            break;
        case AST_AsmIdiv_t:
            fix_idiv_instr(ctx, &node->get._AsmIdiv);
            break;
        case AST_AsmDiv_t:
            fix_div_instr(ctx, &node->get._AsmDiv);
            break;
        case AST_AsmPush_t:
            fix_push_instr(ctx, &node->get._AsmPush);
            break;
        default:
            break;
    }
}

static void fix_fun_toplvl(Ctx ctx, AsmFunction* node) {
    vector_t(unique_ptr_t(AsmInstruction)) instructions = vec_new();
    vec_move(node->instructions, instructions);
    BackendFun* backend_fun = &map_get(ctx->backend->symbol_table, node->name)->get._BackendFun;

    vec_clear(node->instructions);
    vec_reserve(node->instructions, vec_size(instructions));

    ctx->stack_bytes = node->is_ret_memory ? 8l : 0l;
    map_clear(ctx->pseudo_stack_map);
    ctx->p_fix_instrs = &node->instructions;
    vec_push_back(*ctx->p_fix_instrs, uptr_new());

    bool is_ret = false;
    push_callee_saved_regs(ctx, backend_fun->callee_saved_regs);
    for (size_t i = 0; i < vec_size(instructions); ++i) {
        if (instructions[i]) {
            if (instructions[i]->type == AST_AsmRet_t) {
                pop_callee_saved_regs(ctx, backend_fun->callee_saved_regs);
                is_ret = true;
            }
            push_fix_instr(ctx, instructions[i]);
            instructions[i] = uptr_new();

            repl_pseudo_regs(ctx, vec_back(*ctx->p_fix_instrs));
            fix_instr(ctx, vec_back(*ctx->p_fix_instrs));
        }
    }
    if (!is_ret) {
        pop_callee_saved_regs(ctx, backend_fun->callee_saved_regs);
    }
    {
        TLong callee_saved_size = (TLong)vec_size(backend_fun->callee_saved_regs);
        fix_alloc_stack_bytes(ctx, callee_saved_size);
    }
    ctx->p_fix_instrs = NULL;
    vec_delete(instructions);
}

static void fix_toplvl(Ctx ctx, AsmTopLevel* node) {
    switch (node->type) {
        case AST_AsmFunction_t:
            fix_fun_toplvl(ctx, &node->get._AsmFunction);
            break;
        case AST_AsmStaticVariable_t:
            break;
        default:
            THROW_ABORT;
    }
}

static void fix_program(Ctx ctx, AsmProgram* node) {
    for (size_t i = 0; i < vec_size(node->top_levels); ++i) {
        fix_toplvl(ctx, node->top_levels[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fix_stack(AsmProgram* node, BackEndContext* backend) {
    StackFixContext ctx;
    {
        ctx.backend = backend;
        ctx.stack_bytes = 0l;
        ctx.pseudo_stack_map = map_new();
    }
    fix_program(&ctx, node);

    map_delete(ctx.pseudo_stack_map);
}
