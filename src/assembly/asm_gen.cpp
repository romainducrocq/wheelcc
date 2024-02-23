#include "assembly/asm_gen.hpp"
#include "util/error.hpp"
#include "util/ctypes.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/backend_st.hpp"
#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"
#include "ast/asm_ast.hpp"
#include "intermediate/names.hpp"
#include "assembly/registers.hpp"
#include "assembly/st_conv.hpp"
#include "assembly/stack_fix.hpp"

#include <string>
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>

static std::unordered_map<TIdentifier, TIdentifier> static_const_label_map;

static TInt generate_alignment(Type* node) {
    switch(node->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return 4;
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::ULong_t:
        case AST_T::Pointer_t:
            return 8;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AsmImm> generate_int_imm_operand(CConstInt* node) {
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(false, std::move(value));
}

static std::shared_ptr<AsmImm> generate_long_imm_operand(CConstLong* node) {
    bool is_quad = node->value > 2147483647l;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
}

static std::shared_ptr<AsmImm> generate_uint_imm_operand(CConstUInt* node) {
    bool is_quad = node->value > 2147483647u;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
}

static std::shared_ptr<AsmImm> generate_ulong_imm_operand(CConstULong* node) {
    bool is_quad = node->value > 2147483647ul;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_quad), std::move(value));
}

static void append_double_static_constant_top_level(const TIdentifier& identifier, TDouble value, TULong binary,
                                                    TInt byte);

static std::shared_ptr<AsmData> generate_double_static_constant_operand(TDouble value, TULong binary, TInt byte) {
    TIdentifier static_constant_label;
    TIdentifier str_binary = std::to_string(binary);
    if(static_const_label_map.find(str_binary) != static_const_label_map.end()) {
        static_constant_label = static_const_label_map[str_binary];
    }
    else {
        static_constant_label = represent_label_identifier("double");
        static_const_label_map[str_binary] = static_constant_label;
        append_double_static_constant_top_level(static_constant_label, value, binary, byte);
    }
    return std::make_shared<AsmData>(std::move(static_constant_label));
}

static std::shared_ptr<AsmData> generate_double_constant_operand(CConstDouble* node) {
    return generate_double_static_constant_operand(node->value, double_to_binary(node->value), 8);
}

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

static std::shared_ptr<AsmPseudo> generate_pseudo_operand(TacVariable* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmPseudo>(std::move(name));
}

// operand = Imm(int, bool) | Reg(reg) | Pseudo(identifier) | Memory(int, reg) | Data(identifier)
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

