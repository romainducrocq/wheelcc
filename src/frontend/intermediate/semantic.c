#include "util/c_std.h"
#include "util/str2t.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/front_ast.h"
#include "ast/front_symt.h"

#include "frontend/parser/errors.h"

#include "frontend/intermediate/idents.h"
#include "frontend/intermediate/semantic.h"

PairKeyValue(TIdentifier, Structure);

typedef struct SemanticContext {
    ErrorsContext* errors;
    FrontEndContext* frontend;
    IdentifierContext* identifiers;
    // Type checking
    hashmap_t(TIdentifier, size_t) extern_scope_map;
    hashmap_t(TIdentifier, TIdentifier) goto_map;
    vector_t(hashmap_t(TIdentifier, TIdentifier)) scoped_identifier_maps;
    vector_t(hashmap_t(TIdentifier, Structure)) scoped_struct_maps;
    hashset_t(TIdentifier) label_set;
    // Loop labeling
    vector_t(TIdentifier) break_loop_labels;
    vector_t(TIdentifier) continue_loop_labels;
    // Identifier resolution
    TIdentifier fun_def_name;
    CSwitch* p_switch_statement;
    hashset_t(TIdentifier) fun_def_set;
    hashset_t(TIdentifier) struct_def_set;
    hashset_t(TIdentifier) union_def_set;
    vector_t(shared_ptr_t(StaticInit)) * p_static_inits;
} SemanticContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

typedef SemanticContext* Ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Type checking

static bool is_same_type(const Type* type_1, const Type* type_2);

static bool is_same_ptr(const Pointer* ptr_type_1, const Pointer* ptr_type_2) {
    return is_same_type(ptr_type_1->ref_type, ptr_type_2->ref_type);
}

static bool is_same_arr(const Array* arr_type_1, const Array* arr_type_2) {
    return arr_type_1->size == arr_type_2->size && is_same_type(arr_type_1->elem_type, arr_type_2->elem_type);
}

static bool is_same_struct(const Structure* struct_type_1, const Structure* struct_type_2) {
    return struct_type_1->tag == struct_type_2->tag;
}

static bool is_same_type(const Type* type_1, const Type* type_2) {
    if (type_1->type == type_2->type) {
        switch (type_1->type) {
            case AST_Pointer_t:
                return is_same_ptr(&type_1->get._Pointer, &type_2->get._Pointer);
            case AST_Array_t:
                return is_same_arr(&type_1->get._Array, &type_2->get._Array);
            case AST_Structure_t:
                return is_same_struct(&type_1->get._Structure, &type_2->get._Structure);
            case AST_FunType_t:
                THROW_ABORT;
            default:
                return true;
        }
    }
    return false;
}

static bool is_same_fun_type(const FunType* fun_type_1, const FunType* fun_type_2) {
    if (vec_size(fun_type_1->param_types) != vec_size(fun_type_2->param_types)
        || !is_same_type(fun_type_1->ret_type, fun_type_2->ret_type)) {
        return false;
    }
    for (size_t i = 0; i < vec_size(fun_type_1->param_types); ++i) {
        if (!is_same_type(fun_type_1->param_types[i], fun_type_2->param_types[i])) {
            return false;
        }
    }
    return true;
}

static bool is_type_signed(const Type* type) {
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

static bool is_type_char(const Type* type) {
    switch (type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_int(const Type* type) {
    switch (type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_Int_t:
        case AST_Long_t:
        case AST_UChar_t:
        case AST_UInt_t:
        case AST_ULong_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_arithmetic(const Type* type) {
    switch (type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_Int_t:
        case AST_Long_t:
        case AST_Double_t:
        case AST_UChar_t:
        case AST_UInt_t:
        case AST_ULong_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_scalar(const Type* type) {
    switch (type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_Int_t:
        case AST_Long_t:
        case AST_Double_t:
        case AST_UChar_t:
        case AST_UInt_t:
        case AST_ULong_t:
        case AST_Pointer_t:
            return true;
        default:
            return false;
    }
}

static bool is_struct_complete(Ctx ctx, const Structure* struct_type) {
    return map_find(ctx->frontend->struct_typedef_table, struct_type->tag) != map_end();
}

static bool is_type_complete(Ctx ctx, const Type* type) {
    switch (type->type) {
        case AST_Void_t:
            return false;
        case AST_Structure_t:
            return is_struct_complete(ctx, &type->get._Structure);
        default:
            return true;
    }
}

static error_t is_valid_type(Ctx ctx, const Type* type);

static error_t is_valid_ptr(Ctx ctx, const Pointer* ptr_type) {
    CATCH_ENTER;
    TRY(is_valid_type(ctx, ptr_type->ref_type));
    FINALLY;
    CATCH_EXIT;
}

static error_t is_valid_arr(Ctx ctx, const Array* arr_type) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_complete(ctx, arr_type->elem_type)) {
        THROW_AT_TOKEN(ctx->errors->linebuf, GET_SEMANTIC_MSG(MSG_incomplete_arr, str_fmt_arr(arr_type, &type_fmt_1),
                                                 str_fmt_type(arr_type->elem_type, &type_fmt_2)));
    }
    TRY(is_valid_type(ctx, arr_type->elem_type));
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t is_valid_type(Ctx ctx, const Type* type) {
    CATCH_ENTER;
    switch (type->type) {
        case AST_Pointer_t:
            TRY(is_valid_ptr(ctx, &type->get._Pointer));
            break;
        case AST_Array_t:
            TRY(is_valid_arr(ctx, &type->get._Array));
            break;
        case AST_FunType_t:
            THROW_ABORT;
        default:
            break;
    }
    FINALLY;
    CATCH_EXIT;
}

static bool is_exp_lvalue(const CExp* node);

static bool is_dot_exp_lvalue(const CDot* node) { return is_exp_lvalue(node->structure); }

static bool is_exp_lvalue(const CExp* node) {
    switch (node->type) {
        case AST_CString_t:
        case AST_CVar_t:
        case AST_CDereference_t:
        case AST_CSubscript_t:
        case AST_CArrow_t:
            return true;
        case AST_CDot_t:
            return is_dot_exp_lvalue(&node->get._CDot);
        default:
            return false;
    }
}

static bool is_const_null_ptr(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstInt_t:
            return node->constant->get._CConstInt.value == 0;
        case AST_CConstLong_t:
            return node->constant->get._CConstLong.value == 0l;
        case AST_CConstUInt_t:
            return node->constant->get._CConstUInt.value == 0u;
        case AST_CConstULong_t:
            return node->constant->get._CConstULong.value == 0ul;
        default:
            return false;
    }
}

static TInt get_scalar_size(const Type* type) {
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

static TLong get_type_scale(Ctx ctx, const Type* type);

static TLong get_arr_scale(Ctx ctx, const Array* arr_type) {
    TLong size = arr_type->size;
    while (arr_type->elem_type->type == AST_Array_t) {
        arr_type = &arr_type->elem_type->get._Array;
        size *= arr_type->size;
    }
    return get_type_scale(ctx, arr_type->elem_type) * size;
}

static TLong get_struct_scale(Ctx ctx, const Structure* struct_type) {
    THROW_ABORT_IF(map_find(ctx->frontend->struct_typedef_table, struct_type->tag) == map_end());
    return map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
}

static TLong get_type_scale(Ctx ctx, const Type* type) {
    switch (type->type) {
        case AST_Array_t:
            return get_arr_scale(ctx, &type->get._Array);
        case AST_Structure_t:
            return get_struct_scale(ctx, &type->get._Structure);
        default:
            return get_scalar_size(type);
    }
}

static TInt get_type_alignment(Ctx ctx, const Type* type);

static TInt get_arr_alignment(Ctx ctx, const Array* arr_type) { return get_type_alignment(ctx, arr_type->elem_type); }

static TInt get_struct_alignment(Ctx ctx, const Structure* struct_type) {
    THROW_ABORT_IF(map_find(ctx->frontend->struct_typedef_table, struct_type->tag) == map_end());
    return map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->alignment;
}

static TInt get_type_alignment(Ctx ctx, const Type* type) {
    switch (type->type) {
        case AST_Array_t:
            return get_arr_alignment(ctx, &type->get._Array);
        case AST_Structure_t:
            return get_struct_alignment(ctx, &type->get._Structure);
        default:
            return get_scalar_size(type);
    }
}

static shared_ptr_t(Type) get_joint_type(CExp* node_1, CExp* node_2) {
    shared_ptr_t(Type) joint_type = sptr_new();
    if (is_type_char(node_1->exp_type)) {
        shared_ptr_t(Type) exp_type = sptr_new();
        sptr_move(Type, node_1->exp_type, exp_type);
        node_1->exp_type = make_Int();
        joint_type = get_joint_type(node_1, node_2);
        sptr_move(Type, exp_type, node_1->exp_type);
    }
    else if (is_type_char(node_2->exp_type)) {
        shared_ptr_t(Type) exp_type = sptr_new();
        sptr_move(Type, node_2->exp_type, exp_type);
        node_2->exp_type = make_Int();
        joint_type = get_joint_type(node_1, node_2);
        sptr_move(Type, exp_type, node_2->exp_type);
    }
    else if (is_same_type(node_1->exp_type, node_2->exp_type)) {
        sptr_copy(Type, node_1->exp_type, joint_type);
    }
    else if (node_1->exp_type->type == AST_Double_t || node_2->exp_type->type == AST_Double_t) {
        joint_type = make_Double();
    }
    else {
        TInt type_size_1 = get_scalar_size(node_1->exp_type);
        TInt type_size_2 = get_scalar_size(node_2->exp_type);
        if (type_size_1 == type_size_2) {
            if (is_type_signed(node_1->exp_type)) {
                sptr_copy(Type, node_2->exp_type, joint_type);
            }
            else {
                sptr_copy(Type, node_1->exp_type, joint_type);
            }
        }
        else if (type_size_1 > type_size_2) {
            sptr_copy(Type, node_1->exp_type, joint_type);
        }
        else {
            sptr_copy(Type, node_2->exp_type, joint_type);
        }
    }
    return joint_type;
}

static error_t get_joint_ptr_type(Ctx ctx, const CExp* node_1, const CExp* node_2, shared_ptr_t(Type) * joint_type) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (is_same_type(node_1->exp_type, node_2->exp_type)) {
        sptr_copy(Type, node_1->exp_type, *joint_type);
    }
    else if (node_1->type == AST_CConstant_t && is_const_null_ptr(&node_1->get._CConstant)) {
        sptr_copy(Type, node_2->exp_type, *joint_type);
    }
    else if ((node_2->type == AST_CConstant_t && is_const_null_ptr(&node_2->get._CConstant))
             || (node_1->exp_type->type == AST_Pointer_t && node_1->exp_type->get._Pointer.ref_type->type == AST_Void_t
                 && node_2->exp_type->type == AST_Pointer_t)) {
        sptr_copy(Type, node_1->exp_type, *joint_type);
    }
    else if (node_2->exp_type->type == AST_Pointer_t && node_2->exp_type->get._Pointer.ref_type->type == AST_Void_t
             && node_1->exp_type->type == AST_Pointer_t) {
        sptr_copy(Type, node_2->exp_type, *joint_type);
    }
    else {
        THROW_AT_TOKEN(
            node_1->line, GET_SEMANTIC_MSG(MSG_joint_ptr_mismatch, str_fmt_type(node_1->exp_type, &type_fmt_1),
                              str_fmt_type(node_2->exp_type, &type_fmt_2)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static TChar get_const_char_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return node->constant->get._CConstChar.value;
        case AST_CConstInt_t:
            return (TChar)node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return (TChar)node->constant->get._CConstLong.value;
        case AST_CConstDouble_t:
            return (TChar)node->constant->get._CConstDouble.value;
        case AST_CConstUChar_t:
            return (TChar)node->constant->get._CConstUChar.value;
        case AST_CConstUInt_t:
            return (TChar)node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return (TChar)node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static TInt get_const_int_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return (TInt)node->constant->get._CConstChar.value;
        case AST_CConstInt_t:
            return node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return (TInt)node->constant->get._CConstLong.value;
        case AST_CConstDouble_t:
            return (TInt)node->constant->get._CConstDouble.value;
        case AST_CConstUChar_t:
            return (TInt)node->constant->get._CConstUChar.value;
        case AST_CConstUInt_t:
            return (TInt)node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return (TInt)node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static TLong get_const_long_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return (TLong)node->constant->get._CConstChar.value;
        case AST_CConstInt_t:
            return (TLong)node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return node->constant->get._CConstLong.value;
        case AST_CConstDouble_t:
            return (TLong)node->constant->get._CConstDouble.value;
        case AST_CConstUChar_t:
            return (TLong)node->constant->get._CConstUChar.value;
        case AST_CConstUInt_t:
            return (TLong)node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return (TLong)node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static TDouble get_const_dbl_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return (TDouble)node->constant->get._CConstChar.value;
        case AST_CConstInt_t:
            return (TDouble)node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return (TDouble)node->constant->get._CConstLong.value;
        case AST_CConstDouble_t:
            return node->constant->get._CConstDouble.value;
        case AST_CConstUChar_t:
            return (TDouble)node->constant->get._CConstUChar.value;
        case AST_CConstUInt_t:
            return (TDouble)node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return (TDouble)node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static TUChar get_const_uchar_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return (TUChar)node->constant->get._CConstChar.value;
        case AST_CConstInt_t:
            return (TUChar)node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return (TUChar)node->constant->get._CConstLong.value;
        case AST_CConstDouble_t:
            return (TUChar)node->constant->get._CConstDouble.value;
        case AST_CConstUChar_t:
            return node->constant->get._CConstUChar.value;
        case AST_CConstUInt_t:
            return (TUChar)node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return (TUChar)node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static TUInt get_const_uint_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return (TUInt)node->constant->get._CConstChar.value;
        case AST_CConstInt_t:
            return (TUInt)node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return (TUInt)node->constant->get._CConstLong.value;
        case AST_CConstDouble_t:
            return (TUInt)node->constant->get._CConstDouble.value;
        case AST_CConstUChar_t:
            return (TUInt)node->constant->get._CConstUChar.value;
        case AST_CConstUInt_t:
            return node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return (TUInt)node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static TULong get_const_ulong_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
            return (TULong)node->constant->get._CConstChar.value;
        case AST_CConstInt_t:
            return (TULong)node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return (TULong)node->constant->get._CConstLong.value;
        case AST_CConstDouble_t:
            return (TULong)node->constant->get._CConstDouble.value;
        case AST_CConstUChar_t:
            return (TULong)node->constant->get._CConstUChar.value;
        case AST_CConstUInt_t:
            return (TULong)node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static TULong get_const_ptr_value(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstInt_t:
            return (TULong)node->constant->get._CConstInt.value;
        case AST_CConstLong_t:
            return (TULong)node->constant->get._CConstLong.value;
        case AST_CConstUInt_t:
            return (TULong)node->constant->get._CConstUInt.value;
        case AST_CConstULong_t:
            return node->constant->get._CConstULong.value;
        default:
            THROW_ABORT;
    }
}

static size_t get_compound_line(const CCompoundInit* node) {
    THROW_ABORT_IF(vec_empty(node->initializers));
    const CInitializer* initializer = node->initializers[0];
    while (initializer->type == AST_CCompoundInit_t) {
        node = &initializer->get._CCompoundInit;
        THROW_ABORT_IF(vec_empty(node->initializers));
        initializer = node->initializers[0];
    }
    THROW_ABORT_IF(initializer->type != AST_CSingleInit_t);
    return initializer->get._CSingleInit.exp->line;
}

static error_t reslv_struct_type(Ctx ctx, Type* type);

static void check_const_exp(const CConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t: {
            node->_base->exp_type = make_Char();
            break;
        }
        case AST_CConstInt_t: {
            node->_base->exp_type = make_Int();
            break;
        }
        case AST_CConstLong_t: {
            node->_base->exp_type = make_Long();
            break;
        }
        case AST_CConstDouble_t: {
            node->_base->exp_type = make_Double();
            break;
        }
        case AST_CConstUChar_t: {
            node->_base->exp_type = make_UChar();
            break;
        }
        case AST_CConstUInt_t: {
            node->_base->exp_type = make_UInt();
            break;
        }
        case AST_CConstULong_t: {
            node->_base->exp_type = make_ULong();
            break;
        }
        default:
            THROW_ABORT;
    }
}

static void check_string_exp(const CString* node) {
    TLong size = ((TLong)vec_size(node->literal->value)) + 1l;
    shared_ptr_t(Type) elem_type = make_Char();
    node->_base->exp_type = make_Array(size, &elem_type);
}

static error_t check_var_exp(Ctx ctx, const CVar* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    Type* var_type = map_get(ctx->frontend->symbol_table, node->name)->type_t;
    if (var_type->type == AST_FunType_t) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_fun_used_as_var, str_fmt_name(node->name, &name_fmt)));
    }
    sptr_copy(Type, var_type, node->_base->exp_type);
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t check_cast_exp(Ctx ctx, const CCast* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    ctx->errors->linebuf = node->_base->line;
    TRY(reslv_struct_type(ctx, node->target_type));
    if (node->target_type->type != AST_Void_t
        && ((node->exp->exp_type->type == AST_Double_t && node->target_type->type == AST_Pointer_t)
            || (node->exp->exp_type->type == AST_Pointer_t && node->target_type->type == AST_Double_t)
            || !is_type_scalar(node->exp->exp_type) || !is_type_scalar(node->target_type))) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_illegal_cast, str_fmt_type(node->exp->exp_type, &type_fmt_1),
                                   str_fmt_type(node->target_type, &type_fmt_2)));
    }
    TRY(is_valid_type(ctx, node->target_type));
    sptr_copy(Type, node->target_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t cast_exp(Ctx ctx, shared_ptr_t(Type) * exp_type, unique_ptr_t(CExp) * exp) {
    shared_ptr_t(Type) exp_type_cp = sptr_new();
    CATCH_ENTER;
    size_t line = (*exp)->line;
    sptr_copy(Type, *exp_type, exp_type_cp);
    *exp = make_CCast(exp, &exp_type_cp, line);
    TRY(check_cast_exp(ctx, &(*exp)->get._CCast));
    FINALLY;
    free_Type(&exp_type_cp);
    CATCH_EXIT;
}

static error_t cast_assign(Ctx ctx, shared_ptr_t(Type) * exp_type, unique_ptr_t(CExp) * exp) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if ((is_type_arithmetic((*exp)->exp_type) && is_type_arithmetic(*exp_type))
        || ((*exp)->type == AST_CConstant_t && (*exp_type)->type == AST_Pointer_t
            && is_const_null_ptr(&(*exp)->get._CConstant))
        || ((*exp_type)->type == AST_Pointer_t && (*exp_type)->get._Pointer.ref_type->type == AST_Void_t
            && (*exp)->exp_type->type == AST_Pointer_t)
        || ((*exp)->exp_type->type == AST_Pointer_t && (*exp)->exp_type->get._Pointer.ref_type->type == AST_Void_t
            && (*exp_type)->type == AST_Pointer_t)) {
        TRY(cast_exp(ctx, exp_type, exp));
    }
    else {
        THROW_AT_TOKEN((*exp)->line, GET_SEMANTIC_MSG(MSG_illegal_cast, str_fmt_type((*exp)->exp_type, &type_fmt_1),
                                         str_fmt_type(*exp_type, &type_fmt_2)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t promote_char_to_int(Ctx ctx, unique_ptr_t(CExp) * exp) {
    shared_ptr_t(Type) promote_type = sptr_new();
    CATCH_ENTER;
    promote_type = make_Int();
    TRY(cast_exp(ctx, &promote_type, exp));
    FINALLY;
    free_Type(&promote_type);
    CATCH_EXIT;
}

static error_t check_unary_complement_exp(Ctx ctx, CUnary* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp->exp_type)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(&node->unop),
                                              str_fmt_type(node->exp->exp_type, &type_fmt)));
    }

    switch (node->exp->exp_type->type) {
        case AST_Double_t:
            THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(&node->unop),
                                                  str_fmt_type(node->exp->exp_type, &type_fmt)));
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            TRY(promote_char_to_int(ctx, &node->exp));
            break;
        default:
            break;
    }
    sptr_copy(Type, node->exp->exp_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_unary_neg_exp(Ctx ctx, CUnary* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp->exp_type)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(&node->unop),
                                              str_fmt_type(node->exp->exp_type, &type_fmt)));
    }

    switch (node->exp->exp_type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            TRY(promote_char_to_int(ctx, &node->exp));
            break;
        default:
            break;
    }
    sptr_copy(Type, node->exp->exp_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_unary_not_exp(Ctx ctx, const CUnary* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_scalar(node->exp->exp_type)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(&node->unop),
                                              str_fmt_type(node->exp->exp_type, &type_fmt)));
    }

    node->_base->exp_type = make_Int();
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_unary_exp(Ctx ctx, CUnary* node) {
    CATCH_ENTER;
    switch (node->unop.type) {
        case AST_CComplement_t:
            TRY(check_unary_complement_exp(ctx, node));
            break;
        case AST_CNegate_t:
            TRY(check_unary_neg_exp(ctx, node));
            break;
        case AST_CNot_t:
            TRY(check_unary_not_exp(ctx, node));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_binary_add_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) common_type = sptr_new();
    CATCH_ENTER;
    if (is_type_arithmetic(node->exp_left->exp_type) && is_type_arithmetic(node->exp_right->exp_type)) {
        common_type = get_joint_type(node->exp_left, node->exp_right);
    }
    else if (node->exp_left->exp_type->type == AST_Pointer_t
             && is_type_complete(ctx, node->exp_left->exp_type->get._Pointer.ref_type)
             && is_type_int(node->exp_right->exp_type)) {
        common_type = make_Long();
        if (!is_same_type(node->exp_right->exp_type, common_type)) {
            TRY(cast_exp(ctx, &common_type, &node->exp_right));
        }
        sptr_copy(Type, node->exp_left->exp_type, node->_base->exp_type);
        EARLY_EXIT;
    }
    else if (is_type_int(node->exp_left->exp_type) && node->exp_right->exp_type->type == AST_Pointer_t
             && is_type_complete(ctx, node->exp_right->exp_type->get._Pointer.ref_type)) {
        common_type = make_Long();
        if (!is_same_type(node->exp_left->exp_type, common_type)) {
            TRY(cast_exp(ctx, &common_type, &node->exp_left));
        }
        sptr_copy(Type, node->exp_right->exp_type, node->_base->exp_type);
        EARLY_EXIT;
    }
    else {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    if (!is_same_type(node->exp_left->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_right));
    }
    sptr_move(Type, common_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&common_type);
    CATCH_EXIT;
}

