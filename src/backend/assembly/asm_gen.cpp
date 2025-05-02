#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"
#include "ast/interm_ast.hpp"

#include "backend/assembly/asm_gen.hpp"
#include "backend/assembly/registers.hpp"
#include "backend/assembly/stack_fix.hpp"
#include "backend/assembly/symt_cvt.hpp"

enum STRUCT_8B_CLS {
    INTEGER,
    SSE,
    MEMORY
};

enum ASM_LABEL_KIND {
    Lcomisd_nan,
    Ldouble,
    Lsd2si_after,
    Lsd2si_out_of_range,
    Lsi2sd_after,
    Lsi2sd_out_of_range
};

struct AsmGenContext {
    AsmGenContext();

    FunType* p_fun_type_top_level;
    std::array<REGISTER_KIND, 6> arg_registers;
    std::array<REGISTER_KIND, 8> sse_arg_registers;
    std::unordered_map<TIdentifier, TIdentifier> double_constant_table;
    std::unordered_map<TIdentifier, std::vector<STRUCT_8B_CLS>> struct_8b_cls_map;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instructions;
    std::vector<std::unique_ptr<AsmTopLevel>>* p_static_constant_top_levels;
};

AsmGenContext::AsmGenContext() :
    arg_registers({REGISTER_KIND::Di, REGISTER_KIND::Si, REGISTER_KIND::Dx, REGISTER_KIND::Cx, REGISTER_KIND::R8,
        REGISTER_KIND::R9}),
    sse_arg_registers({REGISTER_KIND::Xmm0, REGISTER_KIND::Xmm1, REGISTER_KIND::Xmm2, REGISTER_KIND::Xmm3,
        REGISTER_KIND::Xmm4, REGISTER_KIND::Xmm5, REGISTER_KIND::Xmm6, REGISTER_KIND::Xmm7}) {}

static std::unique_ptr<AsmGenContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

static std::shared_ptr<AsmImm> char_imm_op(CConstChar* node) {
    TULong value = static_cast<TULong>(node->value);
    bool is_neg = node->value < 0;
    return std::make_shared<AsmImm>(std::move(value), true, false, std::move(is_neg));
}

static std::shared_ptr<AsmImm> int_imm_op(CConstInt* node) {
    TULong value = static_cast<TULong>(node->value);
    bool is_byte = node->value <= 127 && node->value >= -128;
    bool is_neg = node->value < 0;
    return std::make_shared<AsmImm>(std::move(value), std::move(is_byte), false, std::move(is_neg));
}

static std::shared_ptr<AsmImm> long_imm_op(CConstLong* node) {
    TULong value = static_cast<TULong>(node->value);
    bool is_byte = node->value <= 127l && node->value >= -128l;
    bool is_quad = node->value > 2147483647l || node->value < -2147483648l;
    bool is_neg = node->value < 0l;
    return std::make_shared<AsmImm>(std::move(value), std::move(is_byte), std::move(is_quad), std::move(is_neg));
}

static std::shared_ptr<AsmImm> uchar_imm(CConstUChar* node) {
    TULong value = static_cast<TULong>(node->value);
    return std::make_shared<AsmImm>(std::move(value), true, false, false);
}

static std::shared_ptr<AsmImm> uint_imm_op(CConstUInt* node) {
    TULong value = static_cast<TULong>(node->value);
    bool is_byte = node->value <= 255u;
    bool is_quad = node->value > 2147483647u;
    return std::make_shared<AsmImm>(std::move(value), std::move(is_byte), std::move(is_quad), false);
}

static std::shared_ptr<AsmImm> ulong_imm_op(CConstULong* node) {
    TULong value = node->value;
    bool is_byte = node->value <= 255ul;
    bool is_quad = node->value > 2147483647ul;
    return std::make_shared<AsmImm>(std::move(value), std::move(is_byte), std::move(is_quad), false);
}

