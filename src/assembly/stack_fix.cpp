#include "assembly/stack_fix.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/backend_st.hpp"
#include "ast/asm_ast.hpp"
#include "assembly/registers.hpp"

#include <inttypes.h>
#include <memory>
#include <vector>
#include <unordered_map>

/**
cdef int32 OFFSET_32_BITS = -4
*/
const int32_t OFFSET_32_BITS = -4;

/**
cdef int32 OFFSET_64_BITS = -8
*/
const int32_t OFFSET_64_BITS = -8;

/**
cdef int32 counter = -1
*/
static TInt counter = 0;

/**
cdef dict[str, int32] pseudo_map = {}
*/
static std::unordered_map<TIdentifier, TInt> pseudo_map;

/**
cdef AsmData replace_pseudo_register_data(AsmPseudo node):
    cdef TIdentifier name = copy_identifier(node.name)
    return AsmData(name)
*/
static std::shared_ptr<AsmData> replace_pseudo_register_data(AsmPseudo* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmData>(std::move(name));
}

/**
cdef AsmStack replace_pseudo_register_stack(AsmPseudo node):
    cdef TInt value = TInt(pseudo_map[node.name.str_t])
    return AsmStack(value)
*/
static std::shared_ptr<AsmStack> replace_pseudo_register_stack(AsmPseudo* node) {
    TInt value = pseudo_map[node->name];
    return std::make_shared<AsmStack>(std::move(value));
}

/**
cdef void allocate_offset_pseudo_register(AssemblyType assembly_type):
    global counter

    if isinstance(assembly_type, LongWord):
        counter += OFFSET_32_BITS
    elif isinstance(assembly_type, (QuadWord, BackendDouble)):
        counter += OFFSET_64_BITS
*/
static void allocate_offset_pseudo_register(AssemblyType* assembly_type) {
    switch(assembly_type->type()) {
        case AST_T::LongWord_t:
            counter += OFFSET_32_BITS;
            break;
        case AST_T::QuadWord_t:
        case AST_T::BackendDouble_t:
            counter += OFFSET_64_BITS;
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef void align_offset_pseudo_register(AssemblyType assembly_type):
    global counter

    if isinstance(assembly_type, LongWord):
        counter += OFFSET_32_BITS
*/
static void align_offset_pseudo_register(AssemblyType* assembly_type) {
    switch(assembly_type->type()) {
        case AST_T::LongWord_t:
            counter += OFFSET_32_BITS;
            break;
        default:
            break;
    }
}

/**
cdef AsmOperand replace_operand_pseudo_register(AsmPseudo node):
    global pseudo_map

    if node.name.str_t not in pseudo_map:
        if backend_symbol_table[node.name.str_t].is_static:
            return replace_pseudo_register_data(node)
        else:
            allocate_offset_pseudo_register(backend_symbol_table[node.name.str_t].assembly_type)
            pseudo_map[node.name.str_t] = counter
            align_offset_pseudo_register(backend_symbol_table[node.name.str_t].assembly_type)
    return replace_pseudo_register_stack(node)
*/
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

    return replace_pseudo_register_stack(node);
}

/**
cdef void replace_mov_pseudo_registers(AsmMov node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_mov_pseudo_registers(AsmMov* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_mov_sx_pseudo_registers(AsmMovSx node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_mov_sx_pseudo_registers(AsmMovSx* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_mov_zero_extend_pseudo_registers(AsmMovZeroExtend node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_mov_zero_extend_pseudo_registers(AsmMovZeroExtend* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_cvttsd2si_pseudo_registers(AsmCvttsd2si node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_cvttsd2si_pseudo_registers(AsmCvttsd2si* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_cvtsi2sd_pseudo_registers(AsmCvtsi2sd node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_cvtsi2sd_pseudo_registers(AsmCvtsi2sd* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_push_pseudo_registers(AsmPush node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
*/
static void replace_push_pseudo_registers(AsmPush* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
}

/**
cdef void replace_cmp_pseudo_registers(AsmCmp node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_cmp_pseudo_registers(AsmCmp* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_set_cc_pseudo_registers(AsmSetCC node):
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_set_cc_pseudo_registers(AsmSetCC* node) {
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_unary_pseudo_registers(AsmUnary node):
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_unary_pseudo_registers(AsmUnary* node) {
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_binary_pseudo_registers(AsmBinary node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
    if isinstance(node.dst, AsmPseudo):
        node.dst = replace_operand_pseudo_register(node.dst)
*/
static void replace_binary_pseudo_registers(AsmBinary* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
    if(node->dst->type() == AST_T::AsmPseudo_t) {
        node->dst = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->dst.get()));
    }
}

