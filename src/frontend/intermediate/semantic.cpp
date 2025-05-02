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
    std::unordered_map<TIdentifier, size_t> external_linkage_scope_map;
    std::vector<std::unordered_map<TIdentifier, TIdentifier>> scoped_identifier_maps;
    std::vector<std::unordered_map<TIdentifier, Structure>> scoped_structure_type_maps;
    std::unordered_map<TIdentifier, TIdentifier> goto_map;
    std::unordered_set<TIdentifier> label_set;
    // Loop labeling
    std::vector<TIdentifier> break_loop_labels;
    std::vector<TIdentifier> continue_loop_labels;
    // Identifier resolution
    TIdentifier function_definition_name;
    CSwitch* p_switch_statement;
    std::unordered_set<TIdentifier> function_definition_set;
    std::unordered_set<TIdentifier> struct_definition_set;
    std::unordered_set<TIdentifier> union_definition_set;
    std::vector<std::shared_ptr<StaticInit>>* p_static_inits;
};

static std::unique_ptr<SemanticContext> context;

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
            case AST_T::Pointer_t:
                return is_same_ptr(static_cast<Pointer*>(type_1), static_cast<Pointer*>(type_2));
            case AST_T::Array_t:
                return is_same_arr(static_cast<Array*>(type_1), static_cast<Array*>(type_2));
            case AST_T::Structure_t:
                return is_same_struct(static_cast<Structure*>(type_1), static_cast<Structure*>(type_2));
            case AST_T::FunType_t:
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

static bool is_type_char(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_int(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::UChar_t:
        case AST_T::UInt_t:
        case AST_T::ULong_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_arithmetic(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::UChar_t:
        case AST_T::UInt_t:
        case AST_T::ULong_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_scalar(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::UChar_t:
        case AST_T::UInt_t:
        case AST_T::ULong_t:
        case AST_T::Pointer_t:
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
        case AST_T::Void_t:
            return false;
        case AST_T::Structure_t:
            return is_struct_complete(static_cast<Structure*>(type));
        default:
            return true;
    }
}

static void is_valid_type(Type* type);

static void is_valid_ptr(Pointer* ptr_type) { is_valid_type(ptr_type->ref_type.get()); }

static void is_valid_arr(Array* arr_type) {
    if (!is_type_complete(arr_type->elem_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::array_of_incomplete_type,
                                        get_type_hr_c_str(arr_type), get_type_hr_c_str(arr_type->elem_type.get())),
            errors->line_buffer);
    }
    is_valid_type(arr_type->elem_type.get());
}

static void is_valid_type(Type* type) {
    switch (type->type()) {
        case AST_T::Pointer_t:
            is_valid_ptr(static_cast<Pointer*>(type));
            break;
        case AST_T::Array_t:
            is_valid_arr(static_cast<Array*>(type));
            break;
        case AST_T::FunType_t:
            RAISE_INTERNAL_ERROR;
        default:
            break;
    }
}

static bool is_exp_lvalue(CExp* node);

static bool is_dot_exp_lvalue(CDot* node) { return is_exp_lvalue(node->structure.get()); }

static bool is_exp_lvalue(CExp* node) {
    switch (node->type()) {
        case AST_T::CString_t:
        case AST_T::CVar_t:
        case AST_T::CDereference_t:
        case AST_T::CSubscript_t:
        case AST_T::CArrow_t:
            return true;
        case AST_T::CDot_t:
            return is_dot_exp_lvalue(static_cast<CDot*>(node));
        default:
            return false;
    }
}

static bool is_const_null_ptr(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstInt_t:
            return static_cast<CConstInt*>(node->constant.get())->value == 0;
        case AST_T::CConstLong_t:
            return static_cast<CConstLong*>(node->constant.get())->value == 0l;
        case AST_T::CConstUInt_t:
            return static_cast<CConstUInt*>(node->constant.get())->value == 0u;
        case AST_T::CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value == 0ul;
        default:
            return false;
    }
}

