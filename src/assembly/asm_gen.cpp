#include "assembly/asm_gen.hpp"
#include "util/error.hpp"
#include "util/names.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/backend_st.hpp"
#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"
#include "ast/asm_ast.hpp"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

/**
cdef dict[str, str] static_const_label_map = {}
*/
static std::unordered_map<TIdentifier, TIdentifier> static_const_label_map;

/**
cdef TInt generate_alignment(Type node):
    if isinstance(node, (Int, UInt)):
        return TInt(4)
    elif isinstance(node, (Long, Double, ULong)):
        return TInt(8)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/
static TInt generate_alignment(Type* node) {
    switch(node->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return 4;
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::ULong_t:
            return 8;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef AsmImm generate_int_imm_operand(CConstInt node):
    cdef bint is_quad = False
    cdef TIdentifier value = TIdentifier(str(node.value.int_t))
    return AsmImm(value, is_quad)
*/
static std::shared_ptr<AsmImm> generate_int_imm_operand(CConstInt* node) {
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(false, std::move(value));
}

/**
cdef AsmImm generate_long_imm_operand(CConstLong node):
    cdef bint is_quad = node.value.long_t > (<int64>2147483647)
    cdef TIdentifier value = TIdentifier(str(node.value.long_t))
    return AsmImm(value, is_quad)
*/
static std::shared_ptr<AsmImm> generate_long_imm_operand(CConstLong* node) {
    bool is_quad = node->value > 2147483647;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
}

/**
cdef AsmImm generate_uint_imm_operand(CConstUInt node):
    cdef bint is_quad = int(node.value.uint_t) > (<uint32>2147483647)
    cdef TIdentifier value = TIdentifier(str(node.value.uint_t))
    return AsmImm(value, is_quad)
*/
static std::shared_ptr<AsmImm> generate_uint_imm_operand(CConstUInt* node) {
    bool is_quad = node->value > 2147483647;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
}

/**
cdef AsmImm generate_ulong_imm_operand(CConstULong node):
    cdef bint is_quad = int(node.value.ulong_t) > (<uint64>2147483647)
    cdef TIdentifier value = TIdentifier(str(node.value.ulong_t))
    return AsmImm(value, is_quad)
*/
static std::shared_ptr<AsmImm> generate_ulong_imm_operand(CConstULong* node) {
    bool is_quad = node->value > 2147483647;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
}

static void append_double_static_constant_top_level(TIdentifier name, double value, TInt byte);

/**
cdef AsmData generate_double_static_constant_operand(double value, int32 byte):
    cdef str str_value = str(value)
    cdef TIdentifier static_constant_label
    if str_value in static_const_label_map:
        static_constant_label = TIdentifier(static_const_label_map[str_value])
    else:
        static_constant_label = represent_label_identifier("double")
        static_const_label_map[str_value] = static_constant_label.str_t
        append_double_static_constant_top_level(copy_identifier(static_constant_label), value, byte)
    return AsmData(static_constant_label)
*/
static std::shared_ptr<AsmData> generate_double_static_constant_operand(double value, TInt byte) {
    TIdentifier static_constant_label;
    TIdentifier str_value = std::to_string(value);
    if(static_const_label_map.find(str_value) != static_const_label_map.end()) {
        static_constant_label = static_const_label_map[str_value];
    }
    else {
        static_constant_label = represent_label_identifier("double");
        static_const_label_map[str_value] = static_constant_label;
        append_double_static_constant_top_level(static_constant_label, value, byte);
    }
    return std::make_shared<AsmData>(std::move(static_constant_label));
}

/**
cdef AsmData generate_double_constant_operand(CConstDouble node):
    return generate_double_static_constant_operand(node.value.double_t, 8)
*/
static std::shared_ptr<AsmData> generate_double_constant_operand(CConstDouble* node) {
    return generate_double_static_constant_operand(node->value, 8);
}

/**
cdef AsmOperand generate_constant_operand(TacConstant node):
    if isinstance(node.constant, CConstInt):
        return generate_int_imm_operand(node.constant)
    elif isinstance(node.constant, CConstLong):
        return generate_long_imm_operand(node.constant)
    elif isinstance(node.constant, CConstDouble):
        return generate_double_constant_operand(node.constant)
    elif isinstance(node.constant, CConstUInt):
        return generate_uint_imm_operand(node.constant)
    elif isinstance(node.constant, CConstULong):
        return generate_ulong_imm_operand(node.constant)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/
static std::shared_ptr<AsmOperand> generate_constant_operand(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstInt_t:
            return generate_int_imm_operand(static_cast<CConstInt*>(node->constant.get()));
        case AST_T::CConstLong_t:
            return generate_long_imm_operand(static_cast<CConstLong*>(node->constant.get()));
        case AST_T::CConstDouble_t:
            return generate_double_constant_operand(static_cast<CConstDouble*>(node->constant.get()));
        case AST_T::CConstUInt_t:
            return generate_uint_imm_operand(static_cast<CConstUInt*>(node->constant.get()));
        case AST_T::CConstULong_t:
            return generate_ulong_imm_operand(static_cast<CConstULong*>(node->constant.get()));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef AsmPseudo generate_pseudo_operand(TacVariable node):
    cdef TIdentifier name = copy_identifier(node.name)
    return AsmPseudo(name)
*/
static std::shared_ptr<AsmPseudo> generate_pseudo_operand(TacVariable* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmPseudo>(std::move(name));
}