/**
cdef void replace_idiv_pseudo_registers(AsmIdiv node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
*/
static void replace_idiv_pseudo_registers(AsmIdiv* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
}

/**
cdef void replace_div_pseudo_registers(AsmDiv node):
    if isinstance(node.src, AsmPseudo):
        node.src = replace_operand_pseudo_register(node.src)
*/
static void replace_div_pseudo_registers(AsmDiv* node) {
    if(node->src->type() == AST_T::AsmPseudo_t) {
        node->src = replace_operand_pseudo_register(static_cast<AsmPseudo*>(node->src.get()));
    }
}

/**
cdef void replace_pseudo_registers(AsmInstruction node):
    if isinstance(node, AsmMov):
        replace_mov_pseudo_registers(node)
    elif isinstance(node, AsmMovSx):
        replace_mov_sx_pseudo_registers(node)
    elif isinstance(node, AsmMovZeroExtend):
        replace_mov_zero_extend_pseudo_registers(node)
    elif isinstance(node, AsmCvttsd2si):
        replace_cvttsd2si_pseudo_registers(node)
    elif isinstance(node, AsmCvtsi2sd):
        replace_cvtsi2sd_pseudo_registers(node)
    elif isinstance(node, AsmPush):
        replace_push_pseudo_registers(node)
    elif isinstance(node, AsmCmp):
        replace_cmp_pseudo_registers(node)
    elif isinstance(node, AsmSetCC):
        replace_set_cc_pseudo_registers(node)
    elif isinstance(node, AsmUnary):
        replace_unary_pseudo_registers(node)
    elif isinstance(node, AsmBinary):
        replace_binary_pseudo_registers(node)
    elif isinstance(node, AsmIdiv):
        replace_idiv_pseudo_registers(node)
    elif isinstance(node, AsmDiv):
        replace_div_pseudo_registers(node)
*/
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
        case AST_T::AsmCvttsd2si_t:
            replace_cvttsd2si_pseudo_registers(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            replace_cvtsi2sd_pseudo_registers(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmPush_t:
            replace_push_pseudo_registers(static_cast<AsmPush*>(node));
            break;
        case AST_T::AsmCmp_t:
            replace_cmp_pseudo_registers(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmSetCC_t:
            replace_set_cc_pseudo_registers(static_cast<AsmSetCC*>(node));
            break;
        case AST_T::AsmUnary_t:
            replace_unary_pseudo_registers(static_cast<AsmUnary*>(node));
            break;
        case AST_T::AsmBinary_t:
            replace_binary_pseudo_registers(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmIdiv_t:
            replace_idiv_pseudo_registers(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            replace_div_pseudo_registers(static_cast<AsmDiv*>(node));
            break;
        default:
            break;
    }
}

/**
cdef AsmBinary allocate_stack_bytes(int32 byte):
    cdef AsmBinaryOp binary_op = AsmSub()
    cdef AssemblyType assembly_type = QuadWord()
    cdef AsmOperand src = AsmImm(TIdentifier(str(byte)), False)
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Sp'))
    return AsmBinary(binary_op, assembly_type, src, dst)
*/
std::unique_ptr<AsmBinary> allocate_stack_bytes(TInt byte) {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmSub>();
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier value = std::to_string(byte);
        src = std::make_shared<AsmImm>(false, std::move(value));
    }
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Sp);
    return std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type), std::move(src), std::move(dst));
}