static error_t check_binary_subtract_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) common_type = sptr_new();
    CATCH_ENTER;
    if (is_type_arithmetic(node->exp_left->exp_type) && is_type_arithmetic(node->exp_right->exp_type)) {
        common_type = get_joint_type(node->exp_left, node->exp_right);
    }
    else if (node->exp_left->exp_type->type == AST_Pointer_t
             && is_type_complete(ctx, node->exp_left->exp_type->get._Pointer.ref_type)) {
        if (is_type_int(node->exp_right->exp_type)) {
            common_type = make_Long();
            if (!is_same_type(node->exp_right->exp_type, common_type)) {
                TRY(cast_exp(ctx, &common_type, &node->exp_right));
            }
            sptr_copy(Type, node->exp_left->exp_type, node->_base->exp_type);
            EARLY_EXIT;
        }
        else if (is_same_type(node->exp_left->exp_type, node->exp_right->exp_type)
                 && !(node->exp_left->type == AST_CConstant_t && is_const_null_ptr(&node->exp_left->get._CConstant))) {
            common_type = make_Long();
            sptr_move(Type, common_type, node->_base->exp_type);
            EARLY_EXIT;
        }
        else {
            THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                                  str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                                  str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
        }
    }
    else {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    if (!is_same_type(node->exp_left->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_right));
    }
    sptr_move(Type, common_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&common_type);
    CATCH_EXIT;
}

static error_t check_multiply_divide_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) common_type = sptr_new();
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp_left->exp_type) || !is_type_arithmetic(node->exp_right->exp_type)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    common_type = get_joint_type(node->exp_left, node->exp_right);
    if (!is_same_type(node->exp_left->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_right));
    }
    sptr_move(Type, common_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&common_type);
    CATCH_EXIT;
}

static error_t check_remainder_bitwise_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) common_type = sptr_new();
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp_left->exp_type) || !is_type_arithmetic(node->exp_right->exp_type)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    common_type = get_joint_type(node->exp_left, node->exp_right);
    if (!is_same_type(node->exp_left->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_right));
    }
    sptr_move(Type, common_type, node->_base->exp_type);
    if (node->_base->exp_type->type == AST_Double_t) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_op, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->_base->exp_type, &type_fmt_1)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&common_type);
    CATCH_EXIT;
}

static error_t check_binary_bitshift_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp_left->exp_type) || !is_type_int(node->exp_right->exp_type)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    else if (is_type_char(node->exp_left->exp_type)) {
        TRY(promote_char_to_int(ctx, &node->exp_left));
    }
    if (!is_same_type(node->exp_left->exp_type, node->exp_right->exp_type)) {
        TRY(cast_exp(ctx, &node->exp_left->exp_type, &node->exp_right));
    }
    sptr_copy(Type, node->exp_left->exp_type, node->_base->exp_type);
    if (node->_base->exp_type->type == AST_Double_t) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_op, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->_base->exp_type, &type_fmt_1)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_bitshift_right_exp(Ctx ctx, CBinary* node) {
    CATCH_ENTER;
    TRY(check_binary_bitshift_exp(ctx, node));
    if (is_type_signed(node->exp_left->exp_type)) {
        node->binop = init_CBitShrArithmetic();
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_binary_logical_exp(Ctx ctx, const CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_scalar(node->exp_left->exp_type) || !is_type_scalar(node->exp_right->exp_type)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    node->_base->exp_type = make_Int();
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_binary_equality_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) common_type = sptr_new();
    CATCH_ENTER;
    if (node->exp_left->exp_type->type == AST_Pointer_t || node->exp_right->exp_type->type == AST_Pointer_t) {
        TRY(get_joint_ptr_type(ctx, node->exp_left, node->exp_right, &common_type));
    }
    else if (is_type_arithmetic(node->exp_left->exp_type) && is_type_arithmetic(node->exp_right->exp_type)) {
        common_type = get_joint_type(node->exp_left, node->exp_right);
    }
    else {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    if (!is_same_type(node->exp_left->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_right));
    }
    node->_base->exp_type = make_Int();
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&common_type);
    CATCH_EXIT;
}

static error_t check_binary_relational_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) common_type = sptr_new();
    CATCH_ENTER;
    if (!is_type_scalar(node->exp_left->exp_type) || !is_type_scalar(node->exp_right->exp_type)
        || (node->exp_left->exp_type->type == AST_Pointer_t
            && (!is_same_type(node->exp_left->exp_type, node->exp_right->exp_type)
                || (node->exp_left->type == AST_CConstant_t && is_const_null_ptr(&node->exp_left->get._CConstant))
                || (node->exp_right->type == AST_CConstant_t
                    && is_const_null_ptr(&node->exp_right->get._CConstant))))) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(&node->binop),
                                              str_fmt_type(node->exp_left->exp_type, &type_fmt_1),
                                              str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }

    common_type = get_joint_type(node->exp_left, node->exp_right);
    if (!is_same_type(node->exp_left->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_right));
    }
    node->_base->exp_type = make_Int();
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&common_type);
    CATCH_EXIT;
}

