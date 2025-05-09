#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/errors.hpp"

#include "frontend/intermediate/idents.hpp"
#include "frontend/intermediate/semantic.hpp"

struct SemanticContext {
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
    std::vector<std::shared_ptr<StaticInit>>* p_static_inits;
};

static std::unique_ptr<SemanticContext> ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

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
                RAISE_INTERNAL_ERROR;
            default:
                return true;
        }
    }
    return false;
}

static bool is_same_fun_type(FunType* fun_type_1, FunType* fun_type_2) {
    if (fun_type_1->param_types.size() != fun_type_2->param_types.size()) {
        return false;
    }
    else if (!is_same_type(fun_type_1->ret_type.get(), fun_type_2->ret_type.get())) {
        return false;
    }
    for (size_t i = 0; i < fun_type_1->param_types.size(); ++i) {
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

static bool is_struct_complete(Structure* struct_type) {
    return frontend->struct_typedef_table.find(struct_type->tag) != frontend->struct_typedef_table.end();
}

static bool is_type_complete(Type* type) {
    switch (type->type()) {
        case AST_Void_t:
            return false;
        case AST_Structure_t:
            return is_struct_complete(static_cast<Structure*>(type));
        default:
            return true;
    }
}

static void is_valid_type(Type* type);

static void is_valid_ptr(Pointer* ptr_type) { is_valid_type(ptr_type->ref_type.get()); }

static void is_valid_arr(Array* arr_type) {
    if (!is_type_complete(arr_type->elem_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_incomplete_arr, fmt_type_c_str(arr_type), fmt_type_c_str(arr_type->elem_type.get())),
            errors->linebuf);
    }
    is_valid_type(arr_type->elem_type.get());
}

static void is_valid_type(Type* type) {
    switch (type->type()) {
        case AST_Pointer_t:
            is_valid_ptr(static_cast<Pointer*>(type));
            break;
        case AST_Array_t:
            is_valid_arr(static_cast<Array*>(type));
            break;
        case AST_FunType_t:
            RAISE_INTERNAL_ERROR;
        default:
            break;
    }
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
            RAISE_INTERNAL_ERROR;
    }
}

static TLong get_type_scale(Type* type);

static TLong get_arr_scale(Array* arr_type) {
    TLong size = arr_type->size;
    while (arr_type->elem_type->type() == AST_Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    return get_type_scale(arr_type->elem_type.get()) * size;
}

static TLong get_struct_scale(Structure* struct_type) {
    ABORT_IF(frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end());
    return frontend->struct_typedef_table[struct_type->tag]->size;
}

static TLong get_type_scale(Type* type) {
    switch (type->type()) {
        case AST_Array_t:
            return get_arr_scale(static_cast<Array*>(type));
        case AST_Structure_t:
            return get_struct_scale(static_cast<Structure*>(type));
        default:
            return get_scalar_size(type);
    }
}

static TInt get_type_alignment(Type* type);

static TInt get_arr_alignment(Array* arr_type) { return get_type_alignment(arr_type->elem_type.get()); }

static TInt get_struct_alignment(Structure* struct_type) {
    ABORT_IF(frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end());
    return frontend->struct_typedef_table[struct_type->tag]->alignment;
}

static TInt get_type_alignment(Type* type) {
    switch (type->type()) {
        case AST_Array_t:
            return get_arr_alignment(static_cast<Array*>(type));
        case AST_Structure_t:
            return get_struct_alignment(static_cast<Structure*>(type));
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
        std::shared_ptr<Type> exp_type_2 = std::move(node_2->exp_type);
        node_2->exp_type = std::make_shared<Int>();
        std::shared_ptr<Type> joint_type = get_joint_type(node_1, node_2);
        node_2->exp_type = std::move(exp_type_2);
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

static std::shared_ptr<Type> get_joint_ptr_type(CExp* node_1, CExp* node_2) {
    if (is_same_type(node_1->exp_type.get(), node_2->exp_type.get())) {
        return node_1->exp_type;
    }
    else if (node_1->type() == AST_CConstant_t && is_const_null_ptr(static_cast<CConstant*>(node_1))) {
        return node_2->exp_type;
    }
    else if (node_2->type() == AST_CConstant_t && is_const_null_ptr(static_cast<CConstant*>(node_2))) {
        return node_1->exp_type;
    }
    else if (node_1->exp_type->type() == AST_Pointer_t
             && static_cast<Pointer*>(node_1->exp_type.get())->ref_type->type() == AST_Void_t
             && node_2->exp_type->type() == AST_Pointer_t) {
        return node_1->exp_type;
    }
    else if (node_2->exp_type->type() == AST_Pointer_t
             && static_cast<Pointer*>(node_2->exp_type.get())->ref_type->type() == AST_Void_t
             && node_1->exp_type->type() == AST_Pointer_t) {
        return node_2->exp_type;
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_joint_ptr_mismatch, fmt_type_c_str(node_1->exp_type.get()),
                                        fmt_type_c_str(node_2->exp_type.get())),
            node_1->line);
    }
}

static TChar get_const_char_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<CConstChar*>(node->constant.get())->value;
        case AST_CConstInt_t:
            return static_cast<TChar>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<TChar>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return static_cast<TChar>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return static_cast<TChar>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<TChar>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<TChar>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TInt get_const_int_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<TInt>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return static_cast<CConstInt*>(node->constant.get())->value;
        case AST_CConstLong_t:
            return static_cast<TInt>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return static_cast<TInt>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return static_cast<TInt>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<TInt>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<TInt>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TLong get_const_long_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<TLong>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return static_cast<TLong>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<CConstLong*>(node->constant.get())->value;
        case AST_CConstDouble_t:
            return static_cast<TLong>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return static_cast<TLong>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<TLong>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<TLong>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TDouble get_const_dbl_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<TDouble>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return static_cast<TDouble>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<TDouble>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return static_cast<CConstDouble*>(node->constant.get())->value;
        case AST_CConstUChar_t:
            return static_cast<TDouble>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<TDouble>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<TDouble>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TUChar get_const_uchar_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<TUChar>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return static_cast<TUChar>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<TUChar>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return static_cast<TUChar>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return static_cast<CConstUChar*>(node->constant.get())->value;
        case AST_CConstUInt_t:
            return static_cast<TUChar>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<TUChar>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TUInt get_const_uint_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<TUInt>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return static_cast<TUInt>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<TUInt>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return static_cast<TUInt>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return static_cast<TUInt>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<CConstUInt*>(node->constant.get())->value;
        case AST_CConstULong_t:
            return static_cast<TUInt>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TULong get_const_ulong_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
            return static_cast<TULong>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_CConstInt_t:
            return static_cast<TULong>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<TULong>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstDouble_t:
            return static_cast<TULong>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_CConstUChar_t:
            return static_cast<TULong>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<TULong>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TULong get_const_ptr_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstInt_t:
            return static_cast<TULong>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_CConstLong_t:
            return static_cast<TULong>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_CConstUInt_t:
            return static_cast<TULong>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static size_t get_compound_line(CInitializer* node) {
    ABORT_IF(node->type() != AST_CCompoundInit_t);
    do {
        node = static_cast<CCompoundInit*>(node)->initializers[0].get();
    }
    while (node->type() == AST_CCompoundInit_t);
    ABORT_IF(node->type() != AST_CSingleInit_t);
    return static_cast<CSingleInit*>(node)->exp->line;
}

static void reslv_struct_type(Type* type);

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
            RAISE_INTERNAL_ERROR;
    }
}

static void check_string_exp(CString* node) {
    TLong size = static_cast<TLong>(node->literal->value.size()) + 1l;
    std::shared_ptr<Type> elem_type = std::make_shared<Char>();
    node->exp_type = std::make_shared<Array>(std::move(size), std::move(elem_type));
}

static void check_var_exp(CVar* node) {
    if (frontend->symbol_table[node->name]->type_t->type() == AST_FunType_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_fun_used_as_var, fmt_name_c_str(node->name)), node->line);
    }
    node->exp_type = frontend->symbol_table[node->name]->type_t;
}

static void check_cast_exp(CCast* node) {
    errors->linebuf = node->line;
    reslv_struct_type(node->target_type.get());
    if (node->target_type->type() != AST_Void_t
        && ((node->exp->exp_type->type() == AST_Double_t && node->target_type->type() == AST_Pointer_t)
            || (node->exp->exp_type->type() == AST_Pointer_t && node->target_type->type() == AST_Double_t)
            || !is_type_scalar(node->exp->exp_type.get()) || !is_type_scalar(node->target_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_illegal_cast, fmt_type_c_str(node->exp->exp_type.get()),
                                        fmt_type_c_str(node->target_type.get())),
            node->line);
    }
    is_valid_type(node->target_type.get());
    node->exp_type = node->target_type;
}

static std::unique_ptr<CCast> cast_exp(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    size_t line = node->line;
    std::unique_ptr<CCast> exp = std::make_unique<CCast>(std::move(node), exp_type, std::move(line));
    check_cast_exp(exp.get());
    return exp;
}

static std::unique_ptr<CCast> cast_assign(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    if (is_type_arithmetic(node->exp_type.get()) && is_type_arithmetic(exp_type.get())) {
        return cast_exp(std::move(node), exp_type);
    }
    else if (node->type() == AST_CConstant_t && exp_type->type() == AST_Pointer_t
             && is_const_null_ptr(static_cast<CConstant*>(node.get()))) {
        return cast_exp(std::move(node), exp_type);
    }
    else if (exp_type->type() == AST_Pointer_t && static_cast<Pointer*>(exp_type.get())->ref_type->type() == AST_Void_t
             && node->exp_type->type() == AST_Pointer_t) {
        return cast_exp(std::move(node), exp_type);
    }
    else if (node->exp_type->type() == AST_Pointer_t
             && static_cast<Pointer*>(node->exp_type.get())->ref_type->type() == AST_Void_t
             && exp_type->type() == AST_Pointer_t) {
        return cast_exp(std::move(node), exp_type);
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_illegal_cast, fmt_type_c_str(node->exp_type.get()), fmt_type_c_str(exp_type.get())),
            node->line);
    }
}

