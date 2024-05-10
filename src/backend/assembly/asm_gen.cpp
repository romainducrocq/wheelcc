#include "backend/assembly/asm_gen.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/back_symt.hpp"
#include "ast/interm_ast.hpp"
#include "ast/back_ast.hpp"
#include "frontend/intermediate/names.hpp"
#include "backend/assembly/registers.hpp"
#include "backend/assembly/symt_cvt.hpp"
#include "backend/assembly/stack_fix.hpp"

#include <string>
#include <memory>
#include <vector>

static std::unique_ptr<AsmGenContext> context;

AsmGenContext::AsmGenContext()
    : ARG_REGISTERS({
        REGISTER_KIND::Di, REGISTER_KIND::Si, REGISTER_KIND::Dx, REGISTER_KIND::Cx, REGISTER_KIND::R8, REGISTER_KIND::R9
    }), ARG_SSE_REGISTERS({
        REGISTER_KIND::Xmm0, REGISTER_KIND::Xmm1, REGISTER_KIND::Xmm2, REGISTER_KIND::Xmm3, REGISTER_KIND::Xmm4,
        REGISTER_KIND::Xmm5, REGISTER_KIND::Xmm6, REGISTER_KIND::Xmm7
    }) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

static std::shared_ptr<AsmImm> generate_char_imm_operand(CConstChar* node) {
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(true, false, std::move(value));
}

static std::shared_ptr<AsmImm> generate_int_imm_operand(CConstInt* node) {
    bool is_byte = node->value <= 255;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_byte), false, std::move(value));
}

static std::shared_ptr<AsmImm> generate_long_imm_operand(CConstLong* node) {
    bool is_byte = node->value <= 255l;
    bool is_quad = node->value > 2147483647l;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_byte), std::move(is_quad), std::move(value));
}

static std::shared_ptr<AsmImm> generate_uchar_imm_operand(CConstUChar* node) {
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(true, false, std::move(value));
}

static std::shared_ptr<AsmImm> generate_uint_imm_operand(CConstUInt* node) {
    bool is_byte = node->value <= 255u;
    bool is_quad = node->value > 2147483647u;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_byte), std::move(is_quad), std::move(value));
}

static std::shared_ptr<AsmImm> generate_ulong_imm_operand(CConstULong* node) {
    bool is_byte = node->value <= 255ul;
    bool is_quad = node->value > 2147483647ul;
    TIdentifier value = std::to_string(node->value);
    return std::make_shared<AsmImm>(std::move(is_byte), std::move(is_quad), std::move(value));
}

static void generate_double_static_constant_top_level(const TIdentifier& identifier, TDouble value, TULong binary,
                                                      TInt byte);

static std::shared_ptr<AsmData> generate_double_static_constant_operand(TDouble value, TULong binary, TInt byte) {
    TIdentifier static_constant_label;
    {
        TIdentifier static_constant_hash = std::to_string(binary);
        if(frontend->static_constant_table.find(static_constant_hash) != frontend->static_constant_table.end()) {
            static_constant_label = frontend->static_constant_table[static_constant_hash];
        }
        else {
            static_constant_label = represent_label_identifier(LABEL_KIND::Ldouble);
            frontend->static_constant_table[static_constant_hash] = static_constant_label;
            generate_double_static_constant_top_level(static_constant_label, value, binary, byte);
        }
    }
    return std::make_shared<AsmData>(std::move(static_constant_label), 0l);
}

static std::shared_ptr<AsmData> generate_double_constant_operand(CConstDouble* node) {
    return generate_double_static_constant_operand(node->value, double_to_binary(node->value), 8);
}