static TIdentifier repr_asm_label(ASM_LABEL_KIND asm_label_kind) {
    std::string name;
    switch (asm_label_kind) {
        case ASM_LABEL_KIND::Lcomisd_nan: {
            name = "comisd_nan";
            break;
        }
        case ASM_LABEL_KIND::Ldouble: {
            name = "double";
            break;
        }
        case ASM_LABEL_KIND::Lsd2si_after: {
            name = "sd2si_after";
            break;
        }
        case ASM_LABEL_KIND::Lsd2si_out_of_range: {
            name = "sd2si_out_of_range";
            break;
        }
        case ASM_LABEL_KIND::Lsi2sd_after: {
            name = "si2sd_after";
            break;
        }
        case ASM_LABEL_KIND::Lsi2sd_out_of_range: {
            name = "si2sd_out_of_range";
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return make_label_identifier(std::move(name));
}

static void dbl_static_const_toplvl(TIdentifier identifier, TIdentifier double_constant, TInt byte);

static std::shared_ptr<AsmData> dbl_static_const_op(TULong binary, TInt byte) {
    TIdentifier double_constant_label;
    {
        TIdentifier double_constant = make_string_identifier(std::to_string(binary));
        if (context->double_constant_table.find(double_constant) != context->double_constant_table.end()) {
            double_constant_label = context->double_constant_table[double_constant];
        }
        else {
            double_constant_label = repr_asm_label(ASM_LABEL_KIND::Ldouble);
            context->double_constant_table[double_constant] = double_constant_label;
            dbl_static_const_toplvl(double_constant_label, double_constant, byte);
        }
    }
    return std::make_shared<AsmData>(std::move(double_constant_label), 0l);
}

static std::shared_ptr<AsmData> dbl_const_op(CConstDouble* node) {
    TULong binary = double_to_binary(node->value);
    TInt byte = binary == 9223372036854775808ul ? 16 : 8;
    return dbl_static_const_op(binary, byte);
}

static std::shared_ptr<AsmOperand> const_op(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return char_imm_op(static_cast<CConstChar*>(node->constant.get()));
        case AST_T::CConstInt_t:
            return int_imm_op(static_cast<CConstInt*>(node->constant.get()));
        case AST_T::CConstLong_t:
            return long_imm_op(static_cast<CConstLong*>(node->constant.get()));
        case AST_T::CConstDouble_t:
            return dbl_const_op(static_cast<CConstDouble*>(node->constant.get()));
        case AST_T::CConstUChar_t:
            return uchar_imm(static_cast<CConstUChar*>(node->constant.get()));
        case AST_T::CConstUInt_t:
            return uint_imm_op(static_cast<CConstUInt*>(node->constant.get()));
        case AST_T::CConstULong_t:
            return ulong_imm_op(static_cast<CConstULong*>(node->constant.get()));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AsmPseudo> pseudo_op(TacVariable* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmPseudo>(std::move(name));
}

static std::shared_ptr<AsmPseudoMem> pseudo_mem_op(TacVariable* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmPseudoMem>(std::move(name), 0l);
}

static std::shared_ptr<AsmOperand> var_op(TacVariable* node) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Array_t:
        case AST_T::Structure_t:
            return pseudo_mem_op(node);
        default:
            return pseudo_op(node);
    }
}

// operand = Imm(int, bool, bool, bool) | Reg(reg) | Pseudo(identifier) | Memory(int, reg) | Data(identifier, int)
//         | PseudoMem(identifier, int) | Indexed(int, reg, reg)
static std::shared_ptr<AsmOperand> gen_op(TacValue* node) {
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return const_op(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return var_op(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// (signed) cond_code = E | NE | L | LE | G | GE
static std::unique_ptr<AsmCondCode> signed_cond_code(TacBinaryOp* node) {
    switch (node->type()) {
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
static std::unique_ptr<AsmCondCode> unsigned_cond_code(TacBinaryOp* node) {
    switch (node->type()) {
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
static std::unique_ptr<AsmUnaryOp> gen_unop(TacUnaryOp* node) {
    switch (node->type()) {
        case AST_T::TacComplement_t:
            return std::make_unique<AsmNot>();
        case AST_T::TacNegate_t:
            return std::make_unique<AsmNeg>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// binary_operator = Add | Sub | Mult | DivDouble | BitAnd | BitOr | BitXor | BitShiftLeft | BitShiftRight |
//                 BitShrArithmetic
static std::unique_ptr<AsmBinaryOp> gen_binop(TacBinaryOp* node) {
    switch (node->type()) {
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
        case AST_T::TacBitShrArithmetic_t:
            return std::make_unique<AsmBitShrArithmetic>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_const_signed(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
        case AST_T::CConstInt_t:
        case AST_T::CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_signed(TacVariable* node) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
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
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return is_const_signed(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_var_signed(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_const_1b(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
        case AST_T::CConstUChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_1b(TacVariable* node) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_1b(TacValue* node) {
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return is_const_1b(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_var_1b(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_const_4b(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstInt_t:
        case AST_T::CConstUInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_4b(TacVariable* node) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_4b(TacValue* node) {
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return is_const_4b(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_var_4b(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_const_dbl(TacConstant* node) { return node->constant->type() == AST_T::CConstDouble_t; }

static bool is_var_dbl(TacVariable* node) {
    return frontend->symbol_table[node->name]->type_t->type() == AST_T::Double_t;
}

static bool is_value_dbl(TacValue* node) {
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return is_const_dbl(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_var_dbl(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_var_struct(TacVariable* node) {
    return frontend->symbol_table[node->name]->type_t->type() == AST_T::Structure_t;
}

static bool is_value_struct(TacValue* node) {
    switch (node->type()) {
        case AST_T::TacVariable_t:
            return is_var_struct(static_cast<TacVariable*>(node));
        case AST_T::TacConstant_t:
            return false;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AssemblyType> const_asm_type(TacConstant* node) {
    switch (node->constant->type()) {
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

static std::shared_ptr<AssemblyType> var_asm_type(TacVariable* node) {
    return convert_backend_assembly_type(node->name);
}

static std::shared_ptr<AssemblyType> gen_asm_type(TacValue* node) {
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return const_asm_type(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return var_asm_type(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<AssemblyType> asm_type_8b(Structure* struct_type, TLong offset) {
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size - offset;
    if (size >= 8l) {
        return std::make_shared<QuadWord>();
    }
    switch (size) {
        case 1l:
            return std::make_shared<Byte>();
        case 4l:
            return std::make_shared<LongWord>();
        default:
            return std::make_shared<ByteArray>(std::move(size), 8);
    }
}

static void struct_8b_clss(Structure* struct_type);

static std::vector<STRUCT_8B_CLS> struct_mem_8b_clss(Structure* struct_type) {
    std::vector<STRUCT_8B_CLS> struct_8b_cls;
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
    while (size > 0l) {
        struct_8b_cls.push_back(STRUCT_8B_CLS::MEMORY);
        size -= 8l;
    }
    return struct_8b_cls;
}

static std::vector<STRUCT_8B_CLS> struct_1_reg_8b_clss(Structure* struct_type) {
    std::vector<STRUCT_8B_CLS> struct_8b_cls {STRUCT_8B_CLS::SSE};
    size_t members_front = struct_type->is_union ? frontend->struct_typedef_table[struct_type->tag]->members.size() : 1;
    for (size_t i = 0; i < members_front; ++i) {
        if (struct_8b_cls[0] == STRUCT_8B_CLS::INTEGER) {
            break;
        }
        Type* member_type = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i)->member_type.get();
        while (member_type->type() == AST_T::Array_t) {
            member_type = static_cast<Array*>(member_type)->elem_type.get();
        }
        if (member_type->type() == AST_T::Structure_t) {
            Structure* member_struct_type = static_cast<Structure*>(member_type);
            struct_8b_clss(member_struct_type);
            if (context->struct_8b_cls_map[member_struct_type->tag][0] == STRUCT_8B_CLS::INTEGER) {
                struct_8b_cls[0] = STRUCT_8B_CLS::INTEGER;
            }
        }
        else if (member_type->type() != AST_T::Double_t) {
            struct_8b_cls[0] = STRUCT_8B_CLS::INTEGER;
        }
    }
    return struct_8b_cls;
}

static std::vector<STRUCT_8B_CLS> struct_2_reg_8b_clss(Structure* struct_type) {
    std::vector<STRUCT_8B_CLS> struct_8b_cls {STRUCT_8B_CLS::SSE, STRUCT_8B_CLS::SSE};
    size_t members_front = struct_type->is_union ? frontend->struct_typedef_table[struct_type->tag]->members.size() : 1;
    for (size_t i = 0; i < members_front; ++i) {
        if (struct_8b_cls[0] == STRUCT_8B_CLS::INTEGER && struct_8b_cls[1] == STRUCT_8B_CLS::INTEGER) {
            break;
        }
        TLong size = 1l;
        Type* member_type = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i)->member_type.get();
        if (member_type->type() == AST_T::Array_t) {
            do {
                Array* member_arr_type = static_cast<Array*>(member_type);
                member_type = member_arr_type->elem_type.get();
                size *= member_arr_type->size;
            }
            while (member_type->type() == AST_T::Array_t);
        }
        if (member_type->type() == AST_T::Structure_t) {
            size *= frontend->struct_typedef_table[static_cast<Structure*>(member_type)->tag]->size;
        }
        else {
            size *= generate_type_alignment(member_type);
        }
        if (size > 8l) {
            if (member_type->type() == AST_T::Structure_t) {
                Structure* member_struct_type = static_cast<Structure*>(member_type);
                struct_8b_clss(member_struct_type);
                if (context->struct_8b_cls_map[member_struct_type->tag].size() > 1) {
                    if (context->struct_8b_cls_map[member_struct_type->tag][0] == STRUCT_8B_CLS::INTEGER) {
                        struct_8b_cls[0] = STRUCT_8B_CLS::INTEGER;
                    }
                    if (context->struct_8b_cls_map[member_struct_type->tag][1] == STRUCT_8B_CLS::INTEGER) {
                        struct_8b_cls[1] = STRUCT_8B_CLS::INTEGER;
                    }
                }
                else {
                    if (context->struct_8b_cls_map[member_struct_type->tag][0] == STRUCT_8B_CLS::INTEGER) {
                        struct_8b_cls[0] = STRUCT_8B_CLS::INTEGER;
                        struct_8b_cls[1] = STRUCT_8B_CLS::INTEGER;
                    }
                }
            }
            else if (member_type->type() != AST_T::Double_t) {
                struct_8b_cls[0] = STRUCT_8B_CLS::INTEGER;
                struct_8b_cls[1] = STRUCT_8B_CLS::INTEGER;
            }
        }
        else {
            if (member_type->type() == AST_T::Structure_t) {
                Structure* member_struct_type = static_cast<Structure*>(member_type);
                struct_8b_clss(member_struct_type);
                if (context->struct_8b_cls_map[member_struct_type->tag][0] == STRUCT_8B_CLS::INTEGER) {
                    struct_8b_cls[0] = STRUCT_8B_CLS::INTEGER;
                }
            }
            else if (member_type->type() != AST_T::Double_t) {
                struct_8b_cls[0] = STRUCT_8B_CLS::INTEGER;
            }
            if (!struct_type->is_union) {
                member_type = GET_STRUCT_TYPEDEF_MEMBER_BACK(struct_type->tag)->member_type.get();
                while (member_type->type() == AST_T::Array_t) {
                    member_type = static_cast<Array*>(member_type)->elem_type.get();
                }
                if (member_type->type() == AST_T::Structure_t) {
                    Structure* member_struct_type = static_cast<Structure*>(member_type);
                    struct_8b_clss(member_struct_type);
                    if (context->struct_8b_cls_map[member_struct_type->tag][0] == STRUCT_8B_CLS::INTEGER) {
                        struct_8b_cls[1] = STRUCT_8B_CLS::INTEGER;
                    }
                }
                else if (member_type->type() != AST_T::Double_t) {
                    struct_8b_cls[1] = STRUCT_8B_CLS::INTEGER;
                }
            }
        }
    }
    return struct_8b_cls;
}

static void struct_8b_clss(Structure* struct_type) {
    if (context->struct_8b_cls_map.find(struct_type->tag) == context->struct_8b_cls_map.end()) {
        std::vector<STRUCT_8B_CLS> struct_8b_cls;
        if (frontend->struct_typedef_table[struct_type->tag]->size > 16l) {
            struct_8b_cls = struct_mem_8b_clss(struct_type);
        }
        else if (frontend->struct_typedef_table[struct_type->tag]->size > 8l) {
            struct_8b_cls = struct_2_reg_8b_clss(struct_type);
        }
        else {
            struct_8b_cls = struct_1_reg_8b_clss(struct_type);
        }
        context->struct_8b_cls_map[struct_type->tag] = std::move(struct_8b_cls);
    }
}

static void param_fun_reg_mask(FunType* fun_type, size_t reg_size, size_t sse_size) {
    if (fun_type->param_reg_mask == NULL_REGISTER_MASK) {
        fun_type->param_reg_mask = REGISTER_MASK_FALSE;
        for (size_t i = 0; i < reg_size; ++i) {
            register_mask_set(fun_type->param_reg_mask, context->arg_registers[i], true);
        }
        for (size_t i = 0; i < sse_size; ++i) {
            register_mask_set(fun_type->param_reg_mask, context->sse_arg_registers[i], true);
        }
    }
}

static void ret_1_reg_mask(FunType* fun_type, bool reg_size) {
    if (fun_type->ret_reg_mask == NULL_REGISTER_MASK) {
        fun_type->ret_reg_mask = REGISTER_MASK_FALSE;
        register_mask_set(fun_type->ret_reg_mask, reg_size ? REGISTER_KIND::Ax : REGISTER_KIND::Xmm0, true);
    }
}

static void ret_2_reg_mask(FunType* fun_type, bool reg_size, bool sse_size) {
    if (fun_type->ret_reg_mask == NULL_REGISTER_MASK) {
        fun_type->ret_reg_mask = REGISTER_MASK_FALSE;
        if (reg_size) {
            register_mask_set(fun_type->ret_reg_mask, REGISTER_KIND::Ax, true);
            register_mask_set(fun_type->ret_reg_mask, sse_size ? REGISTER_KIND::Xmm0 : REGISTER_KIND::Dx, true);
        }
        else if (sse_size) {
            register_mask_set(fun_type->ret_reg_mask, REGISTER_KIND::Xmm0, true);
            register_mask_set(fun_type->ret_reg_mask, REGISTER_KIND::Xmm1, true);
        }
    }
}

static void push_instr(std::unique_ptr<AsmInstruction>&& instruction) {
    context->p_instructions->push_back(std::move(instruction));
}

static void ret_int_instr(TacReturn* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->val.get());
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_val = gen_asm_type(node->val.get());
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_val), std::move(src), std::move(dst)));
    ret_1_reg_mask(context->p_fun_type_top_level, true);
}

static void ret_dbl_instr(TacReturn* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->val.get());
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Xmm0);
    std::shared_ptr<AssemblyType> assembly_type_val = std::make_shared<BackendDouble>();
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_val), std::move(src), std::move(dst)));
    ret_1_reg_mask(context->p_fun_type_top_level, false);
}

static void ret_8b_instr(TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_register) {
    TIdentifier src_name = name;
    std::shared_ptr<AsmOperand> dst = generate_register(arg_register);
    std::shared_ptr<AssemblyType> assembly_type_src =
        struct_type ? asm_type_8b(struct_type, offset) : std::make_shared<BackendDouble>();
    if (assembly_type_src->type() == AST_T::ByteArray_t) {
        TLong size = offset + 2l;
        offset += static_cast<ByteArray*>(assembly_type_src.get())->size - 1l;
        assembly_type_src = std::make_shared<Byte>();
        std::shared_ptr<AsmOperand> src_shl = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AssemblyType> assembly_type_shl = std::make_shared<QuadWord>();
        while (offset >= size) {
            {
                std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
                push_instr(std::make_unique<AsmMov>(assembly_type_src, std::move(src), dst));
            }
            {
                std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitShiftLeft>();
                push_instr(std::make_unique<AsmBinary>(std::move(binary_op), assembly_type_shl, src_shl, dst));
            }
            offset--;
        }
        {
            std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
            push_instr(std::make_unique<AsmMov>(assembly_type_src, std::move(src), dst));
        }
        {
            std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitShiftLeft>();
            push_instr(std::make_unique<AsmBinary>(
                std::move(binary_op), std::move(assembly_type_shl), std::move(src_shl), dst));
        }
        offset--;
        {
            std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(offset));
            push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
        }
    }
    else {
        std::shared_ptr<AsmOperand> src;
        {
            TLong from_offset = offset;
            src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
        }
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
}

static void ret_struct_instr(TacReturn* node) {
    TIdentifier name = static_cast<TacVariable*>(node->val.get())->name;
    Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
    struct_8b_clss(struct_type);
    if (context->struct_8b_cls_map[struct_type->tag][0] == STRUCT_8B_CLS::MEMORY) {
        {
            std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Bp, -8l);
            std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
            std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
            push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
            ret_1_reg_mask(context->p_fun_type_top_level, true);
        }
        {
            TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
            TLong offset = 0l;
            while (size > 0l) {
                std::shared_ptr<AsmOperand> src = gen_op(node->val.get());
                if (src->type() != AST_T::AsmPseudoMem_t) {
                    RAISE_INTERNAL_ERROR;
                }
                static_cast<AsmPseudoMem*>(src.get())->offset = offset;
                std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Ax, offset);
                std::shared_ptr<AssemblyType> assembly_type_src;
                if (size >= 8l) {
                    assembly_type_src = std::make_shared<QuadWord>();
                    size -= 8l;
                    offset += 8l;
                }
                else if (size >= 4l) {
                    assembly_type_src = std::make_shared<LongWord>();
                    size -= 4l;
                    offset += 4l;
                }
                else {
                    assembly_type_src = std::make_shared<Byte>();
                    size--;
                    offset++;
                }
                push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
            }
        }
    }
    else {
        bool reg_size = false;
        switch (context->struct_8b_cls_map[struct_type->tag][0]) {
            case STRUCT_8B_CLS::INTEGER: {
                ret_8b_instr(name, 0l, struct_type, REGISTER_KIND::Ax);
                reg_size = true;
                break;
            }
            case STRUCT_8B_CLS::SSE:
                ret_8b_instr(name, 0l, nullptr, REGISTER_KIND::Xmm0);
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
        if (context->struct_8b_cls_map[struct_type->tag].size() == 2) {
            bool sse_size = !reg_size;
            switch (context->struct_8b_cls_map[struct_type->tag][1]) {
                case STRUCT_8B_CLS::INTEGER:
                    ret_8b_instr(name, 8l, struct_type, reg_size ? REGISTER_KIND::Dx : REGISTER_KIND::Ax);
                    break;
                case STRUCT_8B_CLS::SSE: {
                    ret_8b_instr(name, 8l, nullptr, sse_size ? REGISTER_KIND::Xmm1 : REGISTER_KIND::Xmm0);
                    sse_size = true;
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            ret_2_reg_mask(context->p_fun_type_top_level, reg_size, sse_size);
        }
        else {
            ret_1_reg_mask(context->p_fun_type_top_level, reg_size);
        }
    }
}

static void ret_instr(TacReturn* node) {
    if (node->val) {
        if (is_value_dbl(node->val.get())) {
            ret_dbl_instr(node);
        }
        else if (!is_value_struct(node->val.get())) {
            ret_int_instr(node);
        }
        else {
            ret_struct_instr(node);
        }
    }
    else {
        ret_2_reg_mask(context->p_fun_type_top_level, false, false);
    }
    push_instr(std::make_unique<AsmRet>());
}

static void sign_extend_instr(TacSignExtend* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->src.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node->dst.get());
    push_instr(std::make_unique<AsmMovSx>(
        std::move(assembly_type_src), std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void truncate_imm_byte_instr(AsmImm* node) {
    if (!node->is_byte) {
        node->value %= 256ul;
    }
}

static void truncate_byte_instr(TacTruncate* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<Byte>();
    if (src->type() == AST_T::AsmImm_t) {
        truncate_imm_byte_instr(static_cast<AsmImm*>(src.get()));
    }
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void truncate_imm_long_instr(AsmImm* node) {
    if (node->is_quad) {
        node->value -= 4294967296ul;
    }
}

static void truncate_long_instr(TacTruncate* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
    if (src->type() == AST_T::AsmImm_t) {
        truncate_imm_long_instr(static_cast<AsmImm*>(src.get()));
    }
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void truncate_instr(TacTruncate* node) {
    if (is_value_1b(node->dst.get())) {
        truncate_byte_instr(node);
    }
    else {
        truncate_long_instr(node);
    }
}

static void zero_extend_instr(TacZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->src.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node->dst.get());
    push_instr(std::make_unique<AsmMovZeroExtend>(
        std::move(assembly_type_src), std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void dbl_to_char_instr(TacDoubleToInt* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<LongWord>();
        push_instr(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src), std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<Byte>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void dbl_to_long_instr(TacDoubleToInt* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->dst.get());
    push_instr(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src), std::move(src), std::move(dst)));
}

static void dbl_to_signed_instr(TacDoubleToInt* node) {
    if (is_value_1b(node->dst.get())) {
        dbl_to_char_instr(node);
    }
    else {
        dbl_to_long_instr(node);
    }
}

static void dbl_to_uchar_instr(TacDoubleToUInt* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<LongWord>();
        push_instr(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src), std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<Byte>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void dbl_to_uint_instr(TacDoubleToUInt* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instr(std::make_unique<AsmCvttsd2si>(std::move(assembly_type_src), std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void dbl_to_ulong_instr(TacDoubleToUInt* node) {
    TIdentifier target_out_of_range = repr_asm_label(ASM_LABEL_KIND::Lsd2si_out_of_range);
    TIdentifier target_after = repr_asm_label(ASM_LABEL_KIND::Lsd2si_after);
    std::shared_ptr<AsmOperand> upper_bound_sd = dbl_static_const_op(4890909195324358656ul, 8);
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AsmOperand> dst_out_of_range_sd = generate_register(REGISTER_KIND::Xmm1);
    std::shared_ptr<AssemblyType> assembly_type_sd = std::make_shared<BackendDouble>();
    std::shared_ptr<AssemblyType> assembly_type_si = std::make_shared<QuadWord>();
    push_instr(std::make_unique<AsmCmp>(assembly_type_sd, upper_bound_sd, src));
    {
        std::unique_ptr<AsmCondCode> cond_code_ae = std::make_unique<AsmAE>();
        push_instr(std::make_unique<AsmJmpCC>(target_out_of_range, std::move(cond_code_ae)));
    }
    push_instr(std::make_unique<AsmCvttsd2si>(assembly_type_si, src, dst));
    push_instr(std::make_unique<AsmJmp>(target_after));
    push_instr(std::make_unique<AsmLabel>(std::move(target_out_of_range)));
    push_instr(std::make_unique<AsmMov>(assembly_type_sd, std::move(src), dst_out_of_range_sd));
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_sd_sub = std::make_unique<AsmSub>();
        push_instr(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_sd_sub), std::move(assembly_type_sd),
            std::move(upper_bound_sd), dst_out_of_range_sd));
    }
    push_instr(std::make_unique<AsmCvttsd2si>(assembly_type_si, std::move(dst_out_of_range_sd), dst));
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_si_add = std::make_unique<AsmAdd>();
        std::shared_ptr<AsmOperand> upper_bound_si =
            std::make_shared<AsmImm>(9223372036854775808ul, false, true, false);
        push_instr(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_si_add), std::move(assembly_type_si),
            std::move(upper_bound_si), std::move(dst)));
    }
    push_instr(std::make_unique<AsmLabel>(std::move(target_after)));
}

static void dbl_to_unsigned_instr(TacDoubleToUInt* node) {
    if (is_value_1b(node->dst.get())) {
        dbl_to_uchar_instr(node);
    }
    else if (is_value_4b(node->dst.get())) {
        dbl_to_uint_instr(node);
    }
    else {
        dbl_to_ulong_instr(node);
    }
}

static void char_to_dbl_instr(TacIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<Byte>();
        push_instr(
            std::make_unique<AsmMovSx>(std::move(assembly_type_src), assembly_type_dst, std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void long_to_dbl_instr(TacIntToDouble* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->src.get());
    push_instr(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_src), std::move(src), std::move(dst)));
}

static void signed_to_dbl_instr(TacIntToDouble* node) {
    if (is_value_1b(node->src.get())) {
        char_to_dbl_instr(node);
    }
    else {
        long_to_dbl_instr(node);
    }
}

static void uchar_to_dbl_instr(TacUIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<Byte>();
        push_instr(std::make_unique<AsmMovZeroExtend>(
            std::move(assembly_type_src), assembly_type_dst, std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void uint_to_dbl_instr(TacUIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<LongWord>();
        push_instr(std::make_unique<AsmMovZeroExtend>(
            std::move(assembly_type_src), assembly_type_dst, std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void ulong_to_dbl_instr(TacUIntToDouble* node) {
    TIdentifier target_out_of_range = repr_asm_label(ASM_LABEL_KIND::Lsi2sd_out_of_range);
    TIdentifier target_after = repr_asm_label(ASM_LABEL_KIND::Lsi2sd_after);
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AsmOperand> dst_out_of_range_si = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AsmOperand> dst_out_of_range_si_shr = generate_register(REGISTER_KIND::Dx);
    std::shared_ptr<AssemblyType> assembly_type_si = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> lower_bound_si = std::make_shared<AsmImm>(0ul, true, false, false);
        push_instr(std::make_unique<AsmCmp>(assembly_type_si, std::move(lower_bound_si), src));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_l = std::make_unique<AsmL>();
        push_instr(std::make_unique<AsmJmpCC>(target_out_of_range, std::move(cond_code_l)));
    }
    push_instr(std::make_unique<AsmCvtsi2sd>(assembly_type_si, src, dst));
    push_instr(std::make_unique<AsmJmp>(target_after));
    push_instr(std::make_unique<AsmLabel>(std::move(target_out_of_range)));
    push_instr(std::make_unique<AsmMov>(assembly_type_si, std::move(src), dst_out_of_range_si));
    push_instr(std::make_unique<AsmMov>(assembly_type_si, dst_out_of_range_si, dst_out_of_range_si_shr));
    {
        std::unique_ptr<AsmUnaryOp> unary_op_out_of_range_si_shr = std::make_unique<AsmShr>();
        push_instr(std::make_unique<AsmUnary>(
            std::move(unary_op_out_of_range_si_shr), assembly_type_si, dst_out_of_range_si_shr));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_si_and = std::make_unique<AsmBitAnd>();
        std::shared_ptr<AsmOperand> set_bit_si = std::make_shared<AsmImm>(1ul, true, false, false);
        push_instr(std::make_unique<AsmBinary>(
            std::move(binary_op_out_of_range_si_and), assembly_type_si, std::move(set_bit_si), dst_out_of_range_si));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_si_or = std::make_unique<AsmBitOr>();
        push_instr(std::make_unique<AsmBinary>(std::move(binary_op_out_of_range_si_or), assembly_type_si,
            std::move(dst_out_of_range_si), dst_out_of_range_si_shr));
    }
    push_instr(std::make_unique<AsmCvtsi2sd>(std::move(assembly_type_si), std::move(dst_out_of_range_si_shr), dst));
    {
        std::unique_ptr<AsmBinaryOp> binary_op_out_of_range_sq_add = std::make_unique<AsmAdd>();
        std::shared_ptr<AssemblyType> assembly_type_sq = std::make_shared<BackendDouble>();
        push_instr(std::make_unique<AsmBinary>(
            std::move(binary_op_out_of_range_sq_add), std::move(assembly_type_sq), dst, dst));
    }
    push_instr(std::make_unique<AsmLabel>(std::move(target_after)));
}

static void unsigned_to_double_instr(TacUIntToDouble* node) {
    if (is_value_1b(node->src.get())) {
        uchar_to_dbl_instr(node);
    }
    else if (is_value_4b(node->src.get())) {
        uint_to_dbl_instr(node);
    }
    else {
        ulong_to_dbl_instr(node);
    }
}

static void alloc_stack_instr(TLong byte) { push_instr(allocate_stack_bytes(byte)); }

static void dealloc_stack_instr(TLong byte) { push_instr(deallocate_stack_bytes(byte)); }

static void reg_arg_call_instr(TacValue* node, REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = gen_op(node);
    std::shared_ptr<AsmOperand> dst = generate_register(arg_register);
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node);
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
}

static void stack_arg_call_instr(TacValue* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node);
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node);
    if (src->type() == AST_T::AsmRegister_t || src->type() == AST_T::AsmImm_t
        || assembly_type_src->type() == AST_T::QuadWord_t || assembly_type_src->type() == AST_T::BackendDouble_t) {
        push_instr(std::make_unique<AsmPush>(std::move(src)));
    }
    else {
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        push_instr(std::make_unique<AsmPush>(dst));
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
}

static void reg_8b_arg_call_instr(TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_register) {
    ret_8b_instr(name, offset, struct_type, arg_register);
}

static void quad_stack_arg_call_instr(TIdentifier name, TLong offset) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
    }
    push_instr(std::make_unique<AsmPush>(std::move(src)));
}

static void long_stack_arg_call_instr(TIdentifier name, TLong offset, std::shared_ptr<AssemblyType>&& assembly_type) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
    }
    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_src = std::move(assembly_type);
    push_instr(std::make_unique<AsmPush>(dst));
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
}

static void bytearr_stack_arg_call_instr(TIdentifier name, TLong offset, ByteArray* bytearray_type) {
    {
        TLong to_offset = 0l;
        TLong size = bytearray_type->size;
        std::vector<std::unique_ptr<AsmInstruction>> byte_instructions;
        while (size > 0l) {
            std::unique_ptr<AsmInstruction> byte_instruction;
            {
                std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(name, offset);
                std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Sp, to_offset);
                std::shared_ptr<AssemblyType> assembly_type_src;
                if (size >= 4l) {
                    assembly_type_src = std::make_shared<LongWord>();
                    size -= 4l;
                    offset += 4l;
                    to_offset += 4l;
                }
                else {
                    assembly_type_src = std::make_shared<Byte>();
                    size--;
                    offset++;
                    to_offset++;
                }
                byte_instruction =
                    std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst));
            }
            byte_instructions.push_back(std::move(byte_instruction));
        }
        for (size_t i = byte_instructions.size(); i-- > 0;) {
            push_instr(std::move(byte_instructions[i]));
        }
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmSub>();
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Sp);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instr(std::make_unique<AsmBinary>(
            std::move(binary_op), std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
}

static void stack_8b_arg_call_instr(TIdentifier name, TLong offset, Structure* struct_type) {
    std::shared_ptr<AssemblyType> assembly_type = asm_type_8b(struct_type, offset);
    switch (assembly_type->type()) {
        case AST_T::QuadWord_t:
            quad_stack_arg_call_instr(name, offset);
            break;
        case AST_T::ByteArray_t:
            bytearr_stack_arg_call_instr(name, offset, static_cast<ByteArray*>(assembly_type.get()));
            break;
        default:
            long_stack_arg_call_instr(name, offset, std::move(assembly_type));
            break;
    }
}

static TLong arg_call_instr(TacFunCall* node, FunType* fun_type, bool is_return_memory) {
    size_t reg_size = is_return_memory ? 1 : 0;
    size_t sse_size = 0;
    TLong stack_padding = 0l;
    std::vector<std::unique_ptr<AsmInstruction>> stack_instructions;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instructions = context->p_instructions;
    for (const auto& arg : node->args) {
        if (is_value_dbl(arg.get())) {
            if (sse_size < 8) {
                reg_arg_call_instr(arg.get(), context->sse_arg_registers[sse_size]);
                sse_size++;
            }
            else {
                context->p_instructions = &stack_instructions;
                stack_arg_call_instr(arg.get());
                context->p_instructions = p_instructions;
                stack_padding++;
            }
        }
        else if (!is_value_struct(arg.get())) {
            if (reg_size < 6) {
                reg_arg_call_instr(arg.get(), context->arg_registers[reg_size]);
                reg_size++;
            }
            else {
                context->p_instructions = &stack_instructions;
                stack_arg_call_instr(arg.get());
                context->p_instructions = p_instructions;
                stack_padding++;
            }
        }
        else {
            size_t struct_reg_size = 7;
            size_t struct_sse_size = 9;
            TIdentifier name = static_cast<TacVariable*>(arg.get())->name;
            Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
            struct_8b_clss(struct_type);
            if (context->struct_8b_cls_map[struct_type->tag][0] != STRUCT_8B_CLS::MEMORY) {
                struct_reg_size = 0;
                struct_sse_size = 0;
                for (STRUCT_8B_CLS struct_8b_cls : context->struct_8b_cls_map[struct_type->tag]) {
                    if (struct_8b_cls == STRUCT_8B_CLS::SSE) {
                        struct_sse_size++;
                    }
                    else {
                        struct_reg_size++;
                    }
                }
            }
            if (struct_reg_size + reg_size <= 6 && struct_sse_size + sse_size <= 8) {
                TLong offset = 0l;
                for (STRUCT_8B_CLS struct_8b_cls : context->struct_8b_cls_map[struct_type->tag]) {
                    if (struct_8b_cls == STRUCT_8B_CLS::SSE) {
                        reg_8b_arg_call_instr(name, offset, nullptr, context->sse_arg_registers[sse_size]);
                        sse_size++;
                    }
                    else {
                        reg_8b_arg_call_instr(name, offset, struct_type, context->arg_registers[reg_size]);
                        reg_size++;
                    }
                    offset += 8l;
                }
            }
            else {
                TLong offset = 0l;
                context->p_instructions = &stack_instructions;
                for (size_t i = 0; i < context->struct_8b_cls_map[struct_type->tag].size(); ++i) {
                    stack_8b_arg_call_instr(name, offset, struct_type);
                    offset += 8l;
                    stack_padding++;
                }
                context->p_instructions = p_instructions;
            }
        }
    }
    param_fun_reg_mask(fun_type, reg_size, sse_size);
    if (stack_padding % 2l == 1l) {
        alloc_stack_instr(8l);
        stack_padding++;
    }
    stack_padding *= 8l;
    for (size_t i = stack_instructions.size(); i-- > 0;) {
        push_instr(std::move(stack_instructions[i]));
    }
    return stack_padding;
}

static void ret_call_instr(TacValue* node, REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = generate_register(arg_register);
    std::shared_ptr<AsmOperand> dst = gen_op(node);
    std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node);
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void ret_8b_call_instr(TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_register) {
    TIdentifier dst_name = name;
    std::shared_ptr<AsmOperand> src = generate_register(arg_register);
    std::shared_ptr<AssemblyType> assembly_type_dst =
        struct_type ? asm_type_8b(struct_type, offset) : std::make_shared<BackendDouble>();
    if (assembly_type_dst->type() == AST_T::ByteArray_t) {
        TLong size = static_cast<ByteArray*>(assembly_type_dst.get())->size + offset - 2l;
        assembly_type_dst = std::make_shared<Byte>();
        std::shared_ptr<AsmOperand> src_shr2op = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AssemblyType> assembly_type_shr2op = std::make_shared<QuadWord>();
        while (offset < size) {
            {
                std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
                push_instr(std::make_unique<AsmMov>(assembly_type_dst, src, std::move(dst)));
            }
            {
                std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitShiftRight>();
                push_instr(std::make_unique<AsmBinary>(std::move(binary_op), assembly_type_shr2op, src_shr2op, src));
            }
            offset++;
        }
        {
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
            push_instr(std::make_unique<AsmMov>(assembly_type_dst, src, std::move(dst)));
        }
        {
            std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitShiftRight>();
            push_instr(std::make_unique<AsmBinary>(
                std::move(binary_op), std::move(assembly_type_shr2op), std::move(src_shr2op), src));
        }
        offset++;
        {
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(std::move(dst_name), std::move(offset));
            push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
        }
    }
    else {
        std::shared_ptr<AsmOperand> dst;
        {
            TLong to_offset = offset;
            dst = std::make_shared<AsmPseudoMem>(std::move(dst_name), std::move(to_offset));
        }
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
    }
}

static void call_instr(TacFunCall* node) {
    bool is_return_memory = false;
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    {
        if (node->dst && is_value_struct(node->dst.get())) {
            TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
            Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
            struct_8b_clss(struct_type);
            if (context->struct_8b_cls_map[struct_type->tag][0] == STRUCT_8B_CLS::MEMORY) {
                is_return_memory = true;
                {
                    std::shared_ptr<AsmOperand> src = gen_op(node->dst.get());
                    std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Di);
                    push_instr(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
                }
            }
        }
        TLong stack_padding = arg_call_instr(node, fun_type, is_return_memory);

        {
            TIdentifier name = node->name;
            push_instr(std::make_unique<AsmCall>(std::move(name)));
        }

        if (stack_padding > 0l) {
            dealloc_stack_instr(stack_padding);
        }
    }

    if (node->dst) {
        if (is_return_memory) {
            ret_1_reg_mask(fun_type, true);
        }
        else {
            if (is_value_dbl(node->dst.get())) {
                ret_call_instr(node->dst.get(), REGISTER_KIND::Xmm0);
                ret_1_reg_mask(fun_type, false);
            }
            else if (!is_value_struct(node->dst.get())) {
                ret_call_instr(node->dst.get(), REGISTER_KIND::Ax);
                ret_1_reg_mask(fun_type, true);
            }
            else {
                bool reg_size = false;
                TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
                Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
                switch (context->struct_8b_cls_map[struct_type->tag][0]) {
                    case STRUCT_8B_CLS::INTEGER: {
                        ret_8b_call_instr(name, 0l, struct_type, REGISTER_KIND::Ax);
                        reg_size = true;
                        break;
                    }
                    case STRUCT_8B_CLS::SSE:
                        ret_8b_call_instr(name, 0l, nullptr, REGISTER_KIND::Xmm0);
                        break;
                    default:
                        RAISE_INTERNAL_ERROR;
                }
                if (context->struct_8b_cls_map[struct_type->tag].size() == 2) {
                    bool sse_size = !reg_size;
                    switch (context->struct_8b_cls_map[struct_type->tag][1]) {
                        case STRUCT_8B_CLS::INTEGER:
                            ret_8b_call_instr(name, 8l, struct_type, reg_size ? REGISTER_KIND::Dx : REGISTER_KIND::Ax);
                            break;
                        case STRUCT_8B_CLS::SSE: {
                            ret_8b_call_instr(name, 8l, nullptr, sse_size ? REGISTER_KIND::Xmm1 : REGISTER_KIND::Xmm0);
                            sse_size = true;
                            break;
                        }
                        default:
                            RAISE_INTERNAL_ERROR;
                    }
                    ret_2_reg_mask(fun_type, reg_size, sse_size);
                }
                else {
                    ret_1_reg_mask(fun_type, reg_size);
                }
            }
        }
    }
    else {
        ret_2_reg_mask(fun_type, false, false);
    }
}

static void zero_xmm_reg_instr() {
    std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitXor>();
    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::Xmm0);
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<BackendDouble>();
    push_instr(std::make_unique<AsmBinary>(std::move(binary_op), std::move(assembly_type_src), src, src));
}

static void unop_int_arithmetic_instr(TacUnary* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->src.get());
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        push_instr(std::make_unique<AsmMov>(assembly_type_src, std::move(src), src_dst));
    }
    {
        std::unique_ptr<AsmUnaryOp> unary_op = gen_unop(node->unary_op.get());
        push_instr(std::make_unique<AsmUnary>(std::move(unary_op), std::move(assembly_type_src), std::move(src_dst)));
    }
}

static void unop_dbl_neg_instr(TacUnary* node) {
    std::shared_ptr<AsmOperand> src1_dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src1 = std::make_shared<BackendDouble>();
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src.get());
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1), src1_dst));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmBitXor>();
        std::shared_ptr<AsmOperand> src2 = dbl_static_const_op(9223372036854775808ul, 16);
        push_instr(std::make_unique<AsmBinary>(
            std::move(binary_op), std::move(assembly_type_src1), std::move(src2), std::move(src1_dst)));
    }
}

static void unop_neg_instr(TacUnary* node) {
    if (is_value_dbl(node->src.get())) {
        unop_dbl_neg_instr(node);
    }
    else {
        unop_int_arithmetic_instr(node);
    }
}

static void unop_int_conditional_instr(TacUnary* node) {
    std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(node->dst.get());
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->src.get());
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_src), imm_zero, std::move(src)));
    }
    {
        std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node->dst.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
    }
}

static void unop_dbl_conditional_instr(TacUnary* node) {
    TIdentifier target_nan = repr_asm_label(ASM_LABEL_KIND::Lcomisd_nan);
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(node->dst.get());
    zero_xmm_reg_instr();
    {
        std::shared_ptr<AsmOperand> reg_zero = generate_register(REGISTER_KIND::Xmm0);
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<BackendDouble>();
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_src), std::move(reg_zero), std::move(src)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
    }
    push_instr(std::make_unique<AsmLabel>(std::move(target_nan)));
}

static void unop_conditional_instr(TacUnary* node) {
    if (is_value_dbl(node->src.get())) {
        unop_dbl_conditional_instr(node);
    }
    else {
        unop_int_conditional_instr(node);
    }
}

static void unary_instr(TacUnary* node) {
    switch (node->unary_op->type()) {
        case AST_T::TacComplement_t:
            unop_int_arithmetic_instr(node);
            break;
        case AST_T::TacNegate_t:
            unop_neg_instr(node);
            break;
        case AST_T::TacNot_t:
            unop_conditional_instr(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void binop_arithmetic_instr(TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src1 = gen_asm_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src1.get());
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1), src1_dst));
    }
    {
        std::unique_ptr<AsmBinaryOp> binary_op = gen_binop(node->binary_op.get());
        std::shared_ptr<AsmOperand> src2 = gen_op(node->src2.get());
        push_instr(std::make_unique<AsmBinary>(
            std::move(binary_op), std::move(assembly_type_src1), std::move(src2), std::move(src1_dst)));
    }
}

static void signed_divide_instr(TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_src1 = gen_asm_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src1.get());
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1), src1_dst));
    }
    push_instr(std::make_unique<AsmCdq>(assembly_type_src1));
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(node->src2.get());
        push_instr(std::make_unique<AsmIdiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src1), std::move(src1_dst), std::move(dst)));
    }
}

