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
std::shared_ptr<AsmStack> replace_pseudo_register_stack(AsmPseudo* node) {
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
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef AsmBinary allocate_stack_bytes(int32 byte):
    cdef AsmBinaryOp binary_op = AsmSub()
    cdef AssemblyType assembly_type = QuadWord()
    cdef AsmOperand src = AsmImm(TIdentifier(str(byte)))
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Sp'))
    return AsmBinary(binary_op, assembly_type, src, dst)
*/
std::unique_ptr<AsmBinary> allocate_stack_bytes(TInt byte) {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmSub>();
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    TIdentifier value = std::to_string(byte);
    std::shared_ptr<AsmOperand> src = std::make_shared<AsmImm>(std::move(value));
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Sp);
    return std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type), std::move(src), std::move(dst));
}

/**
cdef AsmBinary deallocate_stack_bytes(int32 byte):
    cdef AsmBinaryOp binary_op = AsmAdd()
    cdef AssemblyType assembly_type = QuadWord()
    cdef AsmOperand src = AsmImm(TIdentifier(str(byte)))
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Sp'))
    return AsmBinary(binary_op, assembly_type, src, dst)
*/
std::unique_ptr<AsmBinary> deallocate_stack_bytes(TInt byte) {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmAdd>();
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<QuadWord>();
    TIdentifier value = std::to_string(byte);
    std::shared_ptr<AsmOperand> src = std::make_shared<AsmImm>(std::move(value));
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Sp);
    return std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type), std::move(src), std::move(dst));
}

/**
cdef list[AsmInstruction] fun_instructions = []
*/
static std::vector<std::unique_ptr<AsmInstruction>>* p_instructions;

static void push_instruction(std::unique_ptr<AsmInstruction>&& instruction) {
    p_instructions->push_back(std::move(instruction));
}

/**
cdef void insert_fun_instruction(Py_ssize_t k, AsmInstruction node):
    fun_instructions[k:k] = [node]
*/

/**
cdef void prepend_alloc_stack():
    cdef int32 byte = -1 * counter

    if byte % 8 != 0:

        raise RuntimeError(
            f"An error occurred in function stack allocation, stack alignment {byte} is not a multiple of 8")

    cdef item_instr = allocate_stack_bytes(byte)
    insert_fun_instruction(0, item_instr)
*/
static void prepend_alloc_stack() {
    TInt byte = -1 * counter;
    if(byte % 8 != 0) {
        RAISE_INTERNAL_ERROR;
    }

    push_instruction(allocate_stack_bytes(byte));
}

/** TODO
cdef void correct_any_from_addr_to_addr_instruction(Py_ssize_t i, Py_ssize_t k):
    # mov | cmp | add | sub | and | or | xor (addr, addr)
    # $ mov addr1, addr2 ->
    #     $ mov addr1, reg
    #     $ mov reg  , addr2
    cdef AsmOperand src_src = fun_instructions[i].src
    fun_instructions[i].src = generate_register(REGISTER_KIND.get('R10'))
    cdef AsmInstruction item_instr = AsmMov(fun_instructions[i].assembly_type, src_src, fun_instructions[i].src)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef void correct_double_mov_from_addr_to_addr_instructions(Py_ssize_t i, Py_ssize_t k):
    # mov<q> (_, addr)
    # $ mov<q> addr1, addr2 ->
    #     $ mov    addr1, reg
    #     $ mov<q> reg  , addr2
    cdef AsmOperand src_src = fun_instructions[i].src
    fun_instructions[i].src = generate_register(REGISTER_KIND.get('Xmm14'))
    cdef AsmInstruction item_instr = AsmMov(fun_instructions[i].assembly_type, src_src, fun_instructions[i].src)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef void correct_mov_sx_from_imm_to_any_instructions(Py_ssize_t i, Py_ssize_t k):
    # movsx (imm, _)
    # $ movsx imm, _ ->
    #     $ mov   imm, reg
    #     $ movsx reg, _
    cdef AsmOperand src_src = fun_instructions[i].src
    fun_instructions[i].src = generate_register(REGISTER_KIND.get('R10'))
    cdef AsmInstruction item_instr = AsmMov(LongWord(), src_src, fun_instructions[i].src)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef void correct_mov_sx_zero_extend_from_any_to_addr_instructions(Py_ssize_t i, Py_ssize_t k):
    # movsx | mov0x (_, addr)
    # $ movsx _, addr ->
    #     $ movsx _  , reg
    #     $ mov   reg, addr
    cdef AsmOperand src_dst = fun_instructions[i].dst
    fun_instructions[i].dst = generate_register(REGISTER_KIND.get('R11'))
    cdef AsmInstruction item_instr = AsmMov(QuadWord(), fun_instructions[i].dst, src_dst)
    insert_fun_instruction(k, item_instr)
*/