static void check_unary_complement_exp(CUnary* node) {
    if (!is_type_arithmetic(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_invalid_unary_op, fmt_unop_c_str(node->unop.get()),
                                        fmt_type_c_str(node->exp->exp_type.get())),
            node->line);
    }

    switch (node->exp->exp_type->type()) {
        case AST_Double_t:
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_invalid_unary_op, fmt_unop_c_str(node->unop.get()),
                                            fmt_type_c_str(node->exp->exp_type.get())),
                node->line);
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t: {
            std::shared_ptr<Type> promote_type = std::make_shared<Int>();
            node->exp = cast_exp(std::move(node->exp), promote_type);
            break;
        }
        default:
            break;
    }
    node->exp_type = node->exp->exp_type;
}

static void check_unary_neg_exp(CUnary* node) {
    if (!is_type_arithmetic(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_invalid_unary_op, fmt_unop_c_str(node->unop.get()),
                                        fmt_type_c_str(node->exp->exp_type.get())),
            node->line);
    }

    switch (node->exp->exp_type->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t: {
            std::shared_ptr<Type> promote_type = std::make_shared<Int>();
            node->exp = cast_exp(std::move(node->exp), promote_type);
            break;
        }
        default:
            break;
    }
    node->exp_type = node->exp->exp_type;
}

static void check_unary_not_exp(CUnary* node) {
    if (!is_type_scalar(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_invalid_unary_op, fmt_unop_c_str(node->unop.get()),
                                        fmt_type_c_str(node->exp->exp_type.get())),
            node->line);
    }

    node->exp_type = std::make_shared<Int>();
}

static void check_unary_exp(CUnary* node) {
    switch (node->unop->type()) {
        case AST_CComplement_t:
            check_unary_complement_exp(node);
            break;
        case AST_CNegate_t:
            check_unary_neg_exp(node);
            break;
        case AST_CNot_t:
            check_unary_not_exp(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void check_binary_add_exp(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if (is_type_arithmetic(node->exp_left->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (node->exp_left->exp_type->type() == AST_Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())
             && is_type_int(node->exp_right->exp_type.get())) {
        common_type = std::make_shared<Long>();
        if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
            node->exp_right = cast_exp(std::move(node->exp_right), common_type);
        }
        node->exp_type = node->exp_left->exp_type;
        return;
    }
    else if (is_type_int(node->exp_left->exp_type.get()) && node->exp_right->exp_type->type() == AST_Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->exp_right->exp_type.get())->ref_type.get())) {
        common_type = std::make_shared<Long>();
        if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
            node->exp_left = cast_exp(std::move(node->exp_left), common_type);
        }
        node->exp_type = node->exp_right->exp_type;
        return;
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_exp(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void check_binary_subtract_exp(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if (is_type_arithmetic(node->exp_left->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (node->exp_left->exp_type->type() == AST_Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())) {
        if (is_type_int(node->exp_right->exp_type.get())) {
            common_type = std::make_shared<Long>();
            if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
                node->exp_right = cast_exp(std::move(node->exp_right), common_type);
            }
            node->exp_type = node->exp_left->exp_type;
            return;
        }
        else if (is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())
                 && !(node->exp_left->type() == AST_CConstant_t
                      && is_const_null_ptr(static_cast<CConstant*>(node->exp_left.get())))) {
            common_type = std::make_shared<Long>();
            node->exp_type = std::move(common_type);
            return;
        }
        else {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                    fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
                node->line);
        }
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_exp(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void check_multiply_divide_exp(CBinary* node) {
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_arithmetic(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_exp(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void check_remainder_bitwise_exp(CBinary* node) {
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_arithmetic(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_exp(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
    if (node->exp_type->type() == AST_Double_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_invalid_binary_op, fmt_binop_c_str(node->binop.get()),
                                        fmt_type_c_str(node->exp_type.get())),
            node->line);
    }
}

static void check_binary_bitshift_exp(CBinary* node) {
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_int(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    else if (is_type_char(node->exp_left->exp_type.get())) {
        std::shared_ptr<Type> left_type = std::make_shared<Int>();
        node->exp_left = cast_exp(std::move(node->exp_left), left_type);
    }
    if (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), node->exp_left->exp_type);
    }
    node->exp_type = node->exp_left->exp_type;
    if (node->exp_type->type() == AST_Double_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_invalid_binary_op, fmt_binop_c_str(node->binop.get()),
                                        fmt_type_c_str(node->exp_type.get())),
            node->line);
    }
}

static void check_bitshift_right_exp(CBinary* node) {
    check_binary_bitshift_exp(node);
    if (is_type_signed(node->exp_left->exp_type.get())) {
        node->binop = std::make_unique<CBitShrArithmetic>();
    }
}

static void check_binary_logical_exp(CBinary* node) {
    if (!is_type_scalar(node->exp_left->exp_type.get()) || !is_type_scalar(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    node->exp_type = std::make_shared<Int>();
}

static void check_binary_equality_exp(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if (node->exp_left->exp_type->type() == AST_Pointer_t || node->exp_right->exp_type->type() == AST_Pointer_t) {
        common_type = get_joint_ptr_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (is_type_arithmetic(node->exp_left->exp_type.get())
             && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_exp(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::make_shared<Int>();
}

static void check_binary_relational_exp(CBinary* node) {
    if (!is_type_scalar(node->exp_left->exp_type.get()) || !is_type_scalar(node->exp_right->exp_type.get())
        || (node->exp_left->exp_type->type() == AST_Pointer_t
            && (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())
                || (node->exp_left->type() == AST_CConstant_t
                    && is_const_null_ptr(static_cast<CConstant*>(node->exp_left.get())))
                || (node->exp_right->type() == AST_CConstant_t
                    && is_const_null_ptr(static_cast<CConstant*>(node->exp_right.get())))))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_binary_ops, fmt_binop_c_str(node->binop.get()),
                fmt_type_c_str(node->exp_left->exp_type.get()), fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_exp(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::make_shared<Int>();
}

static void check_binary_exp(CBinary* node) {
    switch (node->binop->type()) {
        case AST_CAdd_t:
            check_binary_add_exp(node);
            break;
        case AST_CSubtract_t:
            check_binary_subtract_exp(node);
            break;
        case AST_CMultiply_t:
        case AST_CDivide_t:
            check_multiply_divide_exp(node);
            break;
        case AST_CRemainder_t:
        case AST_CBitAnd_t:
        case AST_CBitOr_t:
        case AST_CBitXor_t:
            check_remainder_bitwise_exp(node);
            break;
        case AST_CBitShiftLeft_t:
            check_binary_bitshift_exp(node);
            break;
        case AST_CBitShiftRight_t:
            check_bitshift_right_exp(node);
            break;
        case AST_CAnd_t:
        case AST_COr_t:
            check_binary_logical_exp(node);
            break;
        case AST_CEqual_t:
        case AST_CNotEqual_t:
            check_binary_equality_exp(node);
            break;
        case AST_CLessThan_t:
        case AST_CLessOrEqual_t:
        case AST_CGreaterThan_t:
        case AST_CGreaterOrEqual_t:
            check_binary_relational_exp(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void check_assign_exp(CAssignment* node) {
    if (node->exp_left) {
        if (node->exp_left->exp_type->type() == AST_Void_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG_0(MSG_assign_to_void), node->line);
        }
        else if (!is_exp_lvalue(node->exp_left.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_assign_to_rvalue, fmt_assign_c_str(nullptr, node->unop.get())), node->line);
        }
        else if (!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
            node->exp_right = cast_assign(std::move(node->exp_right), node->exp_left->exp_type);
        }
        node->exp_type = node->exp_left->exp_type;
    }
    else {
        ABORT_IF(node->exp_right->type() != AST_CBinary_t);
        CExp* exp_left = static_cast<CBinary*>(node->exp_right.get())->exp_left.get();
        if (exp_left->type() == AST_CCast_t) {
            exp_left = static_cast<CCast*>(exp_left)->exp.get();
        }
        if (!is_exp_lvalue(exp_left)) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_assign_to_rvalue,
                    fmt_assign_c_str(static_cast<CBinary*>(node->exp_right.get())->binop.get(), node->unop.get())),
                node->line);
        }
        else if (!is_same_type(node->exp_right->exp_type.get(), exp_left->exp_type.get())) {
            node->exp_right = cast_assign(std::move(node->exp_right), exp_left->exp_type);
        }
        node->exp_type = exp_left->exp_type;
    }
}

static void check_conditional_exp(CConditional* node) {
    if (!is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_condition, fmt_type_c_str(node->condition->exp_type.get())), node->line);
    }
    else if (node->exp_middle->exp_type->type() == AST_Void_t && node->exp_right->exp_type->type() == AST_Void_t) {
        node->exp_type = node->exp_middle->exp_type;
        return;
    }
    else if (node->exp_middle->exp_type->type() == AST_Structure_t
             || node->exp_right->exp_type->type() == AST_Structure_t) {
        if (!is_same_type(node->exp_middle->exp_type.get(), node->exp_right->exp_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_invalid_ternary_op, fmt_type_c_str(node->exp_middle->exp_type.get()),
                    fmt_type_c_str(node->exp_right->exp_type.get())),
                node->line);
        }
        node->exp_type = node->exp_middle->exp_type;
        return;
    }

    std::shared_ptr<Type> common_type;
    if (is_type_arithmetic(node->exp_middle->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_middle.get(), node->exp_right.get());
    }
    else if (node->exp_middle->exp_type->type() == AST_Pointer_t
             || node->exp_right->exp_type->type() == AST_Pointer_t) {
        common_type = get_joint_ptr_type(node->exp_middle.get(), node->exp_right.get());
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_ternary_op, fmt_type_c_str(node->exp_middle->exp_type.get()),
                fmt_type_c_str(node->exp_right->exp_type.get())),
            node->line);
    }
    if (!is_same_type(node->exp_middle->exp_type.get(), common_type.get())) {
        node->exp_middle = cast_exp(std::move(node->exp_middle), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_exp(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void check_call_exp(CFunctionCall* node) {
    if (frontend->symbol_table[node->name]->type_t->type() != AST_FunType_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_var_used_as_fun, fmt_name_c_str(node->name)), node->line);
    }
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    if (fun_type->param_types.size() != node->args.size()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_call_with_wrong_argc, fmt_name_c_str(node->name),
                std::to_string(node->args.size()).c_str(), std::to_string(fun_type->param_types.size()).c_str()),
            node->line);
    }
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (!is_same_type(node->args[i]->exp_type.get(), fun_type->param_types[i].get())) {
            node->args[i] = cast_assign(std::move(node->args[i]), fun_type->param_types[i]);
        }
    }
    node->exp_type = fun_type->ret_type;
}

static void check_deref_exp(CDereference* node) {
    if (node->exp->exp_type->type() != AST_Pointer_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_deref_not_ptr, fmt_type_c_str(node->exp->exp_type.get())), node->line);
    }
    node->exp_type = static_cast<Pointer*>(node->exp->exp_type.get())->ref_type;
}