static void unsigned_divide_instr(TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
    std::shared_ptr<AssemblyType> assembly_type_src1 = gen_asm_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src1.get());
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1), src1_dst));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> imm_zero_dst = generate_register(REGISTER_KIND::Dx);
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(imm_zero), std::move(imm_zero_dst)));
    }
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(node->src2.get());
        push_instr(std::make_unique<AsmDiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src1), std::move(src1_dst), std::move(dst)));
    }
}

static void binop_divide_instr(TacBinary* node) {
    if (is_value_dbl(node->src1.get())) {
        binop_arithmetic_instr(node);
    }
    else if (is_value_signed(node->src1.get())) {
        signed_divide_instr(node);
    }
    else {
        unsigned_divide_instr(node);
    }
}

static void signed_remainder_instr(TacBinary* node) {
    std::shared_ptr<AssemblyType> assembly_type_src1 = gen_asm_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1), std::move(src1_dst)));
    }
    push_instr(std::make_unique<AsmCdq>(assembly_type_src1));
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(node->src2.get());
        push_instr(std::make_unique<AsmIdiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst_src = generate_register(REGISTER_KIND::Dx);
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src1), std::move(dst_src), std::move(dst)));
    }
}

static void unsigned_remainder_instr(TacBinary* node) {
    std::shared_ptr<AsmOperand> dst_src = generate_register(REGISTER_KIND::Dx);
    std::shared_ptr<AssemblyType> assembly_type_src1 = gen_asm_type(node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst = generate_register(REGISTER_KIND::Ax);
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(src1), std::move(src1_dst)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        push_instr(std::make_unique<AsmMov>(assembly_type_src1, std::move(imm_zero), dst_src));
    }
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(node->src2.get());
        push_instr(std::make_unique<AsmDiv>(assembly_type_src1, std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src1), std::move(dst_src), std::move(dst)));
    }
}