// (signed) cond_code = E | NE | L | LE | G | GE
static std::unique_ptr<AsmCondCode> generate_signed_condition_code(TacBinaryOp* node) {
    switch(node->type()) {
        case AST_T::TacEqual_t:
            return std::make_unique<AsmE>();
        case AST_T::TacNotEqual_t:
            return std::make_unique<AsmNE>();
        case AST_T::TacLessThan_t:
            return std::make_unique<AsmL>();
        case AST_T::TacLessOrEqual_t:
            return std::make_unique<AsmLE>();
        case AST_T::TacGreaterThan_t:
            return std::make_unique<AsmG>();
        case AST_T::TacGreaterOrEqual_t:
            return std::make_unique<AsmGE>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// (unsigned) cond_code = E | NE | B | BE | A | AE
static std::unique_ptr<AsmCondCode> generate_unsigned_condition_code(TacBinaryOp* node) {
    switch(node->type()) {
        case AST_T::TacEqual_t:
            return std::make_unique<AsmE>();
        case AST_T::TacNotEqual_t:
            return std::make_unique<AsmNE>();
        case AST_T::TacLessThan_t:
            return std::make_unique<AsmB>();
        case AST_T::TacLessOrEqual_t:
            return std::make_unique<AsmBE>();
        case AST_T::TacGreaterThan_t:
            return std::make_unique<AsmA>();
        case AST_T::TacGreaterOrEqual_t:
            return std::make_unique<AsmAE>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// unary_operator = Not | Neg | Shr
static std::unique_ptr<AsmUnaryOp> generate_unary_op(TacUnaryOp* node) {
    switch(node->type()) {
        case AST_T::TacComplement_t:
            return std::make_unique<AsmNot>();
        case AST_T::TacNegate_t:
            return std::make_unique<AsmNeg>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// binary_operator = Add | Sub | Mult | DivDouble | BitAnd | BitOr | BitXor | BitShiftLeft | BitShiftRight
static std::unique_ptr<AsmBinaryOp> generate_binary_op(TacBinaryOp* node) {
    switch(node->type()) {
        case AST_T::TacAdd_t:
            return std::make_unique<AsmAdd>();
        case AST_T::TacSubtract_t:
            return std::make_unique<AsmSub>();
        case AST_T::TacMultiply_t:
            return std::make_unique<AsmMult>();
        case AST_T::TacDivide_t:
            return std::make_unique<AsmDivDouble>();
        case AST_T::TacBitAnd_t:
            return std::make_unique<AsmBitAnd>();
        case AST_T::TacBitOr_t:
            return std::make_unique<AsmBitOr>();
        case AST_T::TacBitXor_t:
            return std::make_unique<AsmBitXor>();
        case AST_T::TacBitShiftLeft_t:
            return std::make_unique<AsmBitShiftLeft>();
        case AST_T::TacBitShiftRight_t:
            return std::make_unique<AsmBitShiftRight>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_constant_value_signed(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstInt_t:
        case AST_T::CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_variable_value_signed(TacVariable* node) {
    switch(symbol_table[node->name]->type_t->type()) {
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::Double_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_signed(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return is_constant_value_signed(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_variable_value_signed(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_constant_value_32_bits(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstInt_t:
        case AST_T::CConstUInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_variable_value_32_bits(TacVariable* node) {
    switch(symbol_table[node->name]->type_t->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_32_bits(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return is_constant_value_32_bits(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_variable_value_32_bits(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_constant_value_double(TacConstant* node) {
    return node->constant->type() == AST_T::CConstDouble_t;
}

static bool is_variable_value_double(TacVariable* node) {
    return symbol_table[node->name]->type_t->type() == AST_T::Double_t;
}

static bool is_value_double(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return is_constant_value_double(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_variable_value_double(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AssemblyType> generate_constant_assembly_type(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstInt_t:
        case AST_T::CConstUInt_t:
            return std::make_shared<LongWord>();
        case AST_T::CConstDouble_t:
            return std::make_shared<BackendDouble>();
        case AST_T::CConstLong_t:
        case AST_T::CConstULong_t:
            return std::make_shared<QuadWord>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AssemblyType> generate_variable_assembly_type(TacVariable* node) {
    return convert_backend_assembly_type(node->name);
}

static std::shared_ptr<AssemblyType> generate_assembly_type(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return generate_constant_assembly_type(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return generate_variable_assembly_type(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::vector<std::unique_ptr<AsmInstruction>>* p_instructions;

static void push_instruction(std::unique_ptr<AsmInstruction>&& instruction) {
    p_instructions->push_back(std::move(instruction));
}

static void generate_return_integer_instructions(TacReturn* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->val.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_val = generate_assembly_type(node->val.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_val), std::move(src),
                                                            std::move(dst)));
    }
    push_instruction(std::make_unique<AsmRet>());
}

static void generate_return_double_instructions(TacReturn* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->val.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm0);
        std::shared_ptr<AssemblyType> assembly_type_val = std::make_shared<BackendDouble>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_val), std::move(src),
                                                            std::move(dst)));
    }
    push_instruction(std::make_unique<AsmRet>());
}

static void generate_return_instructions(TacReturn* node) {
    if(is_value_double(node->val.get())) {
        generate_return_double_instructions(node);
    }
    else {
        generate_return_integer_instructions(node);
    }
}

static void generate_sign_extend_instructions(TacSignExtend* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    push_instruction(std::make_unique<AsmMovSx>(std::move(src), std::move(dst)));
}

static void generate_imm_truncate_instructions(AsmImm* node) {
    if(node->is_quad) {
        TIdentifier value = std::to_string(uintmax_to_uint64(
                                        string_to_uintmax(std::move(node->value), 0)) - 4294967296ul);
        node->value = std::move(value);
    }
}

static void generate_truncate_instructions(TacTruncate* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<LongWord>();
    if(src->type() == AST_T::AsmImm_t) {
        generate_imm_truncate_instructions(static_cast<AsmImm*>(src.get()));
    }
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                        std::move(dst)));
}

static void generate_zero_extend_instructions(TacZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    push_instruction(std::make_unique<AsmMovZeroExtend>(std::move(src), std::move(dst)));
}

static void generate_double_to_signed_instructions(TacDoubleToInt* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->dst.get());
    push_instruction(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src), std::move(src),
                                                              std::move(dst)));
}

static void generate_uint_double_to_unsigned_instructions(TacDoubleToUInt* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src),
                                                                  std::move(src),src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst),
                                                            std::move(src_dst), std::move(dst)));
    }
}

static void generate_ulong_double_to_unsigned_instructions(TacDoubleToUInt* node) {
    TIdentifier target_out_of_range = represent_label_identifier("sd2si_out_of_range");
    TIdentifier target_after = represent_label_identifier("sd2si_after");
    std::shared_ptr<AsmOperand> upper_bound_sd = generate_double_static_constant_operand(9223372036854775808.0,
                                                                                         4890909195324358656ul,
                                                                                         8);
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AsmOperand> dst_out_of_range_sd = generate_register(REGISTER_KIND::Xmm1);
    std::shared_ptr<AsmOperand> src_out_of_range_si = generate_register(REGISTER_KIND::Dx);
    std::shared_ptr<AssemblyType> assembly_type_sd = std::make_shared<BackendDouble>();
    std::shared_ptr<AssemblyType> assembly_type_si = std::make_shared<QuadWord>();
    push_instruction(std::make_unique<AsmCmp>(assembly_type_sd, upper_bound_sd, src));
    {
        std::unique_ptr<AsmCondCode> cond_code_ae = std::make_unique<AsmAE>();
        push_instruction(std::make_unique<AsmJmpCC>(target_out_of_range,
                                                              std::move(cond_code_ae)));
    }
    push_instruction(std::make_unique<AsmCvttsd2si>(assembly_type_si, src, dst));
    push_instruction(std::make_unique<AsmJmp>(target_after));
    push_instruction(std::make_unique<AsmLabel>(std::move(target_out_of_range)));
    push_instruction(std::make_unique<AsmMov>(assembly_type_sd, std::move(src),
                                                        dst_out_of_range_sd));
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_sd_sub = std::make_unique<AsmSub>();
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_sd_sub),
                                                               std::move(assembly_type_sd),
                                                               std::move(upper_bound_sd), dst_out_of_range_sd));
    }
    push_instruction(std::make_unique<AsmCvttsd2si>(assembly_type_si,
                                                              std::move(dst_out_of_range_sd), dst));
    {
        std::shared_ptr<AsmOperand> upper_bound_si = std::make_shared<AsmImm>(true, "9223372036854775808");
        push_instruction(std::make_unique<AsmMov>(assembly_type_si, std::move(upper_bound_si),
                                                            src_out_of_range_si));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_si_add = std::make_unique<AsmAdd>();
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_si_add),
                                                               std::move(assembly_type_si),
                                                               std::move(src_out_of_range_si), std::move(dst)));
    }
    push_instruction(std::make_unique<AsmLabel>(std::move(target_after)));
}

static void generate_double_to_unsigned_instructions(TacDoubleToUInt* node) {
    if(is_value_32_bits(node->dst.get())) {
        generate_uint_double_to_unsigned_instructions(node);
    }
    else {
        generate_ulong_double_to_unsigned_instructions(node);
    }
}

static void generate_signed_to_double_instructions(TacIntToDouble* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    push_instruction(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_src), std::move(src),
                                                             std::move(dst)));
}