/** TODO
cdef void correct_mov_zero_extend_from_any_to_any_instructions(Py_ssize_t i):
    cdef AsmInstruction item_instr = AsmMov(LongWord(), fun_instructions[i].src, fun_instructions[i].dst)
    fun_instructions[i] = item_instr
*/

/** TODO
cdef void correct_cvttsd2si_from_any_to_addr_instructions(Py_ssize_t i, Py_ssize_t k):
    # cvttsd2si (_, addr)
    # $ cvttsd2si _, addr ->
    #     $ cvttsd2si _  , reg
    #     $ mov       reg, addr
    cdef AsmOperand src_dst = fun_instructions[i].dst
    fun_instructions[i].dst = generate_register(REGISTER_KIND.get('R11'))
    cdef AsmInstruction item_instr = AsmMov(fun_instructions[i].assembly_type, fun_instructions[i].dst, src_dst)
    insert_fun_instruction(k, item_instr)
*/

/** TODO
cdef void correct_cvtsi2sd_from_imm_to_any_instructions(Py_ssize_t i, Py_ssize_t k):
    # cvtsi2sd (imm, _)
    # $ cvtsi2sd imm, _ ->
    #     $ mov      imm, reg
    #     $ cvtsi2sd reg, _
    cdef AsmOperand src_src = fun_instructions[i].src
    fun_instructions[i].src = generate_register(REGISTER_KIND.get('R10'))
    cdef AsmInstruction item_instr = AsmMov(fun_instructions[i].assembly_type, src_src, fun_instructions[i].src)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef void correct_cvtsi2sd_from_any_to_addr_instructions(Py_ssize_t i, Py_ssize_t k):
    # cvtsi2sd | add<q> | sub<q> | mul<q> | div<q> | xor<q> (_, addr)
    # $ cvtsi2sd _, addr ->
    #     $ cvtsi2sd _  , reg
    #     $ mov      reg, addr
    cdef AsmOperand src_dst = fun_instructions[i].dst
    fun_instructions[i].dst = generate_register(REGISTER_KIND.get('Xmm15'))
    cdef AsmInstruction item_instr = AsmMov(BackendDouble(), fun_instructions[i].dst, src_dst)
    insert_fun_instruction(k, item_instr)
*/

/** TODO
cdef void correct_cmp_from_any_to_imm_instructions(Py_ssize_t i, Py_ssize_t k):
    # cmp (_, imm)
    # $ cmp reg1, imm ->
    #     $ mov imm , reg2
    #     $ cmp reg1, reg2
    cdef AsmOperand src_dst = fun_instructions[i].dst
    fun_instructions[i].dst = generate_register(REGISTER_KIND.get('R11'))
    cdef AsmInstruction item_instr = AsmMov(fun_instructions[i].assembly_type, src_dst, fun_instructions[i].dst)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef correct_double_cmp_from_any_to_addr_instructions(Py_ssize_t i, Py_ssize_t k):
    # cmp<d> (_, addr)
    # $ cmp<d> _, addr ->
    #     $ mov    addr, reg
    #     $ cmp<d> _   , reg
    cdef AsmOperand dst_dst = fun_instructions[i].dst
    fun_instructions[i].dst = generate_register(REGISTER_KIND.get('Xmm15'))
    cdef AsmInstruction item_instr = AsmMov(BackendDouble(), dst_dst, fun_instructions[i].dst)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef void correct_shl_shr_from_addr_to_addr(Py_ssize_t i, Py_ssize_t k):
    # shl | shr (addr, addr)
    # $ shl addr1, addr2 ->
    #     $ mov addr1, reg
    #     $ shl reg, addr2
    cdef AsmOperand src_src = fun_instructions[i].src
    fun_instructions[i].src = generate_register(REGISTER_KIND.get('Cx'))
    cdef AsmInstruction item_instr = AsmMov(fun_instructions[i].assembly_type, src_src, fun_instructions[i].src)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef void correct_mul_from_any_to_addr(Py_ssize_t i, Py_ssize_t k):
    # imul (_, addr)
    # $ imul imm, addr ->
    #     $ mov  addr, reg
    #     $ imul imm , reg
    #     $ mov  reg , addr
    cdef AsmOperand src_src = fun_instructions[i].dst
    cdef AsmOperand dst_dst = fun_instructions[i].dst
    fun_instructions[i].dst = generate_register(REGISTER_KIND.get('R11'))
    cdef AsmInstruction item_instr1 = AsmMov(fun_instructions[i].assembly_type, src_src, fun_instructions[i].dst)
    cdef AsmInstruction item_instr2 = AsmMov(fun_instructions[i].assembly_type, fun_instructions[i].dst, dst_dst)
    insert_fun_instruction(k - 1, item_instr1)
    insert_fun_instruction(k + 1, item_instr2)
*/