/**
cdef AsmBinary deallocate_stack_bytes(int32 byte):
    cdef AsmBinaryOp binary_op = AsmAdd()
    cdef AssemblyType assembly_type = QuadWord()
    cdef AsmOperand src = AsmImm(TIdentifier(str(byte)), False)
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Sp'))
    return AsmBinary(binary_op, assembly_type, src, dst)
*/
std::unique_ptr<AsmBinary> deallocate_stack_bytes(TInt byte) {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmAdd>();
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier value = std::to_string(byte);
        src = std::make_shared<AsmImm>(false, std::move(value));
    }
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Sp);
    return std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type), std::move(src), std::move(dst));
}

/**
cdef list[AsmInstruction] fix_instructions = []
*/
static std::vector<std::unique_ptr<AsmInstruction>>* p_fix_instructions;

static void push_fix_instruction(std::unique_ptr<AsmInstruction>&& fix_instruction) {
    p_fix_instructions->push_back(std::move(fix_instruction));
}

/**
cdef void swap_fix_instructions_back():
    fix_instructions[-1], fix_instructions[-2] = fix_instructions[-2], fix_instructions[-1]
*/
static void swap_fix_instruction_back() {
    std::swap((*p_fix_instructions)[p_fix_instructions->size()-2], p_fix_instructions->back());
}

/**
cdef void fix_allocate_stack_bytes():
    cdef int32 byte = -1 * counter

    if byte % 8 != 0:

        raise RuntimeError(
            f"An error occurred in function stack allocation, stack alignment {byte} is not a multiple of 8")

    fix_instructions[0].src.value.str_t = str(byte)
*/
// Note: byte is in int32_t (max 0.2gb)
// TODO: make byte to uint32_t
static void fix_allocate_stack_bytes() {
    TInt byte = -1 * counter;
    if(byte % 8 != 0) {
        RAISE_INTERNAL_ERROR;
    }

    AsmImm* imm = static_cast<AsmImm*>(static_cast<AsmBinary*>((*p_fix_instructions)[0].get())->src.get());
    imm->value = std::to_string(byte);
}

static bool is_imm_t(AST_T t) {
    return t == AST_T::AsmImm_t;
}

static bool is_addr_t(AST_T t) {
    return t == AST_T::AsmStack_t || t == AST_T::AsmData_t;
}

/**
cdef void fix_double_mov_from_addr_to_addr_instruction(AsmMov node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Xmm14'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_double_mov_from_addr_to_addr_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm14);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_mov_from_quad_word_imm_to_any_instruction(AsmMov node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = QuadWord()
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_mov_from_quad_word_imm_to_any_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_mov_from_addr_to_addr_instruction(AsmMov node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_mov_from_addr_to_addr_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_mov_instruction(AsmMov node):
    if isinstance(node.assembly_type, BackendDouble):
        if isinstance(node.src, (AsmStack, AsmData)) and \
           isinstance(node.dst, (AsmStack, AsmData)):
            fix_double_mov_from_addr_to_addr_instruction(node)

    else:
        if isinstance(node.src, AsmImm) and \
           node.src.is_quad:
            fix_mov_from_quad_word_imm_to_any_instruction(node)

        if isinstance(node.src, (AsmStack, AsmData)) and \
           isinstance(node.dst, (AsmStack, AsmData)):
            fix_mov_from_addr_to_addr_instruction(node)
*/
static void fix_mov_instruction(AsmMov* node) {
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        if(is_addr_t(node->src->type()) &&
           is_addr_t(node->dst->type())) {
            fix_double_mov_from_addr_to_addr_instruction(node);
        }
    }
    else {
        if(is_imm_t(node->src->type()) &&
           static_cast<AsmImm*>(node->src.get())->is_quad) {
            fix_mov_from_quad_word_imm_to_any_instruction(node);
        }
        if(is_addr_t(node->src->type()) &&
           is_addr_t(node->dst->type())) {
            fix_mov_from_addr_to_addr_instruction(node);
        }
    }
}