static void check_addrof_exp(CAddrOf* node) {
    if (!is_exp_lvalue(node->exp.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG_0(MSG_addrof_rvalue), node->line);
    }
    std::shared_ptr<Type> ref_type = node->exp->exp_type;
    node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
}

static void check_subscript_exp(CSubscript* node) {
    std::shared_ptr<Type> ref_type;
    if (node->primary_exp->exp_type->type() == AST_Pointer_t
        && is_type_complete(static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type.get())
        && is_type_int(node->subscript_exp->exp_type.get())) {
        std::shared_ptr<Type> subscript_type = std::make_shared<Long>();
        if (!is_same_type(node->subscript_exp->exp_type.get(), subscript_type.get())) {
            node->subscript_exp = cast_exp(std::move(node->subscript_exp), subscript_type);
        }
        ref_type = static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type;
    }
    else if (is_type_int(node->primary_exp->exp_type.get()) && node->subscript_exp->exp_type->type() == AST_Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type.get())) {
        std::shared_ptr<Type> primary_type = std::make_shared<Long>();
        if (!is_same_type(node->primary_exp->exp_type.get(), primary_type.get())) {
            node->primary_exp = cast_exp(std::move(node->primary_exp), primary_type);
        }
        ref_type = static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type;
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_subscript, fmt_type_c_str(node->primary_exp->exp_type.get()),
                fmt_type_c_str(node->subscript_exp->exp_type.get())),
            node->line);
    }
    node->exp_type = std::move(ref_type);
}

static void check_sizeof_exp(CSizeOf* node) {
    if (!is_type_complete(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_sizeof_incomplete, fmt_type_c_str(node->exp->exp_type.get())), node->line);
    }
    node->exp_type = std::make_shared<ULong>();
}

static void check_sizeoft_exp(CSizeOfT* node) {
    errors->linebuf = node->line;
    reslv_struct_type(node->target_type.get());
    if (!is_type_complete(node->target_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_sizeof_incomplete, fmt_type_c_str(node->target_type.get())), node->line);
    }
    is_valid_type(node->target_type.get());
    node->exp_type = std::make_shared<ULong>();
}

static void check_dot_exp(CDot* node) {
    if (node->structure->exp_type->type() != AST_Structure_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_dot_not_struct, fmt_name_c_str(node->member),
                                        fmt_type_c_str(node->structure->exp_type.get())),
            node->line);
    }
    Structure* struct_type = static_cast<Structure*>(node->structure->exp_type.get());
    if (frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
        == frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_member_not_in_struct, fmt_type_c_str(struct_type), fmt_name_c_str(node->member)),
            node->line);
    }
    node->exp_type = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
}

static void check_arrow_exp(CArrow* node) {
    if (node->pointer->exp_type->type() != AST_Pointer_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_arrow_not_struct_ptr, fmt_name_c_str(node->member),
                                        fmt_type_c_str(node->pointer->exp_type.get())),
            node->line);
    }
    Pointer* ptr_type = static_cast<Pointer*>(node->pointer->exp_type.get());
    if (ptr_type->ref_type->type() != AST_Structure_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_arrow_not_struct_ptr, fmt_name_c_str(node->member),
                                        fmt_type_c_str(node->pointer->exp_type.get())),
            node->line);
    }
    Structure* struct_type = static_cast<Structure*>(ptr_type->ref_type.get());
    if (frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_arrow_incomplete, fmt_name_c_str(node->member), fmt_type_c_str(struct_type)),
            node->line);
    }
    else if (frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
             == frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_member_not_in_struct, fmt_type_c_str(struct_type), fmt_name_c_str(node->member)),
            node->line);
    }
    node->exp_type = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
}

static std::unique_ptr<CExp> check_scalar_typed_exp(std::unique_ptr<CExp>&& node) {
    std::unique_ptr<CExp> exp = std::move(node);
    return exp;
}

static std::unique_ptr<CAddrOf> check_arr_typed_exp(std::unique_ptr<CExp>&& node) {
    {
        std::shared_ptr<Type> ref_type = static_cast<Array*>(node->exp_type.get())->elem_type;
        node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
    }
    size_t line = node->line;
    std::unique_ptr<CAddrOf> addrof = std::make_unique<CAddrOf>(std::move(node), std::move(line));
    addrof->exp_type = addrof->exp->exp_type;
    return addrof;
}

static std::unique_ptr<CExp> check_struct_typed_exp(std::unique_ptr<CExp>&& node) {
    if (!is_struct_complete(static_cast<Structure*>(node->exp_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_exp_incomplete, fmt_type_c_str(node->exp_type.get())), node->line);
    }

    std::unique_ptr<CExp> exp = std::move(node);
    return exp;
}

static std::unique_ptr<CExp> check_typed_exp(std::unique_ptr<CExp>&& node) {
    switch (node->exp_type->type()) {
        case AST_Array_t:
            return check_arr_typed_exp(std::move(node));
        case AST_Structure_t:
            return check_struct_typed_exp(std::move(node));
        default:
            return check_scalar_typed_exp(std::move(node));
    }
}

static void check_ret_statement(CReturn* node) {
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[ctx->fun_def_name]->type_t.get());
    if (fun_type->ret_type->type() == AST_Void_t) {
        if (node->exp) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_ret_value_in_void_fun, fmt_name_c_str(ctx->fun_def_name)), node->line);
        }
        return;
    }
    else if (!node->exp) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_no_ret_value_in_fun, fmt_name_c_str(ctx->fun_def_name),
                                        fmt_type_c_str(fun_type->ret_type.get())),
            node->line);
    }

    else if (!is_same_type(node->exp->exp_type.get(), fun_type->ret_type.get())) {
        node->exp = cast_assign(std::move(node->exp), fun_type->ret_type);
    }
    node->exp = check_typed_exp(std::move(node->exp));
}

static void check_if_statement(CIf* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_if, fmt_type_c_str(node->condition->exp_type.get())), node->condition->line);
    }
}

static void check_while_statement(CWhile* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_while, fmt_type_c_str(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void check_do_while_statement(CDoWhile* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_do_while, fmt_type_c_str(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void check_for_statement(CFor* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_for, fmt_type_c_str(node->condition->exp_type.get())), node->condition->line);
    }
}

static void check_switch_statement(CSwitch* node) {
    if (!is_type_int(node->match->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_invalid_switch, fmt_type_c_str(node->match->exp_type.get())), node->match->line);
    }
    switch (node->match->exp_type->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t: {
            std::shared_ptr<Type> promote_type = std::make_shared<Int>();
            node->match = cast_exp(std::move(node->match), promote_type);
            break;
        }
        default:
            break;
    }
    switch (node->match->exp_type->type()) {
        case AST_Int_t: {
            std::vector<TInt> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_int_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MSG(MSG_duplicate_case_value, std::to_string(values[i]).c_str()),
                            node->cases[i]->line);
                    }
                }
                esac->constant = std::make_shared<CConstInt>(values[i]);
                esac->exp_type = node->match->exp_type;
            }
            break;
        }
        case AST_Long_t: {
            std::vector<TLong> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_long_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MSG(MSG_duplicate_case_value, std::to_string(values[i]).c_str()),
                            node->cases[i]->line);
                    }
                }
                esac->constant = std::make_shared<CConstLong>(values[i]);
                esac->exp_type = node->match->exp_type;
            }
            break;
        }
        case AST_UInt_t: {
            std::vector<TUInt> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_uint_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MSG(MSG_duplicate_case_value, std::to_string(values[i]).c_str()),
                            node->cases[i]->line);
                    }
                }
                esac->constant = std::make_shared<CConstUInt>(values[i]);
                esac->exp_type = node->match->exp_type;
            }
            break;
        }
        case AST_ULong_t: {
            std::vector<TULong> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                ABORT_IF(node->cases[i]->type() != AST_CConstant_t);
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_ulong_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MSG(MSG_duplicate_case_value, std::to_string(values[i]).c_str()),
                            node->cases[i]->line);
                    }
                }
                esac->constant = std::make_shared<CConstULong>(values[i]);
                esac->exp_type = node->match->exp_type;
            }
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void check_bound_string_init(CString* node, Array* arr_type) {
    if (!is_type_char(arr_type->elem_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_string_init_not_char_arr, fmt_type_c_str(arr_type)), node->line);
    }
    else if (node->literal->value.size() > static_cast<size_t>(arr_type->size)) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_string_init_overflow, std::to_string(arr_type->size).c_str(),
                                        std::to_string(node->literal->value.size()).c_str()),
            node->line);
    }
}

