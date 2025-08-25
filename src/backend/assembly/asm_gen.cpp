#include <memory>

#include "util/c_std.hpp"
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
    CLS_integer,
    CLS_sse,
    CLS_memory
};

enum ASM_LABEL_KIND {
    LBL_Lcomisd_nan,
    LBL_Ldouble,
    LBL_Lsd2si_after,
    LBL_Lsd2si_out_of_range,
    LBL_Lsi2sd_after,
    LBL_Lsi2sd_out_of_range
};

typedef vector_t(STRUCT_8B_CLS) VecSTRUCT_8B_CLS;
PairKeyValue(TIdentifier, VecSTRUCT_8B_CLS);

struct AsmGenContext {
    FrontEndContext* frontend;
    IdentifierContext* identifiers;
    // Assembly generation
    FunType* p_fun_type;
    REGISTER_KIND arg_regs[6];
    REGISTER_KIND sse_arg_regs[8];
    hashmap_t(TIdentifier, TIdentifier) dbl_const_table;
    hashmap_t(TIdentifier, VecSTRUCT_8B_CLS) struct_8b_cls_map;
    vector_t(std::unique_ptr<AsmInstruction>) * p_instrs;
    vector_t(std::unique_ptr<AsmTopLevel>) * p_static_consts;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

typedef AsmGenContext* Ctx;

static std::shared_ptr<AsmImm> char_imm_op(CConstChar* node) {
    TULong value = (TULong)node->value;
    bool is_neg = node->value < 0;
    return std::make_shared<AsmImm>(value, true, false, is_neg);
}

static std::shared_ptr<AsmImm> int_imm_op(CConstInt* node) {
    TULong value = (TULong)node->value;
    bool is_byte = node->value <= 127 && node->value >= -128;
    bool is_neg = node->value < 0;
    return std::make_shared<AsmImm>(value, is_byte, false, is_neg);
}

static std::shared_ptr<AsmImm> long_imm_op(CConstLong* node) {
    TULong value = (TULong)node->value;
    bool is_byte = node->value <= 127l && node->value >= -128l;
    bool is_quad = node->value > 2147483647l || node->value < -2147483648l;
    bool is_neg = node->value < 0l;
    return std::make_shared<AsmImm>(value, is_byte, is_quad, is_neg);
}

static std::shared_ptr<AsmImm> uchar_imm(CConstUChar* node) {
    TULong value = (TULong)node->value;
    return std::make_shared<AsmImm>(value, true, false, false);
}

static std::shared_ptr<AsmImm> uint_imm_op(CConstUInt* node) {
    TULong value = (TULong)node->value;
    bool is_byte = node->value <= 255u;
    bool is_quad = node->value > 2147483647u;
    return std::make_shared<AsmImm>(value, is_byte, is_quad, false);
}

static std::shared_ptr<AsmImm> ulong_imm_op(CConstULong* node) {
    TULong value = node->value;
    bool is_byte = node->value <= 255ul;
    bool is_quad = node->value > 2147483647ul;
    return std::make_shared<AsmImm>(value, is_byte, is_quad, false);
}

static TIdentifier repr_asm_label(Ctx ctx, ASM_LABEL_KIND asm_label_kind) {
    string_t name = str_new(NULL);
    switch (asm_label_kind) {
        case LBL_Lcomisd_nan: {
            name = str_new("comisd_nan");
            break;
        }
        case LBL_Ldouble: {
            name = str_new("double");
            break;
        }
        case LBL_Lsd2si_after: {
            name = str_new("sd2si_after");
            break;
        }
        case LBL_Lsd2si_out_of_range: {
            name = str_new("sd2si_out_of_range");
            break;
        }
        case LBL_Lsi2sd_after: {
            name = str_new("si2sd_after");
            break;
        }
        case LBL_Lsi2sd_out_of_range: {
            name = str_new("si2sd_out_of_range");
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_label_identifier(ctx->identifiers, &name);
}

static void dbl_static_const_toplvl(Ctx ctx, TIdentifier identifier, TIdentifier dbl_const, TInt byte);

static TIdentifier make_binary_identifier(Ctx ctx, TULong binary) {
    string_t strto_binary = str_to_string(binary);
    return make_string_identifier(ctx->identifiers, &strto_binary);
}

static std::shared_ptr<AsmData> dbl_static_const_op(Ctx ctx, TULong binary, TInt byte) {
    TIdentifier dbl_const_label;
    {
        TIdentifier dbl_const = make_binary_identifier(ctx, binary);
        if (map_find(ctx->dbl_const_table, dbl_const) != map_end()) {
            dbl_const_label = map_get(ctx->dbl_const_table, dbl_const);
        }
        else {
            dbl_const_label = repr_asm_label(ctx, LBL_Ldouble);
            map_add(ctx->dbl_const_table, dbl_const, dbl_const_label);
            dbl_static_const_toplvl(ctx, dbl_const_label, dbl_const, byte);
        }
    }
    return std::make_shared<AsmData>(dbl_const_label, 0l);
}

static std::shared_ptr<AsmData> dbl_const_op(Ctx ctx, CConstDouble* node) {
    TULong binary = dbl_to_binary(node->value);
    TInt byte = binary == 9223372036854775808ul ? 16 : 8;
    return dbl_static_const_op(ctx, binary, byte);
}

static std::shared_ptr<AsmOperand> const_op(Ctx ctx, TacConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return char_imm_op(static_cast<CConstChar*>(node->constant.get()));
        case AST_CConstInt_t:
            return int_imm_op(static_cast<CConstInt*>(node->constant.get()));
        case AST_CConstLong_t:
            return long_imm_op(static_cast<CConstLong*>(node->constant.get()));
        case AST_CConstDouble_t:
            return dbl_const_op(ctx, static_cast<CConstDouble*>(node->constant.get()));
        case AST_CConstUChar_t:
            return uchar_imm(static_cast<CConstUChar*>(node->constant.get()));
        case AST_CConstUInt_t:
            return uint_imm_op(static_cast<CConstUInt*>(node->constant.get()));
        case AST_CConstULong_t:
            return ulong_imm_op(static_cast<CConstULong*>(node->constant.get()));
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<AsmPseudo> pseudo_op(TacVariable* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmPseudo>(name);
}

static std::shared_ptr<AsmPseudoMem> pseudo_mem_op(TacVariable* node) {
    TIdentifier name = node->name;
    return std::make_shared<AsmPseudoMem>(name, 0l);
}

static std::shared_ptr<AsmOperand> var_op(Ctx ctx, TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type()) {
        case AST_Array_t:
        case AST_Structure_t:
            return pseudo_mem_op(node);
        default:
            return pseudo_op(node);
    }
}

// operand = Imm(int, bool, bool, bool) | Reg(reg) | Pseudo(identifier) | Memory(int, reg) | Data(identifier, int)
//         | PseudoMem(identifier, int) | Indexed(int, reg, reg)
static std::shared_ptr<AsmOperand> gen_op(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return const_op(ctx, static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return var_op(ctx, static_cast<TacVariable*>(node));
        default:
            THROW_ABORT;
    }
}

// (signed) cond_code = E | NE | L | LE | G | GE
static std::unique_ptr<AsmCondCode> gen_signed_cond_code(TacBinaryOp* node) {
    switch (node->type()) {
        case AST_TacEqual_t:
            return std::make_unique<AsmE>();
        case AST_TacNotEqual_t:
            return std::make_unique<AsmNE>();
        case AST_TacLessThan_t:
            return std::make_unique<AsmL>();
        case AST_TacLessOrEqual_t:
            return std::make_unique<AsmLE>();
        case AST_TacGreaterThan_t:
            return std::make_unique<AsmG>();
        case AST_TacGreaterOrEqual_t:
            return std::make_unique<AsmGE>();
        default:
            THROW_ABORT;
    }
}

// (unsigned) cond_code = E | NE | B | BE | A | AE
static std::unique_ptr<AsmCondCode> gen_unsigned_cond_code(TacBinaryOp* node) {
    switch (node->type()) {
        case AST_TacEqual_t:
            return std::make_unique<AsmE>();
        case AST_TacNotEqual_t:
            return std::make_unique<AsmNE>();
        case AST_TacLessThan_t:
            return std::make_unique<AsmB>();
        case AST_TacLessOrEqual_t:
            return std::make_unique<AsmBE>();
        case AST_TacGreaterThan_t:
            return std::make_unique<AsmA>();
        case AST_TacGreaterOrEqual_t:
            return std::make_unique<AsmAE>();
        default:
            THROW_ABORT;
    }
}

// unary_operator = Not | Neg | Shr
static std::unique_ptr<AsmUnaryOp> gen_unop(TacUnaryOp* node) {
    switch (node->type()) {
        case AST_TacComplement_t:
            return std::make_unique<AsmNot>();
        case AST_TacNegate_t:
            return std::make_unique<AsmNeg>();
        default:
            THROW_ABORT;
    }
}

// binary_operator = Add | Sub | Mult | DivDouble | BitAnd | BitOr | BitXor | BitShiftLeft | BitShiftRight |
//                 BitShrArithmetic
static std::unique_ptr<AsmBinaryOp> gen_binop(TacBinaryOp* node) {
    switch (node->type()) {
        case AST_TacAdd_t:
            return std::make_unique<AsmAdd>();
        case AST_TacSubtract_t:
            return std::make_unique<AsmSub>();
        case AST_TacMultiply_t:
            return std::make_unique<AsmMult>();
        case AST_TacDivide_t:
            return std::make_unique<AsmDivDouble>();
        case AST_TacBitAnd_t:
            return std::make_unique<AsmBitAnd>();
        case AST_TacBitOr_t:
            return std::make_unique<AsmBitOr>();
        case AST_TacBitXor_t:
            return std::make_unique<AsmBitXor>();
        case AST_TacBitShiftLeft_t:
            return std::make_unique<AsmBitShiftLeft>();
        case AST_TacBitShiftRight_t:
            return std::make_unique<AsmBitShiftRight>();
        case AST_TacBitShrArithmetic_t:
            return std::make_unique<AsmBitShrArithmetic>();
        default:
            THROW_ABORT;
    }
}

static bool is_const_signed(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
        case AST_CConstInt_t:
        case AST_CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_signed(Ctx ctx, TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_Int_t:
        case AST_Long_t:
        case AST_Double_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_signed(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return is_const_signed(static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return is_var_signed(ctx, static_cast<TacVariable*>(node));
        default:
            THROW_ABORT;
    }
}

static bool is_const_1b(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
        case AST_CConstUChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_1b(Ctx ctx, TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_1b(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return is_const_1b(static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return is_var_1b(ctx, static_cast<TacVariable*>(node));
        default:
            THROW_ABORT;
    }
}

static bool is_const_4b(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstInt_t:
        case AST_CConstUInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_4b(Ctx ctx, TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type()) {
        case AST_Int_t:
        case AST_UInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_4b(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return is_const_4b(static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return is_var_4b(ctx, static_cast<TacVariable*>(node));
        default:
            THROW_ABORT;
    }
}

static bool is_const_dbl(TacConstant* node) { return node->constant->type() == AST_CConstDouble_t; }

static bool is_var_dbl(Ctx ctx, TacVariable* node) {
    return map_get(ctx->frontend->symbol_table, node->name)->type_t->type() == AST_Double_t;
}

static bool is_value_dbl(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return is_const_dbl(static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return is_var_dbl(ctx, static_cast<TacVariable*>(node));
        default:
            THROW_ABORT;
    }
}

static bool is_var_struct(Ctx ctx, TacVariable* node) {
    return map_get(ctx->frontend->symbol_table, node->name)->type_t->type() == AST_Structure_t;
}

static bool is_value_struct(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacVariable_t:
            return is_var_struct(ctx, static_cast<TacVariable*>(node));
        case AST_TacConstant_t:
            return false;
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<AssemblyType> const_asm_type(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
        case AST_CConstUChar_t:
            return std::make_shared<Byte>();
        case AST_CConstInt_t:
        case AST_CConstUInt_t:
            return std::make_shared<LongWord>();
        case AST_CConstDouble_t:
            return std::make_shared<BackendDouble>();
        case AST_CConstLong_t:
        case AST_CConstULong_t:
            return std::make_shared<QuadWord>();
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<AssemblyType> var_asm_type(Ctx ctx, TacVariable* node) {
    return cvt_backend_asm_type(ctx->frontend, node->name);
}

static std::shared_ptr<AssemblyType> gen_asm_type(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return const_asm_type(static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return var_asm_type(ctx, static_cast<TacVariable*>(node));
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<AssemblyType> asm_type_8b(Ctx ctx, Structure* struct_type, TLong offset) {
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size - offset;
    if (size >= 8l) {
        return std::make_shared<QuadWord>();
    }
    switch (size) {
        case 1l:
            return std::make_shared<Byte>();
        case 4l:
            return std::make_shared<LongWord>();
        default:
            return std::make_shared<ByteArray>(size, 8);
    }
}

static void struct_8b_class(Ctx ctx, Structure* struct_type);

static vector_t(STRUCT_8B_CLS) struct_mem_8b_class(Ctx ctx, Structure* struct_type) {
    vector_t(STRUCT_8B_CLS) struct_8b_cls = vec_new();
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    while (size > 0l) {
        vec_push_back(struct_8b_cls, CLS_memory);
        size -= 8l;
    }
    return struct_8b_cls;
}

static vector_t(STRUCT_8B_CLS) struct_1_reg_8b_class(Ctx ctx, Structure* struct_type) {
    vector_t(STRUCT_8B_CLS) struct_8b_cls = vec_new();
    vec_push_back(struct_8b_cls, CLS_sse);
    StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag).get();
    size_t members_front = struct_type->is_union ? map_size(struct_typedef->members) : 1;
    for (size_t i = 0; i < members_front; ++i) {
        if (struct_8b_cls[0] == CLS_integer) {
            break;
        }
        Type* member_type = get_struct_typedef_member(ctx->frontend, struct_type->tag, i)->member_type.get();
        while (member_type->type() == AST_Array_t) {
            member_type = static_cast<Array*>(member_type)->elem_type.get();
        }
        if (member_type->type() == AST_Structure_t) {
            Structure* member_struct_type = static_cast<Structure*>(member_type);
            struct_8b_class(ctx, member_struct_type);
            if (map_get(ctx->struct_8b_cls_map, member_struct_type->tag)[0] == CLS_integer) {
                struct_8b_cls[0] = CLS_integer;
            }
        }
        else if (member_type->type() != AST_Double_t) {
            struct_8b_cls[0] = CLS_integer;
        }
    }
    return struct_8b_cls;
}

static vector_t(STRUCT_8B_CLS) struct_2_reg_8b_class(Ctx ctx, Structure* struct_type) {
    vector_t(STRUCT_8B_CLS) struct_8b_cls = vec_new();
    vec_push_back(struct_8b_cls, CLS_sse);
    vec_push_back(struct_8b_cls, CLS_sse);
    StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag).get();
    size_t members_front = struct_type->is_union ? map_size(struct_typedef->members) : 1;
    for (size_t i = 0; i < members_front; ++i) {
        if (struct_8b_cls[0] == CLS_integer && struct_8b_cls[1] == CLS_integer) {
            break;
        }
        TLong size = 1l;
        Type* member_type = get_struct_typedef_member(ctx->frontend, struct_type->tag, i)->member_type.get();
        if (member_type->type() == AST_Array_t) {
            do {
                Array* member_arr_type = static_cast<Array*>(member_type);
                member_type = member_arr_type->elem_type.get();
                size *= member_arr_type->size;
            }
            while (member_type->type() == AST_Array_t);
        }
        if (member_type->type() == AST_Structure_t) {
            size *= map_get(ctx->frontend->struct_typedef_table, static_cast<Structure*>(member_type)->tag)->size;
        }
        else {
            size *= gen_type_alignment(ctx->frontend, member_type);
        }
        if (size > 8l) {
            if (member_type->type() == AST_Structure_t) {
                Structure* member_struct_type = static_cast<Structure*>(member_type);
                struct_8b_class(ctx, member_struct_type);
                const vector_t(STRUCT_8B_CLS) member_struct_8b_cls =
                    map_get(ctx->struct_8b_cls_map, member_struct_type->tag);
                if (vec_size(member_struct_8b_cls) > 1) {
                    if (member_struct_8b_cls[0] == CLS_integer) {
                        struct_8b_cls[0] = CLS_integer;
                    }
                    if (member_struct_8b_cls[1] == CLS_integer) {
                        struct_8b_cls[1] = CLS_integer;
                    }
                }
                else {
                    if (member_struct_8b_cls[0] == CLS_integer) {
                        struct_8b_cls[0] = CLS_integer;
                        struct_8b_cls[1] = CLS_integer;
                    }
                }
            }
            else if (member_type->type() != AST_Double_t) {
                struct_8b_cls[0] = CLS_integer;
                struct_8b_cls[1] = CLS_integer;
            }
        }
        else {
            if (member_type->type() == AST_Structure_t) {
                Structure* member_struct_type = static_cast<Structure*>(member_type);
                struct_8b_class(ctx, member_struct_type);
                if (map_get(ctx->struct_8b_cls_map, member_struct_type->tag)[0] == CLS_integer) {
                    struct_8b_cls[0] = CLS_integer;
                }
            }
            else if (member_type->type() != AST_Double_t) {
                struct_8b_cls[0] = CLS_integer;
            }
            if (!struct_type->is_union) {
                member_type = get_struct_typedef_back(ctx->frontend, struct_type->tag)->member_type.get();
                while (member_type->type() == AST_Array_t) {
                    member_type = static_cast<Array*>(member_type)->elem_type.get();
                }
                if (member_type->type() == AST_Structure_t) {
                    Structure* member_struct_type = static_cast<Structure*>(member_type);
                    struct_8b_class(ctx, member_struct_type);
                    if (map_get(ctx->struct_8b_cls_map, member_struct_type->tag)[0] == CLS_integer) {
                        struct_8b_cls[1] = CLS_integer;
                    }
                }
                else if (member_type->type() != AST_Double_t) {
                    struct_8b_cls[1] = CLS_integer;
                }
            }
        }
    }
    return struct_8b_cls;
}

static void struct_8b_class(Ctx ctx, Structure* struct_type) {
    if (map_find(ctx->struct_8b_cls_map, struct_type->tag) == map_end()) {
        vector_t(STRUCT_8B_CLS) struct_8b_cls = vec_new();
        if (map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size > 16l) {
            struct_8b_cls = struct_mem_8b_class(ctx, struct_type);
        }
        else if (map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size > 8l) {
            struct_8b_cls = struct_2_reg_8b_class(ctx, struct_type);
        }
        else {
            struct_8b_cls = struct_1_reg_8b_class(ctx, struct_type);
        }
        map_add(ctx->struct_8b_cls_map, struct_type->tag, vec_new());
        vec_move(&struct_8b_cls, &map_get(ctx->struct_8b_cls_map, struct_type->tag));
    }
}

static void fun_param_reg_mask(Ctx ctx, FunType* fun_type, size_t reg_size, size_t sse_size) {
    if (fun_type->param_reg_mask == NULL_REGISTER_MASK) {
        fun_type->param_reg_mask = REGISTER_MASK_FALSE;
        for (size_t i = 0; i < reg_size; ++i) {
            register_mask_set(&fun_type->param_reg_mask, ctx->arg_regs[i], true);
        }
        for (size_t i = 0; i < sse_size; ++i) {
            register_mask_set(&fun_type->param_reg_mask, ctx->sse_arg_regs[i], true);
        }
    }
}

static void ret_1_reg_mask(FunType* fun_type, bool reg_size) {
    if (fun_type->ret_reg_mask == NULL_REGISTER_MASK) {
        fun_type->ret_reg_mask = REGISTER_MASK_FALSE;
        register_mask_set(&fun_type->ret_reg_mask, reg_size ? REG_Ax : REG_Xmm0, true);
    }
}

static void ret_2_reg_mask(FunType* fun_type, bool reg_size, bool sse_size) {
    if (fun_type->ret_reg_mask == NULL_REGISTER_MASK) {
        fun_type->ret_reg_mask = REGISTER_MASK_FALSE;
        if (reg_size) {
            register_mask_set(&fun_type->ret_reg_mask, REG_Ax, true);
            register_mask_set(&fun_type->ret_reg_mask, sse_size ? REG_Xmm0 : REG_Dx, true);
        }
        else if (sse_size) {
            register_mask_set(&fun_type->ret_reg_mask, REG_Xmm0, true);
            register_mask_set(&fun_type->ret_reg_mask, REG_Xmm1, true);
        }
    }
}

static void push_instr(Ctx ctx, std::unique_ptr<AsmInstruction>&& instr) { vec_move_back(*ctx->p_instrs, instr); }

static void ret_int_instr(Ctx ctx, TacReturn* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->val.get());
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
    std::shared_ptr<AssemblyType> asm_type_val = gen_asm_type(ctx, node->val.get());
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_val), std::move(src), std::move(dst)));
    ret_1_reg_mask(ctx->p_fun_type, true);
}

static void ret_dbl_instr(Ctx ctx, TacReturn* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->val.get());
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Xmm0);
    std::shared_ptr<AssemblyType> asm_type_val = std::make_shared<BackendDouble>();
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_val), std::move(src), std::move(dst)));
    ret_1_reg_mask(ctx->p_fun_type, false);
}

static void ret_8b_instr(Ctx ctx, TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_reg) {
    TIdentifier src_name = name;
    std::shared_ptr<AsmOperand> dst = gen_register(arg_reg);
    std::shared_ptr<AssemblyType> asm_type_src =
        struct_type ? asm_type_8b(ctx, struct_type, offset) : std::make_shared<BackendDouble>();
    if (asm_type_src->type() == AST_ByteArray_t) {
        TLong size = offset + 2l;
        offset += static_cast<ByteArray*>(asm_type_src.get())->size - 1l;
        asm_type_src = std::make_shared<Byte>();
        std::shared_ptr<AsmOperand> src_shl = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AssemblyType> asm_type_shl = std::make_shared<QuadWord>();
        while (offset >= size) {
            {
                std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
                std::shared_ptr<AsmOperand> dst_cp = dst;
                std::shared_ptr<AssemblyType> asm_type_src_cp = asm_type_src;
                push_instr(
                    ctx, std::make_unique<AsmMov>(std::move(asm_type_src_cp), std::move(src), std::move(dst_cp)));
            }
            {
                std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmBitShiftLeft>();
                std::shared_ptr<AssemblyType> asm_type_shl_cp = asm_type_shl;
                std::shared_ptr<AsmOperand> src_shl_cp = src_shl;
                std::shared_ptr<AsmOperand> dst_cp = dst;
                push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type_shl_cp),
                                    std::move(src_shl_cp), std::move(dst_cp)));
            }
            offset--;
        }
        {
            std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
            std::shared_ptr<AsmOperand> dst_cp = dst;
            std::shared_ptr<AssemblyType> asm_type_src_cp = asm_type_src;
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src_cp), std::move(src), std::move(dst_cp)));
        }
        {
            std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmBitShiftLeft>();
            std::shared_ptr<AsmOperand> dst_cp = dst;
            push_instr(ctx, std::make_unique<AsmBinary>(
                                std::move(binop), std::move(asm_type_shl), std::move(src_shl), std::move(dst_cp)));
        }
        offset--;
        {
            std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
        }
    }
    else {
        std::shared_ptr<AsmOperand> src;
        {
            TLong from_offset = offset;
            src = std::make_shared<AsmPseudoMem>(src_name, from_offset);
        }
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
}

