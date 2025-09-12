#include <string.h>

#include "util/c_std.h"
#include "util/str2t.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/front_ast.h"
#include "ast/front_symt.h"
#include "ast/interm_ast.h"

#include "frontend/intermediate/idents.h"
#include "frontend/intermediate/tac_repr.h"

typedef struct TacReprContext {
    FrontEndContext* frontend;
    IdentifierContext* identifiers;
    // Three address code representation
    vector_t(unique_ptr_t(TacInstruction)) * p_instrs;
    vector_t(unique_ptr_t(TacTopLevel)) * p_toplvls;
    vector_t(unique_ptr_t(TacTopLevel)) * p_static_consts;
} TacReprContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code representation

typedef TacReprContext* Ctx;

// unary_operator = Complement | Negate | Not
static unique_ptr_t(TacUnaryOp) repr_unop(CUnaryOp* node) {
    switch (node->type) {
        case AST_CComplement_t:
            return make_TacComplement();
        case AST_CNegate_t:
            return make_TacNegate();
        case AST_CNot_t:
            return make_TacNot();
        default:
            THROW_ABORT;
    }
}

// binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor | BitShiftLeft
//                 | BitShiftRight | BitShrArithmetic | Equal | NotEqual | LessThan | LessOrEqual | GreaterThan |
//                 GreaterOrEqual
static unique_ptr_t(TacBinaryOp) repr_binop(CBinaryOp* node) {
    switch (node->type) {
        case AST_CAdd_t:
            return make_TacAdd();
        case AST_CSubtract_t:
            return make_TacSubtract();
        case AST_CMultiply_t:
            return make_TacMultiply();
        case AST_CDivide_t:
            return make_TacDivide();
        case AST_CRemainder_t:
            return make_TacRemainder();
        case AST_CBitAnd_t:
            return make_TacBitAnd();
        case AST_CBitOr_t:
            return make_TacBitOr();
        case AST_CBitXor_t:
            return make_TacBitXor();
        case AST_CBitShiftLeft_t:
            return make_TacBitShiftLeft();
        case AST_CBitShiftRight_t:
            return make_TacBitShiftRight();
        case AST_CBitShrArithmetic_t:
            return make_TacBitShrArithmetic();
        case AST_CEqual_t:
            return make_TacEqual();
        case AST_CNotEqual_t:
            return make_TacNotEqual();
        case AST_CLessThan_t:
            return make_TacLessThan();
        case AST_CLessOrEqual_t:
            return make_TacLessOrEqual();
        case AST_CGreaterThan_t:
            return make_TacGreaterThan();
        case AST_CGreaterOrEqual_t:
            return make_TacGreaterOrEqual();
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) const_value(CConstant* node) {
    shared_ptr_t(CConst) constant = sptr_new();
    sptr_copy(CConst, node->constant, constant);
    return make_TacConstant(&constant);
}

static shared_ptr_t(TacValue) var_value(CVar* node) {
    TIdentifier name = node->name;
    return make_TacVariable(name);
}

static shared_ptr_t(TacValue) exp_inner_value(Ctx ctx, CExp* node, bool is_ptr) {
    TIdentifier inner_name = repr_var_identifier(ctx->identifiers, node);
    if (map_find(ctx->frontend->symbol_table, inner_name) == map_end()) {
        shared_ptr_t(Type) inner_type = sptr_new();
        if (is_ptr) {
            inner_type = make_Long();
        }
        else {
            sptr_copy(Type, node->exp_type, inner_type);
        }
        unique_ptr_t(IdentifierAttr) inner_attrs = make_LocalAttr();
        unique_ptr_t(Symbol) symbol = make_Symbol(&inner_type, &inner_attrs);
        map_move_add(ctx->frontend->symbol_table, inner_name, symbol);
    }
    return make_TacVariable(inner_name);
}

static shared_ptr_t(TacValue) plain_inner_value(Ctx ctx, CExp* node) { return exp_inner_value(ctx, node, false); }

static shared_ptr_t(TacValue) ptr_inner_value(Ctx ctx, CExp* node) { return exp_inner_value(ctx, node, true); }

// val = Constant(int) | Var(identifier)
static shared_ptr_t(TacValue) repr_value(CExp* node) {
    switch (node->type) {
        case AST_CConstant_t:
            return const_value(&node->get._CConstant);
        case AST_CVar_t:
            return var_value(&node->get._CVar);
        default:
            THROW_ABORT;
    }
}

static void push_instr(Ctx ctx, unique_ptr_t(TacInstruction) instr) { vec_move_back(*ctx->p_instrs, instr); }

static unique_ptr_t(TacExpResult) repr_res_instr(Ctx ctx, CExp* node);
static shared_ptr_t(TacValue) repr_exp_instr(Ctx ctx, CExp* node);

static unique_ptr_t(TacExpResult) const_res_instr(CConstant* node) {
    shared_ptr_t(TacValue) val = repr_value(node->_base);
    return make_TacPlainOperand(&val);
}

static TIdentifier make_literal_identifier(Ctx ctx, CStringLiteral* node) {
    string_t value = string_literal_to_const(node->value);
    return make_string_identifier(ctx->identifiers, &value);
}

static unique_ptr_t(TacExpResult) string_res_instr(Ctx ctx, CString* node) {
    TIdentifier string_const_label;
    {
        TIdentifier string_const = make_literal_identifier(ctx, node->literal);
        ssize_t map_it = map_find(ctx->frontend->string_const_table, string_const);
        if (map_it != map_end()) {
            string_const_label = pair_second(ctx->frontend->string_const_table[map_it]);
        }
        else {
            string_const_label = repr_label_identifier(ctx->identifiers, LBL_Lstring);
            map_add(ctx->frontend->string_const_table, string_const, string_const_label);
            shared_ptr_t(Type) constant_type = sptr_new();
            {
                TLong size = ((TLong)vec_size(node->literal->value)) + 1l;
                shared_ptr_t(Type) elem_type = make_Char();
                constant_type = make_Array(size, &elem_type);
            }
            unique_ptr_t(IdentifierAttr) constant_attrs = uptr_new();
            {
                shared_ptr_t(StaticInit) static_init = sptr_new();
                {
                    shared_ptr_t(CStringLiteral) literal = sptr_new();
                    sptr_copy(CStringLiteral, node->literal, literal);
                    static_init = make_StringInit(string_const, true, &literal);
                }
                constant_attrs = make_ConstantAttr(&static_init);
            }
            unique_ptr_t(Symbol) symbol = make_Symbol(&constant_type, &constant_attrs);
            map_move_add(ctx->frontend->symbol_table, string_const_label, symbol);
        }
    }
    shared_ptr_t(TacValue) val = make_TacVariable(string_const_label);
    return make_TacPlainOperand(&val);
}

static unique_ptr_t(TacExpResult) var_res_instr(CVar* node) {
    shared_ptr_t(TacValue) val = repr_value(node->_base);
    return make_TacPlainOperand(&val);
}

static bool is_type_signed(Type* type) {
    switch (type->type) {
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

static TInt get_scalar_size(Type* type) {
    switch (type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            return 1;
        case AST_Int_t:
        case AST_UInt_t:
            return 4;
        case AST_Long_t:
        case AST_Double_t:
        case AST_ULong_t:
        case AST_Pointer_t:
            return 8;
        default:
            THROW_ABORT;
    }
}

static TLong get_type_scale(Ctx ctx, Type* type);

static TLong get_arr_scale(Ctx ctx, Array* arr_type) {
    TLong size = arr_type->size;
    while (arr_type->elem_type->type == AST_Array_t) {
        arr_type = &arr_type->elem_type->get._Array;
        size *= arr_type->size;
    }
    return get_type_scale(ctx, arr_type->elem_type) * size;
}

static TLong get_struct_scale(Ctx ctx, Structure* struct_type) {
    return map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
}

static TLong get_type_scale(Ctx ctx, Type* type) {
    switch (type->type) {
        case AST_Array_t:
            return get_arr_scale(ctx, &type->get._Array);
        case AST_Structure_t:
            return get_struct_scale(ctx, &type->get._Structure);
        default:
            return get_scalar_size(type);
    }
}

static unique_ptr_t(TacExpResult) cast_complete_res_instr(Ctx ctx, CCast* node) {
    shared_ptr_t(TacValue) src = repr_exp_instr(ctx, node->exp);
    if (node->target_type->type == node->exp->exp_type->type) {
        return make_TacPlainOperand(&src);
    }

    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    if (node->exp->exp_type->type == AST_Double_t) {
        if (is_type_signed(node->target_type)) {
            push_instr(ctx, make_TacDoubleToInt(&src, &dst_cp));
        }
        else {
            push_instr(ctx, make_TacDoubleToUInt(&src, &dst_cp));
        }
    }
    else if (node->target_type->type == AST_Double_t) {
        if (is_type_signed(node->exp->exp_type)) {
            push_instr(ctx, make_TacIntToDouble(&src, &dst_cp));
        }
        else {
            push_instr(ctx, make_TacUIntToDouble(&src, &dst_cp));
        }
    }
    else {
        TInt target_type_size = get_scalar_size(node->target_type);
        TInt inner_type_size = get_scalar_size(node->exp->exp_type);
        if (target_type_size == inner_type_size) {
            push_instr(ctx, make_TacCopy(&src, &dst_cp));
        }
        else if (target_type_size < inner_type_size) {
            push_instr(ctx, make_TacTruncate(&src, &dst_cp));
        }
        else if (is_type_signed(node->exp->exp_type)) {
            push_instr(ctx, make_TacSignExtend(&src, &dst_cp));
        }
        else {
            push_instr(ctx, make_TacZeroExtend(&src, &dst_cp));
        }
    }
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) cast_void_res_instr(Ctx ctx, CCast* node) {
    shared_ptr_t(TacValue) dst = repr_exp_instr(ctx, node->exp);
    free_TacValue(&dst);
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) cast_res_instr(Ctx ctx, CCast* node) {
    if (node->target_type->type == AST_Void_t) {
        return cast_void_res_instr(ctx, node);
    }
    else {
        return cast_complete_res_instr(ctx, node);
    }
}

static unique_ptr_t(TacExpResult) unary_res_instr(Ctx ctx, CUnary* node) {
    shared_ptr_t(TacValue) src = repr_exp_instr(ctx, node->exp);
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    unique_ptr_t(TacUnaryOp) unop = repr_unop(&node->unop);
    push_instr(ctx, make_TacUnary(&unop, &src, &dst_cp));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) binary_any_res_instr(Ctx ctx, CBinary* node);

static unique_ptr_t(TacExpResult) binary_add_ptr_res_instr(Ctx ctx, CBinary* node) {
    TLong scale;
    shared_ptr_t(TacValue) src_ptr = sptr_new();
    shared_ptr_t(TacValue) idx = sptr_new();
    if (node->exp_left->exp_type->type == AST_Pointer_t) {
        scale = get_type_scale(ctx, node->exp_left->exp_type->get._Pointer.ref_type);
        src_ptr = repr_exp_instr(ctx, node->exp_left);
        idx = repr_exp_instr(ctx, node->exp_right);
    }
    else {
        scale = get_type_scale(ctx, node->exp_right->exp_type->get._Pointer.ref_type);
        src_ptr = repr_exp_instr(ctx, node->exp_right);
        idx = repr_exp_instr(ctx, node->exp_left);
    }
    shared_ptr_t(TacValue) dst = ptr_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    push_instr(ctx, make_TacAddPtr(scale, &src_ptr, &idx, &dst_cp));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) binary_add_res_instr(Ctx ctx, CBinary* node) {
    if (node->exp_left->exp_type->type == AST_Pointer_t || node->exp_right->exp_type->type == AST_Pointer_t) {
        return binary_add_ptr_res_instr(ctx, node);
    }
    else {
        return binary_any_res_instr(ctx, node);
    }
}

static unique_ptr_t(TacExpResult) binary_sub_to_ptr_res_instr(Ctx ctx, CBinary* node) {
    TLong scale = get_type_scale(ctx, node->exp_left->exp_type->get._Pointer.ref_type);
    shared_ptr_t(TacValue) src_ptr = repr_exp_instr(ctx, node->exp_left);
    shared_ptr_t(TacValue) idx = sptr_new();
    {
        idx = repr_exp_instr(ctx, node->exp_right);
        shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        unique_ptr_t(TacUnaryOp) unop = make_TacNegate();
        push_instr(ctx, make_TacUnary(&unop, &idx, &dst_cp));
        sptr_move(TacValue, dst, idx);
    }
    shared_ptr_t(TacValue) dst = ptr_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    push_instr(ctx, make_TacAddPtr(scale, &src_ptr, &idx, &dst_cp));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) binary_subtract_ptr_res_instr(Ctx ctx, CBinary* node) {
    shared_ptr_t(TacValue) src_1 = sptr_new();
    {
        src_1 = repr_exp_instr(ctx, node->exp_left);
        shared_ptr_t(TacValue) src_2 = repr_exp_instr(ctx, node->exp_right);
        shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        unique_ptr_t(TacBinaryOp) binop = make_TacSubtract();
        push_instr(ctx, make_TacBinary(&binop, &src_1, &src_2, &dst_cp));
        sptr_move(TacValue, dst, src_1);
    }
    shared_ptr_t(TacValue) src_2 = sptr_new();
    {
        TLong value = get_type_scale(ctx, node->exp_left->exp_type->get._Pointer.ref_type);
        shared_ptr_t(CConst) constant = make_CConstLong(value);
        src_2 = make_TacConstant(&constant);
    }
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    unique_ptr_t(TacBinaryOp) binop = make_TacDivide();
    push_instr(ctx, make_TacBinary(&binop, &src_1, &src_2, &dst_cp));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) binary_subtract_res_instr(Ctx ctx, CBinary* node) {
    if (node->exp_left->exp_type->type == AST_Pointer_t) {
        if (node->exp_right->exp_type->type == AST_Pointer_t) {
            return binary_subtract_ptr_res_instr(ctx, node);
        }
        else {
            return binary_sub_to_ptr_res_instr(ctx, node);
        }
    }
    else {
        return binary_any_res_instr(ctx, node);
    }
}

static unique_ptr_t(TacExpResult) binary_and_res_instr(Ctx ctx, CBinary* node) {
    TIdentifier target_false = repr_label_identifier(ctx->identifiers, LBL_Land_false);
    TIdentifier target_true = repr_label_identifier(ctx->identifiers, LBL_Land_true);
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
    {
        shared_ptr_t(TacValue) condition_left = repr_exp_instr(ctx, node->exp_left);
        push_instr(ctx, make_TacJumpIfZero(target_false, &condition_left));
    }
    {
        shared_ptr_t(TacValue) condition_right = repr_exp_instr(ctx, node->exp_right);
        push_instr(ctx, make_TacJumpIfZero(target_false, &condition_right));
    }
    {
        shared_ptr_t(CConst) constant = make_CConstInt(1);
        shared_ptr_t(TacValue) src_true = make_TacConstant(&constant);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacCopy(&src_true, &dst_cp));
    }
    push_instr(ctx, make_TacJump(target_true));
    push_instr(ctx, make_TacLabel(target_false));
    {
        shared_ptr_t(CConst) constant = make_CConstInt(0);
        shared_ptr_t(TacValue) src_false = make_TacConstant(&constant);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacCopy(&src_false, &dst_cp));
    }
    push_instr(ctx, make_TacLabel(target_true));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) binary_or_res_instr(Ctx ctx, CBinary* node) {
    TIdentifier target_true = repr_label_identifier(ctx->identifiers, LBL_Lor_true);
    TIdentifier target_false = repr_label_identifier(ctx->identifiers, LBL_Lor_false);
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
    {
        shared_ptr_t(TacValue) condition_left = repr_exp_instr(ctx, node->exp_left);
        push_instr(ctx, make_TacJumpIfNotZero(target_true, &condition_left));
    }
    {
        shared_ptr_t(TacValue) condition_right = repr_exp_instr(ctx, node->exp_right);
        push_instr(ctx, make_TacJumpIfNotZero(target_true, &condition_right));
    }
    {
        shared_ptr_t(CConst) constant = make_CConstInt(0);
        shared_ptr_t(TacValue) src_false = make_TacConstant(&constant);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacCopy(&src_false, &dst_cp));
    }
    push_instr(ctx, make_TacJump(target_false));
    push_instr(ctx, make_TacLabel(target_true));
    {
        shared_ptr_t(CConst) constant = make_CConstInt(1);
        shared_ptr_t(TacValue) src_true = make_TacConstant(&constant);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacCopy(&src_true, &dst_cp));
    }
    push_instr(ctx, make_TacLabel(target_false));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) binary_any_res_instr(Ctx ctx, CBinary* node) {
    shared_ptr_t(TacValue) src1 = repr_exp_instr(ctx, node->exp_left);
    shared_ptr_t(TacValue) src2 = repr_exp_instr(ctx, node->exp_right);
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    unique_ptr_t(TacBinaryOp) binop = repr_binop(node->binop);
    push_instr(ctx, make_TacBinary(&binop, &src1, &src2, &dst_cp));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) binary_res_instr(Ctx ctx, CBinary* node) {
    switch (node->binop->type) {
        case AST_CAdd_t:
            return binary_add_res_instr(ctx, node);
        case AST_CSubtract_t:
            return binary_subtract_res_instr(ctx, node);
        case AST_CAnd_t:
            return binary_and_res_instr(ctx, node);
        case AST_COr_t:
            return binary_or_res_instr(ctx, node);
        default:
            return binary_any_res_instr(ctx, node);
    }
}