/** TODO
cdef void correct_binary_from_any_to_addr_instructions(Py_ssize_t i, Py_ssize_t k):
    # add<q> | sub<q> | mul<q> | div<q> | xor<q> (_, addr)
    # $ add<q> _, addr ->
    #     $ add<q> _  , reg
    #     $ mov    reg, addr
    cdef AsmOperand src_dst = fun_instructions[i].dst
    fun_instructions[i].dst = generate_register(REGISTER_KIND.get('Xmm15'))
    cdef AsmInstruction item_instr1 = AsmMov(BackendDouble(), src_dst, fun_instructions[i].dst)
    cdef AsmInstruction item_instr2 = AsmMov(BackendDouble(), fun_instructions[i].dst, src_dst)
    insert_fun_instruction(k - 1, item_instr1)
    insert_fun_instruction(k + 1, item_instr2)
*/

/** TODO
cdef void correct_div_from_imm(Py_ssize_t i, Py_ssize_t k):
    # idiv | div (imm)
    # $ idiv imm ->
    #     $ mov  imm, reg
    #     $ idiv reg
    cdef AsmOperand src_src = fun_instructions[i].src
    fun_instructions[i].src = generate_register(REGISTER_KIND.get('R10'))
    cdef AsmInstruction item_instr = AsmMov(fun_instructions[i].assembly_type, src_src, fun_instructions[i].src)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef void correct_any_from_quad_word_imm_to_any(Py_ssize_t i, Py_ssize_t k):
    # mov | cmp | push | add | sub | mul (q imm, _)
    # $ mov imm<q>, _ ->
    #     $ mov imm<q>, reg
    #     $ mov reg   , _
    cdef AsmOperand src_src = fun_instructions[i].src
    fun_instructions[i].src = generate_register(REGISTER_KIND.get('R10'))
    cdef AsmInstruction item_instr = AsmMov(QuadWord(), src_src, fun_instructions[i].src)
    insert_fun_instruction(k - 1, item_instr)
*/

/** TODO
cdef bint is_from_long_imm_instruction(Py_ssize_t i):
    return isinstance(fun_instructions[i].src, AsmImm) and \
            int(fun_instructions[i].src.value.str_t) > 2147483647
*/

/** TODO
cdef bint is_from_imm_instruction(Py_ssize_t i):
    return isinstance(fun_instructions[i].src, AsmImm)
*/

/** TODO
cdef bint is_to_imm_instruction(Py_ssize_t i):
    return isinstance(fun_instructions[i].dst, AsmImm)
*/

/** TODO
cdef bint is_from_addr_instruction(Py_ssize_t i):
    return isinstance(fun_instructions[i].src, (AsmStack, AsmData))
*/

/** TODO
cdef bint is_to_addr_instruction(Py_ssize_t i):
    return isinstance(fun_instructions[i].dst, (AsmStack, AsmData))
*/

/** TODO
cdef bint is_from_addr_to_addr_instruction(Py_ssize_t i):
    return is_from_addr_instruction(i) and \
           is_to_addr_instruction(i)
*/

