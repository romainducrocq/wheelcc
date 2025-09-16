#include "util/c_std.h"
#include "util/str2t.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/back_ast.h"
#include "ast/back_symt.h"
#include "ast/front_symt.h"
#include "ast/interm_ast.h"

#include "backend/assembly/asm_gen.h"
#include "backend/assembly/registers.h"
#include "backend/assembly/stack_fix.h"
#include "backend/assembly/symt_cvt.h"

typedef enum ASM_LABEL_KIND {
    LBL_Lcomisd_nan,
    LBL_Ldouble,
    LBL_Lsd2si_after,
    LBL_Lsd2si_out_of_range,
    LBL_Lsi2sd_after,
    LBL_Lsi2sd_out_of_range
} ASM_LABEL_KIND;

typedef enum STRUCT8B_CLASS {
    CLS_integer,
    CLS_sse,
    CLS_memory
} STRUCT8B_CLASS;

typedef struct Struct8Bytes {
    size_t size;
    STRUCT8B_CLASS clss[2];
} Struct8Bytes;

PairKeyValue(TIdentifier, Struct8Bytes);

typedef struct AsmGenContext {
    FrontEndContext* frontend;
    IdentifierContext* identifiers;
    // Assembly generation
    FunType* p_fun_type;
    REGISTER_KIND arg_regs[6];
    REGISTER_KIND sse_arg_regs[8];
    hashmap_t(TIdentifier, TIdentifier) dbl_const_table;
    hashmap_t(TIdentifier, Struct8Bytes) struct_8b_map;
    vector_t(unique_ptr_t(AsmInstruction)) * p_instrs;
    vector_t(unique_ptr_t(AsmTopLevel)) * p_static_consts;
} AsmGenContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

typedef AsmGenContext* Ctx;

static shared_ptr_t(AsmOperand) char_imm_op(const CConstChar* node) {
    TULong value = (TULong)node->value;
    bool is_neg = node->value < 0;
    return make_AsmImm(value, true, false, is_neg);
}

static shared_ptr_t(AsmOperand) int_imm_op(const CConstInt* node) {
    TULong value = (TULong)node->value;
    bool is_byte = node->value <= 127 && node->value >= -128;
    bool is_neg = node->value < 0;
    return make_AsmImm(value, is_byte, false, is_neg);
}

static shared_ptr_t(AsmOperand) long_imm_op(const CConstLong* node) {
    TULong value = (TULong)node->value;
    bool is_byte = node->value <= 127l && node->value >= -128l;
    bool is_quad = node->value > 2147483647l || node->value < -2147483648l;
    bool is_neg = node->value < 0l;
    return make_AsmImm(value, is_byte, is_quad, is_neg);
}

static shared_ptr_t(AsmOperand) uchar_imm(const CConstUChar* node) {
    TULong value = (TULong)node->value;
    return make_AsmImm(value, true, false, false);
}

static shared_ptr_t(AsmOperand) uint_imm_op(const CConstUInt* node) {
    TULong value = (TULong)node->value;
    bool is_byte = node->value <= 255u;
    bool is_quad = node->value > 2147483647u;
    return make_AsmImm(value, is_byte, is_quad, false);
}