static void plain_op_postfix_exp_instr(Ctx ctx, TacPlainOperand* res, shared_ptr_t(TacValue) * dst) {
    shared_ptr_t(TacValue) src = sptr_new();
    sptr_copy(TacValue, res->val, src);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, *dst, dst_cp);
    push_instr(ctx, make_TacCopy(&src, &dst_cp));
}

static void deref_ptr_postfix_exp_instr(Ctx ctx, TacDereferencedPointer* res, shared_ptr_t(TacValue) * dst) {
    shared_ptr_t(TacValue) src = sptr_new();
    sptr_copy(TacValue, res->val, src);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, *dst, dst_cp);
    push_instr(ctx, make_TacLoad(&src, &dst_cp));
}

static void sub_obj_postfix_exp_instr(Ctx ctx, TacSubObject* res, shared_ptr_t(TacValue) * dst) {
    TIdentifier src_name = res->base_name;
    TLong offset = res->offset;
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, *dst, dst_cp);
    push_instr(ctx, make_TacCopyFromOffset(src_name, offset, &dst_cp));
}

static void plain_op_assign_res_instr(Ctx ctx, TacPlainOperand* res, shared_ptr_t(TacValue) * src) {
    shared_ptr_t(TacValue) dst = sptr_new();
    sptr_copy(TacValue, res->val, dst);
    push_instr(ctx, make_TacCopy(src, &dst));
}