static void binop_remainder_instr(TacBinary* node) {
    if (is_value_signed(node->src1.get())) {
        signed_remainder_instr(node);
    }
    else {
        unsigned_remainder_instr(node);
    }
}

static void binop_int_conditional_intrs(TacBinary* node) {
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(node->dst.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src1.get());
        std::shared_ptr<AsmOperand> src2 = gen_op(node->src2.get());
        std::shared_ptr<AssemblyType> assembly_type_src1 = gen_asm_type(node->src1.get());
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_src1), std::move(src2), std::move(src1)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node->dst.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code;
        if (is_value_signed(node->src1.get())) {
            cond_code = signed_cond_code(node->binary_op.get());
        }
        else {
            cond_code = unsigned_cond_code(node->binary_op.get());
        }
        push_instr(std::make_unique<AsmSetCC>(std::move(cond_code), std::move(cmp_dst)));
    }
}

static void binop_dbl_conditional_intrs(TacBinary* node) {
    TIdentifier target_nan = repr_asm_label(ASM_LABEL_KIND::Lcomisd_nan);
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(node->dst.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(node->src1.get());
        std::shared_ptr<AsmOperand> src2 = gen_op(node->src2.get());
        std::shared_ptr<AssemblyType> assembly_type_src1 = gen_asm_type(node->src1.get());
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_src1), std::move(src2), std::move(src1)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<LongWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(imm_zero), cmp_dst));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code = unsigned_cond_code(node->binary_op.get());
        if (cond_code->type() == AST_T::AsmNE_t) {
            TIdentifier target_nan_ne = repr_asm_label(ASM_LABEL_KIND::Lcomisd_nan);
            push_instr(std::make_unique<AsmSetCC>(std::move(cond_code), cmp_dst));
            push_instr(std::make_unique<AsmJmp>(target_nan_ne));
            push_instr(std::make_unique<AsmLabel>(std::move(target_nan)));
            {
                std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
                push_instr(std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
            }
            push_instr(std::make_unique<AsmLabel>(std::move(target_nan_ne)));
        }
        else {
            push_instr(std::make_unique<AsmSetCC>(std::move(cond_code), std::move(cmp_dst)));
            push_instr(std::make_unique<AsmLabel>(std::move(target_nan)));
        }
    }
}