static void check_single_init(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    if (!is_same_type(node->exp->exp_type.get(), init_type.get())) {
        node->exp = cast_assign(std::move(node->exp), init_type);
    }
    node->init_type = init_type;
}

static void check_string_init(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    node->exp->exp_type = init_type;
    node->init_type = init_type;
}

static std::unique_ptr<CInitializer> check_zero_init(Type* init_type);

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
                RAISE_INTERNAL_ERROR;
        }
        exp = std::make_unique<CConstant>(std::move(constant), 0);
    }
    return std::make_unique<CSingleInit>(std::move(exp));
}

static std::unique_ptr<CCompoundInit> check_arr_zero_init(Array* arr_type) {
    std::vector<std::unique_ptr<CInitializer>> zero_inits;
    size_t arr_type_size = static_cast<size_t>(arr_type->size);
    zero_inits.reserve(arr_type_size);
    for (size_t i = 0; i < arr_type_size; ++i) {
        std::unique_ptr<CInitializer> initializer = check_zero_init(arr_type->elem_type.get());
        zero_inits.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_inits));
}

static std::unique_ptr<CCompoundInit> check_struct_zero_init(Structure* struct_type) {
    std::vector<std::unique_ptr<CInitializer>> zero_inits;
    zero_inits.reserve(frontend->struct_typedef_table[struct_type->tag]->member_names.size());
    for (TIdentifier member_name : frontend->struct_typedef_table[struct_type->tag]->member_names) {
        const auto& member = frontend->struct_typedef_table[struct_type->tag]->members[member_name];
        std::unique_ptr<CInitializer> initializer = check_zero_init(member->member_type.get());
        zero_inits.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_inits));
}

static std::unique_ptr<CInitializer> check_zero_init(Type* init_type) {
    switch (init_type->type()) {
        case AST_Array_t:
            return check_arr_zero_init(static_cast<Array*>(init_type));
        case AST_Structure_t:
            return check_struct_zero_init(static_cast<Structure*>(init_type));
        default:
            return check_single_zero_init(init_type);
    }
}

static void check_bound_arr_init(CCompoundInit* node, Array* arr_type) {
    if (node->initializers.size() > static_cast<size_t>(arr_type->size)) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_arr_init_overflow, std::to_string(arr_type->size).c_str(),
                                        fmt_type_c_str(arr_type), std::to_string(node->initializers.size()).c_str()),
            get_compound_line(node));
    }
}

static void check_bound_struct_init(CCompoundInit* node, Structure* struct_type) {
    size_t bound = struct_type->is_union ? 1 : frontend->struct_typedef_table[struct_type->tag]->members.size();
    if (node->initializers.size() > bound) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_struct_init_overflow, fmt_type_c_str(struct_type),
                std::to_string(node->initializers.size()).c_str(), std::to_string(bound).c_str()),
            get_compound_line(node));
    }
}

static void check_arr_init(CCompoundInit* node, Array* arr_type, std::shared_ptr<Type>& init_type) {
    while (node->initializers.size() < static_cast<size_t>(arr_type->size)) {
        std::unique_ptr<CInitializer> zero_init = check_zero_init(arr_type->elem_type.get());
        node->initializers.push_back(std::move(zero_init));
    }
    node->init_type = init_type;
}

static void check_struct_init(CCompoundInit* node, Structure* struct_type, std::shared_ptr<Type>& init_type) {
    for (size_t i = node->initializers.size(); i < frontend->struct_typedef_table[struct_type->tag]->members.size();
         ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        std::unique_ptr<CInitializer> zero_init = check_zero_init(member->member_type.get());
        node->initializers.push_back(std::move(zero_init));
    }
    node->init_type = init_type;
}

static void check_ret_fun_decl(CFunctionDeclaration* node) {
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    errors->linebuf = node->line;
    reslv_struct_type(fun_type->ret_type.get());
    is_valid_type(fun_type->ret_type.get());

    switch (fun_type->ret_type->type()) {
        case AST_Array_t:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_ret_arr, fmt_name_c_str(node->name), fmt_type_c_str(fun_type->ret_type.get())),
                node->line);
        case AST_Structure_t: {
            if (node->body && !is_struct_complete(static_cast<Structure*>(fun_type->ret_type.get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_ret_incomplete, fmt_name_c_str(node->name),
                                                fmt_type_c_str(fun_type->ret_type.get())),
                    node->line);
            }
            break;
        }
        default:
            break;
    }
}

static void check_fun_params_decl(CFunctionDeclaration* node) {
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    for (size_t i = 0; i < node->params.size(); ++i) {
        errors->linebuf = node->line;
        reslv_struct_type(fun_type->param_types[i].get());
        if (fun_type->param_types[i]->type() == AST_Void_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_void_param, fmt_name_c_str(node->name), fmt_name_c_str(node->params[i])),
                node->line);
        }
        is_valid_type(fun_type->param_types[i].get());
        if (fun_type->param_types[i]->type() == AST_Array_t) {
            std::shared_ptr<Type> ref_type = static_cast<Array*>(fun_type->param_types[i].get())->elem_type;
            fun_type->param_types[i] = std::make_shared<Pointer>(std::move(ref_type));
        }

        if (node->body) {
            if (fun_type->param_types[i]->type() == AST_Structure_t
                && !is_struct_complete(static_cast<Structure*>(fun_type->param_types[i].get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MSG(MSG_incomplete_param, fmt_name_c_str(node->name), fmt_name_c_str(node->params[i]),
                        fmt_type_c_str(fun_type->param_types[i].get())),
                    node->line);
            }
            std::shared_ptr<Type> type_t = fun_type->param_types[i];
            std::unique_ptr<IdentifierAttr> param_attrs = std::make_unique<LocalAttr>();
            frontend->symbol_table[node->params[i]] =
                std::make_unique<Symbol>(std::move(type_t), std::move(param_attrs));
        }
    }
}

static void check_fun_decl(CFunctionDeclaration* node) {
    ABORT_IF(node->fun_type->type() == AST_Void_t);

    bool is_def = ctx->fun_def_set.find(node->name) != ctx->fun_def_set.end();
    bool is_glob = !(node->storage_class && node->storage_class->type() == AST_CStatic_t);

    if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
        if (!(frontend->symbol_table[node->name]->type_t->type() == AST_FunType_t
                && fun_type->param_types.size() == node->params.size()
                && is_same_fun_type(static_cast<FunType*>(node->fun_type.get()), fun_type))) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_redecl_fun_conflict, fmt_name_c_str(node->name),
                                            fmt_type_c_str(node->fun_type.get()), fmt_type_c_str(fun_type)),
                node->line);
        }
        else if (is_def && node->body) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_redef_fun, fmt_name_c_str(node->name), fmt_type_c_str(node->fun_type.get())),
                node->line);
        }

        FunAttr* fun_attrs = static_cast<FunAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if (!is_glob && fun_attrs->is_glob) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_redecl_static_conflict, fmt_name_c_str(node->name)), node->line);
        }
        is_glob = fun_attrs->is_glob;
    }

    if (node->body) {
        ctx->fun_def_set.insert(node->name);
        is_def = true;
        ctx->fun_def_name = node->name;
    }

    std::shared_ptr<Type> fun_type = node->fun_type;
    std::unique_ptr<IdentifierAttr> fun_attrs = std::make_unique<FunAttr>(std::move(is_def), std::move(is_glob));
    frontend->symbol_table[node->name] = std::make_unique<Symbol>(std::move(fun_type), std::move(fun_attrs));
}

static void push_static_init(std::shared_ptr<StaticInit>&& static_init) {
    ctx->p_static_inits->push_back(std::move(static_init));
}

static void push_zero_static_init(TLong&& byte) {
    if (!ctx->p_static_inits->empty() && ctx->p_static_inits->back()->type() == AST_ZeroInit_t) {
        static_cast<ZeroInit*>(ctx->p_static_inits->back().get())->byte += byte;
    }
    else {
        push_static_init(std::make_shared<ZeroInit>(std::move(byte)));
    }
}

static void check_static_init(CInitializer* node, Type* static_init_type);

static void check_static_no_init(Type* static_init_type, TLong size) {
    TLong byte = static_init_type == nullptr ? size : get_type_scale(static_init_type) * size;
    push_zero_static_init(std::move(byte));
}

static std::shared_ptr<Initial> check_no_initializer(Type* static_init_type) {
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    {
        ctx->p_static_inits = &static_inits;
        check_static_no_init(static_init_type, 1l);
        ctx->p_static_inits = nullptr;
    }
    return std::make_shared<Initial>(std::move(static_inits));
}

