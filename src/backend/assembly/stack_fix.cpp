#include <memory>
#include <unordered_map>
#include <vector>

#include "util/throw.hpp"

#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast_t.hpp" // ast

#include "backend/assembly/registers.hpp"
#include "backend/assembly/stack_fix.hpp"

struct StackFixContext {
    StackFixContext();

    // Pseudo register replacement
    TLong stack_bytes;
    std::unordered_map<TIdentifier, TLong> pseudo_stack_bytes_map;
    // Instruction fix up
    std::vector<std::unique_ptr<AsmInstruction>>* p_fix_instructions;
};

StackFixContext::StackFixContext() : stack_bytes(0l) {}

static std::unique_ptr<StackFixContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement

static std::shared_ptr<AsmData> pseudo_data(AsmPseudo* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmData>(std::move(name), 0l);
}

static std::shared_ptr<AsmData> pseudo_mem_data(AsmPseudoMem* node) {
    TIdentifier name = node->name;
    TLong offset = node->offset;
    return std::make_shared<AsmData>(std::move(name), std::move(offset));
}

static std::shared_ptr<AsmMemory> pseudo_memory(AsmPseudo* node) {
    TLong value = -1l * context->pseudo_stack_bytes_map[node->name];
    return gen_memory(REGISTER_KIND::Bp, std::move(value));
}

static std::shared_ptr<AsmMemory> pseudo_mem_memory(AsmPseudoMem* node) {
    TLong value = -1l * (context->pseudo_stack_bytes_map[node->name] - node->offset);
    return gen_memory(REGISTER_KIND::Bp, std::move(value));
}

static void align_offset_stack_bytes(TInt alignment) {
    TLong offset = context->stack_bytes % alignment;
    if (offset != 0l) {
        context->stack_bytes += alignment - offset;
    }
}

static void align_offset_pseudo(TLong size, TInt alignment) {
    context->stack_bytes += size;
    align_offset_stack_bytes(alignment);
}

