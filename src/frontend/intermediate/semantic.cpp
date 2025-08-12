#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "util/c_std.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/errors.hpp"

#include "frontend/intermediate/idents.hpp"
#include "frontend/intermediate/semantic.hpp"

struct SemanticContext {
    ErrorsContext* errors;
    FrontEndContext* frontend;
    IdentifierContext* identifiers;
    // Type checking
    std::unordered_map<TIdentifier, size_t> extern_scope_map;
    std::vector<std::unordered_map<TIdentifier, TIdentifier>> scoped_identifier_maps;
    std::vector<std::unordered_map<TIdentifier, Structure>> scoped_struct_maps;
    std::unordered_map<TIdentifier, TIdentifier> goto_map;
    std::unordered_set<TIdentifier> label_set;
    // Loop labeling
    std::vector<TIdentifier> break_loop_labels;
    std::vector<TIdentifier> continue_loop_labels;
    // Identifier resolution
    TIdentifier fun_def_name;
    CSwitch* p_switch_statement;
    std::unordered_set<TIdentifier> fun_def_set;
    std::unordered_set<TIdentifier> struct_def_set;
    std::unordered_set<TIdentifier> union_def_set;
    vector_t(std::shared_ptr<StaticInit>) * p_static_inits;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

typedef SemanticContext* Ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Type checking

static bool is_same_type(Type* type_1, Type* type_2);

static bool is_same_ptr(Pointer* ptr_type_1, Pointer* ptr_type_2) {
    return is_same_type(ptr_type_1->ref_type.get(), ptr_type_2->ref_type.get());
}

static bool is_same_arr(Array* arr_type_1, Array* arr_type_2) {
    return arr_type_1->size == arr_type_2->size
           && is_same_type(arr_type_1->elem_type.get(), arr_type_2->elem_type.get());
}

static bool is_same_struct(Structure* struct_type_1, Structure* struct_type_2) {
    return struct_type_1->tag == struct_type_2->tag;
}

static bool is_same_type(Type* type_1, Type* type_2) {
    if (type_1->type() == type_2->type()) {
        switch (type_1->type()) {
            case AST_Pointer_t:
                return is_same_ptr(static_cast<Pointer*>(type_1), static_cast<Pointer*>(type_2));
            case AST_Array_t:
                return is_same_arr(static_cast<Array*>(type_1), static_cast<Array*>(type_2));
            case AST_Structure_t:
                return is_same_struct(static_cast<Structure*>(type_1), static_cast<Structure*>(type_2));
            case AST_FunType_t:
                THROW_ABORT;
            default:
                return true;
        }
    }
    return false;
}

static bool is_same_fun_type(FunType* fun_type_1, FunType* fun_type_2) {
    if (vec_size(fun_type_1->param_types) != vec_size(fun_type_2->param_types)
        || !is_same_type(fun_type_1->ret_type.get(), fun_type_2->ret_type.get())) {
        return false;
    }
    for (size_t i = 0; i < vec_size(fun_type_1->param_types); ++i) {
        if (!is_same_type(fun_type_1->param_types[i].get(), fun_type_2->param_types[i].get())) {
            return false;
        }
    }
    return true;
}

static bool is_type_signed(Type* type) {
    switch (type->type()) {
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

static bool is_type_char(Type* type) {
    switch (type->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_int(Type* type) {
    switch (type->type()) {
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

static bool is_type_arithmetic(Type* type) {
    switch (type->type()) {
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

static bool is_type_scalar(Type* type) {
    switch (type->type()) {
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

static bool is_struct_complete(Ctx ctx, Structure* struct_type) {
    return ctx->frontend->struct_typedef_table.find(struct_type->tag) != ctx->frontend->struct_typedef_table.end();
}

static bool is_type_complete(Ctx ctx, Type* type) {
    switch (type->type()) {
        case AST_Void_t:
            return false;
        case AST_Structure_t:
            return is_struct_complete(ctx, static_cast<Structure*>(type));
        default:
            return true;
    }
}

static error_t is_valid_type(Ctx ctx, Type* type);

static error_t is_valid_ptr(Ctx ctx, Pointer* ptr_type) {
    CATCH_ENTER;
    TRY(is_valid_type(ctx, ptr_type->ref_type.get()));
    FINALLY;
    CATCH_EXIT;
}

static error_t is_valid_arr(Ctx ctx, Array* arr_type) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_complete(ctx, arr_type->elem_type.get())) {
        THROW_AT_LINE(ctx->errors->linebuf, GET_SEMANTIC_MSG(MSG_incomplete_arr, str_fmt_type(arr_type, &type_fmt_1),
                                                str_fmt_type(arr_type->elem_type.get(), &type_fmt_2)));
    }
    TRY(is_valid_type(ctx, arr_type->elem_type.get()));
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t is_valid_type(Ctx ctx, Type* type) {
    CATCH_ENTER;
    switch (type->type()) {
        case AST_Pointer_t:
            TRY(is_valid_ptr(ctx, static_cast<Pointer*>(type)));
            break;
        case AST_Array_t:
            TRY(is_valid_arr(ctx, static_cast<Array*>(type)));
            break;
        case AST_FunType_t:
            THROW_ABORT;
        default:
            break;
    }
    FINALLY;
    CATCH_EXIT;
}

static bool is_exp_lvalue(CExp* node);

static bool is_dot_exp_lvalue(CDot* node) { return is_exp_lvalue(node->structure.get()); }

static bool is_exp_lvalue(CExp* node) {
    switch (node->type()) {
        case AST_CString_t:
        case AST_CVar_t:
        case AST_CDereference_t:
        case AST_CSubscript_t:
        case AST_CArrow_t:
            return true;
        case AST_CDot_t:
            return is_dot_exp_lvalue(static_cast<CDot*>(node));
        default:
            return false;
    }
}

static bool is_const_null_ptr(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstInt_t:
            return static_cast<CConstInt*>(node->constant.get())->value == 0;
        case AST_CConstLong_t:
            return static_cast<CConstLong*>(node->constant.get())->value == 0l;
        case AST_CConstUInt_t:
            return static_cast<CConstUInt*>(node->constant.get())->value == 0u;
        case AST_CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value == 0ul;
        default:
            return false;
    }
}

static TInt get_scalar_size(Type* type) {
    switch (type->type()) {
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
    while (arr_type->elem_type->type() == AST_Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    return get_type_scale(ctx, arr_type->elem_type.get()) * size;
}

static TLong get_struct_scale(Ctx ctx, Structure* struct_type) {
    THROW_ABORT_IF(
        ctx->frontend->struct_typedef_table.find(struct_type->tag) == ctx->frontend->struct_typedef_table.end());
    return ctx->frontend->struct_typedef_table[struct_type->tag]->size;
}

static TLong get_type_scale(Ctx ctx, Type* type) {
    switch (type->type()) {
        case AST_Array_t:
            return get_arr_scale(ctx, static_cast<Array*>(type));
        case AST_Structure_t:
            return get_struct_scale(ctx, static_cast<Structure*>(type));
        default:
            return get_scalar_size(type);
    }
}

static TInt get_type_alignment(Ctx ctx, Type* type);

static TInt get_arr_alignment(Ctx ctx, Array* arr_type) { return get_type_alignment(ctx, arr_type->elem_type.get()); }

static TInt get_struct_alignment(Ctx ctx, Structure* struct_type) {
    THROW_ABORT_IF(
        ctx->frontend->struct_typedef_table.find(struct_type->tag) == ctx->frontend->struct_typedef_table.end());
    return ctx->frontend->struct_typedef_table[struct_type->tag]->alignment;
}

static TInt get_type_alignment(Ctx ctx, Type* type) {
    switch (type->type()) {
        case AST_Array_t:
            return get_arr_alignment(ctx, static_cast<Array*>(type));
        case AST_Structure_t:
            return get_struct_alignment(ctx, static_cast<Structure*>(type));
        default:
            return get_scalar_size(type);
    }
}

static std::shared_ptr<Type> get_joint_type(CExp* node_1, CExp* node_2) {
    if (is_type_char(node_1->exp_type.get())) {
        std::shared_ptr<Type> exp_type = std::move(node_1->exp_type);
        node_1->exp_type = std::make_shared<Int>();
        std::shared_ptr<Type> joint_type = get_joint_type(node_1, node_2);
        node_1->exp_type = std::move(exp_type);
        return joint_type;
    }
    else if (is_type_char(node_2->exp_type.get())) {
        std::shared_ptr<Type> exp_type = std::move(node_2->exp_type);
        node_2->exp_type = std::make_shared<Int>();
        std::shared_ptr<Type> joint_type = get_joint_type(node_1, node_2);
        node_2->exp_type = std::move(exp_type);
        return joint_type;
    }
    else if (is_same_type(node_1->exp_type.get(), node_2->exp_type.get())) {
        return node_1->exp_type;
    }
    else if (node_1->exp_type->type() == AST_Double_t || node_2->exp_type->type() == AST_Double_t) {
        return std::make_shared<Double>();
    }

    TInt type_size_1 = get_scalar_size(node_1->exp_type.get());
    TInt type_size_2 = get_scalar_size(node_2->exp_type.get());
    if (type_size_1 == type_size_2) {
        if (is_type_signed(node_1->exp_type.get())) {
            return node_2->exp_type;
        }
        else {
            return node_1->exp_type;
        }
    }
    else if (type_size_1 > type_size_2) {
        return node_1->exp_type;
    }
    else {
        return node_2->exp_type;
    }
}

static error_t get_joint_ptr_type(Ctx ctx, CExp* node_1, CExp* node_2, return_t(std::shared_ptr<Type>) joint_type) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (is_same_type(node_1->exp_type.get(), node_2->exp_type.get())) {
        *joint_type = node_1->exp_type;
    }
    else if (node_1->type() == AST_CConstant_t && is_const_null_ptr(static_cast<CConstant*>(node_1))) {
        *joint_type = node_2->exp_type;
    }
    else if ((node_2->type() == AST_CConstant_t && is_const_null_ptr(static_cast<CConstant*>(node_2)))
             || (node_1->exp_type->type() == AST_Pointer_t
                 && static_cast<Pointer*>(node_1->exp_type.get())->ref_type->type() == AST_Void_t
                 && node_2->exp_type->type() == AST_Pointer_t)) {
        *joint_type = node_1->exp_type;
    }
    else if (node_2->exp_type->type() == AST_Pointer_t
             && static_cast<Pointer*>(node_2->exp_type.get())->ref_type->type() == AST_Void_t
             && node_1->exp_type->type() == AST_Pointer_t) {
        *joint_type = node_2->exp_type;
    }
    else {
        THROW_AT_LINE(
            node_1->line, GET_SEMANTIC_MSG(MSG_joint_ptr_mismatch, str_fmt_type(node_1->exp_type.get(), &type_fmt_1),
                              str_fmt_type(node_2->exp_type.get(), &type_fmt_2)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static TChar get_const_char_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<CConstChar*>(node->constant.get())->value;
        case AST_CConstInt_t:
            return (TChar)(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return (TChar)(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return (TChar)(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return (TChar)(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return (TChar)(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return (TChar)(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            THROW_ABORT;
    }
}

static TInt get_const_int_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return (TInt)(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return static_cast<CConstInt*>(node->constant.get())->value;
        case AST_CConstLong_t:
            return (TInt)(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return (TInt)(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return (TInt)(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return (TInt)(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return (TInt)(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            THROW_ABORT;
    }
}

static TLong get_const_long_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return (TLong)(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return (TLong)(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<CConstLong*>(node->constant.get())->value;
        case AST_CConstDouble_t:
            return (TLong)(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return (TLong)(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return (TLong)(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return (TLong)(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            THROW_ABORT;
    }
}

static TDouble get_const_dbl_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return (TDouble)(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return (TDouble)(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return (TDouble)(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return static_cast<CConstDouble*>(node->constant.get())->value;
        case AST_CConstUChar_t:
            return (TDouble)(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return (TDouble)(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return (TDouble)(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            THROW_ABORT;
    }
}

static TUChar get_const_uchar_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return (TUChar)(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return (TUChar)(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return (TUChar)(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return (TUChar)(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return static_cast<CConstUChar*>(node->constant.get())->value;
        case AST_CConstUInt_t:
            return (TUChar)(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return (TUChar)(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            THROW_ABORT;
    }
}

static TUInt get_const_uint_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return (TUInt)(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return (TUInt)(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return (TUInt)(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return (TUInt)(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return (TUInt)(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<CConstUInt*>(node->constant.get())->value;
        case AST_CConstULong_t:
            return (TUInt)(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            THROW_ABORT;
    }
}

static TULong get_const_ulong_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return (TULong)(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return (TULong)(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return (TULong)(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return (TULong)(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return (TULong)(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return (TULong)(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value;
        default:
            THROW_ABORT;
    }
}

static TULong get_const_ptr_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstInt_t:
            return (TULong)(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return (TULong)(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return (TULong)(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value;
        default:
            THROW_ABORT;
    }
}

static size_t get_compound_line(CInitializer* node) {
    THROW_ABORT_IF(node->type() != AST_CCompoundInit_t);
    do {
        node = static_cast<CCompoundInit*>(node)->initializers[0].get();
    }
    while (node->type() == AST_CCompoundInit_t);
    THROW_ABORT_IF(node->type() != AST_CSingleInit_t);
    return static_cast<CSingleInit*>(node)->exp->line;
}

static error_t reslv_struct_type(Ctx ctx, Type* type);

static void check_const_exp(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            node->exp_type = std::make_shared<Char>();
            break;
        case AST_CConstInt_t:
            node->exp_type = std::make_shared<Int>();
            break;
        case AST_CConstLong_t:
            node->exp_type = std::make_shared<Long>();
            break;
        case AST_CConstDouble_t:
            node->exp_type = std::make_shared<Double>();
            break;
        case AST_CConstUChar_t:
            node->exp_type = std::make_shared<UChar>();
            break;
        case AST_CConstUInt_t:
            node->exp_type = std::make_shared<UInt>();
            break;
        case AST_CConstULong_t:
            node->exp_type = std::make_shared<ULong>();
            break;
        default:
            THROW_ABORT;
    }
}

static void check_string_exp(CString* node) {
    TLong size = ((TLong)vec_size(node->literal->value)) + 1l;
    std::shared_ptr<Type> elem_type = std::make_shared<Char>();
    node->exp_type = std::make_shared<Array>(size, std::move(elem_type));
}

static error_t check_var_exp(Ctx ctx, CVar* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (ctx->frontend->symbol_table[node->name]->type_t->type() == AST_FunType_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_fun_used_as_var, str_fmt_name(node->name, &name_fmt)));
    }
    node->exp_type = ctx->frontend->symbol_table[node->name]->type_t;
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t check_cast_exp(Ctx ctx, CCast* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, node->target_type.get()));
    if (node->target_type->type() != AST_Void_t
        && ((node->exp->exp_type->type() == AST_Double_t && node->target_type->type() == AST_Pointer_t)
            || (node->exp->exp_type->type() == AST_Pointer_t && node->target_type->type() == AST_Double_t)
            || !is_type_scalar(node->exp->exp_type.get()) || !is_type_scalar(node->target_type.get()))) {
        THROW_AT_LINE(
            node->line, GET_SEMANTIC_MSG(MSG_illegal_cast, str_fmt_type(node->exp->exp_type.get(), &type_fmt_1),
                            str_fmt_type(node->target_type.get(), &type_fmt_2)));
    }
    TRY(is_valid_type(ctx, node->target_type.get()));
    node->exp_type = node->target_type;
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t cast_exp(Ctx ctx, std::shared_ptr<Type>& exp_type, return_t(std::unique_ptr<CExp>) exp) {
    std::shared_ptr<Type> exp_type_cp;
    CATCH_ENTER;
    size_t line = (*exp)->line;
    exp_type_cp = exp_type;
    *exp = std::make_unique<CCast>(std::move(*exp), std::move(exp_type_cp), line);
    TRY(check_cast_exp(ctx, static_cast<CCast*>(exp->get())));
    FINALLY;
    CATCH_EXIT;
}

static error_t cast_assign(Ctx ctx, std::shared_ptr<Type>& exp_type, return_t(std::unique_ptr<CExp>) exp) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if ((is_type_arithmetic((*exp)->exp_type.get()) && is_type_arithmetic(exp_type.get()))
        || ((*exp)->type() == AST_CConstant_t && exp_type->type() == AST_Pointer_t
            && is_const_null_ptr(static_cast<CConstant*>(exp->get())))
        || (exp_type->type() == AST_Pointer_t && static_cast<Pointer*>(exp_type.get())->ref_type->type() == AST_Void_t
            && (*exp)->exp_type->type() == AST_Pointer_t)
        || ((*exp)->exp_type->type() == AST_Pointer_t
            && static_cast<Pointer*>((*exp)->exp_type.get())->ref_type->type() == AST_Void_t
            && exp_type->type() == AST_Pointer_t)) {
        TRY(cast_exp(ctx, exp_type, exp));
    }
    else {
        THROW_AT_LINE(
            (*exp)->line, GET_SEMANTIC_MSG(MSG_illegal_cast, str_fmt_type((*exp)->exp_type.get(), &type_fmt_1),
                              str_fmt_type(exp_type.get(), &type_fmt_2)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t promote_char_to_int(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    std::shared_ptr<Type> promote_type;
    CATCH_ENTER;
    promote_type = std::make_shared<Int>();
    TRY(cast_exp(ctx, promote_type, exp));
    FINALLY;
    CATCH_EXIT;
}

static error_t check_unary_complement_exp(Ctx ctx, CUnary* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp->exp_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(node->unop.get()),
                                      str_fmt_type(node->exp->exp_type.get(), &type_fmt)));
    }

    switch (node->exp->exp_type->type()) {
        case AST_Double_t:
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(node->unop.get()),
                                          str_fmt_type(node->exp->exp_type.get(), &type_fmt)));
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            TRY(promote_char_to_int(ctx, &node->exp));
            break;
        default:
            break;
    }
    node->exp_type = node->exp->exp_type;
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_unary_neg_exp(Ctx ctx, CUnary* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp->exp_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(node->unop.get()),
                                      str_fmt_type(node->exp->exp_type.get(), &type_fmt)));
    }

    switch (node->exp->exp_type->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            TRY(promote_char_to_int(ctx, &node->exp));
            break;
        default:
            break;
    }
    node->exp_type = node->exp->exp_type;
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_unary_not_exp(Ctx ctx, CUnary* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_scalar(node->exp->exp_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_unary_op, get_unop_fmt(node->unop.get()),
                                      str_fmt_type(node->exp->exp_type.get(), &type_fmt)));
    }

    node->exp_type = std::make_shared<Int>();
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_unary_exp(Ctx ctx, CUnary* node) {
    CATCH_ENTER;
    switch (node->unop->type()) {
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
    std::shared_ptr<Type> common_type;
    CATCH_ENTER;
    if (is_type_arithmetic(node->exp_left->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (node->exp_left->exp_type->type() == AST_Pointer_t
             && is_type_complete(ctx, static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())
             && is_type_int(node->exp_right->exp_type.get())) {
        common_type = std::make_shared<Long>();
        if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
            TRY(cast_exp(ctx, common_type, &node->exp_right));
        }
        node->exp_type = node->exp_left->exp_type;
        EARLY_EXIT;
    }
    else if (is_type_int(node->exp_left->exp_type.get()) && node->exp_right->exp_type->type() == AST_Pointer_t
             && is_type_complete(ctx, static_cast<Pointer*>(node->exp_right->exp_type.get())->ref_type.get())) {
        common_type = std::make_shared<Long>();
        if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
            TRY(cast_exp(ctx, common_type, &node->exp_left));
        }
        node->exp_type = node->exp_right->exp_type;
        EARLY_EXIT;
    }
    else {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_right));
    }
    node->exp_type = std::move(common_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_binary_subtract_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::shared_ptr<Type> common_type;
    CATCH_ENTER;
    if (is_type_arithmetic(node->exp_left->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (node->exp_left->exp_type->type() == AST_Pointer_t
             && is_type_complete(ctx, static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())) {
        if (is_type_int(node->exp_right->exp_type.get())) {
            common_type = std::make_shared<Long>();
            if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
                TRY(cast_exp(ctx, common_type, &node->exp_right));
            }
            node->exp_type = node->exp_left->exp_type;
            EARLY_EXIT;
        }
        else if (is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())
                 && !(node->exp_left->type() == AST_CConstant_t
                      && is_const_null_ptr(static_cast<CConstant*>(node->exp_left.get())))) {
            common_type = std::make_shared<Long>();
            node->exp_type = std::move(common_type);
            EARLY_EXIT;
        }
        else {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                          str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                          str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
        }
    }
    else {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_right));
    }
    node->exp_type = std::move(common_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_multiply_divide_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::shared_ptr<Type> common_type;
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_arithmetic(node->exp_right->exp_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_right));
    }
    node->exp_type = std::move(common_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_remainder_bitwise_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::shared_ptr<Type> common_type;
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_arithmetic(node->exp_right->exp_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_right));
    }
    node->exp_type = std::move(common_type);
    if (node->exp_type->type() == AST_Double_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_op, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_type.get(), &type_fmt_1)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_binary_bitshift_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_int(node->exp_right->exp_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    else if (is_type_char(node->exp_left->exp_type.get())) {
        TRY(promote_char_to_int(ctx, &node->exp_left));
    }
    if (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())) {
        TRY(cast_exp(ctx, node->exp_left->exp_type, &node->exp_right));
    }
    node->exp_type = node->exp_left->exp_type;
    if (node->exp_type->type() == AST_Double_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_op, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_type.get(), &type_fmt_1)));
    }
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_bitshift_right_exp(Ctx ctx, CBinary* node) {
    CATCH_ENTER;
    TRY(check_binary_bitshift_exp(ctx, node));
    if (is_type_signed(node->exp_left->exp_type.get())) {
        node->binop = std::make_unique<CBitShrArithmetic>();
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_binary_logical_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_scalar(node->exp_left->exp_type.get()) || !is_type_scalar(node->exp_right->exp_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    node->exp_type = std::make_shared<Int>();
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_binary_equality_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::shared_ptr<Type> common_type;
    CATCH_ENTER;
    if (node->exp_left->exp_type->type() == AST_Pointer_t || node->exp_right->exp_type->type() == AST_Pointer_t) {
        TRY(get_joint_ptr_type(ctx, node->exp_left.get(), node->exp_right.get(), &common_type));
    }
    else if (is_type_arithmetic(node->exp_left->exp_type.get())
             && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_right));
    }
    node->exp_type = std::make_shared<Int>();
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_binary_relational_exp(Ctx ctx, CBinary* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::shared_ptr<Type> common_type;
    CATCH_ENTER;
    if (!is_type_scalar(node->exp_left->exp_type.get()) || !is_type_scalar(node->exp_right->exp_type.get())
        || (node->exp_left->exp_type->type() == AST_Pointer_t
            && (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())
                || (node->exp_left->type() == AST_CConstant_t
                    && is_const_null_ptr(static_cast<CConstant*>(node->exp_left.get())))
                || (node->exp_right->type() == AST_CConstant_t
                    && is_const_null_ptr(static_cast<CConstant*>(node->exp_right.get())))))) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_invalid_binary_ops, get_binop_fmt(node->binop.get()),
                                      str_fmt_type(node->exp_left->exp_type.get(), &type_fmt_1),
                                      str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }

    common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_left));
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_right));
    }
    node->exp_type = std::make_shared<Int>();
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_binary_exp(Ctx ctx, CBinary* node) {
    CATCH_ENTER;
    switch (node->binop->type()) {
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
        if (node->exp_left->exp_type->type() == AST_Void_t) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG_0(MSG_assign_to_void));
        }
        else if (!is_exp_lvalue(node->exp_left.get())) {
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_assign_to_rvalue, get_assign_fmt(nullptr, node->unop.get())));
        }
        else if (!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
            TRY(cast_assign(ctx, node->exp_left->exp_type, &node->exp_right));
        }
        node->exp_type = node->exp_left->exp_type;
    }
    else {
        THROW_ABORT_IF(node->exp_right->type() != AST_CBinary_t);
        CExp* exp_left = static_cast<CBinary*>(node->exp_right.get())->exp_left.get();
        if (exp_left->type() == AST_CCast_t) {
            exp_left = static_cast<CCast*>(exp_left)->exp.get();
        }
        if (!is_exp_lvalue(exp_left)) {
            THROW_AT_LINE(node->line,
                GET_SEMANTIC_MSG(MSG_assign_to_rvalue,
                    get_assign_fmt(static_cast<CBinary*>(node->exp_right.get())->binop.get(), node->unop.get())));
        }
        else if (!is_same_type(node->exp_right->exp_type.get(), exp_left->exp_type.get())) {
            TRY(cast_assign(ctx, exp_left->exp_type, &node->exp_right));
        }
        node->exp_type = exp_left->exp_type;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_conditional_exp(Ctx ctx, CConditional* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::shared_ptr<Type> common_type;
    CATCH_ENTER;
    if (!is_type_scalar(node->condition->exp_type.get())) {
        THROW_AT_LINE(node->line,
            GET_SEMANTIC_MSG(MSG_invalid_condition, str_fmt_type(node->condition->exp_type.get(), &type_fmt_1)));
    }
    else if (node->exp_middle->exp_type->type() == AST_Void_t && node->exp_right->exp_type->type() == AST_Void_t) {
        node->exp_type = node->exp_middle->exp_type;
        EARLY_EXIT;
    }
    else if (node->exp_middle->exp_type->type() == AST_Structure_t
             || node->exp_right->exp_type->type() == AST_Structure_t) {
        if (!is_same_type(node->exp_middle->exp_type.get(), node->exp_right->exp_type.get())) {
            THROW_AT_LINE(node->line,
                GET_SEMANTIC_MSG(MSG_invalid_ternary_op, str_fmt_type(node->exp_middle->exp_type.get(), &type_fmt_1),
                    str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
        }
        node->exp_type = node->exp_middle->exp_type;
        EARLY_EXIT;
    }

    if (is_type_arithmetic(node->exp_middle->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_middle.get(), node->exp_right.get());
    }
    else if (node->exp_middle->exp_type->type() == AST_Pointer_t
             || node->exp_right->exp_type->type() == AST_Pointer_t) {
        TRY(get_joint_ptr_type(ctx, node->exp_middle.get(), node->exp_right.get(), &common_type));
    }
    else {
        THROW_AT_LINE(node->line,
            GET_SEMANTIC_MSG(MSG_invalid_ternary_op, str_fmt_type(node->exp_middle->exp_type.get(), &type_fmt_1),
                str_fmt_type(node->exp_right->exp_type.get(), &type_fmt_2)));
    }
    if (!is_same_type(node->exp_middle->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_middle));
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        TRY(cast_exp(ctx, common_type, &node->exp_right));
    }
    node->exp_type = std::move(common_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_call_exp(Ctx ctx, CFunctionCall* node) {
    string_t name_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    FunType* fun_type;
    if (ctx->frontend->symbol_table[node->name]->type_t->type() != AST_FunType_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_var_used_as_fun, str_fmt_name(node->name, &name_fmt)));
    }
    fun_type = static_cast<FunType*>(ctx->frontend->symbol_table[node->name]->type_t.get());
    if (vec_size(fun_type->param_types) != vec_size(node->args)) {
        strto_fmt_1 = str_to_string(vec_size(node->args));
        strto_fmt_2 = str_to_string(vec_size(fun_type->param_types));
        THROW_AT_LINE(node->line,
            GET_SEMANTIC_MSG(MSG_call_with_wrong_argc, str_fmt_name(node->name, &name_fmt), strto_fmt_1, strto_fmt_2));
    }
    for (size_t i = 0; i < vec_size(node->args); ++i) {
        if (!is_same_type(node->args[i]->exp_type.get(), fun_type->param_types[i].get())) {
            TRY(cast_assign(ctx, fun_type->param_types[i], &node->args[i]));
        }
    }
    node->exp_type = fun_type->ret_type;
    FINALLY;
    str_delete(name_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static error_t check_deref_exp(Ctx ctx, CDereference* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->exp->exp_type->type() != AST_Pointer_t) {
        THROW_AT_LINE(
            node->line, GET_SEMANTIC_MSG(MSG_deref_not_ptr, str_fmt_type(node->exp->exp_type.get(), &type_fmt)));
    }
    node->exp_type = static_cast<Pointer*>(node->exp->exp_type.get())->ref_type;
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_addrof_exp(Ctx ctx, CAddrOf* node) {
    std::shared_ptr<Type> ref_type;
    CATCH_ENTER;
    if (!is_exp_lvalue(node->exp.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG_0(MSG_addrof_rvalue));
    }
    ref_type = node->exp->exp_type;
    node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
    FINALLY;
    CATCH_EXIT;
}

static error_t check_subscript_exp(Ctx ctx, CSubscript* node) {
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::shared_ptr<Type> ref_type;
    std::shared_ptr<Type> subscript_type;
    CATCH_ENTER;
    if (node->primary_exp->exp_type->type() == AST_Pointer_t
        && is_type_complete(ctx, static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type.get())
        && is_type_int(node->subscript_exp->exp_type.get())) {
        subscript_type = std::make_shared<Long>();
        if (!is_same_type(node->subscript_exp->exp_type.get(), subscript_type.get())) {
            TRY(cast_exp(ctx, subscript_type, &node->subscript_exp));
        }
        ref_type = static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type;
    }
    else if (is_type_int(node->primary_exp->exp_type.get()) && node->subscript_exp->exp_type->type() == AST_Pointer_t
             && is_type_complete(ctx, static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type.get())) {
        subscript_type = std::make_shared<Long>();
        if (!is_same_type(node->primary_exp->exp_type.get(), subscript_type.get())) {
            TRY(cast_exp(ctx, subscript_type, &node->primary_exp));
        }
        ref_type = static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type;
    }
    else {
        THROW_AT_LINE(node->line,
            GET_SEMANTIC_MSG(MSG_invalid_subscript, str_fmt_type(node->primary_exp->exp_type.get(), &type_fmt_1),
                str_fmt_type(node->subscript_exp->exp_type.get(), &type_fmt_2)));
    }
    node->exp_type = std::move(ref_type);
    FINALLY;
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_sizeof_exp(Ctx ctx, CSizeOf* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_complete(ctx, node->exp->exp_type.get())) {
        THROW_AT_LINE(
            node->line, GET_SEMANTIC_MSG(MSG_sizeof_incomplete, str_fmt_type(node->exp->exp_type.get(), &type_fmt)));
    }
    node->exp_type = std::make_shared<ULong>();
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_sizeoft_exp(Ctx ctx, CSizeOfT* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, node->target_type.get()));
    if (!is_type_complete(ctx, node->target_type.get())) {
        THROW_AT_LINE(
            node->line, GET_SEMANTIC_MSG(MSG_sizeof_incomplete, str_fmt_type(node->target_type.get(), &type_fmt)));
    }
    TRY(is_valid_type(ctx, node->target_type.get()));
    node->exp_type = std::make_shared<ULong>();
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_dot_exp(Ctx ctx, CDot* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    Structure* struct_type;
    if (node->structure->exp_type->type() != AST_Structure_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_dot_not_struct, str_fmt_name(node->member, &name_fmt),
                                      str_fmt_type(node->structure->exp_type.get(), &type_fmt)));
    }
    struct_type = static_cast<Structure*>(node->structure->exp_type.get());
    if (ctx->frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
        == ctx->frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_member_not_in_struct, str_fmt_type(struct_type, &type_fmt),
                                      str_fmt_name(node->member, &name_fmt)));
    }
    node->exp_type = ctx->frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_arrow_exp(Ctx ctx, CArrow* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    Pointer* ptr_type;
    Structure* struct_type;
    if (node->pointer->exp_type->type() != AST_Pointer_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_arrow_not_struct_ptr, str_fmt_name(node->member, &name_fmt),
                                      str_fmt_type(node->pointer->exp_type.get(), &type_fmt)));
    }
    ptr_type = static_cast<Pointer*>(node->pointer->exp_type.get());
    if (ptr_type->ref_type->type() != AST_Structure_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_arrow_not_struct_ptr, str_fmt_name(node->member, &name_fmt),
                                      str_fmt_type(node->pointer->exp_type.get(), &type_fmt)));
    }
    struct_type = static_cast<Structure*>(ptr_type->ref_type.get());
    if (ctx->frontend->struct_typedef_table.find(struct_type->tag) == ctx->frontend->struct_typedef_table.end()) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_arrow_incomplete, str_fmt_name(node->member, &name_fmt),
                                      str_fmt_type(struct_type, &type_fmt)));
    }
    else if (ctx->frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
             == ctx->frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_member_not_in_struct, str_fmt_type(struct_type, &type_fmt),
                                      str_fmt_name(node->member, &name_fmt)));
    }
    node->exp_type = ctx->frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static void check_arr_typed_exp(std::unique_ptr<CExp>& addrof) {
    {
        std::shared_ptr<Type> ref_type = static_cast<Array*>(addrof->exp_type.get())->elem_type;
        addrof->exp_type = std::make_shared<Pointer>(std::move(ref_type));
    }
    size_t line = addrof->line;
    addrof = std::make_unique<CAddrOf>(std::move(addrof), line);
    addrof->exp_type = static_cast<CAddrOf*>(addrof.get())->exp->exp_type;
}

static error_t check_struct_typed_exp(Ctx ctx, CExp* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_struct_complete(ctx, static_cast<Structure*>(node->exp_type.get()))) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_exp_incomplete, str_fmt_type(node->exp_type.get(), &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_typed_exp(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    switch ((*exp)->exp_type->type()) {
        case AST_Array_t:
            check_arr_typed_exp(*exp);
            break;
        case AST_Structure_t:
            TRY(check_struct_typed_exp(ctx, exp->get()));
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
    FunType* fun_type = static_cast<FunType*>(ctx->frontend->symbol_table[ctx->fun_def_name]->type_t.get());
    if (fun_type->ret_type->type() == AST_Void_t) {
        if (node->exp) {
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_ret_value_in_void_fun, str_fmt_name(ctx->fun_def_name, &name_fmt)));
        }
        EARLY_EXIT;
    }
    else if (!node->exp) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_no_ret_value_in_fun, str_fmt_name(ctx->fun_def_name, &name_fmt),
                                      str_fmt_type(fun_type->ret_type.get(), &type_fmt)));
    }

    else if (!is_same_type(node->exp->exp_type.get(), fun_type->ret_type.get())) {
        TRY(cast_assign(ctx, fun_type->ret_type, &node->exp));
    }
    TRY(check_typed_exp(ctx, &node->exp));
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_if_statement(Ctx ctx, CIf* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        THROW_AT_LINE(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_if, str_fmt_type(node->condition->exp_type.get(), &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_while_statement(Ctx ctx, CWhile* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        THROW_AT_LINE(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_while, str_fmt_type(node->condition->exp_type.get(), &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_do_while_statement(Ctx ctx, CDoWhile* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        THROW_AT_LINE(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_do_while, str_fmt_type(node->condition->exp_type.get(), &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_for_statement(Ctx ctx, CFor* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        THROW_AT_LINE(node->condition->line,
            GET_SEMANTIC_MSG(MSG_invalid_for, str_fmt_type(node->condition->exp_type.get(), &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_switch_int_cases(Ctx ctx, CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    std::vector<TInt> values(vec_size(node->cases));
    CATCH_ENTER;
    for (size_t i = 0; i < values.size(); ++i) {
        THROW_ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
        CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
        values[i] = get_const_int_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_LINE(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        esac->constant = std::make_shared<CConstInt>(values[i]);
        esac->exp_type = node->match->exp_type;
    }
    FINALLY;
    str_delete(strto_fmt);
    CATCH_EXIT;
}

static error_t check_switch_long_cases(Ctx ctx, CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    std::vector<TLong> values(vec_size(node->cases));
    CATCH_ENTER;
    for (size_t i = 0; i < values.size(); ++i) {
        THROW_ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
        CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
        values[i] = get_const_long_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_LINE(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        esac->constant = std::make_shared<CConstLong>(values[i]);
        esac->exp_type = node->match->exp_type;
    }
    FINALLY;
    str_delete(strto_fmt);
    CATCH_EXIT;
}

static error_t check_switch_uint_cases(Ctx ctx, CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    std::vector<TUInt> values(vec_size(node->cases));
    CATCH_ENTER;
    for (size_t i = 0; i < values.size(); ++i) {
        THROW_ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
        CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
        values[i] = get_const_uint_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_LINE(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        esac->constant = std::make_shared<CConstUInt>(values[i]);
        esac->exp_type = node->match->exp_type;
    }
    FINALLY;
    str_delete(strto_fmt);
    CATCH_EXIT;
}

static error_t check_switch_ulong_cases(Ctx ctx, CSwitch* node) {
    string_t strto_fmt = str_new(NULL);
    std::vector<TULong> values(vec_size(node->cases));
    CATCH_ENTER;
    for (size_t i = 0; i < values.size(); ++i) {
        THROW_ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
        CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
        values[i] = get_const_ulong_value(esac);
        for (size_t j = 0; j < i; ++j) {
            if (values[i] == values[j]) {
                strto_fmt = str_to_string(values[i]);
                THROW_AT_LINE(node->cases[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_case_value, strto_fmt));
            }
        }
        esac->constant = std::make_shared<CConstULong>(values[i]);
        esac->exp_type = node->match->exp_type;
    }
    FINALLY;
    str_delete(strto_fmt);
    CATCH_EXIT;
}

static error_t check_switch_statement(Ctx ctx, CSwitch* node) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_int(node->match->exp_type.get())) {
        THROW_AT_LINE(node->match->line,
            GET_SEMANTIC_MSG(MSG_invalid_switch, str_fmt_type(node->match->exp_type.get(), &type_fmt)));
    }
    switch (node->match->exp_type->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            TRY(promote_char_to_int(ctx, &node->match));
            break;
        default:
            break;
    }
    switch (node->match->exp_type->type()) {
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

static error_t check_bound_string_init(Ctx ctx, CString* node, Array* arr_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (!is_type_char(arr_type->elem_type.get())) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_string_init_not_char_arr, str_fmt_type(arr_type, &type_fmt)));
    }
    else if (vec_size(node->literal->value) > (size_t)arr_type->size) {
        strto_fmt_1 = str_to_string(arr_type->size);
        strto_fmt_2 = str_to_string(vec_size(node->literal->value));
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_string_init_overflow, strto_fmt_1, strto_fmt_2));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static error_t check_single_init(Ctx ctx, CSingleInit* node, std::shared_ptr<Type>& init_type) {
    CATCH_ENTER;
    if (!is_same_type(node->exp->exp_type.get(), init_type.get())) {
        TRY(cast_assign(ctx, init_type, &node->exp));
    }
    node->init_type = init_type;
    FINALLY;
    CATCH_EXIT;
}

static void check_string_init(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    node->exp->exp_type = init_type;
    node->init_type = init_type;
}

static std::unique_ptr<CInitializer> check_zero_init(Ctx ctx, Type* init_type);

static std::unique_ptr<CSingleInit> check_single_zero_init(Type* elem_type) {
    std::unique_ptr<CExp> exp;
    {
        std::shared_ptr<CConst> constant;
        switch (elem_type->type()) {
            case AST_Char_t:
            case AST_SChar_t: {
                constant = std::make_shared<CConstChar>(0);
                break;
            }
            case AST_Int_t: {
                constant = std::make_shared<CConstInt>(0);
                break;
            }
            case AST_Long_t: {
                constant = std::make_shared<CConstLong>(0l);
                break;
            }
            case AST_Double_t: {
                constant = std::make_shared<CConstDouble>(0.0);
                break;
            }
            case AST_UChar_t: {
                constant = std::make_shared<CConstUChar>(0u);
                break;
            }
            case AST_UInt_t: {
                constant = std::make_shared<CConstUInt>(0u);
                break;
            }
            case AST_ULong_t:
            case AST_Pointer_t: {
                constant = std::make_shared<CConstULong>(0ul);
                break;
            }
            default:
                THROW_ABORT;
        }
        exp = std::make_unique<CConstant>(std::move(constant), 0);
    }
    return std::make_unique<CSingleInit>(std::move(exp));
}

static std::unique_ptr<CCompoundInit> check_arr_zero_init(Ctx ctx, Array* arr_type) {
    vector_t(std::unique_ptr<CInitializer>) zero_inits = vec_new();
    size_t arr_type_size = (size_t)arr_type->size;
    vec_reserve(zero_inits, arr_type_size);
    for (size_t i = 0; i < arr_type_size; ++i) {
        std::unique_ptr<CInitializer> initializer = check_zero_init(ctx, arr_type->elem_type.get());
        vec_move_back(zero_inits, initializer);
    }
    return std::make_unique<CCompoundInit>(&zero_inits);
}

static std::unique_ptr<CCompoundInit> check_struct_zero_init(Ctx ctx, Structure* struct_type) {
    vector_t(std::unique_ptr<CInitializer>) zero_inits = vec_new();
    vec_reserve(zero_inits, vec_size(ctx->frontend->struct_typedef_table[struct_type->tag]->member_names));
    for (size_t i = 0; i < vec_size(ctx->frontend->struct_typedef_table[struct_type->tag]->member_names); ++i) {
        TIdentifier member_name = ctx->frontend->struct_typedef_table[struct_type->tag]->member_names[i];
        const auto& member = ctx->frontend->struct_typedef_table[struct_type->tag]->members[member_name];
        std::unique_ptr<CInitializer> initializer = check_zero_init(ctx, member->member_type.get());
        vec_move_back(zero_inits, initializer);
    }
    return std::make_unique<CCompoundInit>(&zero_inits);
}

static std::unique_ptr<CInitializer> check_zero_init(Ctx ctx, Type* init_type) {
    switch (init_type->type()) {
        case AST_Array_t:
            return check_arr_zero_init(ctx, static_cast<Array*>(init_type));
        case AST_Structure_t:
            return check_struct_zero_init(ctx, static_cast<Structure*>(init_type));
        default:
            return check_single_zero_init(init_type);
    }
}

static error_t check_bound_arr_init(Ctx ctx, CCompoundInit* node, Array* arr_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (vec_size(node->initializers) > (size_t)arr_type->size) {
        strto_fmt_1 = str_to_string(arr_type->size);
        strto_fmt_2 = str_to_string(vec_size(node->initializers));
        THROW_AT_LINE(get_compound_line(node),
            GET_SEMANTIC_MSG(MSG_arr_init_overflow, strto_fmt_1, str_fmt_type(arr_type, &type_fmt), strto_fmt_2));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static error_t check_bound_struct_init(Ctx ctx, CCompoundInit* node, Structure* struct_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt_1 = str_new(NULL);
    string_t strto_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    size_t bound = struct_type->is_union ? 1 : ctx->frontend->struct_typedef_table[struct_type->tag]->members.size();
    if (vec_size(node->initializers) > bound) {
        strto_fmt_1 = str_to_string(vec_size(node->initializers));
        strto_fmt_2 = str_to_string(bound);
        THROW_AT_LINE(get_compound_line(node),
            GET_SEMANTIC_MSG(MSG_struct_init_overflow, str_fmt_type(struct_type, &type_fmt), strto_fmt_1, strto_fmt_2));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt_1);
    str_delete(strto_fmt_2);
    CATCH_EXIT;
}

static void check_arr_init(Ctx ctx, CCompoundInit* node, Array* arr_type, std::shared_ptr<Type>& init_type) {
    while (vec_size(node->initializers) < (size_t)arr_type->size) {
        std::unique_ptr<CInitializer> zero_init = check_zero_init(ctx, arr_type->elem_type.get());
        vec_move_back(node->initializers, zero_init);
    }
    node->init_type = init_type;
}

static void check_struct_init(Ctx ctx, CCompoundInit* node, Structure* struct_type, std::shared_ptr<Type>& init_type) {
    for (size_t i = vec_size(node->initializers);
         i < ctx->frontend->struct_typedef_table[struct_type->tag]->members.size(); ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        std::unique_ptr<CInitializer> zero_init = check_zero_init(ctx, member->member_type.get());
        vec_move_back(node->initializers, zero_init);
    }
    node->init_type = init_type;
}

static error_t check_ret_fun_decl(Ctx ctx, CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, fun_type->ret_type.get()));
    TRY(is_valid_type(ctx, fun_type->ret_type.get()));

    switch (fun_type->ret_type->type()) {
        case AST_Array_t:
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_ret_arr, str_fmt_name(node->name, &name_fmt),
                                          str_fmt_type(fun_type->ret_type.get(), &type_fmt)));
        case AST_Structure_t: {
            if (node->body && !is_struct_complete(ctx, static_cast<Structure*>(fun_type->ret_type.get()))) {
                THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_ret_incomplete, str_fmt_name(node->name, &name_fmt),
                                              str_fmt_type(fun_type->ret_type.get(), &type_fmt)));
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

static void check_arr_param_decl(FunType* fun_type, size_t i) {
    std::shared_ptr<Type> ref_type = static_cast<Array*>(fun_type->param_types[i].get())->elem_type;
    fun_type->param_types[i] = std::make_shared<Pointer>(std::move(ref_type));
}

static error_t check_fun_params_decl(Ctx ctx, CFunctionDeclaration* node) {
    string_t name_fmt_1 = str_new(NULL);
    string_t name_fmt_2 = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    std::unique_ptr<IdentifierAttr> param_attrs;
    std::shared_ptr<Type> param_type;
    CATCH_ENTER;
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    for (size_t i = 0; i < vec_size(node->params); ++i) {
        ctx->errors->linebuf = node->line;
        TRY(reslv_struct_type(ctx, fun_type->param_types[i].get()));
        if (fun_type->param_types[i]->type() == AST_Void_t) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_void_param, str_fmt_name(node->name, &name_fmt_1),
                                          str_fmt_name(node->params[i], &name_fmt_2)));
        }
        TRY(is_valid_type(ctx, fun_type->param_types[i].get()));
        if (fun_type->param_types[i]->type() == AST_Array_t) {
            check_arr_param_decl(fun_type, i);
        }

        if (node->body) {
            if (fun_type->param_types[i]->type() == AST_Structure_t
                && !is_struct_complete(ctx, static_cast<Structure*>(fun_type->param_types[i].get()))) {
                THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_incomplete_param, str_fmt_name(node->name, &name_fmt_1),
                                              str_fmt_name(node->params[i], &name_fmt_2),
                                              str_fmt_type(fun_type->param_types[i].get(), &type_fmt)));
            }
            param_type = fun_type->param_types[i];
            param_attrs = std::make_unique<LocalAttr>();
            ctx->frontend->symbol_table[node->params[i]] =
                std::make_unique<Symbol>(std::move(param_type), std::move(param_attrs));
        }
    }
    FINALLY;
    str_delete(name_fmt_1);
    str_delete(name_fmt_2);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_fun_decl(Ctx ctx, CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::unique_ptr<IdentifierAttr> glob_fun_attrs;
    std::shared_ptr<Type> glob_fun_type;
    CATCH_ENTER;
    THROW_ABORT_IF(node->fun_type->type() == AST_Void_t);

    bool is_def = ctx->fun_def_set.find(node->name) != ctx->fun_def_set.end();
    bool is_glob = !(node->storage_class && node->storage_class->type() == AST_CStatic_t);

    if (ctx->frontend->symbol_table.find(node->name) != ctx->frontend->symbol_table.end()) {
        FunType* fun_type = static_cast<FunType*>(ctx->frontend->symbol_table[node->name]->type_t.get());
        if (!(ctx->frontend->symbol_table[node->name]->type_t->type() == AST_FunType_t
                && vec_size(fun_type->param_types) == vec_size(node->params)
                && is_same_fun_type(static_cast<FunType*>(node->fun_type.get()), fun_type))) {
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_redecl_fun_conflict, str_fmt_name(node->name, &name_fmt),
                                str_fmt_type(node->fun_type.get(), &type_fmt_1), str_fmt_type(fun_type, &type_fmt_2)));
        }
        else if (is_def && node->body) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redef_fun, str_fmt_name(node->name, &name_fmt),
                                          str_fmt_type(node->fun_type.get(), &type_fmt_1)));
        }

        FunAttr* fun_attrs = static_cast<FunAttr*>(ctx->frontend->symbol_table[node->name]->attrs.get());
        if (!is_glob && fun_attrs->is_glob) {
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_redecl_static_conflict, str_fmt_name(node->name, &name_fmt)));
        }
        is_glob = fun_attrs->is_glob;
    }

    if (node->body) {
        ctx->fun_def_set.insert(node->name);
        is_def = true;
        ctx->fun_def_name = node->name;
    }

    glob_fun_type = node->fun_type;
    glob_fun_attrs = std::make_unique<FunAttr>(is_def, is_glob);
    ctx->frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(glob_fun_type), std::move(glob_fun_attrs));
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static void push_static_init(Ctx ctx, std::shared_ptr<StaticInit>&& static_init) {
    vec_move_back(*ctx->p_static_inits, static_init);
}

static void push_zero_static_init(Ctx ctx, TLong byte) {
    if (!vec_empty(*ctx->p_static_inits) && vec_back(*ctx->p_static_inits)->type() == AST_ZeroInit_t) {
        static_cast<ZeroInit*>(vec_back(*ctx->p_static_inits).get())->byte += byte;
    }
    else {
        push_static_init(ctx, std::make_shared<ZeroInit>(byte));
    }
}

static error_t check_static_init(Ctx ctx, CInitializer* node, Type* static_init_type);

static void check_static_no_init(Ctx ctx, Type* static_init_type, TLong size) {
    TLong byte = static_init_type == nullptr ? size : get_type_scale(ctx, static_init_type) * size;
    push_zero_static_init(ctx, byte);
}

static std::shared_ptr<Initial> check_no_initializer(Ctx ctx, Type* static_init_type) {
    vector_t(std::shared_ptr<StaticInit>) static_inits = vec_new();
    {
        ctx->p_static_inits = &static_inits;
        check_static_no_init(ctx, static_init_type, 1l);
        ctx->p_static_inits = NULL;
    }
    return std::make_shared<Initial>(&static_inits);
}

static TIdentifier make_binary_identifier(Ctx ctx, TULong binary) {
    string_t strto_binary = str_to_string(binary);
    return make_string_identifier(ctx->identifiers, &strto_binary);
}

static error_t check_static_const_init(Ctx ctx, CConstant* node, Type* static_init_type) {
    string_t type_fmt = str_new(NULL);
    string_t strto_fmt = str_new(NULL);
    CATCH_ENTER;
    switch (static_init_type->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = get_const_char_value(node);
            if (value == 0) {
                push_zero_static_init(ctx, 1l);
            }
            else {
                push_static_init(ctx, std::make_shared<CharInit>(value));
            }
            break;
        }
        case AST_Int_t: {
            TInt value = get_const_int_value(node);
            if (value == 0) {
                push_zero_static_init(ctx, 4l);
            }
            else {
                push_static_init(ctx, std::make_shared<IntInit>(value));
            }
            break;
        }
        case AST_Long_t: {
            TLong value = get_const_long_value(node);
            if (value == 0l) {
                push_zero_static_init(ctx, 8l);
            }
            else {
                push_static_init(ctx, std::make_shared<LongInit>(value));
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
                push_static_init(ctx, std::make_shared<DoubleInit>(dbl_const));
            }
            break;
        }
        case AST_UChar_t: {
            TUChar value = get_const_uchar_value(node);
            if (value == 0u) {
                push_zero_static_init(ctx, 1l);
            }
            else {
                push_static_init(ctx, std::make_shared<UCharInit>(value));
            }
            break;
        }
        case AST_UInt_t: {
            TUInt value = get_const_uint_value(node);
            if (value == 0u) {
                push_zero_static_init(ctx, 4l);
            }
            else {
                push_static_init(ctx, std::make_shared<UIntInit>(value));
            }
            break;
        }
        case AST_ULong_t: {
            TULong value = get_const_ulong_value(node);
            if (value == 0ul) {
                push_zero_static_init(ctx, 8l);
            }
            else {
                push_static_init(ctx, std::make_shared<ULongInit>(value));
            }
            break;
        }
        case AST_Pointer_t: {
            switch (node->constant->type()) {
                case AST_CConstChar_t:
                case AST_CConstDouble_t:
                case AST_CConstUChar_t:
                    THROW_AT_LINE(node->line,
                        GET_SEMANTIC_MSG(MSG_static_ptr_init_not_int, str_fmt_type(static_init_type, &type_fmt),
                            get_const_fmt(node->constant.get())));
                default:
                    break;
            }
            TULong value = get_const_ptr_value(node);
            if (value != 0ul) {
                strto_fmt = str_to_string(value);
                THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_static_ptr_init_not_null,
                                              str_fmt_type(static_init_type, &type_fmt), strto_fmt));
            }
            push_zero_static_init(ctx, 8l);
            break;
        }
        default:
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_agg_init_with_single, str_fmt_type(static_init_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    str_delete(strto_fmt);
    CATCH_EXIT;
}

static error_t check_literal_string_init(Ctx ctx, CString* node, Pointer* static_ptr_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (static_ptr_type->ref_type->type() != AST_Char_t) {
        THROW_AT_LINE(
            node->line, GET_SEMANTIC_MSG(MSG_static_ptr_init_string, str_fmt_type(static_ptr_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static TIdentifier make_literal_identifier(Ctx ctx, CStringLiteral* node) {
    string_t value = string_literal_to_const(node->value);
    return make_string_identifier(ctx->identifiers, &value);
}

static void check_static_ptr_string_init(Ctx ctx, CString* node) {
    TIdentifier string_const_label;
    {
        TIdentifier string_const = make_literal_identifier(ctx, node->literal.get());
        if (ctx->frontend->string_const_table.find(string_const) != ctx->frontend->string_const_table.end()) {
            string_const_label = ctx->frontend->string_const_table[string_const];
        }
        else {
            string_const_label = repr_label_identifier(ctx->identifiers, LBL_Lstring);
            ctx->frontend->string_const_table[string_const] = string_const_label;
            std::shared_ptr<Type> constant_type;
            {
                TLong size = ((TLong)vec_size(node->literal->value)) + 1l;
                std::shared_ptr<Type> elem_type = std::make_shared<Char>();
                constant_type = std::make_shared<Array>(size, std::move(elem_type));
            }
            std::unique_ptr<IdentifierAttr> constant_attrs;
            {
                std::shared_ptr<StaticInit> static_init;
                {
                    std::shared_ptr<CStringLiteral> literal = node->literal;
                    static_init = std::make_shared<StringInit>(string_const, true, std::move(literal));
                }
                constant_attrs = std::make_unique<ConstantAttr>(std::move(static_init));
            }
            ctx->frontend->symbol_table[string_const_label] =
                std::make_unique<Symbol>(std::move(constant_type), std::move(constant_attrs));
        }
    }
    push_static_init(ctx, std::make_shared<PointerInit>(string_const_label));
}

static error_t check_static_arr_string_init(Ctx ctx, CString* node, Array* static_arr_type) {
    std::shared_ptr<CStringLiteral> literal;
    CATCH_ENTER;
    TLong byte;
    TRY(check_bound_string_init(ctx, node, static_arr_type));
    byte = static_arr_type->size - ((TLong)vec_size(node->literal->value)) - 1l;
    {
        bool is_null_term = byte >= 0l;
        TIdentifier string_const = make_literal_identifier(ctx, node->literal.get());
        literal = node->literal;
        push_static_init(ctx, std::make_shared<StringInit>(string_const, is_null_term, std::move(literal)));
    }
    if (byte > 0l) {
        push_zero_static_init(ctx, byte);
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_static_string_init(Ctx ctx, CString* node, Type* static_init_type) {
    CATCH_ENTER;
    switch (static_init_type->type()) {
        case AST_Pointer_t:
            TRY(check_literal_string_init(ctx, node, static_cast<Pointer*>(static_init_type)));
            check_static_ptr_string_init(ctx, node);
            break;
        case AST_Array_t:
            TRY(check_static_arr_string_init(ctx, node, static_cast<Array*>(static_init_type)));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_single_static_init(Ctx ctx, CSingleInit* node, Type* static_init_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    switch (node->exp->type()) {
        case AST_CConstant_t:
            TRY(check_static_const_init(ctx, static_cast<CConstant*>(node->exp.get()), static_init_type));
            break;
        case AST_CString_t:
            TRY(check_static_string_init(ctx, static_cast<CString*>(node->exp.get()), static_init_type));
            break;
        default:
            THROW_AT_LINE(node->exp->line,
                GET_SEMANTIC_MSG(MSG_static_init_not_const, str_fmt_type(static_init_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_static_arr_init(Ctx ctx, CCompoundInit* node, Array* arr_type) {
    CATCH_ENTER;
    TRY(check_bound_arr_init(ctx, node, arr_type));

    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        TRY(check_static_init(ctx, node->initializers[i].get(), arr_type->elem_type.get()));
    }
    if ((size_t)arr_type->size > vec_size(node->initializers)) {
        check_static_no_init(ctx, arr_type->elem_type.get(), arr_type->size - vec_size(node->initializers));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_static_struct_init(Ctx ctx, CCompoundInit* node, Structure* struct_type) {
    CATCH_ENTER;
    TLong size;
    TRY(check_bound_struct_init(ctx, node, struct_type));

    size = 0l;
    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        if (member->offset != size) {
            check_static_no_init(ctx, nullptr, member->offset - size);
            size = member->offset;
        }
        TRY(check_static_init(ctx, node->initializers[i].get(), member->member_type.get()));
        size += get_type_scale(ctx, member->member_type.get());
    }
    if (ctx->frontend->struct_typedef_table[struct_type->tag]->size != size) {
        check_static_no_init(ctx, nullptr, ctx->frontend->struct_typedef_table[struct_type->tag]->size - size);
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_static_compound_init(Ctx ctx, CCompoundInit* node, Type* static_init_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    switch (static_init_type->type()) {
        case AST_Array_t:
            TRY(check_static_arr_init(ctx, node, static_cast<Array*>(static_init_type)));
            break;
        case AST_Structure_t:
            TRY(check_static_struct_init(ctx, node, static_cast<Structure*>(static_init_type)));
            break;
        default:
            THROW_AT_LINE(get_compound_line(node),
                GET_SEMANTIC_MSG(MSG_scalar_init_with_compound, str_fmt_type(static_init_type, &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_static_init(Ctx ctx, CInitializer* node, Type* static_init_type) {
    CATCH_ENTER;
    switch (node->type()) {
        case AST_CSingleInit_t:
            TRY(check_single_static_init(ctx, static_cast<CSingleInit*>(node), static_init_type));
            break;
        case AST_CCompoundInit_t:
            TRY(check_static_compound_init(ctx, static_cast<CCompoundInit*>(node), static_init_type));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t check_initializer(
    Ctx ctx, CInitializer* node, Type* static_init_type, return_t(std::shared_ptr<InitialValue>) init_value) {
    vector_t(std::shared_ptr<StaticInit>) static_inits = vec_new();
    CATCH_ENTER;
    {
        ctx->p_static_inits = &static_inits;
        TRY(check_static_init(ctx, node, static_init_type));
        ctx->p_static_inits = NULL;
    }
    *init_value = std::make_shared<Initial>(&static_inits);
    FINALLY;
    for (size_t i = 0; i < vec_size(static_inits); ++i) {
        static_inits[i].reset();
    }
    vec_delete(static_inits);
    CATCH_EXIT;
}

static error_t check_file_var_decl(Ctx ctx, CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::unique_ptr<IdentifierAttr> glob_var_attrs;
    std::shared_ptr<InitialValue> init_value;
    std::shared_ptr<Type> glob_var_type;
    CATCH_ENTER;
    bool is_glob;
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, node->var_type.get()));
    if (node->var_type->type() == AST_Void_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_void_var_decl, str_fmt_name(node->name, &name_fmt)));
    }
    TRY(is_valid_type(ctx, node->var_type.get()));

    is_glob = !(node->storage_class && node->storage_class->type() == AST_CStatic_t);

    if (node->init) {
        if (node->var_type->type() == AST_Structure_t
            && !is_struct_complete(ctx, static_cast<Structure*>(node->var_type.get()))) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_incomplete_var_decl, str_fmt_name(node->name, &name_fmt),
                                          str_fmt_type(node->var_type.get(), &type_fmt_1)));
        }
        TRY(check_initializer(ctx, node->init.get(), node->var_type.get(), &init_value));
    }
    else {
        if (node->storage_class && node->storage_class->type() == AST_CExtern_t) {
            init_value = std::make_shared<NoInitializer>();
        }
        else {
            if (node->var_type->type() == AST_Structure_t
                && !is_struct_complete(ctx, static_cast<Structure*>(node->var_type.get()))) {
                THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_incomplete_var_decl, str_fmt_name(node->name, &name_fmt),
                                              str_fmt_type(node->var_type.get(), &type_fmt_1)));
            }
            init_value = std::make_shared<Tentative>();
        }
    }

    if (ctx->frontend->symbol_table.find(node->name) != ctx->frontend->symbol_table.end()) {
        if (!is_same_type(ctx->frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_redecl_var_conflict, str_fmt_name(node->name, &name_fmt),
                                str_fmt_type(node->var_type.get(), &type_fmt_1),
                                str_fmt_type(ctx->frontend->symbol_table[node->name]->type_t.get(), &type_fmt_2)));
        }

        StaticAttr* var_attrs = static_cast<StaticAttr*>(ctx->frontend->symbol_table[node->name]->attrs.get());
        if (node->storage_class && node->storage_class->type() == AST_CExtern_t) {
            is_glob = var_attrs->is_glob;
        }
        else if (is_glob != var_attrs->is_glob) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redecl_var_storage, str_fmt_name(node->name, &name_fmt)));
        }

        if (var_attrs->init->type() == AST_Initial_t) {
            if (init_value->type() == AST_Initial_t) {
                THROW_AT_LINE(
                    node->line, GET_SEMANTIC_MSG(MSG_redecl_var_storage, str_fmt_name(node->name, &name_fmt)));
            }
            else {
                init_value = var_attrs->init;
            }
        }
    }

    glob_var_type = node->var_type;
    glob_var_attrs = std::make_unique<StaticAttr>(is_glob, std::move(init_value));
    ctx->frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(glob_var_type), std::move(glob_var_attrs));
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_extern_block_var_decl(Ctx ctx, CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt_1 = str_new(NULL);
    string_t type_fmt_2 = str_new(NULL);
    std::unique_ptr<IdentifierAttr> local_var_attrs;
    std::shared_ptr<InitialValue> init_value;
    std::shared_ptr<Type> local_var_type;
    CATCH_ENTER;
    if (node->init) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redef_extern_var, str_fmt_name(node->name, &name_fmt)));
    }
    else if (ctx->frontend->symbol_table.find(node->name) != ctx->frontend->symbol_table.end()) {
        if (!is_same_type(ctx->frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_redecl_var_conflict, str_fmt_name(node->name, &name_fmt),
                                str_fmt_type(node->var_type.get(), &type_fmt_1),
                                str_fmt_type(ctx->frontend->symbol_table[node->name]->type_t.get(), &type_fmt_2)));
        }
        EARLY_EXIT;
    }

    local_var_type = node->var_type;
    init_value = std::make_shared<NoInitializer>();
    local_var_attrs = std::make_unique<StaticAttr>(true, std::move(init_value));
    ctx->frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt_1);
    str_delete(type_fmt_2);
    CATCH_EXIT;
}

static error_t check_static_block_var_decl(Ctx ctx, CVariableDeclaration* node) {
    std::unique_ptr<IdentifierAttr> local_var_attrs;
    std::shared_ptr<InitialValue> init_value;
    std::shared_ptr<Type> local_var_type;
    CATCH_ENTER;
    THROW_ABORT_IF(node->var_type->type() == AST_Structure_t
                   && !is_struct_complete(ctx, static_cast<Structure*>(node->var_type.get())));

    if (node->init) {
        TRY(check_initializer(ctx, node->init.get(), node->var_type.get(), &init_value));
    }
    else {
        init_value = check_no_initializer(ctx, node->var_type.get());
    }

    local_var_type = node->var_type;
    local_var_attrs = std::make_unique<StaticAttr>(false, std::move(init_value));
    ctx->frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
    FINALLY;
    CATCH_EXIT;
}

static error_t check_auto_block_var_decl(Ctx ctx, CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    std::unique_ptr<IdentifierAttr> local_var_attrs;
    std::shared_ptr<Type> local_var_type;
    CATCH_ENTER;
    if (node->var_type->type() == AST_Structure_t
        && !is_struct_complete(ctx, static_cast<Structure*>(node->var_type.get()))) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_incomplete_var_decl, str_fmt_name(node->name, &name_fmt),
                                      str_fmt_type(node->var_type.get(), &type_fmt)));
    }

    local_var_type = node->var_type;
    local_var_attrs = std::make_unique<LocalAttr>();
    ctx->frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
    FINALLY;
    str_delete(name_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_block_var_decl(Ctx ctx, CVariableDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    ctx->errors->linebuf = node->line;
    TRY(reslv_struct_type(ctx, node->var_type.get()));
    if (node->var_type->type() == AST_Void_t) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_void_var_decl, str_fmt_name(node->name, &name_fmt)));
    }
    TRY(is_valid_type(ctx, node->var_type.get()));

    if (node->storage_class) {
        switch (node->storage_class->type()) {
            case AST_CExtern_t:
                TRY(check_extern_block_var_decl(ctx, node));
                break;
            case AST_CStatic_t:
                TRY(check_static_block_var_decl(ctx, node));
                break;
            default:
                THROW_ABORT;
        }
    }
    else {
        TRY(check_auto_block_var_decl(ctx, node));
    }
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t check_struct_members_decl(Ctx ctx, CStructDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    string_t struct_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = 0; i < vec_size(node->members); ++i) {
        for (size_t j = i + 1; j < vec_size(node->members); ++j) {
            if (node->members[i]->member_name == node->members[j]->member_name) {
                THROW_AT_LINE(node->members[i]->line, GET_SEMANTIC_MSG(MSG_duplicate_member_decl,
                                                          str_fmt_struct_name(node->tag, node->is_union, &struct_fmt),
                                                          str_fmt_name(node->members[i]->member_name, &name_fmt)));
            }
        }
        THROW_ABORT_IF(node->members[i].get()->member_type->type() == AST_FunType_t);
        ctx->errors->linebuf = node->members[i]->line;
        TRY(reslv_struct_type(ctx, node->members[i].get()->member_type.get()));
        if (!is_type_complete(ctx, node->members[i].get()->member_type.get())) {
            THROW_AT_LINE(
                node->members[i]->line, GET_SEMANTIC_MSG(MSG_incomplete_member_decl,
                                            str_fmt_struct_name(node->tag, node->is_union, &struct_fmt),
                                            str_fmt_name(node->members[i]->member_name, &name_fmt),
                                            str_fmt_type(node->members[i].get()->member_type.get(), &type_fmt)));
        }
        TRY(is_valid_type(ctx, node->members[i].get()->member_type.get()));
    }
    FINALLY;
    str_delete(name_fmt);
    str_delete(struct_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t check_struct_decl(Ctx ctx, CStructDeclaration* node) {
    string_t struct_fmt = str_new(NULL);
    std::shared_ptr<Type> member_type;
    vector_t(TIdentifier) member_names = vec_new();
    std::unordered_map<TIdentifier, std::unique_ptr<StructMember>> members;
    CATCH_ENTER;
    TInt alignment;
    TLong size;
    if (ctx->frontend->struct_typedef_table.find(node->tag) != ctx->frontend->struct_typedef_table.end()) {
        THROW_AT_LINE(node->line,
            GET_SEMANTIC_MSG(MSG_redecl_struct_in_scope, str_fmt_struct_name(node->tag, node->is_union, &struct_fmt)));
    }
    alignment = 0;
    size = 0l;
    vec_reserve(member_names, vec_size(node->members));
    members.reserve(vec_size(node->members));
    for (size_t i = 0; i < vec_size(node->members); ++i) {
        {
            TIdentifier name = node->members[i]->member_name;
            vec_push_back(member_names, name);
        }
        TInt member_alignment = get_type_alignment(ctx, node->members[i]->member_type.get());
        TLong member_size = get_type_scale(ctx, node->members[i]->member_type.get());
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
            member_type = node->members[i]->member_type;
            members[vec_back(member_names)] = std::make_unique<StructMember>(offset, std::move(member_type));
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
    ctx->frontend->struct_typedef_table[node->tag] =
        std::make_unique<StructTypedef>(alignment, size, &member_names, std::move(members));
    FINALLY;
    str_delete(struct_fmt);
    vec_delete(member_names);
    CATCH_EXIT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loop labeling

static error_t annotate_goto_label(Ctx ctx, CLabel* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (ctx->label_set.find(node->target) != ctx->label_set.end()) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redef_label_in_scope, str_fmt_name(node->target, &name_fmt)));
    }
    ctx->label_set.insert(node->target);
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static void annotate_while_loop(Ctx ctx, CWhile* node) {
    node->target = repr_label_identifier(ctx->identifiers, LBL_Lwhile);
    ctx->break_loop_labels.push_back(node->target);
    ctx->continue_loop_labels.push_back(node->target);
}

static void annotate_do_while_loop(Ctx ctx, CDoWhile* node) {
    node->target = repr_label_identifier(ctx->identifiers, LBL_Ldo_while);
    ctx->break_loop_labels.push_back(node->target);
    ctx->continue_loop_labels.push_back(node->target);
}

static void annotate_for_loop(Ctx ctx, CFor* node) {
    node->target = repr_label_identifier(ctx->identifiers, LBL_Lfor);
    ctx->break_loop_labels.push_back(node->target);
    ctx->continue_loop_labels.push_back(node->target);
}

static void annotate_switch_lookup(Ctx ctx, CSwitch* node) {
    node->is_default = false;
    node->target = repr_label_identifier(ctx->identifiers, LBL_Lswitch);
    ctx->break_loop_labels.push_back(node->target);
}

static error_t annotate_case_jump(Ctx ctx, CCase* node) {
    CATCH_ENTER;
    if (!ctx->p_switch_statement) {
        THROW_AT_LINE(node->value->line, GET_SEMANTIC_MSG_0(MSG_case_out_of_switch));
    }
    node->target = repr_case_identifier(
        ctx->identifiers, ctx->p_switch_statement->target, false, vec_size(ctx->p_switch_statement->cases));
    FINALLY;
    CATCH_EXIT;
}

static error_t annotate_default_jump(Ctx ctx, CDefault* node) {
    CATCH_ENTER;
    if (!ctx->p_switch_statement) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG_0(MSG_default_out_of_switch));
    }
    else if (ctx->p_switch_statement->is_default) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG_0(MSG_multiple_default));
    }
    node->target = ctx->p_switch_statement->target;
    ctx->p_switch_statement->is_default = true;
    FINALLY;
    CATCH_EXIT;
}

static error_t annotate_break_jump(Ctx ctx, CBreak* node) {
    CATCH_ENTER;
    if (ctx->break_loop_labels.empty()) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG_0(MSG_break_out_of_loop));
    }
    node->target = ctx->break_loop_labels.back();
    FINALLY;
    CATCH_EXIT;
}

static error_t annotate_continue_jump(Ctx ctx, CContinue* node) {
    CATCH_ENTER;
    if (ctx->continue_loop_labels.empty()) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG_0(MSG_continue_out_of_loop));
    }
    node->target = ctx->continue_loop_labels.back();
    FINALLY;
    CATCH_EXIT;
}

static void deannotate_loop(Ctx ctx) {
    ctx->break_loop_labels.pop_back();
    ctx->continue_loop_labels.pop_back();
}

static void deannotate_lookup(Ctx ctx) { ctx->break_loop_labels.pop_back(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifier resolution

static bool is_file_scope(Ctx ctx) { return ctx->scoped_identifier_maps.size() == 1; }

static void enter_scope(Ctx ctx) {
    ctx->scoped_identifier_maps.emplace_back();
    ctx->scoped_struct_maps.emplace_back();
}

static void exit_scope(Ctx ctx) {
    for (const auto& identifier : ctx->scoped_identifier_maps.back()) {
        if (ctx->extern_scope_map.find(identifier.first) != ctx->extern_scope_map.end()
            && ctx->extern_scope_map[identifier.first] == ctx->scoped_identifier_maps.size()) {
            ctx->extern_scope_map.erase(identifier.first);
        }
    }
    ctx->scoped_identifier_maps.pop_back();
    ctx->scoped_struct_maps.pop_back();
}

static error_t reslv_label(Ctx ctx, CFunctionDeclaration* node) {
    string_t name_fmt_1 = str_new(NULL);
    string_t name_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    for (const auto& target : ctx->goto_map) {
        if (ctx->label_set.find(target.first) == ctx->label_set.end()) {
            THROW_AT_LINE(ctx->errors->linebuf_map[target.second],
                GET_SEMANTIC_MSG(MSG_undef_goto_target, str_fmt_name(target.first, &name_fmt_1),
                    str_fmt_name(node->name, &name_fmt_2)));
        }
    }
    FINALLY;
    str_delete(name_fmt_1);
    str_delete(name_fmt_2);
    CATCH_EXIT;
}

static error_t reslv_ptr_struct(Ctx ctx, Pointer* ptr_type) {
    CATCH_ENTER;
    TRY(reslv_struct_type(ctx, ptr_type->ref_type.get()));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_arr_struct(Ctx ctx, Array* arr_type) {
    CATCH_ENTER;
    TRY(reslv_struct_type(ctx, arr_type->elem_type.get()));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct(Ctx ctx, Structure* struct_type) {
    string_t struct_fmt = str_new(NULL);
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    if (struct_type->is_union) {
        if (ctx->union_def_set.find(struct_type->tag) != ctx->union_def_set.end()) {
            EARLY_EXIT;
        }
    }
    else if (ctx->struct_def_set.find(struct_type->tag) != ctx->struct_def_set.end()) {
        EARLY_EXIT;
    }
    for (size_t i = ctx->scoped_identifier_maps.size(); i-- > 0;) {
        if (ctx->scoped_struct_maps[i].find(struct_type->tag) != ctx->scoped_struct_maps[i].end()) {
            if (ctx->scoped_struct_maps[i][struct_type->tag].is_union != struct_type->is_union) {
                THROW_AT_LINE(ctx->errors->linebuf,
                    GET_SEMANTIC_MSG(MSG_redecl_struct_conflict, str_fmt_type(struct_type, &type_fmt),
                        str_fmt_struct_name(struct_type->tag, !struct_type->is_union, &struct_fmt)));
            }
            struct_type->tag = ctx->scoped_struct_maps[i][struct_type->tag].tag;
            EARLY_EXIT;
        }
    }
    THROW_AT_LINE(
        ctx->errors->linebuf, GET_SEMANTIC_MSG(MSG_undef_struct_in_scope, str_fmt_type(struct_type, &type_fmt)));
    FINALLY;
    str_delete(struct_fmt);
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t reslv_struct_type(Ctx ctx, Type* type) {
    CATCH_ENTER;
    switch (type->type()) {
        case AST_Pointer_t:
            TRY(reslv_ptr_struct(ctx, static_cast<Pointer*>(type)));
            break;
        case AST_Array_t:
            TRY(reslv_arr_struct(ctx, static_cast<Array*>(type)));
            break;
        case AST_Structure_t:
            TRY(reslv_struct(ctx, static_cast<Structure*>(type)));
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
static error_t reslv_typed_exp(Ctx ctx, return_t(std::unique_ptr<CExp>) exp);

static void reslv_const_exp(CConstant* node) { check_const_exp(node); }

static void reslv_string_exp(CString* node) { check_string_exp(node); }

static error_t reslv_var_exp(Ctx ctx, CVar* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = ctx->scoped_identifier_maps.size(); i-- > 0;) {
        if (ctx->scoped_identifier_maps[i].find(node->name) != ctx->scoped_identifier_maps[i].end()) {
            node->name = ctx->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_undecl_var_in_scope, str_fmt_name(node->name, &name_fmt)));
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
    for (size_t i = ctx->scoped_identifier_maps.size(); i-- > 0;) {
        if (ctx->scoped_identifier_maps[i].find(node->name) != ctx->scoped_identifier_maps[i].end()) {
            node->name = ctx->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_undecl_fun_in_scope, str_fmt_name(node->name, &name_fmt)));
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

static error_t reslv_addrof_expr(Ctx ctx, CAddrOf* node) {
    CATCH_ENTER;
    TRY(reslv_exp(ctx, node->exp.get()));
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

static error_t reslv_sizeof_exp(Ctx ctx, CSizeOf* node) {
    CATCH_ENTER;
    TRY(reslv_exp(ctx, node->exp.get()));
    TRY(check_sizeof_exp(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_sizeoft_exp(Ctx ctx, CSizeOfT* node) {
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
    switch (node->type()) {
        case AST_CConstant_t:
            reslv_const_exp(static_cast<CConstant*>(node));
            break;
        case AST_CString_t:
            reslv_string_exp(static_cast<CString*>(node));
            break;
        case AST_CVar_t:
            TRY(reslv_var_exp(ctx, static_cast<CVar*>(node)));
            break;
        case AST_CCast_t:
            TRY(reslv_cast_exp(ctx, static_cast<CCast*>(node)));
            break;
        case AST_CUnary_t:
            TRY(reslv_unary_exp(ctx, static_cast<CUnary*>(node)));
            break;
        case AST_CBinary_t:
            TRY(reslv_binary_exp(ctx, static_cast<CBinary*>(node)));
            break;
        case AST_CAssignment_t:
            TRY(reslv_assign_exp(ctx, static_cast<CAssignment*>(node)));
            break;
        case AST_CConditional_t:
            TRY(reslv_conditional_exp(ctx, static_cast<CConditional*>(node)));
            break;
        case AST_CFunctionCall_t:
            TRY(reslv_call_exp(ctx, static_cast<CFunctionCall*>(node)));
            break;
        case AST_CDereference_t:
            TRY(reslv_deref_exp(ctx, static_cast<CDereference*>(node)));
            break;
        case AST_CAddrOf_t:
            TRY(reslv_addrof_expr(ctx, static_cast<CAddrOf*>(node)));
            break;
        case AST_CSubscript_t:
            TRY(reslv_subscript_exp(ctx, static_cast<CSubscript*>(node)));
            break;
        case AST_CSizeOf_t:
            TRY(reslv_sizeof_exp(ctx, static_cast<CSizeOf*>(node)));
            break;
        case AST_CSizeOfT_t:
            TRY(reslv_sizeoft_exp(ctx, static_cast<CSizeOfT*>(node)));
            break;
        case AST_CDot_t:
            TRY(reslv_dot_exp(ctx, static_cast<CDot*>(node)));
            break;
        case AST_CArrow_t:
            TRY(reslv_arrow_exp(ctx, static_cast<CArrow*>(node)));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_typed_exp(Ctx ctx, return_t(std::unique_ptr<CExp>) exp) {
    CATCH_ENTER;
    TRY(reslv_exp(ctx, exp->get()));
    TRY(check_typed_exp(ctx, exp));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_block(Ctx ctx, CBlock* node);
static error_t reslv_block_var_decl(Ctx ctx, CVariableDeclaration* node);

static error_t reslv_statement(Ctx ctx, CStatement* node);

static error_t reslv_for_init_decl(Ctx ctx, CInitDecl* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (node->init->storage_class) {
        THROW_AT_LINE(
            node->init->line, GET_SEMANTIC_MSG(MSG_for_init_decl_not_auto, str_fmt_name(node->init->name, &name_fmt),
                                  get_storage_class_fmt(node->init->storage_class.get())));
    }
    TRY(reslv_block_var_decl(ctx, node->init.get()));
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
    switch (node->type()) {
        case AST_CInitDecl_t:
            TRY(reslv_for_init_decl(ctx, static_cast<CInitDecl*>(node)));
            break;
        case AST_CInitExp_t: {
            TRY(reslv_for_init_exp(ctx, static_cast<CInitExp*>(node)));
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
    TRY(reslv_statement(ctx, node->then.get()));
    if (node->else_fi) {
        TRY(reslv_statement(ctx, node->else_fi.get()));
    }
    TRY(check_if_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static void reslv_goto_statement(Ctx ctx, CGoto* node) {
    if (ctx->goto_map.find(node->target) != ctx->goto_map.end()) {
        node->target = ctx->goto_map[node->target];
        ctx->errors->linebuf_map[node->target] = node->line;
    }
    else {
        ctx->goto_map[node->target] = rslv_label_identifier(ctx->identifiers, node->target);
        node->target = ctx->goto_map[node->target];
        ctx->errors->linebuf_map[node->target] = node->line;
    }
}

static error_t reslv_label_statement(Ctx ctx, CLabel* node) {
    CATCH_ENTER;
    TRY(annotate_goto_label(ctx, node));
    if (ctx->goto_map.find(node->target) != ctx->goto_map.end()) {
        node->target = ctx->goto_map[node->target];
    }
    else {
        ctx->goto_map[node->target] = rslv_label_identifier(ctx->identifiers, node->target);
        node->target = ctx->goto_map[node->target];
    }
    TRY(reslv_statement(ctx, node->jump_to.get()));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_compound_statement(Ctx ctx, CCompound* node) {
    CATCH_ENTER;
    enter_scope(ctx);
    TRY(reslv_block(ctx, node->block.get()));
    exit_scope(ctx);
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_while_statement(Ctx ctx, CWhile* node) {
    CATCH_ENTER;
    annotate_while_loop(ctx, node);
    TRY(reslv_typed_exp(ctx, &node->condition));
    TRY(reslv_statement(ctx, node->body.get()));
    deannotate_loop(ctx);
    TRY(check_while_statement(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_do_while_statement(Ctx ctx, CDoWhile* node) {
    CATCH_ENTER;
    annotate_do_while_loop(ctx, node);
    TRY(reslv_statement(ctx, node->body.get()));
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
    TRY(reslv_for_init(ctx, node->init.get()));
    if (node->condition) {
        TRY(reslv_typed_exp(ctx, &node->condition));
    }
    if (node->post) {
        TRY(reslv_typed_exp(ctx, &node->post));
    }
    TRY(reslv_statement(ctx, node->body.get()));
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
        TRY(reslv_statement(ctx, node->body.get()));
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
    TRY(reslv_statement(ctx, node->jump_to.get()));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_default_statement(Ctx ctx, CDefault* node) {
    CATCH_ENTER;
    TRY(annotate_default_jump(ctx, node));
    TRY(reslv_statement(ctx, node->jump_to.get()));
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
    switch (node->type()) {
        case AST_CReturn_t:
            TRY(reslv_ret_statement(ctx, static_cast<CReturn*>(node)));
            break;
        case AST_CExpression_t:
            TRY(reslv_exp_statement(ctx, static_cast<CExpression*>(node)));
            break;
        case AST_CIf_t:
            TRY(reslv_if_statement(ctx, static_cast<CIf*>(node)));
            break;
        case AST_CGoto_t:
            reslv_goto_statement(ctx, static_cast<CGoto*>(node));
            break;
        case AST_CLabel_t:
            TRY(reslv_label_statement(ctx, static_cast<CLabel*>(node)));
            break;
        case AST_CCompound_t:
            TRY(reslv_compound_statement(ctx, static_cast<CCompound*>(node)));
            break;
        case AST_CWhile_t:
            TRY(reslv_while_statement(ctx, static_cast<CWhile*>(node)));
            break;
        case AST_CDoWhile_t:
            TRY(reslv_do_while_statement(ctx, static_cast<CDoWhile*>(node)));
            break;
        case AST_CFor_t:
            TRY(reslv_for_statement(ctx, static_cast<CFor*>(node)));
            break;
        case AST_CSwitch_t:
            TRY(reslv_switch_statement(ctx, static_cast<CSwitch*>(node)));
            break;
        case AST_CCase_t:
            TRY(reslv_case_statement(ctx, static_cast<CCase*>(node)));
            break;
        case AST_CDefault_t:
            TRY(reslv_default_statement(ctx, static_cast<CDefault*>(node)));
            break;
        case AST_CBreak_t:
            TRY(reslv_break_statement(ctx, static_cast<CBreak*>(node)));
            break;
        case AST_CContinue_t:
            TRY(reslv_continue_statement(ctx, static_cast<CContinue*>(node)));
            break;
        case AST_CNull_t:
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_declaration(Ctx ctx, CDeclaration* node);

static error_t reslv_block_items(Ctx ctx, const vector_t(std::unique_ptr<CBlockItem>) node_list) {
    CATCH_ENTER;
    for (size_t i = 0; i < vec_size(node_list); ++i) {
        switch (node_list[i]->type()) {
            case AST_CS_t:
                TRY(reslv_statement(ctx, static_cast<CS*>(node_list[i].get())->statement.get()));
                break;
            case AST_CD_t:
                TRY(reslv_declaration(ctx, static_cast<CD*>(node_list[i].get())->declaration.get()));
                break;
            default:
                THROW_ABORT;
        }
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_block(Ctx ctx, CBlock* node) {
    CATCH_ENTER;
    THROW_ABORT_IF(node->type() != AST_CB_t);
    TRY(reslv_block_items(ctx, static_cast<CB*>(node)->block_items));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_initializer(Ctx ctx, CInitializer* node, std::shared_ptr<Type>& init_type);

static error_t reslv_single_init(Ctx ctx, CSingleInit* node, std::shared_ptr<Type>& init_type) {
    CATCH_ENTER;
    if (node->exp->type() == AST_CString_t && init_type->type() == AST_Array_t) {
        TRY(check_bound_string_init(ctx, static_cast<CString*>(node->exp.get()), static_cast<Array*>(init_type.get())));
        check_string_init(node, init_type);
    }
    else {
        TRY(reslv_typed_exp(ctx, &node->exp));
        TRY(check_single_init(ctx, node, init_type));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_arr_init(Ctx ctx, CCompoundInit* node, Array* arr_type, std::shared_ptr<Type>& init_type) {
    CATCH_ENTER;
    TRY(check_bound_arr_init(ctx, node, arr_type));

    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        TRY(reslv_initializer(ctx, node->initializers[i].get(), arr_type->elem_type));
    }
    check_arr_init(ctx, node, arr_type, init_type);
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct_init(
    Ctx ctx, CCompoundInit* node, Structure* struct_type, std::shared_ptr<Type>& init_type) {
    CATCH_ENTER;
    TRY(check_bound_struct_init(ctx, node, struct_type));

    for (size_t i = 0; i < vec_size(node->initializers); ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        TRY(reslv_initializer(ctx, node->initializers[i].get(), member->member_type));
    }
    check_struct_init(ctx, node, struct_type, init_type);
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_compound_init(Ctx ctx, CCompoundInit* node, std::shared_ptr<Type>& init_type) {
    string_t type_fmt = str_new(NULL);
    CATCH_ENTER;
    switch (init_type->type()) {
        case AST_Array_t:
            TRY(reslv_arr_init(ctx, node, static_cast<Array*>(init_type.get()), init_type));
            break;
        case AST_Structure_t:
            TRY(reslv_struct_init(ctx, node, static_cast<Structure*>(init_type.get()), init_type));
            break;
        default:
            THROW_AT_LINE(get_compound_line(node),
                GET_SEMANTIC_MSG(MSG_scalar_init_with_compound, str_fmt_type(init_type.get(), &type_fmt)));
    }
    FINALLY;
    str_delete(type_fmt);
    CATCH_EXIT;
}

static error_t reslv_initializer(Ctx ctx, CInitializer* node, std::shared_ptr<Type>& init_type) {
    CATCH_ENTER;
    switch (node->type()) {
        case AST_CSingleInit_t:
            TRY(reslv_single_init(ctx, static_cast<CSingleInit*>(node), init_type));
            break;
        case AST_CCompoundInit_t:
            TRY(reslv_compound_init(ctx, static_cast<CCompoundInit*>(node), init_type));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_fun_params_decl(Ctx ctx, CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    for (size_t i = 0; i < vec_size(node->params); ++i) {
        TIdentifier param = node->params[i];
        if (ctx->scoped_identifier_maps.back().find(param) != ctx->scoped_identifier_maps.back().end()) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redecl_var_in_scope, str_fmt_name(param, &name_fmt)));
        }
        ctx->scoped_identifier_maps.back()[param] = rslv_var_identifier(ctx->identifiers, param);
        node->params[i] = ctx->scoped_identifier_maps.back()[param];
    }
    TRY(check_fun_params_decl(ctx, node));
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_fun_declaration(Ctx ctx, CFunctionDeclaration* node) {
    string_t name_fmt = str_new(NULL);
    CATCH_ENTER;
    if (!is_file_scope(ctx)) {
        if (node->body) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_def_nested_fun, str_fmt_name(node->name, &name_fmt)));
        }
        else if (node->storage_class && node->storage_class->type() == AST_CStatic_t) {
            THROW_AT_LINE(
                node->line, GET_SEMANTIC_MSG(MSG_decl_nested_static_fun, str_fmt_name(node->name, &name_fmt)));
        }
    }

    if (ctx->extern_scope_map.find(node->name) == ctx->extern_scope_map.end()) {
        if (ctx->scoped_identifier_maps.back().find(node->name) != ctx->scoped_identifier_maps.back().end()) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redecl_fun_in_scope, str_fmt_name(node->name, &name_fmt)));
        }
        ctx->extern_scope_map[node->name] = ctx->scoped_identifier_maps.size();
    }

    ctx->scoped_identifier_maps.back()[node->name] = node->name;
    TRY(check_ret_fun_decl(ctx, node));

    enter_scope(ctx);
    if (!vec_empty(node->params)) {
        TRY(reslv_fun_params_decl(ctx, node));
    }
    TRY(check_fun_decl(ctx, node));

    if (node->body) {
        TRY(reslv_block(ctx, node->body.get()));
    }
    exit_scope(ctx);
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_file_var_decl(Ctx ctx, CVariableDeclaration* node) {
    CATCH_ENTER;
    if (ctx->extern_scope_map.find(node->name) == ctx->extern_scope_map.end()) {
        ctx->extern_scope_map[node->name] = ctx->scoped_identifier_maps.size();
    }

    ctx->scoped_identifier_maps.back()[node->name] = node->name;
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
    if (ctx->scoped_identifier_maps.back().find(node->name) != ctx->scoped_identifier_maps.back().end()
        && !(ctx->extern_scope_map.find(node->name) != ctx->extern_scope_map.end()
             && (node->storage_class && node->storage_class->type() == AST_CExtern_t))) {
        THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redecl_var_in_scope, str_fmt_name(node->name, &name_fmt)));
    }
    else if (node->storage_class && node->storage_class->type() == AST_CExtern_t) {
        TRY(reslv_file_var_decl(ctx, node));
        EARLY_EXIT;
    }

    ctx->scoped_identifier_maps.back()[node->name] = rslv_var_identifier(ctx->identifiers, node->name);
    node->name = ctx->scoped_identifier_maps.back()[node->name];
    TRY(check_block_var_decl(ctx, node));

    if (node->init && !node->storage_class) {
        TRY(reslv_initializer(ctx, node->init.get(), node->var_type));
    }
    FINALLY;
    str_delete(name_fmt);
    CATCH_EXIT;
}

static error_t reslv_struct_members_decl(Ctx ctx, CStructDeclaration* node) {
    CATCH_ENTER;
    TRY(check_struct_members_decl(ctx, node));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct_declaration(Ctx ctx, CStructDeclaration* node) {
    string_t struct_fmt_1 = str_new(NULL);
    string_t struct_fmt_2 = str_new(NULL);
    CATCH_ENTER;
    if (ctx->scoped_struct_maps.back().find(node->tag) != ctx->scoped_struct_maps.back().end()) {
        node->tag = ctx->scoped_struct_maps.back()[node->tag].tag;
        if (node->is_union) {
            if (ctx->union_def_set.find(node->tag) == ctx->union_def_set.end()) {
                THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redecl_struct_conflict,
                                              str_fmt_struct_name(node->tag, node->is_union, &struct_fmt_1),
                                              str_fmt_struct_name(node->tag, !node->is_union, &struct_fmt_2)));
            }
        }
        else if (ctx->struct_def_set.find(node->tag) == ctx->struct_def_set.end()) {
            THROW_AT_LINE(node->line, GET_SEMANTIC_MSG(MSG_redecl_struct_conflict,
                                          str_fmt_struct_name(node->tag, node->is_union, &struct_fmt_1),
                                          str_fmt_struct_name(node->tag, !node->is_union, &struct_fmt_2)));
        }
    }
    else {
        ctx->scoped_struct_maps.back()[node->tag] = {rslv_struct_tag(ctx->identifiers, node->tag), node->is_union};
        node->tag = ctx->scoped_struct_maps.back()[node->tag].tag;
        if (node->is_union) {
            ctx->union_def_set.insert(node->tag);
        }
        else {
            ctx->struct_def_set.insert(node->tag);
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

static error_t reslv_fun_decl(Ctx ctx, CFunDecl* node) {
    CATCH_ENTER;
    if (is_file_scope(ctx)) {
        ctx->goto_map.clear();
        ctx->label_set.clear();
        ctx->break_loop_labels.clear();
        ctx->continue_loop_labels.clear();
        ctx->p_switch_statement = nullptr;
    }
    TRY(reslv_fun_declaration(ctx, node->fun_decl.get()));
    if (is_file_scope(ctx)) {
        TRY(reslv_label(ctx, node->fun_decl.get()));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_var_decl(Ctx ctx, CVarDecl* node) {
    CATCH_ENTER;
    if (is_file_scope(ctx)) {
        TRY(reslv_file_var_decl(ctx, node->var_decl.get()));
    }
    else {
        TRY(reslv_block_var_decl(ctx, node->var_decl.get()));
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_struct_decl(Ctx ctx, CStructDecl* node) {
    CATCH_ENTER;
    TRY(reslv_struct_declaration(ctx, node->struct_decl.get()));
    FINALLY;
    CATCH_EXIT;
}

static error_t reslv_declaration(Ctx ctx, CDeclaration* node) {
    CATCH_ENTER;
    switch (node->type()) {
        case AST_CFunDecl_t:
            TRY(reslv_fun_decl(ctx, static_cast<CFunDecl*>(node)));
            break;
        case AST_CVarDecl_t:
            TRY(reslv_var_decl(ctx, static_cast<CVarDecl*>(node)));
            break;
        case AST_CStructDecl_t:
            TRY(reslv_struct_decl(ctx, static_cast<CStructDecl*>(node)));
            break;
        default:
            THROW_ABORT;
    }
    FINALLY;
    CATCH_EXIT;
}

static error_t resolve_program(Ctx ctx, CProgram* node) {
    CATCH_ENTER;
    enter_scope(ctx);
    for (const auto& declaration : node->declarations) {
        TRY(reslv_declaration(ctx, declaration.get()));
    }
    FINALLY;
    CATCH_EXIT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

error_t analyze_semantic(
    CProgram* node, ErrorsContext* errors, FrontEndContext* frontend, IdentifierContext* identifiers) {
    SemanticContext ctx;
    {
        ctx.errors = errors;
        ctx.frontend = frontend;
        ctx.identifiers = identifiers;
    }
    CATCH_ENTER;
    TRY(resolve_program(&ctx, node));
    FINALLY;
    CATCH_EXIT;
}