static std::shared_ptr<AsmOperand> generate_constant_operand(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstChar_t:
            return generate_char_imm_operand(static_cast<CConstChar*>(node->constant.get()));
        case AST_T::CConstInt_t:
            return generate_int_imm_operand(static_cast<CConstInt*>(node->constant.get()));
        case AST_T::CConstLong_t:
            return generate_long_imm_operand(static_cast<CConstLong*>(node->constant.get()));
        case AST_T::CConstDouble_t:
            return generate_double_constant_operand(static_cast<CConstDouble*>(node->constant.get()));
        case AST_T::CConstUChar_t:
            return generate_uchar_imm_operand(static_cast<CConstUChar*>(node->constant.get()));
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

static std::shared_ptr<AsmPseudoMem> generate_pseudo_mem_operand(TacVariable* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmPseudoMem>(std::move(name), 0l);
}

static std::shared_ptr<AsmOperand> generate_variable_operand(TacVariable* node) {
    switch(frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Array_t:
        case AST_T::Structure_t:
            return generate_pseudo_mem_operand(node);
        default:
            return generate_pseudo_operand(node);
    }
}

// operand = Imm(int, bool, bool) | Reg(reg) | Pseudo(identifier) | Memory(int, reg) | Data(identifier, int)
//         | PseudoMem(identifier, int) | Indexed(int, reg, reg)
static std::shared_ptr<AsmOperand> generate_operand(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return generate_constant_operand(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return generate_variable_operand(static_cast<TacVariable*>(node));
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

// binary_operator = Add | Sub | Mult | DivDouble | Shl | ShrTwoOp | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight
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
        case AST_T::CConstChar_t:
        case AST_T::CConstInt_t:
        case AST_T::CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_variable_value_signed(TacVariable* node) {
    switch(frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
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

static bool is_constant_value_1byte(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstChar_t:
        case AST_T::CConstUChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_variable_value_1byte(TacVariable* node) {
    switch(frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_1byte(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return is_constant_value_1byte(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_variable_value_1byte(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_constant_value_4byte(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstInt_t:
        case AST_T::CConstUInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_variable_value_4byte(TacVariable* node) {
    switch(frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_4byte(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacConstant_t:
            return is_constant_value_4byte(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_variable_value_4byte(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_constant_value_double(TacConstant* node) {
    return node->constant->type() == AST_T::CConstDouble_t;
}

static bool is_variable_value_double(TacVariable* node) {
    return frontend->symbol_table[node->name]->type_t->type() == AST_T::Double_t;
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

static bool is_variable_value_structure(TacVariable* node) {
    return frontend->symbol_table[node->name]->type_t->type() == AST_T::Structure_t;
}

static bool is_value_structure(TacValue* node) {
    switch(node->type()) {
        case AST_T::TacVariable_t:
            return is_variable_value_structure(static_cast<TacVariable*>(node));
        case AST_T::TacConstant_t:
            return false;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AssemblyType> generate_constant_assembly_type(TacConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstChar_t:
        case AST_T::CConstUChar_t:
            return std::make_shared<Byte>();
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

static std::shared_ptr<AssemblyType> generate_8byte_assembly_type(Structure* struct_type, TLong offset) {
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size - offset;
    if(size >= 8l) {
        return std::make_shared<QuadWord>();
    }
    switch(size) {
        case 1l:
            return std::make_shared<Byte>();
        case 4l:
            return std::make_shared<LongWord>();
        default:
            return std::make_shared<ByteArray>(std::move(size), 8);
    }
}

static void push_instruction(std::unique_ptr<AsmInstruction>&& instruction) {
    context->p_instructions->push_back(std::move(instruction));
}

static void generate_return_integer_instructions(TacReturn* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->val.get());
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_val = generate_assembly_type(node->val.get());
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_val), std::move(src),
                                                        std::move(dst)));
}

static void generate_return_double_instructions(TacReturn* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->val.get());
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm0);
    std::shared_ptr<AssemblyType> assembly_type_val = std::make_shared<BackendDouble>();
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_val), std::move(src),
                                                        std::move(dst)));
}

static void generate_return_instructions(TacReturn* node) {
    if(node->val) {
        if(is_value_double(node->val.get())) {
            generate_return_double_instructions(node);
        }
        else {
            generate_return_integer_instructions(node);
        }
    }
    push_instruction(std::make_unique<AsmRet>());
}

static void generate_sign_extend_instructions(TacSignExtend* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
    push_instruction(std::make_unique<AsmMovSx>(std::move(assembly_type_src), std::move(assembly_type_dst),
                                                          std::move(src), std::move(dst)));
}

static void generate_imm_byte_truncate_instructions(AsmImm* node) {
    if(!node->is_byte) {
        node->value = std::to_string(string_to_uint64(std::move(node->value)) % 256l);
    }
}

static void generate_byte_truncate_instructions(TacTruncate* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<Byte>();
    if(src->type() == AST_T::AsmImm_t) {
        generate_imm_byte_truncate_instructions(static_cast<AsmImm*>(src.get()));
    }
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                        std::move(dst)));
}

static void generate_imm_long_truncate_instructions(AsmImm* node) {
    if(node->is_quad) {
        node->value = std::to_string(string_to_uint64(std::move(node->value)) - 4294967296ul);
    }
}

static void generate_long_truncate_instructions(TacTruncate* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
    if(src->type() == AST_T::AsmImm_t) {
        generate_imm_long_truncate_instructions(static_cast<AsmImm*>(src.get()));
    }
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                        std::move(dst)));
}

static void generate_truncate_instructions(TacTruncate* node) {
    if(is_value_1byte(node->dst.get())) {
        generate_byte_truncate_instructions(node);
    }
    else {
        generate_long_truncate_instructions(node);
    }
}

static void generate_zero_extend_instructions(TacZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
    push_instruction(std::make_unique<AsmMovZeroExtend>(std::move(assembly_type_src),
                                                                  std::move(assembly_type_dst), std::move(src),
                                                                  std::move(dst)));
}

static void generate_char_double_to_signed_instructions(TacDoubleToInt* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<LongWord>();
        push_instruction(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src),
                                                                 std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<Byte>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst),
                                                            std::move(src_dst), std::move(dst)));
    }
}

static void generate_int_long_double_to_signed_instructions(TacDoubleToInt* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->dst.get());
    push_instruction(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src), std::move(src),
                                                              std::move(dst)));
}

static void generate_double_to_signed_instructions(TacDoubleToInt* node) {
    if(is_value_1byte(node->dst.get())) {
        generate_char_double_to_signed_instructions(node);
    }
    else {
        generate_int_long_double_to_signed_instructions(node);
    }
}

static void generate_uchar_double_to_unsigned_instructions(TacDoubleToUInt* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<LongWord>();
        push_instruction(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src),
                                                                 std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<Byte>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst),
                                                           std::move(src_dst), std::move(dst)));
    }
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
    TIdentifier target_out_of_range = represent_label_identifier(LABEL_KIND::Lsd2si_out_of_range);
    TIdentifier target_after = represent_label_identifier(LABEL_KIND::Lsd2si_after);
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
        std::shared_ptr<AsmOperand> upper_bound_si = std::make_shared<AsmImm>(false, true,
                                                                              "9223372036854775808");
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
    if(is_value_1byte(node->dst.get())) {
        generate_uchar_double_to_unsigned_instructions(node);
    }
    else if(is_value_4byte(node->dst.get())) {
        generate_uint_double_to_unsigned_instructions(node);
    }
    else {
        generate_ulong_double_to_unsigned_instructions(node);
    }
}