static void ret_struct_instr(Ctx ctx, TacReturn* node) {
    TIdentifier name = static_cast<TacVariable*>(node->val.get())->name;
    Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, name)->type_t.get());
    struct_8b_class(ctx, struct_type);
    const vector_t(STRUCT_8B_CLS) struct_8b_cls = map_get(ctx->struct_8b_cls_map, struct_type->tag);
    if (struct_8b_cls[0] == CLS_memory) {
        {
            std::shared_ptr<AsmOperand> src = gen_memory(REG_Bp, -8l);
            std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
            std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
            ret_1_reg_mask(ctx->p_fun_type, true);
        }
        {
            TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
            TLong offset = 0l;
            while (size > 0l) {
                std::shared_ptr<AsmOperand> src = gen_op(ctx, node->val.get());
                THROW_ABORT_IF(src->type() != AST_AsmPseudoMem_t);
                static_cast<AsmPseudoMem*>(src.get())->offset = offset;
                std::shared_ptr<AsmOperand> dst = gen_memory(REG_Ax, offset);
                std::shared_ptr<AssemblyType> asm_type_src;
                if (size >= 8l) {
                    asm_type_src = std::make_shared<QuadWord>();
                    size -= 8l;
                    offset += 8l;
                }
                else if (size >= 4l) {
                    asm_type_src = std::make_shared<LongWord>();
                    size -= 4l;
                    offset += 4l;
                }
                else {
                    asm_type_src = std::make_shared<Byte>();
                    size--;
                    offset++;
                }
                push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
            }
        }
    }
    else {
        bool reg_size = false;
        switch (struct_8b_cls[0]) {
            case CLS_integer: {
                ret_8b_instr(ctx, name, 0l, struct_type, REG_Ax);
                reg_size = true;
                break;
            }
            case CLS_sse:
                ret_8b_instr(ctx, name, 0l, NULL, REG_Xmm0);
                break;
            default:
                THROW_ABORT;
        }
        if (vec_size(struct_8b_cls) == 2) {
            bool sse_size = !reg_size;
            switch (struct_8b_cls[1]) {
                case CLS_integer:
                    ret_8b_instr(ctx, name, 8l, struct_type, reg_size ? REG_Dx : REG_Ax);
                    break;
                case CLS_sse: {
                    ret_8b_instr(ctx, name, 8l, NULL, sse_size ? REG_Xmm1 : REG_Xmm0);
                    sse_size = true;
                    break;
                }
                default:
                    THROW_ABORT;
            }
            ret_2_reg_mask(ctx->p_fun_type, reg_size, sse_size);
        }
        else {
            ret_1_reg_mask(ctx->p_fun_type, reg_size);
        }
    }
}