/**
cdef AsmOperand generate_operand(TacValue node):
    # operand = Imm(int) | Reg(reg) | Pseudo(identifier) | Stack(int) | Data(identifier)
    if isinstance(node, TacConstant):
        return generate_constant_operand(node)
    elif isinstance(node, TacVariable):
        return generate_pseudo_operand(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/
// operand = Imm(int) | Reg(reg) | Pseudo(identifier) | Stack(int) | Data(identifier)
static std::shared_ptr<AsmOperand> generate_operand(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return generate_constant_operand(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return generate_pseudo_operand(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/** TODO
cdef AsmCondCode generate_signed_condition_code(TacBinaryOp node):
    # signed_cond_code = E | NE | G | GE | L | LE
    if isinstance(node, TacEqual):
        return AsmE()
    elif isinstance(node, TacNotEqual):
        return AsmNE()
    elif isinstance(node, TacLessThan):
        return AsmL()
    elif isinstance(node, TacLessOrEqual):
        return AsmLE()
    elif isinstance(node, TacGreaterThan):
        return AsmG()
    elif isinstance(node, TacGreaterOrEqual):
        return AsmGE()
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef AsmCondCode generate_unsigned_condition_code(TacBinaryOp node):
    # unsigned_cond_code = E | NE | A | AE | B | BE
    if isinstance(node, TacEqual):
        return AsmE()
    elif isinstance(node, TacNotEqual):
        return AsmNE()
    elif isinstance(node, TacLessThan):
        return AsmB()
    elif isinstance(node, TacLessOrEqual):
        return AsmBE()
    elif isinstance(node, TacGreaterThan):
        return AsmA()
    elif isinstance(node, TacGreaterOrEqual):
        return AsmAE()
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef AsmBinaryOp generate_binary_op(TacBinaryOp node):
    # binary_operator = Add | Sub | Mult | DivDouble | BitAnd | BitOr | BitXor | BitShiftLeft | BitShiftRight
    if isinstance(node, TacAdd):
        return AsmAdd()
    elif isinstance(node, TacSubtract):
        return AsmSub()
    elif isinstance(node, TacMultiply):
        return AsmMult()
    elif isinstance(node, TacDivide):
        return AsmDivDouble()
    elif isinstance(node, TacBitAnd):
        return AsmBitAnd()
    elif isinstance(node, TacBitOr):
        return AsmBitOr()
    elif isinstance(node, TacBitXor):
        return AsmBitXor()
    elif isinstance(node, TacBitShiftLeft):
        return AsmBitShiftLeft()
    elif isinstance(node, TacBitShiftRight):
        return AsmBitShiftRight()
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef AsmUnaryOp generate_unary_op(TacUnaryOp node):
    # unary_operator = Not | Neg | Shr
    if isinstance(node, TacComplement):
        return AsmNot()
    elif isinstance(node, TacNegate):
        return AsmNeg()
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef bint is_constant_value_signed(TacConstant node):
    return is_const_signed(node.constant)
*/

/** TODO
cdef bint is_variable_value_signed(TacVariable node):
    return is_type_signed(symbol_table[node.name.str_t].type_t)
*/

/** TODO
cdef bint is_value_signed(TacValue node):
    if isinstance(node, TacConstant):
        return is_constant_value_signed(node)
    elif isinstance(node, TacVariable):
        return is_variable_value_signed(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef bint is_constant_value_32_bits(TacConstant node):
    return isinstance(node.constant, (CConstInt, CConstUInt))
*/

/** TODO
cdef bint is_variable_value_32_bits(TacVariable node):
    return isinstance(symbol_table[node.name.str_t].type_t, (Int, UInt))
*/

/** TODO
cdef bint is_value_32_bits(TacValue node):
    if isinstance(node, TacConstant):
        return is_constant_value_32_bits(node)
    elif isinstance(node, TacVariable):
        return is_variable_value_32_bits(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef bint is_constant_value_double(TacConstant node):
    return isinstance(node.constant, CConstDouble)
*/

/** TODO
cdef bint is_variable_value_double(TacVariable node):
    return isinstance(symbol_table[node.name.str_t].type_t, Double)
*/

/** TODO
cdef bint is_value_double(TacValue node):
    if isinstance(node, TacConstant):
        return is_constant_value_double(node)
    elif isinstance(node, TacVariable):
        return is_variable_value_double(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef AssemblyType generate_constant_assembly_type(TacConstant node):
    if isinstance(node.constant, (CConstInt, CConstUInt)):
        return LongWord()
    elif isinstance(node.constant, CConstDouble):
        return BackendDouble()
    elif isinstance(node.constant, (CConstLong, CConstULong)):
        return QuadWord()
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef AssemblyType generate_variable_assembly_type(TacVariable node):
    return convert_backend_assembly_type(node.name.str_t)
*/

/** TODO
cdef AssemblyType generate_assembly_type(TacValue node):
    if isinstance(node, TacConstant):
        return generate_constant_assembly_type(node)
    elif isinstance(node, TacVariable):
        return generate_variable_assembly_type(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef list[AsmInstruction] instructions = []
*/

/** TODO
cdef void generate_allocate_stack_instructions(int32 byte):
    instructions.append(allocate_stack_bytes(byte))
*/

/** TODO
cdef void generate_deallocate_stack_instructions(int32 byte):
    instructions.append(deallocate_stack_bytes(byte))
*/