static void generate_char_signed_to_double_instructions(TacIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<Byte>();
        push_instruction(std::make_unique<AsmMovSx>(std::move(assembly_type_src), assembly_type_dst,
                                                             std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_dst),
                                                                 std::move(src_dst), std::move(dst)));
    }
}

static void generate_int_long_signed_to_double_instructions(TacIntToDouble* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    push_instruction(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_src), std::move(src),
                                                             std::move(dst)));
}

static void generate_signed_to_double_instructions(TacIntToDouble* node) {
    if(is_value_1byte(node->src.get())) {
        generate_char_signed_to_double_instructions(node);
    }
    else {
        generate_int_long_signed_to_double_instructions(node);
    }
}

static void generate_uchar_unsigned_to_double_instructions(TacUIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<Byte>();
        push_instruction(std::make_unique<AsmMovZeroExtend>(std::move(assembly_type_src), assembly_type_dst,
                                                                     std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_dst),
                                                                 std::move(src_dst), std::move(dst)));
    }
}

static void generate_uint_unsigned_to_double_instructions(TacUIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<LongWord>();
        push_instruction(std::make_unique<AsmMovZeroExtend>(std::move(assembly_type_src), assembly_type_dst,
                                                                      std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_dst),
                                                                 std::move(src_dst), std::move(dst)));
    }
}

static void generate_ulong_unsigned_to_double_instructions(TacUIntToDouble* node) {
    TIdentifier target_out_of_range = represent_label_identifier(LABEL_KIND::Lsi2sd_out_of_range);
    TIdentifier target_after = represent_label_identifier(LABEL_KIND::Lsi2sd_after);
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AsmOperand> dst_out_of_range_si = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AsmOperand> dst_out_of_range_si_shr = generate_register(REGISTER_KIND::Dx);
    std::shared_ptr<AssemblyType> assembly_type_si = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> lower_bound_si = std::make_shared<AsmImm>(true, false, "0");
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
        std::shared_ptr<AsmOperand> set_bit_si = std::make_shared<AsmImm>(true, false, "1");
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
    if(is_value_1byte(node->src.get())) {
        generate_uchar_unsigned_to_double_instructions(node);
    }
    else if(is_value_4byte(node->src.get())) {
        generate_uint_unsigned_to_double_instructions(node);
    }
    else {
        generate_ulong_unsigned_to_double_instructions(node);
    }
}

static void generate_allocate_stack_instructions(TLong byte) {
    push_instruction(allocate_stack_bytes(byte));
}

static void generate_deallocate_stack_instructions(TLong byte) {
    push_instruction(deallocate_stack_bytes(byte));
}

static void generate_structure_type_classes(Structure* struct_type) {
     if(context->struct_8byte_classes_map.find(struct_type->tag) == context->struct_8byte_classes_map.end()) {
         std::vector<STRUCT_8BYTE_CLASS> struct_8byte_classes;
         if(frontend->struct_typedef_table[struct_type->tag]->size > 16l) {
             TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
             while(size > 0l) {
                 struct_8byte_classes.push_back(STRUCT_8BYTE_CLASS::MEMORY);
                 size -= 8l;
             }
         }
         else {
             Type* member_type = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, 0)->member_type.get();
             while(true) {
                 switch(member_type->type()) {
                     case AST_T::Array_t: {
                         member_type = static_cast<Array*>(member_type)->elem_type.get();
                         break;
                     }
                     case AST_T::Structure_t: {
                         member_type = GET_STRUCT_TYPEDEF_MEMBER(static_cast<Structure*>(member_type)->tag, 0)
                                                                                                     ->member_type.get();
                         break;
                     }
                     default:
                         goto Lbreak_1;
                 }
             }
             Lbreak_1:
             struct_8byte_classes.push_back(member_type->type() == AST_T::Double_t ? STRUCT_8BYTE_CLASS::SSE :
                                                                                     STRUCT_8BYTE_CLASS::INTEGER);
             if(frontend->struct_typedef_table[struct_type->tag]->size > 8l) {
                 member_type = GET_STRUCT_TYPEDEF_MEMBER_BACK(struct_type->tag)->member_type.get();
                 while(true) {
                     switch(member_type->type()) {
                         case AST_T::Array_t: {
                             member_type = static_cast<Array*>(member_type)->elem_type.get();
                             break;
                         }
                         case AST_T::Structure_t: {
                             member_type = GET_STRUCT_TYPEDEF_MEMBER_BACK(static_cast<Structure*>(member_type)->tag)
                                                                                                     ->member_type.get();
                             break;
                         }
                         default:
                             goto Lbreak_2;
                     }
                 }
                 Lbreak_2:
                 struct_8byte_classes.push_back(member_type->type() == AST_T::Double_t ? STRUCT_8BYTE_CLASS::SSE :
                                                                                         STRUCT_8BYTE_CLASS::INTEGER);
             }
         }
         context->struct_8byte_classes_map[struct_type->tag] = std::move(struct_8byte_classes);
     }
}

static void generate_reg_arg_fun_call_instructions(TacValue* node, REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = generate_operand(node);
    std::shared_ptr<AsmOperand> dst = generate_register(arg_register);
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node);
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                        std::move(dst)));
}

