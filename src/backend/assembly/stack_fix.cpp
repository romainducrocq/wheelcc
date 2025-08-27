#include <memory>

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
    vector_t(std::unique_ptr<AsmInstruction>) * p_fix_instrs;
} StackFixContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

typedef StackFixContext* Ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement

static std::shared_ptr<AsmData> pseudo_data(AsmPseudo* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmData>(name, 0l);
}

static std::shared_ptr<AsmData> pseudo_mem_data(AsmPseudoMem* node) {
    TIdentifier name = node->name;
    TLong offset = node->offset;
    return std::make_shared<AsmData>(name, offset);
}

static std::shared_ptr<AsmMemory> pseudo_memory(Ctx ctx, AsmPseudo* node) {
    TLong value = -1l * map_get(ctx->pseudo_stack_map, node->name);
    return gen_memory(REG_Bp, value);
}

static std::shared_ptr<AsmMemory> pseudo_mem_memory(Ctx ctx, AsmPseudoMem* node) {
    TLong value = -1l * (map_get(ctx->pseudo_stack_map, node->name) - node->offset);
    return gen_memory(REG_Bp, value);
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
    switch (asm_type->type()) {
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
    if (asm_type->type() == AST_ByteArray_t) {
        ByteArray* p_asm_type = static_cast<ByteArray*>(asm_type);
        align_offset_pseudo(ctx, p_asm_type->size, p_asm_type->alignment);
    }
    else {
        THROW_ABORT;
    }
}

static std::shared_ptr<AsmOperand> repl_pseudo_op(Ctx ctx, AsmPseudo* node) {
    if (map_find(ctx->pseudo_stack_map, node->name) == map_end()) {

        BackendObj* backend_obj = static_cast<BackendObj*>(map_get(ctx->backend->symbol_table, node->name).get());
        if (backend_obj->is_static) {
            return pseudo_data(node);
        }
        else {
            alloc_offset_pseudo(ctx, backend_obj->asm_type.get());
            map_add(ctx->pseudo_stack_map, node->name, ctx->stack_bytes);
        }
    }

    return pseudo_memory(ctx, node);
}

static std::shared_ptr<AsmOperand> repl_pseudo_mem_op(Ctx ctx, AsmPseudoMem* node) {
    if (map_find(ctx->pseudo_stack_map, node->name) == map_end()) {

        BackendObj* backend_obj = static_cast<BackendObj*>(map_get(ctx->backend->symbol_table, node->name).get());
        if (backend_obj->is_static) {
            return pseudo_mem_data(node);
        }
        else {
            alloc_offset_pseudo_mem(ctx, backend_obj->asm_type.get());
            map_add(ctx->pseudo_stack_map, node->name, ctx->stack_bytes);
        }
    }

    return pseudo_mem_memory(ctx, node);
}

static void repl_mov_pseudo(Ctx ctx, AsmMov* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_mov_sx_pseudo(Ctx ctx, AsmMovSx* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_zero_extend_pseudo(Ctx ctx, AsmMovZeroExtend* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_lea_pseudo(Ctx ctx, AsmLea* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_cvttsd2si_pseudo(Ctx ctx, AsmCvttsd2si* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_cvtsi2sd_pseudo(Ctx ctx, AsmCvtsi2sd* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_unary(Ctx ctx, AsmUnary* node) {
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_binary_pseudo(Ctx ctx, AsmBinary* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_cmp_pseudo(Ctx ctx, AsmCmp* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_idiv_pseudo(Ctx ctx, AsmIdiv* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_div_pseudo(Ctx ctx, AsmDiv* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_set_cc_pseudo(Ctx ctx, AsmSetCC* node) {
    switch (node->dst->type()) {
        case AST_AsmPseudo_t: {
            node->dst = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_push_pseudo(Ctx ctx, AsmPush* node) {
    switch (node->src->type()) {
        case AST_AsmPseudo_t: {
            node->src = repl_pseudo_op(ctx, static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(ctx, static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_pseudo_regs(Ctx ctx, AsmInstruction* node) {
    switch (node->type()) {
        case AST_AsmMov_t:
            repl_mov_pseudo(ctx, static_cast<AsmMov*>(node));
            break;
        case AST_AsmMovSx_t:
            repl_mov_sx_pseudo(ctx, static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            repl_zero_extend_pseudo(ctx, static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            repl_lea_pseudo(ctx, static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            repl_cvttsd2si_pseudo(ctx, static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            repl_cvtsi2sd_pseudo(ctx, static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmUnary_t:
            repl_unary(ctx, static_cast<AsmUnary*>(node));
            break;
        case AST_AsmBinary_t:
            repl_binary_pseudo(ctx, static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            repl_cmp_pseudo(ctx, static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            repl_idiv_pseudo(ctx, static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            repl_div_pseudo(ctx, static_cast<AsmDiv*>(node));
            break;
        case AST_AsmSetCC_t:
            repl_set_cc_pseudo(ctx, static_cast<AsmSetCC*>(node));
            break;
        case AST_AsmPush_t:
            repl_push_pseudo(ctx, static_cast<AsmPush*>(node));
            break;
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Instruction fix up

std::unique_ptr<AsmBinary> alloc_stack_bytes(TLong byte) {
    std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmSub>();
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src;
    {
        TULong value = (TULong)byte;
        bool is_byte = byte <= 127l && byte >= -128l;
        bool is_quad = byte > 2147483647l || byte < -2147483648l;
        bool is_neg = byte < 0l;
        src = std::make_shared<AsmImm>(value, is_byte, is_quad, is_neg);
    }
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Sp);
    return std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type), std::move(src), std::move(dst));
}

static void push_fix_instr(Ctx ctx, std::unique_ptr<AsmInstruction>&& instr) {
    vec_move_back(AsmInstruction, *ctx->p_fix_instrs, instr);
}

static void swap_fix_instr_back(Ctx ctx) {
    std::unique_ptr<AsmInstruction> swap_instr = std::move(vec_back(*ctx->p_fix_instrs));
    vec_back(*ctx->p_fix_instrs) = std::move((*ctx->p_fix_instrs)[vec_size(*ctx->p_fix_instrs) - 2]);
    (*ctx->p_fix_instrs)[vec_size(*ctx->p_fix_instrs) - 2] = std::move(swap_instr);
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

static void push_callee_saved_regs(Ctx ctx, const vector_t(std::shared_ptr<AsmOperand>) callee_saved_regs) {
    for (size_t i = 0; i < vec_size(callee_saved_regs); ++i) {
        std::shared_ptr<AsmOperand> src = callee_saved_regs[i];
        push_fix_instr(ctx, std::make_unique<AsmPush>(std::move(src)));
    }
}

static void pop_callee_saved_regs(Ctx ctx, const vector_t(std::shared_ptr<AsmOperand>) callee_saved_regs) {
    for (size_t i = vec_size(callee_saved_regs); i-- > 0;) {
        THROW_ABORT_IF(callee_saved_regs[i]->type() != AST_AsmRegister_t);
        REGISTER_KIND reg_kind = register_mask_kind(static_cast<AsmRegister*>(callee_saved_regs[i].get())->reg.get());
        std::unique_ptr<AsmReg> reg;
        switch (reg_kind) {
            case REG_Bx: {
                reg = std::make_unique<AsmBx>();
                break;
            }
            case REG_R12: {
                reg = std::make_unique<AsmR12>();
                break;
            }
            case REG_R13: {
                reg = std::make_unique<AsmR13>();
                break;
            }
            case REG_R14: {
                reg = std::make_unique<AsmR14>();
                break;
            }
            case REG_R15: {
                reg = std::make_unique<AsmR15>();
                break;
            }
            default:
                THROW_ABORT;
        }
        push_fix_instr(ctx, std::make_unique<AsmPop>(std::move(reg)));
    }
}

static bool is_op_addr(AsmOperand* node) {
    switch (node->type()) {
        case AST_AsmMemory_t:
        case AST_AsmData_t:
        case AST_AsmIndexed_t:
            return true;
        default:
            return false;
    }
}

static void mov_dbl_from_addr_to_addr(Ctx ctx, AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Xmm14);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void mov_from_quad_imm(Ctx ctx, AsmMov* node) {
    if (node->dst->type() == AST_AsmRegister_t) {
        if (node->asm_type->type() != AST_QuadWord_t) {
            node->asm_type = std::make_shared<QuadWord>();
        }
        return;
    }
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void mov_from_addr_to_addr(Ctx ctx, AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void fix_mov_instr(Ctx ctx, AsmMov* node) {
    if (node->asm_type->type() == AST_BackendDouble_t) {
        if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
            mov_dbl_from_addr_to_addr(ctx, node);
        }
    }
    else {
        if (node->src->type() == AST_AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
            mov_from_quad_imm(ctx, node);
        }
        if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
            mov_from_addr_to_addr(ctx, node);
        }
    }
}

static void mov_sx_from_imm(Ctx ctx, AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type_src;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void mov_sx_to_addr(Ctx ctx, AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REG_R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type_dst;
    node->dst = src;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
}

static void fix_mov_sx_instr(Ctx ctx, AsmMovSx* node) {
    if (node->src->type() == AST_AsmImm_t) {
        mov_sx_from_imm(ctx, node);
    }
    if (is_op_addr(node->dst.get())) {
        mov_sx_to_addr(ctx, node);
    }
}

static void byte_zero_extend_from_imm(Ctx ctx, AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<Byte>();
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void byte_zero_extend_to_addr(Ctx ctx, AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REG_R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type_dst;
    node->dst = src;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
}

static AsmMov* zero_extend_as_mov(Ctx ctx, AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<LongWord>();
    vec_back(*ctx->p_fix_instrs) = std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst));
    return static_cast<AsmMov*>(vec_back(*ctx->p_fix_instrs).get());
}

static void zero_extend_to_addr(Ctx ctx, AsmMov* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REG_R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
}

static void fix_zero_extend_instr(Ctx ctx, AsmMovZeroExtend* node) {
    if (node->asm_type_src->type() == AST_Byte_t) {
        if (node->src->type() == AST_AsmImm_t) {
            byte_zero_extend_from_imm(ctx, node);
        }
        if (is_op_addr(node->dst.get())) {
            byte_zero_extend_to_addr(ctx, node);
        }
    }
    else {
        AsmMov* mov = zero_extend_as_mov(ctx, node);
        if (is_op_addr(mov->dst.get())) {
            zero_extend_to_addr(ctx, mov);
        }
    }
}

static void lea_to_addr(Ctx ctx, AsmLea* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REG_R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
}

static void fix_lea_instr(Ctx ctx, AsmLea* node) {
    if (is_op_addr(node->dst.get())) {
        lea_to_addr(ctx, node);
    }
}

static void cvttsd2si_to_addr(Ctx ctx, AsmCvttsd2si* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REG_R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->dst = src;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
}

static void fix_cvttsd2si_instr(Ctx ctx, AsmCvttsd2si* node) {
    if (is_op_addr(node->dst.get())) {
        cvttsd2si_to_addr(ctx, node);
    }
}

static void cvtsi2sd_from_imm(Ctx ctx, AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void cvtsi2sd_to_addr(Ctx ctx, AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REG_Xmm15);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<BackendDouble>();
    node->dst = src;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
}

static void fix_cvtsi2sd_instr(Ctx ctx, AsmCvtsi2sd* node) {
    if (node->src->type() == AST_AsmImm_t) {
        cvtsi2sd_from_imm(ctx, node);
    }
    if (is_op_addr(node->dst.get())) {
        cvtsi2sd_to_addr(ctx, node);
    }
}

static void binary_dbl_to_addr(Ctx ctx, AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Xmm15);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    {
        std::shared_ptr<AsmOperand> src_cp = src;
        std::shared_ptr<AsmOperand> dst_cp = dst;
        std::shared_ptr<AssemblyType> asm_type_cp = asm_type;
        push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_cp), std::move(src_cp), std::move(dst_cp)));
    }
    swap_fix_instr_back(ctx);
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(dst), std::move(src)));
}

static void binary_from_quad_imm(Ctx ctx, AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void binary_from_addr_to_addr(Ctx ctx, AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void binary_imul_to_addr(Ctx ctx, AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R11);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->dst = dst;
    {
        std::shared_ptr<AsmOperand> src_cp = src;
        std::shared_ptr<AsmOperand> dst_cp = dst;
        std::shared_ptr<AssemblyType> asm_type_cp = asm_type;
        push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_cp), std::move(src_cp), std::move(dst_cp)));
    }
    swap_fix_instr_back(ctx);
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(dst), std::move(src)));
}

static void binary_shx_from_not_imm(Ctx ctx, AsmBinary* node) {
    if (node->src->type() == AST_AsmRegister_t
        && register_mask_kind(static_cast<AsmRegister*>(node->src.get())->reg.get()) == REG_Cx) {
        return;
    }
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Cx);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void fix_binary_instr(Ctx ctx, AsmBinary* node) {
    if (node->asm_type->type() == AST_BackendDouble_t) {
        if (is_op_addr(node->dst.get())) {
            binary_dbl_to_addr(ctx, node);
        }
    }
    else {
        switch (node->binop->type()) {
            case AST_AsmAdd_t:
            case AST_AsmSub_t:
            case AST_AsmBitAnd_t:
            case AST_AsmBitOr_t:
            case AST_AsmBitXor_t: {
                if (node->src->type() == AST_AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
                    binary_from_quad_imm(ctx, node);
                }
                if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
                    binary_from_addr_to_addr(ctx, node);
                }
                break;
            }
            case AST_AsmMult_t: {
                if (node->src->type() == AST_AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
                    binary_from_quad_imm(ctx, node);
                }
                if (is_op_addr(node->dst.get())) {
                    binary_imul_to_addr(ctx, node);
                }
                break;
            }
            case AST_AsmBitShiftLeft_t:
            case AST_AsmBitShiftRight_t:
            case AST_AsmBitShrArithmetic_t: {
                if (node->src->type() == AST_AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
                    binary_from_quad_imm(ctx, node);
                }
                if (node->src->type() != AST_AsmImm_t) {
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
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Xmm15);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void cmp_from_quad_imm(Ctx ctx, AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void cmp_from_addr_to_addr(Ctx ctx, AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void cmp_to_imm(Ctx ctx, AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R11);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->dst = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void fix_cmp_instr(Ctx ctx, AsmCmp* node) {
    if (node->asm_type->type() == AST_BackendDouble_t) {
        if (is_op_addr(node->dst.get())) {
            cmp_dbl_to_addr(ctx, node);
        }
    }
    else {
        if (node->src->type() == AST_AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
            cmp_from_quad_imm(ctx, node);
        }
        if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
            cmp_from_addr_to_addr(ctx, node);
        }
        else if (node->dst->type() == AST_AsmImm_t) {
            cmp_to_imm(ctx, node);
        }
    }
}

static void idiv_from_imm(Ctx ctx, AsmIdiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void fix_idiv_instr(Ctx ctx, AsmIdiv* node) {
    if (node->src->type() == AST_AsmImm_t) {
        idiv_from_imm(ctx, node);
    }
}

static void div_from_imm(Ctx ctx, AsmDiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = node->asm_type;
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void fix_div_instr(Ctx ctx, AsmDiv* node) {
    if (node->src->type() == AST_AsmImm_t) {
        div_from_imm(ctx, node);
    }
}

static void push_dbl_from_xmm_reg(Ctx ctx, AsmPush* node) {
    std::shared_ptr<AsmOperand> src_reg = std::move(node->src);
    std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
    {
        std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmSub>();
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Sp);
        std::shared_ptr<AssemblyType> asm_type_src_cp = asm_type_src;
        vec_back(*ctx->p_fix_instrs) =
            std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type_src_cp), std::move(src), std::move(dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_memory(REG_Sp, 0l);
        push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src_reg), std::move(dst)));
    }
}

static void push_from_quad_imm(Ctx ctx, AsmPush* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REG_R10);
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type), std::move(src), std::move(dst)));
    swap_fix_instr_back(ctx);
}

static void fix_push_instr(Ctx ctx, AsmPush* node) {
    if (node->src->type() == AST_AsmRegister_t) {
        REGISTER_KIND reg_kind = register_mask_kind(static_cast<AsmRegister*>(node->src.get())->reg.get());
        if (reg_kind != REG_Sp && register_mask_bit(reg_kind) > 11) {
            push_dbl_from_xmm_reg(ctx, node);
        }
    }
    else if (node->src->type() == AST_AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
        push_from_quad_imm(ctx, node);
    }
}

static void fix_instr(Ctx ctx, AsmInstruction* node) {
    switch (node->type()) {
        case AST_AsmMov_t:
            fix_mov_instr(ctx, static_cast<AsmMov*>(node));
            break;
        case AST_AsmMovSx_t:
            fix_mov_sx_instr(ctx, static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            fix_zero_extend_instr(ctx, static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            fix_lea_instr(ctx, static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            fix_cvttsd2si_instr(ctx, static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            fix_cvtsi2sd_instr(ctx, static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmBinary_t:
            fix_binary_instr(ctx, static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            fix_cmp_instr(ctx, static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            fix_idiv_instr(ctx, static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            fix_div_instr(ctx, static_cast<AsmDiv*>(node));
            break;
        case AST_AsmPush_t:
            fix_push_instr(ctx, static_cast<AsmPush*>(node));
            break;
        default:
            break;
    }
}

static void fix_fun_toplvl(Ctx ctx, AsmFunction* node) {
    vector_t(std::unique_ptr<AsmInstruction>) instructions = vec_new();
    vec_move(node->instructions, instructions);
    BackendFun* backend_fun = static_cast<BackendFun*>(map_get(ctx->backend->symbol_table, node->name).get());

    vec_clear(node->instructions);
    vec_reserve(node->instructions, vec_size(instructions));

    ctx->stack_bytes = node->is_ret_memory ? 8l : 0l;
    map_clear(ctx->pseudo_stack_map);
    ctx->p_fix_instrs = &node->instructions;
    // TODO
    std::unique_ptr<AsmInstruction> temp(nullptr);
    vec_move_back(AsmInstruction, *ctx->p_fix_instrs, temp);
    // vec_push_back(*ctx->p_fix_instrs, NULL);

    bool is_ret = false;
    push_callee_saved_regs(ctx, backend_fun->callee_saved_regs);
    for (size_t i = 0; i < vec_size(instructions); ++i) {
        if (instructions[i]) {
            if (instructions[i]->type() == AST_AsmRet_t) {
                pop_callee_saved_regs(ctx, backend_fun->callee_saved_regs);
                is_ret = true;
            }
            push_fix_instr(ctx, std::move(instructions[i]));

            repl_pseudo_regs(ctx, vec_back(*ctx->p_fix_instrs).get());
            fix_instr(ctx, vec_back(*ctx->p_fix_instrs).get());
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
    switch (node->type()) {
        case AST_AsmFunction_t:
            fix_fun_toplvl(ctx, static_cast<AsmFunction*>(node));
            break;
        case AST_AsmStaticVariable_t:
            break;
        default:
            THROW_ABORT;
    }
}

static void fix_program(Ctx ctx, AsmProgram* node) {
    for (size_t i = 0; i < vec_size(node->top_levels); ++i) {
        fix_toplvl(ctx, node->top_levels[i].get());
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