/** TODO
cdef void generate_zero_out_xmm_reg_instructions():
    cdef AsmBinaryOp binary_op = AsmBitXor()
    cdef AsmOperand src = generate_register(REGISTER_KIND.get('Xmm0'))
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Xmm0'))
    cdef AssemblyType assembly_type_src = BackendDouble()
    instructions.append(AsmBinary(binary_op, assembly_type_src, src, dst))
*/

/** TODO
cdef list[str] arg_registers = ["Di", "Si", "Dx", "Cx", "R8", "R9"]
*/

/** TODO
cdef list[str] arg_sse_registers = ["Xmm0", "Xmm1", "Xmm2", "Xmm3", "Xmm4", "Xmm5", "Xmm6", "Xmm7"]
*/

/** TODO
cdef void generate_reg_arg_fun_call_instructions(TacValue node, str arg_register):
    cdef AsmOperand src = generate_operand(node)
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get(arg_register))
    cdef AssemblyType assembly_type = generate_assembly_type(node)
    instructions.append(AsmMov(assembly_type, src, dst))
*/

/** TODO
cdef void generate_stack_arg_fun_call_instructions(TacValue node):
    cdef AsmOperand src = generate_operand(node)
    cdef AssemblyType assembly_type = generate_assembly_type(node)
    if isinstance(src, (AsmRegister, AsmImm)) or \
       isinstance(assembly_type, (QuadWord, BackendDouble)):
        instructions.append(AsmPush(src))
        return
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Ax'))
    instructions.append(AsmMov(assembly_type, src, dst))
    instructions.append(AsmPush(dst))
*/

/** TODO
cdef void generate_fun_call_instructions(TacFunCall node):
    cdef int32 stack_padding = 0
    if len(node.args) % 2 == 1:
        stack_padding = 8
        generate_allocate_stack_instructions(stack_padding)

    cdef Py_ssize_t i
    cdef list[Py_ssize_t] i_regs = []
    cdef list[Py_ssize_t] i_sse_regs = []
    cdef list[Py_ssize_t] i_stacks = []
    for i in range(len(node.args)):
        if is_value_double(node.args[i]):
            if len(i_sse_regs) < 8:
                i_sse_regs.append(i)
            else:
                i_stacks.append(i)
        else:
            if len(i_regs) < 6:
                i_regs.append(i)
            else:
                i_stacks.append(i)

    for i in range(len(i_regs)):
        generate_reg_arg_fun_call_instructions(node.args[i_regs[i]], arg_registers[i])

    for i in range(len(i_sse_regs)):
        generate_reg_arg_fun_call_instructions(node.args[i_sse_regs[i]], arg_sse_registers[i])

    for i in range(len(i_stacks)):
        stack_padding += 8
        generate_stack_arg_fun_call_instructions(node.args[i_stacks[- (i + 1)]])

    cdef TIdentifier name = copy_identifier(node.name)
    instructions.append(AsmCall(name))

    if stack_padding:
        generate_deallocate_stack_instructions(stack_padding)

    cdef AsmOperand src
    if is_value_double(node.dst):
        src = generate_register(REGISTER_KIND.get('Xmm0'))
    else:
        src = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_dst = generate_assembly_type(node.dst)
    instructions.append(AsmMov(assembly_type_dst, src, dst))
*/

/** TODO
cdef void generate_sign_extend_instructions(TacSignExtend node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    instructions.append(AsmMovSx(src, dst))
*/

/** TODO
cdef void generate_zero_extend_instructions(TacZeroExtend node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    instructions.append(AsmMovZeroExtend(src, dst))
*/

/** TODO
cdef void generate_imm_truncate_instructions(AsmImm node):
    if node.is_quad:
        node.value.str_t = str(<uint64>(str_to_uint64(node.value.str_t) - (<uint64>4294967296)))
*/

/** TODO
cdef void generate_truncate_instructions(TacTruncate node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src = LongWord()
    if isinstance(src, AsmImm):
        generate_imm_truncate_instructions(src)
    instructions.append(AsmMov(assembly_type_src, src, dst))
*/

/** TODO
cdef void generate_double_to_signed_instructions(TacDoubleToInt node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src = generate_assembly_type(node.dst)
    instructions.append(AsmCvttsd2si(assembly_type_src, src, dst))
*/

/** TODO
cdef void generate_uint_double_to_unsigned_instructions(TacDoubleToUInt node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AsmOperand src_dst = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand dst_src = generate_register(REGISTER_KIND.get('Ax'))
    cdef AssemblyType assembly_type_src = QuadWord()
    cdef AssemblyType assembly_type_dst = LongWord()
    instructions.append(AsmCvttsd2si(assembly_type_src, src, src_dst))
    instructions.append(AsmMov(assembly_type_dst, dst_src, dst))
*/

