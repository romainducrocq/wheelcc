#include "backend/assembly/stack_fix.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/back_ast.hpp"
#include "backend/assembly/registers.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

static TULong counter = 0ul;

static std::unordered_map<TIdentifier, TULong> pseudo_map;

static std::shared_ptr<AsmData> replace_pseudo_register_data(AsmPseudo* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmData>(std::move(name));
}

static std::shared_ptr<AsmData> replace_pseudo_mem_register_data(AsmPseudoMem* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmData>(std::move(name));
}

static std::shared_ptr<AsmMemory> replace_pseudo_register_memory(AsmPseudo* node) {
    TULong value = pseudo_map[node->name];
    return generate_memory(REGISTER_KIND::Bp, std::move(value));
}

static std::shared_ptr<AsmMemory> replace_pseudo_mem_register_memory(AsmPseudoMem* node) {
    TULong value = pseudo_map[node->name] - node->offset;
    return generate_memory(REGISTER_KIND::Bp, std::move(value));
}

static void allocate_offset_pseudo_register(AssemblyType* assembly_type) {
    switch(assembly_type->type()) {
        case AST_T::LongWord_t:
            counter += 4ul;
            break;
        case AST_T::QuadWord_t:
        case AST_T::BackendDouble_t:
            counter += 8ul;
            break;
        case AST_T::ByteArray_t:
            counter += static_cast<ByteArray*>(assembly_type)->size;
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void align_offset_pseudo_register(AssemblyType* assembly_type) {
    switch(assembly_type->type()) {
        case AST_T::LongWord_t:
            counter += 4ul;
            break;
        case AST_T::ByteArray_t:
            // TODO
            counter += counter % static_cast<ByteArray*>(assembly_type)->alignment;
            break;
        default:
            break;
    }
}

static std::shared_ptr<AsmOperand> replace_operand_pseudo_register(AsmPseudo* node) {
    if(pseudo_map.find(node->name) == pseudo_map.end()) {

        BackendObj* backend_obj = static_cast<BackendObj*>(backend_symbol_table[node->name].get());
        if(backend_obj->is_static) {
            return replace_pseudo_register_data(node);
        }
        else {
            allocate_offset_pseudo_register(backend_obj->assembly_type.get());
            pseudo_map[node->name] = counter;
            align_offset_pseudo_register(backend_obj->assembly_type.get());
        }
    }

    return replace_pseudo_register_memory(node);
}

static std::shared_ptr<AsmOperand> replace_operand_pseudo_mem_register(AsmPseudoMem* node) {
    if(pseudo_map.find(node->name) == pseudo_map.end()) {

        BackendObj* backend_obj = static_cast<BackendObj*>(backend_symbol_table[node->name].get());
        if(backend_obj->is_static) {
            return replace_pseudo_mem_register_data(node);
        }
        else {
            allocate_offset_pseudo_register(backend_obj->assembly_type.get());
            pseudo_map[node->name] = counter;
            align_offset_pseudo_register(backend_obj->assembly_type.get());
        }
    }

    return replace_pseudo_mem_register_memory(node);
}

static void replace_mov_pseudo_registers(AsmMov* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_mov_sx_pseudo_registers(AsmMovSx* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_mov_zero_extend_pseudo_registers(AsmMovZeroExtend* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_lea_pseudo_registers(AsmLea* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_cvttsd2si_pseudo_registers(AsmCvttsd2si* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_cvtsi2sd_pseudo_registers(AsmCvtsi2sd* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_unary_pseudo_registers(AsmUnary* node) {
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_binary_pseudo_registers(AsmBinary* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_cmp_pseudo_registers(AsmCmp* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_idiv_pseudo_registers(AsmIdiv* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
}

static void replace_div_pseudo_registers(AsmDiv* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
}

static void replace_set_cc_pseudo_registers(AsmSetCC* node) {
    switch(node->dst->type()) {
        case AST_T::AsmPseudo_t:
            node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->dst = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->dst.get()));
            break;
        default:
            break;
    }
}

static void replace_push_pseudo_registers(AsmPush* node) {
    switch(node->src->type()) {
        case AST_T::AsmPseudo_t:
            node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
            break;
        case AST_T::AsmPseudoMem_t:
            node->src = replace_operand_pseudo_mem_register(static_cast<AsmPseudoMem*>(node->src.get()));
            break;
        default:
            break;
    }
}

static void replace_pseudo_registers(AsmInstruction* node) {
    switch(node->type()) {
        case AST_T::AsmMov_t:
            replace_mov_pseudo_registers(static_cast<AsmMov*>(node));
            break;
        case AST_T::AsmMovSx_t:
            replace_mov_sx_pseudo_registers(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmMovZeroExtend_t:
            replace_mov_zero_extend_pseudo_registers(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_T::AsmLea_t:
            replace_lea_pseudo_registers(static_cast<AsmLea*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            replace_cvttsd2si_pseudo_registers(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            replace_cvtsi2sd_pseudo_registers(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmUnary_t:
            replace_unary_pseudo_registers(static_cast<AsmUnary*>(node));
            break;
        case AST_T::AsmBinary_t:
            replace_binary_pseudo_registers(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmCmp_t:
            replace_cmp_pseudo_registers(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmIdiv_t:
            replace_idiv_pseudo_registers(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            replace_div_pseudo_registers(static_cast<AsmDiv*>(node));
            break;
        case AST_T::AsmSetCC_t:
            replace_set_cc_pseudo_registers(static_cast<AsmSetCC*>(node));
            break;
        case AST_T::AsmPush_t:
            replace_push_pseudo_registers(static_cast<AsmPush*>(node));
            break;
        default:
            break;
    }
}

std::unique_ptr<AsmBinary> allocate_stack_bytes(TULong byte) {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmSub>();
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src;
    {
        bool is_quad = byte > 2147483647ul;
        TIdentifier value = std::to_string(byte);
        src = std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
    }
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Sp);
    return std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type), std::move(src), std::move(dst));
}

std::unique_ptr<AsmBinary> deallocate_stack_bytes(TULong byte) {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmAdd>();
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src;
    {
        bool is_quad = byte > 2147483647ul;
        TIdentifier value = std::to_string(byte);
        src = std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
    }
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Sp);
    return std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type), std::move(src), std::move(dst));
}

static std::vector<std::unique_ptr<AsmInstruction>>* p_fix_instructions;

static void push_fix_instruction(std::unique_ptr<AsmInstruction>&& fix_instruction) {
    p_fix_instructions->push_back(std::move(fix_instruction));
}

static void swap_fix_instruction_back() {
    std::swap((*p_fix_instructions)[p_fix_instructions->size()-2], p_fix_instructions->back());
}

static void fix_allocate_stack_bytes() {
    TULong byte = counter;
    if(byte % 8ul != 0ul) {
        RAISE_INTERNAL_ERROR;
    }
    if(byte > 0ul) {
        (*p_fix_instructions)[0] = allocate_stack_bytes(std::move(byte));
    }
}

static bool is_type_imm(AsmOperand* node) {
    return node->type() == AST_T::AsmImm_t;
}

static bool is_type_addr(AsmOperand* node) {
    switch(node->type()) {
        case AST_T::AsmMemory_t:
        case AST_T::AsmData_t:
        case AST_T::AsmIndexed_t:
            return true;
        default:
            return false;
    }
}

static void fix_double_mov_from_addr_to_addr_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm14);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_mov_from_quad_word_imm_to_any_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_mov_from_addr_to_addr_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_mov_instruction(AsmMov* node) {
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        if(is_type_addr(node->src.get()) &&
           is_type_addr(node->dst.get())) {
            fix_double_mov_from_addr_to_addr_instruction(node);
        }
    }
    else {
        if(is_type_imm(node->src.get()) &&
           static_cast<AsmImm*>(node->src.get())->is_quad) {
            fix_mov_from_quad_word_imm_to_any_instruction(node);
        }
        if(is_type_addr(node->src.get()) &&
           is_type_addr(node->dst.get())) {
            fix_mov_from_addr_to_addr_instruction(node);
        }
    }
}

static void fix_mov_sx_from_imm_to_any_instruction(AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<LongWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_mov_sx_from_any_to_addr_instruction(AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_mov_sx_instruction(AsmMovSx* node) {
    if(is_type_imm(node->src.get())) {
        fix_mov_sx_from_imm_to_any_instruction(node);
    }
    if(is_type_addr(node->dst.get())) {
        fix_mov_sx_from_any_to_addr_instruction(node);
    }
}

static void fix_mov_zero_extend_from_any_to_any_instruction(AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<LongWord>();
    p_fix_instructions->back() = std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst));
}

static void fix_mov_zero_extend_from_any_to_addr_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_mov_zero_extend_instruction(AsmMovZeroExtend* node) {
    fix_mov_zero_extend_from_any_to_any_instruction(node);
    AsmMov* node_2 = static_cast<AsmMov*>(p_fix_instructions->back().get());
    if(is_type_addr(node_2->dst.get())) {
        fix_mov_zero_extend_from_any_to_addr_instruction(node_2);
    }
}

static void fix_lea_from_any_to_addr_instruction(AsmLea* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_lea_instruction(AsmLea* node) {
    if(is_type_addr(node->dst.get())) {
        fix_lea_from_any_to_addr_instruction(node);
    }
}

static void fix_cvttsd2si_from_any_to_addr_instruction(AsmCvttsd2si* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_cvttsd2si_instruction(AsmCvttsd2si* node) {
    if(is_type_addr(node->dst.get())) {
        fix_cvttsd2si_from_any_to_addr_instruction(node);
    }
}

static void fix_cvtsi2sd_from_imm_to_any_instruction(AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_cvtsi2sd_from_any_to_addr_instruction(AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void fix_cvtsi2sd_instruction(AsmCvtsi2sd* node) {
    if(is_type_imm(node->src.get())) {
        fix_cvtsi2sd_from_imm_to_any_instruction(node);
    }
    if(is_type_addr(node->dst.get())) {
        fix_cvtsi2sd_from_any_to_addr_instruction(node);
    }
}

static void fix_double_binary_from_any_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(assembly_type, src, dst));
    swap_fix_instruction_back();
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(dst),
                                                             std::move(src)));
}

static void fix_binary_from_quad_word_imm_to_any_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_binary_any_from_addr_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_binary_imul_from_any_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(assembly_type, src, dst));
    swap_fix_instruction_back();
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(dst),
                                                             std::move(src)));
}

static void fix_binary_shx_from_addr_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Cx);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_binary_instruction(AsmBinary* node) {
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        if(is_type_addr(node->dst.get())) {
            fix_double_binary_from_any_to_addr_instruction(node);
        }
    }
    else {
        switch(node->binary_op->type()) {
            case AST_T::AsmAdd_t:
            case AST_T::AsmSub_t:
            case AST_T::AsmBitAnd_t:
            case AST_T::AsmBitOr_t:
            case AST_T::AsmBitXor_t: {
                if(is_type_imm(node->src.get()) &&
                   static_cast<AsmImm*>(node->src.get())->is_quad) {
                    fix_binary_from_quad_word_imm_to_any_instruction(node);
                }
                if(is_type_addr(node->src.get()) &&
                   is_type_addr(node->dst.get())) {
                    fix_binary_any_from_addr_to_addr_instruction(node);
                }
                break;
            }
            case AST_T::AsmMult_t: {
                if(is_type_imm(node->src.get()) &&
                   static_cast<AsmImm*>(node->src.get())->is_quad) {
                    fix_binary_from_quad_word_imm_to_any_instruction(node);
                }
                if(is_type_addr(node->dst.get())) {
                    fix_binary_imul_from_any_to_addr_instruction(node);
                }
                break;
            }
            case AST_T::AsmBitShiftLeft_t:
            case AST_T::AsmBitShiftRight_t: {
                if(is_type_imm(node->src.get()) &&
                   static_cast<AsmImm*>(node->src.get())->is_quad) {
                    fix_binary_from_quad_word_imm_to_any_instruction(node);
                }
                if(is_type_addr(node->src.get()) &&
                   is_type_addr(node->dst.get())) {
                    fix_binary_shx_from_addr_to_addr_instruction(node);
                }
                break;
            }
            default:
                break;
        }
    }
}

static void fix_double_cmp_from_any_to_addr_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_cmp_from_quad_word_imm_to_any_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_cmp_from_addr_to_addr_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_cmp_from_any_to_imm_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_cmp_instruction(AsmCmp* node) {
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        if(is_type_addr(node->dst.get())) {
            fix_double_cmp_from_any_to_addr_instruction(node);
        }
    }
    else {
        if(is_type_imm(node->src.get()) &&
           static_cast<AsmImm*>(node->src.get())->is_quad) {
            fix_cmp_from_quad_word_imm_to_any_instruction(node);
        }
        if(is_type_addr(node->src.get()) &&
           is_type_addr(node->dst.get())) {
            fix_cmp_from_addr_to_addr_instruction(node);
        }
        else if(is_type_imm(node->dst.get())) {
            fix_cmp_from_any_to_imm_instruction(node);
        }
    }
}

static void fix_idiv_from_imm_instruction(AsmIdiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_idiv_instruction(AsmIdiv* node) {
    if(is_type_imm(node->src.get())) {
        fix_idiv_from_imm_instruction(node);
    }
}

static void fix_div_from_imm_instruction(AsmDiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_div_instruction(AsmDiv* node) {
    if(is_type_imm(node->src.get())) {
        fix_div_from_imm_instruction(node);
    }
}

// TODO (p3, ch20)
// static void fix_double_push_from_xmm_reg_to_any_instruction(AsmPush* node) {
//     // subq $8, %rsp
//     // movsd %xmm0, (%rsp)
// }

static void fix_push_from_quad_word_imm_to_any_instruction(AsmPush* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

static void fix_push_instruction(AsmPush* node) {
    // TODO (p3, ch20)
    // // pushq %xmm0
    // if src is register and src->reg is
    // fix_double_push_from_xmm_reg_to_any_instruction(node);
    if(is_type_imm(node->src.get()) &&
       static_cast<AsmImm*>(node->src.get())->is_quad) {
        fix_push_from_quad_word_imm_to_any_instruction(node);
    }
}

static void fix_instruction(AsmInstruction* node) {
    switch(node->type()) {
        case AST_T::AsmMov_t:
            fix_mov_instruction(static_cast<AsmMov*>(node));
            break;
        case AST_T::AsmMovSx_t:
            fix_mov_sx_instruction(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmMovZeroExtend_t:
            fix_mov_zero_extend_instruction(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_T::AsmLea_t:
            fix_lea_instruction(static_cast<AsmLea*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            fix_cvttsd2si_instruction(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            fix_cvtsi2sd_instruction(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmBinary_t:
            fix_binary_instruction(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmCmp_t:
            fix_cmp_instruction(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmIdiv_t:
            fix_idiv_instruction(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            fix_div_instruction(static_cast<AsmDiv*>(node));
            break;
        case AST_T::AsmPush_t:
            fix_push_instruction(static_cast<AsmPush*>(node));
            break;
        default:
            break;
    }
}

static void fix_function_top_level(AsmFunction* node) {
    std::vector<std::unique_ptr<AsmInstruction>> instructions = std::move(node->instructions);

    node->instructions.clear();
    p_fix_instructions = &node->instructions;
    p_fix_instructions->emplace_back();

    counter = 0ul;
    pseudo_map.clear();
    for(size_t instruction = 0; instruction < instructions.size(); instruction++) {
        push_fix_instruction(std::move(instructions[instruction]));

        replace_pseudo_registers(p_fix_instructions->back().get());
        fix_instruction(p_fix_instructions->back().get());
    }
    fix_allocate_stack_bytes();
    p_fix_instructions = nullptr;
}

static void fix_top_level(AsmTopLevel* node) {
    switch(node->type()) {
        case AST_T::AsmFunction_t:
            fix_function_top_level(static_cast<AsmFunction*>(node));
            break;
        case AST_T::AsmStaticVariable_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void fix_program(AsmProgram* node) {
    for(size_t top_level = 0; top_level < node->top_levels.size(); top_level++) {
        fix_top_level(node->top_levels[top_level].get());
    }
}

void fix_stack(AsmProgram* node) {
    fix_program(node);
}