static void check_static_const_init(CConstant* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = get_const_char_value(node);
            if (value == 0) {
                push_zero_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<CharInit>(std::move(value)));
            }
            break;
        }
        case AST_Int_t: {
            TInt value = get_const_int_value(node);
            if (value == 0) {
                push_zero_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<IntInit>(std::move(value)));
            }
            break;
        }
        case AST_Long_t: {
            TLong value = get_const_long_value(node);
            if (value == 0l) {
                push_zero_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<LongInit>(std::move(value)));
            }
            break;
        }
        case AST_Double_t: {
            TDouble value = get_const_dbl_value(node);
            TULong binary = dbl_to_binary(value);
            if (binary == 0ul) {
                push_zero_static_init(8l);
            }
            else {
                TIdentifier dbl_const = make_string_identifier(std::to_string(binary));
                push_static_init(std::make_shared<DoubleInit>(std::move(dbl_const)));
            }
            break;
        }
        case AST_UChar_t: {
            TUChar value = get_const_uchar_value(node);
            if (value == 0u) {
                push_zero_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<UCharInit>(std::move(value)));
            }
            break;
        }
        case AST_UInt_t: {
            TUInt value = get_const_uint_value(node);
            if (value == 0u) {
                push_zero_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<UIntInit>(std::move(value)));
            }
            break;
        }
        case AST_ULong_t: {
            TULong value = get_const_ulong_value(node);
            if (value == 0ul) {
                push_zero_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<ULongInit>(std::move(value)));
            }
            break;
        }
        case AST_Pointer_t: {
            switch (node->constant->type()) {
                case AST_CConstChar_t:
                case AST_CConstDouble_t:
                case AST_CConstUChar_t:
                    RAISE_RUNTIME_ERROR_AT_LINE(
                        GET_SEMANTIC_MSG(MSG_static_ptr_init_not_int, fmt_type_c_str(static_init_type),
                            fmt_const_c_str(node->constant.get())),
                        node->line);
                default:
                    break;
            }
            TULong value = get_const_ptr_value(node);
            if (value != 0ul) {
                RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_static_ptr_init_not_null,
                                                fmt_type_c_str(static_init_type), std::to_string(value).c_str()),
                    node->line);
            }
            push_zero_static_init(8l);
            break;
        }
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_agg_init_with_single, fmt_type_c_str(static_init_type)), node->line);
    }
}

static void check_static_ptr_string_init(CString* node, Pointer* static_ptr_type) {
    if (static_ptr_type->ref_type->type() != AST_Char_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_static_ptr_init_string, fmt_type_c_str(static_ptr_type)), node->line);
    }

    TIdentifier string_const_label;
    {
        TIdentifier string_const;
        {
            std::string value = string_literal_to_const(node->literal->value);
            string_const = make_string_identifier(std::move(value));
        }
        if (frontend->string_const_table.find(string_const) != frontend->string_const_table.end()) {
            string_const_label = frontend->string_const_table[string_const];
        }
        else {
            string_const_label = repr_label_identifier(LBL_Lstring);
            frontend->string_const_table[string_const] = string_const_label;
            std::shared_ptr<Type> constant_type;
            {
                TLong size = static_cast<TLong>(node->literal->value.size()) + 1l;
                std::shared_ptr<Type> elem_type = std::make_shared<Char>();
                constant_type = std::make_shared<Array>(std::move(size), std::move(elem_type));
            }
            std::unique_ptr<IdentifierAttr> constant_attrs;
            {
                std::shared_ptr<StaticInit> static_init;
                {
                    std::shared_ptr<CStringLiteral> literal = node->literal;
                    static_init = std::make_shared<StringInit>(std::move(string_const), true, std::move(literal));
                }
                constant_attrs = std::make_unique<ConstantAttr>(std::move(static_init));
            }
            frontend->symbol_table[string_const_label] =
                std::make_unique<Symbol>(std::move(constant_type), std::move(constant_attrs));
        }
    }
    push_static_init(std::make_shared<PointerInit>(std::move(string_const_label)));
}

static void check_static_arr_string_init(CString* node, Array* static_arr_type) {
    check_bound_string_init(node, static_arr_type);
    TLong byte = static_arr_type->size - static_cast<TLong>(node->literal->value.size()) - 1l;
    {
        bool is_null_term = byte >= 0l;
        TIdentifier string_const;
        {
            std::string value = string_literal_to_const(node->literal->value);
            string_const = make_string_identifier(std::move(value));
        }
        std::shared_ptr<CStringLiteral> literal = node->literal;
        push_static_init(
            std::make_shared<StringInit>(std::move(string_const), std::move(is_null_term), std::move(literal)));
    }
    if (byte > 0l) {
        push_zero_static_init(std::move(byte));
    }
}

static void check_static_string_init(CString* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_Pointer_t:
            check_static_ptr_string_init(node, static_cast<Pointer*>(static_init_type));
            break;
        case AST_Array_t:
            check_static_arr_string_init(node, static_cast<Array*>(static_init_type));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void check_single_static_init(CSingleInit* node, Type* static_init_type) {
    switch (node->exp->type()) {
        case AST_CConstant_t:
            check_static_const_init(static_cast<CConstant*>(node->exp.get()), static_init_type);
            break;
        case AST_CString_t:
            check_static_string_init(static_cast<CString*>(node->exp.get()), static_init_type);
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_static_init_not_const, fmt_type_c_str(static_init_type)), node->exp->line);
    }
}

static void check_static_arr_init(CCompoundInit* node, Array* arr_type) {
    check_bound_arr_init(node, arr_type);

    for (const auto& initializer : node->initializers) {
        check_static_init(initializer.get(), arr_type->elem_type.get());
    }
    if (static_cast<size_t>(arr_type->size) > node->initializers.size()) {
        check_static_no_init(arr_type->elem_type.get(), arr_type->size - node->initializers.size());
    }
}

static void check_static_struct_init(CCompoundInit* node, Structure* struct_type) {
    check_bound_struct_init(node, struct_type);

    TLong size = 0l;
    for (size_t i = 0; i < node->initializers.size(); ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        if (member->offset != size) {
            check_static_no_init(nullptr, member->offset - size);
            size = member->offset;
        }
        check_static_init(node->initializers[i].get(), member->member_type.get());
        size += get_type_scale(member->member_type.get());
    }
    if (frontend->struct_typedef_table[struct_type->tag]->size != size) {
        check_static_no_init(nullptr, frontend->struct_typedef_table[struct_type->tag]->size - size);
    }
}

static void check_static_compound_init(CCompoundInit* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_Array_t:
            check_static_arr_init(node, static_cast<Array*>(static_init_type));
            break;
        case AST_Structure_t:
            check_static_struct_init(node, static_cast<Structure*>(static_init_type));
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_scalar_init_with_compound, fmt_type_c_str(static_init_type)),
                get_compound_line(node));
    }
}

static void check_static_init(CInitializer* node, Type* static_init_type) {
    switch (node->type()) {
        case AST_CSingleInit_t:
            check_single_static_init(static_cast<CSingleInit*>(node), static_init_type);
            break;
        case AST_CCompoundInit_t:
            check_static_compound_init(static_cast<CCompoundInit*>(node), static_init_type);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<Initial> check_initializer(CInitializer* node, Type* static_init_type) {
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    {
        ctx->p_static_inits = &static_inits;
        check_static_init(node, static_init_type);
        ctx->p_static_inits = nullptr;
    }
    return std::make_shared<Initial>(std::move(static_inits));
}

static void check_file_var_decl(CVariableDeclaration* node) {
    errors->linebuf = node->line;
    reslv_struct_type(node->var_type.get());
    if (node->var_type->type() == AST_Void_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_void_var_decl, fmt_name_c_str(node->name)), node->line);
    }
    is_valid_type(node->var_type.get());

    std::shared_ptr<InitialValue> init_value;
    bool is_glob = !(node->storage_class && node->storage_class->type() == AST_CStatic_t);

    if (node->init) {
        if (node->var_type->type() == AST_Structure_t
            && !is_struct_complete(static_cast<Structure*>(node->var_type.get()))) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_incomplete_var_decl, fmt_name_c_str(node->name),
                                            fmt_type_c_str(node->var_type.get())),
                node->line);
        }
        init_value = check_initializer(node->init.get(), node->var_type.get());
    }
    else {
        if (node->storage_class && node->storage_class->type() == AST_CExtern_t) {
            init_value = std::make_shared<NoInitializer>();
        }
        else {
            if (node->var_type->type() == AST_Structure_t
                && !is_struct_complete(static_cast<Structure*>(node->var_type.get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_incomplete_var_decl, fmt_name_c_str(node->name),
                                                fmt_type_c_str(node->var_type.get())),
                    node->line);
            }
            init_value = std::make_shared<Tentative>();
        }
    }

    if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if (!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_redecl_var_conflict, fmt_name_c_str(node->name),
                                            fmt_type_c_str(node->var_type.get()),
                                            fmt_type_c_str(frontend->symbol_table[node->name]->type_t.get())),
                node->line);
        }

        StaticAttr* glob_var_attrs = static_cast<StaticAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if (node->storage_class && node->storage_class->type() == AST_CExtern_t) {
            is_glob = glob_var_attrs->is_glob;
        }
        else if (is_glob != glob_var_attrs->is_glob) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_redecl_var_storage, fmt_name_c_str(node->name)), node->line);
        }

        if (glob_var_attrs->init->type() == AST_Initial_t) {
            if (init_value->type() == AST_Initial_t) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MSG(MSG_redecl_var_storage, fmt_name_c_str(node->name)), node->line);
            }
            else {
                init_value = glob_var_attrs->init;
            }
        }
    }

    std::shared_ptr<Type> glob_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> glob_var_attrs =
        std::make_unique<StaticAttr>(std::move(is_glob), std::move(init_value));
    frontend->symbol_table[node->name] = std::make_unique<Symbol>(std::move(glob_var_type), std::move(glob_var_attrs));
}