static error_t check_binary_exp(Ctx ctx, CBinary* node) {
    CATCH_ENTER;
    switch (node->binop.type) {
        case AST_CAdd_t:
            TRY(check_binary_add_exp(ctx, node));
            break;
        case AST_CSubtract_t:
            TRY(check_binary_subtract_exp(ctx, node));
            break;
        case AST_CMultiply_t:
        case AST_CDivide_t:
            TRY(check_multiply_divide_exp(ctx, node));
            break;
        case AST_CRemainder_t:
        case AST_CBitAnd_t:
        case AST_CBitOr_t:
        case AST_CBitXor_t:
            TRY(check_remainder_bitwise_exp(ctx, node));
            break;
        case AST_CBitShiftLeft_t:
            TRY(check_binary_bitshift_exp(ctx, node));
            break;
        case AST_CBitShiftRight_t:
            TRY(check_bitshift_right_exp(ctx, node));
            break;
        case AST_CAnd_t:
        case AST_COr_t:
            TRY(check_binary_logical_exp(ctx, node));
            break;
        case AST_CEqual_t:
        case AST_CNotEqual_t:
            TRY(check_binary_equality_exp(ctx, node));
            break;
        case AST_CLessThan_t:
        case AST_CLessOrEqual_t:
        case AST_CGreaterThan_t:
        case AST_CGreaterOrEqual_t:
            TRY(check_binary_relational_exp(ctx, node));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_assign_exp(Ctx ctx, CAssignment* node) {
    CATCH_ENTER;
    if (node->exp_left) {
        if (node->exp_left->exp_type->type == AST_Void_t) {
            THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG_0(MSG_assign_to_void));
        }
        else if (!is_exp_lvalue(node->exp_left)) {
            THROW_AT_TOKEN(
                node->_base->line, GET_SEMANTIC_MSG(MSG_assign_to_rvalue, get_assign_fmt(NULL, &node->unop)));
        }
        else if (!is_same_type(node->exp_right->exp_type, node->exp_left->exp_type)) {
            TRY(cast_assign(ctx, &node->exp_left->exp_type, &node->exp_right));
        }
        sptr_copy(Type, node->exp_left->exp_type, node->_base->exp_type);
    }
    else {
        THROW_ABORT_IF(node->exp_right->type != AST_CBinary_t);
        CExp* exp_left = node->exp_right->get._CBinary.exp_left;
        if (exp_left->type == AST_CCast_t) {
            exp_left = exp_left->get._CCast.exp;
        }
        if (!is_exp_lvalue(exp_left)) {
            THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_assign_to_rvalue,
                                                  get_assign_fmt(&node->exp_right->get._CBinary.binop, &node->unop)));
        }
        else if (!is_same_type(node->exp_right->exp_type, exp_left->exp_type)) {
            TRY(cast_assign(ctx, &exp_left->exp_type, &node->exp_right));
        }
        sptr_copy(Type, exp_left->exp_type, node->_base->exp_type);
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_conditional_exp(Ctx ctx, CConditional* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) common_type = sptr_new();
    CATCH_ENTER;
    if (!is_type_scalar(node->condition->exp_type)) {
        THROW_AT_TOKEN(node->_base->line,
            GET_SEMANTIC_MSG(MSG_invalid_condition, str_fmt_type(node->condition->exp_type, &type_fmt_1)));
    }
    else if (node->exp_middle->exp_type->type == AST_Void_t && node->exp_right->exp_type->type == AST_Void_t) {
        sptr_copy(Type, node->exp_middle->exp_type, node->_base->exp_type);
        EARLY_EXIT;
    }
    else if (node->exp_middle->exp_type->type == AST_Structure_t
             || node->exp_right->exp_type->type == AST_Structure_t) {
        if (!is_same_type(node->exp_middle->exp_type, node->exp_right->exp_type)) {
            THROW_AT_TOKEN(node->_base->line,
                GET_SEMANTIC_MSG(MSG_invalid_ternary_op, str_fmt_type(node->exp_middle->exp_type, &type_fmt_1),
                    str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
        }
        sptr_copy(Type, node->exp_middle->exp_type, node->_base->exp_type);
        EARLY_EXIT;
    }

    if (is_type_arithmetic(node->exp_middle->exp_type) && is_type_arithmetic(node->exp_right->exp_type)) {
        common_type = get_joint_type(node->exp_middle, node->exp_right);
    }
    else if (node->exp_middle->exp_type->type == AST_Pointer_t || node->exp_right->exp_type->type == AST_Pointer_t) {
        TRY(get_joint_ptr_type(ctx, node->exp_middle, node->exp_right, &common_type));
    }
    else {
        THROW_AT_TOKEN(node->_base->line,
            GET_SEMANTIC_MSG(MSG_invalid_ternary_op, str_fmt_type(node->exp_middle->exp_type, &type_fmt_1),
                str_fmt_type(node->exp_right->exp_type, &type_fmt_2)));
    }
    if (!is_same_type(node->exp_middle->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_middle));
    }
    if (!is_same_type(node->exp_right->exp_type, common_type)) {
        TRY(cast_exp(ctx, &common_type, &node->exp_right));
    }
    sptr_move(Type, common_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&common_type);
    CATCH_EXIT;
}

static error_t check_call_exp(Ctx ctx, const CFunctionCall* node) {
    string_t name_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    const Symbol* fun_symbol = map_get(ctx->frontend->symbol_table, node->name);
    const FunType* fun_type = &fun_symbol->type_t->get._FunType;
    if (fun_symbol->type_t->type != AST_FunType_t) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_var_used_as_fun, str_fmt_name(node->name, &name_fmt)));
    }
    else if (vec_size(fun_type->param_types) != vec_size(node->args)) {
        strto_fmt_1 = str_to_string(vec_size(node->args));
        strto_fmt_2 = str_to_string(vec_size(fun_type->param_types));
        THROW_AT_TOKEN(node->_base->line,
            GET_SEMANTIC_MSG(MSG_call_with_wrong_argc, str_fmt_name(node->name, &name_fmt), strto_fmt_1, strto_fmt_2));
    }
    for (size_t i = 0; i < vec_size(node->args); ++i) {
        if (!is_same_type(node->args[i]->exp_type, fun_type->param_types[i])) {
            TRY(cast_assign(ctx, &fun_type->param_types[i], &node->args[i]));
        }
    }
    sptr_copy(Type, fun_type->ret_type, node->_base->exp_type);
    FINALLY;
    str_delete(name_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static error_t check_deref_exp(Ctx ctx, const CDereference* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->exp->exp_type->type != AST_Pointer_t) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_deref_not_ptr, str_fmt_type(node->exp->exp_type, &type_fmt)));
    }
    sptr_copy(Type, node->exp->exp_type->get._Pointer.ref_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_addrof_exp(Ctx ctx, const CAddrOf* node) {
    shared_ptr_t(Type) ref_type = sptr_new();
    CATCH_ENTER;
    if (!is_exp_lvalue(node->exp)) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG_0(MSG_addrof_rvalue));
    }
    sptr_copy(Type, node->exp->exp_type, ref_type);
    node->_base->exp_type = make_Pointer(&ref_type);
    FINALLY;
    free_Type(&ref_type);
    CATCH_EXIT;
}

static error_t check_subscript_exp(Ctx ctx, CSubscript* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    shared_ptr_t(Type) ref_type = sptr_new();
    shared_ptr_t(Type) subscript_type = sptr_new();
    CATCH_ENTER;
    if (node->primary_exp->exp_type->type == AST_Pointer_t
        && is_type_complete(ctx, node->primary_exp->exp_type->get._Pointer.ref_type)
        && is_type_int(node->subscript_exp->exp_type)) {
        subscript_type = make_Long();
        if (!is_same_type(node->subscript_exp->exp_type, subscript_type)) {
            TRY(cast_exp(ctx, &subscript_type, &node->subscript_exp));
        }
        sptr_copy(Type, node->primary_exp->exp_type->get._Pointer.ref_type, ref_type);
    }
    else if (is_type_int(node->primary_exp->exp_type) && node->subscript_exp->exp_type->type == AST_Pointer_t
             && is_type_complete(ctx, node->subscript_exp->exp_type->get._Pointer.ref_type)) {
        subscript_type = make_Long();
        if (!is_same_type(node->primary_exp->exp_type, subscript_type)) {
            TRY(cast_exp(ctx, &subscript_type, &node->primary_exp));
        }
        sptr_copy(Type, node->subscript_exp->exp_type->get._Pointer.ref_type, ref_type);
    }
    else {
        THROW_AT_TOKEN(node->_base->line,
            GET_SEMANTIC_MSG(MSG_invalid_subscript, str_fmt_type(node->primary_exp->exp_type, &type_fmt_1),
                str_fmt_type(node->subscript_exp->exp_type, &type_fmt_2)));
    }
    sptr_move(Type, ref_type, node->_base->exp_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_Type(&ref_type);
    free_Type(&subscript_type);
    CATCH_EXIT;
}

static error_t check_sizeof_exp(Ctx ctx, const CSizeOf* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_complete(ctx, node->exp->exp_type)) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_sizeof_incomplete, str_fmt_type(node->exp->exp_type, &type_fmt)));
    }
    node->_base->exp_type = make_ULong();
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_sizeoft_exp(Ctx ctx, const CSizeOfT* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    ctx->errors->linebuf = node->_base->line;
    TRY(reslv_struct_type(ctx, node->target_type));
    if (!is_type_complete(ctx, node->target_type)) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_sizeof_incomplete, str_fmt_type(node->target_type, &type_fmt)));
    }
    TRY(is_valid_type(ctx, node->target_type));
    node->_base->exp_type = make_ULong();
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_dot_exp(Ctx ctx, const CDot* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    const Structure* struct_type;
    StructTypedef* struct_typedef;
    Type* member_type;
    ssize_t map_it;
    if (node->structure->exp_type->type != AST_Structure_t) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_dot_not_struct, str_fmt_name(node->member, &name_fmt),
                                              str_fmt_type(node->structure->exp_type, &type_fmt)));
    }
    struct_type = &node->structure->exp_type->get._Structure;
    struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    map_it = map_find(struct_typedef->members, node->member);
    if (map_it == map_end()) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_member_not_in_struct, str_fmt_struct(struct_type, &type_fmt),
                                   str_fmt_name(node->member, &name_fmt)));
    }
    member_type = pair_second(struct_typedef->members[map_it])->member_type;
    sptr_copy(Type, member_type, node->_base->exp_type);
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_arrow_exp(Ctx ctx, const CArrow* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    const Pointer* ptr_type;
    const Structure* struct_type;
    StructTypedef* struct_typedef;
    Type* member_type;
    ssize_t map_it;
    if (node->pointer->exp_type->type != AST_Pointer_t) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_arrow_not_struct_ptr, str_fmt_name(node->member, &name_fmt),
                                   str_fmt_type(node->pointer->exp_type, &type_fmt)));
    }
    ptr_type = &node->pointer->exp_type->get._Pointer;
    if (ptr_type->ref_type->type != AST_Structure_t) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_arrow_not_struct_ptr, str_fmt_name(node->member, &name_fmt),
                                   str_fmt_type(node->pointer->exp_type, &type_fmt)));
    }
    struct_type = &ptr_type->ref_type->get._Structure;
    map_it = map_find(ctx->frontend->struct_typedef_table, struct_type->tag);
    if (map_it == map_end()) {
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_arrow_incomplete, str_fmt_name(node->member, &name_fmt),
                                              str_fmt_struct(struct_type, &type_fmt)));
    }
    struct_typedef = pair_second(ctx->frontend->struct_typedef_table[map_it]);
    map_it = map_find(struct_typedef->members, node->member);
    if (map_it == map_end()) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_member_not_in_struct, str_fmt_struct(struct_type, &type_fmt),
                                   str_fmt_name(node->member, &name_fmt)));
    }
    member_type = pair_second(struct_typedef->members[map_it])->member_type;
    sptr_copy(Type, member_type, node->_base->exp_type);
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static void check_arr_typed_exp(unique_ptr_t(CExp) * addrof) {
    {
        shared_ptr_t(Type) ref_type = sptr_new();
        sptr_copy(Type, (*addrof)->exp_type->get._Array.elem_type, ref_type);
        free_Type(&(*addrof)->exp_type);
        (*addrof)->exp_type = make_Pointer(&ref_type);
    }
    size_t line = (*addrof)->line;
    *addrof = make_CAddrOf(addrof, line);
    sptr_copy(Type, (*addrof)->get._CAddrOf.exp->exp_type, (*addrof)->exp_type);
}