/** TODO
cdef void generate_ulong_double_to_unsigned_instructions(TacDoubleToUInt node):
    cdef AsmOperand upper_bound_sd = generate_double_static_constant_operand(9223372036854775808.0, 8)
    cdef AsmOperand src = generate_operand(node.src)
    cdef AssemblyType assembly_type_sd = BackendDouble()
    cdef AsmCondCode cond_code_ae = AsmAE()
    cdef TIdentifier target_out_of_range = represent_label_identifier("sd2si_out_of_range")
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_si = QuadWord()
    cdef TIdentifier target_after = represent_label_identifier("sd2si_after")
    cdef AsmOperand dst_out_of_range_sd = generate_register(REGISTER_KIND.get('Xmm1'))
    cdef AsmBinaryOp binary_op_out_of_range_sd_sub = AsmSub()
    cdef AsmOperand upper_bound_si = AsmImm(TIdentifier("9223372036854775808"), True)
    cdef AsmOperand src_out_of_range_si = generate_register(REGISTER_KIND.get('Dx'))
    cdef AsmBinaryOp binary_op_out_of_range_si_add = AsmAdd()
    instructions.append(AsmCmp(assembly_type_sd, upper_bound_sd, src))
    instructions.append(AsmJmpCC(cond_code_ae, target_out_of_range))
    instructions.append(AsmCvttsd2si(assembly_type_si, src, dst))
    instructions.append(AsmJmp(target_after))
    instructions.append(AsmLabel(target_out_of_range))
    instructions.append(AsmMov(assembly_type_sd, src, dst_out_of_range_sd))
    instructions.append(AsmBinary(binary_op_out_of_range_sd_sub, assembly_type_sd, upper_bound_sd, dst_out_of_range_sd))
    instructions.append(AsmCvttsd2si(assembly_type_si, dst_out_of_range_sd, dst))
    instructions.append(AsmMov(assembly_type_si, upper_bound_si, src_out_of_range_si))
    instructions.append(AsmBinary(binary_op_out_of_range_si_add, assembly_type_si, src_out_of_range_si, dst))
    instructions.append(AsmLabel(target_after))
*/

/** TODO
cdef void generate_double_to_unsigned_instructions(TacDoubleToUInt node):
    if is_value_32_bits(node.dst):
        generate_uint_double_to_unsigned_instructions(node)
    else:
        generate_ulong_double_to_unsigned_instructions(node)
*/

/** TODO
cdef void generate_signed_to_double_instructions(TacIntToDouble node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src = generate_assembly_type(node.src)
    instructions.append(AsmCvtsi2sd(assembly_type_src, src, dst))
*/

/** TODO
cdef void generate_uint_unsigned_to_double_instructions(TacUIntToDouble node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AsmOperand src_dst = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand dst_src = generate_register(REGISTER_KIND.get('Ax'))
    cdef AssemblyType assembly_type_dst = QuadWord()
    instructions.append(AsmMovZeroExtend(src, src_dst))
    instructions.append(AsmCvtsi2sd(assembly_type_dst, dst_src, dst))
*/

/** TODO
cdef void generate_ulong_unsigned_to_double_instructions(TacUIntToDouble node):
    cdef AsmOperand lower_bound_si = AsmImm(TIdentifier("0"), False)
    cdef AsmOperand src = generate_operand(node.src)
    cdef AssemblyType assembly_type_si = QuadWord()
    cdef AsmCondCode cond_code_l = AsmL()
    cdef TIdentifier target_out_of_range = represent_label_identifier("si2sd_out_of_range")
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef TIdentifier target_after = represent_label_identifier("si2sd_after")
    cdef AsmOperand dst_out_of_range_si = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand dst_out_of_range_si_shr = generate_register(REGISTER_KIND.get('Dx'))
    cdef AsmUnaryOp unary_op_out_of_range_si_shr = AsmShr()
    cdef AsmOperand set_bit_si = AsmImm(TIdentifier("1"), False)
    cdef AsmBinaryOp binary_op_out_of_range_si_and = AsmBitAnd()
    cdef AsmBinaryOp binary_op_out_of_range_si_or = AsmBitOr()
    cdef AssemblyType assembly_type_sq = BackendDouble()
    cdef AsmBinaryOp binary_op_out_of_range_sq_add = AsmAdd()
    instructions.append(AsmCmp(assembly_type_si, lower_bound_si, src))
    instructions.append(AsmJmpCC(cond_code_l, target_out_of_range))
    instructions.append(AsmCvtsi2sd(assembly_type_si, src, dst))
    instructions.append(AsmJmp(target_after))
    instructions.append(AsmLabel(target_out_of_range))
    instructions.append(AsmMov(assembly_type_si, src, dst_out_of_range_si))
    instructions.append(AsmMov(assembly_type_si, dst_out_of_range_si, dst_out_of_range_si_shr))
    instructions.append(AsmUnary(unary_op_out_of_range_si_shr, assembly_type_si, dst_out_of_range_si_shr))
    instructions.append(AsmBinary(binary_op_out_of_range_si_and, assembly_type_si, set_bit_si, dst_out_of_range_si))
    instructions.append(AsmBinary(binary_op_out_of_range_si_or, assembly_type_si, dst_out_of_range_si, dst_out_of_range_si_shr))
    instructions.append(AsmCvtsi2sd(assembly_type_si, dst_out_of_range_si_shr, dst))
    instructions.append(AsmBinary(binary_op_out_of_range_sq_add, assembly_type_sq, dst, dst))
    instructions.append(AsmLabel(target_after))
*/

/** TODO
cdef void generate_unsigned_to_double_instructions(TacUIntToDouble node):
    if is_value_32_bits(node.src):
        generate_uint_unsigned_to_double_instructions(node)
    else:
        generate_ulong_unsigned_to_double_instructions(node)
*/

/** TODO
cdef void generate_label_instructions(TacLabel node):
    cdef TIdentifier name = copy_identifier(node.name)
    instructions.append(AsmLabel(name))
*/

/** TODO
cdef void generate_jump_instructions(TacJump node):
    cdef TIdentifier target = copy_identifier(node.target)
    instructions.append(AsmJmp(target))
*/