static void check_extern_block_var_decl(CVariableDeclaration* node) {
    if (node->init) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_redef_extern_var, fmt_name_c_str(node->name)), node->line);
    }
    else if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if (!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_redecl_var_conflict, fmt_name_c_str(node->name),
                                            fmt_type_c_str(node->var_type.get()),
                                            fmt_type_c_str(frontend->symbol_table[node->name]->type_t.get())),
                node->line);
        }
        return;
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs;
    {
        std::shared_ptr<InitialValue> init_value = std::make_shared<NoInitializer>();
        local_var_attrs = std::make_unique<StaticAttr>(true, std::move(init_value));
    }
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void check_static_block_var_decl(CVariableDeclaration* node) {
    ABORT_IF(node->var_type->type() == AST_Structure_t
             && !is_struct_complete(static_cast<Structure*>(node->var_type.get())));

    std::shared_ptr<InitialValue> init_value;
    if (node->init) {
        init_value = check_initializer(node->init.get(), node->var_type.get());
    }
    else {
        init_value = check_no_initializer(node->var_type.get());
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<StaticAttr>(false, std::move(init_value));
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void check_auto_block_var_decl(CVariableDeclaration* node) {
    if (node->var_type->type() == AST_Structure_t
        && !is_struct_complete(static_cast<Structure*>(node->var_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_incomplete_var_decl, fmt_name_c_str(node->name), fmt_type_c_str(node->var_type.get())),
            node->line);
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<LocalAttr>();
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void check_block_var_decl(CVariableDeclaration* node) {
    errors->linebuf = node->line;
    reslv_struct_type(node->var_type.get());
    if (node->var_type->type() == AST_Void_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_void_var_decl, fmt_name_c_str(node->name)), node->line);
    }
    is_valid_type(node->var_type.get());

    if (node->storage_class) {
        switch (node->storage_class->type()) {
            case AST_CExtern_t:
                check_extern_block_var_decl(node);
                break;
            case AST_CStatic_t:
                check_static_block_var_decl(node);
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    else {
        check_auto_block_var_decl(node);
    }
}

static void check_struct_members_decl(CStructDeclaration* node) {
    for (size_t i = 0; i < node->members.size(); ++i) {
        for (size_t j = i + 1; j < node->members.size(); ++j) {
            if (node->members[i]->member_name == node->members[j]->member_name) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MSG(MSG_duplicate_member_decl, fmt_struct_name_c_str(node->tag, node->is_union),
                        fmt_name_c_str(node->members[i]->member_name)),
                    node->members[i]->line);
            }
        }
        ABORT_IF(node->members[i].get()->member_type->type() == AST_FunType_t);
        errors->linebuf = node->members[i]->line;
        reslv_struct_type(node->members[i].get()->member_type.get());
        if (!is_type_complete(node->members[i].get()->member_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_incomplete_member_decl, fmt_struct_name_c_str(node->tag, node->is_union),
                    fmt_name_c_str(node->members[i]->member_name),
                    fmt_type_c_str(node->members[i].get()->member_type.get())),
                node->members[i]->line);
        }
        is_valid_type(node->members[i].get()->member_type.get());
    }
}

static void check_struct_decl(CStructDeclaration* node) {
    if (frontend->struct_typedef_table.find(node->tag) != frontend->struct_typedef_table.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_redecl_struct_in_scope, fmt_struct_name_c_str(node->tag, node->is_union)), node->line);
    }
    TInt alignment = 0;
    TLong size = 0l;
    std::vector<TIdentifier> member_names;
    std::unordered_map<TIdentifier, std::unique_ptr<StructMember>> members;
    member_names.reserve(node->members.size());
    members.reserve(node->members.size());
    for (const auto& member : node->members) {
        {
            TIdentifier name = member->member_name;
            member_names.push_back(name);
        }
        TInt member_alignment = get_type_alignment(member->member_type.get());
        TLong member_size = get_type_scale(member->member_type.get());
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
            std::shared_ptr<Type> member_type = member->member_type;
            members[member_names.back()] = std::make_unique<StructMember>(std::move(offset), std::move(member_type));
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
    frontend->struct_typedef_table[node->tag] = std::make_unique<StructTypedef>(
        std::move(alignment), std::move(size), std::move(member_names), std::move(members));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loop labeling

static void annotate_goto_label(CLabel* node) {
    if (ctx->label_set.find(node->target) != ctx->label_set.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MSG(MSG_redef_label_in_scope, fmt_name_c_str(node->target)), node->line);
    }
    ctx->label_set.insert(node->target);
}

static void annotate_while_loop(CWhile* node) {
    node->target = repr_label_identifier(LBL_Lwhile);
    ctx->break_loop_labels.push_back(node->target);
    ctx->continue_loop_labels.push_back(node->target);
}

static void annotate_do_while_loop(CDoWhile* node) {
    node->target = repr_label_identifier(LBL_Ldo_while);
    ctx->break_loop_labels.push_back(node->target);
    ctx->continue_loop_labels.push_back(node->target);
}

static void annotate_for_loop(CFor* node) {
    node->target = repr_label_identifier(LBL_Lfor);
    ctx->break_loop_labels.push_back(node->target);
    ctx->continue_loop_labels.push_back(node->target);
}

static void annotate_switch_lookup(CSwitch* node) {
    node->is_default = false;
    node->target = repr_label_identifier(LBL_Lswitch);
    ctx->break_loop_labels.push_back(node->target);
}

static void annotate_case_jump(CCase* node) {
    if (!ctx->p_switch_statement) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG_0(MSG_case_out_of_switch), node->value->line);
    }
    node->target = repr_case_identifier(ctx->p_switch_statement->target, false, ctx->p_switch_statement->cases.size());
}

static void annotate_default_jump(CDefault* node) {
    if (!ctx->p_switch_statement) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG_0(MSG_default_out_of_switch), node->line);
    }
    else if (ctx->p_switch_statement->is_default) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG_0(MSG_multiple_default), node->line);
    }
    node->target = ctx->p_switch_statement->target;
    ctx->p_switch_statement->is_default = true;
}

static void annotate_break_jump(CBreak* node) {
    if (ctx->break_loop_labels.empty()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG_0(MSG_break_out_of_loop), node->line);
    }
    node->target = ctx->break_loop_labels.back();
}

static void annotate_continue_jump(CContinue* node) {
    if (ctx->continue_loop_labels.empty()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG_0(MSG_continue_out_of_loop), node->line);
    }
    node->target = ctx->continue_loop_labels.back();
}

static void deannotate_loop() {
    ctx->break_loop_labels.pop_back();
    ctx->continue_loop_labels.pop_back();
}

static void deannotate_lookup() { ctx->break_loop_labels.pop_back(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifier resolution

static bool is_file_scope() { return ctx->scoped_identifier_maps.size() == 1; }

static void enter_scope() {
    ctx->scoped_identifier_maps.emplace_back();
    ctx->scoped_struct_maps.emplace_back();
}

static void exit_scope() {
    for (const auto& identifier : ctx->scoped_identifier_maps.back()) {
        if (ctx->extern_scope_map.find(identifier.first) != ctx->extern_scope_map.end()
            && ctx->extern_scope_map[identifier.first] == ctx->scoped_identifier_maps.size()) {
            ctx->extern_scope_map.erase(identifier.first);
        }
    }
    ctx->scoped_identifier_maps.pop_back();
    ctx->scoped_struct_maps.pop_back();
}

static void reslv_label(CFunctionDeclaration* node) {
    for (const auto& target : ctx->goto_map) {
        if (ctx->label_set.find(target.first) == ctx->label_set.end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_undef_goto_target, fmt_name_c_str(target.first), fmt_name_c_str(node->name)),
                errors->linebuf_map[target.second]);
        }
    }
}

static void reslv_ptr_struct(Pointer* ptr_type) { reslv_struct_type(ptr_type->ref_type.get()); }

static void reslv_arr_struct(Array* arr_type) { reslv_struct_type(arr_type->elem_type.get()); }

static void reslv_struct(Structure* struct_type) {
    if (struct_type->is_union) {
        if (ctx->union_def_set.find(struct_type->tag) != ctx->union_def_set.end()) {
            return;
        }
    }
    else {
        if (ctx->struct_def_set.find(struct_type->tag) != ctx->struct_def_set.end()) {
            return;
        }
    }
    for (size_t i = ctx->scoped_identifier_maps.size(); i-- > 0;) {
        if (ctx->scoped_struct_maps[i].find(struct_type->tag) != ctx->scoped_struct_maps[i].end()) {
            if (ctx->scoped_struct_maps[i][struct_type->tag].is_union != struct_type->is_union) {
                RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_redecl_struct_conflict, fmt_type_c_str(struct_type),
                                                fmt_struct_name_c_str(struct_type->tag, !struct_type->is_union)),
                    errors->linebuf);
            }
            struct_type->tag = ctx->scoped_struct_maps[i][struct_type->tag].tag;
            return;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(
        GET_SEMANTIC_MSG(MSG_undef_struct_in_scope, fmt_type_c_str(struct_type)), errors->linebuf);
}

static void reslv_struct_type(Type* type) {
    switch (type->type()) {
        case AST_Pointer_t:
            reslv_ptr_struct(static_cast<Pointer*>(type));
            break;
        case AST_Array_t:
            reslv_arr_struct(static_cast<Array*>(type));
            break;
        case AST_Structure_t:
            reslv_struct(static_cast<Structure*>(type));
            break;
        case AST_FunType_t:
            RAISE_INTERNAL_ERROR;
        default:
            break;
    }
}

static void reslv_exp(CExp* node);
static std::unique_ptr<CExp> reslv_typed_exp(std::unique_ptr<CExp>&& node);

static void reslv_const_exp(CConstant* node) { check_const_exp(node); }

static void reslv_string_exp(CString* node) { check_string_exp(node); }

static void reslv_var_exp(CVar* node) {
    for (size_t i = ctx->scoped_identifier_maps.size(); i-- > 0;) {
        if (ctx->scoped_identifier_maps[i].find(node->name) != ctx->scoped_identifier_maps[i].end()) {
            node->name = ctx->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_undecl_var_in_scope, fmt_name_c_str(node->name)), node->line);
Lelse:

    check_var_exp(node);
}