static void ret_instr(Ctx ctx, TacReturn* node) {
    if (node->val) {
        if (is_value_dbl(ctx, node->val.get())) {
            ret_dbl_instr(ctx, node);
        }
        else if (!is_value_struct(ctx, node->val.get())) {
            ret_int_instr(ctx, node);
        }
        else {
            ret_struct_instr(ctx, node);
        }
    }
    else {
        ret_2_reg_mask(ctx->p_fun_type, false, false);
    }
    push_instr(ctx, std::make_unique<AsmRet>());
}

static void sign_extend_instr(Ctx ctx, TacSignExtend* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->src.get());
    std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node->dst.get());
    push_instr(ctx,
        std::make_unique<AsmMovSx>(std::move(asm_type_src), std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void truncate_imm_byte_instr(AsmImm* node) {
    if (!node->is_byte) {
        node->value %= 256ul;
    }
}

static void truncate_byte_instr(Ctx ctx, TacTruncate* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<Byte>();
    if (src->type() == AST_AsmImm_t) {
        truncate_imm_byte_instr(static_cast<AsmImm*>(src.get()));
    }
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void truncate_imm_long_instr(AsmImm* node) {
    if (node->is_quad) {
        node->value -= 4294967296ul;
    }
}

static void truncate_long_instr(Ctx ctx, TacTruncate* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<LongWord>();
    if (src->type() == AST_AsmImm_t) {
        truncate_imm_long_instr(static_cast<AsmImm*>(src.get()));
    }
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void truncate_instr(Ctx ctx, TacTruncate* node) {
    if (is_value_1b(ctx, node->dst.get())) {
        truncate_byte_instr(ctx, node);
    }
    else {
        truncate_long_instr(ctx, node);
    }
}

static void zero_extend_instr(Ctx ctx, TacZeroExtend* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->src.get());
    std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node->dst.get());
    push_instr(ctx, std::make_unique<AsmMovZeroExtend>(
                        std::move(asm_type_src), std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void dbl_to_char_instr(Ctx ctx, TacDoubleToInt* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_register(REG_Ax);
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<LongWord>();
        push_instr(ctx, std::make_unique<AsmCvttsd2si>(std::move(asm_type_src), std::move(src), std::move(src_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<Byte>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void dbl_to_long_instr(Ctx ctx, TacDoubleToInt* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->dst.get());
    push_instr(ctx, std::make_unique<AsmCvttsd2si>(std::move(asm_type_src), std::move(src), std::move(dst)));
}

static void dbl_to_signed_instr(Ctx ctx, TacDoubleToInt* node) {
    if (is_value_1b(ctx, node->dst.get())) {
        dbl_to_char_instr(ctx, node);
    }
    else {
        dbl_to_long_instr(ctx, node);
    }
}

static void dbl_to_uchar_instr(Ctx ctx, TacDoubleToUInt* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_register(REG_Ax);
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<LongWord>();
        push_instr(ctx, std::make_unique<AsmCvttsd2si>(std::move(asm_type_src), std::move(src), std::move(src_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<Byte>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void dbl_to_uint_instr(Ctx ctx, TacDoubleToUInt* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_register(REG_Ax);
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
        push_instr(ctx, std::make_unique<AsmCvttsd2si>(std::move(asm_type_src), std::move(src), std::move(src_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<LongWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void dbl_to_ulong_instr(Ctx ctx, TacDoubleToUInt* node) {
    TIdentifier target_out_of_range = repr_asm_label(ctx, LBL_Lsd2si_out_of_range);
    TIdentifier target_after = repr_asm_label(ctx, LBL_Lsd2si_after);
    std::shared_ptr<AsmOperand> upper_bound_sd = dbl_static_const_op(ctx, 4890909195324358656ul, 8);
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AsmOperand> dst_out_of_range_sd = gen_register(REG_Xmm1);
    std::shared_ptr<AssemblyType> asm_type_sd = std::make_shared<BackendDouble>();
    std::shared_ptr<AssemblyType> asm_type_si = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> src_cp = src;
        std::shared_ptr<AsmOperand> upper_bound_sd_cp = upper_bound_sd;
        std::shared_ptr<AssemblyType> asm_type_sd_cp = asm_type_sd;
        push_instr(
            ctx, std::make_unique<AsmCmp>(std::move(asm_type_sd_cp), std::move(upper_bound_sd_cp), std::move(src_cp)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_ae = std::make_unique<AsmAE>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target_out_of_range, std::move(cond_code_ae)));
    }
    {
        std::shared_ptr<AsmOperand> src_cp = src;
        std::shared_ptr<AsmOperand> dst_cp = dst;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(
            ctx, std::make_unique<AsmCvttsd2si>(std::move(asm_type_si_cp), std::move(src_cp), std::move(dst_cp)));
    }
    push_instr(ctx, std::make_unique<AsmJmp>(target_after));
    push_instr(ctx, std::make_unique<AsmLabel>(target_out_of_range));
    {
        std::shared_ptr<AsmOperand> dst_out_of_range_sd_cp = dst_out_of_range_sd;
        std::shared_ptr<AssemblyType> asm_type_sd_cp = asm_type_sd;
        push_instr(ctx,
            std::make_unique<AsmMov>(std::move(asm_type_sd_cp), std::move(src), std::move(dst_out_of_range_sd_cp)));
    }
    {
        std::unique_ptr<AsmBinaryOp> binop_out_of_range_sd_sub = std::make_unique<AsmSub>();
        std::shared_ptr<AsmOperand> dst_out_of_range_sd_cp = dst_out_of_range_sd;
        push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop_out_of_range_sd_sub), std::move(asm_type_sd),
                            std::move(upper_bound_sd), std::move(dst_out_of_range_sd_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst_cp = dst;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(ctx, std::make_unique<AsmCvttsd2si>(
                            std::move(asm_type_si_cp), std::move(dst_out_of_range_sd), std::move(dst_cp)));
    }
    {
        std::unique_ptr<AsmBinaryOp> binop_out_of_range_si_add = std::make_unique<AsmAdd>();
        std::shared_ptr<AsmOperand> upper_bound_si =
            std::make_shared<AsmImm>(9223372036854775808ul, false, true, false);
        push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop_out_of_range_si_add), std::move(asm_type_si),
                            std::move(upper_bound_si), std::move(dst)));
    }
    push_instr(ctx, std::make_unique<AsmLabel>(target_after));
}

static void dbl_to_unsigned_instr(Ctx ctx, TacDoubleToUInt* node) {
    if (is_value_1b(ctx, node->dst.get())) {
        dbl_to_uchar_instr(ctx, node);
    }
    else if (is_value_4b(ctx, node->dst.get())) {
        dbl_to_uint_instr(ctx, node);
    }
    else {
        dbl_to_ulong_instr(ctx, node);
    }
}

static void char_to_dbl_instr(Ctx ctx, TacIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_register(REG_Ax);
    std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<LongWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<Byte>();
        std::shared_ptr<AssemblyType> asm_type_dst_cp = asm_type_dst;
        push_instr(ctx, std::make_unique<AsmMovSx>(std::move(asm_type_src), std::move(asm_type_dst_cp), std::move(src),
                            std::move(src_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmCvtsi2sd>(std::move(asm_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void long_to_dbl_instr(Ctx ctx, TacIntToDouble* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->src.get());
    push_instr(ctx, std::make_unique<AsmCvtsi2sd>(std::move(asm_type_src), std::move(src), std::move(dst)));
}

static void signed_to_dbl_instr(Ctx ctx, TacIntToDouble* node) {
    if (is_value_1b(ctx, node->src.get())) {
        char_to_dbl_instr(ctx, node);
    }
    else {
        long_to_dbl_instr(ctx, node);
    }
}

static void uchar_to_dbl_instr(Ctx ctx, TacUIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_register(REG_Ax);
    std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<LongWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<Byte>();
        std::shared_ptr<AssemblyType> asm_type_dst_cp = asm_type_dst;
        push_instr(ctx, std::make_unique<AsmMovZeroExtend>(std::move(asm_type_src), std::move(asm_type_dst_cp),
                            std::move(src), std::move(src_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmCvtsi2sd>(std::move(asm_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void uint_to_dbl_instr(Ctx ctx, TacUIntToDouble* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_register(REG_Ax);
    std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<LongWord>();
        std::shared_ptr<AssemblyType> asm_type_dst_cp = asm_type_dst;
        push_instr(ctx, std::make_unique<AsmMovZeroExtend>(std::move(asm_type_src), std::move(asm_type_dst_cp),
                            std::move(src), std::move(src_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmCvtsi2sd>(std::move(asm_type_dst), std::move(src_dst), std::move(dst)));
    }
}

static void ulong_to_dbl_instr(Ctx ctx, TacUIntToDouble* node) {
    TIdentifier target_out_of_range = repr_asm_label(ctx, LBL_Lsi2sd_out_of_range);
    TIdentifier target_after = repr_asm_label(ctx, LBL_Lsi2sd_after);
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AsmOperand> dst_out_of_range_si = gen_register(REG_Ax);
    std::shared_ptr<AsmOperand> dst_out_of_range_si_shr = gen_register(REG_Dx);
    std::shared_ptr<AssemblyType> asm_type_si = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> lower_bound_si = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> src_cp = src;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(
            ctx, std::make_unique<AsmCmp>(std::move(asm_type_si_cp), std::move(lower_bound_si), std::move(src_cp)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_l = std::make_unique<AsmL>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target_out_of_range, std::move(cond_code_l)));
    }
    {
        std::shared_ptr<AsmOperand> src_cp = src;
        std::shared_ptr<AsmOperand> dst_cp = dst;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(ctx, std::make_unique<AsmCvtsi2sd>(std::move(asm_type_si_cp), std::move(src_cp), std::move(dst_cp)));
    }
    push_instr(ctx, std::make_unique<AsmJmp>(target_after));
    push_instr(ctx, std::make_unique<AsmLabel>(target_out_of_range));
    {
        std::shared_ptr<AsmOperand> dst_out_of_range_si_cp = dst_out_of_range_si;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(ctx,
            std::make_unique<AsmMov>(std::move(asm_type_si_cp), std::move(src), std::move(dst_out_of_range_si_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst_out_of_range_si_cp = dst_out_of_range_si;
        std::shared_ptr<AsmOperand> dst_out_of_range_si_shr_cp = dst_out_of_range_si_shr;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_si_cp), std::move(dst_out_of_range_si_cp),
                            std::move(dst_out_of_range_si_shr_cp)));
    }
    {
        std::unique_ptr<AsmUnaryOp> unop_out_of_range_si_shr = std::make_unique<AsmShr>();
        std::shared_ptr<AsmOperand> dst_out_of_range_si_shr_cp = dst_out_of_range_si_shr;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(ctx, std::make_unique<AsmUnary>(std::move(unop_out_of_range_si_shr), std::move(asm_type_si_cp),
                            std::move(dst_out_of_range_si_shr_cp)));
    }
    {
        std::unique_ptr<AsmBinaryOp> binop_out_of_range_si_and = std::make_unique<AsmBitAnd>();
        std::shared_ptr<AsmOperand> set_bit_si = std::make_shared<AsmImm>(1ul, true, false, false);
        std::shared_ptr<AsmOperand> dst_out_of_range_si_cp = dst_out_of_range_si;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop_out_of_range_si_and), std::move(asm_type_si_cp),
                            std::move(set_bit_si), std::move(dst_out_of_range_si_cp)));
    }
    {
        std::unique_ptr<AsmBinaryOp> binop_out_of_range_si_or = std::make_unique<AsmBitOr>();
        std::shared_ptr<AsmOperand> dst_out_of_range_si_shr_cp = dst_out_of_range_si_shr;
        std::shared_ptr<AssemblyType> asm_type_si_cp = asm_type_si;
        push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop_out_of_range_si_or), std::move(asm_type_si_cp),
                            std::move(dst_out_of_range_si), std::move(dst_out_of_range_si_shr_cp)));
    }
    {
        std::shared_ptr<AsmOperand> dst_cp = dst;
        push_instr(ctx, std::make_unique<AsmCvtsi2sd>(
                            std::move(asm_type_si), std::move(dst_out_of_range_si_shr), std::move(dst_cp)));
    }
    {
        std::unique_ptr<AsmBinaryOp> binop_out_of_range_sq_add = std::make_unique<AsmAdd>();
        std::shared_ptr<AsmOperand> dst_cp = dst;
        std::shared_ptr<AssemblyType> asm_type_sq = std::make_shared<BackendDouble>();
        push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop_out_of_range_sq_add), std::move(asm_type_sq),
                            std::move(dst), std::move(dst_cp)));
    }
    push_instr(ctx, std::make_unique<AsmLabel>(target_after));
}

static void unsigned_to_dbl_instr(Ctx ctx, TacUIntToDouble* node) {
    if (is_value_1b(ctx, node->src.get())) {
        uchar_to_dbl_instr(ctx, node);
    }
    else if (is_value_4b(ctx, node->src.get())) {
        uint_to_dbl_instr(ctx, node);
    }
    else {
        ulong_to_dbl_instr(ctx, node);
    }
}

static void alloc_stack_instr(Ctx ctx, TLong byte) { push_instr(ctx, alloc_stack_bytes(byte)); }

static void dealloc_stack_instr(Ctx ctx, TLong byte) {
    std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmAdd>();
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
    push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type), std::move(src), std::move(dst)));
}

static void reg_arg_call_instr(Ctx ctx, TacValue* node, REGISTER_KIND arg_reg) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node);
    std::shared_ptr<AsmOperand> dst = gen_register(arg_reg);
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node);
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
}

static void stack_arg_call_instr(Ctx ctx, TacValue* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node);
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node);
    if (src->type() == AST_AsmRegister_t || src->type() == AST_AsmImm_t || asm_type_src->type() == AST_QuadWord_t
        || asm_type_src->type() == AST_BackendDouble_t) {
        push_instr(ctx, std::make_unique<AsmPush>(std::move(src)));
    }
    else {
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        {
            std::shared_ptr<AsmOperand> dst_cp = dst;
            push_instr(ctx, std::make_unique<AsmPush>(std::move(dst_cp)));
        }
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
}

static void reg_8b_arg_call_instr(
    Ctx ctx, TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_reg) {
    ret_8b_instr(ctx, name, offset, struct_type, arg_reg);
}

static void quad_stack_arg_call_instr(Ctx ctx, TIdentifier name, TLong offset) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = std::make_shared<AsmPseudoMem>(src_name, from_offset);
    }
    push_instr(ctx, std::make_unique<AsmPush>(std::move(src)));
}

static void long_stack_arg_call_instr(
    Ctx ctx, TIdentifier name, TLong offset, std::shared_ptr<AssemblyType>* asm_type) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = std::make_shared<AsmPseudoMem>(src_name, from_offset);
    }
    std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
    {
        std::shared_ptr<AsmOperand> dst_cp = dst;
        push_instr(ctx, std::make_unique<AsmPush>(std::move(dst_cp)));
    }
    std::shared_ptr<AssemblyType> asm_type_src = std::move(*asm_type);
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
}

static void bytearr_stack_arg_call_instr(Ctx ctx, TIdentifier name, TLong offset, ByteArray* bytearr_type) {
    {
        TLong to_offset = 0l;
        TLong size = bytearr_type->size;
        vector_t(std::unique_ptr<AsmInstruction>) byte_instrs = vec_new();
        while (size > 0l) {
            std::unique_ptr<AsmInstruction> byte_instr;
            {
                std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(name, offset);
                std::shared_ptr<AsmOperand> dst = gen_memory(REG_Sp, to_offset);
                std::shared_ptr<AssemblyType> asm_type_src;
                if (size >= 4l) {
                    asm_type_src = std::make_shared<LongWord>();
                    size -= 4l;
                    offset += 4l;
                    to_offset += 4l;
                }
                else {
                    asm_type_src = std::make_shared<Byte>();
                    size--;
                    offset++;
                    to_offset++;
                }
                byte_instr = std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst));
            }
            vec_move_back(byte_instrs, byte_instr);
        }
        for (size_t i = vec_size(byte_instrs); i-- > 0;) {
            push_instr(ctx, std::move(byte_instrs[i]));
        }
        vec_delete(byte_instrs);
    }
    {
        std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmSub>();
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Sp);
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
        push_instr(ctx,
            std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type_src), std::move(src), std::move(dst)));
    }
}