/**
cdef void fix_mov_sx_from_imm_to_any_instruction(AsmMovSx node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = LongWord()
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_mov_sx_from_imm_to_any_instruction(AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<LongWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_mov_sx_from_any_to_addr_instruction(AsmMovSx node):
    cdef AsmOperand src = generate_register(REGISTER_KIND.get('R11'))
    cdef AsmOperand dst = node.dst
    cdef AssemblyType assembly_type = QuadWord()
    node.dst = src
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
*/
static void fix_mov_sx_from_any_to_addr_instruction(AsmMovSx* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

/**
cdef void fix_mov_sx_instruction(AsmMovSx node):
    if isinstance(node.src, AsmImm):
        fix_mov_sx_from_imm_to_any_instruction(node)

    if isinstance(node.dst, (AsmStack, AsmData)):
        fix_mov_sx_from_any_to_addr_instruction(node)
*/
static void fix_mov_sx_instruction(AsmMovSx* node) {
    if(is_imm_t(node->src->type())) {
        fix_mov_sx_from_imm_to_any_instruction(node);
    }
    if(is_addr_t(node->dst->type())) {
        fix_mov_sx_from_any_to_addr_instruction(node);
    }
}

/**
cdef void fix_mov_zero_extend_from_any_to_any_instruction(AsmMovZeroExtend node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = node.dst
    cdef AssemblyType assembly_type = LongWord()
    fix_instructions[-1] = AsmMov(assembly_type, src, dst)
*/
static void fix_mov_zero_extend_from_any_to_any_instruction(AsmMovZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<LongWord>();
    p_fix_instructions->back() = std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst));
}