static void generate_uint_unsigned_to_double_instructions(TacUIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        push_instruction(std::make_unique<AsmMovZeroExtend>(std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_dst),
                                                                 std::move(src_dst), std::move(dst)));
    }
}

static void generate_ulong_unsigned_to_double_instructions(TacUIntToDouble* node) {
    TIdentifier target_out_of_range = represent_label_identifier("si2sd_out_of_range");
    TIdentifier target_after = represent_label_identifier("si2sd_after");
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AsmOperand> dst_out_of_range_si = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AsmOperand> dst_out_of_range_si_shr = generate_register(REGISTER_KIND::Dx);
    std::shared_ptr<AssemblyType> assembly_type_si = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> lower_bound_si = std::make_shared<AsmImm>(false, "0");
        push_instruction(std::make_unique<AsmCmp>(assembly_type_si, std::move(lower_bound_si),
                                                            src));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_l = std::make_unique<AsmL>();
        push_instruction(std::make_unique<AsmJmpCC>(target_out_of_range,
                                                              std::move(cond_code_l)));
    }
    push_instruction(std::make_unique<AsmCvtsi2sd>(assembly_type_si, src, dst));
    push_instruction(std::make_unique<AsmJmp>(target_after));
    push_instruction(std::make_unique<AsmLabel>(std::move(target_out_of_range)));
    push_instruction(std::make_unique<AsmMov>(assembly_type_si, std::move(src),
                                                        dst_out_of_range_si));
    push_instruction(std::make_unique<AsmMov>(assembly_type_si, dst_out_of_range_si,
                                                        dst_out_of_range_si_shr));
    {
        std::unique_ptr<AsmUnaryOp> unary_op_out_of_range_si_shr = std::make_unique<AsmShr>();
        push_instruction(std::make_unique<AsmUnary>(std::move(unary_op_out_of_range_si_shr),
                                                              assembly_type_si,
                                                              dst_out_of_range_si_shr));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_si_and = std::make_unique<AsmBitAnd>();
        std::shared_ptr<AsmOperand> set_bit_si = std::make_shared<AsmImm>(false, "1");
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_si_and),
                                                               assembly_type_si, std::move(set_bit_si),
                                                               dst_out_of_range_si));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_si_or = std::make_unique<AsmBitOr>();
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_si_or),
                                                               assembly_type_si,
                                                               std::move(dst_out_of_range_si),
                                                               dst_out_of_range_si_shr));
    }
    push_instruction(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_si),
                                                             std::move(dst_out_of_range_si_shr), dst));
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_sq_add = std::make_unique<AsmAdd>();
        std::shared_ptr<AssemblyType> assembly_type_sq = std::make_shared<BackendDouble>();
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_sq_add),
                                                               std::move(assembly_type_sq), dst, dst));
    }
    push_instruction(std::make_unique<AsmLabel>(std::move(target_after)));
}