static void binop_conditional_intrs(TacBinary* node) {
    if (is_value_dbl(node->src1.get())) {
        binop_dbl_conditional_intrs(node);
    }
    else {
        binop_int_conditional_intrs(node);
    }
}

static void binary_instr(TacBinary* node) {
    switch (node->binary_op->type()) {
        case AST_T::TacAdd_t:
        case AST_T::TacSubtract_t:
        case AST_T::TacMultiply_t:
        case AST_T::TacBitAnd_t:
        case AST_T::TacBitOr_t:
        case AST_T::TacBitXor_t:
        case AST_T::TacBitShiftLeft_t:
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t:
            binop_arithmetic_instr(node);
            break;
        case AST_T::TacDivide_t:
            binop_divide_instr(node);
            break;
        case AST_T::TacRemainder_t:
            binop_remainder_instr(node);
            break;
        case AST_T::TacEqual_t:
        case AST_T::TacNotEqual_t:
        case AST_T::TacLessThan_t:
        case AST_T::TacLessOrEqual_t:
        case AST_T::TacGreaterThan_t:
        case AST_T::TacGreaterOrEqual_t:
            binop_conditional_intrs(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void copy_struct_instr(TacCopy* node) {
    TIdentifier src_name = static_cast<TacVariable*>(node->src.get())->name;
    TIdentifier dst_name = static_cast<TacVariable*>(node->dst.get())->name;
    Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[src_name]->type_t.get());
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
    TLong offset = 0l;
    while (size > 0l) {
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
        std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
        std::shared_ptr<AssemblyType> assembly_type_src;
        if (size >= 8l) {
            assembly_type_src = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            assembly_type_src = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            assembly_type_src = std::make_shared<Byte>();
            size--;
            offset++;
        }
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
}

static void copy_scalar_instr(TacCopy* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->src.get());
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
}

static void copy_instr(TacCopy* node) {
    if (is_value_struct(node->src.get())) {
        copy_struct_instr(node);
    }
    else {
        copy_scalar_instr(node);
    }
}

static void getaddr_instr(TacGetAddress* node) {
    std::shared_ptr<AsmOperand> src;
    {
        if (node->src->type() == AST_T::TacVariable_t) {
            TIdentifier name = static_cast<TacVariable*>(node->src.get())->name;
            frontend->addressed_set.insert(name);
            if (frontend->symbol_table.find(name) != frontend->symbol_table.end()
                && frontend->symbol_table[name]->attrs->type() == AST_T::ConstantAttr_t) {
                src = std::make_shared<AsmData>(std::move(name), 0l);
                goto Lpass;
            }
        }
        src = gen_op(node->src.get());
    Lpass:;
    }
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    push_instr(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
}

static void load_struct_instr(TacLoad* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
    {
        TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
        Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
        TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
        TLong offset = 0l;
        while (size > 0l) {
            std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Ax, offset);
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(name, offset);
            std::shared_ptr<AssemblyType> assembly_type_dst;
            if (size >= 8l) {
                assembly_type_dst = std::make_shared<QuadWord>();
                size -= 8l;
                offset += 8l;
            }
            else if (size >= 4l) {
                assembly_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
            }
            else {
                assembly_type_dst = std::make_shared<Byte>();
                size--;
                offset++;
            }
            push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
        }
    }
}

static void load_scalar_instr(TacLoad* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Ax, 0l);
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node->dst.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
    }
}