static void deref_ptr_assign_res_instr(
    Ctx ctx, TacDereferencedPointer* res, shared_ptr_t(TacValue) * src, unique_ptr_t(TacExpResult) * exp_res) {
    shared_ptr_t(TacValue) src_cp = sptr_new();
    sptr_copy(TacValue, *src, src_cp);
    shared_ptr_t(TacValue) dst = sptr_new();
    sptr_move(TacValue, res->val, dst);
    push_instr(ctx, make_TacStore(&src_cp, &dst));
    free_TacExpResult(exp_res);
    *exp_res = make_TacPlainOperand(src);
}

static void sub_obj_assign_res_instr(
    Ctx ctx, TacSubObject* res, shared_ptr_t(TacValue) * src, unique_ptr_t(TacExpResult) * exp_res) {
    TIdentifier dst_name = res->base_name;
    TLong offset = res->offset;
    shared_ptr_t(TacValue) src_cp = sptr_new();
    sptr_copy(TacValue, *src, src_cp);
    push_instr(ctx, make_TacCopyToOffset(dst_name, offset, &src_cp));
    free_TacExpResult(exp_res);
    *exp_res = make_TacPlainOperand(src);
}

static unique_ptr_t(TacExpResult) assign_res_instr(Ctx ctx, CAssignment* node) {
    shared_ptr_t(TacValue) src = sptr_new();
    unique_ptr_t(TacExpResult) res = uptr_new();
    unique_ptr_t(TacExpResult) res_postfix = uptr_new();
    if (node->exp_left) {
        src = repr_exp_instr(ctx, node->exp_right);
        res = repr_res_instr(ctx, node->exp_left);
    }
    else {
        uint32_t label_count_1 = ctx->identifiers->label_count;
        uint32_t var_count_1 = ctx->identifiers->var_count;
        uint32_t struct_count_1 = ctx->identifiers->struct_count;

        src = repr_exp_instr(ctx, node->exp_right);

        uint32_t label_count_2 = ctx->identifiers->label_count;
        uint32_t var_count_2 = ctx->identifiers->var_count;
        uint32_t struct_count_2 = ctx->identifiers->struct_count;

        ctx->identifiers->label_count = label_count_1;
        ctx->identifiers->var_count = var_count_1;
        ctx->identifiers->struct_count = struct_count_1;

        {
            CExp* exp_left = node->exp_right;
            if (exp_left->type == AST_CCast_t) {
                exp_left = exp_left->get._CCast.exp;
            }
            exp_left = exp_left->get._CBinary.exp_left;
            if (exp_left->type == AST_CCast_t) {
                exp_left = exp_left->get._CCast.exp;
            }

            {
                vector_t(unique_ptr_t(TacInstruction)) noeval_instrs = vec_new();
                vector_t(unique_ptr_t(TacInstruction))* p_instrs = ctx->p_instrs;
                ctx->p_instrs = &noeval_instrs;
                res = repr_res_instr(ctx, exp_left);
                ctx->p_instrs = p_instrs;
                for (size_t i = 0; i < vec_size(noeval_instrs); ++i) {
                    free_TacInstruction(&noeval_instrs[i]);
                }
                vec_delete(noeval_instrs);
            }

            ctx->identifiers->label_count = label_count_2;
            ctx->identifiers->var_count = var_count_2;
            ctx->identifiers->struct_count = struct_count_2;

            if (node->unop.type == AST_CPostfix_t) {
                shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
                switch (res->type) {
                    case AST_TacPlainOperand_t:
                        plain_op_postfix_exp_instr(ctx, &res->get._TacPlainOperand, &dst);
                        break;
                    case AST_TacDereferencedPointer_t:
                        deref_ptr_postfix_exp_instr(ctx, &res->get._TacDereferencedPointer, &dst);
                        break;
                    case AST_TacSubObject_t:
                        sub_obj_postfix_exp_instr(ctx, &res->get._TacSubObject, &dst);
                        break;
                    default:
                        THROW_ABORT;
                }
                res_postfix = make_TacPlainOperand(&dst);
            }
        }
    }
    switch (res->type) {
        case AST_TacPlainOperand_t:
            plain_op_assign_res_instr(ctx, &res->get._TacPlainOperand, &src);
            break;
        case AST_TacDereferencedPointer_t:
            deref_ptr_assign_res_instr(ctx, &res->get._TacDereferencedPointer, &src, &res);
            break;
        case AST_TacSubObject_t:
            sub_obj_assign_res_instr(ctx, &res->get._TacSubObject, &src, &res);
            break;
        default:
            THROW_ABORT;
    }
    if (node->unop.type == AST_CPostfix_t) {
        free_TacExpResult(&res);
        return res_postfix;
    }
    else {
        return res;
    }
}