/**
cdef void fix_mov_zero_extend_from_any_to_addr_instruction(AsmMov node):
    cdef AsmOperand src = generate_register(REGISTER_KIND.get('R11'))
    cdef AsmOperand dst = node.dst
    cdef AssemblyType assembly_type = QuadWord()
    node.dst = src
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
*/
static void fix_mov_zero_extend_from_any_to_addr_instruction(AsmMov* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

/**
cdef void fix_mov_zero_extend_instruction(AsmMovZeroExtend node):
    fix_mov_zero_extend_from_any_to_any_instruction(node)
    cdef AsmMov node_2 = fix_instructions[-1]

    if isinstance(node_2.dst, (AsmStack, AsmData)):
        fix_mov_zero_extend_from_any_to_addr_instruction(node_2)
*/
static void fix_mov_zero_extend_instruction(AsmMovZeroExtend* node) {
    fix_mov_zero_extend_from_any_to_any_instruction(node);
    AsmMov* node_2 = static_cast<AsmMov*>(p_fix_instructions->back().get());
    if(is_addr_t(node_2->dst->type())) {
        fix_mov_zero_extend_from_any_to_addr_instruction(node_2);
    }
}

/**
cdef void fix_cvttsd2si_from_any_to_addr_instruction(AsmCvttsd2si node):
    cdef AsmOperand src = generate_register(REGISTER_KIND.get('R11'))
    cdef AsmOperand dst = node.dst
    cdef AssemblyType assembly_type = node.assembly_type
    node.dst = src
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
*/
static void fix_cvttsd2si_from_any_to_addr_instruction(AsmCvttsd2si* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

/**
cdef void fix_cvttsd2si_instruction(AsmCvttsd2si node):
    if isinstance(node.dst, (AsmStack, AsmData)):
        fix_cvttsd2si_from_any_to_addr_instruction(node)
*/
static void fix_cvttsd2si_instruction(AsmCvttsd2si* node) {
    if(is_addr_t(node->dst->type())) {
        fix_cvttsd2si_from_any_to_addr_instruction(node);
    }
}

/**
cdef void fix_cvtsi2sd_from_imm_to_any_instruction(AsmCvtsi2sd node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_cvtsi2sd_from_imm_to_any_instruction(AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_cvtsi2sd_from_any_to_addr_instruction(AsmCvtsi2sd node):
    cdef AsmOperand src = generate_register(REGISTER_KIND.get('Xmm15'))
    cdef AsmOperand dst = node.dst
    cdef AssemblyType assembly_type = BackendDouble()
    node.dst = src
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
*/
static void fix_cvtsi2sd_from_any_to_addr_instruction(AsmCvtsi2sd* node) {
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AsmOperand> dst = std::move(node->dst);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = src;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

/**
cdef void fix_cvtsi2sd_instruction(AsmCvtsi2sd node):
    if isinstance(node.src, AsmImm):
        fix_cvtsi2sd_from_imm_to_any_instruction(node)

    if isinstance(node.dst, (AsmStack, AsmData)):
        fix_cvtsi2sd_from_any_to_addr_instruction(node)
*/
static void fix_cvtsi2sd_instruction(AsmCvtsi2sd* node) {
    if(is_imm_t(node->src->type())) {
        fix_cvtsi2sd_from_imm_to_any_instruction(node);
    }
    if(is_addr_t(node->dst->type())) {
        fix_cvtsi2sd_from_any_to_addr_instruction(node);
    }
}

/**
cdef fix_double_cmp_from_any_to_addr_instruction(AsmCmp node):
    cdef AsmOperand src = node.dst
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Xmm15'))
    cdef AssemblyType assembly_type = BackendDouble()
    node.dst = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_double_cmp_from_any_to_addr_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_cmp_from_quad_word_imm_to_any_instruction(AsmCmp node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = QuadWord()
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_cmp_from_quad_word_imm_to_any_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::shared_ptr<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_cmp_from_addr_to_addr_instruction(AsmCmp node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_cmp_from_addr_to_addr_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_cmp_from_any_to_imm_instruction(AsmCmp node):
    cdef AsmOperand src = node.dst
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R11'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.dst = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_cmp_from_any_to_imm_instruction(AsmCmp* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_cmp_instruction(AsmCmp node):
    if isinstance(node.assembly_type, BackendDouble):
        if isinstance(node.dst, (AsmStack, AsmData)):
            fix_double_cmp_from_any_to_addr_instruction(node)

    else:
        if isinstance(node.src, AsmImm) and \
           node.src.is_quad:
            fix_cmp_from_quad_word_imm_to_any_instruction(node)

        if isinstance(node.src, (AsmStack, AsmData)) and \
           isinstance(node.dst, (AsmStack, AsmData)):
            fix_cmp_from_addr_to_addr_instruction(node)

        elif isinstance(node.dst, AsmImm):
            fix_cmp_from_any_to_imm_instruction(node)
*/
static void fix_cmp_instruction(AsmCmp* node) {
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        if(is_addr_t(node->dst->type())) {
            fix_double_cmp_from_any_to_addr_instruction(node);
        }
    }
    else {
        if(is_imm_t(node->src->type()) &&
           static_cast<AsmImm*>(node->src.get())->is_quad) {
            fix_cmp_from_quad_word_imm_to_any_instruction(node);
        }
        if(is_addr_t(node->src->type()) &&
           is_addr_t(node->dst->type())) {
            fix_cmp_from_addr_to_addr_instruction(node);
        }
        else if(is_imm_t(node->dst->type())) {
            fix_cmp_from_any_to_imm_instruction(node);
        }
    }
}

/**
cdef void fix_push_from_quad_word_imm_to_any_instruction(AsmPush node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = QuadWord()
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_push_from_quad_word_imm_to_any_instruction(AsmPush* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_push_instruction(AsmPush node):
    if isinstance(node.src, AsmImm) and \
       node.src.is_quad:
        fix_push_from_quad_word_imm_to_any_instruction(node)
*/
static void fix_push_instruction(AsmPush* node) {
    if(is_imm_t(node->src->type()) &&
       static_cast<AsmImm*>(node->src.get())->is_quad) {
        fix_push_from_quad_word_imm_to_any_instruction(node);
    }
}

/**
cdef void fix_double_binary_from_any_to_addr_instruction(AsmBinary node):
    cdef AsmOperand src = node.dst
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Xmm15'))
    cdef AssemblyType assembly_type = BackendDouble()
    node.dst = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
    fix_instructions.append(AsmMov(assembly_type, dst, src))
*/
static void fix_double_binary_from_any_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm15);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(assembly_type, src, dst));
    swap_fix_instruction_back();
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(dst), std::move(src)));
}