static void stack_8b_arg_call_instr(Ctx ctx, TIdentifier name, TLong offset, Structure* struct_type) {
    std::shared_ptr<AssemblyType> asm_type = asm_type_8b(ctx, struct_type, offset);
    switch (asm_type->type()) {
        case AST_QuadWord_t:
            quad_stack_arg_call_instr(ctx, name, offset);
            break;
        case AST_ByteArray_t:
            bytearr_stack_arg_call_instr(ctx, name, offset, static_cast<ByteArray*>(asm_type.get()));
            break;
        default:
            long_stack_arg_call_instr(ctx, name, offset, &asm_type);
            break;
    }
}

static TLong arg_call_instr(Ctx ctx, TacFunCall* node, FunType* fun_type, bool is_ret_memory) {
    size_t reg_size = is_ret_memory ? 1 : 0;
    size_t sse_size = 0;
    TLong stack_padding = 0l;
    vector_t(std::unique_ptr<AsmInstruction>) stack_instrs = vec_new();
    vector_t(std::unique_ptr<AsmInstruction>)* p_instrs = ctx->p_instrs;
    for (size_t i = 0; i < vec_size(node->args); ++i) {
        TacValue* arg = node->args[i].get();
        if (is_value_dbl(ctx, arg)) {
            if (sse_size < 8) {
                reg_arg_call_instr(ctx, arg, ctx->sse_arg_regs[sse_size]);
                sse_size++;
            }
            else {
                ctx->p_instrs = &stack_instrs;
                stack_arg_call_instr(ctx, arg);
                ctx->p_instrs = p_instrs;
                stack_padding++;
            }
        }
        else if (!is_value_struct(ctx, arg)) {
            if (reg_size < 6) {
                reg_arg_call_instr(ctx, arg, ctx->arg_regs[reg_size]);
                reg_size++;
            }
            else {
                ctx->p_instrs = &stack_instrs;
                stack_arg_call_instr(ctx, arg);
                ctx->p_instrs = p_instrs;
                stack_padding++;
            }
        }
        else {
            size_t struct_reg_size = 7;
            size_t struct_sse_size = 9;
            TIdentifier name = static_cast<TacVariable*>(arg)->name;
            Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, name)->type_t.get());
            struct_8b_class(ctx, struct_type);
            const vector_t(STRUCT_8B_CLS) struct_8b_cls = map_get(ctx->struct_8b_cls_map, struct_type->tag);
            if (struct_8b_cls[0] != CLS_memory) {
                struct_reg_size = 0;
                struct_sse_size = 0;
                for (size_t j = 0; j < vec_size(struct_8b_cls); ++j) {
                    if (struct_8b_cls[j] == CLS_sse) {
                        struct_sse_size++;
                    }
                    else {
                        struct_reg_size++;
                    }
                }
            }
            if (struct_reg_size + reg_size <= 6 && struct_sse_size + sse_size <= 8) {
                TLong offset = 0l;
                for (size_t j = 0; j < vec_size(struct_8b_cls); ++j) {
                    if (struct_8b_cls[j] == CLS_sse) {
                        reg_8b_arg_call_instr(ctx, name, offset, NULL, ctx->sse_arg_regs[sse_size]);
                        sse_size++;
                    }
                    else {
                        reg_8b_arg_call_instr(ctx, name, offset, struct_type, ctx->arg_regs[reg_size]);
                        reg_size++;
                    }
                    offset += 8l;
                }
            }
            else {
                TLong offset = 0l;
                ctx->p_instrs = &stack_instrs;
                for (size_t j = 0; j < vec_size(struct_8b_cls); ++j) {
                    stack_8b_arg_call_instr(ctx, name, offset, struct_type);
                    offset += 8l;
                    stack_padding++;
                }
                ctx->p_instrs = p_instrs;
            }
        }
    }
    fun_param_reg_mask(ctx, fun_type, reg_size, sse_size);
    if (stack_padding % 2l == 1l) {
        alloc_stack_instr(ctx, 8l);
        stack_padding++;
    }
    stack_padding *= 8l;
    for (size_t i = vec_size(stack_instrs); i-- > 0;) {
        push_instr(ctx, std::move(stack_instrs[i]));
    }
    vec_delete(stack_instrs);
    return stack_padding;
}