/** TODO
cdef void correct_function_top_level(AsmFunction node):
    global fun_instructions
    fun_instructions = node.instructions

    cdef Py_ssize_t i, k
    cdef Py_ssize_t instruction
    cdef Py_ssize_t count_insert = 0
    cdef Py_ssize_t l = len(fun_instructions)
    for instruction in range(l):
        k = l - instruction
        i = - (instruction + 1 + count_insert)

        replace_pseudo_registers(fun_instructions[i])

        if isinstance(fun_instructions[i], AsmMov):
            if isinstance(fun_instructions[i].assembly_type, BackendDouble):
                if is_from_addr_to_addr_instruction(i):
                    correct_double_mov_from_addr_to_addr_instructions(i, k)
                    count_insert += 1
            else:
                if is_from_long_imm_instruction(i):
                    correct_any_from_quad_word_imm_to_any(i, k)
                    count_insert += 1

                if is_from_addr_to_addr_instruction(i):
                    correct_any_from_addr_to_addr_instruction(i, k)
                    count_insert += 1

        elif isinstance(fun_instructions[i], AsmMovSx):
            if is_from_imm_instruction(i):
                correct_mov_sx_from_imm_to_any_instructions(i, k)
                k += 1
                count_insert += 1

            if is_to_addr_instruction(i):
                correct_mov_sx_zero_extend_from_any_to_addr_instructions(i, k)
                count_insert += 1

        elif isinstance(fun_instructions[i], AsmMovZeroExtend):
            correct_mov_zero_extend_from_any_to_any_instructions(i)

            if is_to_addr_instruction(i):
                correct_mov_sx_zero_extend_from_any_to_addr_instructions(i, k)
                count_insert += 1

        elif isinstance(fun_instructions[i], AsmCvttsd2si):
            if is_to_addr_instruction(i):
                correct_cvttsd2si_from_any_to_addr_instructions(i, k)
                count_insert += 1

        elif isinstance(fun_instructions[i], AsmCvtsi2sd):
            if is_from_imm_instruction(i):
                correct_cvtsi2sd_from_imm_to_any_instructions(i, k)
                k += 1
                count_insert += 1

            if is_to_addr_instruction(i):
                correct_cvtsi2sd_from_any_to_addr_instructions(i, k)
                count_insert += 1

        elif isinstance(fun_instructions[i], AsmCmp):
            if isinstance(fun_instructions[i].assembly_type, BackendDouble):
                if is_to_addr_instruction(i):
                    correct_double_cmp_from_any_to_addr_instructions(i, k)
                    count_insert += 1
            else:
                if is_from_long_imm_instruction(i):
                    correct_any_from_quad_word_imm_to_any(i, k)
                    count_insert += 1

                if is_from_addr_to_addr_instruction(i):
                    correct_any_from_addr_to_addr_instruction(i, k)
                    count_insert += 1

                elif is_to_imm_instruction(i):
                    correct_cmp_from_any_to_imm_instructions(i, k)
                    count_insert += 1

        elif isinstance(fun_instructions[i], AsmPush):
            if is_from_long_imm_instruction(i):
                correct_any_from_quad_word_imm_to_any(i, k)
                count_insert += 1

        elif isinstance(fun_instructions[i], AsmBinary):
            if isinstance(fun_instructions[i].assembly_type, BackendDouble):
                if is_to_addr_instruction(i):
                    correct_binary_from_any_to_addr_instructions(i, k)
                    count_insert += 2
            else:
                if isinstance(fun_instructions[i].binary_op,
                              (AsmAdd, AsmSub, AsmBitAnd, AsmBitOr, AsmBitXor)):
                    if is_from_long_imm_instruction(i):
                        correct_any_from_quad_word_imm_to_any(i, k)
                        count_insert += 1

                    if is_from_addr_to_addr_instruction(i):
                        correct_any_from_addr_to_addr_instruction(i, k)
                        count_insert += 1

                elif isinstance(fun_instructions[i].binary_op,
                                (AsmBitShiftLeft, AsmBitShiftRight)):
                    if is_from_long_imm_instruction(i):
                        correct_any_from_quad_word_imm_to_any(i, k)
                        count_insert += 1

                    if is_from_addr_to_addr_instruction(i):
                        correct_shl_shr_from_addr_to_addr(i, k)
                        count_insert += 1

                elif isinstance(fun_instructions[i].binary_op, AsmMult):
                    if is_from_long_imm_instruction(i):
                        correct_any_from_quad_word_imm_to_any(i, k)
                        k += 1
                        count_insert += 1

                    if is_to_addr_instruction(i):
                        correct_mul_from_any_to_addr(i, k)
                        count_insert += 2

        elif isinstance(fun_instructions[i], (AsmIdiv, AsmDiv)):
            if is_from_imm_instruction(i):
                correct_div_from_imm(i, k)
                count_insert += 1

    prepend_alloc_stack()
*/

/** TODO
cdef void correct_variable_stack_top_level(AsmStaticVariable node):
    pass
*/

/** TODO
cdef void correct_top_level(AsmTopLevel node):
    if isinstance(node, AsmFunction):
        correct_function_top_level(node)
    elif isinstance(node, AsmStaticVariable):
        correct_variable_stack_top_level(node)
    elif isinstance(node, AsmStaticConstant):
        pass
    else:

        raise RuntimeError(
            "An error occurred in stack management, not all nodes were visited")
*/

/** TODO
cdef void init_correct_instructions():
    global counter
    counter = 0
    pseudo_map.clear()
*/

/** TODO
cdef void correct_instructions(AsmProgram node):
    cdef Py_ssize_t top_level
    for top_level in range(len(node.top_levels)):
        init_correct_instructions()
        correct_top_level(node.top_levels[top_level])
*/

/** TODO
cdef void correct_stack(AsmProgram asm_ast):

    correct_instructions(asm_ast)
*/