static shared_ptr_t(AsmOperand) ulong_imm_op(const CConstULong* node) {
    TULong value = node->value;
    bool is_byte = node->value <= 255ul;
    bool is_quad = node->value > 2147483647ul;
    return make_AsmImm(value, is_byte, is_quad, false);
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

static shared_ptr_t(AsmOperand) dbl_static_const_op(Ctx ctx, TULong binary, TInt byte) {
    TIdentifier dbl_const_label;
    {
        TIdentifier dbl_const = make_binary_identifier(ctx, binary);
        ssize_t map_it = map_find(ctx->dbl_const_table, dbl_const);
        if (map_it != map_end()) {
            dbl_const_label = pair_second(ctx->dbl_const_table[map_it]);
        }
        else {
            dbl_const_label = repr_asm_label(ctx, LBL_Ldouble);
            map_add(ctx->dbl_const_table, dbl_const, dbl_const_label);
            dbl_static_const_toplvl(ctx, dbl_const_label, dbl_const, byte);
        }
    }
    return make_AsmData(dbl_const_label, 0l);
}

static shared_ptr_t(AsmOperand) dbl_const_op(Ctx ctx, const CConstDouble* node) {
    TULong binary = dbl_to_binary(node->value);
    TInt byte = binary == 9223372036854775808ul ? 16 : 8;
    return dbl_static_const_op(ctx, binary, byte);
}

static shared_ptr_t(AsmOperand) const_op(Ctx ctx, const TacConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return char_imm_op(&node->constant->get._CConstChar);
        case AST_CConstInt_t:
            return int_imm_op(&node->constant->get._CConstInt);
        case AST_CConstLong_t:
            return long_imm_op(&node->constant->get._CConstLong);
        case AST_CConstDouble_t:
            return dbl_const_op(ctx, &node->constant->get._CConstDouble);
        case AST_CConstUChar_t:
            return uchar_imm(&node->constant->get._CConstUChar);
        case AST_CConstUInt_t:
            return uint_imm_op(&node->constant->get._CConstUInt);
        case AST_CConstULong_t:
            return ulong_imm_op(&node->constant->get._CConstULong);
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(AsmOperand) pseudo_op(const TacVariable* node) {
    TIdentifier name = node->name;
    return make_AsmPseudo(name);
}

static shared_ptr_t(AsmOperand) pseudo_mem_op(const TacVariable* node) {
    TIdentifier name = node->name;
    return make_AsmPseudoMem(name, 0l);
}

static shared_ptr_t(AsmOperand) var_op(Ctx ctx, const TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Array_t:
        case AST_Structure_t:
            return pseudo_mem_op(node);
        default:
            return pseudo_op(node);
    }
}

// operand = Imm(int, bool, bool, bool) | Reg(reg) | Pseudo(identifier) | Memory(int, reg) | Data(identifier, int)
//         | PseudoMem(identifier, int) | Indexed(int, reg, reg)
static shared_ptr_t(AsmOperand) gen_op(Ctx ctx, const TacValue* node) {
    switch (node->type) {
        case AST_TacConstant_t:
            return const_op(ctx, &node->get._TacConstant);
        case AST_TacVariable_t:
            return var_op(ctx, &node->get._TacVariable);
        default:
            THROW_ABORT;
    }
}

// (signed) cond_code = E | NE | L | LE | G | GE
static AsmCondCode gen_signed_cond_code(const TacBinaryOp* node) {
    switch (node->type) {
        case AST_TacEqual_t:
            return init_AsmE();
        case AST_TacNotEqual_t:
            return init_AsmNE();
        case AST_TacLessThan_t:
            return init_AsmL();
        case AST_TacLessOrEqual_t:
            return init_AsmLE();
        case AST_TacGreaterThan_t:
            return init_AsmG();
        case AST_TacGreaterOrEqual_t:
            return init_AsmGE();
        default:
            THROW_ABORT;
    }
}

// (unsigned) cond_code = E | NE | B | BE | A | AE
static AsmCondCode gen_unsigned_cond_code(const TacBinaryOp* node) {
    switch (node->type) {
        case AST_TacEqual_t:
            return init_AsmE();
        case AST_TacNotEqual_t:
            return init_AsmNE();
        case AST_TacLessThan_t:
            return init_AsmB();
        case AST_TacLessOrEqual_t:
            return init_AsmBE();
        case AST_TacGreaterThan_t:
            return init_AsmA();
        case AST_TacGreaterOrEqual_t:
            return init_AsmAE();
        default:
            THROW_ABORT;
    }
}

// unary_operator = Not | Neg | Shr
static AsmUnaryOp gen_unop(const TacUnaryOp* node) {
    switch (node->type) {
        case AST_TacComplement_t:
            return init_AsmNot();
        case AST_TacNegate_t:
            return init_AsmNeg();
        default:
            THROW_ABORT;
    }
}

// binary_operator = Add | Sub | Mult | DivDouble | BitAnd | BitOr | BitXor | BitShiftLeft | BitShiftRight |
//                 BitShrArithmetic
static AsmBinaryOp gen_binop(const TacBinaryOp* node) {
    switch (node->type) {
        case AST_TacAdd_t:
            return init_AsmAdd();
        case AST_TacSubtract_t:
            return init_AsmSub();
        case AST_TacMultiply_t:
            return init_AsmMult();
        case AST_TacDivide_t:
            return init_AsmDivDouble();
        case AST_TacBitAnd_t:
            return init_AsmBitAnd();
        case AST_TacBitOr_t:
            return init_AsmBitOr();
        case AST_TacBitXor_t:
            return init_AsmBitXor();
        case AST_TacBitShiftLeft_t:
            return init_AsmBitShiftLeft();
        case AST_TacBitShiftRight_t:
            return init_AsmBitShiftRight();
        case AST_TacBitShrArithmetic_t:
            return init_AsmBitShrArithmetic();
        default:
            THROW_ABORT;
    }
}

static bool is_const_signed(const TacConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
        case AST_CConstInt_t:
        case AST_CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_signed(Ctx ctx, const TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
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

static bool is_value_signed(Ctx ctx, const TacValue* node) {
    switch (node->type) {
        case AST_TacConstant_t:
            return is_const_signed(&node->get._TacConstant);
        case AST_TacVariable_t:
            return is_var_signed(ctx, &node->get._TacVariable);
        default:
            THROW_ABORT;
    }
}

static bool is_const_1b(const TacConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
        case AST_CConstUChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_1b(Ctx ctx, const TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_1b(Ctx ctx, const TacValue* node) {
    switch (node->type) {
        case AST_TacConstant_t:
            return is_const_1b(&node->get._TacConstant);
        case AST_TacVariable_t:
            return is_var_1b(ctx, &node->get._TacVariable);
        default:
            THROW_ABORT;
    }
}

static bool is_const_4b(const TacConstant* node) {
    switch (node->constant->type) {
        case AST_CConstInt_t:
        case AST_CConstUInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_4b(Ctx ctx, const TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Int_t:
        case AST_UInt_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_4b(Ctx ctx, const TacValue* node) {
    switch (node->type) {
        case AST_TacConstant_t:
            return is_const_4b(&node->get._TacConstant);
        case AST_TacVariable_t:
            return is_var_4b(ctx, &node->get._TacVariable);
        default:
            THROW_ABORT;
    }
}

static bool is_const_dbl(const TacConstant* node) { return node->constant->type == AST_CConstDouble_t; }

static bool is_var_dbl(Ctx ctx, const TacVariable* node) {
    return map_get(ctx->frontend->symbol_table, node->name)->type_t->type == AST_Double_t;
}

static bool is_value_dbl(Ctx ctx, const TacValue* node) {
    switch (node->type) {
        case AST_TacConstant_t:
            return is_const_dbl(&node->get._TacConstant);
        case AST_TacVariable_t:
            return is_var_dbl(ctx, &node->get._TacVariable);
        default:
            THROW_ABORT;
    }
}

static bool is_var_struct(Ctx ctx, const TacVariable* node) {
    return map_get(ctx->frontend->symbol_table, node->name)->type_t->type == AST_Structure_t;
}

static bool is_value_struct(Ctx ctx, const TacValue* node) {
    switch (node->type) {
        case AST_TacVariable_t:
            return is_var_struct(ctx, &node->get._TacVariable);
        case AST_TacConstant_t:
            return false;
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(AssemblyType) const_asm_type(const TacConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
        case AST_CConstUChar_t:
            return make_Byte();
        case AST_CConstInt_t:
        case AST_CConstUInt_t:
            return make_LongWord();
        case AST_CConstDouble_t:
            return make_BackendDouble();
        case AST_CConstLong_t:
        case AST_CConstULong_t:
            return make_QuadWord();
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(AssemblyType) var_asm_type(Ctx ctx, const TacVariable* node) {
    return cvt_backend_asm_type(ctx->frontend, node->name);
}

static shared_ptr_t(AssemblyType) gen_asm_type(Ctx ctx, const TacValue* node) {
    switch (node->type) {
        case AST_TacConstant_t:
            return const_asm_type(&node->get._TacConstant);
        case AST_TacVariable_t:
            return var_asm_type(ctx, &node->get._TacVariable);
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(AssemblyType) asm_type_8b(Ctx ctx, const Structure* struct_type, TLong offset) {
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size - offset;
    if (size >= 8l) {
        return make_QuadWord();
    }
    switch (size) {
        case 1l:
            return make_Byte();
        case 4l:
            return make_LongWord();
        default:
            return make_ByteArray(size, 8);
    }
}

static void struct_8b_class(Ctx ctx, const Structure* struct_type);

static void struct_1_reg_8b_class(Ctx ctx, const Structure* struct_type) {
    Struct8Bytes struct_8b = {1, {CLS_sse, CLS_memory}};
    const StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    size_t members_front = struct_type->is_union ? map_size(struct_typedef->members) : 1;
    for (size_t i = 0; i < members_front; ++i) {
        if (struct_8b.clss[0] == CLS_integer) {
            break;
        }
        const Type* member_type = get_struct_typedef_member(ctx->frontend, struct_type->tag, i)->member_type;
        while (member_type->type == AST_Array_t) {
            member_type = member_type->get._Array.elem_type;
        }
        if (member_type->type == AST_Structure_t) {
            const Structure* member_struct_type = &member_type->get._Structure;
            struct_8b_class(ctx, member_struct_type);
            if (map_get(ctx->struct_8b_map, member_struct_type->tag).clss[0] == CLS_integer) {
                struct_8b.clss[0] = CLS_integer;
            }
        }
        else if (member_type->type != AST_Double_t) {
            struct_8b.clss[0] = CLS_integer;
        }
    }
    map_add(ctx->struct_8b_map, struct_type->tag, struct_8b);
}

static void struct_2_reg_8b_class(Ctx ctx, const Structure* struct_type) {
    Struct8Bytes struct_8b = {2, {CLS_sse, CLS_sse}};
    const StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    size_t members_front = struct_type->is_union ? map_size(struct_typedef->members) : 1;
    for (size_t i = 0; i < members_front; ++i) {
        if (struct_8b.clss[0] == CLS_integer && struct_8b.clss[1] == CLS_integer) {
            break;
        }
        TLong size = 1l;
        const Type* member_type = get_struct_typedef_member(ctx->frontend, struct_type->tag, i)->member_type;
        if (member_type->type == AST_Array_t) {
            do {
                const Array* member_arr_type = &member_type->get._Array;
                member_type = member_arr_type->elem_type;
                size *= member_arr_type->size;
            }
            while (member_type->type == AST_Array_t);
        }
        if (member_type->type == AST_Structure_t) {
            size *= map_get(ctx->frontend->struct_typedef_table, member_type->get._Structure.tag)->size;
        }
        else {
            size *= gen_type_alignment(ctx->frontend, member_type);
        }
        if (size > 8l) {
            if (member_type->type == AST_Structure_t) {
                const Structure* member_struct_type = &member_type->get._Structure;
                struct_8b_class(ctx, member_struct_type);
                const Struct8Bytes* member_struct_8b = &map_get(ctx->struct_8b_map, member_struct_type->tag);
                if (member_struct_8b->size > 1) {
                    if (member_struct_8b->clss[0] == CLS_integer) {
                        struct_8b.clss[0] = CLS_integer;
                    }
                    if (member_struct_8b->clss[1] == CLS_integer) {
                        struct_8b.clss[1] = CLS_integer;
                    }
                }
                else if (member_struct_8b->clss[0] == CLS_integer) {
                    struct_8b.clss[0] = CLS_integer;
                    struct_8b.clss[1] = CLS_integer;
                }
            }
            else if (member_type->type != AST_Double_t) {
                struct_8b.clss[0] = CLS_integer;
                struct_8b.clss[1] = CLS_integer;
            }
        }
        else {
            if (member_type->type == AST_Structure_t) {
                const Structure* member_struct_type = &member_type->get._Structure;
                struct_8b_class(ctx, member_struct_type);
                if (map_get(ctx->struct_8b_map, member_struct_type->tag).clss[0] == CLS_integer) {
                    struct_8b.clss[0] = CLS_integer;
                }
            }
            else if (member_type->type != AST_Double_t) {
                struct_8b.clss[0] = CLS_integer;
            }
            if (!struct_type->is_union) {
                member_type = get_struct_typedef_back(ctx->frontend, struct_type->tag)->member_type;
                while (member_type->type == AST_Array_t) {
                    member_type = member_type->get._Array.elem_type;
                }
                if (member_type->type == AST_Structure_t) {
                    const Structure* member_struct_type = &member_type->get._Structure;
                    struct_8b_class(ctx, member_struct_type);
                    if (map_get(ctx->struct_8b_map, member_struct_type->tag).clss[0] == CLS_integer) {
                        struct_8b.clss[1] = CLS_integer;
                    }
                }
                else if (member_type->type != AST_Double_t) {
                    struct_8b.clss[1] = CLS_integer;
                }
            }
        }
    }
    map_add(ctx->struct_8b_map, struct_type->tag, struct_8b);
}

static void struct_8b_class(Ctx ctx, const Structure* struct_type) {
    if (map_find(ctx->struct_8b_map, struct_type->tag) == map_end()) {
        TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
        if (size > 16l) {
            Struct8Bytes struct_8b = {3, {CLS_memory, CLS_memory}};
            size -= 24l;
            while (size > 0l) {
                struct_8b.size++;
                size -= 8l;
            }
            map_add(ctx->struct_8b_map, struct_type->tag, struct_8b);
        }
        else if (size > 8l) {
            struct_2_reg_8b_class(ctx, struct_type);
        }
        else {
            struct_1_reg_8b_class(ctx, struct_type);
        }
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

static void push_instr(Ctx ctx, unique_ptr_t(AsmInstruction) instr) { vec_move_back(*ctx->p_instrs, instr); }

static void ret_int_instr(Ctx ctx, const TacReturn* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->val);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
    shared_ptr_t(AssemblyType) asm_type_val = gen_asm_type(ctx, node->val);
    push_instr(ctx, make_AsmMov(&asm_type_val, &src, &dst));
    ret_1_reg_mask(ctx->p_fun_type, true);
}

static void ret_dbl_instr(Ctx ctx, const TacReturn* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->val);
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Xmm0);
    shared_ptr_t(AssemblyType) asm_type_val = make_BackendDouble();
    push_instr(ctx, make_AsmMov(&asm_type_val, &src, &dst));
    ret_1_reg_mask(ctx->p_fun_type, false);
}

static void ret_8b_instr(Ctx ctx, TIdentifier name, TLong offset, const Structure* struct_type, REGISTER_KIND arg_reg) {
    TIdentifier src_name = name;
    shared_ptr_t(AsmOperand) dst = gen_register(arg_reg);
    shared_ptr_t(AssemblyType) asm_type_src =
        struct_type ? asm_type_8b(ctx, struct_type, offset) : make_BackendDouble();
    if (asm_type_src->type == AST_ByteArray_t) {
        TLong size = offset + 2l;
        offset += asm_type_src->get._ByteArray.size - 1l;
        free_AssemblyType(&asm_type_src);
        asm_type_src = make_Byte();
        shared_ptr_t(AsmOperand) src_shl = make_AsmImm(8ul, true, false, false);
        shared_ptr_t(AssemblyType) asm_type_shl = make_QuadWord();
        while (offset >= size) {
            {
                shared_ptr_t(AsmOperand) src = make_AsmPseudoMem(src_name, offset);
                shared_ptr_t(AsmOperand) dst_cp = sptr_new();
                sptr_copy(AsmOperand, dst, dst_cp);
                shared_ptr_t(AssemblyType) asm_type_src_cp = sptr_new();
                sptr_copy(AssemblyType, asm_type_src, asm_type_src_cp);
                push_instr(ctx, make_AsmMov(&asm_type_src_cp, &src, &dst_cp));
            }
            {
                AsmBinaryOp binop = init_AsmBitShiftLeft();
                shared_ptr_t(AssemblyType) asm_type_shl_cp = sptr_new();
                sptr_copy(AssemblyType, asm_type_shl, asm_type_shl_cp);
                shared_ptr_t(AsmOperand) src_shl_cp = sptr_new();
                sptr_copy(AsmOperand, src_shl, src_shl_cp);
                shared_ptr_t(AsmOperand) dst_cp = sptr_new();
                sptr_copy(AsmOperand, dst, dst_cp);
                push_instr(ctx, make_AsmBinary(&binop, &asm_type_shl_cp, &src_shl_cp, &dst_cp));
            }
            offset--;
        }
        {
            shared_ptr_t(AsmOperand) src = make_AsmPseudoMem(src_name, offset);
            shared_ptr_t(AsmOperand) dst_cp = sptr_new();
            sptr_copy(AsmOperand, dst, dst_cp);
            shared_ptr_t(AssemblyType) asm_type_src_cp = sptr_new();
            sptr_copy(AssemblyType, asm_type_src, asm_type_src_cp);
            push_instr(ctx, make_AsmMov(&asm_type_src_cp, &src, &dst_cp));
        }
        {
            AsmBinaryOp binop = init_AsmBitShiftLeft();
            shared_ptr_t(AsmOperand) dst_cp = sptr_new();
            sptr_copy(AsmOperand, dst, dst_cp);
            push_instr(ctx, make_AsmBinary(&binop, &asm_type_shl, &src_shl, &dst_cp));
        }
        offset--;
        {
            shared_ptr_t(AsmOperand) src = make_AsmPseudoMem(src_name, offset);
            push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
        }
    }
    else {
        shared_ptr_t(AsmOperand) src = sptr_new();
        {
            TLong from_offset = offset;
            src = make_AsmPseudoMem(src_name, from_offset);
        }
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
}

static void ret_struct_instr(Ctx ctx, const TacReturn* node) {
    TIdentifier name = node->val->get._TacVariable.name;
    const Structure* struct_type = &map_get(ctx->frontend->symbol_table, name)->type_t->get._Structure;
    struct_8b_class(ctx, struct_type);
    const Struct8Bytes* struct_8b = &map_get(ctx->struct_8b_map, struct_type->tag);
    if (struct_8b->clss[0] == CLS_memory) {
        {
            shared_ptr_t(AsmOperand) src = gen_memory(REG_Bp, -8l);
            shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
            shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
            push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
            ret_1_reg_mask(ctx->p_fun_type, true);
        }
        {
            TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
            TLong offset = 0l;
            while (size > 0l) {
                shared_ptr_t(AsmOperand) src = gen_op(ctx, node->val);
                THROW_ABORT_IF(src->type != AST_AsmPseudoMem_t);
                src->get._AsmPseudoMem.offset = offset;
                shared_ptr_t(AsmOperand) dst = gen_memory(REG_Ax, offset);
                shared_ptr_t(AssemblyType) asm_type_src = sptr_new();
                if (size >= 8l) {
                    asm_type_src = make_QuadWord();
                    size -= 8l;
                    offset += 8l;
                }
                else if (size >= 4l) {
                    asm_type_src = make_LongWord();
                    size -= 4l;
                    offset += 4l;
                }
                else {
                    asm_type_src = make_Byte();
                    size--;
                    offset++;
                }
                push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
            }
        }
    }
    else {
        bool reg_size = false;
        switch (struct_8b->clss[0]) {
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
        if (struct_8b->size == 2) {
            bool sse_size = !reg_size;
            switch (struct_8b->clss[1]) {
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

static void ret_instr(Ctx ctx, const TacReturn* node) {
    if (node->val) {
        if (is_value_dbl(ctx, node->val)) {
            ret_dbl_instr(ctx, node);
        }
        else if (!is_value_struct(ctx, node->val)) {
            ret_int_instr(ctx, node);
        }
        else {
            ret_struct_instr(ctx, node);
        }
    }
    else {
        ret_2_reg_mask(ctx->p_fun_type, false, false);
    }
    push_instr(ctx, make_AsmRet());
}

static void sign_extend_instr(Ctx ctx, const TacSignExtend* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->src);
    shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node->dst);
    push_instr(ctx, make_AsmMovSx(&asm_type_src, &asm_type_dst, &src, &dst));
}

static void truncate_imm_byte_instr(AsmImm* node) {
    if (!node->is_byte) {
        node->value %= 256ul;
    }
}

static void truncate_byte_instr(Ctx ctx, const TacTruncate* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_dst = make_Byte();
    if (src->type == AST_AsmImm_t) {
        truncate_imm_byte_instr(&src->get._AsmImm);
    }
    push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
}

static void truncate_imm_long_instr(AsmImm* node) {
    if (node->is_quad) {
        node->value -= 4294967296ul;
    }
}

static void truncate_long_instr(Ctx ctx, const TacTruncate* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_dst = make_LongWord();
    if (src->type == AST_AsmImm_t) {
        truncate_imm_long_instr(&src->get._AsmImm);
    }
    push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
}

static void truncate_instr(Ctx ctx, const TacTruncate* node) {
    if (is_value_1b(ctx, node->dst)) {
        truncate_byte_instr(ctx, node);
    }
    else {
        truncate_long_instr(ctx, node);
    }
}

static void zero_extend_instr(Ctx ctx, const TacZeroExtend* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->src);
    shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node->dst);
    push_instr(ctx, make_AsmMovZeroExtend(&asm_type_src, &asm_type_dst, &src, &dst));
}

static void dbl_to_char_instr(Ctx ctx, const TacDoubleToInt* node) {
    shared_ptr_t(AsmOperand) src_dst = gen_register(REG_Ax);
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src = make_LongWord();
        push_instr(ctx, make_AsmCvttsd2si(&asm_type_src, &src, &src_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        shared_ptr_t(AssemblyType) asm_type_dst = make_Byte();
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src_dst, &dst));
    }
}

static void dbl_to_long_instr(Ctx ctx, const TacDoubleToInt* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->dst);
    push_instr(ctx, make_AsmCvttsd2si(&asm_type_src, &src, &dst));
}

static void dbl_to_signed_instr(Ctx ctx, const TacDoubleToInt* node) {
    if (is_value_1b(ctx, node->dst)) {
        dbl_to_char_instr(ctx, node);
    }
    else {
        dbl_to_long_instr(ctx, node);
    }
}

static void dbl_to_uchar_instr(Ctx ctx, const TacDoubleToUInt* node) {
    shared_ptr_t(AsmOperand) src_dst = gen_register(REG_Ax);
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src = make_LongWord();
        push_instr(ctx, make_AsmCvttsd2si(&asm_type_src, &src, &src_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        shared_ptr_t(AssemblyType) asm_type_dst = make_Byte();
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src_dst, &dst));
    }
}

static void dbl_to_uint_instr(Ctx ctx, const TacDoubleToUInt* node) {
    shared_ptr_t(AsmOperand) src_dst = gen_register(REG_Ax);
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
        push_instr(ctx, make_AsmCvttsd2si(&asm_type_src, &src, &src_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        shared_ptr_t(AssemblyType) asm_type_dst = make_LongWord();
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src_dst, &dst));
    }
}

static void dbl_to_ulong_instr(Ctx ctx, const TacDoubleToUInt* node) {
    TIdentifier target_out_of_range = repr_asm_label(ctx, LBL_Lsd2si_out_of_range);
    TIdentifier target_after = repr_asm_label(ctx, LBL_Lsd2si_after);
    shared_ptr_t(AsmOperand) upper_bound_sd = dbl_static_const_op(ctx, 4890909195324358656ul, 8);
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AsmOperand) dst_out_of_range_sd = gen_register(REG_Xmm1);
    shared_ptr_t(AssemblyType) asm_type_sd = make_BackendDouble();
    shared_ptr_t(AssemblyType) asm_type_si = make_QuadWord();
    {
        shared_ptr_t(AsmOperand) src_cp = sptr_new();
        sptr_copy(AsmOperand, src, src_cp);
        shared_ptr_t(AsmOperand) upper_bound_sd_cp = sptr_new();
        sptr_copy(AsmOperand, upper_bound_sd, upper_bound_sd_cp);
        shared_ptr_t(AssemblyType) asm_type_sd_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_sd, asm_type_sd_cp);
        push_instr(ctx, make_AsmCmp(&asm_type_sd_cp, &upper_bound_sd_cp, &src_cp));
    }
    {
        AsmCondCode cond_code_ae = init_AsmAE();
        push_instr(ctx, make_AsmJmpCC(target_out_of_range, &cond_code_ae));
    }
    {
        shared_ptr_t(AsmOperand) src_cp = sptr_new();
        sptr_copy(AsmOperand, src, src_cp);
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmCvttsd2si(&asm_type_si_cp, &src_cp, &dst_cp));
    }
    push_instr(ctx, make_AsmJmp(target_after));
    push_instr(ctx, make_AsmLabel(target_out_of_range));
    {
        shared_ptr_t(AsmOperand) dst_out_of_range_sd_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_sd, dst_out_of_range_sd_cp);
        shared_ptr_t(AssemblyType) asm_type_sd_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_sd, asm_type_sd_cp);
        push_instr(ctx, make_AsmMov(&asm_type_sd_cp, &src, &dst_out_of_range_sd_cp));
    }
    {
        AsmBinaryOp binop_out_of_range_sd_sub = init_AsmSub();
        shared_ptr_t(AsmOperand) dst_out_of_range_sd_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_sd, dst_out_of_range_sd_cp);
        push_instr(
            ctx, make_AsmBinary(&binop_out_of_range_sd_sub, &asm_type_sd, &upper_bound_sd, &dst_out_of_range_sd_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmCvttsd2si(&asm_type_si_cp, &dst_out_of_range_sd, &dst_cp));
    }
    {
        AsmBinaryOp binop_out_of_range_si_add = init_AsmAdd();
        shared_ptr_t(AsmOperand) upper_bound_si = make_AsmImm(9223372036854775808ul, false, true, false);
        push_instr(ctx, make_AsmBinary(&binop_out_of_range_si_add, &asm_type_si, &upper_bound_si, &dst));
    }
    push_instr(ctx, make_AsmLabel(target_after));
}

static void dbl_to_unsigned_instr(Ctx ctx, const TacDoubleToUInt* node) {
    if (is_value_1b(ctx, node->dst)) {
        dbl_to_uchar_instr(ctx, node);
    }
    else if (is_value_4b(ctx, node->dst)) {
        dbl_to_uint_instr(ctx, node);
    }
    else {
        dbl_to_ulong_instr(ctx, node);
    }
}

static void char_to_dbl_instr(Ctx ctx, const TacIntToDouble* node) {
    shared_ptr_t(AsmOperand) src_dst = gen_register(REG_Ax);
    shared_ptr_t(AssemblyType) asm_type_dst = make_LongWord();
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src = make_Byte();
        shared_ptr_t(AssemblyType) asm_type_dst_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_dst, asm_type_dst_cp);
        push_instr(ctx, make_AsmMovSx(&asm_type_src, &asm_type_dst_cp, &src, &src_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmCvtsi2sd(&asm_type_dst, &src_dst, &dst));
    }
}

static void long_to_dbl_instr(Ctx ctx, const TacIntToDouble* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->src);
    push_instr(ctx, make_AsmCvtsi2sd(&asm_type_src, &src, &dst));
}

static void signed_to_dbl_instr(Ctx ctx, const TacIntToDouble* node) {
    if (is_value_1b(ctx, node->src)) {
        char_to_dbl_instr(ctx, node);
    }
    else {
        long_to_dbl_instr(ctx, node);
    }
}

static void uchar_to_dbl_instr(Ctx ctx, const TacUIntToDouble* node) {
    shared_ptr_t(AsmOperand) src_dst = gen_register(REG_Ax);
    shared_ptr_t(AssemblyType) asm_type_dst = make_LongWord();
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src = make_Byte();
        shared_ptr_t(AssemblyType) asm_type_dst_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_dst, asm_type_dst_cp);
        push_instr(ctx, make_AsmMovZeroExtend(&asm_type_src, &asm_type_dst_cp, &src, &src_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmCvtsi2sd(&asm_type_dst, &src_dst, &dst));
    }
}

static void uint_to_dbl_instr(Ctx ctx, const TacUIntToDouble* node) {
    shared_ptr_t(AsmOperand) src_dst = gen_register(REG_Ax);
    shared_ptr_t(AssemblyType) asm_type_dst = make_QuadWord();
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src = make_LongWord();
        shared_ptr_t(AssemblyType) asm_type_dst_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_dst, asm_type_dst_cp);
        push_instr(ctx, make_AsmMovZeroExtend(&asm_type_src, &asm_type_dst_cp, &src, &src_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmCvtsi2sd(&asm_type_dst, &src_dst, &dst));
    }
}

static void ulong_to_dbl_instr(Ctx ctx, const TacUIntToDouble* node) {
    TIdentifier target_out_of_range = repr_asm_label(ctx, LBL_Lsi2sd_out_of_range);
    TIdentifier target_after = repr_asm_label(ctx, LBL_Lsi2sd_after);
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AsmOperand) dst_out_of_range_si = gen_register(REG_Ax);
    shared_ptr_t(AsmOperand) dst_out_of_range_si_shr = gen_register(REG_Dx);
    shared_ptr_t(AssemblyType) asm_type_si = make_QuadWord();
    {
        shared_ptr_t(AsmOperand) lower_bound_si = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) src_cp = sptr_new();
        sptr_copy(AsmOperand, src, src_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmCmp(&asm_type_si_cp, &lower_bound_si, &src_cp));
    }
    {
        AsmCondCode cond_code_l = init_AsmL();
        push_instr(ctx, make_AsmJmpCC(target_out_of_range, &cond_code_l));
    }
    {
        shared_ptr_t(AsmOperand) src_cp = sptr_new();
        sptr_copy(AsmOperand, src, src_cp);
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmCvtsi2sd(&asm_type_si_cp, &src_cp, &dst_cp));
    }
    push_instr(ctx, make_AsmJmp(target_after));
    push_instr(ctx, make_AsmLabel(target_out_of_range));
    {
        shared_ptr_t(AsmOperand) dst_out_of_range_si_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_si, dst_out_of_range_si_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmMov(&asm_type_si_cp, &src, &dst_out_of_range_si_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst_out_of_range_si_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_si, dst_out_of_range_si_cp);
        shared_ptr_t(AsmOperand) dst_out_of_range_si_shr_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_si_shr, dst_out_of_range_si_shr_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmMov(&asm_type_si_cp, &dst_out_of_range_si_cp, &dst_out_of_range_si_shr_cp));
    }
    {
        AsmUnaryOp unop_out_of_range_si_shr = init_AsmShr();
        shared_ptr_t(AsmOperand) dst_out_of_range_si_shr_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_si_shr, dst_out_of_range_si_shr_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmUnary(&unop_out_of_range_si_shr, &asm_type_si_cp, &dst_out_of_range_si_shr_cp));
    }
    {
        AsmBinaryOp binop_out_of_range_si_and = init_AsmBitAnd();
        shared_ptr_t(AsmOperand) set_bit_si = make_AsmImm(1ul, true, false, false);
        shared_ptr_t(AsmOperand) dst_out_of_range_si_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_si, dst_out_of_range_si_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(
            ctx, make_AsmBinary(&binop_out_of_range_si_and, &asm_type_si_cp, &set_bit_si, &dst_out_of_range_si_cp));
    }
    {
        AsmBinaryOp binop_out_of_range_si_or = init_AsmBitOr();
        shared_ptr_t(AsmOperand) dst_out_of_range_si_shr_cp = sptr_new();
        sptr_copy(AsmOperand, dst_out_of_range_si_shr, dst_out_of_range_si_shr_cp);
        shared_ptr_t(AssemblyType) asm_type_si_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_si, asm_type_si_cp);
        push_instr(ctx, make_AsmBinary(&binop_out_of_range_si_or, &asm_type_si_cp, &dst_out_of_range_si,
                            &dst_out_of_range_si_shr_cp));
    }
    {
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        push_instr(ctx, make_AsmCvtsi2sd(&asm_type_si, &dst_out_of_range_si_shr, &dst_cp));
    }
    {
        AsmBinaryOp binop_out_of_range_sq_add = init_AsmAdd();
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        shared_ptr_t(AssemblyType) asm_type_sq = make_BackendDouble();
        push_instr(ctx, make_AsmBinary(&binop_out_of_range_sq_add, &asm_type_sq, &dst, &dst_cp));
    }
    push_instr(ctx, make_AsmLabel(target_after));
}