static void ret_call_instr(Ctx ctx, TacValue* node, REGISTER_KIND arg_reg) {
    std::shared_ptr<AsmOperand> src = gen_register(arg_reg);
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node);
    std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node);
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void ret_8b_call_instr(Ctx ctx, TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_reg) {
    TIdentifier dst_name = name;
    std::shared_ptr<AsmOperand> src = gen_register(arg_reg);
    std::shared_ptr<AssemblyType> asm_type_dst =
        struct_type ? asm_type_8b(ctx, struct_type, offset) : std::make_shared<BackendDouble>();
    if (asm_type_dst->type() == AST_ByteArray_t) {
        TLong size = static_cast<ByteArray*>(asm_type_dst.get())->size + offset - 2l;
        asm_type_dst = std::make_shared<Byte>();
        std::shared_ptr<AsmOperand> src_shr2op = std::make_shared<AsmImm>(8ul, true, false, false);
        std::shared_ptr<AssemblyType> asm_type_shr2op = std::make_shared<QuadWord>();
        while (offset < size) {
            {
                std::shared_ptr<AsmOperand> src_cp = src;
                std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
                std::shared_ptr<AssemblyType> asm_type_dst_cp = asm_type_dst;
                push_instr(
                    ctx, std::make_unique<AsmMov>(std::move(asm_type_dst_cp), std::move(src_cp), std::move(dst)));
            }
            {
                std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmBitShiftRight>();
                std::shared_ptr<AsmOperand> src_shr2op_cp = src_shr2op;
                std::shared_ptr<AsmOperand> src_cp = src;
                std::shared_ptr<AssemblyType> asm_type_shr2op_cp = asm_type_shr2op;
                push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type_shr2op_cp),
                                    std::move(src_shr2op_cp), std::move(src_cp)));
            }
            offset++;
        }
        {
            std::shared_ptr<AsmOperand> src_cp = src;
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
            std::shared_ptr<AssemblyType> asm_type_dst_cp = asm_type_dst;
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst_cp), std::move(src_cp), std::move(dst)));
        }
        {
            std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmBitShiftRight>();
            std::shared_ptr<AsmOperand> src_cp = src;
            push_instr(ctx, std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type_shr2op),
                                std::move(src_shr2op), std::move(src_cp)));
        }
        offset++;
        {
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
        }
    }
    else {
        std::shared_ptr<AsmOperand> dst;
        {
            TLong to_offset = offset;
            dst = std::make_shared<AsmPseudoMem>(dst_name, to_offset);
        }
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
    }
}

static void call_instr(Ctx ctx, TacFunCall* node) {
    bool is_ret_memory = false;
    FunType* fun_type = static_cast<FunType*>(map_get(ctx->frontend->symbol_table, node->name)->type_t.get());
    {
        if (node->dst && is_value_struct(ctx, node->dst.get())) {
            TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
            Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, name)->type_t.get());
            struct_8b_class(ctx, struct_type);
            if (map_get(ctx->struct_8b_cls_map, struct_type->tag)[0] == CLS_memory) {
                is_ret_memory = true;
                {
                    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->dst.get());
                    std::shared_ptr<AsmOperand> dst = gen_register(REG_Di);
                    push_instr(ctx, std::make_unique<AsmLea>(std::move(src), std::move(dst)));
                }
            }
        }
        TLong stack_padding = arg_call_instr(ctx, node, fun_type, is_ret_memory);

        {
            TIdentifier name = node->name;
            push_instr(ctx, std::make_unique<AsmCall>(name));
        }

        if (stack_padding > 0l) {
            dealloc_stack_instr(ctx, stack_padding);
        }
    }

    if (node->dst) {
        if (is_ret_memory) {
            ret_1_reg_mask(fun_type, true);
        }
        else {
            if (is_value_dbl(ctx, node->dst.get())) {
                ret_call_instr(ctx, node->dst.get(), REG_Xmm0);
                ret_1_reg_mask(fun_type, false);
            }
            else if (!is_value_struct(ctx, node->dst.get())) {
                ret_call_instr(ctx, node->dst.get(), REG_Ax);
                ret_1_reg_mask(fun_type, true);
            }
            else {
                bool reg_size = false;
                TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
                Structure* struct_type =
                    static_cast<Structure*>(map_get(ctx->frontend->symbol_table, name)->type_t.get());
                const vector_t(STRUCT_8B_CLS) struct_8b_cls = map_get(ctx->struct_8b_cls_map, struct_type->tag);
                switch (struct_8b_cls[0]) {
                    case CLS_integer: {
                        ret_8b_call_instr(ctx, name, 0l, struct_type, REG_Ax);
                        reg_size = true;
                        break;
                    }
                    case CLS_sse:
                        ret_8b_call_instr(ctx, name, 0l, NULL, REG_Xmm0);
                        break;
                    default:
                        THROW_ABORT;
                }
                if (vec_size(struct_8b_cls) == 2) {
                    bool sse_size = !reg_size;
                    switch (struct_8b_cls[1]) {
                        case CLS_integer:
                            ret_8b_call_instr(ctx, name, 8l, struct_type, reg_size ? REG_Dx : REG_Ax);
                            break;
                        case CLS_sse: {
                            ret_8b_call_instr(ctx, name, 8l, NULL, sse_size ? REG_Xmm1 : REG_Xmm0);
                            sse_size = true;
                            break;
                        }
                        default:
                            THROW_ABORT;
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

static void zero_xmm_reg_instr(Ctx ctx) {
    std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmBitXor>();
    std::shared_ptr<AsmOperand> src = gen_register(REG_Xmm0);
    std::shared_ptr<AsmOperand> src_cp = src;
    std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<BackendDouble>();
    push_instr(
        ctx, std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type_src), std::move(src), std::move(src_cp)));
}

static void unop_int_arithmetic_instr(Ctx ctx, TacUnary* node) {
    std::shared_ptr<AsmOperand> src_dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->src.get());
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src_cp = asm_type_src;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src_cp), std::move(src), std::move(src_dst_cp)));
    }
    {
        std::unique_ptr<AsmUnaryOp> unop = gen_unop(node->unop.get());
        push_instr(ctx, std::make_unique<AsmUnary>(std::move(unop), std::move(asm_type_src), std::move(src_dst)));
    }
}

static void unop_dbl_neg_instr(Ctx ctx, TacUnary* node) {
    std::shared_ptr<AsmOperand> src1_dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src1 = std::make_shared<BackendDouble>();
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> src1_dst_cp = src1_dst;
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(src1), std::move(src1_dst_cp)));
    }
    {
        std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmBitXor>();
        std::shared_ptr<AsmOperand> src2 = dbl_static_const_op(ctx, 9223372036854775808ul, 16);
        push_instr(ctx, std::make_unique<AsmBinary>(
                            std::move(binop), std::move(asm_type_src1), std::move(src2), std::move(src1_dst)));
    }
}

static void unop_neg_instr(Ctx ctx, TacUnary* node) {
    if (is_value_dbl(ctx, node->src.get())) {
        unop_dbl_neg_instr(ctx, node);
    }
    else {
        unop_int_arithmetic_instr(ctx, node);
    }
}

static void unop_int_conditional_instr(Ctx ctx, TacUnary* node) {
    std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(ctx, node->dst.get());
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> imm_zero_cp = imm_zero;
        std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->src.get());
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_src), std::move(imm_zero_cp), std::move(src)));
    }
    {
        std::shared_ptr<AsmOperand> cmp_dst_cp = cmp_dst;
        std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(imm_zero), std::move(cmp_dst_cp)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(ctx, std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
    }
}

static void unop_dbl_conditional_instr(Ctx ctx, TacUnary* node) {
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(ctx, node->dst.get());
    zero_xmm_reg_instr(ctx);
    {
        std::shared_ptr<AsmOperand> reg_zero = gen_register(REG_Xmm0);
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<BackendDouble>();
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_src), std::move(reg_zero), std::move(src)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> cmp_dst_cp = cmp_dst;
        std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<LongWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(imm_zero), std::move(cmp_dst_cp)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(ctx, std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
    }
    push_instr(ctx, std::make_unique<AsmLabel>(target_nan));
}

static void unop_conditional_instr(Ctx ctx, TacUnary* node) {
    if (is_value_dbl(ctx, node->src.get())) {
        unop_dbl_conditional_instr(ctx, node);
    }
    else {
        unop_int_conditional_instr(ctx, node);
    }
}

static void unary_instr(Ctx ctx, TacUnary* node) {
    switch (node->unop->type()) {
        case AST_TacComplement_t:
            unop_int_arithmetic_instr(ctx, node);
            break;
        case AST_TacNegate_t:
            unop_neg_instr(ctx, node);
            break;
        case AST_TacNot_t:
            unop_conditional_instr(ctx, node);
            break;
        default:
            THROW_ABORT;
    }
}

static void binop_arithmetic_instr(Ctx ctx, TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src1 = gen_asm_type(ctx, node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst_cp = src1_dst;
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(src1), std::move(src1_dst_cp)));
    }
    {
        std::unique_ptr<AsmBinaryOp> binop = gen_binop(node->binop.get());
        std::shared_ptr<AsmOperand> src2 = gen_op(ctx, node->src2.get());
        push_instr(ctx, std::make_unique<AsmBinary>(
                            std::move(binop), std::move(asm_type_src1), std::move(src2), std::move(src1_dst)));
    }
}

static void signed_divide_instr(Ctx ctx, TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = gen_register(REG_Ax);
    std::shared_ptr<AssemblyType> asm_type_src1 = gen_asm_type(ctx, node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst_cp = src1_dst;
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(src1), std::move(src1_dst_cp)));
    }
    {
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmCdq>(std::move(asm_type_src1_cp)));
    }
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(ctx, node->src2.get());
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmIdiv>(std::move(asm_type_src1_cp), std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1), std::move(src1_dst), std::move(dst)));
    }
}

static void unsigned_divide_instr(Ctx ctx, TacBinary* node) {
    std::shared_ptr<AsmOperand> src1_dst = gen_register(REG_Ax);
    std::shared_ptr<AssemblyType> asm_type_src1 = gen_asm_type(ctx, node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst_cp = src1_dst;
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(src1), std::move(src1_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> imm_zero_dst = gen_register(REG_Dx);
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(
            ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(imm_zero), std::move(imm_zero_dst)));
    }
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(ctx, node->src2.get());
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmDiv>(std::move(asm_type_src1_cp), std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1), std::move(src1_dst), std::move(dst)));
    }
}

static void binop_divide_instr(Ctx ctx, TacBinary* node) {
    if (is_value_dbl(ctx, node->src1.get())) {
        binop_arithmetic_instr(ctx, node);
    }
    else if (is_value_signed(ctx, node->src1.get())) {
        signed_divide_instr(ctx, node);
    }
    else {
        unsigned_divide_instr(ctx, node);
    }
}

static void signed_remainder_instr(Ctx ctx, TacBinary* node) {
    std::shared_ptr<AssemblyType> asm_type_src1 = gen_asm_type(ctx, node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(src1), std::move(src1_dst)));
    }
    {
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmCdq>(std::move(asm_type_src1_cp)));
    }
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(ctx, node->src2.get());
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmIdiv>(std::move(asm_type_src1_cp), std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst_src = gen_register(REG_Dx);
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1), std::move(dst_src), std::move(dst)));
    }
}

static void unsigned_remainder_instr(Ctx ctx, TacBinary* node) {
    std::shared_ptr<AsmOperand> dst_src = gen_register(REG_Dx);
    std::shared_ptr<AssemblyType> asm_type_src1 = gen_asm_type(ctx, node->src1.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src1.get());
        std::shared_ptr<AsmOperand> src1_dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(src1), std::move(src1_dst)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> dst_src_cp = dst_src;
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(
            ctx, std::make_unique<AsmMov>(std::move(asm_type_src1_cp), std::move(imm_zero), std::move(dst_src_cp)));
    }
    {
        std::shared_ptr<AsmOperand> src2 = gen_op(ctx, node->src2.get());
        std::shared_ptr<AssemblyType> asm_type_src1_cp = asm_type_src1;
        push_instr(ctx, std::make_unique<AsmDiv>(std::move(asm_type_src1_cp), std::move(src2)));
    }
    {
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src1), std::move(dst_src), std::move(dst)));
    }
}