static error_t check_struct_typed_exp(Ctx ctx, const CExp* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_struct_complete(ctx, &node->exp_type->get._Structure)) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_exp_incomplete, str_fmt_type(node->exp_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_typed_exp(Ctx ctx, unique_ptr_t(CExp) * exp) {
    CATCH_ENTER;
    switch ((*exp)->exp_type->type) {
        case AST_Array_t:
            check_arr_typed_exp(exp);
            break;
        case AST_Structure_t:
            TRY(check_struct_typed_exp(ctx, *exp));
            break;
        default:
            break;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_ret_statement(Ctx ctx, CReturn* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    FunType* fun_type = &map_get(ctx->frontend->symbol_table, ctx->fun_def_name)->type_t->get._FunType;
    if (fun_type->ret_type->type == AST_Void_t) {
        if (node->exp) {
            THROW_AT_TOKEN(
                node->line, GET_SEMANTIC_MSG(MSG_ret_value_in_void_fun, str_fmt_name(ctx->fun_def_name, &name_fmt)));
        }
        EARLY_EXIT;
    }
    else if (!node->exp) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_no_ret_value_in_fun, str_fmt_name(ctx->fun_def_name, &name_fmt),
                                       str_fmt_type(fun_type->ret_type, &type_fmt)));
    }

    else if (!is_same_type(node->exp->exp_type, fun_type->ret_type)) {
        TRY(cast_assign(ctx, &fun_type->ret_type, &node->exp));
    }
    TRY(check_typed_exp(ctx, &node->exp));
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_if_statement(Ctx ctx, const CIf* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type)) {
        THROW_AT_TOKEN(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_if, str_fmt_type(node->condition->exp_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_while_statement(Ctx ctx, const CWhile* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type)) {
        THROW_AT_TOKEN(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_while, str_fmt_type(node->condition->exp_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_do_while_statement(Ctx ctx, const CDoWhile* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type)) {
        THROW_AT_TOKEN(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_do_while, str_fmt_type(node->condition->exp_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_for_statement(Ctx ctx, const CFor* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type)) {
        THROW_AT_TOKEN(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_for, str_fmt_type(node->condition->exp_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_switch_int_cases(Ctx ctx, const CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    vector_t(TInt) values = vec_new();
    CATCH_ENTER;
    vec_resize(values, vec_size(node->cases));
    for (size_t i = 0; i < vec_size(values); ++i) {
        THROW_ABORT_IF(node->cases[i]->type != AST_CConstant_t);
        CConstant* esac = &node->cases[i]->get._CConstant;
        values[i] = get_const_int_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_TOKEN(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        free_CConst(&esac->constant);
        esac->constant = make_CConstInt(values[i]);
        sptr_copy(Type, node->match->exp_type, esac->_base->exp_type);
    }
    FINALLY;
    str_delete(strto_fmt);
    vec_delete(values);
    CATCH_EXIT;
}

static error_t check_switch_long_cases(Ctx ctx, const CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    vector_t(TLong) values = vec_new();
    CATCH_ENTER;
    vec_resize(values, vec_size(node->cases));
    for (size_t i = 0; i < vec_size(values); ++i) {
        THROW_ABORT_IF(node->cases[i]->type != AST_CConstant_t);
        CConstant* esac = &node->cases[i]->get._CConstant;
        values[i] = get_const_long_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_TOKEN(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        free_CConst(&esac->constant);
        esac->constant = make_CConstLong(values[i]);
        sptr_copy(Type, node->match->exp_type, esac->_base->exp_type);
    }
    FINALLY;
    str_delete(strto_fmt);
    vec_delete(values);
    CATCH_EXIT;
}

static error_t check_switch_uint_cases(Ctx ctx, const CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    vector_t(TUInt) values = vec_new();
    CATCH_ENTER;
    vec_resize(values, vec_size(node->cases));
    for (size_t i = 0; i < vec_size(values); ++i) {
        THROW_ABORT_IF(node->cases[i]->type != AST_CConstant_t);
        CConstant* esac = &node->cases[i]->get._CConstant;
        values[i] = get_const_uint_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_TOKEN(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        free_CConst(&esac->constant);
        esac->constant = make_CConstUInt(values[i]);
        sptr_copy(Type, node->match->exp_type, esac->_base->exp_type);
    }
    FINALLY;
    str_delete(strto_fmt);
    vec_delete(values);
    CATCH_EXIT;
}

static error_t check_switch_ulong_cases(Ctx ctx, const CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    vector_t(TULong) values = vec_new();
    CATCH_ENTER;
    vec_resize(values, vec_size(node->cases));
    for (size_t i = 0; i < vec_size(values); ++i) {
        THROW_ABORT_IF(node->cases[i]->type != AST_CConstant_t);
        CConstant* esac = &node->cases[i]->get._CConstant;
        values[i] = get_const_ulong_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_TOKEN(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        free_CConst(&esac->constant);
        esac->constant = make_CConstULong(values[i]);
        sptr_copy(Type, node->match->exp_type, esac->_base->exp_type);
    }
    FINALLY;
    str_delete(strto_fmt);
    vec_delete(values);
    CATCH_EXIT;
}

static error_t check_switch_statement(Ctx ctx, CSwitch* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_int(node->match->exp_type)) {
        THROW_AT_TOKEN(
            node->match->line, GET_SEMANTIC_MSG(MSG_invalid_switch, str_fmt_type(node->match->exp_type, &type_fmt)));
    }
    switch (node->match->exp_type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            TRY(promote_char_to_int(ctx, &node->match));
            break;
        default:
            break;
    }
    switch (node->match->exp_type->type) {
        case AST_Int_t:
            TRY(check_switch_int_cases(ctx, node));
            break;
        case AST_Long_t:
            TRY(check_switch_long_cases(ctx, node));
            break;
        case AST_UInt_t:
            TRY(check_switch_uint_cases(ctx, node));
            break;
        case AST_ULong_t:
            TRY(check_switch_ulong_cases(ctx, node));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_bound_string_init(Ctx ctx, const CString* node, const Array* arr_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_char(arr_type->elem_type)) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_string_init_not_char_arr, str_fmt_arr(arr_type, &type_fmt)));
    }
    else if (vec_size(node->literal->value) > (size_t)arr_type->size) {
        strto_fmt_1 = str_to_string(arr_type->size);
        strto_fmt_2 = str_to_string(vec_size(node->literal->value));
        THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_string_init_overflow, strto_fmt_1, strto_fmt_2));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static error_t check_single_init(Ctx ctx, CSingleInit* node, shared_ptr_t(Type) * init_type) {
    CATCH_ENTER;
    if (!is_same_type(node->exp->exp_type, *init_type)) {
        TRY(cast_assign(ctx, init_type, &node->exp));
    }
    sptr_copy(Type, *init_type, node->_base->init_type);
    FINALLY;
    CATCH_EXIT;
}

static void check_string_init(const CSingleInit* node, shared_ptr_t(Type) * init_type) {
    sptr_copy(Type, *init_type, node->exp->exp_type);
    sptr_copy(Type, *init_type, node->_base->init_type);
}

static unique_ptr_t(CInitializer) check_zero_init(Ctx ctx, const Type* init_type);

static unique_ptr_t(CInitializer) check_single_zero_init(const Type* elem_type) {
    unique_ptr_t(CExp) exp = uptr_new();
    {
        shared_ptr_t(CConst) constant = sptr_new();
        switch (elem_type->type) {
            case AST_Char_t:
            case AST_SChar_t: {
                constant = make_CConstChar(0);
                break;
            }
            case AST_Int_t: {
                constant = make_CConstInt(0);
                break;
            }
            case AST_Long_t: {
                constant = make_CConstLong(0l);
                break;
            }
            case AST_Double_t: {
                constant = make_CConstDouble(0.0);
                break;
            }
            case AST_UChar_t: {
                constant = make_CConstUChar(0u);
                break;
            }
            case AST_UInt_t: {
                constant = make_CConstUInt(0u);
                break;
            }
            case AST_ULong_t:
            case AST_Pointer_t: {
                constant = make_CConstULong(0ul);
                break;
            }
            default:
                THROW_ABORT;
        }
        exp = make_CConstant(&constant, 0);
    }
    return make_CSingleInit(&exp);
}

static unique_ptr_t(CInitializer) check_arr_zero_init(Ctx ctx, const Array* arr_type) {
    vector_t(unique_ptr_t(CInitializer)) zero_inits = vec_new();
    size_t arr_type_size = (size_t)arr_type->size;
    vec_reserve(zero_inits, arr_type_size);
    for (size_t i = 0; i < arr_type_size; ++i) {
        unique_ptr_t(CInitializer) initializer = check_zero_init(ctx, arr_type->elem_type);
        vec_move_back(zero_inits, initializer);
    }
    return make_CCompoundInit(&zero_inits);
}

static unique_ptr_t(CInitializer) check_struct_zero_init(Ctx ctx, const Structure* struct_type) {
    vector_t(unique_ptr_t(CInitializer)) zero_inits = vec_new();
    const StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    vec_reserve(zero_inits, vec_size(struct_typedef->member_names));
    for (size_t i = 0; i < vec_size(struct_typedef->member_names); ++i) {
        const StructMember* member = get_struct_typedef_member(ctx->frontend, struct_type->tag, i);
        unique_ptr_t(CInitializer) initializer = check_zero_init(ctx, member->member_type);
        vec_move_back(zero_inits, initializer);
    }
    return make_CCompoundInit(&zero_inits);
}

static unique_ptr_t(CInitializer) check_zero_init(Ctx ctx, const Type* init_type) {
    switch (init_type->type) {
        case AST_Array_t:
            return check_arr_zero_init(ctx, &init_type->get._Array);
        case AST_Structure_t:
            return check_struct_zero_init(ctx, &init_type->get._Structure);
        default:
            return check_single_zero_init(init_type);
    }
}

static error_t check_bound_arr_init(Ctx ctx, const CCompoundInit* node, const Array* arr_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (vec_size(node->initializers) > (size_t)arr_type->size) {
        strto_fmt_1 = str_to_string(arr_type->size);
        strto_fmt_2 = str_to_string(vec_size(node->initializers));
        THROW_AT_TOKEN(get_compound_line(node),
            GET_SEMANTIC_MSG(MSG_arr_init_overflow, strto_fmt_1, str_fmt_arr(arr_type, &type_fmt), strto_fmt_2));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static error_t check_bound_struct_init(Ctx ctx, const CCompoundInit* node, const Structure* struct_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    const StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    size_t bound = struct_type->is_union ? 1 : map_size(struct_typedef->members);
    if (vec_size(node->initializers) > bound) {
        strto_fmt_1 = str_to_string(vec_size(node->initializers));
        strto_fmt_2 = str_to_string(bound);
        THROW_AT_TOKEN(get_compound_line(node), GET_SEMANTIC_MSG(MSG_struct_init_overflow,
                                                    str_fmt_struct(struct_type, &type_fmt), strto_fmt_1, strto_fmt_2));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static void check_arr_init(Ctx ctx, CCompoundInit* node, const Array* arr_type, shared_ptr_t(Type) * init_type) {
    while (vec_size(node->initializers) < (size_t)arr_type->size) {
        unique_ptr_t(CInitializer) zero_init = check_zero_init(ctx, arr_type->elem_type);
        vec_move_back(node->initializers, zero_init);
    }
    sptr_copy(Type, *init_type, node->_base->init_type);
}

static void check_struct_init(
    Ctx ctx, CCompoundInit* node, const Structure* struct_type, shared_ptr_t(Type) * init_type) {
    const StructTypedef* struct_typedef = map_get(ctx->frontend->struct_typedef_table, struct_type->tag);
    for (size_t i = vec_size(node->initializers); i < map_size(struct_typedef->members); ++i) {
        const StructMember* member = get_struct_typedef_member(ctx->frontend, struct_type->tag, i);
        unique_ptr_t(CInitializer) zero_init = check_zero_init(ctx, member->member_type);
        vec_move_back(node->initializers, zero_init);
    }
    sptr_copy(Type, *init_type, node->_base->init_type);
}

static error_t check_ret_fun_decl(Ctx ctx, const CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    const FunType* fun_type = &node->fun_type->get._FunType;
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, fun_type->ret_type));
    TRY(is_valid_type(ctx, fun_type->ret_type));

    switch (fun_type->ret_type->type) {
        case AST_Array_t:
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_ret_arr, str_fmt_name(node->name, &name_fmt),
                                           str_fmt_type(fun_type->ret_type, &type_fmt)));
        case AST_Structure_t: {
            if (node->body && !is_struct_complete(ctx, &fun_type->ret_type->get._Structure)) {
                THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_ret_incomplete, str_fmt_name(node->name, &name_fmt),
                                               str_fmt_type(fun_type->ret_type, &type_fmt)));
            }
            break;
        }
        default:
            break;
    }
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static void check_arr_param_decl(const FunType* fun_type, size_t i) {
    shared_ptr_t(Type) ref_type = sptr_new();
    sptr_copy(Type, fun_type->param_types[i]->get._Array.elem_type, ref_type);
    free_Type(&fun_type->param_types[i]);
    fun_type->param_types[i] = make_Pointer(&ref_type);
}

static error_t check_fun_params_decl(Ctx ctx, const CFunctionDeclaration* node) {
    string_t name_fmt_1 = str_new(NULL);
    string_t name_fmt_2 = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    unique_ptr_t(IdentifierAttr) param_attrs = uptr_new();
    unique_ptr_t(Symbol) symbol = uptr_new();
    shared_ptr_t(Type) param_type = sptr_new();
    CATCH_ENTER;
    const FunType* fun_type = &node->fun_type->get._FunType;
    for (size_t i = 0; i < vec_size(node->params); ++i) {
        ctx->errors->linebuf = node->line;
        TRY(reslv_struct_type(ctx, fun_type->param_types[i]));
        if (fun_type->param_types[i]->type == AST_Void_t) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_void_param, str_fmt_name(node->name, &name_fmt_1),
                                           str_fmt_name(node->params[i], &name_fmt_2)));
        }
        TRY(is_valid_type(ctx, fun_type->param_types[i]));
        if (fun_type->param_types[i]->type == AST_Array_t) {
            check_arr_param_decl(fun_type, i);
        }

        if (node->body) {
            if (fun_type->param_types[i]->type == AST_Structure_t
                && !is_struct_complete(ctx, &fun_type->param_types[i]->get._Structure)) {
                THROW_AT_TOKEN(node->line,
                    GET_SEMANTIC_MSG(MSG_incomplete_param, str_fmt_name(node->name, &name_fmt_1),
                        str_fmt_name(node->params[i], &name_fmt_2), str_fmt_type(fun_type->param_types[i], &type_fmt)));
            }
            sptr_copy(Type, fun_type->param_types[i], param_type);
            param_attrs = make_LocalAttr();
            THROW_ABORT_IF(map_find(ctx->frontend->symbol_table, node->params[i]) != map_end());
            symbol = make_Symbol(&param_type, &param_attrs);
            map_move_add(ctx->frontend->symbol_table, node->params[i], symbol);
        }
    }
    FINALLY;
    str_delete(name_fmt_1);
    str_delete(name_fmt_2);
    str_delete(type_fmt);
    free_IdentifierAttr(&param_attrs);
    free_Symbol(&symbol);
    free_Type(&param_type);
    CATCH_EXIT;
}

static error_t check_fun_decl(Ctx ctx, const CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    unique_ptr_t(IdentifierAttr) glob_fun_attrs = uptr_new();
    unique_ptr_t(Symbol) symbol = uptr_new();
    shared_ptr_t(Type) glob_fun_type = sptr_new();
    CATCH_ENTER;
    THROW_ABORT_IF(node->fun_type->type == AST_Void_t);

    bool is_def = set_find(ctx->fun_def_set, node->name) != set_end();
    bool is_glob = node->storage_class.type != AST_CStatic_t;

    ssize_t map_it = map_find(ctx->frontend->symbol_table, node->name);
    if (map_it != map_end()) {
        Symbol* fun_symbol = pair_second(ctx->frontend->symbol_table[map_it]);
        const FunType* fun_type = &fun_symbol->type_t->get._FunType;
        if (!(fun_symbol->type_t->type == AST_FunType_t && vec_size(fun_type->param_types) == vec_size(node->params)
                && is_same_fun_type(&node->fun_type->get._FunType, fun_type))) {
            THROW_AT_TOKEN(node->line,
                GET_SEMANTIC_MSG(MSG_redecl_fun_conflict, str_fmt_name(node->name, &name_fmt),
                    str_fmt_type(node->fun_type, &type_fmt_1), str_fmt_type(fun_symbol->type_t, &type_fmt_2)));
        }
        else if (is_def && node->body) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redef_fun, str_fmt_name(node->name, &name_fmt),
                                           str_fmt_type(node->fun_type, &type_fmt_1)));
        }

        const FunAttr* fun_attrs = &fun_symbol->attrs->get._FunAttr;
        if (!is_glob && fun_attrs->is_glob) {
            THROW_AT_TOKEN(
                node->line, GET_SEMANTIC_MSG(MSG_redecl_static_conflict, str_fmt_name(node->name, &name_fmt)));
        }
        is_glob = fun_attrs->is_glob;
        free_Symbol(&fun_symbol);
    }

    if (node->body) {
        set_insert(ctx->fun_def_set, node->name);
        is_def = true;
        ctx->fun_def_name = node->name;
    }

    sptr_copy(Type, node->fun_type, glob_fun_type);
    glob_fun_attrs = make_FunAttr(is_def, is_glob);
    symbol = make_Symbol(&glob_fun_type, &glob_fun_attrs);
    map_move_add(ctx->frontend->symbol_table, node->name, symbol);
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_IdentifierAttr(&glob_fun_attrs);
    free_Symbol(&symbol);
    free_Type(&glob_fun_type);
    CATCH_EXIT;
}

static void push_static_init(Ctx ctx, shared_ptr_t(StaticInit) static_init) {
    vec_move_back(*ctx->p_static_inits, static_init);
}

static void push_zero_static_init(Ctx ctx, TLong byte) {
    if (!vec_empty(*ctx->p_static_inits) && vec_back(*ctx->p_static_inits)->type == AST_ZeroInit_t) {
        vec_back(*ctx->p_static_inits)->get._ZeroInit.byte += byte;
    }
    else {
        push_static_init(ctx, make_ZeroInit(byte));
    }
}

static error_t check_static_init(Ctx ctx, const CInitializer* node, const Type* static_init_type);

static void check_static_no_init(Ctx ctx, const Type* static_init_type, TLong size) {
    TLong byte = static_init_type == NULL ? size : get_type_scale(ctx, static_init_type) * size;
    push_zero_static_init(ctx, byte);
}

static shared_ptr_t(InitialValue) check_no_initializer(Ctx ctx, const Type* static_init_type) {
    vector_t(shared_ptr_t(StaticInit)) static_inits = vec_new();
    {
        ctx->p_static_inits = &static_inits;
        check_static_no_init(ctx, static_init_type, 1l);
        ctx->p_static_inits = NULL;
    }
    return make_Initial(&static_inits);
}

static TIdentifier make_binary_identifier(Ctx ctx, TULong binary) {
    string_t strto_binary = str_to_string(binary);
    return make_string_identifier(ctx->identifiers, &strto_binary);
}

static error_t check_static_const_init(Ctx ctx, const CConstant* node, const Type* static_init_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt = str_new(NULL);
    CATCH_ENTER;
    switch (static_init_type->type) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = get_const_char_value(node);
            if (value == 0) {
                push_zero_static_init(ctx, 1l);
            }
            else {
                push_static_init(ctx, make_CharInit(value));
            }
            break;
        }
        case AST_Int_t: {
            TInt value = get_const_int_value(node);
            if (value == 0) {
                push_zero_static_init(ctx, 4l);
            }
            else {
                push_static_init(ctx, make_IntInit(value));
            }
            break;
        }
        case AST_Long_t: {
            TLong value = get_const_long_value(node);
            if (value == 0l) {
                push_zero_static_init(ctx, 8l);
            }
            else {
                push_static_init(ctx, make_LongInit(value));
            }
            break;
        }
        case AST_Double_t: {
            TDouble value = get_const_dbl_value(node);
            TULong binary = dbl_to_binary(value);
            if (binary == 0ul) {
                push_zero_static_init(ctx, 8l);
            }
            else {
                TIdentifier dbl_const = make_binary_identifier(ctx, binary);
                push_static_init(ctx, make_DoubleInit(dbl_const));
            }
            break;
        }
        case AST_UChar_t: {
            TUChar value = get_const_uchar_value(node);
            if (value == 0u) {
                push_zero_static_init(ctx, 1l);
            }
            else {
                push_static_init(ctx, make_UCharInit(value));
            }
            break;
        }
        case AST_UInt_t: {
            TUInt value = get_const_uint_value(node);
            if (value == 0u) {
                push_zero_static_init(ctx, 4l);
            }
            else {
                push_static_init(ctx, make_UIntInit(value));
            }
            break;
        }
        case AST_ULong_t: {
            TULong value = get_const_ulong_value(node);
            if (value == 0ul) {
                push_zero_static_init(ctx, 8l);
            }
            else {
                push_static_init(ctx, make_ULongInit(value));
            }
            break;
        }
        case AST_Pointer_t: {
            switch (node->constant->type) {
                case AST_CConstChar_t:
                case AST_CConstDouble_t:
                case AST_CConstUChar_t:
                    THROW_AT_TOKEN(node->_base->line,
                        GET_SEMANTIC_MSG(MSG_static_ptr_init_not_int, str_fmt_type(static_init_type, &type_fmt),
                            get_const_fmt(node->constant)));
                default:
                    break;
            }
            TULong value = get_const_ptr_value(node);
            if (value != 0ul) {
                strto_fmt = str_to_string(value);
                THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_static_ptr_init_not_null,
                                                      str_fmt_type(static_init_type, &type_fmt), strto_fmt));
            }
            push_zero_static_init(ctx, 8l);
            break;
        }
        default:
            THROW_AT_TOKEN(node->_base->line,
                GET_SEMANTIC_MSG(MSG_agg_init_with_single, str_fmt_type(static_init_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt);
    CATCH_EXIT;
}

static error_t check_literal_string_init(Ctx ctx, const CString* node, const Pointer* static_ptr_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (static_ptr_type->ref_type->type != AST_Char_t) {
        THROW_AT_TOKEN(
            node->_base->line, GET_SEMANTIC_MSG(MSG_static_ptr_init_string, str_fmt_ptr(static_ptr_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static TIdentifier make_literal_identifier(Ctx ctx, const CStringLiteral* node) {
    string_t value = string_literal_to_const(node->value);
    return make_string_identifier(ctx->identifiers, &value);
}

static void check_static_ptr_string_init(Ctx ctx, const CString* node) {
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
    push_static_init(ctx, make_PointerInit(string_const_label));
}

static error_t check_static_arr_string_init(Ctx ctx, const CString* node, const Array* static_arr_type) {
    shared_ptr_t(CStringLiteral) literal = sptr_new();
    CATCH_ENTER;
    TLong byte;
    TRY(check_bound_string_init(ctx, node, static_arr_type));
    byte = static_arr_type->size - ((TLong)vec_size(node->literal->value)) - 1l;
    {
        bool is_null_term = byte >= 0l;
        TIdentifier string_const = make_literal_identifier(ctx, node->literal);
        sptr_copy(CStringLiteral, node->literal, literal);
        push_static_init(ctx, make_StringInit(string_const, is_null_term, &literal));
    }
    if (byte > 0l) {
        push_zero_static_init(ctx, byte);
    }
    FINALLY;
    free_CStringLiteral(&literal);
    CATCH_EXIT;
}

static error_t check_static_string_init(Ctx ctx, const CString* node, const Type* static_init_type) {
    CATCH_ENTER;
    switch (static_init_type->type) {
        case AST_Pointer_t:
            TRY(check_literal_string_init(ctx, node, &static_init_type->get._Pointer));
            check_static_ptr_string_init(ctx, node);
            break;
        case AST_Array_t:
            TRY(check_static_arr_string_init(ctx, node, &static_init_type->get._Array));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_single_static_init(Ctx ctx, const CSingleInit* node, const Type* static_init_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    switch (node->exp->type) {
        case AST_CConstant_t:
            TRY(check_static_const_init(ctx, &node->exp->get._CConstant, static_init_type));
            break;
        case AST_CString_t:
            TRY(check_static_string_init(ctx, &node->exp->get._CString, static_init_type));
            break;
        default:
            THROW_AT_TOKEN(node->exp->line,
                GET_SEMANTIC_MSG(MSG_static_init_not_const, str_fmt_type(static_init_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_static_arr_init(Ctx ctx, const CCompoundInit* node, const Array* arr_type) {
    CATCH_ENTER;
    TRY(check_bound_arr_init(ctx, node, arr_type));

    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        TRY(check_static_init(ctx, node->initializers[i], arr_type->elem_type));
    }
    if ((size_t)arr_type->size > vec_size(node->initializers)) {
        check_static_no_init(ctx, arr_type->elem_type, arr_type->size - vec_size(node->initializers));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_static_struct_init(Ctx ctx, const CCompoundInit* node, const Structure* struct_type) {
    CATCH_ENTER;
    TLong size;
    TRY(check_bound_struct_init(ctx, node, struct_type));

    size = 0l;
    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        const StructMember* member = get_struct_typedef_member(ctx->frontend, struct_type->tag, i);
        if (member->offset != size) {
            check_static_no_init(ctx, NULL, member->offset - size);
            size = member->offset;
        }
        TRY(check_static_init(ctx, node->initializers[i], member->member_type));
        size += get_type_scale(ctx, member->member_type);
    }
    size -= map_get(ctx->frontend->struct_typedef_table, struct_type->tag)->size;
    if (size != 0l) {
        check_static_no_init(ctx, NULL, -1l * size);
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_static_compound_init(Ctx ctx, const CCompoundInit* node, const Type* static_init_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    switch (static_init_type->type) {
        case AST_Array_t:
            TRY(check_static_arr_init(ctx, node, &static_init_type->get._Array));
            break;
        case AST_Structure_t:
            TRY(check_static_struct_init(ctx, node, &static_init_type->get._Structure));
            break;
        default:
            THROW_AT_TOKEN(get_compound_line(node),
                GET_SEMANTIC_MSG(MSG_scalar_init_with_compound, str_fmt_type(static_init_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_static_init(Ctx ctx, const CInitializer* node, const Type* static_init_type) {
    CATCH_ENTER;
    switch (node->type) {
        case AST_CSingleInit_t:
            TRY(check_single_static_init(ctx, &node->get._CSingleInit, static_init_type));
            break;
        case AST_CCompoundInit_t:
            TRY(check_static_compound_init(ctx, &node->get._CCompoundInit, static_init_type));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_initializer(
    Ctx ctx, const CInitializer* node, const Type* static_init_type, shared_ptr_t(InitialValue) * init_value) {
    vector_t(shared_ptr_t(StaticInit)) static_inits = vec_new();
    CATCH_ENTER;
    {
        ctx->p_static_inits = &static_inits;
        TRY(check_static_init(ctx, node, static_init_type));
        ctx->p_static_inits = NULL;
    }
    *init_value = make_Initial(&static_inits);
    FINALLY;
    for (size_t i = 0; i < vec_size(static_inits); ++i) {
        free_StaticInit(&static_inits[i]);
    }
    vec_delete(static_inits);
    CATCH_EXIT;
}

static error_t check_file_var_decl(Ctx ctx, const CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    unique_ptr_t(IdentifierAttr) glob_var_attrs = uptr_new();
    unique_ptr_t(Symbol) symbol = uptr_new();
    shared_ptr_t(InitialValue) init_value = sptr_new();
    shared_ptr_t(Type) glob_var_type = sptr_new();
    CATCH_ENTER;
    bool is_glob;
    ssize_t map_it;
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, node->var_type));
    if (node->var_type->type == AST_Void_t) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_void_var_decl, str_fmt_name(node->name, &name_fmt)));
    }
    TRY(is_valid_type(ctx, node->var_type));

    is_glob = node->storage_class.type != AST_CStatic_t;

    if (node->init) {
        if (node->var_type->type == AST_Structure_t && !is_struct_complete(ctx, &node->var_type->get._Structure)) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_incomplete_var_decl, str_fmt_name(node->name, &name_fmt),
                                           str_fmt_type(node->var_type, &type_fmt_1)));
        }
        TRY(check_initializer(ctx, node->init, node->var_type, &init_value));
    }
    else {
        if (node->storage_class.type == AST_CExtern_t) {
            init_value = make_NoInitializer();
        }
        else {
            if (node->var_type->type == AST_Structure_t && !is_struct_complete(ctx, &node->var_type->get._Structure)) {
                THROW_AT_TOKEN(
                    node->line, GET_SEMANTIC_MSG(MSG_incomplete_var_decl, str_fmt_name(node->name, &name_fmt),
                                    str_fmt_type(node->var_type, &type_fmt_1)));
            }
            init_value = make_Tentative();
        }
    }

    map_it = map_find(ctx->frontend->symbol_table, node->name);
    if (map_it != map_end()) {
        Symbol* var_symbol = pair_second(ctx->frontend->symbol_table[map_it]);
        if (!is_same_type(var_symbol->type_t, node->var_type)) {
            THROW_AT_TOKEN(node->line,
                GET_SEMANTIC_MSG(MSG_redecl_var_conflict, str_fmt_name(node->name, &name_fmt),
                    str_fmt_type(node->var_type, &type_fmt_1), str_fmt_type(var_symbol->type_t, &type_fmt_2)));
        }

        const StaticAttr* var_attrs = &var_symbol->attrs->get._StaticAttr;
        if (node->storage_class.type == AST_CExtern_t) {
            is_glob = var_attrs->is_glob;
        }
        else if (is_glob != var_attrs->is_glob) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redecl_var_storage, str_fmt_name(node->name, &name_fmt)));
        }

        if (var_attrs->init->type == AST_Initial_t) {
            if (init_value->type == AST_Initial_t) {
                THROW_AT_TOKEN(
                    node->line, GET_SEMANTIC_MSG(MSG_redecl_var_storage, str_fmt_name(node->name, &name_fmt)));
            }
            else {
                sptr_copy(InitialValue, var_attrs->init, init_value);
            }
        }
        free_Symbol(&var_symbol);
    }

    sptr_copy(Type, node->var_type, glob_var_type);
    glob_var_attrs = make_StaticAttr(is_glob, &init_value);
    symbol = make_Symbol(&glob_var_type, &glob_var_attrs);
    map_move_add(ctx->frontend->symbol_table, node->name, symbol);
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_IdentifierAttr(&glob_var_attrs);
    free_Symbol(&symbol);
    free_InitialValue(&init_value);
    free_Type(&glob_var_type);
    CATCH_EXIT;
}

static error_t check_extern_block_var_decl(Ctx ctx, const CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    unique_ptr_t(IdentifierAttr) local_var_attrs = uptr_new();
    unique_ptr_t(Symbol) symbol = uptr_new();
    shared_ptr_t(InitialValue) init_value = sptr_new();
    shared_ptr_t(Type) local_var_type = sptr_new();
    CATCH_ENTER;
    ssize_t map_it;
    if (node->init) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redef_extern_var, str_fmt_name(node->name, &name_fmt)));
    }
    map_it = map_find(ctx->frontend->symbol_table, node->name);
    if (map_it != map_end()) {
        const Type* var_type = pair_second(ctx->frontend->symbol_table[map_it])->type_t;
        if (!is_same_type(var_type, node->var_type)) {
            THROW_AT_TOKEN(
                node->line, GET_SEMANTIC_MSG(MSG_redecl_var_conflict, str_fmt_name(node->name, &name_fmt),
                                str_fmt_type(node->var_type, &type_fmt_1), str_fmt_type(var_type, &type_fmt_2)));
        }
        EARLY_EXIT;
    }

    sptr_copy(Type, node->var_type, local_var_type);
    init_value = make_NoInitializer();
    local_var_attrs = make_StaticAttr(true, &init_value);
    symbol = make_Symbol(&local_var_type, &local_var_attrs);
    map_move_add(ctx->frontend->symbol_table, node->name, symbol);
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    free_IdentifierAttr(&local_var_attrs);
    free_Symbol(&symbol);
    free_InitialValue(&init_value);
    free_Type(&local_var_type);
    CATCH_EXIT;
}

static error_t check_static_block_var_decl(Ctx ctx, const CVariableDeclaration* node) {
    unique_ptr_t(IdentifierAttr) local_var_attrs = uptr_new();
    unique_ptr_t(Symbol) symbol = uptr_new();
    shared_ptr_t(InitialValue) init_value = sptr_new();
    shared_ptr_t(Type) local_var_type = sptr_new();
    CATCH_ENTER;
    THROW_ABORT_IF(
        node->var_type->type == AST_Structure_t && !is_struct_complete(ctx, &node->var_type->get._Structure));

    if (node->init) {
        TRY(check_initializer(ctx, node->init, node->var_type, &init_value));
    }
    else {
        init_value = check_no_initializer(ctx, node->var_type);
    }

    sptr_copy(Type, node->var_type, local_var_type);
    local_var_attrs = make_StaticAttr(false, &init_value);
    THROW_ABORT_IF(map_find(ctx->frontend->symbol_table, node->name) != map_end());
    symbol = make_Symbol(&local_var_type, &local_var_attrs);
    map_move_add(ctx->frontend->symbol_table, node->name, symbol);
    FINALLY;
    free_IdentifierAttr(&local_var_attrs);
    free_Symbol(&symbol);
    free_InitialValue(&init_value);
    free_Type(&local_var_type);
    CATCH_EXIT;
}

static error_t check_auto_block_var_decl(Ctx ctx, const CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    unique_ptr_t(IdentifierAttr) local_var_attrs = uptr_new();
    unique_ptr_t(Symbol) symbol = uptr_new();
    shared_ptr_t(Type) local_var_type = sptr_new();
    CATCH_ENTER;
    if (node->var_type->type == AST_Structure_t && !is_struct_complete(ctx, &node->var_type->get._Structure)) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_incomplete_var_decl, str_fmt_name(node->name, &name_fmt),
                                       str_fmt_type(node->var_type, &type_fmt)));
    }

    sptr_copy(Type, node->var_type, local_var_type);
    local_var_attrs = make_LocalAttr();
    THROW_ABORT_IF(map_find(ctx->frontend->symbol_table, node->name) != map_end());
    symbol = make_Symbol(&local_var_type, &local_var_attrs);
    map_move_add(ctx->frontend->symbol_table, node->name, symbol);
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    free_IdentifierAttr(&local_var_attrs);
    free_Symbol(&symbol);
    free_Type(&local_var_type);
    CATCH_EXIT;
}

static error_t check_block_var_decl(Ctx ctx, const CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, node->var_type));
    if (node->var_type->type == AST_Void_t) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_void_var_decl, str_fmt_name(node->name, &name_fmt)));
    }
    TRY(is_valid_type(ctx, node->var_type));

    switch (node->storage_class.type) {
        case AST_CStorageClass_t:
            TRY(check_auto_block_var_decl(ctx, node));
            break;
        case AST_CExtern_t:
            TRY(check_extern_block_var_decl(ctx, node));
            break;
        case AST_CStatic_t:
            TRY(check_static_block_var_decl(ctx, node));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t check_struct_members_decl(Ctx ctx, const CStructDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t struct_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = 0; i < vec_size(node->members); ++i) {
        for (size_t j = i + 1; j < vec_size(node->members); ++j) {
            if (node->members[i]->member_name == node->members[j]->member_name) {
                THROW_AT_TOKEN(node->members[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_member_decl,
                                                           str_fmt_struct_name(node->tag, node->is_union, &struct_fmt),
                                                           str_fmt_name(node->members[i]->member_name, &name_fmt)));
            }
        }
        THROW_ABORT_IF(node->members[i]->member_type->type == AST_FunType_t);
        ctx->errors->linebuf = node->members[i]->line;
        TRY(reslv_struct_type(ctx, node->members[i]->member_type));
        if (!is_type_complete(ctx, node->members[i]->member_type)) {
            THROW_AT_TOKEN(node->members[i]->line, GET_SEMANTIC_MSG(MSG_incomplete_member_decl,
                                                       str_fmt_struct_name(node->tag, node->is_union, &struct_fmt),
                                                       str_fmt_name(node->members[i]->member_name, &name_fmt),
                                                       str_fmt_type(node->members[i]->member_type, &type_fmt)));
        }
        TRY(is_valid_type(ctx, node->members[i]->member_type));
    }
    FINALLY;
    str_delete(name_fmt);
    str_delete(struct_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_struct_decl(Ctx ctx, const CStructDeclaration* node) {
    string_t struct_fmt = str_new(NULL);
    unique_ptr_t(StructMember) struct_member = uptr_new();
    unique_ptr_t(StructTypedef) struct_typedef = uptr_new();
    shared_ptr_t(Type) member_type = sptr_new();
    vector_t(TIdentifier) member_names = vec_new();
    hashmap_t(TIdentifier, UPtrStructMember) members = map_new();
    CATCH_ENTER;
    TInt alignment;
    TLong size;
    if (map_find(ctx->frontend->struct_typedef_table, node->tag) != map_end()) {
        THROW_AT_TOKEN(node->line,
            GET_SEMANTIC_MSG(MSG_redecl_struct_in_scope, str_fmt_struct_name(node->tag, node->is_union, &struct_fmt)));
    }
    alignment = 0;
    size = 0l;
    vec_reserve(member_names, vec_size(node->members));
    for (size_t i = 0; i < vec_size(node->members); ++i) {
        {
            TIdentifier name = node->members[i]->member_name;
            vec_push_back(member_names, name);
        }
        TInt member_alignment = get_type_alignment(ctx, node->members[i]->member_type);
        TLong member_size = get_type_scale(ctx, node->members[i]->member_type);
        {
            TLong offset = 0l;
            if (node->is_union) {
                if (size < member_size) {
                    size = member_size;
                }
            }
            else {
                offset = size % member_alignment;
                if (offset != 0l) {
                    size += member_alignment - offset;
                }
                offset = size;
                size += member_size;
            }
            sptr_copy(Type, node->members[i]->member_type, member_type);

            THROW_ABORT_IF(map_find(members, vec_back(member_names)) != map_end());
            struct_member = make_StructMember(offset, &member_type);
            map_move_add(members, vec_back(member_names), struct_member);
        }
        if (alignment < member_alignment) {
            alignment = member_alignment;
        }
    }
    {
        TLong offset = size % alignment;
        if (offset != 0l) {
            size += alignment - offset;
        }
    }
    struct_typedef = make_StructTypedef(alignment, size, &member_names, &members);
    map_move_add(ctx->frontend->struct_typedef_table, node->tag, struct_typedef);
    FINALLY;
    str_delete(struct_fmt);
    free_StructMember(&struct_member);
    free_StructTypedef(&struct_typedef);
    free_Type(&member_type);
    vec_delete(member_names);
    for (size_t i = 0; i < map_size(members); ++i) {
        free_StructMember(&pair_second(members[i]));
    }
    map_delete(members);
    CATCH_EXIT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loop labeling

static error_t annotate_goto_label(Ctx ctx, const CLabel* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (set_find(ctx->label_set, node->target) != set_end()) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redef_label_in_scope, str_fmt_name(node->target, &name_fmt)));
    }
    set_insert(ctx->label_set, node->target);
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static void annotate_while_loop(Ctx ctx, CWhile* node) {
    node->target = repr_label_identifier(ctx->identifiers, LBL_Lwhile);
    vec_push_back(ctx->break_loop_labels, node->target);
    vec_push_back(ctx->continue_loop_labels, node->target);
}

static void annotate_do_while_loop(Ctx ctx, CDoWhile* node) {
    node->target = repr_label_identifier(ctx->identifiers, LBL_Ldo_while);
    vec_push_back(ctx->break_loop_labels, node->target);
    vec_push_back(ctx->continue_loop_labels, node->target);
}

static void annotate_for_loop(Ctx ctx, CFor* node) {
    node->target = repr_label_identifier(ctx->identifiers, LBL_Lfor);
    vec_push_back(ctx->break_loop_labels, node->target);
    vec_push_back(ctx->continue_loop_labels, node->target);
}

static void annotate_switch_lookup(Ctx ctx, CSwitch* node) {
    node->is_default = false;
    node->target = repr_label_identifier(ctx->identifiers, LBL_Lswitch);
    vec_push_back(ctx->break_loop_labels, node->target);
}

static error_t annotate_case_jump(Ctx ctx, CCase* node) {
    CATCH_ENTER;
    if (!ctx->p_switch_statement) {
        THROW_AT_TOKEN(node->value->line, GET_SEMANTIC_MSG_0(MSG_case_out_of_switch));
    }
    node->target = repr_case_identifier(
        ctx->identifiers, ctx->p_switch_statement->target, false, vec_size(ctx->p_switch_statement->cases));
    FINALLY;
    CATCH_EXIT;
}

static error_t annotate_default_jump(Ctx ctx, CDefault* node) {
    CATCH_ENTER;
    if (!ctx->p_switch_statement) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG_0(MSG_default_out_of_switch));
    }
    else if (ctx->p_switch_statement->is_default) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG_0(MSG_multiple_default));
    }
    node->target = ctx->p_switch_statement->target;
    ctx->p_switch_statement->is_default = true;
    FINALLY;
    CATCH_EXIT;
}