static void unsigned_to_dbl_instr(Ctx ctx, const TacUIntToDouble* node) {
    if (is_value_1b(ctx, node->src)) {
        uchar_to_dbl_instr(ctx, node);
    }
    else if (is_value_4b(ctx, node->src)) {
        uint_to_dbl_instr(ctx, node);
    }
    else {
        ulong_to_dbl_instr(ctx, node);
    }
}

static void alloc_stack_instr(Ctx ctx, TLong byte) { push_instr(ctx, alloc_stack_bytes(byte)); }

static void dealloc_stack_instr(Ctx ctx, TLong byte) {
    AsmBinaryOp binop = init_AsmAdd();
    shared_ptr_t(AssemblyType) asm_type = make_QuadWord();
    shared_ptr_t(AsmOperand) src = sptr_new();
    {
        TULong value = (TULong)byte;
        bool is_byte = byte <= 127l && byte >= -128l;
        bool is_quad = byte > 2147483647l || byte < -2147483648l;
        bool is_neg = byte < 0l;
        src = make_AsmImm(value, is_byte, is_quad, is_neg);
    }
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Sp);
    push_instr(ctx, make_AsmBinary(&binop, &asm_type, &src, &dst));
}

static void reg_arg_call_instr(Ctx ctx, const TacValue* node, REGISTER_KIND arg_reg) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node);
    shared_ptr_t(AsmOperand) dst = gen_register(arg_reg);
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node);
    push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
}