static unique_ptr_t(TacExpResult) conditional_complete_res_instr(Ctx ctx, CConditional* node) {
    TIdentifier target_else = repr_label_identifier(ctx->identifiers, LBL_Lternary_else);
    TIdentifier target_false = repr_label_identifier(ctx->identifiers, LBL_Lternary_false);
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node->_base);
    {
        shared_ptr_t(TacValue) condition = repr_exp_instr(ctx, node->condition);
        push_instr(ctx, make_TacJumpIfZero(target_else, &condition));
    }
    {
        shared_ptr_t(TacValue) src_middle = repr_exp_instr(ctx, node->exp_middle);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacCopy(&src_middle, &dst_cp));
    }
    push_instr(ctx, make_TacJump(target_false));
    push_instr(ctx, make_TacLabel(target_else));
    {
        shared_ptr_t(TacValue) src_right = repr_exp_instr(ctx, node->exp_right);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacCopy(&src_right, &dst_cp));
    }
    push_instr(ctx, make_TacLabel(target_false));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) conditional_void_res_instr(Ctx ctx, CConditional* node) {
    TIdentifier target_else = repr_label_identifier(ctx->identifiers, LBL_Lternary_else);
    TIdentifier target_false = repr_label_identifier(ctx->identifiers, LBL_Lternary_false);
    shared_ptr_t(TacValue) dst = sptr_new();
    {
        shared_ptr_t(TacValue) condition = repr_exp_instr(ctx, node->condition);
        push_instr(ctx, make_TacJumpIfZero(target_else, &condition));
    }
    dst = repr_exp_instr(ctx, node->exp_middle);
    free_TacValue(&dst);
    push_instr(ctx, make_TacJump(target_false));
    push_instr(ctx, make_TacLabel(target_else));
    dst = repr_exp_instr(ctx, node->exp_right);
    free_TacValue(&dst);
    push_instr(ctx, make_TacLabel(target_false));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) conditional_res_instr(Ctx ctx, CConditional* node) {
    if (node->exp_middle->exp_type->type == AST_Void_t) {
        return conditional_void_res_instr(ctx, node);
    }
    else {
        return conditional_complete_res_instr(ctx, node);
    }
}

static unique_ptr_t(TacExpResult) call_res_instr(Ctx ctx, CFunctionCall* node) {
    TIdentifier name = node->name;
    vector_t(shared_ptr_t(TacValue)) args = vec_new();
    vec_reserve(args, vec_size(node->args));
    for (size_t i = 0; i < vec_size(node->args); ++i) {
        shared_ptr_t(TacValue) arg = repr_exp_instr(ctx, node->args[i]);
        vec_move_back(args, arg);
    }
    shared_ptr_t(TacValue) dst = sptr_new();
    if (node->_base->exp_type->type != AST_Void_t) {
        dst = plain_inner_value(ctx, node->_base);
    }
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    push_instr(ctx, make_TacFunCall(name, &args, &dst_cp));
    return make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) deref_res_instr(Ctx ctx, CDereference* node) {
    shared_ptr_t(TacValue) val = repr_exp_instr(ctx, node->exp);
    return make_TacDereferencedPointer(&val);
}

static void plain_op_addrof_res_instr(Ctx ctx, TacPlainOperand* res, CAddrOf* node) {
    shared_ptr_t(TacValue) src = sptr_new();
    sptr_move(TacValue, res->val, src);
    shared_ptr_t(TacValue) dst = ptr_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    push_instr(ctx, make_TacGetAddress(&src, &dst_cp));
    sptr_move(TacValue, dst, res->val);
}

static void deref_ptr_addrof_res_instr(TacDereferencedPointer* res, unique_ptr_t(TacExpResult) * exp_res) {
    shared_ptr_t(TacValue) val = sptr_new();
    sptr_move(TacValue, res->val, val);
    free_TacExpResult(exp_res);
    *exp_res = make_TacPlainOperand(&val);
}

static void sub_obj_addrof_res_instr(Ctx ctx, TacSubObject* res, CAddrOf* node, unique_ptr_t(TacExpResult) * exp_res) {
    shared_ptr_t(TacValue) dst = ptr_inner_value(ctx, node->_base);
    {
        TIdentifier name = res->base_name;
        shared_ptr_t(TacValue) src = make_TacVariable(name);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacGetAddress(&src, &dst_cp));
    }
    if (res->offset > 0l) {
        shared_ptr_t(TacValue) src_ptr = sptr_new();
        sptr_copy(TacValue, dst, src_ptr);
        shared_ptr_t(TacValue) idx = sptr_new();
        {
            TLong offset = res->offset;
            shared_ptr_t(CConst) constant = make_CConstLong(offset);
            idx = make_TacConstant(&constant);
        }
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacAddPtr(1l, &src_ptr, &idx, &dst_cp));
    }
    free_TacExpResult(exp_res);
    *exp_res = make_TacPlainOperand(&dst);
}

static unique_ptr_t(TacExpResult) addrof_res_instr(Ctx ctx, CAddrOf* node) {
    unique_ptr_t(TacExpResult) res = repr_res_instr(ctx, node->exp);
    switch (res->type) {
        case AST_TacPlainOperand_t:
            plain_op_addrof_res_instr(ctx, &res->get._TacPlainOperand, node);
            break;
        case AST_TacDereferencedPointer_t:
            deref_ptr_addrof_res_instr(&res->get._TacDereferencedPointer, &res);
            break;
        case AST_TacSubObject_t:
            sub_obj_addrof_res_instr(ctx, &res->get._TacSubObject, node, &res);
            break;
        default:
            THROW_ABORT;
    }
    return res;
}

static unique_ptr_t(TacExpResult) subscript_res_instr(Ctx ctx, CSubscript* node) {
    TLong scale;
    shared_ptr_t(TacValue) src_ptr = sptr_new();
    shared_ptr_t(TacValue) idx = sptr_new();
    if (node->primary_exp->exp_type->type == AST_Pointer_t) {
        scale = get_type_scale(ctx, node->primary_exp->exp_type->get._Pointer.ref_type);
        src_ptr = repr_exp_instr(ctx, node->primary_exp);
        idx = repr_exp_instr(ctx, node->subscript_exp);
    }
    else {
        scale = get_type_scale(ctx, node->subscript_exp->exp_type->get._Pointer.ref_type);
        src_ptr = repr_exp_instr(ctx, node->subscript_exp);
        idx = repr_exp_instr(ctx, node->primary_exp);
    }
    shared_ptr_t(TacValue) dst = ptr_inner_value(ctx, node->_base);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    push_instr(ctx, make_TacAddPtr(scale, &src_ptr, &idx, &dst_cp));
    return make_TacDereferencedPointer(&dst);
}

static unique_ptr_t(TacExpResult) sizeof_res_instr(Ctx ctx, CSizeOf* node) {
    shared_ptr_t(CConst) constant = sptr_new();
    {
        TULong value = (TULong)get_type_scale(ctx, node->exp->exp_type);
        constant = make_CConstULong(value);
    }
    shared_ptr_t(TacValue) val = make_TacConstant(&constant);
    return make_TacPlainOperand(&val);
}

static unique_ptr_t(TacExpResult) sizeoft_res_instr(Ctx ctx, CSizeOfT* node) {
    shared_ptr_t(CConst) constant = sptr_new();
    {
        TULong value = (TULong)get_type_scale(ctx, node->target_type);
        constant = make_CConstULong(value);
    }
    shared_ptr_t(TacValue) val = make_TacConstant(&constant);
    return make_TacPlainOperand(&val);
}

static void plain_op_dot_res_instr(TacPlainOperand* res, TLong member_offset, unique_ptr_t(TacExpResult) * exp_res) {
    THROW_ABORT_IF(res->val->type != AST_TacVariable_t);
    TIdentifier base_name = res->val->get._TacVariable.name;
    TLong offset = member_offset;
    free_TacExpResult(exp_res);
    *exp_res = make_TacSubObject(base_name, offset);
}