static void binop_remainder_instr(Ctx ctx, TacBinary* node) {
    if (is_value_signed(ctx, node->src1.get())) {
        signed_remainder_instr(ctx, node);
    }
    else {
        unsigned_remainder_instr(ctx, node);
    }
}

static void binop_int_conditional_instr(Ctx ctx, TacBinary* node) {
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(ctx, node->dst.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src1.get());
        std::shared_ptr<AsmOperand> src2 = gen_op(ctx, node->src2.get());
        std::shared_ptr<AssemblyType> asm_type_src1 = gen_asm_type(ctx, node->src1.get());
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_src1), std::move(src2), std::move(src1)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> cmp_dst_cp = cmp_dst;
        std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(imm_zero), std::move(cmp_dst_cp)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code;
        if (is_value_signed(ctx, node->src1.get())) {
            cond_code = gen_signed_cond_code(node->binop.get());
        }
        else {
            cond_code = gen_unsigned_cond_code(node->binop.get());
        }
        push_instr(ctx, std::make_unique<AsmSetCC>(std::move(cond_code), std::move(cmp_dst)));
    }
}

static void binop_dbl_conditional_instr(Ctx ctx, TacBinary* node) {
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    std::shared_ptr<AsmOperand> cmp_dst = gen_op(ctx, node->dst.get());
    {
        std::shared_ptr<AsmOperand> src1 = gen_op(ctx, node->src1.get());
        std::shared_ptr<AsmOperand> src2 = gen_op(ctx, node->src2.get());
        std::shared_ptr<AssemblyType> asm_type_src1 = gen_asm_type(ctx, node->src1.get());
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_src1), std::move(src2), std::move(src1)));
    }
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> cmp_dst_cp = cmp_dst;
        std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<LongWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(imm_zero), std::move(cmp_dst_cp)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code = gen_unsigned_cond_code(node->binop.get());
        if (cond_code->type() == AST_AsmNE_t) {
            TIdentifier target_nan_ne = repr_asm_label(ctx, LBL_Lcomisd_nan);
            {
                std::shared_ptr<AsmOperand> cmp_dst_cp = cmp_dst;
                push_instr(ctx, std::make_unique<AsmSetCC>(std::move(cond_code), std::move(cmp_dst_cp)));
            }
            push_instr(ctx, std::make_unique<AsmJmp>(target_nan_ne));
            push_instr(ctx, std::make_unique<AsmLabel>(target_nan));
            {
                std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
                push_instr(ctx, std::make_unique<AsmSetCC>(std::move(cond_code_e), std::move(cmp_dst)));
            }
            push_instr(ctx, std::make_unique<AsmLabel>(target_nan_ne));
        }
        else {
            push_instr(ctx, std::make_unique<AsmSetCC>(std::move(cond_code), std::move(cmp_dst)));
            push_instr(ctx, std::make_unique<AsmLabel>(target_nan));
        }
    }
}

static void binop_conditional_instr(Ctx ctx, TacBinary* node) {
    if (is_value_dbl(ctx, node->src1.get())) {
        binop_dbl_conditional_instr(ctx, node);
    }
    else {
        binop_int_conditional_instr(ctx, node);
    }
}

static void binary_instr(Ctx ctx, TacBinary* node) {
    switch (node->binop->type()) {
        case AST_TacAdd_t:
        case AST_TacSubtract_t:
        case AST_TacMultiply_t:
        case AST_TacBitAnd_t:
        case AST_TacBitOr_t:
        case AST_TacBitXor_t:
        case AST_TacBitShiftLeft_t:
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t:
            binop_arithmetic_instr(ctx, node);
            break;
        case AST_TacDivide_t:
            binop_divide_instr(ctx, node);
            break;
        case AST_TacRemainder_t:
            binop_remainder_instr(ctx, node);
            break;
        case AST_TacEqual_t:
        case AST_TacNotEqual_t:
        case AST_TacLessThan_t:
        case AST_TacLessOrEqual_t:
        case AST_TacGreaterThan_t:
        case AST_TacGreaterOrEqual_t:
            binop_conditional_instr(ctx, node);
            break;
        default:
            THROW_ABORT;
    }
}

static void copy_struct_instr(Ctx ctx, TacCopy* node) {
    TIdentifier src_name = static_cast<TacVariable*>(node->src.get())->name;
    TIdentifier dst_name = static_cast<TacVariable*>(node->dst.get())->name;
    Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, src_name)->type_t.get());
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    TLong offset = 0l;
    while (size > 0l) {
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
        std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
        std::shared_ptr<AssemblyType> asm_type_src;
        if (size >= 8l) {
            asm_type_src = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            asm_type_src = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            asm_type_src = std::make_shared<Byte>();
            size--;
            offset++;
        }
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
}

static void copy_scalar_instr(Ctx ctx, TacCopy* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->src.get());
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
}

static void copy_instr(Ctx ctx, TacCopy* node) {
    if (is_value_struct(ctx, node->src.get())) {
        copy_struct_instr(ctx, node);
    }
    else {
        copy_scalar_instr(ctx, node);
    }
}

static void getaddr_instr(Ctx ctx, TacGetAddress* node) {
    std::shared_ptr<AsmOperand> src;
    {
        if (node->src->type() == AST_TacVariable_t) {
            TIdentifier name = static_cast<TacVariable*>(node->src.get())->name;
            set_insert(ctx->frontend->addressed_set, name);
            if (map_find(ctx->frontend->symbol_table, name) != map_end()
                && map_get(ctx->frontend->symbol_table, name)->attrs->type() == AST_ConstantAttr_t) {
                src = std::make_shared<AsmData>(name, 0l);
                goto Lpass;
            }
        }
        src = gen_op(ctx, node->src.get());
    Lpass:;
    }
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    push_instr(ctx, std::make_unique<AsmLea>(std::move(src), std::move(dst)));
}

static void load_struct_instr(Ctx ctx, TacLoad* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
    {
        TIdentifier name = static_cast<TacVariable*>(node->dst.get())->name;
        Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, name)->type_t.get());
        TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
        TLong offset = 0l;
        while (size > 0l) {
            std::shared_ptr<AsmOperand> src = gen_memory(REG_Ax, offset);
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(name, offset);
            std::shared_ptr<AssemblyType> asm_type_dst;
            if (size >= 8l) {
                asm_type_dst = std::make_shared<QuadWord>();
                size -= 8l;
                offset += 8l;
            }
            else if (size >= 4l) {
                asm_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
            }
            else {
                asm_type_dst = std::make_shared<Byte>();
                size--;
                offset++;
            }
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
        }
    }
}

static void load_scalar_instr(Ctx ctx, TacLoad* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_memory(REG_Ax, 0l);
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
    }
}

static void load_instr(Ctx ctx, TacLoad* node) {
    if (is_value_struct(ctx, node->dst.get())) {
        load_struct_instr(ctx, node);
    }
    else {
        load_scalar_instr(ctx, node);
    }
}

static void store_struct_instr(Ctx ctx, TacStore* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->dst_ptr.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
    {
        TIdentifier name = static_cast<TacVariable*>(node->src.get())->name;
        Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, name)->type_t.get());
        TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
        TLong offset = 0l;
        while (size > 0l) {
            std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(name, offset);
            std::shared_ptr<AsmOperand> dst = gen_memory(REG_Ax, offset);
            std::shared_ptr<AssemblyType> asm_type_dst;
            if (size >= 8l) {
                asm_type_dst = std::make_shared<QuadWord>();
                size -= 8l;
                offset += 8l;
            }
            else if (size >= 4l) {
                asm_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
            }
            else {
                asm_type_dst = std::make_shared<Byte>();
                size--;
                offset++;
            }
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
        }
    }
}

static void store_scalar_instr(Ctx ctx, TacStore* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->dst_ptr.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
        std::shared_ptr<AsmOperand> dst = gen_memory(REG_Ax, 0l);
        std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node->src.get());
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
    }
}

static void store_instr(Ctx ctx, TacStore* node) {
    if (is_value_struct(ctx, node->src.get())) {
        store_struct_instr(ctx, node);
    }
    else {
        store_scalar_instr(ctx, node);
    }
}

static void const_idx_add_ptr_instr(Ctx ctx, TacAddPtr* node) {
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
    {
        TLong idx;
        {
            CConst* constant = static_cast<TacConstant*>(node->idx.get())->constant.get();
            THROW_ABORT_IF(constant->type() != AST_CConstLong_t);
            idx = static_cast<CConstLong*>(constant)->value;
        }
        std::shared_ptr<AsmOperand> src = gen_memory(REG_Ax, idx * node->scale);
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void scalar_idx_add_ptr_instr(Ctx ctx, TacAddPtr* node) {
    std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src_cp = asm_type_src;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src_cp), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->idx.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Dx);
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_indexed(REG_Ax, REG_Dx, node->scale);
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void aggr_idx_add_ptr_instr(Ctx ctx, TacAddPtr* node) {
    std::shared_ptr<AssemblyType> asm_type_src = std::make_shared<QuadWord>();
    std::shared_ptr<AsmOperand> src_dst = gen_register(REG_Dx);
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src_ptr.get());
        std::shared_ptr<AsmOperand> dst = gen_register(REG_Ax);
        std::shared_ptr<AssemblyType> asm_type_src_cp = asm_type_src;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src_cp), std::move(src), std::move(dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_op(ctx, node->idx.get());
        std::shared_ptr<AsmOperand> src_dst_cp = src_dst;
        std::shared_ptr<AssemblyType> asm_type_src_cp = asm_type_src;
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src_cp), std::move(src), std::move(src_dst_cp)));
    }
    {
        std::shared_ptr<AsmOperand> src;
        {
            TULong value = (TULong)node->scale;
            bool is_byte = node->scale <= 127l && node->scale >= -128l;
            bool is_quad = node->scale > 2147483647l || node->scale < -2147483648l;
            bool is_neg = node->scale < 0l;
            src = std::make_shared<AsmImm>(value, is_byte, is_quad, is_neg);
        }
        std::unique_ptr<AsmBinaryOp> binop = std::make_unique<AsmMult>();
        push_instr(ctx,
            std::make_unique<AsmBinary>(std::move(binop), std::move(asm_type_src), std::move(src), std::move(src_dst)));
    }
    {
        std::shared_ptr<AsmOperand> src = gen_indexed(REG_Ax, REG_Dx, 1l);
        std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
        push_instr(ctx, std::make_unique<AsmLea>(std::move(src), std::move(dst)));
    }
}

static void var_idx_add_ptr_instr(Ctx ctx, TacAddPtr* node) {
    switch (node->scale) {
        case 1l:
        case 2l:
        case 4l:
        case 8l:
            scalar_idx_add_ptr_instr(ctx, node);
            break;
        default:
            aggr_idx_add_ptr_instr(ctx, node);
            break;
    }
}

static void add_ptr_instr(Ctx ctx, TacAddPtr* node) {
    switch (node->idx->type()) {
        case AST_TacConstant_t:
            const_idx_add_ptr_instr(ctx, node);
            break;
        case AST_TacVariable_t:
            var_idx_add_ptr_instr(ctx, node);
            break;
        default:
            THROW_ABORT;
    }
}

static void cp_to_offset_struct_instr(Ctx ctx, TacCopyToOffset* node) {
    TIdentifier src_name = static_cast<TacVariable*>(node->src.get())->name;
    Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, src_name)->type_t.get());
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    TLong offset = 0l;
    while (size > 0l) {
        std::shared_ptr<AsmOperand> src = std::make_shared<AsmPseudoMem>(src_name, offset);
        std::shared_ptr<AsmOperand> dst;
        {
            TIdentifier dst_name = node->dst_name;
            TLong to_offset = offset + node->offset;
            dst = std::make_shared<AsmPseudoMem>(dst_name, to_offset);
        }
        std::shared_ptr<AssemblyType> asm_type_src;
        if (size >= 8l) {
            asm_type_src = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            asm_type_src = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            asm_type_src = std::make_shared<Byte>();
            size--;
            offset++;
        }
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
    }
}