static void generate_stack_arg_fun_call_instructions(TacValue* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node);
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node);
    if(src->type() == AST_T::AsmRegister_t ||
       src->type() == AST_T::AsmImm_t ||
       assembly_type_src->type() == AST_T::QuadWord_t ||
       assembly_type_src->type() == AST_T::BackendDouble_t) {
        push_instruction(std::make_unique<AsmPush>(std::move(src)));
    }
    else {
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        push_instruction(std::make_unique<AsmPush>(dst));
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_8byte_reg_arg_fun_call_instructions(const TIdentifier& name, TLong offset, Structure* struct_type,
                                                         REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
    }
    std::shared_ptr<AsmOperand> dst = generate_register(arg_register);
    std::shared_ptr<AssemblyType> assembly_type_src = struct_type ? generate_8byte_assembly_type(struct_type, offset) :
                                                                    std::make_shared<BackendDouble>();
    if(assembly_type_src->type() == AST_T::ByteArray_t) {
        // TODO
        // if assembly_type is ByteArray(size, alignment):
        //     copy_bytes_to_reg(assembly_arg, r, size)
    }
    else {
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_8byte_stack_arg_fun_call_instructions(const TIdentifier& name, TLong offset,
                                                           Structure* struct_type) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
    }
    std::shared_ptr<AssemblyType> assembly_type_src = generate_8byte_assembly_type(struct_type, offset);
    switch(assembly_type_src->type()) {
        case AST_T::QuadWord_t:
            push_instruction(std::make_unique<AsmPush>(std::move(src)));
            break;
        case AST_T::ByteArray_t: {
            // TODO - in reverse order!
            // if assembly_type is ByteArray(size, alignment):
            //     emit(Binary(Sub, Quadword, Imm(8), Reg(SP)))
            //     copy_bytes(from=assembly_arg, to=Memory(SP, 0), count=size)
            break;
        }
        default: {
            std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
            push_instruction(std::make_unique<AsmPush>(dst));
            push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src),
                                                                std::move(src), std::move(dst)));
        }
    }
}

static void generate_arg_fun_call_instructions(TacFunCall* node, TLong& stack_padding, size_t reg_size) {
    std::vector<std::unique_ptr<AsmInstruction>> reg_instructions;
    std::vector<std::unique_ptr<AsmInstruction>> sse_instructions;
    std::vector<std::unique_ptr<AsmInstruction>> stack_instructions;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instructions = context->p_instructions;
    for(size_t arg = 0; arg < node->args.size(); arg++) {
        if(is_value_double(node->args[arg].get())) {
            if(sse_instructions.size() < 8) {
                context->p_instructions = &sse_instructions;
                generate_reg_arg_fun_call_instructions(node->args[arg].get(),
                                                       context->ARG_SSE_REGISTERS[sse_instructions.size()]);
            }
            else {
                context->p_instructions = &stack_instructions;
                generate_stack_arg_fun_call_instructions(node->args[arg].get());
                stack_padding += 8l;
            }
        }
        else if(!is_value_structure(node->args[arg].get())) {
            if(reg_size < 6) {
                context->p_instructions = &reg_instructions;
                generate_reg_arg_fun_call_instructions(node->args[arg].get(),
                                                       context->ARG_REGISTERS[reg_size]);
                reg_size++;
            }
            else {
                context->p_instructions = &stack_instructions;
                generate_stack_arg_fun_call_instructions(node->args[arg].get());
                stack_padding += 8l;
            }
        }
        else {
            size_t struct_reg_size = 6;
            size_t struct_sse_size = 8;
            TIdentifier name = static_cast<TacVariable*>(node->args[arg].get())->name;
            Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
            generate_structure_type_classes(struct_type);
            if(context->struct_8byte_classes_map[struct_type->tag][0] != STRUCT_8BYTE_CLASS::MEMORY) {
                struct_reg_size = 0;
                struct_sse_size = 0;
                for(size_t struct_8byte_class = 0;
                    struct_8byte_class < context->struct_8byte_classes_map[struct_type->tag].size();
                    struct_8byte_class++) {
                    if(context->struct_8byte_classes_map[struct_type->tag][struct_8byte_class]
                                                                                           == STRUCT_8BYTE_CLASS::SSE) {
                        struct_sse_size++;
                    }
                    else {
                        struct_reg_size++;
                    }
                }
            }
            if(struct_reg_size + reg_size < 6 &&
               struct_sse_size + sse_instructions.size() < 8) {
                TLong offset = 0l;
                for(size_t struct_8byte_class = 0;
                    struct_8byte_class < context->struct_8byte_classes_map[struct_type->tag].size();
                    struct_8byte_class++) {
                    if(context->struct_8byte_classes_map[struct_type->tag][struct_8byte_class] ==
                                                                                              STRUCT_8BYTE_CLASS::SSE) {
                        context->p_instructions = &sse_instructions;
                        generate_8byte_reg_arg_fun_call_instructions(name, offset, nullptr,
                                                         context->ARG_SSE_REGISTERS[sse_instructions.size()]);
                    }
                    else {
                        context->p_instructions = &reg_instructions;
                        generate_8byte_reg_arg_fun_call_instructions(name, offset, struct_type,
                                                                     context->ARG_REGISTERS[reg_size]);
                        reg_size++;
                    }
                    offset += 8l;
                }
            }
            else {
                TLong offset = 0l;
                context->p_instructions = &stack_instructions;
                for(size_t throwaway = 0; throwaway < context->struct_8byte_classes_map[struct_type->tag].size();
                    throwaway++) {
                    generate_8byte_stack_arg_fun_call_instructions(name, offset, struct_type);
                    offset += 8l;
                }
            }
        }
    }
    context->p_instructions = p_instructions;

    context->p_instructions->insert(context->p_instructions->end(),
                                    std::make_move_iterator(reg_instructions.begin()),
                                    std::make_move_iterator(reg_instructions.end()));
    context->p_instructions->insert(context->p_instructions->end(),
                                    std::make_move_iterator(sse_instructions.begin()),
                                    std::make_move_iterator(sse_instructions.end()));
    context->p_instructions->reserve(context->p_instructions->size() + stack_instructions.size());
    for(size_t stack_instruction = stack_instructions.size(); stack_instruction-- > 0;) {
        push_instruction(std::move(stack_instructions[stack_instruction]));
    }
}