static void deref_ptr_dot_res_instr(Ctx ctx, TacDereferencedPointer* res, CDot* node, TLong member_offset) {
    if (member_offset > 0l) {
        shared_ptr_t(TacValue) src_ptr = sptr_new();
        sptr_move(TacValue, res->val, src_ptr);
        shared_ptr_t(TacValue) idx = sptr_new();
        {
            TLong offset = member_offset;
            shared_ptr_t(CConst) constant = make_CConstLong(offset);
            idx = make_TacConstant(&constant);
        }
        shared_ptr_t(TacValue) dst = ptr_inner_value(ctx, node->_base);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacAddPtr(1l, &src_ptr, &idx, &dst_cp));
        sptr_move(TacValue, dst, res->val);
    }
}

static void sub_obj_dot_res_instr(TacSubObject* res, TLong member_offset) { res->offset += member_offset; }

static unique_ptr_t(TacExpResult) dot_res_instr(Ctx ctx, CDot* node) {
    THROW_ABORT_IF(node->structure->exp_type->type != AST_Structure_t);
    Structure* struct_type = &node->structure->exp_type->get._Structure;
    StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    TLong member_offset = map_get(struct_typedef->members, node->member)->offset;
    unique_ptr_t(TacExpResult) res = repr_res_instr(ctx, node->structure);
    switch (res->type) {
        case AST_TacPlainOperand_t:
            plain_op_dot_res_instr(&res->get._TacPlainOperand, member_offset, &res);
            break;
        case AST_TacDereferencedPointer_t:
            deref_ptr_dot_res_instr(ctx, &res->get._TacDereferencedPointer, node, member_offset);
            break;
        case AST_TacSubObject_t:
            sub_obj_dot_res_instr(&res->get._TacSubObject, member_offset);
            break;
        default:
            THROW_ABORT;
    }
    return res;
}

static unique_ptr_t(TacExpResult) arrow_res_instr(Ctx ctx, CArrow* node) {
    THROW_ABORT_IF(node->pointer->exp_type->type != AST_Pointer_t);
    Pointer* ptr_type = &node->pointer->exp_type->get._Pointer;
    THROW_ABORT_IF(ptr_type->ref_type->type != AST_Structure_t);
    Structure* struct_type = &ptr_type->ref_type->get._Structure;
    StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    TLong member_offset = map_get(struct_typedef->members, node->member)->offset;
    shared_ptr_t(TacValue) val = repr_exp_instr(ctx, node->pointer);
    if (member_offset > 0l) {
        shared_ptr_t(TacValue) idx = sptr_new();
        {
            TLong offset = member_offset;
            shared_ptr_t(CConst) constant = make_CConstLong(offset);
            idx = make_TacConstant(&constant);
        }
        shared_ptr_t(TacValue) dst = ptr_inner_value(ctx, node->_base);
        shared_ptr_t(TacValue) dst_cp = sptr_new();
        sptr_copy(TacValue, dst, dst_cp);
        push_instr(ctx, make_TacAddPtr(1l, &val, &idx, &dst_cp));
        sptr_move(TacValue, dst, val);
    }
    return make_TacDereferencedPointer(&val);
}