/** TODO
cdef void generate_return_integer_instructions(TacReturn node):
    cdef AsmOperand src = generate_operand(node.val)
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Ax'))
    cdef AssemblyType assembly_type_val = generate_assembly_type(node.val)
    instructions.append(AsmMov(assembly_type_val, src, dst))
    instructions.append(AsmRet())
*/

/** TODO
cdef void generate_return_double_instructions(TacReturn node):
    cdef AsmOperand src = generate_operand(node.val)
    cdef AsmOperand dst = generate_register(REGISTER_KIND.get('Xmm0'))
    cdef AssemblyType assembly_type_val = BackendDouble()
    instructions.append(AsmMov(assembly_type_val, src, dst))
    instructions.append(AsmRet())
*/

/** TODO
cdef void generate_return_instructions(TacReturn node):
    if is_value_double(node.val):
        generate_return_double_instructions(node)
    else:
        generate_return_integer_instructions(node)
*/

/** TODO
cdef void generate_copy_instructions(TacCopy node):
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src = generate_assembly_type(node.src)
    instructions.append(AsmMov(assembly_type_src, src, dst))
*/

/** TODO
cdef void generate_jump_if_zero_integer_instructions(TacJumpIfZero node):
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmCondCode cond_code_e = AsmE()
    cdef TIdentifier target = copy_identifier(node.target)
    cdef AsmOperand condition = generate_operand(node.condition)
    cdef AssemblyType assembly_type_cond = generate_assembly_type(node.condition)
    instructions.append(AsmCmp(assembly_type_cond, imm_zero, condition))
    instructions.append(AsmJmpCC(cond_code_e, target))
*/

/** TODO
cdef void generate_jump_if_zero_double_instructions(TacJumpIfZero node):
    cdef AsmOperand reg_zero = generate_register(REGISTER_KIND.get('Xmm0'))
    cdef AsmCondCode cond_code_e = AsmE()
    cdef TIdentifier target = copy_identifier(node.target)
    cdef AsmOperand condition = generate_operand(node.condition)
    cdef AssemblyType assembly_type_cond = BackendDouble()
    generate_zero_out_xmm_reg_instructions()
    instructions.append(AsmCmp(assembly_type_cond, condition, reg_zero))
    instructions.append(AsmJmpCC(cond_code_e, target))
*/

/** TODO
cdef void generate_jump_if_zero_instructions(TacJumpIfZero node):
    if is_value_double(node.condition):
        generate_jump_if_zero_double_instructions(node)
    else:
        generate_jump_if_zero_integer_instructions(node)
*/

/** TODO
cdef void generate_jump_if_not_zero_integer_instructions(TacJumpIfNotZero node):
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmCondCode cond_code_ne = AsmNE()
    cdef TIdentifier target = copy_identifier(node.target)
    cdef AsmOperand condition = generate_operand(node.condition)
    cdef AssemblyType assembly_type_cond = generate_assembly_type(node.condition)
    instructions.append(AsmCmp(assembly_type_cond, imm_zero, condition))
    instructions.append(AsmJmpCC(cond_code_ne, target))
*/

/** TODO
cdef void generate_jump_if_not_zero_double_instructions(TacJumpIfNotZero node):
    cdef AsmOperand reg_zero = generate_register(REGISTER_KIND.get('Xmm0'))
    cdef AsmCondCode cond_code_ne = AsmNE()
    cdef TIdentifier target = copy_identifier(node.target)
    cdef AsmOperand condition = generate_operand(node.condition)
    cdef AssemblyType assembly_type_cond = BackendDouble()
    generate_zero_out_xmm_reg_instructions()
    instructions.append(AsmCmp(assembly_type_cond, condition, reg_zero))
    instructions.append(AsmJmpCC(cond_code_ne, target))
*/

/** TODO
cdef void generate_jump_if_not_zero_instructions(TacJumpIfNotZero node):
    if is_value_double(node.condition):
        generate_jump_if_not_zero_double_instructions(node)
    else:
        generate_jump_if_not_zero_integer_instructions(node)
*/

/** TODO
cdef void generate_unary_operator_conditional_integer_instructions(TacUnary node):
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmCondCode cond_code_e = AsmE()
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand cmp_dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src = generate_assembly_type(node.src)
    cdef AssemblyType assembly_type_dst = generate_assembly_type(node.dst)
    instructions.append(AsmCmp(assembly_type_src, imm_zero, src))
    instructions.append(AsmMov(assembly_type_dst, imm_zero, cmp_dst))
    instructions.append(AsmSetCC(cond_code_e, cmp_dst))
*/

/** TODO
cdef void generate_unary_operator_conditional_double_instructions(TacUnary node):
    cdef AsmOperand reg_zero = generate_register(REGISTER_KIND.get('Xmm0'))
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmCondCode cond_code_e = AsmE()
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand cmp_dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src = BackendDouble()
    cdef AssemblyType assembly_type_dst = LongWord()
    generate_zero_out_xmm_reg_instructions()
    instructions.append(AsmCmp(assembly_type_src, reg_zero, src))
    instructions.append(AsmMov(assembly_type_dst, imm_zero, cmp_dst))
    instructions.append(AsmSetCC(cond_code_e, cmp_dst))
*/

/** TODO
cdef void generate_unary_operator_conditional_instructions(TacUnary node):
    if is_value_double(node.src):
        generate_unary_operator_conditional_double_instructions(node)
    else:
        generate_unary_operator_conditional_integer_instructions(node)
*/