static void generate_unsigned_to_double_instructions(TacUIntToDouble* node) {
    if(is_value_32_bits(node->src.get())) {
        generate_uint_unsigned_to_double_instructions(node);
    }
    else {
        generate_ulong_unsigned_to_double_instructions(node);
    }
}

static void generate_allocate_stack_instructions(TInt byte) {
    push_instruction(allocate_stack_bytes(byte));
}

static void generate_deallocate_stack_instructions(TInt byte) {
    push_instruction(deallocate_stack_bytes(byte));
}

static std::array<REGISTER_KIND, 6> ARG_REGISTERS = { REGISTER_KIND::Di, REGISTER_KIND::Si, REGISTER_KIND::Dx,
                                                      REGISTER_KIND::Cx, REGISTER_KIND::R8, REGISTER_KIND::R9 };

static std::array<REGISTER_KIND, 8> ARG_SSE_REGISTERS = { REGISTER_KIND::Xmm0, REGISTER_KIND::Xmm1, REGISTER_KIND::Xmm2,
                                                          REGISTER_KIND::Xmm3, REGISTER_KIND::Xmm4, REGISTER_KIND::Xmm5,
                                                          REGISTER_KIND::Xmm6, REGISTER_KIND::Xmm7 };

static void generate_reg_arg_fun_call_instructions(TacValue* node, REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = generate_operand(node);
    std::shared_ptr<AsmOperand> dst = generate_register(arg_register);
    std::shared_ptr<AssemblyType> assembly_type = generate_assembly_type(node);
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), std::move(dst)));
}

static void generate_stack_arg_fun_call_instructions(TacValue* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node);
    std::shared_ptr<AssemblyType> assembly_type = generate_assembly_type(node);

    if(src->type() == AST_T::AsmRegister_t ||
       src->type() == AST_T::AsmImm_t ||
       assembly_type->type() == AST_T::QuadWord_t ||
       assembly_type->type() == AST_T::BackendDouble_t) {
        push_instruction(std::make_unique<AsmPush>(std::move(src)));
        return;
    }

    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type), std::move(src), dst));
    push_instruction(std::make_unique<AsmPush>(std::move(dst)));
}

static void generate_fun_call_instructions(TacFunCall* node) {
    TInt stack_padding = 0;
    if(node->args.size() % 2 == 1) {
        stack_padding = 8;
        generate_allocate_stack_instructions(stack_padding);
    }

    std::vector<size_t> i_regs;
    std::vector<size_t> i_sse_regs;
    std::vector<size_t> i_stacks;
    for(size_t i = 0; i < node->args.size(); i++) {
        if(is_value_double(node->args[i].get())) {
            if(i_sse_regs.size() < 8) {
                i_sse_regs.push_back(i);
            }
            else {
                i_stacks.push_back(i);
            }
        }
        else {
            if(i_regs.size() < 6) {
                i_regs.push_back(i);
            }
            else {
                i_stacks.push_back(i);
            }
        }
    }

    for(size_t i = 0; i < i_regs.size(); i++) {
        generate_reg_arg_fun_call_instructions(node->args[i_regs[i]].get(), ARG_REGISTERS[i]);
    }
    for(size_t i = 0; i < i_sse_regs.size(); i++) {
        generate_reg_arg_fun_call_instructions(node->args[i_sse_regs[i]].get(), ARG_SSE_REGISTERS[i]);
    }
    for(size_t i = i_stacks.size(); i-- > 0;) {
        stack_padding += 8;
        generate_stack_arg_fun_call_instructions(node->args[i_stacks[i]].get());
    }

    {
        TIdentifier name = node->name;
        push_instruction(std::make_unique<AsmCall>(std::move(name)));
    }

    if(stack_padding > 0) {
        generate_deallocate_stack_instructions(stack_padding);
    }

    std::shared_ptr<AsmOperand> src;
    if(is_value_double(node->dst.get())) {
        src = generate_register(REGISTER_KIND::Xmm0);
    }
    else {
        src = generate_register(REGISTER_KIND::Ax);
    }
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                        std::move(dst)));
}

static void generate_unary_operator_conditional_integer_instructions(TacUnary* node) {
    std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
    std::shared_ptr<AsmOperand> cmp_dst = generate_operand(node->dst.get());
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_src), imm_zero,
                                                            std::move(src)));
    }
    {
        std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst),
                                                            std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instruction(std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
    }
}

static void generate_zero_out_xmm_reg_instructions() {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitXor>();
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::Xmm0);
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<BackendDouble>();
    push_instruction(std::make_unique<AsmBinary>(std::move(binary_op),
                                                          std::move(assembly_type_src), src, src));
}