/**
cdef void fix_binary_from_quad_word_imm_to_any_instruction(AsmBinary node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = QuadWord()
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_binary_from_quad_word_imm_to_any_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_binary_any_from_addr_to_addr_instruction(AsmBinary node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_binary_any_from_addr_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_binary_shx_from_addr_to_addr_instruction(AsmBinary node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Cx'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_binary_shx_from_addr_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Cx);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_binary_imul_from_any_to_addr_instruction(AsmBinary node):
    cdef AsmOperand src = node.dst
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R11'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.dst = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
    fix_instructions.append(AsmMov(assembly_type, dst, src))
*/
static void fix_binary_imul_from_any_to_addr_instruction(AsmBinary* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->dst);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R11);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->dst = dst;
    push_fix_instruction(std::make_unique<AsmMov>(assembly_type, src, dst));
    swap_fix_instruction_back();
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(dst), std::move(src)));
}

/**
cdef void fix_binary_instruction(AsmBinary node):
    if isinstance(node.assembly_type, BackendDouble):
        if isinstance(node.dst, (AsmStack, AsmData)):
            fix_double_binary_from_any_to_addr_instruction(node)

    else:
        if isinstance(node.binary_op,
                      (AsmAdd, AsmSub, AsmBitAnd, AsmBitOr, AsmBitXor)):
            if isinstance(node.src, AsmImm) and \
               node.src.is_quad:
                fix_binary_from_quad_word_imm_to_any_instruction(node)

            if isinstance(node.src, (AsmStack, AsmData)) and \
               isinstance(node.dst, (AsmStack, AsmData)):
                fix_binary_any_from_addr_to_addr_instruction(node)

        elif isinstance(node.binary_op,
                        (AsmBitShiftLeft, AsmBitShiftRight)):
            if isinstance(node.src, AsmImm) and \
               node.src.is_quad:
                fix_binary_from_quad_word_imm_to_any_instruction(node)

            if isinstance(node.src, (AsmStack, AsmData)) and \
               isinstance(node.dst, (AsmStack, AsmData)):
                fix_binary_shx_from_addr_to_addr_instruction(node)

        elif isinstance(node.binary_op, AsmMult):
            if isinstance(node.src, AsmImm) and \
               node.src.is_quad:
                fix_binary_from_quad_word_imm_to_any_instruction(node)

            if isinstance(node.dst, (AsmStack, AsmData)):
                fix_binary_imul_from_any_to_addr_instruction(node)
*/
static void fix_binary_instruction(AsmBinary* node) {
    if(node->assembly_type->type() == AST_T::BackendDouble_t) {
        if(is_addr_t(node->dst->type())) {
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
                if(is_imm_t(node->src->type()) &&
                   static_cast<AsmImm*>(node->src.get())->is_quad) {
                    fix_binary_from_quad_word_imm_to_any_instruction(node);
                }
                if(is_addr_t(node->src->type()) &&
                   is_addr_t(node->dst->type())) {
                    fix_binary_any_from_addr_to_addr_instruction(node);
                }
                break;
            }
            case AST_T::AsmBitShiftLeft_t:
            case AST_T::AsmBitShiftRight_t: {
                if(is_imm_t(node->src->type()) &&
                   static_cast<AsmImm*>(node->src.get())->is_quad) {
                    fix_binary_from_quad_word_imm_to_any_instruction(node);
                }
                if(is_addr_t(node->src->type()) &&
                   is_addr_t(node->dst->type())) {
                    fix_binary_shx_from_addr_to_addr_instruction(node);
                }
                break;
            }
            case AST_T::AsmMult_t: {
                if(is_imm_t(node->src->type()) &&
                   static_cast<AsmImm*>(node->src.get())->is_quad) {
                    fix_binary_from_quad_word_imm_to_any_instruction(node);
                }
                if(is_addr_t(node->dst->type())) {
                    fix_binary_imul_from_any_to_addr_instruction(node);
                }
                break;
            }
            default:
                break;
        }
    }
}

/**
cdef void fix_idiv_from_imm_instruction(AsmIdiv node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_idiv_from_imm_instruction(AsmIdiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_idiv_instruction(AsmIdiv node):
    if isinstance(node.src, AsmImm):
        fix_idiv_from_imm_instruction(node)
*/
static void fix_idiv_instruction(AsmIdiv* node) {
    if(is_imm_t(node->src->type())) {
        fix_idiv_from_imm_instruction(node);
    }
}