static error_t annotate_break_jump(Ctx ctx, CBreak* node) {
    CATCH_ENTER;
    if (vec_empty(ctx->break_loop_labels)) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG_0(MSG_break_out_of_loop));
    }
    node->target = vec_back(ctx->break_loop_labels);
    FINALLY;
    CATCH_EXIT;
}

static error_t annotate_continue_jump(Ctx ctx, CContinue* node) {
    CATCH_ENTER;
    if (vec_empty(ctx->continue_loop_labels)) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG_0(MSG_continue_out_of_loop));
    }
    node->target = vec_back(ctx->continue_loop_labels);
    FINALLY;
    CATCH_EXIT;
}

static void deannotate_loop(Ctx ctx) {
    vec_pop_back(ctx->break_loop_labels);
    vec_pop_back(ctx->continue_loop_labels);
}

static void deannotate_lookup(Ctx ctx) { vec_pop_back(ctx->break_loop_labels); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifier resolution

static bool is_file_scope(Ctx ctx) { return vec_size(ctx->scoped_identifier_maps) == 1; }

static void enter_scope(Ctx ctx) {
    vec_push_back(ctx->scoped_identifier_maps, map_new());
    vec_push_back(ctx->scoped_struct_maps, map_new());
}

static void exit_scope(Ctx ctx) {
    for (size_t i = 0; i < map_size(vec_back(ctx->scoped_identifier_maps)); ++i) {
        TIdentifier identifier = pair_first(vec_back(ctx->scoped_identifier_maps)[i]);
        ssize_t map_it = map_find(ctx->extern_scope_map, identifier);
        if (map_it != map_end()
            && pair_second(ctx->extern_scope_map[map_it]) == vec_size(ctx->scoped_identifier_maps)) {
            map_erase(ctx->extern_scope_map, identifier);
        }
    }
    map_delete(vec_back(ctx->scoped_identifier_maps));
    vec_pop_back(ctx->scoped_identifier_maps);
    map_delete(vec_back(ctx->scoped_struct_maps));
    vec_pop_back(ctx->scoped_struct_maps);
}

static error_t reslv_label(Ctx ctx, const CFunctionDeclaration* node) {
    string_t name_fmt_1 = str_new(NULL);
    string_t name_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = 0; i < map_size(ctx->goto_map); ++i) {
        if (set_find(ctx->label_set, pair_first(ctx->goto_map[i])) == set_end()) {
            THROW_AT_TOKEN(map_get(ctx->errors->linebuf_map, pair_second(ctx->goto_map[i])),
                GET_SEMANTIC_MSG(MSG_undef_goto_target, str_fmt_name(pair_first(ctx->goto_map[i]), &name_fmt_1),
                    str_fmt_name(node->name, &name_fmt_2)));
        }
    }
    FINALLY;
    str_delete(name_fmt_1);
    str_delete(name_fmt_2);
    CATCH_EXIT;
}