/** TODO
cdef void generate_unary_operator_arithmetic_integer_instructions(TacUnary node):
    cdef AsmUnaryOp unary_op = generate_unary_op(node.unary_op)
    cdef AsmOperand src = generate_operand(node.src)
    cdef AsmOperand src_dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src = generate_assembly_type(node.src)
    instructions.append(AsmMov(assembly_type_src, src, src_dst))
    instructions.append(AsmUnary(unary_op, assembly_type_src, src_dst))
*/

/** TODO
cdef void generate_unary_operator_arithmetic_double_negate_instructions(TacUnary node):
    cdef AsmBinaryOp binary_op = AsmBitXor()
    cdef AsmOperand src1 = generate_operand(node.src)
    cdef AsmOperand src2 = generate_double_static_constant_operand(-0.0, 16)
    cdef AsmOperand src1_dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src1 = BackendDouble()
    instructions.append(AsmMov(assembly_type_src1, src1, src1_dst))
    instructions.append(AsmBinary(binary_op, assembly_type_src1, src2, src1_dst))
*/

/** TODO
cdef void generate_unary_operator_arithmetic_negate_instructions(TacUnary node):
    if is_value_double(node.src):
        generate_unary_operator_arithmetic_double_negate_instructions(node)
    else:
        generate_unary_operator_arithmetic_integer_instructions(node)
*/

/** TODO
cdef void generate_unary_instructions(TacUnary node):
    if isinstance(node.unary_op, TacNot):
        generate_unary_operator_conditional_instructions(node)
    elif isinstance(node.unary_op, TacComplement):
        generate_unary_operator_arithmetic_integer_instructions(node)
    elif isinstance(node.unary_op, TacNegate):
        generate_unary_operator_arithmetic_negate_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef void generate_binary_operator_conditional_integer_instructions(TacBinary node):
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmCondCode cond_code
    if is_value_signed(node.src1):
        cond_code = generate_signed_condition_code(node.binary_op)
    else:
        cond_code = generate_unsigned_condition_code(node.binary_op)
    cdef AsmOperand src1 = generate_operand(node.src1)
    cdef AsmOperand src2 = generate_operand(node.src2)
    cdef AsmOperand cmp_dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src1 = generate_assembly_type(node.src1)
    cdef AssemblyType assembly_type_dst = generate_assembly_type(node.dst)
    instructions.append(AsmCmp(assembly_type_src1, src2, src1))
    instructions.append(AsmMov(assembly_type_dst, imm_zero, cmp_dst))
    instructions.append(AsmSetCC(cond_code, cmp_dst))
*/

/** TODO
cdef void generate_binary_operator_conditional_double_instructions(TacBinary node):
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmCondCode cond_code = generate_unsigned_condition_code(node.binary_op)
    cdef AsmCondCode cond_code_p = AsmP()
    cdef TIdentifier target_nan = represent_label_identifier("comisd_nan")
    cdef AsmOperand src1 = generate_operand(node.src1)
    cdef AsmOperand src2 = generate_operand(node.src2)
    cdef AsmOperand cmp_dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src1 = generate_assembly_type(node.src1)
    cdef AssemblyType assembly_type_dst = LongWord()
    instructions.append(AsmCmp(assembly_type_src1, src2, src1))
    instructions.append(AsmMov(assembly_type_dst, imm_zero, cmp_dst))
    instructions.append(AsmJmpCC(cond_code_p, target_nan))
    instructions.append(AsmSetCC(cond_code, cmp_dst))
    instructions.append(AsmLabel(target_nan))
*/

/** TODO
cdef void generate_binary_operator_conditional_instructions(TacBinary node):
    if is_value_double(node.src1):
        generate_binary_operator_conditional_double_instructions(node)
    else:
        generate_binary_operator_conditional_integer_instructions(node)
*/

/** TODO
cdef void generate_binary_operator_arithmetic_instructions(TacBinary node):
    cdef AsmBinaryOp binary_op = generate_binary_op(node.binary_op)
    cdef AsmOperand src1 = generate_operand(node.src1)
    cdef AsmOperand src2 = generate_operand(node.src2)
    cdef AsmOperand src1_dst = generate_operand(node.dst)
    cdef AssemblyType assembly_type_src1 = generate_assembly_type(node.src1)
    instructions.append(AsmMov(assembly_type_src1, src1, src1_dst))
    instructions.append(AsmBinary(binary_op, assembly_type_src1, src2, src1_dst))
*/

/** TODO
cdef void generate_binary_operator_arithmetic_signed_divide_instructions(TacBinary node):
    cdef AsmOperand src1 = generate_operand(node.src1)
    cdef AsmOperand src2 = generate_operand(node.src2)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AsmOperand src1_dst = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand dst_src = generate_register(REGISTER_KIND.get('Ax'))
    cdef AssemblyType assembly_type_src1 = generate_assembly_type(node.src1)
    instructions.append(AsmMov(assembly_type_src1, src1, src1_dst))
    instructions.append(AsmCdq(assembly_type_src1))
    instructions.append(AsmIdiv(assembly_type_src1, src2))
    instructions.append(AsmMov(assembly_type_src1, dst_src, dst))
*/

/** TODO
cdef void generate_binary_operator_arithmetic_unsigned_divide_instructions(TacBinary node):
    cdef AsmOperand src1 = generate_operand(node.src1)
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmOperand src2 = generate_operand(node.src2)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AsmOperand src1_dst = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand imm_zero_dst = generate_register(REGISTER_KIND.get('Dx'))
    cdef AsmOperand dst_src = generate_register(REGISTER_KIND.get('Ax'))
    cdef AssemblyType assembly_type_src1 = generate_assembly_type(node.src1)
    instructions.append(AsmMov(assembly_type_src1, src1, src1_dst))
    instructions.append(AsmMov(assembly_type_src1, imm_zero, imm_zero_dst))
    instructions.append(AsmDiv(assembly_type_src1, src2))
    instructions.append(AsmMov(assembly_type_src1, dst_src, dst))
*/