static unique_ptr_t(TacExpResult) repr_res_instr(Ctx ctx, CExp* node) {
    switch (node->type) {
        case AST_CConstant_t:
            return const_res_instr(&node->get._CConstant);
        case AST_CString_t:
            return string_res_instr(ctx, &node->get._CString);
        case AST_CVar_t:
            return var_res_instr(&node->get._CVar);
        case AST_CCast_t:
            return cast_res_instr(ctx, &node->get._CCast);
        case AST_CUnary_t:
            return unary_res_instr(ctx, &node->get._CUnary);
        case AST_CBinary_t:
            return binary_res_instr(ctx, &node->get._CBinary);
        case AST_CAssignment_t:
            return assign_res_instr(ctx, &node->get._CAssignment);
        case AST_CConditional_t:
            return conditional_res_instr(ctx, &node->get._CConditional);
        case AST_CFunctionCall_t:
            return call_res_instr(ctx, &node->get._CFunctionCall);
        case AST_CDereference_t:
            return deref_res_instr(ctx, &node->get._CDereference);
        case AST_CAddrOf_t:
            return addrof_res_instr(ctx, &node->get._CAddrOf);
        case AST_CSubscript_t:
            return subscript_res_instr(ctx, &node->get._CSubscript);
        case AST_CSizeOf_t:
            return sizeof_res_instr(ctx, &node->get._CSizeOf);
        case AST_CSizeOfT_t:
            return sizeoft_res_instr(ctx, &node->get._CSizeOfT);
        case AST_CDot_t:
            return dot_res_instr(ctx, &node->get._CDot);
        case AST_CArrow_t:
            return arrow_res_instr(ctx, &node->get._CArrow);
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) plain_op_exp_instr(TacPlainOperand* res) {
    shared_ptr_t(TacValue) dst = sptr_new();
    sptr_move(TacValue, res->val, dst);
    return dst;
}

static shared_ptr_t(TacValue) deref_ptr_exp_instr(Ctx ctx, TacDereferencedPointer* res, CExp* node) {
    shared_ptr_t(TacValue) src = sptr_new();
    sptr_move(TacValue, res->val, src);
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    push_instr(ctx, make_TacLoad(&src, &dst_cp));
    return dst;
}

static shared_ptr_t(TacValue) sub_obj_exp_instr(Ctx ctx, TacSubObject* res, CExp* node) {
    TIdentifier src_name = res->base_name;
    TLong offset = res->offset;
    shared_ptr_t(TacValue) dst = plain_inner_value(ctx, node);
    shared_ptr_t(TacValue) dst_cp = sptr_new();
    sptr_copy(TacValue, dst, dst_cp);
    push_instr(ctx, make_TacCopyFromOffset(src_name, offset, &dst_cp));
    return dst;
}

// exp_result = PlainOperand(val) | DereferencedPointer(val) | SubObject(val)
static shared_ptr_t(TacValue) repr_exp_instr(Ctx ctx, CExp* node) {
    shared_ptr_t(TacValue) val = sptr_new();
    unique_ptr_t(TacExpResult) res = repr_res_instr(ctx, node);
    switch (res->type) {
        case AST_TacPlainOperand_t: {
            val = plain_op_exp_instr(&res->get._TacPlainOperand);
            break;
        }
        case AST_TacDereferencedPointer_t: {
            val = deref_ptr_exp_instr(ctx, &res->get._TacDereferencedPointer, node);
            break;
        }
        case AST_TacSubObject_t: {
            val = sub_obj_exp_instr(ctx, &res->get._TacSubObject, node);
            break;
        }
        default:
            THROW_ABORT;
    }
    free_TacExpResult(&res);
    return val;
}

static void repr_block(Ctx ctx, CBlock* node);

static void statement_instr(Ctx ctx, CStatement* node);
static void var_decl_instr(Ctx ctx, CVariableDeclaration* node);

static void ret_statement_instr(Ctx ctx, CReturn* node) {
    shared_ptr_t(TacValue) val = sptr_new();
    if (node->exp) {
        val = repr_exp_instr(ctx, node->exp);
    }
    push_instr(ctx, make_TacReturn(&val));
}

static void exp_statement_instr(Ctx ctx, CExpression* node) {
    unique_ptr_t(TacExpResult) res = repr_res_instr(ctx, node->exp);
    free_TacExpResult(&res);
}

static void if_only_statement_instr(Ctx ctx, CIf* node) {
    TIdentifier target_false = repr_label_identifier(ctx->identifiers, LBL_Lif_false);
    {
        shared_ptr_t(TacValue) condition = repr_exp_instr(ctx, node->condition);
        push_instr(ctx, make_TacJumpIfZero(target_false, &condition));
    }
    statement_instr(ctx, node->then);
    push_instr(ctx, make_TacLabel(target_false));
}

static void if_else_statement_instr(Ctx ctx, CIf* node) {
    TIdentifier target_else = repr_label_identifier(ctx->identifiers, LBL_Lif_else);
    TIdentifier target_false = repr_label_identifier(ctx->identifiers, LBL_Lif_false);
    {
        shared_ptr_t(TacValue) condition = repr_exp_instr(ctx, node->condition);
        push_instr(ctx, make_TacJumpIfZero(target_else, &condition));
    }
    statement_instr(ctx, node->then);
    push_instr(ctx, make_TacJump(target_false));
    push_instr(ctx, make_TacLabel(target_else));
    statement_instr(ctx, node->else_fi);
    push_instr(ctx, make_TacLabel(target_false));
}

static void if_statement_instr(Ctx ctx, CIf* node) {
    if (node->else_fi) {
        if_else_statement_instr(ctx, node);
    }
    else {
        if_only_statement_instr(ctx, node);
    }
}

static void goto_statement_instr(Ctx ctx, CGoto* node) {
    TIdentifier target_label = node->target;
    push_instr(ctx, make_TacJump(target_label));
}

static void label_statement_instr(Ctx ctx, CLabel* node) {
    TIdentifier target_label = node->target;
    push_instr(ctx, make_TacLabel(target_label));
    statement_instr(ctx, node->jump_to);
}

static void statement_compound_instr(Ctx ctx, CCompound* node) { repr_block(ctx, node->block); }

static void while_statement_instr(Ctx ctx, CWhile* node) {
    TIdentifier target_break = repr_loop_identifier(ctx->identifiers, LBL_Lbreak, node->target);
    TIdentifier target_continue = repr_loop_identifier(ctx->identifiers, LBL_Lcontinue, node->target);
    push_instr(ctx, make_TacLabel(target_continue));
    {
        shared_ptr_t(TacValue) condition = repr_exp_instr(ctx, node->condition);
        push_instr(ctx, make_TacJumpIfZero(target_break, &condition));
    }
    statement_instr(ctx, node->body);
    push_instr(ctx, make_TacJump(target_continue));
    push_instr(ctx, make_TacLabel(target_break));
}

static void do_while_statement_instr(Ctx ctx, CDoWhile* node) {
    TIdentifier target_do_while_start = repr_label_identifier(ctx->identifiers, LBL_Ldo_while_start);
    TIdentifier target_break = repr_loop_identifier(ctx->identifiers, LBL_Lbreak, node->target);
    TIdentifier target_continue = repr_loop_identifier(ctx->identifiers, LBL_Lcontinue, node->target);
    push_instr(ctx, make_TacLabel(target_do_while_start));
    statement_instr(ctx, node->body);
    push_instr(ctx, make_TacLabel(target_continue));
    {
        shared_ptr_t(TacValue) condition = repr_exp_instr(ctx, node->condition);
        push_instr(ctx, make_TacJumpIfNotZero(target_do_while_start, &condition));
    }
    push_instr(ctx, make_TacLabel(target_break));
}

static void for_init_decl_instr(Ctx ctx, CInitDecl* node) { var_decl_instr(ctx, node->init); }

static void for_init_exp_instr(Ctx ctx, CInitExp* node) {
    if (node->init) {
        unique_ptr_t(TacExpResult) res = repr_res_instr(ctx, node->init);
        free_TacExpResult(&res);
    }
}

static void for_init_statement_instr(Ctx ctx, CForInit* node) {
    switch (node->type) {
        case AST_CInitDecl_t:
            for_init_decl_instr(ctx, &node->get._CInitDecl);
            break;
        case AST_CInitExp_t:
            for_init_exp_instr(ctx, &node->get._CInitExp);
            break;
        default:
            THROW_ABORT;
    }
}

static void for_statement_instr(Ctx ctx, CFor* node) {
    TIdentifier target_for_start = repr_label_identifier(ctx->identifiers, LBL_Lfor_start);
    TIdentifier target_break = repr_loop_identifier(ctx->identifiers, LBL_Lbreak, node->target);
    TIdentifier target_continue = repr_loop_identifier(ctx->identifiers, LBL_Lcontinue, node->target);
    for_init_statement_instr(ctx, node->init);
    push_instr(ctx, make_TacLabel(target_for_start));
    if (node->condition) {
        shared_ptr_t(TacValue) condition = repr_exp_instr(ctx, node->condition);
        push_instr(ctx, make_TacJumpIfZero(target_break, &condition));
    }
    statement_instr(ctx, node->body);
    push_instr(ctx, make_TacLabel(target_continue));
    if (node->post) {
        unique_ptr_t(TacExpResult) res = repr_res_instr(ctx, node->post);
        free_TacExpResult(&res);
    }
    push_instr(ctx, make_TacJump(target_for_start));
    push_instr(ctx, make_TacLabel(target_break));
}

static void switch_statement_instr(Ctx ctx, CSwitch* node) {
    TIdentifier target_break = repr_loop_identifier(ctx->identifiers, LBL_Lbreak, node->target);
    {
        shared_ptr_t(TacValue) match = repr_exp_instr(ctx, node->match);
        for (size_t i = 0; i < vec_size(node->cases); ++i) {
            TIdentifier target_case = repr_case_identifier(ctx->identifiers, node->target, true, i);
            shared_ptr_t(TacValue) case_match = sptr_new();
            {
                shared_ptr_t(TacValue) match_cp = sptr_new();
                sptr_copy(TacValue, match, match_cp);
                shared_ptr_t(TacValue) esac = repr_exp_instr(ctx, node->cases[i]);
                case_match = plain_inner_value(ctx, node->cases[i]);
                shared_ptr_t(TacValue) case_match_cp = sptr_new();
                sptr_copy(TacValue, case_match, case_match_cp);
                unique_ptr_t(TacBinaryOp) binop = make_TacEqual();
                push_instr(ctx, make_TacBinary(&binop, &match_cp, &esac, &case_match_cp));
            }
            push_instr(ctx, make_TacJumpIfNotZero(target_case, &case_match));
        }
        free_TacValue(&match);
    }
    if (node->is_default) {
        TIdentifier target_default = repr_loop_identifier(ctx->identifiers, LBL_Ldefault, node->target);
        push_instr(ctx, make_TacJump(target_default));
        statement_instr(ctx, node->body);
    }
    else if (!vec_empty(node->cases)) {
        push_instr(ctx, make_TacJump(target_break));
        statement_instr(ctx, node->body);
    }
    push_instr(ctx, make_TacLabel(target_break));
}

static void case_statement_instr(Ctx ctx, CCase* node) {
    TIdentifier target_case = repr_loop_identifier(ctx->identifiers, LBL_Lcase, node->target);
    push_instr(ctx, make_TacLabel(target_case));
    statement_instr(ctx, node->jump_to);
}

static void default_statement_instr(Ctx ctx, CDefault* node) {
    TIdentifier target_default = repr_loop_identifier(ctx->identifiers, LBL_Ldefault, node->target);
    push_instr(ctx, make_TacLabel(target_default));
    statement_instr(ctx, node->jump_to);
}

static void break_statement_instr(Ctx ctx, CBreak* node) {
    TIdentifier target_break = repr_loop_identifier(ctx->identifiers, LBL_Lbreak, node->target);
    push_instr(ctx, make_TacJump(target_break));
}

static void continue_statement_instr(Ctx ctx, CContinue* node) {
    TIdentifier target_continue = repr_loop_identifier(ctx->identifiers, LBL_Lcontinue, node->target);
    push_instr(ctx, make_TacJump(target_continue));
}

static void statement_instr(Ctx ctx, CStatement* node) {
    switch (node->type) {
        case AST_CReturn_t:
            ret_statement_instr(ctx, &node->get._CReturn);
            break;
        case AST_CExpression_t:
            exp_statement_instr(ctx, &node->get._CExpression);
            break;
        case AST_CIf_t:
            if_statement_instr(ctx, &node->get._CIf);
            break;
        case AST_CGoto_t:
            goto_statement_instr(ctx, &node->get._CGoto);
            break;
        case AST_CLabel_t:
            label_statement_instr(ctx, &node->get._CLabel);
            break;
        case AST_CCompound_t:
            statement_compound_instr(ctx, &node->get._CCompound);
            break;
        case AST_CWhile_t:
            while_statement_instr(ctx, &node->get._CWhile);
            break;
        case AST_CDoWhile_t:
            do_while_statement_instr(ctx, &node->get._CDoWhile);
            break;
        case AST_CFor_t:
            for_statement_instr(ctx, &node->get._CFor);
            break;
        case AST_CSwitch_t:
            switch_statement_instr(ctx, &node->get._CSwitch);
            break;
        case AST_CCase_t:
            case_statement_instr(ctx, &node->get._CCase);
            break;
        case AST_CDefault_t:
            default_statement_instr(ctx, &node->get._CDefault);
            break;
        case AST_CBreak_t:
            break_statement_instr(ctx, &node->get._CBreak);
            break;
        case AST_CContinue_t:
            continue_statement_instr(ctx, &node->get._CContinue);
            break;
        case AST_CNull_t:
            break;
        default:
            THROW_ABORT;
    }
}

static void compound_init_instr(Ctx ctx, CInitializer* node, Type* init_type, TIdentifier symbol, TLong* size);

static void string_single_init_instr(Ctx ctx, CString* node, Array* arr_type, TIdentifier symbol, TLong size) {
    size_t byte_at = 0;

    size_t bytes_size = (size_t)arr_type->size;
    size_t bytes_copy =
        arr_type->size > (TLong)vec_size(node->literal->value) ? vec_size(node->literal->value) : bytes_size;

    while (byte_at < bytes_copy) {
        TIdentifier dst_name = symbol;
        TLong offset = size + ((TLong)byte_at);
        shared_ptr_t(TacValue) src = sptr_new();
        {
            shared_ptr_t(CConst) constant = sptr_new();
            {
                size_t bytes_left = bytes_size - byte_at;
                if (bytes_left < 4) {
                    TChar value = string_bytes_to_int8(node->literal->value, byte_at);
                    constant = make_CConstChar(value);
                    byte_at++;
                }
                else if (bytes_left < 8) {
                    TInt value = string_bytes_to_int32(node->literal->value, byte_at);
                    constant = make_CConstInt(value);
                    byte_at += 4;
                }
                else {
                    TLong value = string_bytes_to_int64(node->literal->value, byte_at);
                    constant = make_CConstLong(value);
                    byte_at += 8;
                }
            }
            src = make_TacConstant(&constant);
        }
        push_instr(ctx, make_TacCopyToOffset(dst_name, offset, &src));
    }

    while (byte_at < bytes_size) {
        TIdentifier dst_name = symbol;
        TLong offset = size + ((TLong)byte_at);
        shared_ptr_t(TacValue) src = sptr_new();
        {
            shared_ptr_t(CConst) constant = sptr_new();
            {
                size_t bytes_left = bytes_size - byte_at;
                if (bytes_left < 4) {
                    constant = make_CConstChar(0);
                    byte_at++;
                }
                else if (bytes_left < 8) {
                    constant = make_CConstInt(0);
                    byte_at += 4;
                }
                else {
                    constant = make_CConstLong(0l);
                    byte_at += 8;
                }
            }
            src = make_TacConstant(&constant);
        }
        push_instr(ctx, make_TacCopyToOffset(dst_name, offset, &src));
    }
}

static void single_init_instr(Ctx ctx, CSingleInit* node, Type* init_type, TIdentifier symbol) {
    if (node->exp->type == AST_CString_t && init_type->type == AST_Array_t) {
        string_single_init_instr(ctx, &node->exp->get._CString, &init_type->get._Array, symbol, 0l);
    }
    else {
        shared_ptr_t(TacValue) src = repr_exp_instr(ctx, node->exp);
        shared_ptr_t(TacValue) dst = sptr_new();
        {
            TIdentifier name = symbol;
            unique_ptr_t(CExp) exp = make_CVar(name, 0);
            dst = repr_value(exp);
            free_CExp(&exp);
        }
        push_instr(ctx, make_TacCopy(&src, &dst));
    }
}

static void scalar_compound_init_instr(Ctx ctx, CSingleInit* node, Type* init_type, TIdentifier symbol, TLong* size) {
    if (node->exp->type == AST_CString_t && init_type->type == AST_Array_t) {
        string_single_init_instr(ctx, &node->exp->get._CString, &init_type->get._Array, symbol, *size);
    }
    else {
        TIdentifier dst_name = symbol;
        TLong offset = *size;
        shared_ptr_t(TacValue) src = repr_exp_instr(ctx, node->exp);
        push_instr(ctx, make_TacCopyToOffset(dst_name, offset, &src));
    }
}

static void arr_compound_init_instr(Ctx ctx, CCompoundInit* node, Array* arr_type, TIdentifier symbol, TLong* size) {
    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        compound_init_instr(ctx, node->initializers[i], arr_type->elem_type, symbol, size);
        if (node->initializers[i]->type == AST_CSingleInit_t) {
            *size += get_type_scale(ctx, arr_type->elem_type);
        }
    }
}