static error_t reslv_ptr_struct(Ctx ctx, const Pointer* ptr_type) {
    CATCH_ENTER;
    TRY(reslv_struct_type(ctx, ptr_type->ref_type));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_arr_struct(Ctx ctx, const Array* arr_type) {
    CATCH_ENTER;
    TRY(reslv_struct_type(ctx, arr_type->elem_type));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct(Ctx ctx, Structure* struct_type) {
    string_t struct_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (struct_type->is_union) {
        if (set_find(ctx->union_def_set, struct_type->tag) != set_end()) {
            EARLY_EXIT;
        }
    }
    else if (set_find(ctx->struct_def_set, struct_type->tag) != set_end()) {
        EARLY_EXIT;
    }
    for (size_t i = vec_size(ctx->scoped_identifier_maps); i-- > 0;) {
        ssize_t map_it = map_find(ctx->scoped_struct_maps[i], struct_type->tag);
        if (map_it != map_end()) {
            const Structure* structure = &pair_second(ctx->scoped_struct_maps[i][map_it]);
            if (structure->is_union != struct_type->is_union) {
                THROW_AT_TOKEN(ctx->errors->linebuf,
                    GET_SEMANTIC_MSG(MSG_redecl_struct_conflict, str_fmt_struct(struct_type, &type_fmt),
                        str_fmt_struct_name(struct_type->tag, !struct_type->is_union, &struct_fmt)));
            }
            struct_type->tag = structure->tag;
            EARLY_EXIT;
        }
    }
    THROW_AT_TOKEN(
        ctx->errors->linebuf, GET_SEMANTIC_MSG(MSG_undef_struct_in_scope, str_fmt_struct(struct_type, &type_fmt)));
    FINALLY;
    str_delete(struct_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t reslv_struct_type(Ctx ctx, Type* type) {
    CATCH_ENTER;
    switch (type->type) {
        case AST_Pointer_t:
            TRY(reslv_ptr_struct(ctx, &type->get._Pointer));
            break;
        case AST_Array_t:
            TRY(reslv_arr_struct(ctx, &type->get._Array));
            break;
        case AST_Structure_t:
            TRY(reslv_struct(ctx, &type->get._Structure));
            break;
        case AST_FunType_t:
            THROW_ABORT;
        default:
            break;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_exp(Ctx ctx, CExp* node);
static error_t reslv_typed_exp(Ctx ctx, unique_ptr_t(CExp) * exp);

static void reslv_const_exp(const CConstant* node) { check_const_exp(node); }

static void reslv_string_exp(const CString* node) { check_string_exp(node); }

static error_t reslv_var_exp(Ctx ctx, CVar* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = vec_size(ctx->scoped_identifier_maps); i-- > 0;) {
        ssize_t map_it = map_find(ctx->scoped_identifier_maps[i], node->name);
        if (map_it != map_end()) {
            node->name = pair_second(ctx->scoped_identifier_maps[i][map_it]);
            goto Lelse;
        }
    }
    THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_undecl_var_in_scope, str_fmt_name(node->name, &name_fmt)));
Lelse:

    TRY(check_var_exp(ctx, node));
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_cast_exp(Ctx ctx, CCast* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->exp));
    TRY(check_cast_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_unary_exp(Ctx ctx, CUnary* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->exp));
    TRY(check_unary_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_binary_exp(Ctx ctx, CBinary* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->exp_left));
    TRY(reslv_typed_exp(ctx, &node->exp_right));
    TRY(check_binary_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_assign_exp(Ctx ctx, CAssignment* node) {
    CATCH_ENTER;
    if (node->exp_left) {
        TRY(reslv_typed_exp(ctx, &node->exp_left));
    }
    TRY(reslv_typed_exp(ctx, &node->exp_right));
    TRY(check_assign_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_conditional_exp(Ctx ctx, CConditional* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->condition));
    TRY(reslv_typed_exp(ctx, &node->exp_middle));
    TRY(reslv_typed_exp(ctx, &node->exp_right));
    TRY(check_conditional_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_call_exp(Ctx ctx, CFunctionCall* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = vec_size(ctx->scoped_identifier_maps); i-- > 0;) {
        ssize_t map_it = map_find(ctx->scoped_identifier_maps[i], node->name);
        if (map_it != map_end()) {
            node->name = pair_second(ctx->scoped_identifier_maps[i][map_it]);
            goto Lelse;
        }
    }
    THROW_AT_TOKEN(node->_base->line, GET_SEMANTIC_MSG(MSG_undecl_fun_in_scope, str_fmt_name(node->name, &name_fmt)));
Lelse:

    for (size_t i = 0; i < vec_size(node->args); ++i) {
        TRY(reslv_typed_exp(ctx, &node->args[i]));
    }
    TRY(check_call_exp(ctx, node));
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_deref_exp(Ctx ctx, CDereference* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->exp));
    TRY(check_deref_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_addrof_expr(Ctx ctx, const CAddrOf* node) {
    CATCH_ENTER;
    TRY(reslv_exp(ctx, node->exp));
    TRY(check_addrof_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_subscript_exp(Ctx ctx, CSubscript* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->primary_exp));
    TRY(reslv_typed_exp(ctx, &node->subscript_exp));
    TRY(check_subscript_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_sizeof_exp(Ctx ctx, const CSizeOf* node) {
    CATCH_ENTER;
    TRY(reslv_exp(ctx, node->exp));
    TRY(check_sizeof_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_sizeoft_exp(Ctx ctx, const CSizeOfT* node) {
    CATCH_ENTER;
    TRY(check_sizeoft_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_dot_exp(Ctx ctx, CDot* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->structure));
    TRY(check_dot_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_arrow_exp(Ctx ctx, CArrow* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->pointer));
    TRY(check_arrow_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_exp(Ctx ctx, CExp* node) {
    CATCH_ENTER;
    switch (node->type) {
        case AST_CConstant_t:
            reslv_const_exp(&node->get._CConstant);
            break;
        case AST_CString_t:
            reslv_string_exp(&node->get._CString);
            break;
        case AST_CVar_t:
            TRY(reslv_var_exp(ctx, &node->get._CVar));
            break;
        case AST_CCast_t:
            TRY(reslv_cast_exp(ctx, &node->get._CCast));
            break;
        case AST_CUnary_t:
            TRY(reslv_unary_exp(ctx, &node->get._CUnary));
            break;
        case AST_CBinary_t:
            TRY(reslv_binary_exp(ctx, &node->get._CBinary));
            break;
        case AST_CAssignment_t:
            TRY(reslv_assign_exp(ctx, &node->get._CAssignment));
            break;
        case AST_CConditional_t:
            TRY(reslv_conditional_exp(ctx, &node->get._CConditional));
            break;
        case AST_CFunctionCall_t:
            TRY(reslv_call_exp(ctx, &node->get._CFunctionCall));
            break;
        case AST_CDereference_t:
            TRY(reslv_deref_exp(ctx, &node->get._CDereference));
            break;
        case AST_CAddrOf_t:
            TRY(reslv_addrof_expr(ctx, &node->get._CAddrOf));
            break;
        case AST_CSubscript_t:
            TRY(reslv_subscript_exp(ctx, &node->get._CSubscript));
            break;
        case AST_CSizeOf_t:
            TRY(reslv_sizeof_exp(ctx, &node->get._CSizeOf));
            break;
        case AST_CSizeOfT_t:
            TRY(reslv_sizeoft_exp(ctx, &node->get._CSizeOfT));
            break;
        case AST_CDot_t:
            TRY(reslv_dot_exp(ctx, &node->get._CDot));
            break;
        case AST_CArrow_t:
            TRY(reslv_arrow_exp(ctx, &node->get._CArrow));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_typed_exp(Ctx ctx, unique_ptr_t(CExp) * exp) {
    CATCH_ENTER;
    TRY(reslv_exp(ctx, *exp));
    TRY(check_typed_exp(ctx, exp));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_block(Ctx ctx, const CBlock* node);
static error_t reslv_block_var_decl(Ctx ctx, CVariableDeclaration* node);

static error_t reslv_statement(Ctx ctx, CStatement* node);

static error_t reslv_for_init_decl(Ctx ctx, const CInitDecl* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->init->storage_class.type != AST_CStorageClass_t) {
        THROW_AT_TOKEN(
            node->init->line, GET_SEMANTIC_MSG(MSG_for_init_decl_not_auto, str_fmt_name(node->init->name, &name_fmt),
                                  get_storage_class_fmt(&node->init->storage_class)));
    }
    TRY(reslv_block_var_decl(ctx, node->init));
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_for_init_exp(Ctx ctx, CInitExp* node) {
    CATCH_ENTER;
    if (node->init) {
        TRY(reslv_typed_exp(ctx, &node->init));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_for_init(Ctx ctx, CForInit* node) {
    CATCH_ENTER;
    switch (node->type) {
        case AST_CInitDecl_t:
            TRY(reslv_for_init_decl(ctx, &node->get._CInitDecl));
            break;
        case AST_CInitExp_t: {
            TRY(reslv_for_init_exp(ctx, &node->get._CInitExp));
            break;
        }
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_ret_statement(Ctx ctx, CReturn* node) {
    CATCH_ENTER;
    if (node->exp) {
        TRY(reslv_typed_exp(ctx, &node->exp));
    }
    TRY(check_ret_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_exp_statement(Ctx ctx, CExpression* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->exp));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_if_statement(Ctx ctx, CIf* node) {
    CATCH_ENTER;
    TRY(reslv_typed_exp(ctx, &node->condition));
    TRY(reslv_statement(ctx, node->then));
    if (node->else_fi) {
        TRY(reslv_statement(ctx, node->else_fi));
    }
    TRY(check_if_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static void reslv_goto_statement(Ctx ctx, CGoto* node) {
    ssize_t map_it = map_find(ctx->goto_map, node->target);
    if (map_it != map_end()) {
        node->target = pair_second(ctx->goto_map[map_it]);
        map_add(ctx->errors->linebuf_map, node->target, node->line);
    }
    else {
        TIdentifier target = rslv_label_identifier(ctx->identifiers, node->target);
        map_add(ctx->goto_map, node->target, target);
        node->target = target;
        map_add(ctx->errors->linebuf_map, node->target, node->line);
    }
}

static error_t reslv_label_statement(Ctx ctx, CLabel* node) {
    CATCH_ENTER;
    ssize_t map_it;
    TRY(annotate_goto_label(ctx, node));
    map_it = map_find(ctx->goto_map, node->target);
    if (map_it != map_end()) {
        node->target = pair_second(ctx->goto_map[map_it]);
    }
    else {
        TIdentifier target = rslv_label_identifier(ctx->identifiers, node->target);
        map_add(ctx->goto_map, node->target, target);
        node->target = target;
    }
    TRY(reslv_statement(ctx, node->jump_to));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_compound_statement(Ctx ctx, const CCompound* node) {
    CATCH_ENTER;
    enter_scope(ctx);
    TRY(reslv_block(ctx, node->block));
    exit_scope(ctx);
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_while_statement(Ctx ctx, CWhile* node) {
    CATCH_ENTER;
    annotate_while_loop(ctx, node);
    TRY(reslv_typed_exp(ctx, &node->condition));
    TRY(reslv_statement(ctx, node->body));
    deannotate_loop(ctx);
    TRY(check_while_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_do_while_statement(Ctx ctx, CDoWhile* node) {
    CATCH_ENTER;
    annotate_do_while_loop(ctx, node);
    TRY(reslv_statement(ctx, node->body));
    TRY(reslv_typed_exp(ctx, &node->condition));
    deannotate_loop(ctx);
    TRY(check_do_while_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_for_statement(Ctx ctx, CFor* node) {
    CATCH_ENTER;
    annotate_for_loop(ctx, node);
    enter_scope(ctx);
    TRY(reslv_for_init(ctx, node->init));
    if (node->condition) {
        TRY(reslv_typed_exp(ctx, &node->condition));
    }
    if (node->post) {
        TRY(reslv_typed_exp(ctx, &node->post));
    }
    TRY(reslv_statement(ctx, node->body));
    exit_scope(ctx);
    deannotate_loop(ctx);
    TRY(check_for_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_switch_statement(Ctx ctx, CSwitch* node) {
    CATCH_ENTER;
    annotate_switch_lookup(ctx, node);
    enter_scope(ctx);
    TRY(reslv_typed_exp(ctx, &node->match));
    {
        CSwitch* p_switch_statement = ctx->p_switch_statement;
        ctx->p_switch_statement = node;
        TRY(reslv_statement(ctx, node->body));
        ctx->p_switch_statement = p_switch_statement;
    }
    exit_scope(ctx);
    deannotate_lookup(ctx);
    TRY(check_switch_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_case_statement(Ctx ctx, CCase* node) {
    CATCH_ENTER;
    TRY(annotate_case_jump(ctx, node));
    TRY(reslv_typed_exp(ctx, &node->value));
    vec_move_back(ctx->p_switch_statement->cases, node->value);
    TRY(reslv_statement(ctx, node->jump_to));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_default_statement(Ctx ctx, CDefault* node) {
    CATCH_ENTER;
    TRY(annotate_default_jump(ctx, node));
    TRY(reslv_statement(ctx, node->jump_to));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_break_statement(Ctx ctx, CBreak* node) {
    CATCH_ENTER;
    TRY(annotate_break_jump(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_continue_statement(Ctx ctx, CContinue* node) {
    CATCH_ENTER;
    TRY(annotate_continue_jump(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_statement(Ctx ctx, CStatement* node) {
    CATCH_ENTER;
    switch (node->type) {
        case AST_CReturn_t:
            TRY(reslv_ret_statement(ctx, &node->get._CReturn));
            break;
        case AST_CExpression_t:
            TRY(reslv_exp_statement(ctx, &node->get._CExpression));
            break;
        case AST_CIf_t:
            TRY(reslv_if_statement(ctx, &node->get._CIf));
            break;
        case AST_CGoto_t:
            reslv_goto_statement(ctx, &node->get._CGoto);
            break;
        case AST_CLabel_t:
            TRY(reslv_label_statement(ctx, &node->get._CLabel));
            break;
        case AST_CCompound_t:
            TRY(reslv_compound_statement(ctx, &node->get._CCompound));
            break;
        case AST_CWhile_t:
            TRY(reslv_while_statement(ctx, &node->get._CWhile));
            break;
        case AST_CDoWhile_t:
            TRY(reslv_do_while_statement(ctx, &node->get._CDoWhile));
            break;
        case AST_CFor_t:
            TRY(reslv_for_statement(ctx, &node->get._CFor));
            break;
        case AST_CSwitch_t:
            TRY(reslv_switch_statement(ctx, &node->get._CSwitch));
            break;
        case AST_CCase_t:
            TRY(reslv_case_statement(ctx, &node->get._CCase));
            break;
        case AST_CDefault_t:
            TRY(reslv_default_statement(ctx, &node->get._CDefault));
            break;
        case AST_CBreak_t:
            TRY(reslv_break_statement(ctx, &node->get._CBreak));
            break;
        case AST_CContinue_t:
            TRY(reslv_continue_statement(ctx, &node->get._CContinue));
            break;
        case AST_CNull_t:
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_declaration(Ctx ctx, const CDeclaration* node);

static error_t reslv_block_items(Ctx ctx, vector_t(unique_ptr_t(CBlockItem)) node_list) {
    CATCH_ENTER;
    for (size_t i = 0; i < vec_size(node_list); ++i) {
        switch (node_list[i]->type) {
            case AST_CS_t:
                TRY(reslv_statement(ctx, node_list[i]->get._CS.statement));
                break;
            case AST_CD_t:
                TRY(reslv_declaration(ctx, node_list[i]->get._CD.declaration));
                break;
            default:
                THROW_ABORT;
        }
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_block(Ctx ctx, const CBlock* node) {
    CATCH_ENTER;
    THROW_ABORT_IF(node->type != AST_CB_t);
    TRY(reslv_block_items(ctx, node->get._CB.block_items));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_initializer(Ctx ctx, CInitializer* node, shared_ptr_t(Type) * init_type);

static error_t reslv_single_init(Ctx ctx, CSingleInit* node, shared_ptr_t(Type) * init_type) {
    CATCH_ENTER;
    if (node->exp->type == AST_CString_t && (*init_type)->type == AST_Array_t) {
        TRY(check_bound_string_init(ctx, &node->exp->get._CString, &(*init_type)->get._Array));
        check_string_init(node, init_type);
    }
    else {
        TRY(reslv_typed_exp(ctx, &node->exp));
        TRY(check_single_init(ctx, node, init_type));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_arr_init(Ctx ctx, CCompoundInit* node, Array* arr_type, shared_ptr_t(Type) * init_type) {
    CATCH_ENTER;
    TRY(check_bound_arr_init(ctx, node, arr_type));

    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        TRY(reslv_initializer(ctx, node->initializers[i], &arr_type->elem_type));
    }
    check_arr_init(ctx, node, arr_type, init_type);
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct_init(
    Ctx ctx, CCompoundInit* node, const Structure* struct_type, shared_ptr_t(Type) * init_type) {
    CATCH_ENTER;
    TRY(check_bound_struct_init(ctx, node, struct_type));

    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        StructMember* member = get_struct_typedef_member(ctx->frontend, struct_type->tag, i);
        TRY(reslv_initializer(ctx, node->initializers[i], &member->member_type));
    }
    check_struct_init(ctx, node, struct_type, init_type);
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_compound_init(Ctx ctx, CCompoundInit* node, shared_ptr_t(Type) * init_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    switch ((*init_type)->type) {
        case AST_Array_t:
            TRY(reslv_arr_init(ctx, node, &(*init_type)->get._Array, init_type));
            break;
        case AST_Structure_t:
            TRY(reslv_struct_init(ctx, node, &(*init_type)->get._Structure, init_type));
            break;
        default:
            THROW_AT_TOKEN(get_compound_line(node),
                GET_SEMANTIC_MSG(MSG_scalar_init_with_compound, str_fmt_type(*init_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t reslv_initializer(Ctx ctx, CInitializer* node, shared_ptr_t(Type) * init_type) {
    CATCH_ENTER;
    switch (node->type) {
        case AST_CSingleInit_t:
            TRY(reslv_single_init(ctx, &node->get._CSingleInit, init_type));
            break;
        case AST_CCompoundInit_t:
            TRY(reslv_compound_init(ctx, &node->get._CCompoundInit, init_type));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_fun_params_decl(Ctx ctx, const CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = 0; i < vec_size(node->params); ++i) {
        TIdentifier param = node->params[i];
        if (map_find(vec_back(ctx->scoped_identifier_maps), param) != map_end()) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redecl_var_in_scope, str_fmt_name(param, &name_fmt)));
        }
        param = rslv_var_identifier(ctx->identifiers, param);
        map_add(vec_back(ctx->scoped_identifier_maps), node->params[i], param);
        node->params[i] = param;
    }
    TRY(check_fun_params_decl(ctx, node));
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_fun_declaration(Ctx ctx, const CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_file_scope(ctx)) {
        if (node->body) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_def_nested_fun, str_fmt_name(node->name, &name_fmt)));
        }
        else if (node->storage_class.type == AST_CStatic_t) {
            THROW_AT_TOKEN(
                node->line, GET_SEMANTIC_MSG(MSG_decl_nested_static_fun, str_fmt_name(node->name, &name_fmt)));
        }
    }

    if (map_find(ctx->extern_scope_map, node->name) == map_end()) {
        if (map_find(vec_back(ctx->scoped_identifier_maps), node->name) != map_end()) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redecl_fun_in_scope, str_fmt_name(node->name, &name_fmt)));
        }
        map_add(ctx->extern_scope_map, node->name, vec_size(ctx->scoped_identifier_maps));
    }

    map_add(vec_back(ctx->scoped_identifier_maps), node->name, node->name);
    TRY(check_ret_fun_decl(ctx, node));

    enter_scope(ctx);
    if (!vec_empty(node->params)) {
        TRY(reslv_fun_params_decl(ctx, node));
    }
    TRY(check_fun_decl(ctx, node));

    if (node->body) {
        TRY(reslv_block(ctx, node->body));
    }
    exit_scope(ctx);
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_file_var_decl(Ctx ctx, const CVariableDeclaration* node) {
    CATCH_ENTER;
    if (map_find(ctx->extern_scope_map, node->name) == map_end()) {
        map_add(ctx->extern_scope_map, node->name, vec_size(ctx->scoped_identifier_maps));
    }

    map_add(vec_back(ctx->scoped_identifier_maps), node->name, node->name);
    if (is_file_scope(ctx)) {
        TRY(check_file_var_decl(ctx, node));
    }
    else {
        TRY(check_block_var_decl(ctx, node));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_block_var_decl(Ctx ctx, CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (map_find(vec_back(ctx->scoped_identifier_maps), node->name) != map_end()
        && !(map_find(ctx->extern_scope_map, node->name) != map_end() && node->storage_class.type == AST_CExtern_t)) {
        THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redecl_var_in_scope, str_fmt_name(node->name, &name_fmt)));
    }
    else if (node->storage_class.type == AST_CExtern_t) {
        TRY(reslv_file_var_decl(ctx, node));
        EARLY_EXIT;
    }

    {
        TIdentifier name = rslv_var_identifier(ctx->identifiers, node->name);
        map_add(vec_back(ctx->scoped_identifier_maps), node->name, name);
        node->name = name;
    }
    TRY(check_block_var_decl(ctx, node));

    if (node->init && node->storage_class.type == AST_CStorageClass_t) {
        TRY(reslv_initializer(ctx, node->init, &node->var_type));
    }
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_struct_members_decl(Ctx ctx, const CStructDeclaration* node) {
    CATCH_ENTER;
    TRY(check_struct_members_decl(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct_declaration(Ctx ctx, CStructDeclaration* node) {
    string_t struct_fmt_1 = str_new(NULL);
    string_t struct_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    ssize_t map_it = map_find(vec_back(ctx->scoped_struct_maps), node->tag);
    if (map_it != map_end()) {
        node->tag = pair_second(vec_back(ctx->scoped_struct_maps)[map_it]).tag;
        if (node->is_union) {
            if (set_find(ctx->union_def_set, node->tag) == set_end()) {
                THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redecl_struct_conflict,
                                               str_fmt_struct_name(node->tag, node->is_union, &struct_fmt_1),
                                               str_fmt_struct_name(node->tag, !node->is_union, &struct_fmt_2)));
            }
        }
        else if (set_find(ctx->struct_def_set, node->tag) == set_end()) {
            THROW_AT_TOKEN(node->line, GET_SEMANTIC_MSG(MSG_redecl_struct_conflict,
                                           str_fmt_struct_name(node->tag, node->is_union, &struct_fmt_1),
                                           str_fmt_struct_name(node->tag, !node->is_union, &struct_fmt_2)));
        }
    }
    else {
        {
            Structure structure = {rslv_struct_tag(ctx->identifiers, node->tag), node->is_union};
            map_add(vec_back(ctx->scoped_struct_maps), node->tag, structure);
            node->tag = structure.tag;
        }
        if (node->is_union) {
            set_insert(ctx->union_def_set, node->tag);
        }
        else {
            set_insert(ctx->struct_def_set, node->tag);
        }
    }
    if (!vec_empty(node->members)) {
        TRY(reslv_struct_members_decl(ctx, node));
        TRY(check_struct_decl(ctx, node));
    }
    FINALLY;
    str_delete(struct_fmt_1);
    str_delete(struct_fmt_2);
    CATCH_EXIT;
}

static error_t reslv_fun_decl(Ctx ctx, const CFunDecl* node) {
    CATCH_ENTER;
    if (is_file_scope(ctx)) {
        map_clear(ctx->goto_map);
        set_clear(ctx->label_set);
        vec_clear(ctx->break_loop_labels);
        vec_clear(ctx->continue_loop_labels);
        ctx->p_switch_statement = NULL;
    }
    TRY(reslv_fun_declaration(ctx, node->fun_decl));
    if (is_file_scope(ctx)) {
        TRY(reslv_label(ctx, node->fun_decl));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_var_decl(Ctx ctx, const CVarDecl* node) {
    CATCH_ENTER;
    if (is_file_scope(ctx)) {
        TRY(reslv_file_var_decl(ctx, node->var_decl));
    }
    else {
        TRY(reslv_block_var_decl(ctx, node->var_decl));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct_decl(Ctx ctx, const CStructDecl* node) {
    CATCH_ENTER;
    TRY(reslv_struct_declaration(ctx, node->struct_decl));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_declaration(Ctx ctx, const CDeclaration* node) {
    CATCH_ENTER;
    switch (node->type) {
        case AST_CFunDecl_t:
            TRY(reslv_fun_decl(ctx, &node->get._CFunDecl));
            break;
        case AST_CVarDecl_t:
            TRY(reslv_var_decl(ctx, &node->get._CVarDecl));
            break;
        case AST_CStructDecl_t:
            TRY(reslv_struct_decl(ctx, &node->get._CStructDecl));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t resolve_program(Ctx ctx, const CProgram* node) {
    CATCH_ENTER;
    enter_scope(ctx);
    for (size_t i = 0; i < vec_size(node->declarations); ++i) {
        TRY(reslv_declaration(ctx, node->declarations[i]));
    }
    FINALLY;
    CATCH_EXIT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

error_t analyze_semantic(
    const CProgram* node, ErrorsContext* errors, FrontEndContext* frontend, IdentifierContext* identifiers) {
    SemanticContext ctx;
    {
        ctx.errors = errors;
        ctx.frontend = frontend;
        ctx.identifiers = identifiers;
        ctx.extern_scope_map = map_new();
        ctx.goto_map = map_new();
        ctx.scoped_identifier_maps = vec_new();
        ctx.scoped_struct_maps = vec_new();
        ctx.label_set = set_new();
        ctx.break_loop_labels = vec_new();
        ctx.continue_loop_labels = vec_new();
        ctx.fun_def_set = set_new();
        ctx.struct_def_set = set_new();
        ctx.union_def_set = set_new();
    }
    CATCH_ENTER;
    TRY(resolve_program(&ctx, node));

    FINALLY;
    map_delete(ctx.extern_scope_map);
    map_delete(ctx.goto_map);
    for (size_t i = 0; i < vec_size(ctx.scoped_identifier_maps); ++i) {
        map_delete(ctx.scoped_identifier_maps[i]);
    }
    vec_delete(ctx.scoped_identifier_maps);
    for (size_t i = 0; i < vec_size(ctx.scoped_struct_maps); ++i) {
        map_delete(ctx.scoped_struct_maps[i]);
    }
    vec_delete(ctx.scoped_struct_maps);
    set_delete(ctx.label_set);
    vec_delete(ctx.break_loop_labels);
    vec_delete(ctx.continue_loop_labels);
    set_delete(ctx.fun_def_set);
    set_delete(ctx.struct_def_set);
    set_delete(ctx.union_def_set);

    map_delete(errors->linebuf_map);
    for (size_t i = 0; i < vec_size(errors->fopen_lines); ++i) {
        str_delete(errors->fopen_lines[i].filename);
    }
    vec_delete(errors->fopen_lines);
    vec_delete(errors->token_infos);
    CATCH_EXIT;
}