static void stack_arg_call_instr(Ctx ctx, const TacValue* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node);
    switch (src->type) {
        case AST_AsmRegister_t:
        case AST_AsmImm_t:
            push_instr(ctx, make_AsmPush(&src));
            return;
        default:
            break;
    }
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node);
    switch (asm_type_src->type) {
        case AST_QuadWord_t:
        case AST_BackendDouble_t:
            push_instr(ctx, make_AsmPush(&src));
            free_AssemblyType(&asm_type_src);
            return;
        default:
            break;
    }
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
    {
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        push_instr(ctx, make_AsmPush(&dst_cp));
    }
    push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
}

static void reg_8b_arg_call_instr(
    Ctx ctx, TIdentifier name, TLong offset, const Structure* struct_type, REGISTER_KIND arg_reg) {
    ret_8b_instr(ctx, name, offset, struct_type, arg_reg);
}

static void quad_stack_arg_call_instr(Ctx ctx, TIdentifier name, TLong offset) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = make_AsmPseudoMem(src_name, from_offset);
    }
    push_instr(ctx, make_AsmPush(&src));
}

static void long_stack_arg_call_instr(Ctx ctx, TIdentifier name, TLong offset, shared_ptr_t(AssemblyType) * asm_type) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    {
        TIdentifier src_name = name;
        TLong from_offset = offset;
        src = make_AsmPseudoMem(src_name, from_offset);
    }
    shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
    {
        shared_ptr_t(AsmOperand) dst_cp = sptr_new();
        sptr_copy(AsmOperand, dst, dst_cp);
        push_instr(ctx, make_AsmPush(&dst_cp));
    }
    shared_ptr_t(AssemblyType) asm_type_src = sptr_new();
    sptr_move(AssemblyType, *asm_type, asm_type_src);
    push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
}

static void bytearr_stack_arg_call_instr(Ctx ctx, TIdentifier name, TLong offset, const ByteArray* bytearr_type) {
    {
        TLong to_offset = 0l;
        TLong size = bytearr_type->size;
        vector_t(unique_ptr_t(AsmInstruction)) byte_instrs = vec_new();
        while (size > 0l) {
            unique_ptr_t(AsmInstruction) byte_instr = uptr_new();
            {
                shared_ptr_t(AsmOperand) src = make_AsmPseudoMem(name, offset);
                shared_ptr_t(AsmOperand) dst = gen_memory(REG_Sp, to_offset);
                shared_ptr_t(AssemblyType) asm_type_src = sptr_new();
                if (size >= 4l) {
                    asm_type_src = make_LongWord();
                    size -= 4l;
                    offset += 4l;
                    to_offset += 4l;
                }
                else {
                    asm_type_src = make_Byte();
                    size--;
                    offset++;
                    to_offset++;
                }
                byte_instr = make_AsmMov(&asm_type_src, &src, &dst);
            }
            vec_move_back(byte_instrs, byte_instr);
        }
        for (size_t i = vec_size(byte_instrs); i-- > 0;) {
            push_instr(ctx, byte_instrs[i]);
            byte_instrs[i] = uptr_new();
        }
        vec_delete(byte_instrs);
    }
    {
        AsmBinaryOp binop = init_AsmSub();
        shared_ptr_t(AsmOperand) src = make_AsmImm(8ul, true, false, false);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Sp);
        shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
        push_instr(ctx, make_AsmBinary(&binop, &asm_type_src, &src, &dst));
    }
}

static void stack_8b_arg_call_instr(Ctx ctx, TIdentifier name, TLong offset, const Structure* struct_type) {
    shared_ptr_t(AssemblyType) asm_type = asm_type_8b(ctx, struct_type, offset);
    switch (asm_type->type) {
        case AST_QuadWord_t:
            quad_stack_arg_call_instr(ctx, name, offset);
            break;
        case AST_ByteArray_t:
            bytearr_stack_arg_call_instr(ctx, name, offset, &asm_type->get._ByteArray);
            break;
        default:
            long_stack_arg_call_instr(ctx, name, offset, &asm_type);
            break;
    }
    free_AssemblyType(&asm_type);
}