static TInt get_scalar_size(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return 1;
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

static TLong get_type_scale(Type* type);

static TLong get_arr_scale(Array* arr_type) {
    TLong size = arr_type->size;
    while (arr_type->elem_type->type() == AST_T::Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    return get_type_scale(arr_type->elem_type.get()) * size;
}

static TLong get_struct_scale(Structure* struct_type) {
    if (frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        RAISE_INTERNAL_ERROR;
    }
    return frontend->struct_typedef_table[struct_type->tag]->size;
}

static TLong get_type_scale(Type* type) {
    switch (type->type()) {
        case AST_T::Array_t:
            return get_arr_scale(static_cast<Array*>(type));
        case AST_T::Structure_t:
            return get_struct_scale(static_cast<Structure*>(type));
        default:
            return get_scalar_size(type);
    }
}

static TInt get_type_alignment(Type* type);

static TInt get_arr_alignment(Array* arr_type) { return get_type_alignment(arr_type->elem_type.get()); }

static TInt get_struct_alignment(Structure* struct_type) {
    if (frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        RAISE_INTERNAL_ERROR;
    }
    return frontend->struct_typedef_table[struct_type->tag]->alignment;
}

static TInt get_type_alignment(Type* type) {
    switch (type->type()) {
        case AST_T::Array_t:
            return get_arr_alignment(static_cast<Array*>(type));
        case AST_T::Structure_t:
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
    else if (node_1->exp_type->type() == AST_T::Double_t || node_2->exp_type->type() == AST_T::Double_t) {
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
    else if (node_1->type() == AST_T::CConstant_t && is_const_null_ptr(static_cast<CConstant*>(node_1))) {
        return node_2->exp_type;
    }
    else if (node_2->type() == AST_T::CConstant_t && is_const_null_ptr(static_cast<CConstant*>(node_2))) {
        return node_1->exp_type;
    }
    else if (node_1->exp_type->type() == AST_T::Pointer_t
             && static_cast<Pointer*>(node_1->exp_type.get())->ref_type->type() == AST_T::Void_t
             && node_2->exp_type->type() == AST_T::Pointer_t) {
        return node_1->exp_type;
    }
    else if (node_2->exp_type->type() == AST_T::Pointer_t
             && static_cast<Pointer*>(node_2->exp_type.get())->ref_type->type() == AST_T::Void_t
             && node_1->exp_type->type() == AST_T::Pointer_t) {
        return node_2->exp_type;
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::joint_pointer_type_mismatch,
                get_type_hr_c_str(node_1->exp_type.get()), get_type_hr_c_str(node_2->exp_type.get())),
            node_1->line);
    }
}

static TChar get_const_char_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return static_cast<CConstChar*>(node->constant.get())->value;
        case AST_T::CConstInt_t:
            return static_cast<TChar>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<TChar>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_T::CConstDouble_t:
            return static_cast<TChar>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_T::CConstUChar_t:
            return static_cast<TChar>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_T::CConstUInt_t:
            return static_cast<TChar>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<TChar>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TInt get_const_int_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return static_cast<TInt>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_T::CConstInt_t:
            return static_cast<CConstInt*>(node->constant.get())->value;
        case AST_T::CConstLong_t:
            return static_cast<TInt>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_T::CConstDouble_t:
            return static_cast<TInt>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_T::CConstUChar_t:
            return static_cast<TInt>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_T::CConstUInt_t:
            return static_cast<TInt>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<TInt>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TLong get_const_long_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return static_cast<TLong>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_T::CConstInt_t:
            return static_cast<TLong>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<CConstLong*>(node->constant.get())->value;
        case AST_T::CConstDouble_t:
            return static_cast<TLong>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_T::CConstUChar_t:
            return static_cast<TLong>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_T::CConstUInt_t:
            return static_cast<TLong>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<TLong>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TDouble get_const_dbl_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return static_cast<TDouble>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_T::CConstInt_t:
            return static_cast<TDouble>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<TDouble>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_T::CConstDouble_t:
            return static_cast<CConstDouble*>(node->constant.get())->value;
        case AST_T::CConstUChar_t:
            return static_cast<TDouble>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_T::CConstUInt_t:
            return static_cast<TDouble>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<TDouble>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TUChar get_const_uchar_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return static_cast<TUChar>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_T::CConstInt_t:
            return static_cast<TUChar>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<TUChar>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_T::CConstDouble_t:
            return static_cast<TUChar>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_T::CConstUChar_t:
            return static_cast<CConstUChar*>(node->constant.get())->value;
        case AST_T::CConstUInt_t:
            return static_cast<TUChar>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<TUChar>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TUInt get_const_uint_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return static_cast<TUInt>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_T::CConstInt_t:
            return static_cast<TUInt>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<TUInt>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_T::CConstDouble_t:
            return static_cast<TUInt>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_T::CConstUChar_t:
            return static_cast<TUInt>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_T::CConstUInt_t:
            return static_cast<CConstUInt*>(node->constant.get())->value;
        case AST_T::CConstULong_t:
            return static_cast<TUInt>(static_cast<CConstULong*>(node->constant.get())->value);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TULong get_const_ulong_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            return static_cast<TULong>(static_cast<CConstChar*>(node->constant.get())->value);
        case AST_T::CConstInt_t:
            return static_cast<TULong>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<TULong>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_T::CConstDouble_t:
            return static_cast<TULong>(static_cast<CConstDouble*>(node->constant.get())->value);
        case AST_T::CConstUChar_t:
            return static_cast<TULong>(static_cast<CConstUChar*>(node->constant.get())->value);
        case AST_T::CConstUInt_t:
            return static_cast<TULong>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TULong get_const_ptr_value(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstInt_t:
            return static_cast<TULong>(static_cast<CConstInt*>(node->constant.get())->value);
        case AST_T::CConstLong_t:
            return static_cast<TULong>(static_cast<CConstLong*>(node->constant.get())->value);
        case AST_T::CConstUInt_t:
            return static_cast<TULong>(static_cast<CConstUInt*>(node->constant.get())->value);
        case AST_T::CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static size_t get_compound_line(CInitializer* node) {
    if (node->type() != AST_T::CCompoundInit_t) {
        RAISE_INTERNAL_ERROR;
    }
    do {
        node = static_cast<CCompoundInit*>(node)->initializers[0].get();
    }
    while (node->type() == AST_T::CCompoundInit_t);
    if (node->type() != AST_T::CSingleInit_t) {
        RAISE_INTERNAL_ERROR;
    }
    return static_cast<CSingleInit*>(node)->exp->line;
}

static void reslv_struct_type(Type* type);

static void check_const_exp(CConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
            node->exp_type = std::make_shared<Char>();
            break;
        case AST_T::CConstInt_t:
            node->exp_type = std::make_shared<Int>();
            break;
        case AST_T::CConstLong_t:
            node->exp_type = std::make_shared<Long>();
            break;
        case AST_T::CConstDouble_t:
            node->exp_type = std::make_shared<Double>();
            break;
        case AST_T::CConstUChar_t:
            node->exp_type = std::make_shared<UChar>();
            break;
        case AST_T::CConstUInt_t:
            node->exp_type = std::make_shared<UInt>();
            break;
        case AST_T::CConstULong_t:
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
    if (frontend->symbol_table[node->name]->type_t->type() == AST_T::FunType_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_used_as_variable, get_name_hr_c_str(node->name)),
            node->line);
    }
    node->exp_type = frontend->symbol_table[node->name]->type_t;
}

static void check_cast_exp(CCast* node) {
    errors->line_buffer = node->line;
    reslv_struct_type(node->target_type.get());
    if (node->target_type->type() != AST_T::Void_t
        && ((node->exp->exp_type->type() == AST_T::Double_t && node->target_type->type() == AST_T::Pointer_t)
            || (node->exp->exp_type->type() == AST_T::Pointer_t && node->target_type->type() == AST_T::Double_t)
            || !is_type_scalar(node->exp->exp_type.get()) || !is_type_scalar(node->target_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::illegal_conversion_from_type_to,
                get_type_hr_c_str(node->exp->exp_type.get()), get_type_hr_c_str(node->target_type.get())),
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
    else if (node->type() == AST_T::CConstant_t && exp_type->type() == AST_T::Pointer_t
             && is_const_null_ptr(static_cast<CConstant*>(node.get()))) {
        return cast_exp(std::move(node), exp_type);
    }
    else if (exp_type->type() == AST_T::Pointer_t
             && static_cast<Pointer*>(exp_type.get())->ref_type->type() == AST_T::Void_t
             && node->exp_type->type() == AST_T::Pointer_t) {
        return cast_exp(std::move(node), exp_type);
    }
    else if (node->exp_type->type() == AST_T::Pointer_t
             && static_cast<Pointer*>(node->exp_type.get())->ref_type->type() == AST_T::Void_t
             && exp_type->type() == AST_T::Pointer_t) {
        return cast_exp(std::move(node), exp_type);
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::illegal_conversion_from_type_to,
                                        get_type_hr_c_str(node->exp_type.get()), get_type_hr_c_str(exp_type.get())),
            node->line);
    }
}

static void check_unary_complement_exp(CUnary* node) {
    if (!is_type_arithmetic(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                get_unary_op_hr_c_str(node->unary_op.get()), get_type_hr_c_str(node->exp->exp_type.get())),
            node->line);
    }

    switch (node->exp->exp_type->type()) {
        case AST_T::Double_t:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                    get_unary_op_hr_c_str(node->unary_op.get()), get_type_hr_c_str(node->exp->exp_type.get())),
                node->line);
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t: {
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
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                get_unary_op_hr_c_str(node->unary_op.get()), get_type_hr_c_str(node->exp->exp_type.get())),
            node->line);
    }

    switch (node->exp->exp_type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t: {
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
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                get_unary_op_hr_c_str(node->unary_op.get()), get_type_hr_c_str(node->exp->exp_type.get())),
            node->line);
    }

    node->exp_type = std::make_shared<Int>();
}

static void check_unary_exp(CUnary* node) {
    switch (node->unary_op->type()) {
        case AST_T::CComplement_t:
            check_unary_complement_exp(node);
            break;
        case AST_T::CNegate_t:
            check_unary_neg_exp(node);
            break;
        case AST_T::CNot_t:
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
    else if (node->exp_left->exp_type->type() == AST_T::Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())
             && is_type_int(node->exp_right->exp_type.get())) {
        common_type = std::make_shared<Long>();
        if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
            node->exp_right = cast_exp(std::move(node->exp_right), common_type);
        }
        node->exp_type = node->exp_left->exp_type;
        return;
    }
    else if (is_type_int(node->exp_left->exp_type.get()) && node->exp_right->exp_type->type() == AST_T::Pointer_t
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
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
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
    else if (node->exp_left->exp_type->type() == AST_T::Pointer_t
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
                 && !(node->exp_left->type() == AST_T::CConstant_t
                      && is_const_null_ptr(static_cast<CConstant*>(node->exp_left.get())))) {
            common_type = std::make_shared<Long>();
            node->exp_type = std::move(common_type);
            return;
        }
        else {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                    get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                    get_type_hr_c_str(node->exp_right->exp_type.get())),
                node->line);
        }
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
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
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
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
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
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
    if (node->exp_type->type() == AST_T::Double_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_type,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_type.get())),
            node->line);
    }
}

static void check_binary_bitshift_exp(CBinary* node) {
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_int(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
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
    if (node->exp_type->type() == AST_T::Double_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_type,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_type.get())),
            node->line);
    }
}

static void check_bitshift_right_exp(CBinary* node) {
    check_binary_bitshift_exp(node);
    if (is_type_signed(node->exp_left->exp_type.get())) {
        node->binary_op = std::make_unique<CBitShrArithmetic>();
    }
}

static void check_binary_logical_exp(CBinary* node) {
    if (!is_type_scalar(node->exp_left->exp_type.get()) || !is_type_scalar(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
            node->line);
    }

    node->exp_type = std::make_shared<Int>();
}