static void generate_unary_operator_conditional_double_instructions(TacUnary* node) {
    std::shared_ptr<AsmOperand> cmp_dst = generate_operand(node->dst.get());
    generate_zero_out_xmm_reg_instructions();
    {
        std::shared_ptr<AsmOperand> reg_zero = generate_register(REGISTER_KIND::Xmm0);
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<BackendDouble>();
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_src),
                                                            std::move(reg_zero), std::move(src)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst),
                                                            std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instruction(std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
    }
}

static void generate_unary_operator_conditional_instructions(TacUnary* node) {
    if(is_value_double(node->src.get())) {
        generate_unary_operator_conditional_double_instructions(node);
    }
    else {
        generate_unary_operator_conditional_integer_instructions(node);
    }
}

static void generate_unary_operator_arithmetic_integer_instructions(TacUnary* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        push_instruction(std::make_unique<AsmMov>(assembly_type_src, std::move(src),
                                                            src_dst));
    }
    {
        std::unique_ptr<AsmUnaryOp> unary_op = generate_unary_op(node->unary_op.get());
        push_instruction(std::make_unique<AsmUnary>(std::move(unary_op),
                                                             std::move(assembly_type_src),
                                                             std::move(src_dst)));
    }
}

static void generate_unary_operator_arithmetic_double_negate_instructions(TacUnary* node) {
    std::shared_ptr<AsmOperand> src1_dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src1 = std::make_shared<BackendDouble>();
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src.get());
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1),
                                                            src1_dst));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitXor>();
        std::shared_ptr<AsmOperand> src2 = generate_double_static_constant_operand(-0.0,
                                                                                   9223372036854775808ul,
                                                                                   16);
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op),
                                                              std::move(assembly_type_src1),
                                                              std::move(src2), std::move(src1_dst)));
    }
}

static void generate_unary_operator_arithmetic_negate_instructions(TacUnary* node) {
    if(is_value_double(node->src.get())) {
        generate_unary_operator_arithmetic_double_negate_instructions(node);
    }
    else {
        generate_unary_operator_arithmetic_integer_instructions(node);
    }
}

static void generate_unary_instructions(TacUnary* node) {
    switch(node->unary_op->type()) {
        case AST_T::TacNot_t:
            generate_unary_operator_conditional_instructions(node);
            break;
        case AST_T::TacComplement_t:
            generate_unary_operator_arithmetic_integer_instructions(node);
            break;
        case AST_T::TacNegate_t:
            generate_unary_operator_arithmetic_negate_instructions(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void generate_binary_operator_arithmetic_instructions(TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1),
                                                            src1_dst));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op = generate_binary_op(node->binary_op.get());
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op),
                                                              std::move(assembly_type_src1),
                                                              std::move(src2), std::move(src1_dst)));
    }
}

static void generate_binary_operator_arithmetic_signed_divide_instructions(TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1),
                                                            src1_dst));
    }
    push_instruction(std::make_unique<AsmCdq>(assembly_type_src1));
    {
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        push_instruction(std::make_unique<AsmIdiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src1),
                                                            std::move(src1_dst), std::move(dst)));
    }
}

static void generate_binary_operator_arithmetic_unsigned_divide_instructions(TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1),
                                                            src1_dst));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
        std::shared_ptr<AsmOperand> imm_zero_dst = generate_register(REGISTER_KIND::Dx);
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(imm_zero),
                                                            std::move(imm_zero_dst)));
    }
    {
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        push_instruction(std::make_unique<AsmDiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src1),
                                                            std::move(src1_dst), std::move(dst)));
    }
}

static void generate_binary_operator_arithmetic_divide_instructions(TacBinary* node) {
    if(is_value_double(node->src1.get())) {
        generate_binary_operator_arithmetic_instructions(node);
    }
    else if(is_value_signed(node->src1.get())) {
        generate_binary_operator_arithmetic_signed_divide_instructions(node);
    }
    else {
        generate_binary_operator_arithmetic_unsigned_divide_instructions(node);
    }
}

static void generate_binary_operator_arithmetic_signed_remainder_instructions(TacBinary* node) {
    std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1),
                                                            std::move(src1_dst)));
    }
    push_instruction(std::make_unique<AsmCdq>(assembly_type_src1));
    {
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        push_instruction(std::make_unique<AsmIdiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst_src = generate_register(REGISTER_KIND::Dx);
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src1),
                                                            std::move(dst_src), std::move(dst)));
    }
}

static void generate_binary_operator_arithmetic_unsigned_remainder_instructions(TacBinary* node) {
    std::shared_ptr<AsmOperand> dst_src = generate_register(REGISTER_KIND::Dx);
    std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1),
                                                            std::move(src1_dst)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
        push_instruction(std::make_unique<AsmMov>(assembly_type_src1, std::move(imm_zero),
                                                            dst_src));
    }
    {
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        push_instruction(std::make_unique<AsmDiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src1),
                                                            std::move(dst_src), std::move(dst)));
    }
}

static void generate_binary_operator_arithmetic_remainder_instructions(TacBinary* node) {
    if(is_value_signed(node->src1.get())) {
        generate_binary_operator_arithmetic_signed_remainder_instructions(node);
    }
    else {
        generate_binary_operator_arithmetic_unsigned_remainder_instructions(node);
    }
}