static TLong arg_call_instr(Ctx ctx, const TacFunCall* node, FunType* fun_type, bool is_ret_memory) {
    size_t reg_size = is_ret_memory ? 1 : 0;
    size_t sse_size = 0;
    TLong stack_padding = 0l;
    vector_t(unique_ptr_t(AsmInstruction)) stack_instrs = vec_new();
    vector_t(unique_ptr_t(AsmInstruction))* p_instrs = ctx->p_instrs;
    for (size_t i = 0; i < vec_size(node->args); ++i) {
        const TacValue* arg = node->args[i];
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
            TIdentifier name = arg->get._TacVariable.name;
            const Structure* struct_type = &map_get(ctx->frontend->symbol_table, name)->type_t->get._Structure;
            struct_8b_class(ctx, struct_type);
            const Struct8Bytes* struct_8b = &map_get(ctx->struct_8b_map, struct_type->tag);
            if (struct_8b->clss[0] != CLS_memory) {
                struct_reg_size = 0;
                struct_sse_size = 0;
                for (size_t j = 0; j < struct_8b->size; ++j) {
                    if (struct_8b->clss[j] == CLS_sse) {
                        struct_sse_size++;
                    }
                    else {
                        struct_reg_size++;
                    }
                }
            }
            if (struct_reg_size + reg_size <= 6 && struct_sse_size + sse_size <= 8) {
                TLong offset = 0l;
                for (size_t j = 0; j < struct_8b->size; ++j) {
                    if (struct_8b->clss[j] == CLS_sse) {
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
                for (size_t j = 0; j < struct_8b->size; ++j) {
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
        push_instr(ctx, stack_instrs[i]);
        stack_instrs[i] = uptr_new();
    }
    vec_delete(stack_instrs);
    return stack_padding;
}

static void ret_call_instr(Ctx ctx, const TacValue* node, REGISTER_KIND arg_reg) {
    shared_ptr_t(AsmOperand) src = gen_register(arg_reg);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node);
    shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node);
    push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
}

static void ret_8b_call_instr(
    Ctx ctx, TIdentifier name, TLong offset, const Structure* struct_type, REGISTER_KIND arg_reg) {
    TIdentifier dst_name = name;
    shared_ptr_t(AsmOperand) src = gen_register(arg_reg);
    shared_ptr_t(AssemblyType) asm_type_dst =
        struct_type ? asm_type_8b(ctx, struct_type, offset) : make_BackendDouble();
    if (asm_type_dst->type == AST_ByteArray_t) {
        TLong size = asm_type_dst->get._ByteArray.size + offset - 2l;
        free_AssemblyType(&asm_type_dst);
        asm_type_dst = make_Byte();
        shared_ptr_t(AsmOperand) src_shr2op = make_AsmImm(8ul, true, false, false);
        shared_ptr_t(AssemblyType) asm_type_shr2op = make_QuadWord();
        while (offset < size) {
            {
                shared_ptr_t(AsmOperand) src_cp = sptr_new();
                sptr_copy(AsmOperand, src, src_cp);
                shared_ptr_t(AsmOperand) dst = make_AsmPseudoMem(dst_name, offset);
                shared_ptr_t(AssemblyType) asm_type_dst_cp = sptr_new();
                sptr_copy(AssemblyType, asm_type_dst, asm_type_dst_cp);
                push_instr(ctx, make_AsmMov(&asm_type_dst_cp, &src_cp, &dst));
            }
            {
                AsmBinaryOp binop = init_AsmBitShiftRight();
                shared_ptr_t(AsmOperand) src_shr2op_cp = sptr_new();
                sptr_copy(AsmOperand, src_shr2op, src_shr2op_cp);
                shared_ptr_t(AsmOperand) src_cp = sptr_new();
                sptr_copy(AsmOperand, src, src_cp);
                shared_ptr_t(AssemblyType) asm_type_shr2op_cp = sptr_new();
                sptr_copy(AssemblyType, asm_type_shr2op, asm_type_shr2op_cp);
                push_instr(ctx, make_AsmBinary(&binop, &asm_type_shr2op_cp, &src_shr2op_cp, &src_cp));
            }
            offset++;
        }
        {
            shared_ptr_t(AsmOperand) src_cp = sptr_new();
            sptr_copy(AsmOperand, src, src_cp);
            shared_ptr_t(AsmOperand) dst = make_AsmPseudoMem(dst_name, offset);
            shared_ptr_t(AssemblyType) asm_type_dst_cp = sptr_new();
            sptr_copy(AssemblyType, asm_type_dst, asm_type_dst_cp);
            push_instr(ctx, make_AsmMov(&asm_type_dst_cp, &src_cp, &dst));
        }
        {
            AsmBinaryOp binop = init_AsmBitShiftRight();
            shared_ptr_t(AsmOperand) src_cp = sptr_new();
            sptr_copy(AsmOperand, src, src_cp);
            push_instr(ctx, make_AsmBinary(&binop, &asm_type_shr2op, &src_shr2op, &src_cp));
        }
        offset++;
        {
            shared_ptr_t(AsmOperand) dst = make_AsmPseudoMem(dst_name, offset);
            push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
        }
    }
    else {
        shared_ptr_t(AsmOperand) dst = sptr_new();
        {
            TLong to_offset = offset;
            dst = make_AsmPseudoMem(dst_name, to_offset);
        }
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
    }
}

static void call_instr(Ctx ctx, const TacFunCall* node) {
    bool is_ret_memory = false;
    FunType* fun_type = &map_get(ctx->frontend->symbol_table, node->name)->type_t->get._FunType;
    if (node->dst && is_value_struct(ctx, node->dst)) {
        TIdentifier name = node->dst->get._TacVariable.name;
        const Structure* struct_type = &map_get(ctx->frontend->symbol_table, name)->type_t->get._Structure;
        struct_8b_class(ctx, struct_type);
        if (map_get(ctx->struct_8b_map, struct_type->tag).clss[0] == CLS_memory) {
            is_ret_memory = true;
            {
                shared_ptr_t(AsmOperand) src = gen_op(ctx, node->dst);
                shared_ptr_t(AsmOperand) dst = gen_register(REG_Di);
                push_instr(ctx, make_AsmLea(&src, &dst));
            }
        }
    }
    {
        TLong stack_padding = arg_call_instr(ctx, node, fun_type, is_ret_memory);

        {
            TIdentifier name = node->name;
            push_instr(ctx, make_AsmCall(name));
        }

        if (stack_padding > 0l) {
            dealloc_stack_instr(ctx, stack_padding);
        }
    }

    if (!node->dst) {
        ret_2_reg_mask(fun_type, false, false);
    }
    else if (is_ret_memory) {
        ret_1_reg_mask(fun_type, true);
    }
    else if (is_value_dbl(ctx, node->dst)) {
        ret_call_instr(ctx, node->dst, REG_Xmm0);
        ret_1_reg_mask(fun_type, false);
    }
    else if (!is_value_struct(ctx, node->dst)) {
        ret_call_instr(ctx, node->dst, REG_Ax);
        ret_1_reg_mask(fun_type, true);
    }
    else {
        bool reg_size = false;
        TIdentifier name = node->dst->get._TacVariable.name;
        const Structure* struct_type = &map_get(ctx->frontend->symbol_table, name)->type_t->get._Structure;
        const Struct8Bytes* struct_8b = &map_get(ctx->struct_8b_map, struct_type->tag);
        switch (struct_8b->clss[0]) {
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
        if (struct_8b->size == 2) {
            bool sse_size = !reg_size;
            switch (struct_8b->clss[1]) {
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

static void zero_xmm_reg_instr(Ctx ctx) {
    AsmBinaryOp binop = init_AsmBitXor();
    shared_ptr_t(AsmOperand) src = gen_register(REG_Xmm0);
    shared_ptr_t(AsmOperand) src_cp = sptr_new();
    sptr_copy(AsmOperand, src, src_cp);
    shared_ptr_t(AssemblyType) asm_type_src = make_BackendDouble();
    push_instr(ctx, make_AsmBinary(&binop, &asm_type_src, &src, &src_cp));
}

static void unop_int_arithmetic_instr(Ctx ctx, const TacUnary* node) {
    shared_ptr_t(AsmOperand) src_dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->src);
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src, asm_type_src_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src_cp, &src, &src_dst_cp));
    }
    {
        AsmUnaryOp unop = gen_unop(&node->unop);
        push_instr(ctx, make_AsmUnary(&unop, &asm_type_src, &src_dst));
    }
}

static void unop_dbl_neg_instr(Ctx ctx, const TacUnary* node) {
    shared_ptr_t(AsmOperand) src1_dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src1 = make_BackendDouble();
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) src1_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src1_dst, src1_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &src1, &src1_dst_cp));
    }
    {
        AsmBinaryOp binop = init_AsmBitXor();
        shared_ptr_t(AsmOperand) src2 = dbl_static_const_op(ctx, 9223372036854775808ul, 16);
        push_instr(ctx, make_AsmBinary(&binop, &asm_type_src1, &src2, &src1_dst));
    }
}

static void unop_neg_instr(Ctx ctx, const TacUnary* node) {
    if (is_value_dbl(ctx, node->src)) {
        unop_dbl_neg_instr(ctx, node);
    }
    else {
        unop_int_arithmetic_instr(ctx, node);
    }
}

static void unop_int_conditional_instr(Ctx ctx, const TacUnary* node) {
    shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
    shared_ptr_t(AsmOperand) cmp_dst = gen_op(ctx, node->dst);
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) imm_zero_cp = sptr_new();
        sptr_copy(AsmOperand, imm_zero, imm_zero_cp);
        shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->src);
        push_instr(ctx, make_AsmCmp(&asm_type_src, &imm_zero_cp, &src));
    }
    {
        shared_ptr_t(AsmOperand) cmp_dst_cp = sptr_new();
        sptr_copy(AsmOperand, cmp_dst, cmp_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node->dst);
        push_instr(ctx, make_AsmMov(&asm_type_dst, &imm_zero, &cmp_dst_cp));
    }
    {
        AsmCondCode cond_code_e = init_AsmE();
        push_instr(ctx, make_AsmSetCC(&cond_code_e, &cmp_dst));
    }
}

static void unop_dbl_conditional_instr(Ctx ctx, const TacUnary* node) {
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    shared_ptr_t(AsmOperand) cmp_dst = gen_op(ctx, node->dst);
    zero_xmm_reg_instr(ctx);
    {
        shared_ptr_t(AsmOperand) reg_zero = gen_register(REG_Xmm0);
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AssemblyType) asm_type_src = make_BackendDouble();
        push_instr(ctx, make_AsmCmp(&asm_type_src, &reg_zero, &src));
    }
    {
        shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) cmp_dst_cp = sptr_new();
        sptr_copy(AsmOperand, cmp_dst, cmp_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_dst = make_LongWord();
        push_instr(ctx, make_AsmMov(&asm_type_dst, &imm_zero, &cmp_dst_cp));
    }
    {
        AsmCondCode cond_code_p = init_AsmP();
        push_instr(ctx, make_AsmJmpCC(target_nan, &cond_code_p));
    }
    {
        AsmCondCode cond_code_e = init_AsmE();
        push_instr(ctx, make_AsmSetCC(&cond_code_e, &cmp_dst));
    }
    push_instr(ctx, make_AsmLabel(target_nan));
}

static void unop_conditional_instr(Ctx ctx, const TacUnary* node) {
    if (is_value_dbl(ctx, node->src)) {
        unop_dbl_conditional_instr(ctx, node);
    }
    else {
        unop_int_conditional_instr(ctx, node);
    }
}

static void unary_instr(Ctx ctx, const TacUnary* node) {
    switch (node->unop.type) {
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

static void binop_arithmetic_instr(Ctx ctx, const TacBinary* node) {
    shared_ptr_t(AsmOperand) src1_dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src1 = gen_asm_type(ctx, node->src1);
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src1);
        shared_ptr_t(AsmOperand) src1_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src1_dst, src1_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &src1, &src1_dst_cp));
    }
    {
        AsmBinaryOp binop = gen_binop(&node->binop);
        shared_ptr_t(AsmOperand) src2 = gen_op(ctx, node->src2);
        push_instr(ctx, make_AsmBinary(&binop, &asm_type_src1, &src2, &src1_dst));
    }
}

static void signed_divide_instr(Ctx ctx, const TacBinary* node) {
    shared_ptr_t(AsmOperand) src1_dst = gen_register(REG_Ax);
    shared_ptr_t(AssemblyType) asm_type_src1 = gen_asm_type(ctx, node->src1);
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src1);
        shared_ptr_t(AsmOperand) src1_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src1_dst, src1_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &src1, &src1_dst_cp));
    }
    {
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmCdq(&asm_type_src1_cp));
    }
    {
        shared_ptr_t(AsmOperand) src2 = gen_op(ctx, node->src2);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmIdiv(&asm_type_src1_cp, &src2));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmMov(&asm_type_src1, &src1_dst, &dst));
    }
}

static void unsigned_divide_instr(Ctx ctx, const TacBinary* node) {
    shared_ptr_t(AsmOperand) src1_dst = gen_register(REG_Ax);
    shared_ptr_t(AssemblyType) asm_type_src1 = gen_asm_type(ctx, node->src1);
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src1);
        shared_ptr_t(AsmOperand) src1_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src1_dst, src1_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &src1, &src1_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) imm_zero_dst = gen_register(REG_Dx);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &imm_zero, &imm_zero_dst));
    }
    {
        shared_ptr_t(AsmOperand) src2 = gen_op(ctx, node->src2);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmDiv(&asm_type_src1_cp, &src2));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmMov(&asm_type_src1, &src1_dst, &dst));
    }
}

static void binop_divide_instr(Ctx ctx, const TacBinary* node) {
    if (is_value_dbl(ctx, node->src1)) {
        binop_arithmetic_instr(ctx, node);
    }
    else if (is_value_signed(ctx, node->src1)) {
        signed_divide_instr(ctx, node);
    }
    else {
        unsigned_divide_instr(ctx, node);
    }
}

static void signed_remainder_instr(Ctx ctx, const TacBinary* node) {
    shared_ptr_t(AssemblyType) asm_type_src1 = gen_asm_type(ctx, node->src1);
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src1);
        shared_ptr_t(AsmOperand) src1_dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &src1, &src1_dst));
    }
    {
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmCdq(&asm_type_src1_cp));
    }
    {
        shared_ptr_t(AsmOperand) src2 = gen_op(ctx, node->src2);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmIdiv(&asm_type_src1_cp, &src2));
    }
    {
        shared_ptr_t(AsmOperand) dst_src = gen_register(REG_Dx);
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmMov(&asm_type_src1, &dst_src, &dst));
    }
}