static void alloc_offset_pseudo(AssemblyType* assembly_type) {
    switch (assembly_type->type()) {
        case AST_T::Byte_t:
            align_offset_pseudo(1l, 1);
            break;
        case AST_T::LongWord_t:
            align_offset_pseudo(4l, 4);
            break;
        case AST_T::QuadWord_t:
        case AST_T::BackendDouble_t:
            align_offset_pseudo(8l, 8);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void alloc_offset_pseudo_mem(AssemblyType* assembly_type) {
    switch (assembly_type->type()) {
        case AST_T::ByteArray_t: {
            ByteArray* p_assembly_type = static_cast<ByteArray*>(assembly_type);
            align_offset_pseudo(p_assembly_type->size, p_assembly_type->alignment);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AsmOperand> repl_pseudo_op(AsmPseudo* node) {
    if (context->pseudo_stack_bytes_map.find(node->name) == context->pseudo_stack_bytes_map.end()) {

        BackendObj* backend_obj = static_cast<BackendObj*>(backend->backend_symbol_table[node->name].get());
        if (backend_obj->is_static) {
            return pseudo_data(node);
        }
        else {
            alloc_offset_pseudo(backend_obj->assembly_type.get());
            context->pseudo_stack_bytes_map[node->name] = context->stack_bytes;
        }
    }

    return pseudo_memory(node);
}

static std::shared_ptr<AsmOperand> repl_pseudo_mem_op(AsmPseudoMem* node) {
    if (context->pseudo_stack_bytes_map.find(node->name) == context->pseudo_stack_bytes_map.end()) {

        BackendObj* backend_obj = static_cast<BackendObj*>(backend->backend_symbol_table[node->name].get());
        if (backend_obj->is_static) {
            return pseudo_mem_data(node);
        }
        else {
            alloc_offset_pseudo_mem(backend_obj->assembly_type.get());
            context->pseudo_stack_bytes_map[node->name] = context->stack_bytes;
        }
    }

    return pseudo_mem_memory(node);
}

static void repl_mov_pseudo(AsmMov* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_mov_sx_pseudo(AsmMovSx* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_zero_extend_pseudo(AsmMovZeroExtend* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_lea_pseudo(AsmLea* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_cvttsd2si_pseudo(AsmCvttsd2si* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_cvtsi2sd_pseudo(AsmCvtsi2sd* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_unary(AsmUnary* node) {
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_binary_pseudo(AsmBinary* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_cmp_pseudo(AsmCmp* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_idiv_pseudo(AsmIdiv* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_div_pseudo(AsmDiv* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_set_cc_pseudo(AsmSetCC* node) {
    switch (node->dst->type()) {
        case AST_T::AsmPseudo_t: {
            node->dst = repl_pseudo_op(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->dst = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_push_pseudo(AsmPush* node) {
    switch (node->src->type()) {
        case AST_T::AsmPseudo_t: {
            node->src = repl_pseudo_op(static_cast<AsmPseudo*>(node->src.get()));
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            node->src = repl_pseudo_mem_op(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        }
        default:
            break;
    }
}

static void repl_pseudo_regs(AsmInstruction* node) {
    switch (node->type()) {
        case AST_T::AsmMov_t:
            repl_mov_pseudo(static_cast<AsmMov*>(node));
            break;
        case AST_T::AsmMovSx_t:
            repl_mov_sx_pseudo(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmMovZeroExtend_t:
            repl_zero_extend_pseudo(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_T::AsmLea_t:
            repl_lea_pseudo(static_cast<AsmLea*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            repl_cvttsd2si_pseudo(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            repl_cvtsi2sd_pseudo(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmUnary_t:
            repl_unary(static_cast<AsmUnary*>(node));
            break;
        case AST_T::AsmBinary_t:
            repl_binary_pseudo(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmCmp_t:
            repl_cmp_pseudo(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmIdiv_t:
            repl_idiv_pseudo(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            repl_div_pseudo(static_cast<AsmDiv*>(node));
            break;
        case AST_T::AsmSetCC_t:
            repl_set_cc_pseudo(static_cast<AsmSetCC*>(node));
            break;
        case AST_T::AsmPush_t:
            repl_push_pseudo(static_cast<AsmPush*>(node));
            break;
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Instruction fix up

std::unique_ptr<AsmBinary> alloc_stack_bytes(TLong byte) {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmSub>();
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src;
    {
        TULong value = static_cast<TULong>(byte);
        bool is_byte = byte <= 127l && byte >= -128l;
        bool is_quad = byte > 2147483647l || byte < -2147483648l;
        bool is_neg = byte < 0l;
        src = std::make_shared<AsmImm>(std::move(value), std::move(is_byte), std::move(is_quad), std::move(is_neg));
    }
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::Sp);
    return std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type), std::move(src), std::move(dst));
}

static void push_fix_instr(std::unique_ptr<AsmInstruction>&& fix_instr) {
    context->p_fix_instructions->push_back(std::move(fix_instr));
}

static void swap_fix_instr_back() {
    std::swap(
        (*context->p_fix_instructions)[context->p_fix_instructions->size() - 2], context->p_fix_instructions->back());
}

static void fix_alloc_stack_bytes(TLong callee_saved_size) {
    TLong callee_saved_bytes = callee_saved_size * 8l;
    context->stack_bytes += callee_saved_bytes;
    if (context->stack_bytes > 0l) {
        align_offset_stack_bytes(16);
        context->stack_bytes -= callee_saved_bytes;
        (*context->p_fix_instructions)[0] = alloc_stack_bytes(context->stack_bytes);
    }
}

static void push_callee_saved_regs(const std::vector<std::shared_ptr<AsmOperand>>& callee_saved_registers) {
    for (std::shared_ptr<AsmOperand> src : callee_saved_registers) {
        push_fix_instr(std::make_unique<AsmPush>(std::move(src)));
    }
}

static void pop_callee_saved_regs(const std::vector<std::shared_ptr<AsmOperand>>& callee_saved_registers) {
    for (size_t i = callee_saved_registers.size(); i-- > 0;) {
        if (callee_saved_registers[i]->type() != AST_T::AsmRegister_t) {
            RAISE_INTERNAL_ERROR;
        }
        REGISTER_KIND register_kind =
            register_mask_kind(static_cast<AsmRegister*>(callee_saved_registers[i].get())->reg.get());
        std::unique_ptr<AsmReg> reg;
        switch (register_kind) {
            case REGISTER_KIND::Bx: {
                reg = std::make_unique<AsmBx>();
                break;
            }
            case REGISTER_KIND::R12: {
                reg = std::make_unique<AsmR12>();
                break;
            }
            case REGISTER_KIND::R13: {
                reg = std::make_unique<AsmR13>();
                break;
            }
            case REGISTER_KIND::R14: {
                reg = std::make_unique<AsmR14>();
                break;
            }
            case REGISTER_KIND::R15: {
                reg = std::make_unique<AsmR15>();
                break;
            }
            default:
                RAISE_INTERNAL_ERROR;
        }
        push_fix_instr(std::make_unique<AsmPop>(std::move(reg)));
    }
}

static bool is_op_addr(AsmOperand* node) {
    switch (node->type()) {
        case AST_T::AsmMemory_t:
        case AST_T::AsmData_t:
        case AST_T::AsmIndexed_t:
            return true;
        default:
            return false;
    }
}

static void mov_dbl_from_addr_to_addr(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::Xmm14);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void mov_from_quad_imm(AsmMov* node) {
    if (node->dst->type() == AST_T::AsmRegister_t) {
        if (node->assembly_type->type() != AST_T::QuadWord_t) {
            node->assembly_type = std::make_shared<QuadWord>();
        }
        return;
    }
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void mov_from_addr_to_addr(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void fix_mov_instr(AsmMov* node) {
    if (node->assembly_type->type() == AST_T::BackendDouble_t) {
        if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
            mov_dbl_from_addr_to_addr(node);
        }
    }
    else {
        if (node->src->type() == AST_T::AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
            mov_from_quad_imm(node);
        }
        if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
            mov_from_addr_to_addr(node);
        }
    }
}

static void mov_sx_from_imm(AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type_src;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void mov_sx_to_addr(AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type_dst;
    node->dst = src;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_mov_sx_instr(AsmMovSx* node) {
    if (node->src->type() == AST_T::AsmImm_t) {
        mov_sx_from_imm(node);
    }
    if (is_op_addr(node->dst.get())) {
        mov_sx_to_addr(node);
    }
}

static void byte_zero_extend_from_imm(AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<Byte>();
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void byte_zero_extend_to_addr(AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type_dst;
    node->dst = src;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static AsmMov* zero_extend_as_mov(AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<LongWord>();
    context->p_fix_instructions->back() =
        std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst));
    return static_cast<AsmMov*>(context->p_fix_instructions->back().get());
}

static void zero_extend_to_addr(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_zero_extend_instr(AsmMovZeroExtend* node) {
    if (node->assembly_type_src->type() == AST_T::Byte_t) {
        if (node->src->type() == AST_T::AsmImm_t) {
            byte_zero_extend_from_imm(node);
        }
        if (is_op_addr(node->dst.get())) {
            byte_zero_extend_to_addr(node);
        }
    }
    else {
        AsmMov* mov = zero_extend_as_mov(node);
        if (is_op_addr(mov->dst.get())) {
            zero_extend_to_addr(mov);
        }
    }
}

static void lea_to_addr(AsmLea* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_lea_instr(AsmLea* node) {
    if (is_op_addr(node->dst.get())) {
        lea_to_addr(node);
    }
}

static void cvttsd2si_to_addr(AsmCvttsd2si* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = src;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_cvttsd2si_instr(AsmCvttsd2si* node) {
    if (is_op_addr(node->dst.get())) {
        cvttsd2si_to_addr(node);
    }
}

static void cvtsi2sd_from_imm(AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void cvtsi2sd_to_addr(AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = gen_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = src;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_cvtsi2sd_instr(AsmCvtsi2sd* node) {
    if (node->src->type() == AST_T::AsmImm_t) {
        cvtsi2sd_from_imm(node);
    }
    if (is_op_addr(node->dst.get())) {
        cvtsi2sd_to_addr(node);
    }
}

static void binary_dbl_to_addr(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    push_fix_instr(std::make_unique<AsmMov>(assembly_type, src, dst));
    swap_fix_instr_back();
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(dst), std::move(src)));
}

static void binary_from_quad_imm(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void binary_from_addr_to_addr(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void binary_imul_to_addr(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R11);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = dst;
    push_fix_instr(std::make_unique<AsmMov>(assembly_type, src, dst));
    swap_fix_instr_back();
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(dst), std::move(src)));
}

static void binary_shx_from_not_imm(AsmBinary* node) {
    if (node->src->type() == AST_T::AsmRegister_t
        && register_mask_kind(static_cast<AsmRegister*>(node->src.get())->reg.get()) == REGISTER_KIND::Cx) {
        return;
    }
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::Cx);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void fix_binary_instr(AsmBinary* node) {
    if (node->assembly_type->type() == AST_T::BackendDouble_t) {
        if (is_op_addr(node->dst.get())) {
            binary_dbl_to_addr(node);
        }
    }
    else {
        switch (node->binary_op->type()) {
            case AST_T::AsmAdd_t:
            case AST_T::AsmSub_t:
            case AST_T::AsmBitAnd_t:
            case AST_T::AsmBitOr_t:
            case AST_T::AsmBitXor_t: {
                if (node->src->type() == AST_T::AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
                    binary_from_quad_imm(node);
                }
                if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
                    binary_from_addr_to_addr(node);
                }
                break;
            }
            case AST_T::AsmMult_t: {
                if (node->src->type() == AST_T::AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
                    binary_from_quad_imm(node);
                }
                if (is_op_addr(node->dst.get())) {
                    binary_imul_to_addr(node);
                }
                break;
            }
            case AST_T::AsmBitShiftLeft_t:
            case AST_T::AsmBitShiftRight_t:
            case AST_T::AsmBitShrArithmetic_t: {
                if (node->src->type() == AST_T::AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
                    binary_from_quad_imm(node);
                }
                if (node->src->type() != AST_T::AsmImm_t) {
                    binary_shx_from_not_imm(node);
                }
                break;
            }
            default:
                break;
        }
    }
}

static void cmp_dbl_to_addr(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void cmp_from_quad_imm(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void cmp_from_addr_to_addr(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void cmp_to_imm(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R11);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void fix_cmp_instr(AsmCmp* node) {
    if (node->assembly_type->type() == AST_T::BackendDouble_t) {
        if (is_op_addr(node->dst.get())) {
            cmp_dbl_to_addr(node);
        }
    }
    else {
        if (node->src->type() == AST_T::AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
            cmp_from_quad_imm(node);
        }
        if (is_op_addr(node->src.get()) && is_op_addr(node->dst.get())) {
            cmp_from_addr_to_addr(node);
        }
        else if (node->dst->type() == AST_T::AsmImm_t) {
            cmp_to_imm(node);
        }
    }
}

static void idiv_from_imm(AsmIdiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void fix_idiv_instr(AsmIdiv* node) {
    if (node->src->type() == AST_T::AsmImm_t) {
        idiv_from_imm(node);
    }
}

static void div_from_imm(AsmDiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void fix_div_instr(AsmDiv* node) {
    if (node->src->type() == AST_T::AsmImm_t) {
        div_from_imm(node);
    }
}

static void push_dbl_from_xmm_reg(AsmPush* node) {
    std::shared_ptr<AsmOperand> src_reg = std::move(node->src);
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
    {
        std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmSub>();
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::Sp);
        context->p_fix_instructions->back() =
            std::make_unique<AsmBinary>(std::move(binary_op), assembly_type_src, std::move(src), std::move(dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_memory(REGISTER_KIND::Sp, 0l);
        push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src_reg), std::move(dst)));
    }
}

static void push_from_quad_imm(AsmPush* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = gen_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instr(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instr_back();
}

static void fix_push_instr(AsmPush* node) {
    if (node->src->type() == AST_T::AsmRegister_t) {
        REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmRegister*>(node->src.get())->reg.get());
        if (register_kind != REGISTER_KIND::Sp && register_mask_bit(register_kind) > 11) {
            push_dbl_from_xmm_reg(node);
        }
    }
    else if (node->src->type() == AST_T::AsmImm_t && static_cast<AsmImm*>(node->src.get())->is_quad) {
        push_from_quad_imm(node);
    }
}

static void fix_instr(AsmInstruction* node) {
    switch (node->type()) {
        case AST_T::AsmMov_t:
            fix_mov_instr(static_cast<AsmMov*>(node));
            break;
        case AST_T::AsmMovSx_t:
            fix_mov_sx_instr(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmMovZeroExtend_t:
            fix_zero_extend_instr(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_T::AsmLea_t:
            fix_lea_instr(static_cast<AsmLea*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            fix_cvttsd2si_instr(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            fix_cvtsi2sd_instr(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmBinary_t:
            fix_binary_instr(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmCmp_t:
            fix_cmp_instr(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmIdiv_t:
            fix_idiv_instr(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            fix_div_instr(static_cast<AsmDiv*>(node));
            break;
        case AST_T::AsmPush_t:
            fix_push_instr(static_cast<AsmPush*>(node));
            break;
        default:
            break;
    }
}

static void fun_toplvl(AsmFunction* node) {
    std::vector<std::unique_ptr<AsmInstruction>> instructions = std::move(node->instructions);
    BackendFun* backend_fun = static_cast<BackendFun*>(backend->backend_symbol_table[node->name].get());

    node->instructions.clear();
    node->instructions.reserve(instructions.size());

    context->stack_bytes = node->is_return_memory ? 8l : 0l;
    context->pseudo_stack_bytes_map.clear();
    context->p_fix_instructions = &node->instructions;
    context->p_fix_instructions->emplace_back();

    bool is_ret = false;
    push_callee_saved_regs(backend_fun->callee_saved_registers);
    for (size_t i = 0; i < instructions.size(); ++i) {
        if (instructions[i]) {
            if (instructions[i]->type() == AST_T::AsmRet_t) {
                pop_callee_saved_regs(backend_fun->callee_saved_registers);
                is_ret = true;
            }
            push_fix_instr(std::move(instructions[i]));

            repl_pseudo_regs(context->p_fix_instructions->back().get());
            fix_instr(context->p_fix_instructions->back().get());
        }
    }
    if (!is_ret) {
        pop_callee_saved_regs(backend_fun->callee_saved_registers);
    }
    {
        TLong callee_saved_size = static_cast<TLong>(backend_fun->callee_saved_registers.size());
        fix_alloc_stack_bytes(callee_saved_size);
    }
    context->p_fix_instructions = nullptr;
}

static void fix_toplvl(AsmTopLevel* node) {
    switch (node->type()) {
        case AST_T::AsmFunction_t:
            fun_toplvl(static_cast<AsmFunction*>(node));
            break;
        case AST_T::AsmStaticVariable_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void fix_program(AsmProgram* node) {
    for (const auto& top_level : node->top_levels) {
        fix_toplvl(top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fix_stack(AsmProgram* node) {
    context = std::make_unique<StackFixContext>();
    fix_program(node);
    context.reset();
}