static void check_binary_equality_exp(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t || node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        common_type = get_joint_ptr_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (is_type_arithmetic(node->exp_left->exp_type.get())
             && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
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
        || (node->exp_left->exp_type->type() == AST_T::Pointer_t
            && (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())
                || (node->exp_left->type() == AST_T::CConstant_t
                    && is_const_null_ptr(static_cast<CConstant*>(node->exp_left.get())))
                || (node->exp_right->type() == AST_T::CConstant_t
                    && is_const_null_ptr(static_cast<CConstant*>(node->exp_right.get())))))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr_c_str(node->binary_op.get()), get_type_hr_c_str(node->exp_left->exp_type.get()),
                get_type_hr_c_str(node->exp_right->exp_type.get())),
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
    switch (node->binary_op->type()) {
        case AST_T::CAdd_t:
            check_binary_add_exp(node);
            break;
        case AST_T::CSubtract_t:
            check_binary_subtract_exp(node);
            break;
        case AST_T::CMultiply_t:
        case AST_T::CDivide_t:
            check_multiply_divide_exp(node);
            break;
        case AST_T::CRemainder_t:
        case AST_T::CBitAnd_t:
        case AST_T::CBitOr_t:
        case AST_T::CBitXor_t:
            check_remainder_bitwise_exp(node);
            break;
        case AST_T::CBitShiftLeft_t:
            check_binary_bitshift_exp(node);
            break;
        case AST_T::CBitShiftRight_t:
            check_bitshift_right_exp(node);
            break;
        case AST_T::CAnd_t:
        case AST_T::COr_t:
            check_binary_logical_exp(node);
            break;
        case AST_T::CEqual_t:
        case AST_T::CNotEqual_t:
            check_binary_equality_exp(node);
            break;
        case AST_T::CLessThan_t:
        case AST_T::CLessOrEqual_t:
        case AST_T::CGreaterThan_t:
        case AST_T::CGreaterOrEqual_t:
            check_binary_relational_exp(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void check_assign_exp(CAssignment* node) {
    if (node->exp_left) {
        if (node->exp_left->exp_type->type() == AST_T::Void_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE_0(MESSAGE_SEMANTIC::assignment_to_void_type), node->line);
        }
        else if (!is_exp_lvalue(node->exp_left.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::assignment_to_rvalue,
                                            get_assignment_hr_c_str(nullptr, node->unary_op.get())),
                node->line);
        }
        else if (!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
            node->exp_right = cast_assign(std::move(node->exp_right), node->exp_left->exp_type);
        }
        node->exp_type = node->exp_left->exp_type;
    }
    else {
        if (node->exp_right->type() != AST_T::CBinary_t) {
            RAISE_INTERNAL_ERROR;
        }
        CExp* exp_left = static_cast<CBinary*>(node->exp_right.get())->exp_left.get();
        if (exp_left->type() == AST_T::CCast_t) {
            exp_left = static_cast<CCast*>(exp_left)->exp.get();
        }
        if (!is_exp_lvalue(exp_left)) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::assignment_to_rvalue,
                    get_assignment_hr_c_str(
                        static_cast<CBinary*>(node->exp_right.get())->binary_op.get(), node->unary_op.get())),
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
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::conditional_on_invalid_condition_type,
                                        get_type_hr_c_str(node->condition->exp_type.get())),
            node->line);
    }
    else if (node->exp_middle->exp_type->type() == AST_T::Void_t
             && node->exp_right->exp_type->type() == AST_T::Void_t) {
        node->exp_type = node->exp_middle->exp_type;
        return;
    }
    else if (node->exp_middle->exp_type->type() == AST_T::Structure_t
             || node->exp_right->exp_type->type() == AST_T::Structure_t) {
        if (!is_same_type(node->exp_middle->exp_type.get(), node->exp_right->exp_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::ternary_on_invalid_operand_types,
                                            get_type_hr_c_str(node->exp_middle->exp_type.get()),
                                            get_type_hr_c_str(node->exp_right->exp_type.get())),
                node->line);
        }
        node->exp_type = node->exp_middle->exp_type;
        return;
    }

    std::shared_ptr<Type> common_type;
    if (is_type_arithmetic(node->exp_middle->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_middle.get(), node->exp_right.get());
    }
    else if (node->exp_middle->exp_type->type() == AST_T::Pointer_t
             || node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        common_type = get_joint_ptr_type(node->exp_middle.get(), node->exp_right.get());
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::ternary_on_invalid_operand_types,
                                        get_type_hr_c_str(node->exp_middle->exp_type.get()),
                                        get_type_hr_c_str(node->exp_right->exp_type.get())),
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
    if (frontend->symbol_table[node->name]->type_t->type() != AST_T::FunType_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_used_as_function, get_name_hr_c_str(node->name)),
            node->line);
    }
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    if (fun_type->param_types.size() != node->args.size()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_called_with_wrong_number_of_arguments,
                get_name_hr_c_str(node->name), std::to_string(node->args.size()).c_str(),
                std::to_string(fun_type->param_types.size()).c_str()),
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
    if (node->exp->exp_type->type() != AST_T::Pointer_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::dereference_non_pointer,
                                        get_type_hr_c_str(node->exp->exp_type.get())),
            node->line);
    }
    node->exp_type = static_cast<Pointer*>(node->exp->exp_type.get())->ref_type;
}

static void check_addrof_exp(CAddrOf* node) {
    if (!is_exp_lvalue(node->exp.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE_0(MESSAGE_SEMANTIC::address_of_rvalue), node->line);
    }
    std::shared_ptr<Type> ref_type = node->exp->exp_type;
    node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
}

static void check_subscript_exp(CSubscript* node) {
    std::shared_ptr<Type> ref_type;
    if (node->primary_exp->exp_type->type() == AST_T::Pointer_t
        && is_type_complete(static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type.get())
        && is_type_int(node->subscript_exp->exp_type.get())) {
        std::shared_ptr<Type> subscript_type = std::make_shared<Long>();
        if (!is_same_type(node->subscript_exp->exp_type.get(), subscript_type.get())) {
            node->subscript_exp = cast_exp(std::move(node->subscript_exp), subscript_type);
        }
        ref_type = static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type;
    }
    else if (is_type_int(node->primary_exp->exp_type.get()) && node->subscript_exp->exp_type->type() == AST_T::Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type.get())) {
        std::shared_ptr<Type> primary_type = std::make_shared<Long>();
        if (!is_same_type(node->primary_exp->exp_type.get(), primary_type.get())) {
            node->primary_exp = cast_exp(std::move(node->primary_exp), primary_type);
        }
        ref_type = static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type;
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::subscript_array_with_invalid_types,
                                        get_type_hr_c_str(node->primary_exp->exp_type.get()),
                                        get_type_hr_c_str(node->subscript_exp->exp_type.get())),
            node->line);
    }
    node->exp_type = std::move(ref_type);
}

static void check_sizeof_exp(CSizeOf* node) {
    if (!is_type_complete(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::get_size_of_incomplete_type,
                                        get_type_hr_c_str(node->exp->exp_type.get())),
            node->line);
    }
    node->exp_type = std::make_shared<ULong>();
}

static void check_sizeoft_exp(CSizeOfT* node) {
    errors->line_buffer = node->line;
    reslv_struct_type(node->target_type.get());
    if (!is_type_complete(node->target_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::get_size_of_incomplete_type,
                                        get_type_hr_c_str(node->target_type.get())),
            node->line);
    }
    is_valid_type(node->target_type.get());
    node->exp_type = std::make_shared<ULong>();
}

static void check_dot_exp(CDot* node) {
    if (node->structure->exp_type->type() != AST_T::Structure_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::dot_on_non_structure_type, get_name_hr_c_str(node->member),
                get_type_hr_c_str(node->structure->exp_type.get())),
            node->line);
    }
    Structure* struct_type = static_cast<Structure*>(node->structure->exp_type.get());
    if (frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
        == frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::member_not_in_structure_type,
                                        get_type_hr_c_str(struct_type), get_name_hr_c_str(node->member)),
            node->line);
    }
    node->exp_type = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
}

static void check_arrow_exp(CArrow* node) {
    if (node->pointer->exp_type->type() != AST_T::Pointer_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::arrow_on_non_pointer_to_structure_type,
                get_name_hr_c_str(node->member), get_type_hr_c_str(node->pointer->exp_type.get())),
            node->line);
    }
    Pointer* ptr_type = static_cast<Pointer*>(node->pointer->exp_type.get());
    if (ptr_type->ref_type->type() != AST_T::Structure_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::arrow_on_non_pointer_to_structure_type,
                get_name_hr_c_str(node->member), get_type_hr_c_str(node->pointer->exp_type.get())),
            node->line);
    }
    Structure* struct_type = static_cast<Structure*>(ptr_type->ref_type.get());
    if (frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::arrow_on_incomplete_structure_type,
                                        get_name_hr_c_str(node->member), get_type_hr_c_str(struct_type)),
            node->line);
    }
    else if (frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
             == frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::member_not_in_structure_type,
                                        get_type_hr_c_str(struct_type), get_name_hr_c_str(node->member)),
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
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::incomplete_structure_type_in_expression,
                                        get_type_hr_c_str(node->exp_type.get())),
            node->line);
    }

    std::unique_ptr<CExp> exp = std::move(node);
    return exp;
}