static void unsigned_remainder_instr(Ctx ctx, const TacBinary* node) {
    shared_ptr_t(AsmOperand) dst_src = gen_register(REG_Dx);
    shared_ptr_t(AssemblyType) asm_type_src1 = gen_asm_type(ctx, node->src1);
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src1);
        shared_ptr_t(AsmOperand) src1_dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &src1, &src1_dst));
    }
    {
        shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) dst_src_cp = sptr_new();
        sptr_copy(AsmOperand, dst_src, dst_src_cp);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src1_cp, &imm_zero, &dst_src_cp));
    }
    {
        shared_ptr_t(AsmOperand) src2 = gen_op(ctx, node->src2);
        shared_ptr_t(AssemblyType) asm_type_src1_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src1, asm_type_src1_cp);
        push_instr(ctx, make_AsmDiv(&asm_type_src1_cp, &src2));
    }
    {
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmMov(&asm_type_src1, &dst_src, &dst));
    }
}

static void binop_remainder_instr(Ctx ctx, const TacBinary* node) {
    if (is_value_signed(ctx, node->src1)) {
        signed_remainder_instr(ctx, node);
    }
    else {
        unsigned_remainder_instr(ctx, node);
    }
}

static void binop_int_conditional_instr(Ctx ctx, const TacBinary* node) {
    shared_ptr_t(AsmOperand) cmp_dst = gen_op(ctx, node->dst);
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src1);
        shared_ptr_t(AsmOperand) src2 = gen_op(ctx, node->src2);
        shared_ptr_t(AssemblyType) asm_type_src1 = gen_asm_type(ctx, node->src1);
        push_instr(ctx, make_AsmCmp(&asm_type_src1, &src2, &src1));
    }
    {
        shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) cmp_dst_cp = sptr_new();
        sptr_copy(AsmOperand, cmp_dst, cmp_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node->dst);
        push_instr(ctx, make_AsmMov(&asm_type_dst, &imm_zero, &cmp_dst_cp));
    }
    {
        AsmCondCode cond_code = init_AsmCondCode();
        if (is_value_signed(ctx, node->src1)) {
            cond_code = gen_signed_cond_code(&node->binop);
        }
        else {
            cond_code = gen_unsigned_cond_code(&node->binop);
        }
        push_instr(ctx, make_AsmSetCC(&cond_code, &cmp_dst));
    }
}

static void binop_dbl_conditional_instr(Ctx ctx, const TacBinary* node) {
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    shared_ptr_t(AsmOperand) cmp_dst = gen_op(ctx, node->dst);
    {
        shared_ptr_t(AsmOperand) src1 = gen_op(ctx, node->src1);
        shared_ptr_t(AsmOperand) src2 = gen_op(ctx, node->src2);
        shared_ptr_t(AssemblyType) asm_type_src1 = gen_asm_type(ctx, node->src1);
        push_instr(ctx, make_AsmCmp(&asm_type_src1, &src2, &src1));
    }
    {
        shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) cmp_dst_cp = sptr_new();
        sptr_copy(AsmOperand, cmp_dst, cmp_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_dst = make_LongWord();
        push_instr(ctx, make_AsmMov(&asm_type_dst, &imm_zero, &cmp_dst_cp));
    }
    {
        AsmCondCode cond_code_p = init_AsmP();
        push_instr(ctx, make_AsmJmpCC(target_nan, &cond_code_p));
    }
    {
        AsmCondCode cond_code = gen_unsigned_cond_code(&node->binop);
        if (cond_code.type == AST_AsmNE_t) {
            TIdentifier target_nan_ne = repr_asm_label(ctx, LBL_Lcomisd_nan);
            {
                shared_ptr_t(AsmOperand) cmp_dst_cp = sptr_new();
                sptr_copy(AsmOperand, cmp_dst, cmp_dst_cp);
                push_instr(ctx, make_AsmSetCC(&cond_code, &cmp_dst_cp));
            }
            push_instr(ctx, make_AsmJmp(target_nan_ne));
            push_instr(ctx, make_AsmLabel(target_nan));
            {
                AsmCondCode cond_code_e = init_AsmE();
                push_instr(ctx, make_AsmSetCC(&cond_code_e, &cmp_dst));
            }
            push_instr(ctx, make_AsmLabel(target_nan_ne));
        }
        else {
            push_instr(ctx, make_AsmSetCC(&cond_code, &cmp_dst));
            push_instr(ctx, make_AsmLabel(target_nan));
        }
    }
}

static void binop_conditional_instr(Ctx ctx, const TacBinary* node) {
    if (is_value_dbl(ctx, node->src1)) {
        binop_dbl_conditional_instr(ctx, node);
    }
    else {
        binop_int_conditional_instr(ctx, node);
    }
}

static void binary_instr(Ctx ctx, const TacBinary* node) {
    switch (node->binop.type) {
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

static void copy_struct_instr(Ctx ctx, const TacCopy* node) {
    TIdentifier src_name = node->src->get._TacVariable.name;
    TIdentifier dst_name = node->dst->get._TacVariable.name;
    const Structure* struct_type = &map_get(ctx->frontend->symbol_table, src_name)->type_t->get._Structure;
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    TLong offset = 0l;
    while (size > 0l) {
        shared_ptr_t(AsmOperand) src = make_AsmPseudoMem(src_name, offset);
        shared_ptr_t(AsmOperand) dst = make_AsmPseudoMem(dst_name, offset);
        shared_ptr_t(AssemblyType) asm_type_src = sptr_new();
        if (size >= 8l) {
            asm_type_src = make_QuadWord();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            asm_type_src = make_LongWord();
            size -= 4l;
            offset += 4l;
        }
        else {
            asm_type_src = make_Byte();
            size--;
            offset++;
        }
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
}

static void copy_scalar_instr(Ctx ctx, const TacCopy* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->src);
    push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
}

static void copy_instr(Ctx ctx, const TacCopy* node) {
    if (is_value_struct(ctx, node->src)) {
        copy_struct_instr(ctx, node);
    }
    else {
        copy_scalar_instr(ctx, node);
    }
}

static void getaddr_instr(Ctx ctx, const TacGetAddress* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    {
        if (node->src->type == AST_TacVariable_t) {
            TIdentifier name = node->src->get._TacVariable.name;
            set_insert(ctx->frontend->addressed_set, name);
            ssize_t map_it = map_find(ctx->frontend->symbol_table, name);
            if (map_it != map_end()
                && pair_second(ctx->frontend->symbol_table[map_it])->attrs->type == AST_ConstantAttr_t) {
                src = make_AsmData(name, 0l);
                goto Lpass;
            }
        }
        src = gen_op(ctx, node->src);
    Lpass:;
    }
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    push_instr(ctx, make_AsmLea(&src, &dst));
}

static void load_struct_instr(Ctx ctx, const TacLoad* node) {
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src_ptr);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
    {
        TIdentifier name = node->dst->get._TacVariable.name;
        const Structure* struct_type = &map_get(ctx->frontend->symbol_table, name)->type_t->get._Structure;
        TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
        TLong offset = 0l;
        while (size > 0l) {
            shared_ptr_t(AsmOperand) src = gen_memory(REG_Ax, offset);
            shared_ptr_t(AsmOperand) dst = make_AsmPseudoMem(name, offset);
            shared_ptr_t(AssemblyType) asm_type_dst = sptr_new();
            if (size >= 8l) {
                asm_type_dst = make_QuadWord();
                size -= 8l;
                offset += 8l;
            }
            else if (size >= 4l) {
                asm_type_dst = make_LongWord();
                size -= 4l;
                offset += 4l;
            }
            else {
                asm_type_dst = make_Byte();
                size--;
                offset++;
            }
            push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
        }
    }
}

static void load_scalar_instr(Ctx ctx, const TacLoad* node) {
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src_ptr);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
    {
        shared_ptr_t(AsmOperand) src = gen_memory(REG_Ax, 0l);
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node->dst);
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
    }
}

static void load_instr(Ctx ctx, const TacLoad* node) {
    if (is_value_struct(ctx, node->dst)) {
        load_struct_instr(ctx, node);
    }
    else {
        load_scalar_instr(ctx, node);
    }
}

static void store_struct_instr(Ctx ctx, const TacStore* node) {
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->dst_ptr);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
    {
        TIdentifier name = node->src->get._TacVariable.name;
        const Structure* struct_type = &map_get(ctx->frontend->symbol_table, name)->type_t->get._Structure;
        TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
        TLong offset = 0l;
        while (size > 0l) {
            shared_ptr_t(AsmOperand) src = make_AsmPseudoMem(name, offset);
            shared_ptr_t(AsmOperand) dst = gen_memory(REG_Ax, offset);
            shared_ptr_t(AssemblyType) asm_type_dst = sptr_new();
            if (size >= 8l) {
                asm_type_dst = make_QuadWord();
                size -= 8l;
                offset += 8l;
            }
            else if (size >= 4l) {
                asm_type_dst = make_LongWord();
                size -= 4l;
                offset += 4l;
            }
            else {
                asm_type_dst = make_Byte();
                size--;
                offset++;
            }
            push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
        }
    }
}

static void store_scalar_instr(Ctx ctx, const TacStore* node) {
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->dst_ptr);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
        shared_ptr_t(AsmOperand) dst = gen_memory(REG_Ax, 0l);
        shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node->src);
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
    }
}

static void store_instr(Ctx ctx, const TacStore* node) {
    if (is_value_struct(ctx, node->src)) {
        store_struct_instr(ctx, node);
    }
    else {
        store_scalar_instr(ctx, node);
    }
}

static void const_idx_add_ptr_instr(Ctx ctx, const TacAddPtr* node) {
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src_ptr);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
    {
        shared_ptr_t(AsmOperand) src = sptr_new();
        {
            const CConst* constant = node->idx->get._TacConstant.constant;
            THROW_ABORT_IF(constant->type != AST_CConstLong_t);
            src = gen_memory(REG_Ax, constant->get._CConstLong.value * node->scale);
        }
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmLea(&src, &dst));
    }
}

static void scalar_idx_add_ptr_instr(Ctx ctx, const TacAddPtr* node) {
    shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src_ptr);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src, asm_type_src_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src_cp, &src, &dst));
    }
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->idx);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Dx);
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
    {
        shared_ptr_t(AsmOperand) src = gen_indexed(REG_Ax, REG_Dx, node->scale);
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmLea(&src, &dst));
    }
}

static void aggr_idx_add_ptr_instr(Ctx ctx, const TacAddPtr* node) {
    shared_ptr_t(AssemblyType) asm_type_src = make_QuadWord();
    shared_ptr_t(AsmOperand) src_dst = gen_register(REG_Dx);
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src_ptr);
        shared_ptr_t(AsmOperand) dst = gen_register(REG_Ax);
        shared_ptr_t(AssemblyType) asm_type_src_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src, asm_type_src_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src_cp, &src, &dst));
    }
    {
        shared_ptr_t(AsmOperand) src = gen_op(ctx, node->idx);
        shared_ptr_t(AsmOperand) src_dst_cp = sptr_new();
        sptr_copy(AsmOperand, src_dst, src_dst_cp);
        shared_ptr_t(AssemblyType) asm_type_src_cp = sptr_new();
        sptr_copy(AssemblyType, asm_type_src, asm_type_src_cp);
        push_instr(ctx, make_AsmMov(&asm_type_src_cp, &src, &src_dst_cp));
    }
    {
        shared_ptr_t(AsmOperand) src = sptr_new();
        {
            TULong value = (TULong)node->scale;
            bool is_byte = node->scale <= 127l && node->scale >= -128l;
            bool is_quad = node->scale > 2147483647l || node->scale < -2147483648l;
            bool is_neg = node->scale < 0l;
            src = make_AsmImm(value, is_byte, is_quad, is_neg);
        }
        AsmBinaryOp binop = init_AsmMult();
        push_instr(ctx, make_AsmBinary(&binop, &asm_type_src, &src, &src_dst));
    }
    {
        shared_ptr_t(AsmOperand) src = gen_indexed(REG_Ax, REG_Dx, 1l);
        shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
        push_instr(ctx, make_AsmLea(&src, &dst));
    }
}