static void struct_compound_init_instr(
    Ctx ctx, CCompoundInit* node, Structure* struct_type, TIdentifier symbol, TLong* size) {
    for (size_t i = vec_size(node->initializers); i-- > 0;) {
        StructMember* member = get_struct_typedef_member(ctx->frontend, struct_type->tag, i);
        TLong offset = *size + member->offset;
        compound_init_instr(ctx, node->initializers[i], member->member_type, symbol, &offset);
    }
    *size += get_struct_scale(ctx, struct_type);
}

static void aggr_compound_init_instr(Ctx ctx, CCompoundInit* node, Type* init_type, TIdentifier symbol, TLong* size) {
    switch (init_type->type) {
        case AST_Array_t:
            arr_compound_init_instr(ctx, node, &init_type->get._Array, symbol, size);
            break;
        case AST_Structure_t:
            struct_compound_init_instr(ctx, node, &init_type->get._Structure, symbol, size);
            break;
        default:
            THROW_ABORT;
    }
}

static void compound_init_instr(Ctx ctx, CInitializer* node, Type* init_type, TIdentifier symbol, TLong* size) {
    switch (node->type) {
        case AST_CSingleInit_t:
            scalar_compound_init_instr(ctx, &node->get._CSingleInit, init_type, symbol, size);
            break;
        case AST_CCompoundInit_t:
            aggr_compound_init_instr(ctx, &node->get._CCompoundInit, init_type, symbol, size);
            break;
        default:
            THROW_ABORT;
    }
}

static void var_decl_instr(Ctx ctx, CVariableDeclaration* node) {
    Type* init_type = map_get(ctx->frontend->symbol_table, node->name)->type_t;
    switch (node->init->type) {
        case AST_CSingleInit_t:
            single_init_instr(ctx, &node->init->get._CSingleInit, init_type, node->name);
            break;
        case AST_CCompoundInit_t: {
            TLong size = 0l;
            aggr_compound_init_instr(ctx, &node->init->get._CCompoundInit, init_type, node->name, &size);
            break;
        }
        default:
            THROW_ABORT;
    }
}

static void var_declaration_instr(Ctx ctx, CVarDecl* node) {
    if (node->var_decl->init
        && map_get(ctx->frontend->symbol_table, node->var_decl->name)->attrs->type != AST_StaticAttr_t) {
        var_decl_instr(ctx, node->var_decl);
    }
}

static void declaration_instr(Ctx ctx, CDeclaration* node) {
    switch (node->type) {
        case AST_CFunDecl_t:
        case AST_CStructDecl_t:
            break;
        case AST_CVarDecl_t:
            var_declaration_instr(ctx, &node->get._CVarDecl);
            break;
        default:
            THROW_ABORT;
    }
}