static std::unique_ptr<CExp> check_typed_exp(std::unique_ptr<CExp>&& node) {
    switch (node->exp_type->type()) {
        case AST_T::Array_t:
            return check_arr_typed_exp(std::move(node));
        case AST_T::Structure_t:
            return check_struct_typed_exp(std::move(node));
        default:
            return check_scalar_typed_exp(std::move(node));
    }
}

static void check_ret_statement(CReturn* node) {
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[context->function_definition_name]->type_t.get());
    if (fun_type->ret_type->type() == AST_T::Void_t) {
        if (node->exp) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::return_value_in_void_function,
                                            get_name_hr_c_str(context->function_definition_name)),
                node->line);
        }
        return;
    }
    else if (!node->exp) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::no_return_value_in_non_void_function,
                get_name_hr_c_str(context->function_definition_name), get_type_hr_c_str(fun_type->ret_type.get())),
            node->line);
    }

    else if (!is_same_type(node->exp->exp_type.get(), fun_type->ret_type.get())) {
        node->exp = cast_assign(std::move(node->exp), fun_type->ret_type);
    }
    node->exp = check_typed_exp(std::move(node->exp));
}

static void check_if_statement(CIf* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::if_used_with_condition_type,
                                        get_type_hr_c_str(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void check_while_statement(CWhile* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::while_used_with_condition_type,
                                        get_type_hr_c_str(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void check_do_while_statement(CDoWhile* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::do_while_used_with_condition_type,
                                        get_type_hr_c_str(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void check_for_statement(CFor* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::for_used_with_condition_type,
                                        get_type_hr_c_str(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void check_switch_statement(CSwitch* node) {
    if (!is_type_int(node->match->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::switch_used_with_match_type,
                                        get_type_hr_c_str(node->match->exp_type.get())),
            node->match->line);
    }
    switch (node->match->exp_type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t: {
            std::shared_ptr<Type> promote_type = std::make_shared<Int>();
            node->match = cast_exp(std::move(node->match), promote_type);
            break;
        }
        default:
            break;
    }
    switch (node->match->exp_type->type()) {
        case AST_T::Int_t: {
            std::vector<TInt> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                if (node->cases[i]->type() != AST_T::CConstant_t) {
                    RAISE_INTERNAL_ERROR;
                }
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_int_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MESSAGE(
                                MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i]).c_str()),
                            node->cases[i]->line);
                    }
                }
                esac->constant = std::make_shared<CConstInt>(values[i]);
                esac->exp_type = node->match->exp_type;
            }
            break;
        }
        case AST_T::Long_t: {
            std::vector<TLong> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                if (node->cases[i]->type() != AST_T::CConstant_t) {
                    RAISE_INTERNAL_ERROR;
                }
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_long_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MESSAGE(
                                MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i]).c_str()),
                            node->cases[i]->line);
                    }
                }
                esac->constant = std::make_shared<CConstLong>(values[i]);
                esac->exp_type = node->match->exp_type;
            }
            break;
        }
        case AST_T::UInt_t: {
            std::vector<TUInt> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                if (node->cases[i]->type() != AST_T::CConstant_t) {
                    RAISE_INTERNAL_ERROR;
                }
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_uint_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MESSAGE(
                                MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i]).c_str()),
                            node->cases[i]->line);
                    }
                }
                esac->constant = std::make_shared<CConstUInt>(values[i]);
                esac->exp_type = node->match->exp_type;
            }
            break;
        }
        case AST_T::ULong_t: {
            std::vector<TULong> values(node->cases.size());
            for (size_t i = 0; i < values.size(); ++i) {
                if (node->cases[i]->type() != AST_T::CConstant_t) {
                    RAISE_INTERNAL_ERROR;
                }
                CConstant* esac = static_cast<CConstant*>(node->cases[i].get());
                values[i] = get_const_ulong_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_SEMANTIC_MESSAGE(
                                MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i]).c_str()),
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
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::non_char_array_initialized_from_string, get_type_hr_c_str(arr_type)),
            node->line);
    }
    else if (node->literal->value.size() > static_cast<size_t>(arr_type->size)) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::string_initialized_with_too_many_characters,
                std::to_string(arr_type->size).c_str(), std::to_string(node->literal->value.size()).c_str()),
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
            case AST_T::Char_t:
            case AST_T::SChar_t: {
                constant = std::make_shared<CConstChar>(0);
                break;
            }
            case AST_T::Int_t: {
                constant = std::make_shared<CConstInt>(0);
                break;
            }
            case AST_T::Long_t: {
                constant = std::make_shared<CConstLong>(0l);
                break;
            }
            case AST_T::Double_t: {
                constant = std::make_shared<CConstDouble>(0.0);
                break;
            }
            case AST_T::UChar_t: {
                constant = std::make_shared<CConstUChar>(0u);
                break;
            }
            case AST_T::UInt_t: {
                constant = std::make_shared<CConstUInt>(0u);
                break;
            }
            case AST_T::ULong_t:
            case AST_T::Pointer_t: {
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
    std::vector<std::unique_ptr<CInitializer>> zero_initializers;
    size_t arr_type_size = static_cast<size_t>(arr_type->size);
    zero_initializers.reserve(arr_type_size);
    for (size_t i = 0; i < arr_type_size; ++i) {
        std::unique_ptr<CInitializer> initializer = check_zero_init(arr_type->elem_type.get());
        zero_initializers.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_initializers));
}

static std::unique_ptr<CCompoundInit> check_struct_zero_init(Structure* struct_type) {
    std::vector<std::unique_ptr<CInitializer>> zero_initializers;
    zero_initializers.reserve(frontend->struct_typedef_table[struct_type->tag]->member_names.size());
    for (TIdentifier member_name : frontend->struct_typedef_table[struct_type->tag]->member_names) {
        const auto& member = frontend->struct_typedef_table[struct_type->tag]->members[member_name];
        std::unique_ptr<CInitializer> initializer = check_zero_init(member->member_type.get());
        zero_initializers.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_initializers));
}

static std::unique_ptr<CInitializer> check_zero_init(Type* init_type) {
    switch (init_type->type()) {
        case AST_T::Array_t:
            return check_arr_zero_init(static_cast<Array*>(init_type));
        case AST_T::Structure_t:
            return check_struct_zero_init(static_cast<Structure*>(init_type));
        default:
            return check_single_zero_init(init_type);
    }
}

static void check_bound_arr_init(CCompoundInit* node, Array* arr_type) {
    if (node->initializers.size() > static_cast<size_t>(arr_type->size)) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::array_initialized_with_too_many_initializers,
                                        std::to_string(arr_type->size).c_str(), get_type_hr_c_str(arr_type),
                                        std::to_string(node->initializers.size()).c_str()),
            get_compound_line(node));
    }
}

static void check_bound_struct_init(CCompoundInit* node, Structure* struct_type) {
    size_t bound = struct_type->is_union ? 1 : frontend->struct_typedef_table[struct_type->tag]->members.size();
    if (node->initializers.size() > bound) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::structure_initialized_with_too_many_members,
                get_type_hr_c_str(struct_type), std::to_string(node->initializers.size()).c_str(),
                std::to_string(bound).c_str()),
            get_compound_line(node));
    }
}

static void check_arr_init(CCompoundInit* node, Array* arr_type, std::shared_ptr<Type>& init_type) {
    while (node->initializers.size() < static_cast<size_t>(arr_type->size)) {
        std::unique_ptr<CInitializer> zero_initializer = check_zero_init(arr_type->elem_type.get());
        node->initializers.push_back(std::move(zero_initializer));
    }
    node->init_type = init_type;
}

static void check_struct_init(CCompoundInit* node, Structure* struct_type, std::shared_ptr<Type>& init_type) {
    for (size_t i = node->initializers.size(); i < frontend->struct_typedef_table[struct_type->tag]->members.size();
         ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        std::unique_ptr<CInitializer> zero_initializer = check_zero_init(member->member_type.get());
        node->initializers.push_back(std::move(zero_initializer));
    }
    node->init_type = init_type;
}