static void generate_return_fun_call_instructions(TacValue* node, REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = generate_register(arg_register);
    std::shared_ptr<AsmOperand> dst = generate_operand(node);
    std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node);
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                        std::move(dst)));
}

static void generate_8byte_return_fun_call_instructions(const TIdentifier& name, TLong offset, Structure* struct_type,
                                                        REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = generate_register(arg_register);
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = name;
        TLong to_offset = offset;
        dst = std::make_shared<AsmPseudoMem>(std::move(dst_name), std::move(to_offset));
    }
    std::shared_ptr<AssemblyType> assembly_type_dst = struct_type ? generate_8byte_assembly_type(struct_type, offset) :
                                                                    std::make_shared<BackendDouble>();
    if(assembly_type_dst->type() == AST_T::ByteArray_t) {
        // TODO
        // if t is ByteArray(size, alignment):
        //     copy_bytes_from_reg(r, op, size)
    }
    else {
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_fun_call_instructions(TacFunCall* node) {
    bool is_return_memory = false;
    {
        TLong stack_padding = 0l;
        if(node->args.size() % 2 == 1) {
            stack_padding = 8l;
            generate_allocate_stack_instructions(stack_padding);
        }

        size_t reg_size = 0;
        if(node->dst &&
           is_value_structure(node->dst.get())) {
            TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
            Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
            generate_structure_type_classes(struct_type);
            if(context->struct_8byte_classes_map[struct_type->tag][0] == STRUCT_8BYTE_CLASS::MEMORY) {
                is_return_memory = true;
                {
                    std::shared_ptr<AsmOperand> src = generate_operand(node->dst.get());
                    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Di);
                    push_instruction(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
                }
                reg_size++;
            }
        }
        generate_arg_fun_call_instructions(node, stack_padding, reg_size);

        {
            TIdentifier name = node->name;
            push_instruction(std::make_unique<AsmCall>(std::move(name)));
        }

        if(stack_padding > 0l) {
            generate_deallocate_stack_instructions(stack_padding);
        }
    }

    if(node->dst &&
       !is_return_memory) {
        if(is_value_double(node->dst.get())) {
            generate_return_fun_call_instructions(node->dst.get(), REGISTER_KIND::Xmm0);
        }
        else if(!is_value_structure(node->dst.get())) {
            generate_return_fun_call_instructions(node->dst.get(), REGISTER_KIND::Ax);
        }
        else {
            bool sse_size = false;
            TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
            Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
            switch(context->struct_8byte_classes_map[struct_type->tag][0]) {
                case STRUCT_8BYTE_CLASS::INTEGER:
                    generate_8byte_return_fun_call_instructions(name, 0l, struct_type,
                                                                REGISTER_KIND::Ax);
                    break;
                case STRUCT_8BYTE_CLASS::SSE: {
                    generate_8byte_return_fun_call_instructions(name, 0l, nullptr,
                                                                REGISTER_KIND::Xmm0);
                    sse_size = true;
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(context->struct_8byte_classes_map[struct_type->tag].size() == 2) {
                switch(context->struct_8byte_classes_map[struct_type->tag][1]) {
                    case STRUCT_8BYTE_CLASS::INTEGER: {
                        if(sse_size) {
                            std::unique_ptr<AsmInstruction> sse_instruction = std::move(context->p_instructions->back());
                            context->p_instructions->pop_back();
                            generate_8byte_return_fun_call_instructions(name, 8l, struct_type,
                                                                        REGISTER_KIND::Di);
                            push_instruction(std::move(sse_instruction));
                        }
                        else {
                            generate_8byte_return_fun_call_instructions(name, 8l, struct_type,
                                                                        REGISTER_KIND::Di);
                        }
                        break;
                    }
                    case STRUCT_8BYTE_CLASS::SSE:
                        generate_8byte_return_fun_call_instructions(name, 8l, nullptr,
                                                                    REGISTER_KIND::Xmm1);
                        break;
                    default:
                        RAISE_INTERNAL_ERROR;
                }
            }
        }
    }
}

static void generate_unary_operator_conditional_integer_instructions(TacUnary* node) {
    std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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
    TIdentifier target_nan = represent_label_identifier(LABEL_KIND::Lcomisd_nan);
    std::shared_ptr<AsmOperand> cmp_dst = generate_operand(node->dst.get());
    {
        std::shared_ptr<AsmOperand> src1 = generate_operand(node->src1.get());
        std::shared_ptr<AsmOperand> src2 = generate_operand(node->src2.get());
        std::shared_ptr<AssemblyType> assembly_type_src1 = generate_assembly_type(node->src1.get());
        push_instruction(std::make_unique<AsmCmp>(std::move(assembly_type_src1),
                                                            std::move(src2), std::move(src1)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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

static void generate_copy_structure_instructions(TacCopy* node) {
    TIdentifier src_name = static_cast<TacVariable*>(node->src.get())->name;
    TIdentifier dst_name = static_cast<TacVariable*>(node->dst.get())->name;
    Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[src_name]->type_t.get());
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
    TLong offset = 0l;
    while(size > 0l) {
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
        std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
        std::shared_ptr<AssemblyType> assembly_type_src;
        if(size >= 8l) {
            assembly_type_src = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if(size >= 4l) {
            assembly_type_src = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            assembly_type_src = std::make_shared<Byte>();
            size -= 1l;
            offset += 1l;
        }
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_copy_scalar_instructions(TacCopy* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                        std::move(dst)));
}

static void generate_copy_instructions(TacCopy* node) {
    if(is_value_structure(node->src.get())) {
        generate_copy_structure_instructions(node);
    }
    else {
        generate_copy_scalar_instructions(node);
    }
}

static void generate_get_address_instructions(TacGetAddress* node) {
    std::shared_ptr<AsmOperand> src;
    {
        if(node->src->type() == AST_T::TacVariable_t) {
            TIdentifier name = static_cast<TacVariable*>(node->src.get())->name;
            if(frontend->symbol_table.find(name) != frontend->symbol_table.end() &&
               frontend->symbol_table[name]->attrs->type() == AST_T::ConstantAttr_t) {
                src = std::make_shared<AsmData>(std::move(name), 0l);
                goto Lpass;
            }
        }
        src = generate_operand(node->src.get());
        Lpass: ;
    }
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    push_instruction(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
}

static void generate_load_structure_instructions(TacLoad* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
        Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
        TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
        TLong offset = 0l;
        while(size > 0l) {
            std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Ax, offset);
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(name, offset);
            std::shared_ptr<AssemblyType> assembly_type_dst;
            if(size >= 8l) {
                assembly_type_dst = std::make_shared<QuadWord>();
                size -= 8l;
                offset += 8l;
            }
            else if(size >= 4l) {
                assembly_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
            }
            else {
                assembly_type_dst = std::make_shared<Byte>();
                size -= 1l;
                offset += 1l;
            }
            push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                                std::move(dst)));
        }
    }
}

static void generate_load_scalar_instructions(TacLoad* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Ax, 0l);
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_load_instructions(TacLoad* node) {
    if(is_value_structure(node->dst.get())) {
        generate_load_structure_instructions(node);
    }
    else {
        generate_load_scalar_instructions(node);
    }
}

static void generate_store_structure_instructions(TacStore* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->dst_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        TIdentifier name = static_cast<TacVariable*>(node->src.get())->name;
        Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
        TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
        TLong offset = 0l;
        while(size > 0l) {
            std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(name, offset);
            std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Ax, offset);
            std::shared_ptr<AssemblyType> assembly_type_dst;
            if(size >= 8l) {
                assembly_type_dst = std::make_shared<QuadWord>();
                size -= 8l;
                offset += 8l;
            }
            else if(size >= 4l) {
                assembly_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
            }
            else {
                assembly_type_dst = std::make_shared<Byte>();
                size -= 1l;
                offset += 1l;
            }
            push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                                std::move(dst)));
        }
    }
}