static void load_instr(TacLoad* node) {
    if (is_value_struct(node->dst.get())) {
        load_struct_instr(node);
    }
    else {
        load_scalar_instr(node);
    }
}

static void store_struct_instr(TacStore* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->dst_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
    {
        TIdentifier name = static_cast<TacVariable*>(node->src.get())->name;
        Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[name]->type_t.get());
        TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
        TLong offset = 0l;
        while (size > 0l) {
            std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(name, offset);
            std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Ax, offset);
            std::shared_ptr<AssemblyType> assembly_type_dst;
            if (size >= 8l) {
                assembly_type_dst = std::make_shared<QuadWord>();
                size -= 8l;
                offset += 8l;
            }
            else if (size >= 4l) {
                assembly_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
            }
            else {
                assembly_type_dst = std::make_shared<Byte>();
                size--;
                offset++;
            }
            push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
        }
    }
}

static void store_scalar_instr(TacStore* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->dst_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
        std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Ax, 0l);
        std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node->src.get());
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
    }
}

static void store_instr(TacStore* node) {
    if (is_value_struct(node->src.get())) {
        store_struct_instr(node);
    }
    else {
        store_scalar_instr(node);
    }
}

static void const_idx_add_ptr_instr(TacAddPtr* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
    {
        TLong index;
        {
            CConst* constant = static_cast<TacConstant*>(node->index.get())->constant.get();
            if (constant->type() != AST_T::CConstLong_t) {
                RAISE_INTERNAL_ERROR;
            }
            index = static_cast<CConstLong*>(constant)->value;
        }
        std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Ax, index * node->scale);
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void scalar_idx_add_ptr_instr(TacAddPtr* node) {
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        push_instr(std::make_unique<AsmMov>(assembly_type_src, std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->index.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Dx);
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_indexed(REGISTER_KIND::Ax, REGISTER_KIND::Dx, node->scale);
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void aggr_idx_add_ptr_instr(TacAddPtr* node) {
    std::shared_ptr<AssemblyType> assembly_type_src = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src_dst = generate_register(REGISTER_KIND::Dx);
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = generate_register(REGISTER_KIND::Ax);
        push_instr(std::make_unique<AsmMov>(assembly_type_src, std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_op(node->index.get());
        push_instr(std::make_unique<AsmMov>(assembly_type_src, std::move(src), src_dst));
    }
    {
        std::shared_ptr<AsmOperand> src;
        {
            TULong value = static_cast<TULong>(node->scale);
            bool is_byte = node->scale <= 127l && node->scale >= -128l;
            bool is_quad = node->scale > 2147483647l || node->scale < -2147483648l;
            bool is_neg = node->scale < 0l;
            src = std::make_shared<AsmImm>(std::move(value), std::move(is_byte), std::move(is_quad), std::move(is_neg));
        }
        std::unique_ptr<AsmBinaryOp> binary_op = std::make_unique<AsmMult>();
        push_instr(std::make_unique<AsmBinary>(
            std::move(binary_op), std::move(assembly_type_src), std::move(src), std::move(src_dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = generate_indexed(REGISTER_KIND::Ax, REGISTER_KIND::Dx, 1l);
        std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
        push_instr(std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void var_idx_add_ptr_instr(TacAddPtr* node) {
    switch (node->scale) {
        case 1l:
        case 2l:
        case 4l:
        case 8l:
            scalar_idx_add_ptr_instr(node);
            break;
        default:
            aggr_idx_add_ptr_instr(node);
            break;
    }
}

static void add_ptr_instr(TacAddPtr* node) {
    switch (node->index->type()) {
        case AST_T::TacConstant_t:
            const_idx_add_ptr_instr(node);
            break;
        case AST_T::TacVariable_t:
            var_idx_add_ptr_instr(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void cp_to_offset_struct_instr(TacCopyToOffset* node) {
    TIdentifier src_name = static_cast<TacVariable*>(node->src.get())->name;
    Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[src_name]->type_t.get());
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
    TLong offset = 0l;
    while (size > 0l) {
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
        std::shared_ptr<AsmOperand> dst;
        {
            TIdentifier dst_name = node->dst_name;
            TLong to_offset = offset + node->offset;
            dst = std::make_shared<AsmPseudoMem>(std::move(dst_name), std::move(to_offset));
        }
        std::shared_ptr<AssemblyType> assembly_type_src;
        if (size >= 8l) {
            assembly_type_src = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            assembly_type_src = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            assembly_type_src = std::make_shared<Byte>();
            size--;
            offset++;
        }
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
    }
}

static void cp_to_offset_scalar_instr(TacCopyToOffset* node) {
    std::shared_ptr<AsmOperand> src = gen_op(node->src.get());
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = node->dst_name;
        TLong to_offset = node->offset;
        dst = std::make_shared<AsmPseudoMem>(std::move(dst_name), std::move(to_offset));
    }
    std::shared_ptr<AssemblyType> assembly_type_src = gen_asm_type(node->src.get());
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_src), std::move(src), std::move(dst)));
}

static void cp_to_offset_instr(TacCopyToOffset* node) {
    if (is_value_struct(node->src.get())) {
        cp_to_offset_struct_instr(node);
    }
    else {
        cp_to_offset_scalar_instr(node);
    }
}

static void cp_from_offset_struct_instr(TacCopyFromOffset* node) {
    TIdentifier dst_name = static_cast<TacVariable*>(node->dst.get())->name;
    Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[dst_name]->type_t.get());
    TLong size = frontend->struct_typedef_table[struct_type->tag]->size;
    TLong offset = 0l;
    while (size > 0l) {
        std::shared_ptr<AsmOperand> src;
        {
            TIdentifier src_name = node->src_name;
            TLong from_offset = offset + node->offset;
            src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
        }
        std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
        std::shared_ptr<AssemblyType> assembly_type_dst;
        if (size >= 8l) {
            assembly_type_dst = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            assembly_type_dst = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            assembly_type_dst = std::make_shared<Byte>();
            size--;
            offset++;
        }
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
    }
}

static void cp_from_offset_scalar_instr(TacCopyFromOffset* node) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = node->src_name;
        TLong from_offset = node->offset;
        src = std::make_shared<AsmPseudoMem>(std::move(src_name), std::move(from_offset));
    }
    std::shared_ptr<AsmOperand> dst = gen_op(node->dst.get());
    std::shared_ptr<AssemblyType> assembly_type_dst = gen_asm_type(node->dst.get());
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void cp_from_offset_instr(TacCopyFromOffset* node) {
    if (is_value_struct(node->dst.get())) {
        cp_from_offset_struct_instr(node);
    }
    else {
        cp_from_offset_scalar_instr(node);
    }
}

static void jmp_instr(TacJump* node) {
    TIdentifier target = node->target;
    push_instr(std::make_unique<AsmJmp>(std::move(target)));
}

static void jmp_eq_0_int_instr(TacJumpIfZero* node) {
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> condition = gen_op(node->condition.get());
        std::shared_ptr<AssemblyType> assembly_type_cond = gen_asm_type(node->condition.get());
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_cond), std::move(imm_zero), std::move(condition)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_e)));
    }
}

static void jmp_eq_0_dbl_instr(TacJumpIfZero* node) {
    TIdentifier target_nan = repr_asm_label(ASM_LABEL_KIND::Lcomisd_nan);
    zero_xmm_reg_instr();
    {
        std::shared_ptr<AsmOperand> condition = gen_op(node->condition.get());
        std::shared_ptr<AsmOperand> reg_zero = generate_register(REGISTER_KIND::Xmm0);
        std::shared_ptr<AssemblyType> assembly_type_cond = std::make_shared<BackendDouble>();
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_cond), std::move(condition), std::move(reg_zero)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_e)));
    }
    push_instr(std::make_unique<AsmLabel>(std::move(target_nan)));
}