/** TODO
cdef void fix_div_from_imm_instruction(AsmDiv node):
    cdef AsmOperand src = node.src
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('R10'))
    cdef AssemblyType assembly_type = node.assembly_type
    node.src = dst
    # only for cython
    fix_instructions[-1] = node
    #
    fix_instructions.append(AsmMov(assembly_type, src, dst))
    swap_fix_instructions_back()
*/
static void fix_div_from_imm_instruction(AsmDiv* node) {
    std::shared_ptr<AsmOperand> src = std::move(node->src);
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::R10);
    std::shared_ptr<AssemblyType> assembly_type = node->assembly_type;
    node->src = dst;
    push_fix_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
    swap_fix_instruction_back();
}

/**
cdef void fix_div_instruction(AsmDiv node):
    if isinstance(node.src, AsmImm):
        fix_div_from_imm_instruction(node)
*/
static void fix_div_instruction(AsmDiv* node) {
    if(is_imm_t(node->src->type())) {
        fix_div_from_imm_instruction(node);
    }
}

/**
cdef void fix_instruction():
    if isinstance(fix_instructions[-1], AsmMov):
        fix_mov_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmMovSx):
        fix_mov_sx_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmMovZeroExtend):
        fix_mov_zero_extend_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmCvttsd2si):
        fix_cvttsd2si_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmCvtsi2sd):
        fix_cvtsi2sd_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmCmp):
        fix_cmp_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmPush):
        fix_push_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmBinary):
        fix_binary_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmIdiv):
        fix_idiv_instruction(fix_instructions[-1])

    elif isinstance(fix_instructions[-1], AsmDiv):
        fix_div_instruction(fix_instructions[-1])
*/
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
        case AST_T::AsmCvttsd2si_t:
            fix_cvttsd2si_instruction(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            fix_cvtsi2sd_instruction(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmCmp_t:
            fix_cmp_instruction(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmPush_t:
            fix_push_instruction(static_cast<AsmPush*>(node));
            break;
        case AST_T::AsmBinary_t:
            fix_binary_instruction(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmIdiv_t:
            fix_idiv_instruction(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            fix_div_instruction(static_cast<AsmDiv*>(node));
            break;
        default:
            break;
    }
}

/**
cdef void fix_function_top_level(AsmFunction node):
    global counter
    global fix_instructions

    counter = 0
    pseudo_map.clear()
    fix_instructions = [allocate_stack_bytes(0)]

    cdef Py_ssize_t instruction
    for instruction in range(len(node.instructions)):
        replace_pseudo_registers(node.instructions[instruction])

        fix_instructions.append(node.instructions[instruction])
        node.instructions[instruction] = None

        fix_instruction()

    fix_allocate_stack_bytes()

    node.instructions.clear()
    node.instructions = fix_instructions
*/
static void fix_function_top_level(AsmFunction* node) {
    std::vector<std::unique_ptr<AsmInstruction>> instructions = std::move(node->instructions);

    node->instructions.clear();
    p_fix_instructions = &node->instructions;
    push_fix_instruction(allocate_stack_bytes(0));

    counter = 0;
    pseudo_map.clear();
    for(size_t instruction = 0; instruction < instructions.size(); instruction++) {
        push_fix_instruction(std::move(instructions[instruction]));

        replace_pseudo_registers(p_fix_instructions->back().get());
        fix_instruction(p_fix_instructions->back().get());
    }
    fix_allocate_stack_bytes();
    p_fix_instructions = nullptr;
}

/** TODO
cdef void fix_static_variable_top_level(AsmStaticVariable node):
    pass
*/

/** TODO
cdef void fix_top_level(AsmTopLevel node):
    if isinstance(node, AsmFunction):
        fix_function_top_level(node)
    elif isinstance(node, AsmStaticVariable):
        fix_static_variable_top_level(node)
    else:

        raise RuntimeError(
            "An error occurred in stack management, not all nodes were visited")
*/

/** TODO
cdef void fix_program(AsmProgram node):
    cdef Py_ssize_t top_level
    for top_level in range(len(node.top_levels)):
        fix_top_level(node.top_levels[top_level])
*/

/** TODO
cdef void fix_stack(AsmProgram asm_ast):

    fix_program(asm_ast)
*/