/** TODO
cdef void generate_binary_operator_arithmetic_divide_instructions(TacBinary node):
    if is_value_double(node.src1):
        generate_binary_operator_arithmetic_instructions(node)
    elif is_value_signed(node.src1):
        generate_binary_operator_arithmetic_signed_divide_instructions(node)
    else:
        generate_binary_operator_arithmetic_unsigned_divide_instructions(node)
*/

/** TODO
cdef void generate_binary_operator_arithmetic_signed_remainder_instructions(TacBinary node):
    cdef AsmOperand src1 = generate_operand(node.src1)
    cdef AsmOperand src2 = generate_operand(node.src2)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AsmOperand src1_dst = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand dst_src = generate_register(REGISTER_KIND.get('Dx'))
    cdef AssemblyType assembly_type_src1 = generate_assembly_type(node.src1)
    instructions.append(AsmMov(assembly_type_src1, src1, src1_dst))
    instructions.append(AsmCdq(assembly_type_src1))
    instructions.append(AsmIdiv(assembly_type_src1, src2))
    instructions.append(AsmMov(assembly_type_src1, dst_src, dst))
*/

/** TODO
cdef void generate_binary_operator_arithmetic_unsigned_remainder_instructions(TacBinary node):
    cdef AsmOperand src1 = generate_operand(node.src1)
    cdef AsmOperand imm_zero = AsmImm(TIdentifier("0"), False)
    cdef AsmOperand src2 = generate_operand(node.src2)
    cdef AsmOperand dst = generate_operand(node.dst)
    cdef AsmOperand src1_dst = generate_register(REGISTER_KIND.get('Ax'))
    cdef AsmOperand imm_zero_dst = generate_register(REGISTER_KIND.get('Dx'))
    cdef AsmOperand dst_src = generate_register(REGISTER_KIND.get('Dx'))
    cdef AssemblyType assembly_type_src1 = generate_assembly_type(node.src1)
    instructions.append(AsmMov(assembly_type_src1, src1, src1_dst))
    instructions.append(AsmMov(assembly_type_src1, imm_zero, imm_zero_dst))
    instructions.append(AsmDiv(assembly_type_src1, src2))
    instructions.append(AsmMov(assembly_type_src1, dst_src, dst))
*/

/** TODO
cdef void generate_binary_operator_arithmetic_remainder_instructions(TacBinary node):
    if is_value_signed(node.src1):
        generate_binary_operator_arithmetic_signed_remainder_instructions(node)
    else:
        generate_binary_operator_arithmetic_unsigned_remainder_instructions(node)
*/

/** TODO
cdef void generate_binary_instructions(TacBinary node):
    if isinstance(node.binary_op, (TacEqual, TacNotEqual, TacLessThan, TacLessOrEqual, TacGreaterThan,
                                   TacGreaterOrEqual)):
        generate_binary_operator_conditional_instructions(node)
    elif isinstance(node.binary_op, (TacAdd, TacSubtract, TacMultiply, TacBitAnd, TacBitOr, TacBitXor,
                                     TacBitShiftLeft, TacBitShiftRight)):
        generate_binary_operator_arithmetic_instructions(node)
    elif isinstance(node.binary_op, TacDivide):
        generate_binary_operator_arithmetic_divide_instructions(node)
    elif isinstance(node.binary_op, TacRemainder):
        generate_binary_operator_arithmetic_remainder_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef void generate_instructions(TacInstruction node):
    if isinstance(node, TacFunCall):
        generate_fun_call_instructions(node)
    elif isinstance(node, TacSignExtend):
        generate_sign_extend_instructions(node)
    elif isinstance(node, TacZeroExtend):
        generate_zero_extend_instructions(node)
    elif isinstance(node, TacTruncate):
        generate_truncate_instructions(node)
    elif isinstance(node, TacDoubleToInt):
        generate_double_to_signed_instructions(node)
    elif isinstance(node, TacDoubleToUInt):
        generate_double_to_unsigned_instructions(node)
    elif isinstance(node, TacIntToDouble):
        generate_signed_to_double_instructions(node)
    elif isinstance(node, TacUIntToDouble):
        generate_unsigned_to_double_instructions(node)
    elif isinstance(node, TacLabel):
        generate_label_instructions(node)
    elif isinstance(node, TacJump):
        generate_jump_instructions(node)
    elif isinstance(node, TacReturn):
        generate_return_instructions(node)
    elif isinstance(node, TacCopy):
        generate_copy_instructions(node)
    elif isinstance(node, TacJumpIfZero):
        generate_jump_if_zero_instructions(node)
    elif isinstance(node, TacJumpIfNotZero):
        generate_jump_if_not_zero_instructions(node)
    elif isinstance(node, TacUnary):
        generate_unary_instructions(node)
    elif isinstance(node, TacBinary):
        generate_binary_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef void generate_list_instructions(list[TacInstruction] list_node):
    # instruction = Mov(assembly_type, operand src, operand dst) | MovSx(operand src, operand dst)
    #             | MovZeroExtend(operand src, operand dst) | Cvttsd2si(assembly_type, operand, operand)
    #             | Cvtsi2sd(assembly_type, operand, operand) | Unary(unary_operator, assembly_type, operand)
    #             | Binary(binary_operator, assembly_type, operand, operand) | Cmp(assembly_type, operand, operand)
    #             | Idiv(assembly_type, operand) | Div(assembly_type, operand) | Cdq(assembly_type)
    #             | Jmp(identifier) | JmpCC(cond_code, identifier) | SetCC(cond_code, operand) | Label(identifier)
    #             | AllocateStack(int) | DeallocateStack(int) | Push(operand) | Call(identifier) | Ret
    cdef Py_ssize_t instruction
    for instruction in range(len(list_node)):
        generate_instructions(list_node[instruction])
*/