static void generate_store_scalar_instructions(TacStore* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->dst_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
        std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Ax, 0l);
        std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->src.get());
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_store_instructions(TacStore* node) {
    if(is_value_structure(node->src.get())) {
        generate_store_structure_instructions(node);
    }
    else {
        generate_store_scalar_instructions(node);
    }
}

static void generate_constant_index_add_ptr_instructions(TacAddPtr* node) {
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        TLong index;
        {
            CConst* constant = static_cast<TacConstant*>(node->index.get())->constant.get();
            if(constant->type() != AST_T::CConstLong_t) {
                RAISE_INTERNAL_ERROR;
            }
            index = static_cast<CConstLong*>(constant)->value;
        }
        std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Ax, index * node->scale);
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void generate_scalar_scale_variable_index_add_ptr_instructions(TacAddPtr* node) {
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        push_instruction(std::make_unique<AsmMov>(assembly_type_src, std::move(src),
                                                            std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->index.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Dx);
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_indexed(REGISTER_KIND::Ax,
                                                           REGISTER_KIND::Dx, node->scale);
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void generate_aggregate_scale_variable_index_add_ptr_instructions(TacAddPtr* node) {
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Dx);
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        push_instruction(std::make_unique<AsmMov>(assembly_type_src, std::move(src),
                                                            std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_operand(node->index.get());
        push_instruction(std::make_unique<AsmMov>(assembly_type_src, std::move(src), src_dst));
    }
    {
        bool is_byte = node->scale <= 255l;
        bool is_quad = node->scale > 2147483647l;
        TIdentifier value = std::to_string(node->scale);
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmImm>(std::move(is_byte), std::move(is_quad),
                                                                   std::move(value));
        std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmMult>();
        push_instruction(std::make_unique<AsmBinary>(std::move(binary_op),
                                                              std::move(assembly_type_src), std::move(src),
                                                              std::move(src_dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_indexed(REGISTER_KIND::Ax,
                                                           REGISTER_KIND::Dx, 1l);
        std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
        push_instruction(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void generate_variable_index_add_ptr_instructions(TacAddPtr* node) {
    switch(node->scale) {
        case 1l:
        case 2l:
        case 4l:
        case 8l:
            generate_scalar_scale_variable_index_add_ptr_instructions(node);
            break;
        default:
            generate_aggregate_scale_variable_index_add_ptr_instructions(node);
            break;
    }
}

static void generate_add_ptr_instructions(TacAddPtr* node) {
    switch(node->index->type()) {
        case AST_T::TacConstant_t:
            generate_constant_index_add_ptr_instructions(node);
            break;
        case AST_T::TacVariable_t:
            generate_variable_index_add_ptr_instructions(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void generate_copy_to_offset_structure_instructions(TacCopyToOffset* node) {
    TIdentifier src_name = static_cast<TacVariable*>(node->src.get())->name;
    Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[src_name]->type_t.get());
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
    TLong offset = 0l;
    while(size > 0l) {
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
        std::shared_ptr<AsmOperand> dst;
        {
            TIdentifier dst_name = node->dst_name;
            TLong to_offset = offset + node->offset;
            dst = std::make_shared<AsmPseudoMem>(std::move(dst_name),std::move(to_offset));
        }
        std::shared_ptr<AssemblyType> assembly_type_src;
        if(size >= 8l) {
            assembly_type_src = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if(size >= 4l) {
            assembly_type_src = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            assembly_type_src = std::make_shared<Byte>();
            size -= 1l;
            offset += 1l;
        }
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_copy_to_offset_scalar_instructions(TacCopyToOffset* node) {
    std::shared_ptr<AsmOperand> src = generate_operand(node->src.get());
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = node->dst_name;
        TLong to_offset = node->offset;
        dst = std::make_shared<AsmPseudoMem>(std::move(dst_name), std::move(to_offset));
    }
    std::shared_ptr<AssemblyType> assembly_type_src = generate_assembly_type(node->src.get());
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src),
                                                        std::move(dst)));
}

static void generate_copy_to_offset_instructions(TacCopyToOffset* node) {
    if(is_value_structure(node->src.get())) {
        generate_copy_to_offset_structure_instructions(node);
    }
    else {
        generate_copy_to_offset_scalar_instructions(node);
    }
}

static void generate_copy_from_offset_structure_instructions(TacCopyFromOffset* node) {
    TIdentifier dst_name = static_cast<TacVariable*>(node->dst.get())->name;
    Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[dst_name]->type_t.get());
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
    TLong offset = 0l;
    while(size > 0l) {
        std::shared_ptr<AsmOperand> src;
        {
            TIdentifier src_name = node->src_name;
            TLong from_offset = offset + node->offset;
            src = std::make_shared<AsmPseudoMem>(std::move(src_name),std::move(from_offset));
        }
        std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
        std::shared_ptr<AssemblyType> assembly_type_dst;
        if(size >= 8l) {
            assembly_type_dst = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if(size >= 4l) {
            assembly_type_dst = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            assembly_type_dst = std::make_shared<Byte>();
            size -= 1l;
            offset += 1l;
        }
        push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                            std::move(dst)));
    }
}

static void generate_copy_from_offset_scalar_instructions(TacCopyFromOffset* node) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = node->src_name;
        TLong from_offset = node->offset;
        src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
    }
    std::shared_ptr<AsmOperand> dst = generate_operand(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = generate_assembly_type(node->dst.get());
    push_instruction(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src),
                                                        std::move(dst)));
}

static void generate_copy_from_offset_instructions(TacCopyFromOffset* node) {
    if(is_value_structure(node->dst.get())) {
        generate_copy_from_offset_structure_instructions(node);
    }
    else {
        generate_copy_from_offset_scalar_instructions(node);
    }
}

static void generate_jump_instructions(TacJump* node) {
    TIdentifier target = node->target;
    push_instruction(std::make_unique<AsmJmp>(std::move(target)));
}

static void generate_jump_if_zero_integer_instructions(TacJumpIfZero* node) {
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(true, false, "0");
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
        case AST_T::TacAddPtr_t:
            generate_add_ptr_instructions(static_cast<TacAddPtr*>(node));
            break;
        case AST_T::TacCopyToOffset_t:
            generate_copy_to_offset_instructions(static_cast<TacCopyToOffset*>(node));
            break;
        case AST_T::TacCopyFromOffset_t:
            generate_copy_from_offset_instructions(static_cast<TacCopyFromOffset*>(node));
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

// instruction = Mov(assembly_type, operand, operand) | MovSx(assembly_type, assembly_type, operand, operand)
//             | MovZeroExtend(assembly_type, assembly_type, operand, operand) | Lea(operand, operand)
//             | Cvttsd2si(assembly_type, operand, operand) | Cvtsi2sd(assembly_type, operand, operand)
//             | Unary(unary_operator, assembly_type, operand) | Binary(binary_operator, assembly_type, operand, operand)
//             | Cmp(assembly_type, operand, operand) | Idiv(assembly_type, operand) | Div(assembly_type, operand)
//             | Cdq(assembly_type) | Jmp(identifier) | JmpCC(cond_code, identifier) | SetCC(cond_code, operand)
//             | Label(identifier) | Push(operand) | Call(identifier) | Ret
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

static void generate_stack_param_function_instructions(const TIdentifier& param, TLong byte) {
    std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Bp, (byte + 2l) * 8l);
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
    {
        context->p_instructions = &body;

        {
            size_t param_reg = 0;
            size_t param_sse = 0;
            TLong param_stack = 0l;
            for(size_t param = 0; param < node->params.size(); param++) {
                if(frontend->symbol_table[node->params[param]]->type_t->type() == AST_T::Double_t) {
                    if(param_sse < 8) {
                        generate_reg_param_function_instructions(node->params[param],
                                                                 context->ARG_SSE_REGISTERS[param_sse]);
                        param_sse += 1;
                    }
                    else {
                        generate_stack_param_function_instructions(node->params[param], param_stack);
                        param_stack += 1l;
                    }
                }
                else {
                    if(param_reg < 6) {
                        generate_reg_param_function_instructions(node->params[param],
                                                                 context->ARG_REGISTERS[param_reg]);
                        param_reg += 1;
                    }
                    else {
                        generate_stack_param_function_instructions(node->params[param], param_stack);
                        param_stack += 1l;
                    }
                }
            }
        }

        generate_list_instructions(node->body);
        context->p_instructions = nullptr;
    }

    return std::make_unique<AsmFunction>(std::move(name), std::move(is_global), std::move(body));
}

static std::unique_ptr<AsmStaticVariable> generate_static_variable_top_level(TacStaticVariable* node) {
    TIdentifier name = node->name;
    bool is_global = node->is_global;
    TInt alignment = generate_type_alignment(node->static_init_type.get());
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    for(std::shared_ptr<StaticInit> static_init: node->static_inits) {
        static_inits.push_back(std::move(static_init));
    }
    return std::make_unique<AsmStaticVariable>(std::move(name), std::move(alignment), std::move(is_global),
                                               std::move(static_inits));
}

static void push_static_constant_top_levels(std::unique_ptr<AsmTopLevel>&& static_constant_top_levels) {
    context->p_static_constant_top_levels->push_back(std::move(static_constant_top_levels));
}

static void generate_double_static_constant_top_level(const TIdentifier& identifier, TDouble value, TULong binary,
                                                      TInt byte) {
    TIdentifier name = identifier;
    TInt alignment = byte;
    std::shared_ptr<StaticInit> static_init = std::make_shared<DoubleInit>(value, binary);
    push_static_constant_top_levels(std::make_unique<AsmStaticConstant>(std::move(name),
                                                                                             std::move(alignment),
                                                                                             std::move(static_init)));
}

static std::unique_ptr<AsmStaticConstant> generate_static_constant_top_level(TacStaticConstant* node) {
    TIdentifier name = node->name;
    TInt alignment = generate_type_alignment(node->static_init_type.get());
    std::shared_ptr<StaticInit> static_init = node->static_init;
    return std::make_unique<AsmStaticConstant>(std::move(name), std::move(alignment), std::move(static_init));
}

// top_level = Function(identifier, bool, instruction*) | StaticVariable(identifier, bool, int, static_init*)
//           | StaticConstant(identifier, int, static_init)
static std::unique_ptr<AsmTopLevel> generate_top_level(TacTopLevel* node) {
    switch(node->type()) {
        case AST_T::TacFunction_t:
            return generate_function_top_level(static_cast<TacFunction*>(node));
        case AST_T::TacStaticVariable_t:
            return generate_static_variable_top_level(static_cast<TacStaticVariable*>(node));
        case AST_T::TacStaticConstant_t:
            return generate_static_constant_top_level(static_cast<TacStaticConstant*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// AST = Program(top_level*, top_level*)
static std::unique_ptr<AsmProgram> generate_program(TacProgram* node) {
    std::vector<std::unique_ptr<AsmTopLevel>> static_constant_top_levels;
    {
        for(size_t top_level = 0; top_level < node->static_constant_top_levels.size(); top_level++) {
            std::unique_ptr<AsmTopLevel> static_constant_top_level = generate_top_level(
                                                                node->static_constant_top_levels[top_level].get());
            static_constant_top_levels.push_back(std::move(static_constant_top_level));
        }
    }

    std::vector<std::unique_ptr<AsmTopLevel>> top_levels;
    {
        context->p_static_constant_top_levels = &static_constant_top_levels;
        for(size_t top_level = 0; top_level < node->static_variable_top_levels.size(); top_level++) {
            std::unique_ptr<AsmTopLevel> static_variable_top_level = generate_top_level(
                                                                node->static_variable_top_levels[top_level].get());
            top_levels.push_back(std::move(static_variable_top_level));
        }
        for(size_t top_level = 0; top_level < node->function_top_levels.size(); top_level++) {
            std::unique_ptr<AsmTopLevel> function_top_level = generate_top_level(
                                                                       node->function_top_levels[top_level].get());
            top_levels.push_back(std::move(function_top_level));
        }
        context->p_static_constant_top_levels = nullptr;
    }

    return std::make_unique<AsmProgram>(std::move(static_constant_top_levels), std::move(top_levels));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<AsmProgram> assembly_generation(std::unique_ptr<TacProgram> tac_ast) {
    context = std::make_unique<AsmGenContext>();
    std::unique_ptr<AsmProgram> asm_ast = generate_program(tac_ast.get());
    context.reset();

    tac_ast.reset();
    if(!asm_ast) {
        RAISE_INTERNAL_ERROR;
    }

    convert_symbol_table(asm_ast.get());
    fix_stack(asm_ast.get());
    return asm_ast;
}