static void check_ret_fun_decl(CFunctionDeclaration* node) {
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    errors->line_buffer = node->line;
    reslv_struct_type(fun_type->ret_type.get());
    is_valid_type(fun_type->ret_type.get());

    switch (fun_type->ret_type->type()) {
        case AST_T::Array_t:
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_returns_array,
                                            get_name_hr_c_str(node->name), get_type_hr_c_str(fun_type->ret_type.get())),
                node->line);
        case AST_T::Structure_t: {
            if (node->body && !is_struct_complete(static_cast<Structure*>(fun_type->ret_type.get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_returns_incomplete_structure_type,
                        get_name_hr_c_str(node->name), get_type_hr_c_str(fun_type->ret_type.get())),
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
        errors->line_buffer = node->line;
        reslv_struct_type(fun_type->param_types[i].get());
        if (fun_type->param_types[i]->type() == AST_T::Void_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::parameter_with_type_void,
                                            get_name_hr_c_str(node->name), get_name_hr_c_str(node->params[i])),
                node->line);
        }
        is_valid_type(fun_type->param_types[i].get());
        if (fun_type->param_types[i]->type() == AST_T::Array_t) {
            std::shared_ptr<Type> ref_type = static_cast<Array*>(fun_type->param_types[i].get())->elem_type;
            fun_type->param_types[i] = std::make_shared<Pointer>(std::move(ref_type));
        }

        if (node->body) {
            if (fun_type->param_types[i]->type() == AST_T::Structure_t
                && !is_struct_complete(static_cast<Structure*>(fun_type->param_types[i].get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::parameter_with_incomplete_structure_type,
                        get_name_hr_c_str(node->name), get_name_hr_c_str(node->params[i]),
                        get_type_hr_c_str(fun_type->param_types[i].get())),
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
    if (node->fun_type->type() == AST_T::Void_t) {
        RAISE_INTERNAL_ERROR;
    }

    bool is_defined = context->function_definition_set.find(node->name) != context->function_definition_set.end();
    bool is_global = !(node->storage_class && node->storage_class->type() == AST_T::CStatic_t);

    if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
        if (!(frontend->symbol_table[node->name]->type_t->type() == AST_T::FunType_t
                && fun_type->param_types.size() == node->params.size()
                && is_same_fun_type(static_cast<FunType*>(node->fun_type.get()), fun_type))) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_redeclared_with_conflicting_type,
                    get_name_hr_c_str(node->name), get_type_hr_c_str(node->fun_type.get()),
                    get_type_hr_c_str(fun_type)),
                node->line);
        }
        else if (is_defined && node->body) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_redefined,
                                            get_name_hr_c_str(node->name), get_type_hr_c_str(node->fun_type.get())),
                node->line);
        }

        FunAttr* fun_attrs = static_cast<FunAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if (!is_global && fun_attrs->is_global) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::non_static_function_redeclared_static,
                                            get_name_hr_c_str(node->name)),
                node->line);
        }
        is_global = fun_attrs->is_global;
    }

    if (node->body) {
        context->function_definition_set.insert(node->name);
        is_defined = true;
        context->function_definition_name = node->name;
    }

    std::shared_ptr<Type> fun_type = node->fun_type;
    std::unique_ptr<IdentifierAttr> fun_attrs = std::make_unique<FunAttr>(std::move(is_defined), std::move(is_global));
    frontend->symbol_table[node->name] = std::make_unique<Symbol>(std::move(fun_type), std::move(fun_attrs));
}

static void push_static_init(std::shared_ptr<StaticInit>&& static_init) {
    context->p_static_inits->push_back(std::move(static_init));
}

static void push_zero_static_init(TLong&& byte) {
    if (!context->p_static_inits->empty() && context->p_static_inits->back()->type() == AST_T::ZeroInit_t) {
        static_cast<ZeroInit*>(context->p_static_inits->back().get())->byte += byte;
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
        context->p_static_inits = &static_inits;
        check_static_no_init(static_init_type, 1l);
        context->p_static_inits = nullptr;
    }
    return std::make_shared<Initial>(std::move(static_inits));
}

static void check_static_const_init(CConstant* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value = get_const_char_value(node);
            if (value == 0) {
                push_zero_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<CharInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Int_t: {
            TInt value = get_const_int_value(node);
            if (value == 0) {
                push_zero_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<IntInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Long_t: {
            TLong value = get_const_long_value(node);
            if (value == 0l) {
                push_zero_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<LongInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Double_t: {
            TDouble value = get_const_dbl_value(node);
            TULong binary = double_to_binary(value);
            if (binary == 0ul) {
                push_zero_static_init(8l);
            }
            else {
                TIdentifier double_constant = make_string_identifier(std::to_string(binary));
                push_static_init(std::make_shared<DoubleInit>(std::move(double_constant)));
            }
            break;
        }
        case AST_T::UChar_t: {
            TUChar value = get_const_uchar_value(node);
            if (value == 0u) {
                push_zero_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<UCharInit>(std::move(value)));
            }
            break;
        }
        case AST_T::UInt_t: {
            TUInt value = get_const_uint_value(node);
            if (value == 0u) {
                push_zero_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<UIntInit>(std::move(value)));
            }
            break;
        }
        case AST_T::ULong_t: {
            TULong value = get_const_ulong_value(node);
            if (value == 0ul) {
                push_zero_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<ULongInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Pointer_t: {
            switch (node->constant->type()) {
                case AST_T::CConstChar_t:
                case AST_T::CConstDouble_t:
                case AST_T::CConstUChar_t:
                    RAISE_RUNTIME_ERROR_AT_LINE(
                        GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::static_pointer_initialized_from_non_integer,
                            get_type_hr_c_str(static_init_type), get_const_hr_c_str(node->constant.get())),
                        node->line);
                default:
                    break;
            }
            TULong value = get_const_ptr_value(node);
            if (value != 0ul) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::static_pointer_initialized_from_non_null,
                        get_type_hr_c_str(static_init_type), std::to_string(value).c_str()),
                    node->line);
            }
            push_zero_static_init(8l);
            break;
        }
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::aggregate_initialized_with_single_initializer,
                    get_type_hr_c_str(static_init_type)),
                node->line);
    }
}

static void check_static_ptr_string_init(CString* node, Pointer* static_ptr_type) {
    if (static_ptr_type->ref_type->type() != AST_T::Char_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(
                MESSAGE_SEMANTIC::static_non_char_pointer_initialized_from_string, get_type_hr_c_str(static_ptr_type)),
            node->line);
    }

    TIdentifier string_constant_label;
    {
        TIdentifier string_constant;
        {
            std::string value = string_literal_to_string_constant(node->literal->value);
            string_constant = make_string_identifier(std::move(value));
        }
        if (frontend->string_constant_table.find(string_constant) != frontend->string_constant_table.end()) {
            string_constant_label = frontend->string_constant_table[string_constant];
        }
        else {
            string_constant_label = represent_label_identifier(LABEL_KIND::Lstring);
            frontend->string_constant_table[string_constant] = string_constant_label;
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
                    static_init = std::make_shared<StringInit>(std::move(string_constant), true, std::move(literal));
                }
                constant_attrs = std::make_unique<ConstantAttr>(std::move(static_init));
            }
            frontend->symbol_table[string_constant_label] =
                std::make_unique<Symbol>(std::move(constant_type), std::move(constant_attrs));
        }
    }
    push_static_init(std::make_shared<PointerInit>(std::move(string_constant_label)));
}

static void check_static_arr_string_init(CString* node, Array* static_arr_type) {
    check_bound_string_init(node, static_arr_type);
    TLong byte = static_arr_type->size - static_cast<TLong>(node->literal->value.size()) - 1l;
    {
        bool is_null_terminated = byte >= 0l;
        TIdentifier string_constant;
        {
            std::string value = string_literal_to_string_constant(node->literal->value);
            string_constant = make_string_identifier(std::move(value));
        }
        std::shared_ptr<CStringLiteral> literal = node->literal;
        push_static_init(std::make_shared<StringInit>(
            std::move(string_constant), std::move(is_null_terminated), std::move(literal)));
    }
    if (byte > 0l) {
        push_zero_static_init(std::move(byte));
    }
}