static void reslv_cast_exp(CCast* node) {
    node->exp = reslv_typed_exp(std::move(node->exp));
    check_cast_exp(node);
}

static void reslv_unary_exp(CUnary* node) {
    node->exp = reslv_typed_exp(std::move(node->exp));
    check_unary_exp(node);
}

static void reslv_binary_exp(CBinary* node) {
    node->exp_left = reslv_typed_exp(std::move(node->exp_left));
    node->exp_right = reslv_typed_exp(std::move(node->exp_right));
    check_binary_exp(node);
}

static void reslv_assign_exp(CAssignment* node) {
    if (node->exp_left) {
        node->exp_left = reslv_typed_exp(std::move(node->exp_left));
    }
    node->exp_right = reslv_typed_exp(std::move(node->exp_right));
    check_assign_exp(node);
}

static void reslv_conditional_exp(CConditional* node) {
    node->condition = reslv_typed_exp(std::move(node->condition));
    node->exp_middle = reslv_typed_exp(std::move(node->exp_middle));
    node->exp_right = reslv_typed_exp(std::move(node->exp_right));
    check_conditional_exp(node);
}

static void reslv_call_exp(CFunctionCall* node) {
    for (size_t i = ctx->scoped_identifier_maps.size(); i-- > 0;) {
        if (ctx->scoped_identifier_maps[i].find(node->name) != ctx->scoped_identifier_maps[i].end()) {
            node->name = ctx->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_undecl_fun_in_scope, fmt_name_c_str(node->name)), node->line);
Lelse:

    for (size_t i = 0; i < node->args.size(); ++i) {
        node->args[i] = reslv_typed_exp(std::move(node->args[i]));
    }
    check_call_exp(node);
}

static void reslv_deref_exp(CDereference* node) {
    node->exp = reslv_typed_exp(std::move(node->exp));
    check_deref_exp(node);
}

static void reslv_addrof_expr(CAddrOf* node) {
    reslv_exp(node->exp.get());
    check_addrof_exp(node);
}

static void reslv_subscript_exp(CSubscript* node) {
    node->primary_exp = reslv_typed_exp(std::move(node->primary_exp));
    node->subscript_exp = reslv_typed_exp(std::move(node->subscript_exp));
    check_subscript_exp(node);
}

static void reslv_sizeof_exp(CSizeOf* node) {
    reslv_exp(node->exp.get());
    check_sizeof_exp(node);
}

static void reslv_sizeoft_exp(CSizeOfT* node) { check_sizeoft_exp(node); }

static void reslv_dot_exp(CDot* node) {
    node->structure = reslv_typed_exp(std::move(node->structure));
    check_dot_exp(node);
}

static void reslv_arrow_exp(CArrow* node) {
    node->pointer = reslv_typed_exp(std::move(node->pointer));
    check_arrow_exp(node);
}

static void reslv_exp(CExp* node) {
    switch (node->type()) {
        case AST_CConstant_t:
            reslv_const_exp(static_cast<CConstant*>(node));
            break;
        case AST_CString_t:
            reslv_string_exp(static_cast<CString*>(node));
            break;
        case AST_CVar_t:
            reslv_var_exp(static_cast<CVar*>(node));
            break;
        case AST_CCast_t:
            reslv_cast_exp(static_cast<CCast*>(node));
            break;
        case AST_CUnary_t:
            reslv_unary_exp(static_cast<CUnary*>(node));
            break;
        case AST_CBinary_t:
            reslv_binary_exp(static_cast<CBinary*>(node));
            break;
        case AST_CAssignment_t:
            reslv_assign_exp(static_cast<CAssignment*>(node));
            break;
        case AST_CConditional_t:
            reslv_conditional_exp(static_cast<CConditional*>(node));
            break;
        case AST_CFunctionCall_t:
            reslv_call_exp(static_cast<CFunctionCall*>(node));
            break;
        case AST_CDereference_t:
            reslv_deref_exp(static_cast<CDereference*>(node));
            break;
        case AST_CAddrOf_t:
            reslv_addrof_expr(static_cast<CAddrOf*>(node));
            break;
        case AST_CSubscript_t:
            reslv_subscript_exp(static_cast<CSubscript*>(node));
            break;
        case AST_CSizeOf_t:
            reslv_sizeof_exp(static_cast<CSizeOf*>(node));
            break;
        case AST_CSizeOfT_t:
            reslv_sizeoft_exp(static_cast<CSizeOfT*>(node));
            break;
        case AST_CDot_t:
            reslv_dot_exp(static_cast<CDot*>(node));
            break;
        case AST_CArrow_t:
            reslv_arrow_exp(static_cast<CArrow*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<CExp> reslv_typed_exp(std::unique_ptr<CExp>&& node) {
    reslv_exp(node.get());
    return check_typed_exp(std::move(node));
}

static void reslv_block(CBlock* node);
static void reslv_block_var_decl(CVariableDeclaration* node);

static void reslv_statement(CStatement* node);

static void reslv_for_init_decl(CInitDecl* node) {
    if (node->init->storage_class) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_for_init_decl_not_auto, fmt_name_c_str(node->init->name),
                                        fmt_storage_class_c_str(node->init->storage_class.get())),
            node->init->line);
    }
    reslv_block_var_decl(node->init.get());
}

static void reslv_for_init_exp(CInitExp* node) {
    if (node->init) {
        node->init = reslv_typed_exp(std::move(node->init));
    }
}