static void generate_binary_operator_conditional_integer_instructions(TacBinary* node) {
    std::shared_ptr<AsmOperand> cmp_dst = generate_operand(node->dst.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_src1),
                                                            std::move(src2), std::move(src1)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
        std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst),
                                                            std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code;
        if(is_value_signed(node->src1.get())) {
            cond_code = generate_signed_condition_code(node->binary_op.get());
        }
        else {
            cond_code = generate_unsigned_condition_code(node->binary_op.get());
        }
        push_instruction(std::make_unique<AsmSetCC>(std::move(cond_code), std::move(cmp_dst)));
    }
}

static void generate_binary_operator_conditional_double_instructions(TacBinary* node) {
    TIdentifier target_nan = represent_label_identifier("comisd_nan");
    std::shared_ptr<AsmOperand> cmp_dst = generate_operand(node->dst.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_src1),
                                                            std::move(src2), std::move(src1)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst),
                                                            std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instruction(std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code = generate_unsigned_condition_code(node->binary_op.get());
        push_instruction(std::make_unique<AsmSetCC>(std::move(cond_code), std::move(cmp_dst)));
    }
    push_instruction(std::make_unique<AsmLabel>(std::move(target_nan)));
}

static void generate_binary_operator_conditional_instructions(TacBinary* node) {
    if(is_value_double(node->src1.get())) {
        generate_binary_operator_conditional_double_instructions(node);
    }
    else {
        generate_binary_operator_conditional_integer_instructions(node);
    }
}

static void generate_binary_instructions(TacBinary* node) {
    switch(node->binary_op->type()) {
        case AST_T::TacAdd_t:
        case AST_T::TacSubtract_t:
        case AST_T::TacMultiply_t:
        case AST_T::TacBitAnd_t:
        case AST_T::TacBitOr_t:
        case AST_T::TacBitXor_t:
        case AST_T::TacBitShiftLeft_t:
        case AST_T::TacBitShiftRight_t:
            generate_binary_operator_arithmetic_instructions(node);
            break;
        case AST_T::TacDivide_t:
            generate_binary_operator_arithmetic_divide_instructions(node);
            break;
        case AST_T::TacRemainder_t:
            generate_binary_operator_arithmetic_remainder_instructions(node);
            break;
        case AST_T::TacEqual_t:
        case AST_T::TacNotEqual_t:
        case AST_T::TacLessThan_t:
        case AST_T::TacLessOrEqual_t:
        case AST_T::TacGreaterThan_t:
        case AST_T::TacGreaterOrEqual_t:
            generate_binary_operator_conditional_instructions(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void generate_copy_instructions(TacCopy* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                        std::move(dst)));
}

static void generate_get_address_instructions(TacGetAddress* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    push_instruction(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
}

static void generate_load_instructions(TacLoad* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Ax, 0);
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_store_instructions(TacStore* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->dst_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Ax, 0);
        std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->src.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_jump_instructions(TacJump* node) {
    TIdentifier target = node->target;
    push_instruction(std::make_unique<AsmJmp>(std::move(target)));
}

static void generate_jump_if_zero_integer_instructions(TacJumpIfZero* node) {
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
        std::shared_ptr<AsmOperand> condition = generate_operand(node->condition.get());
        std::shared_ptr<AssemblyType> assembly_type_cond = generate_assembly_type(node->condition.get());
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_cond),
                                                            std::move(imm_zero), std::move(condition)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instruction(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_e)));
    }
}

static void generate_jump_if_zero_double_instructions(TacJumpIfZero* node) {
    generate_zero_out_xmm_reg_instructions();
    {
        std::shared_ptr<AsmOperand> condition = generate_operand(node->condition.get());
        std::shared_ptr<AsmOperand> reg_zero = generate_register(REGISTER_KIND::Xmm0);
        std::shared_ptr<AssemblyType> assembly_type_cond = std::make_shared<BackendDouble>();
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_cond),
                                                            std::move(condition), std::move(reg_zero)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instruction(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_e)));
    }
}

static void generate_jump_if_zero_instructions(TacJumpIfZero* node) {
    if(is_value_double(node->condition.get())) {
        generate_jump_if_zero_double_instructions(node);
    }
    else {
        generate_jump_if_zero_integer_instructions(node);
    }
}

static void generate_jump_if_not_zero_integer_instructions(TacJumpIfNotZero* node) {
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(false, "0");
        std::shared_ptr<AsmOperand> condition = generate_operand(node->condition.get());
        std::shared_ptr<AssemblyType> assembly_type_cond = generate_assembly_type(node->condition.get());
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_cond),
                                                            std::move(imm_zero), std::move(condition)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_ne = std::make_unique<AsmNE>();
        push_instruction(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_ne)));
    }
}