static void check_static_string_init(CString* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_T::Pointer_t:
            check_static_ptr_string_init(node, static_cast<Pointer*>(static_init_type));
            break;
        case AST_T::Array_t:
            check_static_arr_string_init(node, static_cast<Array*>(static_init_type));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void check_single_static_init(CSingleInit* node, Type* static_init_type) {
    switch (node->exp->type()) {
        case AST_T::CConstant_t:
            check_static_const_init(static_cast<CConstant*>(node->exp.get()), static_init_type);
            break;
        case AST_T::CString_t:
            check_static_string_init(static_cast<CString*>(node->exp.get()), static_init_type);
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::static_initialized_with_non_constant,
                                            get_type_hr_c_str(static_init_type)),
                node->exp->line);
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
        case AST_T::Array_t:
            check_static_arr_init(node, static_cast<Array*>(static_init_type));
            break;
        case AST_T::Structure_t:
            check_static_struct_init(node, static_cast<Structure*>(static_init_type));
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::scalar_initialized_with_compound_initializer,
                    get_type_hr_c_str(static_init_type)),
                get_compound_line(node));
    }
}

static void check_static_init(CInitializer* node, Type* static_init_type) {
    switch (node->type()) {
        case AST_T::CSingleInit_t:
            check_single_static_init(static_cast<CSingleInit*>(node), static_init_type);
            break;
        case AST_T::CCompoundInit_t:
            check_static_compound_init(static_cast<CCompoundInit*>(node), static_init_type);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<Initial> check_initializer(CInitializer* node, Type* static_init_type) {
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    {
        context->p_static_inits = &static_inits;
        check_static_init(node, static_init_type);
        context->p_static_inits = nullptr;
    }
    return std::make_shared<Initial>(std::move(static_inits));
}

static void check_file_var_decl(CVariableDeclaration* node) {
    errors->line_buffer = node->line;
    reslv_struct_type(node->var_type.get());
    if (node->var_type->type() == AST_T::Void_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_declared_with_type_void, get_name_hr_c_str(node->name)),
            node->line);
    }
    is_valid_type(node->var_type.get());

    std::shared_ptr<InitialValue> initial_value;
    bool is_global = !(node->storage_class && node->storage_class->type() == AST_T::CStatic_t);

    if (node->init) {
        if (node->var_type->type() == AST_T::Structure_t
            && !is_struct_complete(static_cast<Structure*>(node->var_type.get()))) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type,
                    get_name_hr_c_str(node->name), get_type_hr_c_str(node->var_type.get())),
                node->line);
        }
        initial_value = check_initializer(node->init.get(), node->var_type.get());
    }
    else {
        if (node->storage_class && node->storage_class->type() == AST_T::CExtern_t) {
            initial_value = std::make_shared<NoInitializer>();
        }
        else {
            if (node->var_type->type() == AST_T::Structure_t
                && !is_struct_complete(static_cast<Structure*>(node->var_type.get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type,
                        get_name_hr_c_str(node->name), get_type_hr_c_str(node->var_type.get())),
                    node->line);
            }
            initial_value = std::make_shared<Tentative>();
        }
    }

    if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if (!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_type,
                    get_name_hr_c_str(node->name), get_type_hr_c_str(node->var_type.get()),
                    get_type_hr_c_str(frontend->symbol_table[node->name]->type_t.get())),
                node->line);
        }

        StaticAttr* global_var_attrs = static_cast<StaticAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if (node->storage_class && node->storage_class->type() == AST_T::CExtern_t) {
            is_global = global_var_attrs->is_global;
        }
        else if (is_global != global_var_attrs->is_global) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(
                    MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_storage, get_name_hr_c_str(node->name)),
                node->line);
        }

        if (global_var_attrs->init->type() == AST_T::Initial_t) {
            if (initial_value->type() == AST_T::Initial_t) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(
                        MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_storage, get_name_hr_c_str(node->name)),
                    node->line);
            }
            else {
                initial_value = global_var_attrs->init;
            }
        }
    }

    std::shared_ptr<Type> global_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> global_var_attrs =
        std::make_unique<StaticAttr>(std::move(is_global), std::move(initial_value));
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(global_var_type), std::move(global_var_attrs));
}

static void check_extern_block_var_decl(CVariableDeclaration* node) {
    if (node->init) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::extern_variable_defined, get_name_hr_c_str(node->name)), node->line);
    }
    else if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if (!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_type,
                    get_name_hr_c_str(node->name), get_type_hr_c_str(node->var_type.get()),
                    get_type_hr_c_str(frontend->symbol_table[node->name]->type_t.get())),
                node->line);
        }
        return;
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs;
    {
        std::shared_ptr<InitialValue> initial_value = std::make_shared<NoInitializer>();
        local_var_attrs = std::make_unique<StaticAttr>(true, std::move(initial_value));
    }
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void check_static_block_var_decl(CVariableDeclaration* node) {
    if (node->var_type->type() == AST_T::Structure_t
        && !is_struct_complete(static_cast<Structure*>(node->var_type.get()))) {
        RAISE_INTERNAL_ERROR;
    }

    std::shared_ptr<InitialValue> initial_value;
    if (node->init) {
        initial_value = check_initializer(node->init.get(), node->var_type.get());
    }
    else {
        initial_value = check_no_initializer(node->var_type.get());
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<StaticAttr>(false, std::move(initial_value));
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void check_auto_block_var_decl(CVariableDeclaration* node) {
    if (node->var_type->type() == AST_T::Structure_t
        && !is_struct_complete(static_cast<Structure*>(node->var_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type,
                get_name_hr_c_str(node->name), get_type_hr_c_str(node->var_type.get())),
            node->line);
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<LocalAttr>();
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void check_block_var_decl(CVariableDeclaration* node) {
    errors->line_buffer = node->line;
    reslv_struct_type(node->var_type.get());
    if (node->var_type->type() == AST_T::Void_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_declared_with_type_void, get_name_hr_c_str(node->name)),
            node->line);
    }
    is_valid_type(node->var_type.get());

    if (node->storage_class) {
        switch (node->storage_class->type()) {
            case AST_T::CExtern_t:
                check_extern_block_var_decl(node);
                break;
            case AST_T::CStatic_t:
                check_static_block_var_decl(node);
                break;
            default:
                break;
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
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::structure_declared_with_duplicate_member,
                        get_struct_name_hr_c_str(node->tag, node->is_union),
                        get_name_hr_c_str(node->members[i]->member_name)),
                    node->members[i]->line);
            }
        }
        if (node->members[i].get()->member_type->type() == AST_T::FunType_t) {
            RAISE_INTERNAL_ERROR;
        }
        errors->line_buffer = node->members[i]->line;
        reslv_struct_type(node->members[i].get()->member_type.get());
        if (!is_type_complete(node->members[i].get()->member_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::member_declared_with_incomplete_type,
                                            get_struct_name_hr_c_str(node->tag, node->is_union),
                                            get_name_hr_c_str(node->members[i]->member_name),
                                            get_type_hr_c_str(node->members[i].get()->member_type.get())),
                node->members[i]->line);
        }
        is_valid_type(node->members[i].get()->member_type.get());
    }
}

static void check_struct_decl(CStructDeclaration* node) {
    if (frontend->struct_typedef_table.find(node->tag) != frontend->struct_typedef_table.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::structure_redeclared_in_scope,
                                        get_struct_name_hr_c_str(node->tag, node->is_union)),
            node->line);
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
    if (context->label_set.find(node->target) != context->label_set.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::label_redefined_in_scope, get_name_hr_c_str(node->target)),
            node->line);
    }
    context->label_set.insert(node->target);
}

static void annotate_while_loop(CWhile* node) {
    node->target = represent_label_identifier(LABEL_KIND::Lwhile);
    context->break_loop_labels.push_back(node->target);
    context->continue_loop_labels.push_back(node->target);
}

static void annotate_do_while_loop(CDoWhile* node) {
    node->target = represent_label_identifier(LABEL_KIND::Ldo_while);
    context->break_loop_labels.push_back(node->target);
    context->continue_loop_labels.push_back(node->target);
}

static void annotate_for_loop(CFor* node) {
    node->target = represent_label_identifier(LABEL_KIND::Lfor);
    context->break_loop_labels.push_back(node->target);
    context->continue_loop_labels.push_back(node->target);
}

static void annotate_switch_lookup(CSwitch* node) {
    node->is_default = false;
    node->target = represent_label_identifier(LABEL_KIND::Lswitch);
    context->break_loop_labels.push_back(node->target);
}

static void annotate_case_jump(CCase* node) {
    if (!context->p_switch_statement) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE_0(MESSAGE_SEMANTIC::case_outside_of_switch), node->value->line);
    }
    node->target = represent_case_identifier(
        context->p_switch_statement->target, false, context->p_switch_statement->cases.size());
}