// instruction = Return(val?) | SignExtend(val, val) | Truncate(val, val) | ZeroExtend(val, val)
//             | TacDoubleToInt(val, val) | TacDoubleToUInt(val, val) | TacIntToDouble(val, val)
//             | TacUIntToDouble(val, val) | FunCall(identifier, val*, val?) | Unary(unary_operator, val, val)
//             | Binary(binary_operator, val, val, val) | Copy(val, val) | GetAddress(val, val) | Load(val, val)
//             | Store(val, val) | AddPtr(int, val, val, val) | CopyToOffset(identifier, int, val)
//             | CopyFromOffset(identifier, int, val) | Jump(identifier) | JumpIfZero(val, identifier)
//             | JumpIfNotZero(val, identifier) | Label(identifier)
static void repr_instr_list(Ctx ctx, vector_t(unique_ptr_t(CBlockItem)) node_list) {
    for (size_t i = 0; i < vec_size(node_list); ++i) {
        switch (node_list[i]->type) {
            case AST_CS_t:
                statement_instr(ctx, node_list[i]->get._CS.statement);
                break;
            case AST_CD_t:
                declaration_instr(ctx, node_list[i]->get._CD.declaration);
                break;
            default:
                THROW_ABORT;
        }
    }
}

static void repr_block(Ctx ctx, CBlock* node) {
    if (node->type == AST_CB_t) {
        repr_instr_list(ctx, node->get._CB.block_items);
    }
    else {
        THROW_ABORT;
    }
}

static unique_ptr_t(TacTopLevel) repr_fun_toplvl(Ctx ctx, CFunctionDeclaration* node) {
    TIdentifier name = node->name;
    bool is_glob = map_get(ctx->frontend->symbol_table, node->name)->attrs->get._FunAttr.is_glob;

    vector_t(TIdentifier) params = vec_new();
    vec_resize(params, vec_size(node->params));
    memcpy(params, node->params, sizeof(TIdentifier) * vec_size(node->params));

    vector_t(unique_ptr_t(TacInstruction)) body = vec_new();
    {
        ctx->p_instrs = &body;
        repr_block(ctx, node->body);
        {
            shared_ptr_t(CConst) constant = make_CConstInt(0);
            shared_ptr_t(TacValue) val = make_TacConstant(&constant);
            push_instr(ctx, make_TacReturn(&val));
        }
        ctx->p_instrs = NULL;
    }

    return make_TacFunction(name, is_glob, &params, &body);
}

static void push_toplvl(Ctx ctx, unique_ptr_t(TacTopLevel) top_level) { vec_move_back(*ctx->p_toplvls, top_level); }

static void fun_decl_toplvl(Ctx ctx, CFunDecl* node) {
    if (node->fun_decl->body) {
        push_toplvl(ctx, repr_fun_toplvl(ctx, node->fun_decl));
    }
}

// (function) top_level = Function(identifier, bool, identifier*, instruction*)
static void declaration_toplvl(Ctx ctx, CDeclaration* node) {
    switch (node->type) {
        case AST_CFunDecl_t:
            fun_decl_toplvl(ctx, &node->get._CFunDecl);
            break;
        case AST_CVarDecl_t:
        case AST_CStructDecl_t:
            break;
        default:
            THROW_ABORT;
    }
}

static vector_t(shared_ptr_t(StaticInit)) tentative_static_toplvl(Ctx ctx, Type* static_init_type) {
    vector_t(shared_ptr_t(StaticInit)) static_inits = vec_new();
    {
        TLong byte = get_type_scale(ctx, static_init_type);
        shared_ptr_t(StaticInit) static_init = make_ZeroInit(byte);
        vec_move_back(static_inits, static_init);
    }
    return static_inits;
}

static vector_t(shared_ptr_t(StaticInit)) initial_static_toplvl(Initial* node) {
    vector_t(shared_ptr_t(StaticInit)) static_inits = vec_new();
    vec_reserve(static_inits, vec_size(node->static_inits));
    for (size_t i = 0; i < vec_size(node->static_inits); ++i) {
        shared_ptr_t(StaticInit) static_init = sptr_new();
        sptr_copy(StaticInit, node->static_inits[i], static_init);
        vec_move_back(static_inits, static_init);
    }
    return static_inits;
}

static void repr_static_var_toplvl(Ctx ctx, Symbol* node, TIdentifier symbol) {
    StaticAttr* static_attr = &node->attrs->get._StaticAttr;
    if (static_attr->init->type == AST_NoInitializer_t) {
        return;
    }

    TIdentifier name = symbol;
    bool is_glob = static_attr->is_glob;
    shared_ptr_t(Type) static_init_type = sptr_new();
    sptr_copy(Type, node->type_t, static_init_type);
    vector_t(shared_ptr_t(StaticInit)) static_inits = vec_new();
    switch (static_attr->init->type) {
        case AST_Tentative_t:
            static_inits = tentative_static_toplvl(ctx, static_init_type);
            break;
        case AST_Initial_t:
            static_inits = initial_static_toplvl(&static_attr->init->get._Initial);
            break;
        default:
            THROW_ABORT;
    }

    push_toplvl(ctx, make_TacStaticVariable(name, is_glob, &static_init_type, &static_inits));
}

static void push_static_const_toplvl(Ctx ctx, unique_ptr_t(TacTopLevel) static_const_toplvls) {
    vec_move_back(*ctx->p_static_consts, static_const_toplvls);
}

static void repr_static_const_toplvl(Ctx ctx, Symbol* node, TIdentifier symbol) {
    TIdentifier name = symbol;
    shared_ptr_t(Type) static_init_type = sptr_new();
    sptr_copy(Type, node->type_t, static_init_type);
    shared_ptr_t(StaticInit) static_init = sptr_new();
    sptr_copy(StaticInit, node->attrs->get._ConstantAttr.static_init, static_init);
    push_static_const_toplvl(ctx, make_TacStaticConstant(name, &static_init_type, &static_init));
}

// (static variable) top_level = StaticVariable(identifier, bool, type, static_init*)
// (static constant) top_level = StaticConstant(identifier, type, static_init)
static void symbol_toplvl(Ctx ctx, Symbol* node, TIdentifier symbol) {
    switch (node->attrs->type) {
        case AST_StaticAttr_t:
            repr_static_var_toplvl(ctx, node, symbol);
            break;
        case AST_ConstantAttr_t:
            repr_static_const_toplvl(ctx, node, symbol);
            break;
        default:
            break;
    }
}

// AST = Program(top_level*, top_level*, top_level*)
static unique_ptr_t(TacProgram) repr_program(Ctx ctx, CProgram* node) {
    vector_t(unique_ptr_t(TacTopLevel)) fun_toplvls = vec_new();
    {
        ctx->p_toplvls = &fun_toplvls;
        for (size_t i = 0; i < vec_size(node->declarations); ++i) {
            declaration_toplvl(ctx, node->declarations[i]);
        }
        ctx->p_toplvls = NULL;
    }

    vector_t(unique_ptr_t(TacTopLevel)) static_var_toplvls = vec_new();
    vector_t(unique_ptr_t(TacTopLevel)) static_const_toplvls = vec_new();
    {
        ctx->p_toplvls = &static_var_toplvls;
        ctx->p_static_consts = &static_const_toplvls;
        for (size_t i = 0; i < map_size(ctx->frontend->symbol_table); ++i) {
            const pair_t(TIdentifier, UPtrSymbol)* symbol = &ctx->frontend->symbol_table[i];
            symbol_toplvl(ctx, pair_second(*symbol), pair_first(*symbol));
        }
        ctx->p_toplvls = NULL;
        ctx->p_static_consts = NULL;
    }

    return make_TacProgram(&static_const_toplvls, &static_var_toplvls, &fun_toplvls);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unique_ptr_t(TacProgram) represent_three_address_code(
    unique_ptr_t(CProgram) * c_ast, FrontEndContext* frontend, IdentifierContext* identifiers) {
    TacReprContext ctx;
    {
        ctx.frontend = frontend;
        ctx.identifiers = identifiers;
    }
    unique_ptr_t(TacProgram) tac_ast = repr_program(&ctx, *c_ast);

    free_CProgram(c_ast);
    THROW_ABORT_IF(!tac_ast);
    return tac_ast;
}