static void generate_jump_if_not_zero_double_instructions(TacJumpIfNotZero* node) {
    generate_zero_out_xmm_reg_instructions();
    {
        std::shared_ptr<AsmOperand> condition = generate_operand(node->condition.get());
        std::shared_ptr<AsmOperand> reg_zero = generate_register(REGISTER_KIND::Xmm0);
        std::shared_ptr<AssemblyType> assembly_type_cond = std::make_shared<BackendDouble>();
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_cond),
                                                            std::move(condition), std::move(reg_zero)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_ne = std::make_unique<AsmNE>();
        push_instruction(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_ne)));
    }
}

static void generate_jump_if_not_zero_instructions(TacJumpIfNotZero* node) {
    if(is_value_double(node->condition.get())) {
        generate_jump_if_not_zero_double_instructions(node);
    }
    else {
        generate_jump_if_not_zero_integer_instructions(node);
    }
}

static void generate_label_instructions(TacLabel* node) {
    TIdentifier name = node->name;
    push_instruction(std::make_unique<AsmLabel>(std::move(name)));
}

static void generate_instructions(TacInstruction* node) {
    switch(node->type()) {
        case AST_T::TacReturn_t:
            generate_return_instructions(static_cast<TacReturn*>(node));
            break;
        case AST_T::TacSignExtend_t:
            generate_sign_extend_instructions(static_cast<TacSignExtend*>(node));
            break;
        case AST_T::TacTruncate_t:
            generate_truncate_instructions(static_cast<TacTruncate*>(node));
            break;
        case AST_T::TacZeroExtend_t:
            generate_zero_extend_instructions(static_cast<TacZeroExtend*>(node));
            break;
        case AST_T::TacDoubleToInt_t:
            generate_double_to_signed_instructions(static_cast<TacDoubleToInt*>(node));
            break;
        case AST_T::TacDoubleToUInt_t:
            generate_double_to_unsigned_instructions(static_cast<TacDoubleToUInt*>(node));
            break;
        case AST_T::TacIntToDouble_t:
            generate_signed_to_double_instructions(static_cast<TacIntToDouble*>(node));
            break;
        case AST_T::TacUIntToDouble_t:
            generate_unsigned_to_double_instructions(static_cast<TacUIntToDouble*>(node));
            break;
        case AST_T::TacFunCall_t:
            generate_fun_call_instructions(static_cast<TacFunCall*>(node));
            break;
        case AST_T::TacUnary_t:
            generate_unary_instructions(static_cast<TacUnary*>(node));
            break;
        case AST_T::TacBinary_t:
            generate_binary_instructions(static_cast<TacBinary*>(node));
            break;
        case AST_T::TacCopy_t:
            generate_copy_instructions(static_cast<TacCopy*>(node));
            break;
        case AST_T::TacGetAddress_t:
            generate_get_address_instructions(static_cast<TacGetAddress*>(node));
            break;
        case AST_T::TacLoad_t:
            generate_load_instructions(static_cast<TacLoad*>(node));
            break;
        case AST_T::TacStore_t:
            generate_store_instructions(static_cast<TacStore*>(node));
            break;
        case AST_T::TacJump_t:
            generate_jump_instructions(static_cast<TacJump*>(node));
            break;
        case AST_T::TacJumpIfZero_t:
            generate_jump_if_zero_instructions(static_cast<TacJumpIfZero*>(node));
            break;
        case AST_T::TacJumpIfNotZero_t:
            generate_jump_if_not_zero_instructions(static_cast<TacJumpIfNotZero*>(node));
            break;
        case AST_T::TacLabel_t:
            generate_label_instructions(static_cast<TacLabel*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// instruction = Mov(assembly_type, operand, operand) | MovSx(operand, operand) | MovZeroExtend(operand, operand)
//             | Lea(operand, operand) | Cvttsd2si(assembly_type, operand, operand)
//             | Cvtsi2sd(assembly_type, operand, operand) | Unary(unary_operator, assembly_type, operand)
//             | Binary(binary_operator, assembly_type, operand, operand) | Cmp(assembly_type, operand, operand)
//             | Idiv(assembly_type, operand) | Div(assembly_type, operand) | Cdq(assembly_type) | Jmp(identifier)
//             | JmpCC(cond_code, identifier) | SetCC(cond_code, operand) | Label(identifier) | Push(operand)
//             | Call(identifier) | Ret
static void generate_list_instructions(std::vector<std::unique_ptr<TacInstruction>>& list_node) {
    for(size_t instruction = 0; instruction < list_node.size(); instruction++) {
        generate_instructions(list_node[instruction].get());
    }
}

static void generate_reg_param_function_instructions(const TIdentifier& param, REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = generate_register(arg_register);
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier name = param;
        dst = std::make_shared<AsmPseudo>(std::move(name));
    }
    std::shared_ptr<AssemblyType> assembly_type_param = convert_backend_assembly_type(param);
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_param), std::move(src),
                                                        std::move(dst)));
}