static void annotate_default_jump(CDefault* node) {
    if (!context->p_switch_statement) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE_0(MESSAGE_SEMANTIC::default_outside_of_switch), node->line);
    }
    else if (context->p_switch_statement->is_default) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE_0(MESSAGE_SEMANTIC::more_than_one_default_in_switch), node->line);
    }
    node->target = context->p_switch_statement->target;
    context->p_switch_statement->is_default = true;
}

static void annotate_break_jump(CBreak* node) {
    if (context->break_loop_labels.empty()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE_0(MESSAGE_SEMANTIC::break_outside_of_loop), node->line);
    }
    node->target = context->break_loop_labels.back();
}

static void annotate_continue_jump(CContinue* node) {
    if (context->continue_loop_labels.empty()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE_0(MESSAGE_SEMANTIC::continue_outside_of_loop), node->line);
    }
    node->target = context->continue_loop_labels.back();
}

static void deannotate_loop() {
    context->break_loop_labels.pop_back();
    context->continue_loop_labels.pop_back();
}

static void deannotate_lookup() { context->break_loop_labels.pop_back(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifier resolution

static bool is_file_scope() { return context->scoped_identifier_maps.size() == 1; }

static void enter_scope() {
    context->scoped_identifier_maps.emplace_back();
    context->scoped_structure_type_maps.emplace_back();
}

static void exit_scope() {
    for (const auto& identifier : context->scoped_identifier_maps.back()) {
        if (context->external_linkage_scope_map.find(identifier.first) != context->external_linkage_scope_map.end()
            && context->external_linkage_scope_map[identifier.first] == context->scoped_identifier_maps.size()) {
            context->external_linkage_scope_map.erase(identifier.first);
        }
    }
    context->scoped_identifier_maps.pop_back();
    context->scoped_structure_type_maps.pop_back();
}

static void reslv_label(CFunctionDeclaration* node) {
    for (const auto& target : context->goto_map) {
        if (context->label_set.find(target.first) == context->label_set.end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::goto_with_undefined_target_label,
                                            get_name_hr_c_str(target.first), get_name_hr_c_str(node->name)),
                errors->line_buffer_map[target.second]);
        }
    }
}

static void reslv_ptr_struct(Pointer* ptr_type) { reslv_struct_type(ptr_type->ref_type.get()); }

static void reslv_arr_struct(Array* arr_type) { reslv_struct_type(arr_type->elem_type.get()); }

static void reslv_struct(Structure* struct_type) {
    if (struct_type->is_union) {
        if (context->union_definition_set.find(struct_type->tag) != context->union_definition_set.end()) {
            return;
        }
    }
    else {
        if (context->struct_definition_set.find(struct_type->tag) != context->struct_definition_set.end()) {
            return;
        }
    }
    for (size_t i = context->scoped_identifier_maps.size(); i-- > 0;) {
        if (context->scoped_structure_type_maps[i].find(struct_type->tag)
            != context->scoped_structure_type_maps[i].end()) {
            if (context->scoped_structure_type_maps[i][struct_type->tag].is_union != struct_type->is_union) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::structure_conflicts_with_previously_declared,
                        get_type_hr_c_str(struct_type),
                        get_struct_name_hr_c_str(struct_type->tag, !struct_type->is_union)),
                    errors->line_buffer);
            }
            struct_type->tag = context->scoped_structure_type_maps[i][struct_type->tag].tag;
            return;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(
        GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::structure_not_defined_in_scope, get_type_hr_c_str(struct_type)),
        errors->line_buffer);
}