static void var_idx_add_ptr_instr(Ctx ctx, const TacAddPtr* node) {
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

static void add_ptr_instr(Ctx ctx, const TacAddPtr* node) {
    switch (node->idx->type) {
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

static void cp_to_offset_struct_instr(Ctx ctx, const TacCopyToOffset* node) {
    TIdentifier src_name = node->src->get._TacVariable.name;
    const Structure* struct_type = &map_get(ctx->frontend->symbol_table, src_name)->type_t->get._Structure;
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    TLong offset = 0l;
    while (size > 0l) {
        shared_ptr_t(AsmOperand) src = make_AsmPseudoMem(src_name, offset);
        shared_ptr_t(AsmOperand) dst = sptr_new();
        {
            TIdentifier dst_name = node->dst_name;
            TLong to_offset = offset + node->offset;
            dst = make_AsmPseudoMem(dst_name, to_offset);
        }
        shared_ptr_t(AssemblyType) asm_type_src = sptr_new();
        if (size >= 8l) {
            asm_type_src = make_QuadWord();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            asm_type_src = make_LongWord();
            size -= 4l;
            offset += 4l;
        }
        else {
            asm_type_src = make_Byte();
            size--;
            offset++;
        }
        push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
    }
}

static void cp_to_offset_scalar_instr(Ctx ctx, const TacCopyToOffset* node) {
    shared_ptr_t(AsmOperand) src = gen_op(ctx, node->src);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    {
        TIdentifier dst_name = node->dst_name;
        TLong to_offset = node->offset;
        dst = make_AsmPseudoMem(dst_name, to_offset);
    }
    shared_ptr_t(AssemblyType) asm_type_src = gen_asm_type(ctx, node->src);
    push_instr(ctx, make_AsmMov(&asm_type_src, &src, &dst));
}

static void cp_to_offset_instr(Ctx ctx, const TacCopyToOffset* node) {
    if (is_value_struct(ctx, node->src)) {
        cp_to_offset_struct_instr(ctx, node);
    }
    else {
        cp_to_offset_scalar_instr(ctx, node);
    }
}

static void cp_from_offset_struct_instr(Ctx ctx, const TacCopyFromOffset* node) {
    TIdentifier dst_name = node->dst->get._TacVariable.name;
    const Structure* struct_type = &map_get(ctx->frontend->symbol_table, dst_name)->type_t->get._Structure;
    TLong size = map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    TLong offset = 0l;
    while (size > 0l) {
        shared_ptr_t(AsmOperand) src = sptr_new();
        {
            TIdentifier src_name = node->src_name;
            TLong from_offset = offset + node->offset;
            src = make_AsmPseudoMem(src_name, from_offset);
        }
        shared_ptr_t(AsmOperand) dst = make_AsmPseudoMem(dst_name, offset);
        shared_ptr_t(AssemblyType) asm_type_dst = sptr_new();
        if (size >= 8l) {
            asm_type_dst = make_QuadWord();
            size -= 8l;
            offset += 8l;
        }
        else if (size >= 4l) {
            asm_type_dst = make_LongWord();
            size -= 4l;
            offset += 4l;
        }
        else {
            asm_type_dst = make_Byte();
            size--;
            offset++;
        }
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
    }
}

static void cp_from_offset_scalar_instr(Ctx ctx, const TacCopyFromOffset* node) {
    shared_ptr_t(AsmOperand) src = sptr_new();
    {
        TIdentifier src_name = node->src_name;
        TLong from_offset = node->offset;
        src = make_AsmPseudoMem(src_name, from_offset);
    }
    shared_ptr_t(AsmOperand) dst = gen_op(ctx, node->dst);
    shared_ptr_t(AssemblyType) asm_type_dst = gen_asm_type(ctx, node->dst);
    push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
}

static void cp_from_offset_instr(Ctx ctx, const TacCopyFromOffset* node) {
    if (is_value_struct(ctx, node->dst)) {
        cp_from_offset_struct_instr(ctx, node);
    }
    else {
        cp_from_offset_scalar_instr(ctx, node);
    }
}

static void jump_instr(Ctx ctx, const TacJump* node) {
    TIdentifier target = node->target;
    push_instr(ctx, make_AsmJmp(target));
}

static void jmp_eq_0_int_instr(Ctx ctx, const TacJumpIfZero* node) {
    {
        shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) condition = gen_op(ctx, node->condition);
        shared_ptr_t(AssemblyType) asm_type_cond = gen_asm_type(ctx, node->condition);
        push_instr(ctx, make_AsmCmp(&asm_type_cond, &imm_zero, &condition));
    }
    {
        TIdentifier target = node->target;
        AsmCondCode cond_code_e = init_AsmE();
        push_instr(ctx, make_AsmJmpCC(target, &cond_code_e));
    }
}

static void jmp_eq_0_dbl_instr(Ctx ctx, const TacJumpIfZero* node) {
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    zero_xmm_reg_instr(ctx);
    {
        shared_ptr_t(AsmOperand) condition = gen_op(ctx, node->condition);
        shared_ptr_t(AsmOperand) reg_zero = gen_register(REG_Xmm0);
        shared_ptr_t(AssemblyType) asm_type_cond = make_BackendDouble();
        push_instr(ctx, make_AsmCmp(&asm_type_cond, &condition, &reg_zero));
    }
    {
        AsmCondCode cond_code_p = init_AsmP();
        push_instr(ctx, make_AsmJmpCC(target_nan, &cond_code_p));
    }
    {
        TIdentifier target = node->target;
        AsmCondCode cond_code_e = init_AsmE();
        push_instr(ctx, make_AsmJmpCC(target, &cond_code_e));
    }
    push_instr(ctx, make_AsmLabel(target_nan));
}

static void jmp_eq_0_instr(Ctx ctx, const TacJumpIfZero* node) {
    if (is_value_dbl(ctx, node->condition)) {
        jmp_eq_0_dbl_instr(ctx, node);
    }
    else {
        jmp_eq_0_int_instr(ctx, node);
    }
}

static void jmp_ne_0_int_instr(Ctx ctx, const TacJumpIfNotZero* node) {
    {
        shared_ptr_t(AsmOperand) imm_zero = make_AsmImm(0ul, true, false, false);
        shared_ptr_t(AsmOperand) condition = gen_op(ctx, node->condition);
        shared_ptr_t(AssemblyType) asm_type_cond = gen_asm_type(ctx, node->condition);
        push_instr(ctx, make_AsmCmp(&asm_type_cond, &imm_zero, &condition));
    }
    {
        TIdentifier target = node->target;
        AsmCondCode cond_code_ne = init_AsmNE();
        push_instr(ctx, make_AsmJmpCC(target, &cond_code_ne));
    }
}

static void jmp_ne_0_dbl_instr(Ctx ctx, const TacJumpIfNotZero* node) {
    TIdentifier target = node->target;
    TIdentifier target_nan = repr_asm_label(ctx, LBL_Lcomisd_nan);
    TIdentifier target_nan_ne = repr_asm_label(ctx, LBL_Lcomisd_nan);
    zero_xmm_reg_instr(ctx);
    {
        shared_ptr_t(AsmOperand) condition = gen_op(ctx, node->condition);
        shared_ptr_t(AsmOperand) reg_zero = gen_register(REG_Xmm0);
        shared_ptr_t(AssemblyType) asm_type_cond = make_BackendDouble();
        push_instr(ctx, make_AsmCmp(&asm_type_cond, &condition, &reg_zero));
    }
    {
        AsmCondCode cond_code_p = init_AsmP();
        push_instr(ctx, make_AsmJmpCC(target_nan, &cond_code_p));
    }
    {
        AsmCondCode cond_code_ne = init_AsmNE();
        push_instr(ctx, make_AsmJmpCC(target, &cond_code_ne));
    }
    push_instr(ctx, make_AsmJmp(target_nan_ne));
    push_instr(ctx, make_AsmLabel(target_nan));
    {
        AsmCondCode cond_code_e = init_AsmE();
        push_instr(ctx, make_AsmJmpCC(target, &cond_code_e));
    }
    push_instr(ctx, make_AsmLabel(target_nan_ne));
}

static void jmp_ne_0_instr(Ctx ctx, const TacJumpIfNotZero* node) {
    if (is_value_dbl(ctx, node->condition)) {
        jmp_ne_0_dbl_instr(ctx, node);
    }
    else {
        jmp_ne_0_int_instr(ctx, node);
    }
}

static void label_instr(Ctx ctx, const TacLabel* node) {
    TIdentifier name = node->name;
    push_instr(ctx, make_AsmLabel(name));
}

static void gen_instr(Ctx ctx, const TacInstruction* node) {
    switch (node->type) {
        case AST_TacReturn_t:
            ret_instr(ctx, &node->get._TacReturn);
            break;
        case AST_TacSignExtend_t:
            sign_extend_instr(ctx, &node->get._TacSignExtend);
            break;
        case AST_TacTruncate_t:
            truncate_instr(ctx, &node->get._TacTruncate);
            break;
        case AST_TacZeroExtend_t:
            zero_extend_instr(ctx, &node->get._TacZeroExtend);
            break;
        case AST_TacDoubleToInt_t:
            dbl_to_signed_instr(ctx, &node->get._TacDoubleToInt);
            break;
        case AST_TacDoubleToUInt_t:
            dbl_to_unsigned_instr(ctx, &node->get._TacDoubleToUInt);
            break;
        case AST_TacIntToDouble_t:
            signed_to_dbl_instr(ctx, &node->get._TacIntToDouble);
            break;
        case AST_TacUIntToDouble_t:
            unsigned_to_dbl_instr(ctx, &node->get._TacUIntToDouble);
            break;
        case AST_TacFunCall_t:
            call_instr(ctx, &node->get._TacFunCall);
            break;
        case AST_TacUnary_t:
            unary_instr(ctx, &node->get._TacUnary);
            break;
        case AST_TacBinary_t:
            binary_instr(ctx, &node->get._TacBinary);
            break;
        case AST_TacCopy_t:
            copy_instr(ctx, &node->get._TacCopy);
            break;
        case AST_TacGetAddress_t:
            getaddr_instr(ctx, &node->get._TacGetAddress);
            break;
        case AST_TacLoad_t:
            load_instr(ctx, &node->get._TacLoad);
            break;
        case AST_TacStore_t:
            store_instr(ctx, &node->get._TacStore);
            break;
        case AST_TacAddPtr_t:
            add_ptr_instr(ctx, &node->get._TacAddPtr);
            break;
        case AST_TacCopyToOffset_t:
            cp_to_offset_instr(ctx, &node->get._TacCopyToOffset);
            break;
        case AST_TacCopyFromOffset_t:
            cp_from_offset_instr(ctx, &node->get._TacCopyFromOffset);
            break;
        case AST_TacJump_t:
            jump_instr(ctx, &node->get._TacJump);
            break;
        case AST_TacJumpIfZero_t:
            jmp_eq_0_instr(ctx, &node->get._TacJumpIfZero);
            break;
        case AST_TacJumpIfNotZero_t:
            jmp_ne_0_instr(ctx, &node->get._TacJumpIfNotZero);
            break;
        case AST_TacLabel_t:
            label_instr(ctx, &node->get._TacLabel);
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
static void gen_instr_list(Ctx ctx, vector_t(unique_ptr_t(TacInstruction)) node_list) {
    for (size_t i = 0; i < vec_size(node_list); ++i) {
        if (node_list[i]) {
            gen_instr(ctx, node_list[i]);
        }
    }
}

static void reg_fun_param_instr(Ctx ctx, TIdentifier name, REGISTER_KIND arg_reg) {
    shared_ptr_t(AsmOperand) src = gen_register(arg_reg);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    {
        TIdentifier dst_name = name;
        dst = make_AsmPseudo(dst_name);
    }
    shared_ptr_t(AssemblyType) asm_type_dst = cvt_backend_asm_type(ctx->frontend, name);
    push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
}

static void stack_fun_param_instr(Ctx ctx, TIdentifier name, TLong stack_bytes) {
    shared_ptr_t(AsmOperand) src = gen_memory(REG_Bp, stack_bytes);
    shared_ptr_t(AsmOperand) dst = sptr_new();
    {
        TIdentifier dst_name = name;
        dst = make_AsmPseudo(dst_name);
    }
    shared_ptr_t(AssemblyType) asm_type_dst = cvt_backend_asm_type(ctx->frontend, name);
    push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
}

static void reg_8b_fun_param_instr(
    Ctx ctx, TIdentifier name, TLong offset, const Structure* struct_type, REGISTER_KIND arg_reg) {
    ret_8b_call_instr(ctx, name, offset, struct_type, arg_reg);
}

static void stack_8b_fun_param_instr(
    Ctx ctx, TIdentifier name, TLong stack_bytes, TLong offset, const Structure* struct_type) {
    shared_ptr_t(AssemblyType) asm_type_dst = asm_type_8b(ctx, struct_type, offset);
    if (asm_type_dst->type == AST_ByteArray_t) {
        TLong size = asm_type_dst->get._ByteArray.size;
        free_AssemblyType(&asm_type_dst);
        while (size > 0l) {
            shared_ptr_t(AsmOperand) src = gen_memory(REG_Bp, stack_bytes);
            shared_ptr_t(AsmOperand) dst = make_AsmPseudoMem(name, offset);
            if (size >= 4l) {
                asm_type_dst = make_LongWord();
                size -= 4l;
                offset += 4l;
                stack_bytes += 4l;
            }
            else {
                asm_type_dst = make_Byte();
                size--;
                offset++;
                stack_bytes++;
            }
            push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
        }
    }
    else {
        shared_ptr_t(AsmOperand) src = gen_memory(REG_Bp, stack_bytes);
        shared_ptr_t(AsmOperand) dst = sptr_new();
        {
            TIdentifier dst_name = name;
            TLong to_offset = offset;
            dst = make_AsmPseudoMem(dst_name, to_offset);
        }
        push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
    }
}

static void fun_param_toplvl(Ctx ctx, const TacFunction* node, FunType* fun_type, bool is_ret_memory) {
    size_t reg_size = is_ret_memory ? 1 : 0;
    size_t sse_size = 0;
    TLong stack_bytes = 16l;
    for (size_t i = 0; i < vec_size(node->params); ++i) {
        TIdentifier param = node->params[i];
        const Type* param_type = map_get(ctx->frontend->symbol_table, param)->type_t;
        if (param_type->type == AST_Double_t) {
            if (sse_size < 8) {
                reg_fun_param_instr(ctx, param, ctx->sse_arg_regs[sse_size]);
                sse_size++;
            }
            else {
                stack_fun_param_instr(ctx, param, stack_bytes);
                stack_bytes += 8l;
            }
        }
        else if (param_type->type != AST_Structure_t) {
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
            const Structure* struct_type = &param_type->get._Structure;
            struct_8b_class(ctx, struct_type);
            const Struct8Bytes* struct_8b = &map_get(ctx->struct_8b_map, struct_type->tag);
            if (struct_8b->clss[0] != CLS_memory) {
                struct_reg_size = 0;
                struct_sse_size = 0;
                for (size_t j = 0; j < struct_8b->size; ++j) {
                    if (struct_8b->clss[j] == CLS_sse) {
                        struct_sse_size++;
                    }
                    else {
                        struct_reg_size++;
                    }
                }
            }
            if (struct_reg_size + reg_size <= 6 && struct_sse_size + sse_size <= 8) {
                TLong offset = 0l;
                for (size_t j = 0; j < struct_8b->size; ++j) {
                    if (struct_8b->clss[j] == CLS_sse) {
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
                for (size_t j = 0; j < struct_8b->size; ++j) {
                    stack_8b_fun_param_instr(ctx, param, stack_bytes, offset, struct_type);
                    stack_bytes += 8l;
                    offset += 8l;
                }
            }
        }
    }
    fun_param_reg_mask(ctx, fun_type, reg_size, sse_size);
}

static unique_ptr_t(AsmTopLevel) gen_fun_toplvl(Ctx ctx, const TacFunction* node) {
    TIdentifier name = node->name;
    bool is_glob = node->is_glob;
    bool is_ret_memory = false;

    vector_t(unique_ptr_t(AsmInstruction)) body = vec_new();
    vec_reserve(body, vec_size(node->body));
    {
        ctx->p_instrs = &body;

        FunType* fun_type = &map_get(ctx->frontend->symbol_table, node->name)->type_t->get._FunType;
        if (fun_type->ret_type->type == AST_Structure_t) {
            const Structure* struct_type = &fun_type->ret_type->get._Structure;
            struct_8b_class(ctx, struct_type);
            if (map_get(ctx->struct_8b_map, struct_type->tag).clss[0] == CLS_memory) {
                is_ret_memory = true;
                {
                    shared_ptr_t(AsmOperand) src = gen_register(REG_Di);
                    shared_ptr_t(AsmOperand) dst = gen_memory(REG_Bp, -8l);
                    shared_ptr_t(AssemblyType) asm_type_dst = make_QuadWord();
                    push_instr(ctx, make_AsmMov(&asm_type_dst, &src, &dst));
                }
            }
        }
        fun_param_toplvl(ctx, node, fun_type, is_ret_memory);

        ctx->p_fun_type = fun_type;
        gen_instr_list(ctx, node->body);
        ctx->p_fun_type = NULL;
        ctx->p_instrs = NULL;
    }

    return make_AsmFunction(name, is_glob, is_ret_memory, &body);
}

static unique_ptr_t(AsmTopLevel) gen_static_var_toplvl(Ctx ctx, const TacStaticVariable* node) {
    TIdentifier name = node->name;
    bool is_glob = node->is_glob;
    TInt alignment = gen_type_alignment(ctx->frontend, node->static_init_type);
    vector_t(shared_ptr_t(StaticInit)) static_inits = vec_new();
    vec_reserve(static_inits, vec_size(node->static_inits));
    for (size_t i = 0; i < vec_size(node->static_inits); ++i) {
        shared_ptr_t(StaticInit) static_init = sptr_new();
        sptr_copy(StaticInit, node->static_inits[i], static_init);
        vec_move_back(static_inits, static_init);
    }
    return make_AsmStaticVariable(name, alignment, is_glob, &static_inits);
}

static void push_static_const_toplvl(Ctx ctx, unique_ptr_t(AsmTopLevel) static_const_toplvls) {
    vec_move_back(*ctx->p_static_consts, static_const_toplvls);
}

static void dbl_static_const_toplvl(Ctx ctx, TIdentifier identifier, TIdentifier dbl_const, TInt byte) {
    TIdentifier name = identifier;
    TInt alignment = byte;
    shared_ptr_t(StaticInit) static_init = make_DoubleInit(dbl_const);
    push_static_const_toplvl(ctx, make_AsmStaticConstant(name, alignment, &static_init));
}

static unique_ptr_t(AsmTopLevel) gen_static_const_toplvl(Ctx ctx, const TacStaticConstant* node) {
    TIdentifier name = node->name;
    TInt alignment = gen_type_alignment(ctx->frontend, node->static_init_type);
    shared_ptr_t(StaticInit) static_init = sptr_new();
    sptr_copy(StaticInit, node->static_init, static_init);
    return make_AsmStaticConstant(name, alignment, &static_init);
}

// top_level = Function(identifier, bool, bool, instruction*) | StaticVariable(identifier, bool, int, static_init*)
//           | StaticConstant(identifier, int, static_init)
static unique_ptr_t(AsmTopLevel) gen_toplvl(Ctx ctx, const TacTopLevel* node) {
    switch (node->type) {
        case AST_TacFunction_t:
            return gen_fun_toplvl(ctx, &node->get._TacFunction);
        case AST_TacStaticVariable_t:
            return gen_static_var_toplvl(ctx, &node->get._TacStaticVariable);
        case AST_TacStaticConstant_t:
            return gen_static_const_toplvl(ctx, &node->get._TacStaticConstant);
        default:
            THROW_ABORT;
    }
}

// AST = Program(top_level*, top_level*)
static unique_ptr_t(AsmProgram) gen_program(Ctx ctx, const TacProgram* node) {
    vector_t(unique_ptr_t(AsmTopLevel)) static_const_toplvls = vec_new();
    vec_reserve(static_const_toplvls, vec_size(node->static_const_toplvls));
    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        unique_ptr_t(AsmTopLevel) static_const_toplvl = gen_toplvl(ctx, node->static_const_toplvls[i]);
        vec_move_back(static_const_toplvls, static_const_toplvl);
    }

    vector_t(unique_ptr_t(AsmTopLevel)) top_levels = vec_new();
    vec_reserve(top_levels, vec_size(node->static_var_toplvls) + vec_size(node->fun_toplvls));
    {
        ctx->p_static_consts = &static_const_toplvls;

        for (size_t i = 0; i < vec_size(node->static_var_toplvls); ++i) {
            unique_ptr_t(AsmTopLevel) static_var_toplvl = gen_toplvl(ctx, node->static_var_toplvls[i]);
            vec_move_back(top_levels, static_var_toplvl);
        }
        for (size_t i = 0; i < vec_size(node->fun_toplvls); ++i) {
            unique_ptr_t(AsmTopLevel) fun_toplvl = gen_toplvl(ctx, node->fun_toplvls[i]);
            vec_move_back(top_levels, fun_toplvl);
        }
        ctx->p_static_consts = NULL;
    }

    return make_AsmProgram(&static_const_toplvls, &top_levels);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unique_ptr_t(AsmProgram)
    generate_assembly(unique_ptr_t(TacProgram) * tac_ast, FrontEndContext* frontend, IdentifierContext* identifiers) {
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
        ctx.struct_8b_map = map_new();
    }
    unique_ptr_t(AsmProgram) asm_ast = gen_program(&ctx, *tac_ast);

    free_TacProgram(tac_ast);
    THROW_ABORT_IF(!asm_ast);
    map_delete(ctx.dbl_const_table);
    map_delete(ctx.struct_8b_map);
    return asm_ast;
}