static void generate_stack_param_function_instructions(const TIdentifier& param, TInt byte) {
    std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Bp, (byte + 2) * 8);
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier name = param;
        dst = std::make_shared<AsmPseudo>(std::move(name));
    }
    std::shared_ptr<AssemblyType> assembly_type_param = convert_backend_assembly_type(param);
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_param), std::move(src),
                                                        std::move(dst)));
}

static std::unique_ptr<AsmFunction> generate_function_top_level(TacFunction* node) {
    TIdentifier name = node->name;
    bool is_global = node->is_global;

    std::vector<std::unique_ptr<AsmInstruction>> body;
    p_instructions = &body;

    size_t param_reg = 0;
    size_t param_sse_reg = 0;
    TInt param_stack = 0;
    for(size_t param = 0; param < node->params.size(); param++) {
        if(symbol_table[node->params[param]]->type_t->type() == AST_T::Double_t) {
            if(param_sse_reg < 8) {
                generate_reg_param_function_instructions(node->params[param],
                                                         ARG_SSE_REGISTERS[param_sse_reg]);
                param_sse_reg += 1;
            }
            else {
                generate_stack_param_function_instructions(node->params[param], param_stack);
                param_stack += 1;
            }
        }
        else {
            if(param_reg < 6) {
                generate_reg_param_function_instructions(node->params[param], ARG_REGISTERS[param_reg]);
                param_reg += 1;
            }
            else {
                generate_stack_param_function_instructions(node->params[param], param_stack);
                param_stack += 1;
            }
        }
    }

    generate_list_instructions(node->body);
    p_instructions = nullptr;

    return std::make_unique<AsmFunction>(std::move(name), std::move(is_global), std::move(body));
}

static std::unique_ptr<AsmStaticVariable> generate_static_variable_top_level(TacStaticVariable* node) {
    TIdentifier name = node->name;
    bool is_global = node->is_global;
    TInt alignment = generate_alignment(node->static_init_type.get());
    std::shared_ptr<StaticInit> initial_value = node->initial_value;
    return std::make_unique<AsmStaticVariable>(std::move(name), std::move(alignment), std::move(is_global),
                                               std::move(initial_value));
}

static std::vector<std::unique_ptr<AsmTopLevel>>* p_static_constant_top_levels;

static void push_static_constant_top_levels(std::unique_ptr<AsmTopLevel>&& static_constant_top_levels) {
    p_static_constant_top_levels->push_back(std::move(static_constant_top_levels));
}

static void append_double_static_constant_top_level(const TIdentifier& identifier, TDouble value, TULong binary,
                                                    TInt byte) {
    TIdentifier name = identifier;
    TInt alignment = byte;
    std::shared_ptr<StaticInit> initial_value = std::make_shared<DoubleInit>(value, binary);
    push_static_constant_top_levels(std::make_unique<AsmStaticConstant>(std::move(name),
                                                                                            std::move(alignment),
                                                                                            std::move(initial_value)));
}

// top_level = Function(identifier, bool, instruction*) | StaticVariable(identifier, bool, int, static_init)
//           | StaticConstant(identifier, int, static_init)
static std::unique_ptr<AsmTopLevel> generate_top_level(TacTopLevel* node) {
    switch(node->type()) {
        case AST_T::TacFunction_t:
            return generate_function_top_level(static_cast<TacFunction*>(node));
        case AST_T::TacStaticVariable_t:
            return generate_static_variable_top_level(static_cast<TacStaticVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// AST = Program(top_level*, top_level*)
static std::unique_ptr<AsmProgram> generate_program(TacProgram* node) {
    std::vector<std::unique_ptr<AsmTopLevel>> static_constant_top_levels;
    p_static_constant_top_levels = &static_constant_top_levels;

    std::vector<std::unique_ptr<AsmTopLevel>> top_levels;
    for(size_t top_level = 0; top_level < node->static_variable_top_levels.size(); top_level++) {
        std::unique_ptr<AsmTopLevel> static_variable_top_level =
                generate_top_level(node->static_variable_top_levels[top_level].get());
        top_levels.push_back(std::move(static_variable_top_level));
    }
    for(size_t top_level = 0; top_level < node->function_top_levels.size(); top_level++) {
        std::unique_ptr<AsmTopLevel> function_top_level =
                generate_top_level(node->function_top_levels[top_level].get());
        top_levels.push_back(std::move(function_top_level));
    }
    p_static_constant_top_levels = nullptr;

    return std::make_unique<AsmProgram>(std::move(static_constant_top_levels), std::move(top_levels));
}

std::unique_ptr<AsmProgram> assembly_generation(std::unique_ptr<TacProgram> tac_ast) {
    std::unique_ptr<AsmProgram> asm_ast = generate_program(tac_ast.get());
    tac_ast.reset();
    if(!asm_ast) {
        RAISE_INTERNAL_ERROR;
    }
    convert_symbol_table(asm_ast.get());
    fix_stack(asm_ast.get());
    return asm_ast;
}