static void jmp_eq_0_instr(TacJumpIfZero* node) {
    if (is_value_dbl(node->condition.get())) {
        jmp_eq_0_dbl_instr(node);
    }
    else {
        jmp_eq_0_int_instr(node);
    }
}

static void jmp_ne_0_int_instr(TacJumpIfNotZero* node) {
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> condition = gen_op(node->condition.get());
        std::shared_ptr<AssemblyType> assembly_type_cond = gen_asm_type(node->condition.get());
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_cond), std::move(imm_zero), std::move(condition)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_ne = std::make_unique<AsmNE>();
        push_instr(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_ne)));
    }
}

static void jmp_ne_0_dbl_instr(TacJumpIfNotZero* node) {
    TIdentifier target = node->target;
    TIdentifier target_nan = repr_asm_label(ASM_LABEL_KIND::Lcomisd_nan);
    TIdentifier target_nan_ne = repr_asm_label(ASM_LABEL_KIND::Lcomisd_nan);
    zero_xmm_reg_instr();
    {
        std::shared_ptr<AsmOperand> condition = gen_op(node->condition.get());
        std::shared_ptr<AsmOperand> reg_zero = generate_register(REGISTER_KIND::Xmm0);
        std::shared_ptr<AssemblyType> assembly_type_cond = std::make_shared<BackendDouble>();
        push_instr(std::make_unique<AsmCmp>(std::move(assembly_type_cond), std::move(condition), std::move(reg_zero)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_ne = std::make_unique<AsmNE>();
        push_instr(std::make_unique<AsmJmpCC>(target, std::move(cond_code_ne)));
    }
    push_instr(std::make_unique<AsmJmp>(target_nan_ne));
    push_instr(std::make_unique<AsmLabel>(std::move(target_nan)));
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(std::make_unique<AsmJmpCC>(std::move(target), std::move(cond_code_e)));
    }
    push_instr(std::make_unique<AsmLabel>(std::move(target_nan_ne)));
}

static void jmp_ne_0_instr(TacJumpIfNotZero* node) {
    if (is_value_dbl(node->condition.get())) {
        jmp_ne_0_dbl_instr(node);
    }
    else {
        jmp_ne_0_int_instr(node);
    }
}

static void label_instr(TacLabel* node) {
    TIdentifier name = node->name;
    push_instr(std::make_unique<AsmLabel>(std::move(name)));
}

static void gen_instr(TacInstruction* node) {
    switch (node->type()) {
        case AST_T::TacReturn_t:
            ret_instr(static_cast<TacReturn*>(node));
            break;
        case AST_T::TacSignExtend_t:
            sign_extend_instr(static_cast<TacSignExtend*>(node));
            break;
        case AST_T::TacTruncate_t:
            truncate_instr(static_cast<TacTruncate*>(node));
            break;
        case AST_T::TacZeroExtend_t:
            zero_extend_instr(static_cast<TacZeroExtend*>(node));
            break;
        case AST_T::TacDoubleToInt_t:
            dbl_to_signed_instr(static_cast<TacDoubleToInt*>(node));
            break;
        case AST_T::TacDoubleToUInt_t:
            dbl_to_unsigned_instr(static_cast<TacDoubleToUInt*>(node));
            break;
        case AST_T::TacIntToDouble_t:
            signed_to_dbl_instr(static_cast<TacIntToDouble*>(node));
            break;
        case AST_T::TacUIntToDouble_t:
            unsigned_to_double_instr(static_cast<TacUIntToDouble*>(node));
            break;
        case AST_T::TacFunCall_t:
            call_instr(static_cast<TacFunCall*>(node));
            break;
        case AST_T::TacUnary_t:
            unary_instr(static_cast<TacUnary*>(node));
            break;
        case AST_T::TacBinary_t:
            binary_instr(static_cast<TacBinary*>(node));
            break;
        case AST_T::TacCopy_t:
            copy_instr(static_cast<TacCopy*>(node));
            break;
        case AST_T::TacGetAddress_t:
            getaddr_instr(static_cast<TacGetAddress*>(node));
            break;
        case AST_T::TacLoad_t:
            load_instr(static_cast<TacLoad*>(node));
            break;
        case AST_T::TacStore_t:
            store_instr(static_cast<TacStore*>(node));
            break;
        case AST_T::TacAddPtr_t:
            add_ptr_instr(static_cast<TacAddPtr*>(node));
            break;
        case AST_T::TacCopyToOffset_t:
            cp_to_offset_instr(static_cast<TacCopyToOffset*>(node));
            break;
        case AST_T::TacCopyFromOffset_t:
            cp_from_offset_instr(static_cast<TacCopyFromOffset*>(node));
            break;
        case AST_T::TacJump_t:
            jmp_instr(static_cast<TacJump*>(node));
            break;
        case AST_T::TacJumpIfZero_t:
            jmp_eq_0_instr(static_cast<TacJumpIfZero*>(node));
            break;
        case AST_T::TacJumpIfNotZero_t:
            jmp_ne_0_instr(static_cast<TacJumpIfNotZero*>(node));
            break;
        case AST_T::TacLabel_t:
            label_instr(static_cast<TacLabel*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// instruction = Mov(assembly_type, operand, operand) | MovSx(assembly_type, assembly_type, operand, operand)
//             | MovZeroExtend(assembly_type, assembly_type, operand, operand) | Lea(operand, operand)
//             | Cvttsd2si(assembly_type, operand, operand) | Cvtsi2sd(assembly_type, operand, operand)
//             | Unary(unary_operator, assembly_type, operand) | Binary(binary_operator, assembly_type, operand,
//             operand) | Cmp(assembly_type, operand, operand) | Idiv(assembly_type, operand) | Div(assembly_type,
//             operand) | Cdq(assembly_type) | Jmp(identifier) | JmpCC(cond_code, identifier) | SetCC(cond_code,
//             operand) | Label(identifier) | Push(operand) | Pop(reg) | Call(identifier) | Ret
static void gen_instr_list(const std::vector<std::unique_ptr<TacInstruction>>& list_node) {
    for (const auto& instruction : list_node) {
        if (instruction) {
            gen_instr(instruction.get());
        }
    }
}

static void reg_param_fun_instr(TIdentifier name, REGISTER_KIND arg_register) {
    std::shared_ptr<AsmOperand> src = generate_register(arg_register);
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = name;
        dst = std::make_shared<AsmPseudo>(std::move(dst_name));
    }
    std::shared_ptr<AssemblyType> assembly_type_dst = convert_backend_assembly_type(name);
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void stack_param_fun_instr(TIdentifier name, TLong stack_bytes) {
    std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Bp, stack_bytes);
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = name;
        dst = std::make_shared<AsmPseudo>(std::move(dst_name));
    }
    std::shared_ptr<AssemblyType> assembly_type_dst = convert_backend_assembly_type(name);
    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
}

static void reg_8b_param_fun_instr(TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_register) {
    ret_8b_call_instr(name, offset, struct_type, arg_register);
}

static void stack_8b_param_fun_instr(TIdentifier name, TLong stack_bytes, TLong offset, Structure* struct_type) {
    std::shared_ptr<AssemblyType> assembly_type_dst = asm_type_8b(struct_type, offset);
    if (assembly_type_dst->type() == AST_T::ByteArray_t) {
        TLong size = static_cast<ByteArray*>(assembly_type_dst.get())->size;
        while (size > 0l) {
            std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Bp, stack_bytes);
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(name, offset);
            if (size >= 4l) {
                assembly_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
                stack_bytes += 4l;
            }
            else {
                assembly_type_dst = std::make_shared<Byte>();
                size--;
                offset++;
                stack_bytes++;
            }
            push_instr(std::make_unique<AsmMov>(assembly_type_dst, std::move(src), std::move(dst)));
        }
    }
    else {
        std::shared_ptr<AsmOperand> src = generate_memory(REGISTER_KIND::Bp, stack_bytes);
        std::shared_ptr<AsmOperand> dst;
        {
            TIdentifier dst_name = name;
            TLong to_offset = offset;
            dst = std::make_shared<AsmPseudoMem>(std::move(dst_name), std::move(to_offset));
        }
        push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
    }
}

static void param_fun_toplvl(TacFunction* node, FunType* fun_type, bool is_return_memory) {
    size_t reg_size = is_return_memory ? 1 : 0;
    size_t sse_size = 0;
    TLong stack_bytes = 16l;
    for (TIdentifier param : node->params) {
        if (frontend->symbol_table[param]->type_t->type() == AST_T::Double_t) {
            if (sse_size < 8) {
                reg_param_fun_instr(param, context->sse_arg_registers[sse_size]);
                sse_size++;
            }
            else {
                stack_param_fun_instr(param, stack_bytes);
                stack_bytes += 8l;
            }
        }
        else if (frontend->symbol_table[param]->type_t->type() != AST_T::Structure_t) {
            if (reg_size < 6) {
                reg_param_fun_instr(param, context->arg_registers[reg_size]);
                reg_size++;
            }
            else {
                stack_param_fun_instr(param, stack_bytes);
                stack_bytes += 8l;
            }
        }
        else {
            size_t struct_reg_size = 7;
            size_t struct_sse_size = 9;
            Structure* struct_type = static_cast<Structure*>(frontend->symbol_table[param]->type_t.get());
            struct_8b_clss(struct_type);
            if (context->struct_8b_cls_map[struct_type->tag][0] != STRUCT_8B_CLS::MEMORY) {
                struct_reg_size = 0;
                struct_sse_size = 0;
                for (STRUCT_8B_CLS struct_8b_cls : context->struct_8b_cls_map[struct_type->tag]) {
                    if (struct_8b_cls == STRUCT_8B_CLS::SSE) {
                        struct_sse_size++;
                    }
                    else {
                        struct_reg_size++;
                    }
                }
            }
            if (struct_reg_size + reg_size <= 6 && struct_sse_size + sse_size <= 8) {
                TLong offset = 0l;
                for (STRUCT_8B_CLS struct_8b_cls : context->struct_8b_cls_map[struct_type->tag]) {
                    if (struct_8b_cls == STRUCT_8B_CLS::SSE) {
                        reg_8b_param_fun_instr(param, offset, nullptr, context->sse_arg_registers[sse_size]);
                        sse_size++;
                    }
                    else {
                        reg_8b_param_fun_instr(param, offset, struct_type, context->arg_registers[reg_size]);
                        reg_size++;
                    }
                    offset += 8l;
                }
            }
            else {
                TLong offset = 0l;
                for (size_t i = 0; i < context->struct_8b_cls_map[struct_type->tag].size(); ++i) {
                    stack_8b_param_fun_instr(param, stack_bytes, offset, struct_type);
                    stack_bytes += 8l;
                    offset += 8l;
                }
            }
        }
    }
    param_fun_reg_mask(fun_type, reg_size, sse_size);
}

static std::unique_ptr<AsmFunction> fun_toplvl(TacFunction* node) {
    TIdentifier name = node->name;
    bool is_global = node->is_global;
    bool is_return_memory = false;

    std::vector<std::unique_ptr<AsmInstruction>> body;
    {
        context->p_instructions = &body;

        FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
        if (fun_type->ret_type->type() == AST_T::Structure_t) {
            Structure* struct_type = static_cast<Structure*>(fun_type->ret_type.get());
            struct_8b_clss(struct_type);
            if (context->struct_8b_cls_map[struct_type->tag][0] == STRUCT_8B_CLS::MEMORY) {
                is_return_memory = true;
                {
                    std::shared_ptr<AsmOperand> src = generate_register(REGISTER_KIND::Di);
                    std::shared_ptr<AsmOperand> dst = generate_memory(REGISTER_KIND::Bp, -8l);
                    std::shared_ptr<AssemblyType> assembly_type_dst = std::make_shared<QuadWord>();
                    push_instr(std::make_unique<AsmMov>(std::move(assembly_type_dst), std::move(src), std::move(dst)));
                }
            }
        }
        param_fun_toplvl(node, fun_type, is_return_memory);

        context->p_fun_type_top_level = fun_type;
        gen_instr_list(node->body);
        context->p_fun_type_top_level = nullptr;
        context->p_instructions = nullptr;
    }

    return std::make_unique<AsmFunction>(
        std::move(name), std::move(is_global), std::move(is_return_memory), std::move(body));
}

static std::unique_ptr<AsmStaticVariable> static_var_toplvl(TacStaticVariable* node) {
    TIdentifier name = node->name;
    bool is_global = node->is_global;
    TInt alignment = generate_type_alignment(node->static_init_type.get());
    std::vector<std::shared_ptr<StaticInit>> static_inits(node->static_inits.begin(), node->static_inits.end());
    return std::make_unique<AsmStaticVariable>(
        std::move(name), std::move(alignment), std::move(is_global), std::move(static_inits));
}

static void push_static_const_toplvl(std::unique_ptr<AsmTopLevel>&& static_constant_top_levels) {
    context->p_static_constant_top_levels->push_back(std::move(static_constant_top_levels));
}

static void dbl_static_const_toplvl(TIdentifier identifier, TIdentifier double_constant, TInt byte) {
    TIdentifier name = identifier;
    TInt alignment = byte;
    std::shared_ptr<StaticInit> static_init = std::make_shared<DoubleInit>(std::move(double_constant));
    push_static_const_toplvl(
        std::make_unique<AsmStaticConstant>(std::move(name), std::move(alignment), std::move(static_init)));
}

static std::unique_ptr<AsmStaticConstant> static_const_toplvl(TacStaticConstant* node) {
    TIdentifier name = node->name;
    TInt alignment = generate_type_alignment(node->static_init_type.get());
    std::shared_ptr<StaticInit> static_init = node->static_init;
    return std::make_unique<AsmStaticConstant>(std::move(name), std::move(alignment), std::move(static_init));
}

// top_level = Function(identifier, bool, bool, instruction*) | StaticVariable(identifier, bool, int, static_init*)
//           | StaticConstant(identifier, int, static_init)
static std::unique_ptr<AsmTopLevel> gen_toplvl(TacTopLevel* node) {
    switch (node->type()) {
        case AST_T::TacFunction_t:
            return fun_toplvl(static_cast<TacFunction*>(node));
        case AST_T::TacStaticVariable_t:
            return static_var_toplvl(static_cast<TacStaticVariable*>(node));
        case AST_T::TacStaticConstant_t:
            return static_const_toplvl(static_cast<TacStaticConstant*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

// AST = Program(top_level*, top_level*)
static std::unique_ptr<AsmProgram> gen_program(TacProgram* node) {
    std::vector<std::unique_ptr<AsmTopLevel>> static_constant_top_levels;
    static_constant_top_levels.reserve(node->static_constant_top_levels.size());
    for (const auto& top_level : node->static_constant_top_levels) {
        std::unique_ptr<AsmTopLevel> static_constant_top_level = gen_toplvl(top_level.get());
        static_constant_top_levels.push_back(std::move(static_constant_top_level));
    }

    std::vector<std::unique_ptr<AsmTopLevel>> top_levels;
    top_levels.reserve(node->static_variable_top_levels.size() + node->function_top_levels.size());
    {
        context->p_static_constant_top_levels = &static_constant_top_levels;

        for (const auto& top_level : node->static_variable_top_levels) {
            std::unique_ptr<AsmTopLevel> static_variable_top_level = gen_toplvl(top_level.get());
            top_levels.push_back(std::move(static_variable_top_level));
        }
        for (const auto& top_level : node->function_top_levels) {
            std::unique_ptr<AsmTopLevel> function_top_level = gen_toplvl(top_level.get());
            top_levels.push_back(std::move(function_top_level));
        }
        context->p_static_constant_top_levels = nullptr;
    }

    return std::make_unique<AsmProgram>(std::move(static_constant_top_levels), std::move(top_levels));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<AsmProgram> assembly_generation(std::unique_ptr<TacProgram> tac_ast) {
    context = std::make_unique<AsmGenContext>();
    std::unique_ptr<AsmProgram> asm_ast = gen_program(tac_ast.get());
    context.reset();

    tac_ast.reset();
    if (!asm_ast) {
        RAISE_INTERNAL_ERROR;
    }
    return asm_ast;
}