static void cp_to_offset_scalar_instr(Ctx ctx, TacCopyToOffset* node) {
    std::shared_ptr<AsmOperand> src = gen_op(ctx, node->src.get());
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = node->dst_name;
        TLong to_offset = node->offset;
        dst = std::make_shared<AsmPseudoMem>(dst_name, to_offset);
    }
    std::shared_ptr<AssemblyType> asm_type_src = gen_asm_type(ctx, node->src.get());
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_src), std::move(src), std::move(dst)));
}

static void cp_to_offset_instr(Ctx ctx, TacCopyToOffset* node) {
    if (is_value_struct(ctx, node->src.get())) {
        cp_to_offset_struct_instr(ctx, node);
    }
    else {
        cp_to_offset_scalar_instr(ctx, node);
    }
}

static void cp_from_offset_struct_instr(Ctx ctx, TacCopyFromOffset* node) {
    TIdentifier dst_name = static_cast<TacVariable*>(node->dst.get())->name;
    Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, dst_name)->type_t.get());
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    TLong offset = 0l;
    while (size > 0l) {
        std::shared_ptr<AsmOperand> src;
        {
            TIdentifier src_name = node->src_name;
            TLong from_offset = offset + node->offset;
            src = std::make_shared<AsmPseudoMem>(src_name, from_offset);
        }
        std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(dst_name, offset);
        std::shared_ptr<AssemblyType> asm_type_dst;
        if (size >= 8l) {
            asm_type_dst = std::make_shared<QuadWord>();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            asm_type_dst = std::make_shared<LongWord>();
            size -= 4l;
            offset += 4l;
        }
        else {
            asm_type_dst = std::make_shared<Byte>();
            size--;
            offset++;
        }
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
    }
}

static void cp_from_offset_scalar_instr(Ctx ctx, TacCopyFromOffset* node) {
    std::shared_ptr<AsmOperand> src;
    {
        TIdentifier src_name = node->src_name;
        TLong from_offset = node->offset;
        src = std::make_shared<AsmPseudoMem>(src_name, from_offset);
    }
    std::shared_ptr<AsmOperand> dst = gen_op(ctx, node->dst.get());
    std::shared_ptr<AssemblyType> asm_type_dst = gen_asm_type(ctx, node->dst.get());
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void cp_from_offset_instr(Ctx ctx, TacCopyFromOffset* node) {
    if (is_value_struct(ctx, node->dst.get())) {
        cp_from_offset_struct_instr(ctx, node);
    }
    else {
        cp_from_offset_scalar_instr(ctx, node);
    }
}

static void jump_instr(Ctx ctx, TacJump* node) {
    TIdentifier target = node->target;
    push_instr(ctx, std::make_unique<AsmJmp>(target));
}

static void jmp_eq_0_int_instr(Ctx ctx, TacJumpIfZero* node) {
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> condition = gen_op(ctx, node->condition.get());
        std::shared_ptr<AssemblyType> asm_type_cond = gen_asm_type(ctx, node->condition.get());
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_cond), std::move(imm_zero), std::move(condition)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target, std::move(cond_code_e)));
    }
}

static void jmp_eq_0_dbl_instr(Ctx ctx, TacJumpIfZero* node) {
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    zero_xmm_reg_instr(ctx);
    {
        std::shared_ptr<AsmOperand> condition = gen_op(ctx, node->condition.get());
        std::shared_ptr<AsmOperand> reg_zero = gen_register(REG_Xmm0);
        std::shared_ptr<AssemblyType> asm_type_cond = std::make_shared<BackendDouble>();
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_cond), std::move(condition), std::move(reg_zero)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target, std::move(cond_code_e)));
    }
    push_instr(ctx, std::make_unique<AsmLabel>(target_nan));
}

static void jmp_eq_0_instr(Ctx ctx, TacJumpIfZero* node) {
    if (is_value_dbl(ctx, node->condition.get())) {
        jmp_eq_0_dbl_instr(ctx, node);
    }
    else {
        jmp_eq_0_int_instr(ctx, node);
    }
}

static void jmp_ne_0_int_instr(Ctx ctx, TacJumpIfNotZero* node) {
    {
        std::shared_ptr<AsmOperand> imm_zero = std::make_shared<AsmImm>(0ul, true, false, false);
        std::shared_ptr<AsmOperand> condition = gen_op(ctx, node->condition.get());
        std::shared_ptr<AssemblyType> asm_type_cond = gen_asm_type(ctx, node->condition.get());
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_cond), std::move(imm_zero), std::move(condition)));
    }
    {
        TIdentifier target = node->target;
        std::unique_ptr<AsmCondCode> cond_code_ne = std::make_unique<AsmNE>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target, std::move(cond_code_ne)));
    }
}

static void jmp_ne_0_dbl_instr(Ctx ctx, TacJumpIfNotZero* node) {
    TIdentifier target = node->target;
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    TIdentifier target_nan_ne = repr_asm_label(ctx, LBL_Lcomisd_nan);
    zero_xmm_reg_instr(ctx);
    {
        std::shared_ptr<AsmOperand> condition = gen_op(ctx, node->condition.get());
        std::shared_ptr<AsmOperand> reg_zero = gen_register(REG_Xmm0);
        std::shared_ptr<AssemblyType> asm_type_cond = std::make_shared<BackendDouble>();
        push_instr(ctx, std::make_unique<AsmCmp>(std::move(asm_type_cond), std::move(condition), std::move(reg_zero)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_p = std::make_unique<AsmP>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target_nan, std::move(cond_code_p)));
    }
    {
        std::unique_ptr<AsmCondCode> cond_code_ne = std::make_unique<AsmNE>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target, std::move(cond_code_ne)));
    }
    push_instr(ctx, std::make_unique<AsmJmp>(target_nan_ne));
    push_instr(ctx, std::make_unique<AsmLabel>(target_nan));
    {
        std::unique_ptr<AsmCondCode> cond_code_e = std::make_unique<AsmE>();
        push_instr(ctx, std::make_unique<AsmJmpCC>(target, std::move(cond_code_e)));
    }
    push_instr(ctx, std::make_unique<AsmLabel>(target_nan_ne));
}

static void jmp_ne_0_instr(Ctx ctx, TacJumpIfNotZero* node) {
    if (is_value_dbl(ctx, node->condition.get())) {
        jmp_ne_0_dbl_instr(ctx, node);
    }
    else {
        jmp_ne_0_int_instr(ctx, node);
    }
}

static void label_instr(Ctx ctx, TacLabel* node) {
    TIdentifier name = node->name;
    push_instr(ctx, std::make_unique<AsmLabel>(name));
}

static void gen_instr(Ctx ctx, TacInstruction* node) {
    switch (node->type()) {
        case AST_TacReturn_t:
            ret_instr(ctx, static_cast<TacReturn*>(node));
            break;
        case AST_TacSignExtend_t:
            sign_extend_instr(ctx, static_cast<TacSignExtend*>(node));
            break;
        case AST_TacTruncate_t:
            truncate_instr(ctx, static_cast<TacTruncate*>(node));
            break;
        case AST_TacZeroExtend_t:
            zero_extend_instr(ctx, static_cast<TacZeroExtend*>(node));
            break;
        case AST_TacDoubleToInt_t:
            dbl_to_signed_instr(ctx, static_cast<TacDoubleToInt*>(node));
            break;
        case AST_TacDoubleToUInt_t:
            dbl_to_unsigned_instr(ctx, static_cast<TacDoubleToUInt*>(node));
            break;
        case AST_TacIntToDouble_t:
            signed_to_dbl_instr(ctx, static_cast<TacIntToDouble*>(node));
            break;
        case AST_TacUIntToDouble_t:
            unsigned_to_dbl_instr(ctx, static_cast<TacUIntToDouble*>(node));
            break;
        case AST_TacFunCall_t:
            call_instr(ctx, static_cast<TacFunCall*>(node));
            break;
        case AST_TacUnary_t:
            unary_instr(ctx, static_cast<TacUnary*>(node));
            break;
        case AST_TacBinary_t:
            binary_instr(ctx, static_cast<TacBinary*>(node));
            break;
        case AST_TacCopy_t:
            copy_instr(ctx, static_cast<TacCopy*>(node));
            break;
        case AST_TacGetAddress_t:
            getaddr_instr(ctx, static_cast<TacGetAddress*>(node));
            break;
        case AST_TacLoad_t:
            load_instr(ctx, static_cast<TacLoad*>(node));
            break;
        case AST_TacStore_t:
            store_instr(ctx, static_cast<TacStore*>(node));
            break;
        case AST_TacAddPtr_t:
            add_ptr_instr(ctx, static_cast<TacAddPtr*>(node));
            break;
        case AST_TacCopyToOffset_t:
            cp_to_offset_instr(ctx, static_cast<TacCopyToOffset*>(node));
            break;
        case AST_TacCopyFromOffset_t:
            cp_from_offset_instr(ctx, static_cast<TacCopyFromOffset*>(node));
            break;
        case AST_TacJump_t:
            jump_instr(ctx, static_cast<TacJump*>(node));
            break;
        case AST_TacJumpIfZero_t:
            jmp_eq_0_instr(ctx, static_cast<TacJumpIfZero*>(node));
            break;
        case AST_TacJumpIfNotZero_t:
            jmp_ne_0_instr(ctx, static_cast<TacJumpIfNotZero*>(node));
            break;
        case AST_TacLabel_t:
            label_instr(ctx, static_cast<TacLabel*>(node));
            break;
        default:
            THROW_ABORT;
    }
}

// instruction = Mov(assembly_type, operand, operand) | MovSx(assembly_type, assembly_type, operand, operand)
//             | MovZeroExtend(assembly_type, assembly_type, operand, operand) | Lea(operand, operand)
//             | Cvttsd2si(assembly_type, operand, operand) | Cvtsi2sd(assembly_type, operand, operand)
//             | Unary(unary_operator, assembly_type, operand) | Binary(binary_operator, assembly_type, operand,
//             operand) | Cmp(assembly_type, operand, operand) | Idiv(assembly_type, operand) | Div(assembly_type,
//             operand) | Cdq(assembly_type) | Jmp(identifier) | JmpCC(cond_code, identifier) | SetCC(cond_code,
//             operand) | Label(identifier) | Push(operand) | Pop(reg) | Call(identifier) | Ret
static void gen_instr_list(Ctx ctx, const vector_t(std::unique_ptr<TacInstruction>) node_list) {
    for (size_t i = 0; i < vec_size(node_list); ++i) {
        if (node_list[i]) {
            gen_instr(ctx, node_list[i].get());
        }
    }
}

static void reg_fun_param_instr(Ctx ctx, TIdentifier name, REGISTER_KIND arg_reg) {
    std::shared_ptr<AsmOperand> src = gen_register(arg_reg);
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = name;
        dst = std::make_shared<AsmPseudo>(dst_name);
    }
    std::shared_ptr<AssemblyType> asm_type_dst = cvt_backend_asm_type(ctx->frontend, name);
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void stack_fun_param_instr(Ctx ctx, TIdentifier name, TLong stack_bytes) {
    std::shared_ptr<AsmOperand> src = gen_memory(REG_Bp, stack_bytes);
    std::shared_ptr<AsmOperand> dst;
    {
        TIdentifier dst_name = name;
        dst = std::make_shared<AsmPseudo>(dst_name);
    }
    std::shared_ptr<AssemblyType> asm_type_dst = cvt_backend_asm_type(ctx->frontend, name);
    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
}