/** TODO
cdef void generate_reg_param_function_instructions(TIdentifier node, str arg_register):
    cdef AsmOperand src = generate_register(REGISTER_KIND.get(arg_register))
    cdef TIdentifier name = copy_identifier(node)
    cdef AsmOperand dst = AsmPseudo(name)
    cdef AssemblyType assembly_type_param = convert_backend_assembly_type(node.str_t)
    instructions.append(AsmMov(assembly_type_param, src, dst))
*/

/** TODO
cdef void generate_stack_param_function_instructions(TIdentifier node, int32 byte):
    cdef AsmOperand src = AsmStack(TInt((byte + 2) * 8))
    cdef TIdentifier name = copy_identifier(node)
    cdef AsmOperand dst = AsmPseudo(name)
    cdef AssemblyType assembly_type_param = convert_backend_assembly_type(node.str_t)
    instructions.append(AsmMov(assembly_type_param, src, dst))
*/

/** TODO
cdef AsmFunction generate_function_top_level(TacFunction node):
    global instructions

    cdef TIdentifier name = copy_identifier(node.name)
    cdef bint is_global = node.is_global

    cdef list[TacInstruction] body = []
    instructions = body
    cdef Py_ssize_t param
    cdef Py_ssize_t param_reg = 0
    cdef Py_ssize_t param_sse_reg = 0
    cdef int32 param_stack = 0
    for param in range(len(node.params)):
        if isinstance(symbol_table[node.params[param].str_t].type_t, Double):
            if param_sse_reg < 8:
                generate_reg_param_function_instructions(node.params[param], arg_sse_registers[param_sse_reg])
                param_sse_reg += 1
            else:
                generate_stack_param_function_instructions(node.params[param], param_stack)
                param_stack += 1
        else:
            if param_reg < 6:
                generate_reg_param_function_instructions(node.params[param], arg_registers[param_reg])
                param_reg += 1
            else:
                generate_stack_param_function_instructions(node.params[param], param_stack)
                param_stack += 1
    generate_list_instructions(node.body)
    return AsmFunction(name, is_global, body)
*/

/** TODO
cdef AsmStaticVariable generate_static_variable_top_level(TacStaticVariable node):
    cdef TIdentifier name = copy_identifier(node.name)
    cdef bint is_global = node.is_global
    cdef TInt alignment = generate_alignment(node.static_init_type)
    cdef StaticInit initial_value = node.initial_value
    return AsmStaticVariable(name, is_global, alignment, initial_value)
*/

/** TODO
cdef list[AsmTopLevel] p_static_constant_top_levels = []*/

/** TODO
cdef void append_double_static_constant_top_level(TIdentifier name, double value, int32 byte):
    cdef TInt alignment = TInt(byte)
    cdef StaticInit initial_value = DoubleInit(TDouble(value))
    p_static_constant_top_levels.append(AsmStaticConstant(name, alignment, initial_value))
*/
static void append_double_static_constant_top_level(TIdentifier /*name*/, double /*value*/, TInt /*byte*/) {
    ; // TODO for forward decl only
}

/** TODO
cdef AsmTopLevel generate_top_level(TacTopLevel node):
    # top_level = Function(identifier name, bool global, instruction* instructions)
    #           | StaticVariable(identifier, bool global, int alignment, static_init initial_value)
    #           | AsmStaticConstant(identifier, int alignment, static_init initial_value)
    if isinstance(node, TacFunction):
        return generate_function_top_level(node)
    elif isinstance(node, TacStaticVariable):
        return generate_static_variable_top_level(node)
    else:

        raise RuntimeError(
            "An error occurred in assembly generation, not all nodes were visited")
*/

/** TODO
cdef AsmProgram generate_program(TacProgram node):
    # program = Program(function_definition)
    global p_static_constant_top_levels
    static_const_label_map.clear()

    cdef list[TacTopLevel] static_constant_top_levels = []
    p_static_constant_top_levels = static_constant_top_levels

    cdef Py_ssize_t top_level
    cdef list[AsmTopLevel] top_levels = []
    for top_level in range(len(node.static_variable_top_levels)):
        top_levels.append(generate_top_level(node.static_variable_top_levels[top_level]))
    for top_level in range(len(node.function_top_levels)):
        top_levels.append(generate_top_level(node.function_top_levels[top_level]))

    return AsmProgram(static_constant_top_levels, top_levels)
*/

/** TODO
cdef AsmProgram assembly_generation(TacProgram tac_ast):

    cdef AsmProgram asm_ast = generate_program(tac_ast)

    if not asm_ast:
        raise RuntimeError(
            "An error occurred in assembly generation, ASM was not generated")

    convert_symbol_table(asm_ast)

    fix_stack(asm_ast)

    return asm_ast
*/