static void reslv_struct_type(Type* type) {
    switch (type->type()) {
        case AST_T::Pointer_t:
            reslv_ptr_struct(static_cast<Pointer*>(type));
            break;
        case AST_T::Array_t:
            reslv_arr_struct(static_cast<Array*>(type));
            break;
        case AST_T::Structure_t:
            reslv_struct(static_cast<Structure*>(type));
            break;
        case AST_T::FunType_t:
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
    for (size_t i = context->scoped_identifier_maps.size(); i-- > 0;) {
        if (context->scoped_identifier_maps[i].find(node->name) != context->scoped_identifier_maps[i].end()) {
            node->name = context->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(
        GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_not_declared_in_scope, get_name_hr_c_str(node->name)),
        node->line);
Lelse:

    check_var_exp(node);
}

static void reslv_cast_expression(CCast* node) {
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
    for (size_t i = context->scoped_identifier_maps.size(); i-- > 0;) {
        if (context->scoped_identifier_maps[i].find(node->name) != context->scoped_identifier_maps[i].end()) {
            node->name = context->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(
        GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_not_declared_in_scope, get_name_hr_c_str(node->name)),
        node->line);
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
        case AST_T::CConstant_t:
            reslv_const_exp(static_cast<CConstant*>(node));
            break;
        case AST_T::CString_t:
            reslv_string_exp(static_cast<CString*>(node));
            break;
        case AST_T::CVar_t:
            reslv_var_exp(static_cast<CVar*>(node));
            break;
        case AST_T::CCast_t:
            reslv_cast_expression(static_cast<CCast*>(node));
            break;
        case AST_T::CUnary_t:
            reslv_unary_exp(static_cast<CUnary*>(node));
            break;
        case AST_T::CBinary_t:
            reslv_binary_exp(static_cast<CBinary*>(node));
            break;
        case AST_T::CAssignment_t:
            reslv_assign_exp(static_cast<CAssignment*>(node));
            break;
        case AST_T::CConditional_t:
            reslv_conditional_exp(static_cast<CConditional*>(node));
            break;
        case AST_T::CFunctionCall_t:
            reslv_call_exp(static_cast<CFunctionCall*>(node));
            break;
        case AST_T::CDereference_t:
            reslv_deref_exp(static_cast<CDereference*>(node));
            break;
        case AST_T::CAddrOf_t:
            reslv_addrof_expr(static_cast<CAddrOf*>(node));
            break;
        case AST_T::CSubscript_t:
            reslv_subscript_exp(static_cast<CSubscript*>(node));
            break;
        case AST_T::CSizeOf_t:
            reslv_sizeof_exp(static_cast<CSizeOf*>(node));
            break;
        case AST_T::CSizeOfT_t:
            reslv_sizeoft_exp(static_cast<CSizeOfT*>(node));
            break;
        case AST_T::CDot_t:
            reslv_dot_exp(static_cast<CDot*>(node));
            break;
        case AST_T::CArrow_t:
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
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::for_initial_declared_with_non_automatic_storage,
                get_name_hr_c_str(node->init->name), get_storage_class_hr_c_str(node->init->storage_class.get())),
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
        case AST_T::CInitDecl_t:
            reslv_for_init_decl(static_cast<CInitDecl*>(node));
            break;
        case AST_T::CInitExp_t: {
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
    if (context->goto_map.find(node->target) != context->goto_map.end()) {
        node->target = context->goto_map[node->target];
        errors->line_buffer_map[node->target] = node->line;
    }
    else {
        context->goto_map[node->target] = resolve_label_identifier(node->target);
        node->target = context->goto_map[node->target];
        errors->line_buffer_map[node->target] = node->line;
    }
}

static void reslv_label_statement(CLabel* node) {
    annotate_goto_label(node);
    if (context->goto_map.find(node->target) != context->goto_map.end()) {
        node->target = context->goto_map[node->target];
    }
    else {
        context->goto_map[node->target] = resolve_label_identifier(node->target);
        node->target = context->goto_map[node->target];
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
        CSwitch* p_switch_statement = context->p_switch_statement;
        context->p_switch_statement = node;
        reslv_statement(node->body.get());
        context->p_switch_statement = p_switch_statement;
    }
    exit_scope();
    deannotate_lookup();
    check_switch_statement(node);
}

static void reslv_case_statement(CCase* node) {
    annotate_case_jump(node);
    node->value = reslv_typed_exp(std::move(node->value));
    context->p_switch_statement->cases.push_back(std::move(node->value));
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
        case AST_T::CReturn_t:
            reslv_ret_statement(static_cast<CReturn*>(node));
            break;
        case AST_T::CExpression_t:
            reslv_exp_statement(static_cast<CExpression*>(node));
            break;
        case AST_T::CIf_t:
            reslv_if_statement(static_cast<CIf*>(node));
            break;
        case AST_T::CGoto_t:
            reslv_goto_statement(static_cast<CGoto*>(node));
            break;
        case AST_T::CLabel_t:
            reslv_label_statement(static_cast<CLabel*>(node));
            break;
        case AST_T::CCompound_t:
            reslv_compound_statement(static_cast<CCompound*>(node));
            break;
        case AST_T::CWhile_t:
            reslv_while_statement(static_cast<CWhile*>(node));
            break;
        case AST_T::CDoWhile_t:
            reslv_do_while_statement(static_cast<CDoWhile*>(node));
            break;
        case AST_T::CFor_t:
            reslv_for_statement(static_cast<CFor*>(node));
            break;
        case AST_T::CSwitch_t:
            reslv_switch_statement(static_cast<CSwitch*>(node));
            break;
        case AST_T::CCase_t:
            reslv_case_statement(static_cast<CCase*>(node));
            break;
        case AST_T::CDefault_t:
            reslv_default_statement(static_cast<CDefault*>(node));
            break;
        case AST_T::CBreak_t:
            reslv_break_statement(static_cast<CBreak*>(node));
            break;
        case AST_T::CContinue_t:
            reslv_continue_statement(static_cast<CContinue*>(node));
            break;
        case AST_T::CNull_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void reslv_declaration(CDeclaration* node);

static void reslv_block_items(const std::vector<std::unique_ptr<CBlockItem>>& list_node) {
    for (const auto& block_item : list_node) {
        switch (block_item->type()) {
            case AST_T::CS_t:
                reslv_statement(static_cast<CS*>(block_item.get())->statement.get());
                break;
            case AST_T::CD_t:
                reslv_declaration(static_cast<CD*>(block_item.get())->declaration.get());
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

static void reslv_block(CBlock* node) {
    switch (node->type()) {
        case AST_T::CB_t:
            reslv_block_items(static_cast<CB*>(node)->block_items);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void reslv_initializer(CInitializer* node, std::shared_ptr<Type>& init_type);

static void reslv_single_init(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    if (node->exp->type() == AST_T::CString_t && init_type->type() == AST_T::Array_t) {
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
        case AST_T::Array_t:
            reslv_arr_init(node, static_cast<Array*>(init_type.get()), init_type);
            break;
        case AST_T::Structure_t:
            reslv_struct_init(node, static_cast<Structure*>(init_type.get()), init_type);
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(
                    MESSAGE_SEMANTIC::scalar_initialized_with_compound_initializer, get_type_hr_c_str(init_type.get())),
                get_compound_line(node));
    }
}

static void reslv_initializer(CInitializer* node, std::shared_ptr<Type>& init_type) {
    switch (node->type()) {
        case AST_T::CSingleInit_t:
            reslv_single_init(static_cast<CSingleInit*>(node), init_type);
            break;
        case AST_T::CCompoundInit_t:
            reslv_compound_init(static_cast<CCompoundInit*>(node), init_type);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void reslv_fun_params_decl(CFunctionDeclaration* node) {
    for (TIdentifier& param : node->params) {
        if (context->scoped_identifier_maps.back().find(param) != context->scoped_identifier_maps.back().end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_redeclared_in_scope, get_name_hr_c_str(param)),
                node->line);
        }
        context->scoped_identifier_maps.back()[param] = resolve_variable_identifier(param);
        param = context->scoped_identifier_maps.back()[param];
    }
    check_fun_params_decl(node);
}

static void reslv_fun_declaration(CFunctionDeclaration* node) {
    if (!is_file_scope()) {
        if (node->body) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::nested_function_defined, get_name_hr_c_str(node->name)),
                node->line);
        }
        else if (node->storage_class && node->storage_class->type() == AST_T::CStatic_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::nested_static_function_declared, get_name_hr_c_str(node->name)),
                node->line);
        }
    }

    if (context->external_linkage_scope_map.find(node->name) == context->external_linkage_scope_map.end()) {
        if (context->scoped_identifier_maps.back().find(node->name) != context->scoped_identifier_maps.back().end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::function_redeclared_in_scope, get_name_hr_c_str(node->name)),
                node->line);
        }
        context->external_linkage_scope_map[node->name] = context->scoped_identifier_maps.size();
    }

    context->scoped_identifier_maps.back()[node->name] = node->name;
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
    if (context->external_linkage_scope_map.find(node->name) == context->external_linkage_scope_map.end()) {
        context->external_linkage_scope_map[node->name] = context->scoped_identifier_maps.size();
    }

    context->scoped_identifier_maps.back()[node->name] = node->name;
    if (is_file_scope()) {
        check_file_var_decl(node);
    }
    else {
        check_block_var_decl(node);
    }
}

static void reslv_block_var_decl(CVariableDeclaration* node) {
    if (context->scoped_identifier_maps.back().find(node->name) != context->scoped_identifier_maps.back().end()
        && !(context->external_linkage_scope_map.find(node->name) != context->external_linkage_scope_map.end()
             && (node->storage_class && node->storage_class->type() == AST_T::CExtern_t))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::variable_redeclared_in_scope, get_name_hr_c_str(node->name)),
            node->line);
    }
    else if (node->storage_class && node->storage_class->type() == AST_T::CExtern_t) {
        reslv_file_var_decl(node);
        return;
    }

    context->scoped_identifier_maps.back()[node->name] = resolve_variable_identifier(node->name);
    node->name = context->scoped_identifier_maps.back()[node->name];
    check_block_var_decl(node);

    if (node->init && !node->storage_class) {
        reslv_initializer(node->init.get(), node->var_type);
    }
}

static void reslv_struct_members_decl(CStructDeclaration* node) { check_struct_members_decl(node); }

static void reslv_struct_declaration(CStructDeclaration* node) {
    if (context->scoped_structure_type_maps.back().find(node->tag)
        != context->scoped_structure_type_maps.back().end()) {
        node->tag = context->scoped_structure_type_maps.back()[node->tag].tag;
        if (node->is_union) {
            if (context->union_definition_set.find(node->tag) == context->union_definition_set.end()) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::structure_conflicts_with_previously_declared,
                        get_struct_name_hr_c_str(node->tag, node->is_union),
                        get_struct_name_hr_c_str(node->tag, !node->is_union)),
                    node->line);
            }
        }
        else {
            if (context->struct_definition_set.find(node->tag) == context->struct_definition_set.end()) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_SEMANTIC_MESSAGE(MESSAGE_SEMANTIC::structure_conflicts_with_previously_declared,
                        get_struct_name_hr_c_str(node->tag, node->is_union),
                        get_struct_name_hr_c_str(node->tag, !node->is_union)),
                    node->line);
            }
        }
    }
    else {
        context->scoped_structure_type_maps.back()[node->tag] = {resolve_structure_tag(node->tag), node->is_union};
        node->tag = context->scoped_structure_type_maps.back()[node->tag].tag;
        if (node->is_union) {
            context->union_definition_set.insert(node->tag);
        }
        else {
            context->struct_definition_set.insert(node->tag);
        }
    }
    if (!node->members.empty()) {
        reslv_struct_members_decl(node);
        check_struct_decl(node);
    }
}

static void reslv_fun_decl(CFunDecl* node) {
    if (is_file_scope()) {
        context->goto_map.clear();
        context->label_set.clear();
        context->break_loop_labels.clear();
        context->continue_loop_labels.clear();
        context->p_switch_statement = nullptr;
    }
    reslv_fun_declaration(node->function_decl.get());
    if (is_file_scope()) {
        reslv_label(node->function_decl.get());
    }
}

static void reslv_var_decl(CVarDecl* node) {
    if (is_file_scope()) {
        reslv_file_var_decl(node->variable_decl.get());
    }
    else {
        reslv_block_var_decl(node->variable_decl.get());
    }
}

static void reslv_struct_decl(CStructDecl* node) { reslv_struct_declaration(node->struct_decl.get()); }

static void reslv_declaration(CDeclaration* node) {
    switch (node->type()) {
        case AST_T::CFunDecl_t:
            reslv_fun_decl(static_cast<CFunDecl*>(node));
            break;
        case AST_T::CVarDecl_t:
            reslv_var_decl(static_cast<CVarDecl*>(node));
            break;
        case AST_T::CStructDecl_t:
            reslv_struct_decl(static_cast<CStructDecl*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void reslv_identifiers(CProgram* node) {
    enter_scope();
    for (const auto& declaration : node->declarations) {
        reslv_declaration(declaration.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void analyze_semantic(CProgram* node) {
    context = std::make_unique<SemanticContext>();
    reslv_identifiers(node);
    context.reset();
}