static void reslv_for_init(CForInit* node) {
    switch (node->type()) {
        case AST_CInitDecl_t:
            reslv_for_init_decl(static_cast<CInitDecl*>(node));
            break;
        case AST_CInitExp_t: {
            reslv_for_init_exp(static_cast<CInitExp*>(node));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void reslv_ret_statement(CReturn* node) {
    if (node->exp) {
        node->exp = reslv_typed_exp(std::move(node->exp));
    }
    check_ret_statement(node);
}

static void reslv_exp_statement(CExpression* node) { node->exp = reslv_typed_exp(std::move(node->exp)); }

static void reslv_if_statement(CIf* node) {
    node->condition = reslv_typed_exp(std::move(node->condition));
    reslv_statement(node->then.get());
    if (node->else_fi) {
        reslv_statement(node->else_fi.get());
    }
    check_if_statement(node);
}

static void reslv_goto_statement(CGoto* node) {
    if (ctx->goto_map.find(node->target) != ctx->goto_map.end()) {
        node->target = ctx->goto_map[node->target];
        errors->linebuf_map[node->target] = node->line;
    }
    else {
        ctx->goto_map[node->target] = rslv_label_identifier(node->target);
        node->target = ctx->goto_map[node->target];
        errors->linebuf_map[node->target] = node->line;
    }
}

static void reslv_label_statement(CLabel* node) {
    annotate_goto_label(node);
    if (ctx->goto_map.find(node->target) != ctx->goto_map.end()) {
        node->target = ctx->goto_map[node->target];
    }
    else {
        ctx->goto_map[node->target] = rslv_label_identifier(node->target);
        node->target = ctx->goto_map[node->target];
    }
    reslv_statement(node->jump_to.get());
}

static void reslv_compound_statement(CCompound* node) {
    enter_scope();
    reslv_block(node->block.get());
    exit_scope();
}

static void reslv_while_statement(CWhile* node) {
    annotate_while_loop(node);
    node->condition = reslv_typed_exp(std::move(node->condition));
    reslv_statement(node->body.get());
    deannotate_loop();
    check_while_statement(node);
}

static void reslv_do_while_statement(CDoWhile* node) {
    annotate_do_while_loop(node);
    reslv_statement(node->body.get());
    node->condition = reslv_typed_exp(std::move(node->condition));
    deannotate_loop();
    check_do_while_statement(node);
}

static void reslv_for_statement(CFor* node) {
    annotate_for_loop(node);
    enter_scope();
    reslv_for_init(node->init.get());
    if (node->condition) {
        node->condition = reslv_typed_exp(std::move(node->condition));
    }
    if (node->post) {
        node->post = reslv_typed_exp(std::move(node->post));
    }
    reslv_statement(node->body.get());
    exit_scope();
    deannotate_loop();
    check_for_statement(node);
}

static void reslv_switch_statement(CSwitch* node) {
    annotate_switch_lookup(node);
    enter_scope();
    node->match = reslv_typed_exp(std::move(node->match));
    {
        CSwitch* p_switch_statement = ctx->p_switch_statement;
        ctx->p_switch_statement = node;
        reslv_statement(node->body.get());
        ctx->p_switch_statement = p_switch_statement;
    }
    exit_scope();
    deannotate_lookup();
    check_switch_statement(node);
}

static void reslv_case_statement(CCase* node) {
    annotate_case_jump(node);
    node->value = reslv_typed_exp(std::move(node->value));
    ctx->p_switch_statement->cases.push_back(std::move(node->value));
    reslv_statement(node->jump_to.get());
}

static void reslv_default_statement(CDefault* node) {
    annotate_default_jump(node);
    reslv_statement(node->jump_to.get());
}

static void reslv_break_statement(CBreak* node) { annotate_break_jump(node); }

static void reslv_continue_statement(CContinue* node) { annotate_continue_jump(node); }

static void reslv_statement(CStatement* node) {
    switch (node->type()) {
        case AST_CReturn_t:
            reslv_ret_statement(static_cast<CReturn*>(node));
            break;
        case AST_CExpression_t:
            reslv_exp_statement(static_cast<CExpression*>(node));
            break;
        case AST_CIf_t:
            reslv_if_statement(static_cast<CIf*>(node));
            break;
        case AST_CGoto_t:
            reslv_goto_statement(static_cast<CGoto*>(node));
            break;
        case AST_CLabel_t:
            reslv_label_statement(static_cast<CLabel*>(node));
            break;
        case AST_CCompound_t:
            reslv_compound_statement(static_cast<CCompound*>(node));
            break;
        case AST_CWhile_t:
            reslv_while_statement(static_cast<CWhile*>(node));
            break;
        case AST_CDoWhile_t:
            reslv_do_while_statement(static_cast<CDoWhile*>(node));
            break;
        case AST_CFor_t:
            reslv_for_statement(static_cast<CFor*>(node));
            break;
        case AST_CSwitch_t:
            reslv_switch_statement(static_cast<CSwitch*>(node));
            break;
        case AST_CCase_t:
            reslv_case_statement(static_cast<CCase*>(node));
            break;
        case AST_CDefault_t:
            reslv_default_statement(static_cast<CDefault*>(node));
            break;
        case AST_CBreak_t:
            reslv_break_statement(static_cast<CBreak*>(node));
            break;
        case AST_CContinue_t:
            reslv_continue_statement(static_cast<CContinue*>(node));
            break;
        case AST_CNull_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void reslv_declaration(CDeclaration* node);

static void reslv_block_items(const std::vector<std::unique_ptr<CBlockItem>>& node_list) {
    for (const auto& block_item : node_list) {
        switch (block_item->type()) {
            case AST_CS_t:
                reslv_statement(static_cast<CS*>(block_item.get())->statement.get());
                break;
            case AST_CD_t:
                reslv_declaration(static_cast<CD*>(block_item.get())->declaration.get());
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

static void reslv_block(CBlock* node) {
    if (node->type() == AST_CB_t) {
        reslv_block_items(static_cast<CB*>(node)->block_items);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void reslv_initializer(CInitializer* node, std::shared_ptr<Type>& init_type);

static void reslv_single_init(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    if (node->exp->type() == AST_CString_t && init_type->type() == AST_Array_t) {
        check_bound_string_init(static_cast<CString*>(node->exp.get()), static_cast<Array*>(init_type.get()));
        check_string_init(node, init_type);
    }
    else {
        node->exp = reslv_typed_exp(std::move(node->exp));
        check_single_init(node, init_type);
    }
}

static void reslv_arr_init(CCompoundInit* node, Array* arr_type, std::shared_ptr<Type>& init_type) {
    check_bound_arr_init(node, arr_type);

    for (const auto& initializer : node->initializers) {
        reslv_initializer(initializer.get(), arr_type->elem_type);
    }
    check_arr_init(node, arr_type, init_type);
}

static void reslv_struct_init(CCompoundInit* node, Structure* struct_type, std::shared_ptr<Type>& init_type) {
    check_bound_struct_init(node, struct_type);

    for (size_t i = 0; i < node->initializers.size(); ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        reslv_initializer(node->initializers[i].get(), member->member_type);
    }
    check_struct_init(node, struct_type, init_type);
}

static void reslv_compound_init(CCompoundInit* node, std::shared_ptr<Type>& init_type) {
    switch (init_type->type()) {
        case AST_Array_t:
            reslv_arr_init(node, static_cast<Array*>(init_type.get()), init_type);
            break;
        case AST_Structure_t:
            reslv_struct_init(node, static_cast<Structure*>(init_type.get()), init_type);
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_scalar_init_with_compound, fmt_type_c_str(init_type.get())),
                get_compound_line(node));
    }
}

static void reslv_initializer(CInitializer* node, std::shared_ptr<Type>& init_type) {
    switch (node->type()) {
        case AST_CSingleInit_t:
            reslv_single_init(static_cast<CSingleInit*>(node), init_type);
            break;
        case AST_CCompoundInit_t:
            reslv_compound_init(static_cast<CCompoundInit*>(node), init_type);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void reslv_fun_params_decl(CFunctionDeclaration* node) {
    for (TIdentifier& param : node->params) {
        if (ctx->scoped_identifier_maps.back().find(param) != ctx->scoped_identifier_maps.back().end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_redecl_var_in_scope, fmt_name_c_str(param)), node->line);
        }
        ctx->scoped_identifier_maps.back()[param] = rslv_var_identifier(param);
        param = ctx->scoped_identifier_maps.back()[param];
    }
    check_fun_params_decl(node);
}

static void reslv_fun_declaration(CFunctionDeclaration* node) {
    if (!is_file_scope()) {
        if (node->body) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_def_nested_fun, fmt_name_c_str(node->name)), node->line);
        }
        else if (node->storage_class && node->storage_class->type() == AST_CStatic_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_decl_nested_static_fun, fmt_name_c_str(node->name)), node->line);
        }
    }

    if (ctx->extern_scope_map.find(node->name) == ctx->extern_scope_map.end()) {
        if (ctx->scoped_identifier_maps.back().find(node->name) != ctx->scoped_identifier_maps.back().end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MSG(MSG_redecl_fun_in_scope, fmt_name_c_str(node->name)), node->line);
        }
        ctx->extern_scope_map[node->name] = ctx->scoped_identifier_maps.size();
    }

    ctx->scoped_identifier_maps.back()[node->name] = node->name;
    check_ret_fun_decl(node);

    enter_scope();
    if (!node->params.empty()) {
        reslv_fun_params_decl(node);
    }
    check_fun_decl(node);

    if (node->body) {
        reslv_block(node->body.get());
    }
    exit_scope();
}

static void reslv_file_var_decl(CVariableDeclaration* node) {
    if (ctx->extern_scope_map.find(node->name) == ctx->extern_scope_map.end()) {
        ctx->extern_scope_map[node->name] = ctx->scoped_identifier_maps.size();
    }

    ctx->scoped_identifier_maps.back()[node->name] = node->name;
    if (is_file_scope()) {
        check_file_var_decl(node);
    }
    else {
        check_block_var_decl(node);
    }
}

static void reslv_block_var_decl(CVariableDeclaration* node) {
    if (ctx->scoped_identifier_maps.back().find(node->name) != ctx->scoped_identifier_maps.back().end()
        && !(ctx->extern_scope_map.find(node->name) != ctx->extern_scope_map.end()
             && (node->storage_class && node->storage_class->type() == AST_CExtern_t))) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MSG(MSG_redecl_var_in_scope, fmt_name_c_str(node->name)), node->line);
    }
    else if (node->storage_class && node->storage_class->type() == AST_CExtern_t) {
        reslv_file_var_decl(node);
        return;
    }

    ctx->scoped_identifier_maps.back()[node->name] = rslv_var_identifier(node->name);
    node->name = ctx->scoped_identifier_maps.back()[node->name];
    check_block_var_decl(node);

    if (node->init && !node->storage_class) {
        reslv_initializer(node->init.get(), node->var_type);
    }
}

static void reslv_struct_members_decl(CStructDeclaration* node) { check_struct_members_decl(node); }

static void reslv_struct_declaration(CStructDeclaration* node) {
    if (ctx->scoped_struct_maps.back().find(node->tag) != ctx->scoped_struct_maps.back().end()) {
        node->tag = ctx->scoped_struct_maps.back()[node->tag].tag;
        if (node->is_union) {
            if (ctx->union_def_set.find(node->tag) == ctx->union_def_set.end()) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MSG(MSG_redecl_struct_conflict, fmt_struct_name_c_str(node->tag, node->is_union),
                        fmt_struct_name_c_str(node->tag, !node->is_union)),
                    node->line);
            }
        }
        else {
            if (ctx->struct_def_set.find(node->tag) == ctx->struct_def_set.end()) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MSG(MSG_redecl_struct_conflict, fmt_struct_name_c_str(node->tag, node->is_union),
                        fmt_struct_name_c_str(node->tag, !node->is_union)),
                    node->line);
            }
        }
    }
    else {
        ctx->scoped_struct_maps.back()[node->tag] = {rslv_struct_tag(node->tag), node->is_union};
        node->tag = ctx->scoped_struct_maps.back()[node->tag].tag;
        if (node->is_union) {
            ctx->union_def_set.insert(node->tag);
        }
        else {
            ctx->struct_def_set.insert(node->tag);
        }
    }
    if (!node->members.empty()) {
        reslv_struct_members_decl(node);
        check_struct_decl(node);
    }
}

static void reslv_fun_decl(CFunDecl* node) {
    if (is_file_scope()) {
        ctx->goto_map.clear();
        ctx->label_set.clear();
        ctx->break_loop_labels.clear();
        ctx->continue_loop_labels.clear();
        ctx->p_switch_statement = nullptr;
    }
    reslv_fun_declaration(node->fun_decl.get());
    if (is_file_scope()) {
        reslv_label(node->fun_decl.get());
    }
}

static void reslv_var_decl(CVarDecl* node) {
    if (is_file_scope()) {
        reslv_file_var_decl(node->var_decl.get());
    }
    else {
        reslv_block_var_decl(node->var_decl.get());
    }
}

static void reslv_struct_decl(CStructDecl* node) { reslv_struct_declaration(node->struct_decl.get()); }

static void reslv_declaration(CDeclaration* node) {
    switch (node->type()) {
        case AST_CFunDecl_t:
            reslv_fun_decl(static_cast<CFunDecl*>(node));
            break;
        case AST_CVarDecl_t:
            reslv_var_decl(static_cast<CVarDecl*>(node));
            break;
        case AST_CStructDecl_t:
            reslv_struct_decl(static_cast<CStructDecl*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_identifiers(CProgram* node) {
    enter_scope();
    for (const auto& declaration : node->declarations) {
        reslv_declaration(declaration.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void analyze_semantic(CProgram* node) {
    ctx = std::make_unique<SemanticContext>();
    resolve_identifiers(node);
    ctx.reset();
}