static void reg_8b_fun_param_instr(
    Ctx ctx, TIdentifier name, TLong offset, Structure* struct_type, REGISTER_KIND arg_reg) {
    ret_8b_call_instr(ctx, name, offset, struct_type, arg_reg);
}

static void stack_8b_fun_param_instr(
    Ctx ctx, TIdentifier name, TLong stack_bytes, TLong offset, Structure* struct_type) {
    std::shared_ptr<AssemblyType> asm_type_dst = asm_type_8b(ctx, struct_type, offset);
    if (asm_type_dst->type() == AST_ByteArray_t) {
        TLong size = static_cast<ByteArray*>(asm_type_dst.get())->size;
        while (size > 0l) {
            std::shared_ptr<AsmOperand> src = gen_memory(REG_Bp, stack_bytes);
            std::shared_ptr<AsmOperand> dst = std::make_shared<AsmPseudoMem>(name, offset);
            if (size >= 4l) {
                asm_type_dst = std::make_shared<LongWord>();
                size -= 4l;
                offset += 4l;
                stack_bytes += 4l;
            }
            else {
                asm_type_dst = std::make_shared<Byte>();
                size--;
                offset++;
                stack_bytes++;
            }
            push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
        }
    }
    else {
        std::shared_ptr<AsmOperand> src = gen_memory(REG_Bp, stack_bytes);
        std::shared_ptr<AsmOperand> dst;
        {
            TIdentifier dst_name = name;
            TLong to_offset = offset;
            dst = std::make_shared<AsmPseudoMem>(dst_name, to_offset);
        }
        push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
    }
}

static void fun_param_toplvl(Ctx ctx, TacFunction* node, FunType* fun_type, bool is_ret_memory) {
    size_t reg_size = is_ret_memory ? 1 : 0;
    size_t sse_size = 0;
    TLong stack_bytes = 16l;
    for (size_t i = 0; i < vec_size(node->params); ++i) {
        TIdentifier param = node->params[i];
        if (map_get(ctx->frontend->symbol_table, param)->type_t->type() == AST_Double_t) {
            if (sse_size < 8) {
                reg_fun_param_instr(ctx, param, ctx->sse_arg_regs[sse_size]);
                sse_size++;
            }
            else {
                stack_fun_param_instr(ctx, param, stack_bytes);
                stack_bytes += 8l;
            }
        }
        else if (map_get(ctx->frontend->symbol_table, param)->type_t->type() != AST_Structure_t) {
            if (reg_size < 6) {
                reg_fun_param_instr(ctx, param, ctx->arg_regs[reg_size]);
                reg_size++;
            }
            else {
                stack_fun_param_instr(ctx, param, stack_bytes);
                stack_bytes += 8l;
            }
        }
        else {
            size_t struct_reg_size = 7;
            size_t struct_sse_size = 9;
            Structure* struct_type = static_cast<Structure*>(map_get(ctx->frontend->symbol_table, param)->type_t.get());
            struct_8b_class(ctx, struct_type);
            const vector_t(STRUCT_8B_CLS) struct_8b_cls = map_get(ctx->struct_8b_cls_map, struct_type->tag);
            if (struct_8b_cls[0] != CLS_memory) {
                struct_reg_size = 0;
                struct_sse_size = 0;
                for (size_t j = 0; j < vec_size(struct_8b_cls); ++j) {
                    if (struct_8b_cls[j] == CLS_sse) {
                        struct_sse_size++;
                    }
                    else {
                        struct_reg_size++;
                    }
                }
            }
            if (struct_reg_size + reg_size <= 6 && struct_sse_size + sse_size <= 8) {
                TLong offset = 0l;
                for (size_t j = 0; j < vec_size(struct_8b_cls); ++j) {
                    if (struct_8b_cls[j] == CLS_sse) {
                        reg_8b_fun_param_instr(ctx, param, offset, NULL, ctx->sse_arg_regs[sse_size]);
                        sse_size++;
                    }
                    else {
                        reg_8b_fun_param_instr(ctx, param, offset, struct_type, ctx->arg_regs[reg_size]);
                        reg_size++;
                    }
                    offset += 8l;
                }
            }
            else {
                TLong offset = 0l;
                for (size_t j = 0; j < vec_size(struct_8b_cls); ++j) {
                    stack_8b_fun_param_instr(ctx, param, stack_bytes, offset, struct_type);
                    stack_bytes += 8l;
                    offset += 8l;
                }
            }
        }
    }
    fun_param_reg_mask(ctx, fun_type, reg_size, sse_size);
}

static std::unique_ptr<AsmFunction> gen_fun_toplvl(Ctx ctx, TacFunction* node) {
    TIdentifier name = node->name;
    bool is_glob = node->is_glob;
    bool is_ret_memory = false;

    vector_t(std::unique_ptr<AsmInstruction>) body = vec_new();
    {
        ctx->p_instrs = &body;

        FunType* fun_type = static_cast<FunType*>(map_get(ctx->frontend->symbol_table, node->name)->type_t.get());
        if (fun_type->ret_type->type() == AST_Structure_t) {
            Structure* struct_type = static_cast<Structure*>(fun_type->ret_type.get());
            struct_8b_class(ctx, struct_type);
            if (map_get(ctx->struct_8b_cls_map, struct_type->tag)[0] == CLS_memory) {
                is_ret_memory = true;
                {
                    std::shared_ptr<AsmOperand> src = gen_register(REG_Di);
                    std::shared_ptr<AsmOperand> dst = gen_memory(REG_Bp, -8l);
                    std::shared_ptr<AssemblyType> asm_type_dst = std::make_shared<QuadWord>();
                    push_instr(ctx, std::make_unique<AsmMov>(std::move(asm_type_dst), std::move(src), std::move(dst)));
                }
            }
        }
        fun_param_toplvl(ctx, node, fun_type, is_ret_memory);

        ctx->p_fun_type = fun_type;
        gen_instr_list(ctx, node->body);
        ctx->p_fun_type = NULL;
        ctx->p_instrs = NULL;
    }

    return std::make_unique<AsmFunction>(name, is_glob, is_ret_memory, &body);
}

static std::unique_ptr<AsmStaticVariable> gen_static_var_toplvl(Ctx ctx, TacStaticVariable* node) {
    TIdentifier name = node->name;
    bool is_glob = node->is_glob;
    TInt alignment = gen_type_alignment(ctx->frontend, node->static_init_type.get());
    vector_t(std::shared_ptr<StaticInit>) static_inits = vec_new();
    vec_reserve(static_inits, vec_size(node->static_inits));
    for (size_t i = 0; i < vec_size(node->static_inits); ++i) {
        std::shared_ptr<StaticInit> static_init = node->static_inits[i];
        vec_move_back(static_inits, static_init);
    }
    return std::make_unique<AsmStaticVariable>(name, alignment, is_glob, &static_inits);
}

static void push_static_const_toplvl(Ctx ctx, std::unique_ptr<AsmTopLevel>&& static_const_toplvls) {
    vec_move_back(*ctx->p_static_consts, static_const_toplvls);
}

static void dbl_static_const_toplvl(Ctx ctx, TIdentifier identifier, TIdentifier dbl_const, TInt byte) {
    TIdentifier name = identifier;
    TInt alignment = byte;
    std::shared_ptr<StaticInit> static_init = std::make_shared<DoubleInit>(dbl_const);
    push_static_const_toplvl(ctx, std::make_unique<AsmStaticConstant>(name, alignment, std::move(static_init)));
}

static std::unique_ptr<AsmStaticConstant> gen_static_const_toplvl(Ctx ctx, TacStaticConstant* node) {
    TIdentifier name = node->name;
    TInt alignment = gen_type_alignment(ctx->frontend, node->static_init_type.get());
    std::shared_ptr<StaticInit> static_init = node->static_init;
    return std::make_unique<AsmStaticConstant>(name, alignment, std::move(static_init));
}

// top_level = Function(identifier, bool, bool, instruction*) | StaticVariable(identifier, bool, int, static_init*)
//           | StaticConstant(identifier, int, static_init)
static std::unique_ptr<AsmTopLevel> gen_toplvl(Ctx ctx, TacTopLevel* node) {
    switch (node->type()) {
        case AST_TacFunction_t:
            return gen_fun_toplvl(ctx, static_cast<TacFunction*>(node));
        case AST_TacStaticVariable_t:
            return gen_static_var_toplvl(ctx, static_cast<TacStaticVariable*>(node));
        case AST_TacStaticConstant_t:
            return gen_static_const_toplvl(ctx, static_cast<TacStaticConstant*>(node));
        default:
            THROW_ABORT;
    }
}

// AST = Program(top_level*, top_level*)
static std::unique_ptr<AsmProgram> gen_program(Ctx ctx, TacProgram* node) {
    vector_t(std::unique_ptr<AsmTopLevel>) static_const_toplvls = vec_new();
    vec_reserve(static_const_toplvls, vec_size(node->static_const_toplvls));
    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        std::unique_ptr<AsmTopLevel> static_const_toplvl = gen_toplvl(ctx, node->static_const_toplvls[i].get());
        vec_move_back(static_const_toplvls, static_const_toplvl);
    }

    vector_t(std::unique_ptr<AsmTopLevel>) top_levels = vec_new();
    vec_reserve(top_levels, vec_size(node->static_var_toplvls) + vec_size(node->fun_toplvls));
    {
        ctx->p_static_consts = &static_const_toplvls;

        for (size_t i = 0; i < vec_size(node->static_var_toplvls); ++i) {
            std::unique_ptr<AsmTopLevel> static_var_toplvl = gen_toplvl(ctx, node->static_var_toplvls[i].get());
            vec_move_back(top_levels, static_var_toplvl);
        }
        for (size_t i = 0; i < vec_size(node->fun_toplvls); ++i) {
            std::unique_ptr<AsmTopLevel> fun_toplvl = gen_toplvl(ctx, node->fun_toplvls[i].get());
            vec_move_back(top_levels, fun_toplvl);
        }
        ctx->p_static_consts = NULL;
    }

    return std::make_unique<AsmProgram>(&static_const_toplvls, &top_levels);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<AsmProgram> generate_assembly(
    std::unique_ptr<TacProgram>* tac_ast, FrontEndContext* frontend, IdentifierContext* identifiers) {
    AsmGenContext ctx;
    {
        ctx.frontend = frontend;
        ctx.identifiers = identifiers;

        ctx.arg_regs[0] = REG_Di;
        ctx.arg_regs[1] = REG_Si;
        ctx.arg_regs[2] = REG_Dx;
        ctx.arg_regs[3] = REG_Cx;
        ctx.arg_regs[4] = REG_R8;
        ctx.arg_regs[5] = REG_R9;

        ctx.sse_arg_regs[0] = REG_Xmm0;
        ctx.sse_arg_regs[1] = REG_Xmm1;
        ctx.sse_arg_regs[2] = REG_Xmm2;
        ctx.sse_arg_regs[3] = REG_Xmm3;
        ctx.sse_arg_regs[4] = REG_Xmm4;
        ctx.sse_arg_regs[5] = REG_Xmm5;
        ctx.sse_arg_regs[6] = REG_Xmm6;
        ctx.sse_arg_regs[7] = REG_Xmm7;

        ctx.dbl_const_table = map_new();
        ctx.struct_8b_cls_map = map_new();
    }
    std::unique_ptr<AsmProgram> asm_ast = gen_program(&ctx, tac_ast->get());

    tac_ast->reset();
    THROW_ABORT_IF(!asm_ast);
    map_delete(ctx.dbl_const_table);
    for (size_t i = 0; i < map_size(ctx.struct_8b_cls_map); ++i) {
        vec_delete(pair_second(ctx.struct_8b_cls_map[i]));
    }
    map_delete(ctx.struct_8b_cls_map);
    return asm_ast;
}
