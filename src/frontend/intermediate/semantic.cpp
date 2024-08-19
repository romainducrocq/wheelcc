#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/errors.hpp"

#include "frontend/intermediate/names.hpp"
#include "frontend/intermediate/semantic.hpp"

static std::unique_ptr<SemanticContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Type checking

static bool is_same_type(Type* type_1, Type* type_2);

static bool is_pointer_same_type(Pointer* ptr_type_1, Pointer* ptr_type_2) {
    return is_same_type(ptr_type_1->ref_type.get(), ptr_type_2->ref_type.get());
}

static bool is_array_same_type(Array* arr_type_1, Array* arr_type_2) {
    return arr_type_1->size == arr_type_2->size
           && is_same_type(arr_type_1->elem_type.get(), arr_type_2->elem_type.get());
}

static bool is_structure_same_type(Structure* struct_type_1, Structure* struct_type_2) {
    return struct_type_1->tag.compare(struct_type_2->tag) == 0;
}

static bool is_same_type(Type* type_1, Type* type_2) {
    if (type_1->type() == type_2->type()) {
        switch (type_1->type()) {
            case AST_T::Pointer_t:
                return is_pointer_same_type(static_cast<Pointer*>(type_1), static_cast<Pointer*>(type_2));
            case AST_T::Array_t:
                return is_array_same_type(static_cast<Array*>(type_1), static_cast<Array*>(type_2));
            case AST_T::Structure_t:
                return is_structure_same_type(static_cast<Structure*>(type_1), static_cast<Structure*>(type_2));
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

static bool is_type_character(Type* type) {
    switch (type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_integer(Type* type) {
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

static bool is_struct_type_complete(Structure* struct_type) {
    return frontend->struct_typedef_table.find(struct_type->tag) != frontend->struct_typedef_table.end();
}

static bool is_type_complete(Type* type) {
    switch (type->type()) {
        case AST_T::Void_t:
            return false;
        case AST_T::Structure_t:
            return is_struct_type_complete(static_cast<Structure*>(type));
        default:
            return true;
    }
}

static void is_valid_type(Type* type);

static void is_pointer_valid_type(Pointer* ptr_type) { is_valid_type(ptr_type->ref_type.get()); }

static void is_array_valid_type(Array* arr_type) {
    if (!is_type_complete(arr_type->elem_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::array_of_incomplete_type,
                                        get_type_hr(arr_type), get_type_hr(arr_type->elem_type.get())),
            errors->line_buffer);
    }
    is_valid_type(arr_type->elem_type.get());
}

static void is_valid_type(Type* type) {
    switch (type->type()) {
        case AST_T::Pointer_t:
            is_pointer_valid_type(static_cast<Pointer*>(type));
            break;
        case AST_T::Array_t:
            is_array_valid_type(static_cast<Array*>(type));
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

static bool is_constant_null_pointer(CConstant* node) {
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

static TInt get_scalar_type_size(Type* type) {
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

static TLong get_array_aggregate_type_scale(Array* arr_type) {
    TLong size = arr_type->size;
    while (arr_type->elem_type->type() == AST_T::Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    return get_type_scale(arr_type->elem_type.get()) * size;
}

static TLong get_structure_aggregate_type_scale(Structure* struct_type) {
    if (frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        RAISE_INTERNAL_ERROR;
    }
    return frontend->struct_typedef_table[struct_type->tag]->size;
}

static TLong get_type_scale(Type* type) {
    switch (type->type()) {
        case AST_T::Array_t:
            return get_array_aggregate_type_scale(static_cast<Array*>(type));
        case AST_T::Structure_t:
            return get_structure_aggregate_type_scale(static_cast<Structure*>(type));
        default:
            return get_scalar_type_size(type);
    }
}

static TInt get_type_alignment(Type* type);

static TInt get_array_aggregate_type_alignment(Array* arr_type) {
    return get_type_alignment(arr_type->elem_type.get());
}

static TInt get_structure_aggregate_type_alignment(Structure* struct_type) {
    if (frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        RAISE_INTERNAL_ERROR;
    }
    return frontend->struct_typedef_table[struct_type->tag]->alignment;
}

static TInt get_type_alignment(Type* type) {
    switch (type->type()) {
        case AST_T::Array_t:
            return get_array_aggregate_type_alignment(static_cast<Array*>(type));
        case AST_T::Structure_t:
            return get_structure_aggregate_type_alignment(static_cast<Structure*>(type));
        default:
            return get_scalar_type_size(type);
    }
}

static std::shared_ptr<Type> get_joint_type(CExp* node_1, CExp* node_2) {
    if (is_type_character(node_1->exp_type.get())) {
        std::shared_ptr<Type> exp_type = std::move(node_1->exp_type);
        node_1->exp_type = std::make_shared<Int>();
        std::shared_ptr<Type> joint_type = get_joint_type(node_1, node_2);
        node_1->exp_type = std::move(exp_type);
        return joint_type;
    }
    else if (is_type_character(node_2->exp_type.get())) {
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

    TInt type_size_1 = get_scalar_type_size(node_1->exp_type.get());
    TInt type_size_2 = get_scalar_type_size(node_2->exp_type.get());
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

static std::shared_ptr<Type> get_joint_pointer_type(CExp* node_1, CExp* node_2) {
    if (is_same_type(node_1->exp_type.get(), node_2->exp_type.get())) {
        return node_1->exp_type;
    }
    else if (node_1->type() == AST_T::CConstant_t && is_constant_null_pointer(static_cast<CConstant*>(node_1))) {
        return node_2->exp_type;
    }
    else if (node_2->type() == AST_T::CConstant_t && is_constant_null_pointer(static_cast<CConstant*>(node_2))) {
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
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::joint_pointer_type_mismatch,
                                        get_type_hr(node_1->exp_type.get()), get_type_hr(node_2->exp_type.get())),
            node_1->line);
    }
}

static TChar get_char_constant_value(CConstant* node) {
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

static TInt get_int_constant_value(CConstant* node) {
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

static TLong get_long_constant_value(CConstant* node) {
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

static TDouble get_double_constant_value(CConstant* node) {
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

static TUChar get_uchar_constant_value(CConstant* node) {
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

static TUInt get_uint_constant_value(CConstant* node) {
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

static TULong get_ulong_constant_value(CConstant* node) {
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

static size_t get_compound_init_line(CInitializer* node) {
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

static void resolve_struct_type(Type* type);

static void checktype_constant_expression(CConstant* node) {
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

static void checktype_string_expression(CString* node) {
    TLong size = static_cast<TLong>(node->literal->value.size()) + 1l;
    std::shared_ptr<Type> elem_type = std::make_shared<Char>();
    node->exp_type = std::make_shared<Array>(std::move(size), std::move(elem_type));
}

static void checktype_var_expression(CVar* node) {
    if (frontend->symbol_table[node->name]->type_t->type() == AST_T::FunType_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_used_as_variable, get_name_hr(node->name)), node->line);
    }
    node->exp_type = frontend->symbol_table[node->name]->type_t;
}

static void checktype_cast_expression(CCast* node) {
    errors->line_buffer = node->line;
    resolve_struct_type(node->target_type.get());
    if (node->target_type->type() != AST_T::Void_t
        && ((node->exp->exp_type->type() == AST_T::Double_t && node->target_type->type() == AST_T::Pointer_t)
            || (node->exp->exp_type->type() == AST_T::Pointer_t && node->target_type->type() == AST_T::Double_t)
            || !is_type_scalar(node->exp->exp_type.get()) || !is_type_scalar(node->target_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::illegal_conversion_from_type_to,
                                        get_type_hr(node->exp->exp_type.get()), get_type_hr(node->target_type.get())),
            node->line);
    }
    is_valid_type(node->target_type.get());
    node->exp_type = node->target_type;
}

static std::unique_ptr<CCast> cast_expression(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    size_t line = node->line;
    std::unique_ptr<CCast> exp = std::make_unique<CCast>(std::move(node), exp_type, std::move(line));
    checktype_cast_expression(exp.get());
    return exp;
}

static std::unique_ptr<CCast> cast_by_assignment(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    if (is_type_arithmetic(node->exp_type.get()) && is_type_arithmetic(exp_type.get())) {
        return cast_expression(std::move(node), exp_type);
    }
    else if (node->type() == AST_T::CConstant_t && exp_type->type() == AST_T::Pointer_t
             && is_constant_null_pointer(static_cast<CConstant*>(node.get()))) {
        return cast_expression(std::move(node), exp_type);
    }
    else if (exp_type->type() == AST_T::Pointer_t
             && static_cast<Pointer*>(exp_type.get())->ref_type->type() == AST_T::Void_t
             && node->exp_type->type() == AST_T::Pointer_t) {
        return cast_expression(std::move(node), exp_type);
    }
    else if (node->exp_type->type() == AST_T::Pointer_t
             && static_cast<Pointer*>(node->exp_type.get())->ref_type->type() == AST_T::Void_t
             && exp_type->type() == AST_T::Pointer_t) {
        return cast_expression(std::move(node), exp_type);
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::illegal_conversion_from_type_to,
                                        get_type_hr(node->exp_type.get()), get_type_hr(exp_type.get())),
            node->line);
    }
}

static void checktype_unary_not_expression(CUnary* node) {
    if (!is_type_scalar(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                                        get_unary_op_hr(node->unary_op.get()), get_type_hr(node->exp->exp_type.get())),
            node->line);
    }

    node->exp_type = std::make_shared<Int>();
}

static void checktype_unary_complement_expression(CUnary* node) {
    if (!is_type_arithmetic(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                                        get_unary_op_hr(node->unary_op.get()), get_type_hr(node->exp->exp_type.get())),
            node->line);
    }

    switch (node->exp->exp_type->type()) {
        case AST_T::Double_t:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                    get_unary_op_hr(node->unary_op.get()), get_type_hr(node->exp->exp_type.get())),
                node->line);
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t: {
            std::shared_ptr<Type> promote_type = std::make_shared<Int>();
            node->exp = cast_expression(std::move(node->exp), promote_type);
            break;
        }
        default:
            break;
    }
    node->exp_type = node->exp->exp_type;
}

static void checktype_unary_negate_expression(CUnary* node) {
    if (!is_type_arithmetic(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::unary_on_invalid_operand_type,
                                        get_unary_op_hr(node->unary_op.get()), get_type_hr(node->exp->exp_type.get())),
            node->line);
    }

    switch (node->exp->exp_type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t: {
            std::shared_ptr<Type> promote_type = std::make_shared<Int>();
            node->exp = cast_expression(std::move(node->exp), promote_type);
            break;
        }
        default:
            break;
    }
    node->exp_type = node->exp->exp_type;
}

static void checktype_unary_expression(CUnary* node) {
    switch (node->unary_op->type()) {
        case AST_T::CNot_t:
            checktype_unary_not_expression(node);
            break;
        case AST_T::CComplement_t:
            checktype_unary_complement_expression(node);
            break;
        case AST_T::CNegate_t:
            checktype_unary_negate_expression(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void checktype_binary_arithmetic_add_expression(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if (is_type_arithmetic(node->exp_left->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (node->exp_left->exp_type->type() == AST_T::Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())
             && is_type_integer(node->exp_right->exp_type.get())) {
        common_type = std::make_shared<Long>();
        if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
            node->exp_right = cast_expression(std::move(node->exp_right), common_type);
        }
        node->exp_type = node->exp_left->exp_type;
        return;
    }
    else if (is_type_integer(node->exp_left->exp_type.get()) && node->exp_right->exp_type->type() == AST_T::Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->exp_right->exp_type.get())->ref_type.get())) {
        common_type = std::make_shared<Long>();
        if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
            node->exp_left = cast_expression(std::move(node->exp_left), common_type);
        }
        node->exp_type = node->exp_right->exp_type;
        return;
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_binary_arithmetic_subtract_expression(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if (is_type_arithmetic(node->exp_left->exp_type.get()) && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (node->exp_left->exp_type->type() == AST_T::Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())) {
        if (is_type_integer(node->exp_right->exp_type.get())) {
            common_type = std::make_shared<Long>();
            if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
                node->exp_right = cast_expression(std::move(node->exp_right), common_type);
            }
            node->exp_type = node->exp_left->exp_type;
            return;
        }
        else if (is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())
                 && !(node->exp_left->type() == AST_T::CConstant_t
                      && is_constant_null_pointer(static_cast<CConstant*>(node->exp_left.get())))) {
            common_type = std::make_shared<Long>();
            node->exp_type = std::move(common_type);
            return;
        }
        else {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                    get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                    get_type_hr(node->exp_right->exp_type.get())),
                node->line);
        }
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_binary_arithmetic_multiply_divide_expression(CBinary* node) {
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_arithmetic(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_binary_arithmetic_remainder_bitwise_expression(CBinary* node) {
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_arithmetic(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
    if (node->exp_type->type() == AST_T::Double_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_type,
                                        get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_type.get())),
            node->line);
    }
}

static void checktype_binary_arithmetic_bitshift_expression(CBinary* node) {
    if (!is_type_arithmetic(node->exp_left->exp_type.get()) || !is_type_integer(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    else if (is_type_character(node->exp_left->exp_type.get())) {
        std::shared_ptr<Type> left_type = std::make_shared<Int>();
        node->exp_left = cast_expression(std::move(node->exp_left), left_type);
    }
    if (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), node->exp_left->exp_type);
    }
    node->exp_type = node->exp_left->exp_type;
    if (node->exp_type->type() == AST_T::Double_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_type,
                                        get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_type.get())),
            node->line);
    }
}

static void checktype_binary_arithmetic_bitshift_right_expression(CBinary* node) {
    checktype_binary_arithmetic_bitshift_expression(node);
    if (is_type_signed(node->exp_left->exp_type.get())) {
        node->binary_op = std::make_unique<CBitShrArithmetic>();
    }
}

static void checktype_binary_logical_expression(CBinary* node) {
    if (!is_type_scalar(node->exp_left->exp_type.get()) || !is_type_scalar(node->exp_right->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    node->exp_type = std::make_shared<Int>();
}

static void checktype_binary_comparison_equality_expression(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if (node->exp_left->exp_type->type() == AST_T::Pointer_t || node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        common_type = get_joint_pointer_type(node->exp_left.get(), node->exp_right.get());
    }
    else if (is_type_arithmetic(node->exp_left->exp_type.get())
             && is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::make_shared<Int>();
}

static void checktype_binary_comparison_relational_expression(CBinary* node) {
    if (!is_type_scalar(node->exp_left->exp_type.get()) || !is_type_scalar(node->exp_right->exp_type.get())
        || (node->exp_left->exp_type->type() == AST_T::Pointer_t
            && (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())
                || (node->exp_left->type() == AST_T::CConstant_t
                    && is_constant_null_pointer(static_cast<CConstant*>(node->exp_left.get())))
                || (node->exp_right->type() == AST_T::CConstant_t
                    && is_constant_null_pointer(static_cast<CConstant*>(node->exp_right.get())))))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::binary_on_invalid_operand_types,
                get_binary_op_hr(node->binary_op.get()), get_type_hr(node->exp_left->exp_type.get()),
                get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if (!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::make_shared<Int>();
}

static void checktype_binary_expression(CBinary* node) {
    switch (node->binary_op->type()) {
        case AST_T::CAdd_t:
            checktype_binary_arithmetic_add_expression(node);
            break;
        case AST_T::CSubtract_t:
            checktype_binary_arithmetic_subtract_expression(node);
            break;
        case AST_T::CMultiply_t:
        case AST_T::CDivide_t:
            checktype_binary_arithmetic_multiply_divide_expression(node);
            break;
        case AST_T::CRemainder_t:
        case AST_T::CBitAnd_t:
        case AST_T::CBitOr_t:
        case AST_T::CBitXor_t:
            checktype_binary_arithmetic_remainder_bitwise_expression(node);
            break;
        case AST_T::CBitShiftLeft_t:
            checktype_binary_arithmetic_bitshift_expression(node);
            break;
        case AST_T::CBitShiftRight_t:
            checktype_binary_arithmetic_bitshift_right_expression(node);
            break;
        case AST_T::CAnd_t:
        case AST_T::COr_t:
            checktype_binary_logical_expression(node);
            break;
        case AST_T::CEqual_t:
        case AST_T::CNotEqual_t:
            checktype_binary_comparison_equality_expression(node);
            break;
        case AST_T::CLessThan_t:
        case AST_T::CLessOrEqual_t:
        case AST_T::CGreaterThan_t:
        case AST_T::CGreaterOrEqual_t:
            checktype_binary_comparison_relational_expression(node);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void checktype_assignment_expression(CAssignment* node) {
    if (node->exp_left) {
        if (node->exp_left->exp_type->type() == AST_T::Void_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::assignment_to_void_type), node->line);
        }
        else if (!is_exp_lvalue(node->exp_left.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::assignment_to_rvalue, get_assignment_hr(nullptr)),
                node->line);
        }
        else if (!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
            node->exp_right = cast_by_assignment(std::move(node->exp_right), node->exp_left->exp_type);
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
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::assignment_to_rvalue,
                    get_assignment_hr(static_cast<CBinary*>(node->exp_right.get())->binary_op.get())),
                node->line);
        }
        else if (!is_same_type(node->exp_right->exp_type.get(), exp_left->exp_type.get())) {
            node->exp_right = cast_by_assignment(std::move(node->exp_right), exp_left->exp_type);
        }
        node->exp_type = exp_left->exp_type;
    }
}

static void checktype_conditional_expression(CConditional* node) {
    if (!is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::conditional_on_invalid_condition_type,
                                        get_type_hr(node->condition->exp_type.get())),
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
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::ternary_on_invalid_operand_types,
                    get_type_hr(node->exp_middle->exp_type.get()), get_type_hr(node->exp_right->exp_type.get())),
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
        common_type = get_joint_pointer_type(node->exp_middle.get(), node->exp_right.get());
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::ternary_on_invalid_operand_types,
                get_type_hr(node->exp_middle->exp_type.get()), get_type_hr(node->exp_right->exp_type.get())),
            node->line);
    }
    if (!is_same_type(node->exp_middle->exp_type.get(), common_type.get())) {
        node->exp_middle = cast_expression(std::move(node->exp_middle), common_type);
    }
    if (!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_function_call_expression(CFunctionCall* node) {
    if (frontend->symbol_table[node->name]->type_t->type() != AST_T::FunType_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_used_as_function, get_name_hr(node->name)), node->line);
    }
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    if (fun_type->param_types.size() != node->args.size()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_called_with_wrong_number_of_arguments,
                get_name_hr(node->name), std::to_string(node->args.size()),
                std::to_string(fun_type->param_types.size())),
            node->line);
    }
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (!is_same_type(node->args[i]->exp_type.get(), fun_type->param_types[i].get())) {
            node->args[i] = cast_by_assignment(std::move(node->args[i]), fun_type->param_types[i]);
        }
    }
    node->exp_type = fun_type->ret_type;
}

static void checktype_dereference_expression(CDereference* node) {
    if (node->exp->exp_type->type() != AST_T::Pointer_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::dereference_non_pointer, get_type_hr(node->exp->exp_type.get())),
            node->line);
    }
    node->exp_type = static_cast<Pointer*>(node->exp->exp_type.get())->ref_type;
}

static void checktype_addrof_expression(CAddrOf* node) {
    if (!is_exp_lvalue(node->exp.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::address_of_rvalue), node->line);
    }
    std::shared_ptr<Type> ref_type = node->exp->exp_type;
    node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
}

static void checktype_subscript_expression(CSubscript* node) {
    std::shared_ptr<Type> ref_type;
    if (node->primary_exp->exp_type->type() == AST_T::Pointer_t
        && is_type_complete(static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type.get())
        && is_type_integer(node->subscript_exp->exp_type.get())) {
        std::shared_ptr<Type> subscript_type = std::make_shared<Long>();
        if (!is_same_type(node->subscript_exp->exp_type.get(), subscript_type.get())) {
            node->subscript_exp = cast_expression(std::move(node->subscript_exp), subscript_type);
        }
        ref_type = static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type;
    }
    else if (is_type_integer(node->primary_exp->exp_type.get())
             && node->subscript_exp->exp_type->type() == AST_T::Pointer_t
             && is_type_complete(static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type.get())) {
        std::shared_ptr<Type> primary_type = std::make_shared<Long>();
        if (!is_same_type(node->primary_exp->exp_type.get(), primary_type.get())) {
            node->primary_exp = cast_expression(std::move(node->primary_exp), primary_type);
        }
        ref_type = static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type;
    }
    else {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::subscript_array_with_invalid_types,
                get_type_hr(node->primary_exp->exp_type.get()), get_type_hr(node->subscript_exp->exp_type.get())),
            node->line);
    }
    node->exp_type = std::move(ref_type);
}

static void checktype_sizeof_expression(CSizeOf* node) {
    if (!is_type_complete(node->exp->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::get_size_of_incomplete_type,
                                        get_type_hr(node->exp->exp_type.get())),
            node->line);
    }
    node->exp_type = std::make_shared<ULong>();
}

static void checktype_sizeoft_expression(CSizeOfT* node) {
    errors->line_buffer = node->line;
    resolve_struct_type(node->target_type.get());
    if (!is_type_complete(node->target_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::get_size_of_incomplete_type,
                                        get_type_hr(node->target_type.get())),
            node->line);
    }
    is_valid_type(node->target_type.get());
    node->exp_type = std::make_shared<ULong>();
}

static void checktype_dot_expression(CDot* node) {
    if (node->structure->exp_type->type() != AST_T::Structure_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::dot_on_non_structure_type,
                                        get_name_hr(node->member), get_type_hr(node->structure->exp_type.get())),
            node->line);
    }
    Structure* struct_type = static_cast<Structure*>(node->structure->exp_type.get());
    if (frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
        == frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::member_not_in_structure_type,
                                        get_type_hr(struct_type), get_name_hr(node->member)),
            node->line);
    }
    node->exp_type = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
}

static void checktype_arrow_expression(CArrow* node) {
    if (node->pointer->exp_type->type() != AST_T::Pointer_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::arrow_on_non_pointer_to_structure_type,
                                        get_name_hr(node->member), get_type_hr(node->pointer->exp_type.get())),
            node->line);
    }
    Pointer* ptr_type = static_cast<Pointer*>(node->pointer->exp_type.get());
    if (ptr_type->ref_type->type() != AST_T::Structure_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::arrow_on_non_pointer_to_structure_type,
                                        get_name_hr(node->member), get_type_hr(node->pointer->exp_type.get())),
            node->line);
    }
    Structure* struct_type = static_cast<Structure*>(ptr_type->ref_type.get());
    if (frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::arrow_on_incomplete_structure_type,
                                        get_name_hr(node->member), get_type_hr(struct_type)),
            node->line);
    }
    else if (frontend->struct_typedef_table[struct_type->tag]->members.find(node->member)
             == frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::member_not_in_structure_type,
                                        get_type_hr(struct_type), get_name_hr(node->member)),
            node->line);
    }
    node->exp_type = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
}

static std::unique_ptr<CExp> checktype_scalar_typed_expression(std::unique_ptr<CExp>&& node) {
    std::unique_ptr<CExp> exp = std::move(node);
    return exp;
}

static std::unique_ptr<CAddrOf> checktype_array_aggregate_typed_expression(std::unique_ptr<CExp>&& node) {
    {
        std::shared_ptr<Type> ref_type = static_cast<Array*>(node->exp_type.get())->elem_type;
        node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
    }
    size_t line = node->line;
    std::unique_ptr<CAddrOf> addrof = std::make_unique<CAddrOf>(std::move(node), std::move(line));
    addrof->exp_type = addrof->exp->exp_type;
    return addrof;
}

static std::unique_ptr<CExp> checktype_structure_aggregate_typed_expression(std::unique_ptr<CExp>&& node) {
    if (!is_struct_type_complete(static_cast<Structure*>(node->exp_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::incomplete_structure_type_in_expression,
                                        get_type_hr(node->exp_type.get())),
            node->line);
    }

    std::unique_ptr<CExp> exp = std::move(node);
    return exp;
}

static std::unique_ptr<CExp> checktype_typed_expression(std::unique_ptr<CExp>&& node) {
    switch (node->exp_type->type()) {
        case AST_T::Array_t:
            return checktype_array_aggregate_typed_expression(std::move(node));
        case AST_T::Structure_t:
            return checktype_structure_aggregate_typed_expression(std::move(node));
        default:
            return checktype_scalar_typed_expression(std::move(node));
    }
}

static void checktype_return_statement(CReturn* node) {
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[context->function_definition_name]->type_t.get());
    if (fun_type->ret_type->type() == AST_T::Void_t) {
        if (node->exp) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::return_value_in_void_function,
                                            get_name_hr(context->function_definition_name)),
                node->line);
        }
        return;
    }
    else if (!node->exp) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::no_return_value_in_non_void_function,
                get_name_hr(context->function_definition_name), get_type_hr(fun_type->ret_type.get())),
            node->line);
    }

    else if (!is_same_type(node->exp->exp_type.get(), fun_type->ret_type.get())) {
        node->exp = cast_by_assignment(std::move(node->exp), fun_type->ret_type);
    }
    node->exp = checktype_typed_expression(std::move(node->exp));
}

static void checktype_if_statement(CIf* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::if_used_with_condition_type,
                                        get_type_hr(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void checktype_while_statement(CWhile* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::while_used_with_condition_type,
                                        get_type_hr(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void checktype_do_while_statement(CDoWhile* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::do_while_used_with_condition_type,
                                        get_type_hr(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void checktype_for_statement(CFor* node) {
    if (node->condition && !is_type_scalar(node->condition->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::for_used_with_condition_type,
                                        get_type_hr(node->condition->exp_type.get())),
            node->condition->line);
    }
}

static void checktype_switch_statement(CSwitch* node) {
    if (!is_type_integer(node->match->exp_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::switch_used_with_match_type,
                                        get_type_hr(node->match->exp_type.get())),
            node->match->line);
    }
    switch (node->match->exp_type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t: {
            std::shared_ptr<Type> promote_type = std::make_shared<Int>();
            node->match = cast_expression(std::move(node->match), promote_type);
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
                values[i] = get_int_constant_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_ERROR_MESSAGE(
                                ERROR_MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i])),
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
                values[i] = get_long_constant_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_ERROR_MESSAGE(
                                ERROR_MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i])),
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
                values[i] = get_uint_constant_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_ERROR_MESSAGE(
                                ERROR_MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i])),
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
                values[i] = get_ulong_constant_value(esac);
                for (size_t j = 0; j < i; ++j) {
                    if (values[i] == values[j]) {
                        RAISE_RUNTIME_ERROR_AT_LINE(
                            GET_ERROR_MESSAGE(
                                ERROR_MESSAGE_SEMANTIC::duplicate_case_value_in_switch, std::to_string(values[i])),
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

static void checktype_bound_array_single_init_string_initializer(CString* node, Array* arr_type) {
    if (!is_type_character(arr_type->elem_type.get())) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::non_char_array_initialized_from_string, get_type_hr(arr_type)),
            node->line);
    }
    else if (node->literal->value.size() > static_cast<size_t>(arr_type->size)) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::string_initialized_with_too_many_characters,
                std::to_string(arr_type->size), std::to_string(node->literal->value.size())),
            node->line);
    }
}

static void checktype_single_init_initializer(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    if (!is_same_type(node->exp->exp_type.get(), init_type.get())) {
        node->exp = cast_by_assignment(std::move(node->exp), init_type);
    }
    node->init_type = init_type;
}

static void checktype_array_single_init_string_initializer(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    node->exp->exp_type = init_type;
    node->init_type = init_type;
}

static std::unique_ptr<CInitializer> checktype_zero_initializer(Type* init_type);

static std::unique_ptr<CSingleInit> checktype_single_init_zero_initializer(Type* elem_type) {
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

static std::unique_ptr<CCompoundInit> checktype_array_compound_init_zero_initializer(Array* arr_type) {
    std::vector<std::unique_ptr<CInitializer>> zero_initializers;
    size_t arr_type_size = static_cast<size_t>(arr_type->size);
    zero_initializers.reserve(arr_type_size);
    for (size_t i = 0; i < arr_type_size; ++i) {
        std::unique_ptr<CInitializer> initializer = checktype_zero_initializer(arr_type->elem_type.get());
        zero_initializers.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_initializers));
}

static std::unique_ptr<CCompoundInit> checktype_structure_compound_init_zero_initializer(Structure* struct_type) {
    std::vector<std::unique_ptr<CInitializer>> zero_initializers;
    zero_initializers.reserve(frontend->struct_typedef_table[struct_type->tag]->member_names.size());
    for (const auto& member_name : frontend->struct_typedef_table[struct_type->tag]->member_names) {
        const auto& member = frontend->struct_typedef_table[struct_type->tag]->members[member_name];
        std::unique_ptr<CInitializer> initializer = checktype_zero_initializer(member->member_type.get());
        zero_initializers.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_initializers));
}

static std::unique_ptr<CInitializer> checktype_zero_initializer(Type* init_type) {
    switch (init_type->type()) {
        case AST_T::Array_t:
            return checktype_array_compound_init_zero_initializer(static_cast<Array*>(init_type));
        case AST_T::Structure_t:
            return checktype_structure_compound_init_zero_initializer(static_cast<Structure*>(init_type));
        default:
            return checktype_single_init_zero_initializer(init_type);
    }
}

static void checktype_bound_array_compound_init_initializer(CCompoundInit* node, Array* arr_type) {
    if (node->initializers.size() > static_cast<size_t>(arr_type->size)) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::array_initialized_with_too_many_initializers,
                std::to_string(arr_type->size), get_type_hr(arr_type), std::to_string(node->initializers.size())),
            get_compound_init_line(node));
    }
}

static void checktype_bound_structure_compound_init_initializer(CCompoundInit* node, Structure* struct_type) {
    switch (struct_type->data_type->type()) {
        case AST_T::Struct_t: {
            if (node->initializers.size() > frontend->struct_typedef_table[struct_type->tag]->members.size()) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::structure_initialized_with_too_many_members,
                        get_type_hr(struct_type), std::to_string(node->initializers.size()),
                        std::to_string(frontend->struct_typedef_table[struct_type->tag]->members.size())),
                    get_compound_init_line(node));
            }
            break;
        }
        case AST_T::Union_t: {
            if (node->initializers.size() > 1) {
                RAISE_INTERNAL_ERROR;
                // TODO RAISE_RUNTIME_ERROR_AT_LINE
            }
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void checktype_array_compound_init_initializer(
    CCompoundInit* node, Array* arr_type, std::shared_ptr<Type>& init_type) {
    while (node->initializers.size() < static_cast<size_t>(arr_type->size)) {
        std::unique_ptr<CInitializer> zero_initializer = checktype_zero_initializer(arr_type->elem_type.get());
        node->initializers.push_back(std::move(zero_initializer));
    }
    node->init_type = init_type;
}

static void checktype_structure_compound_init_initializer(
    CCompoundInit* node, Structure* struct_type, std::shared_ptr<Type>& init_type) {
    for (size_t i = node->initializers.size(); i < frontend->struct_typedef_table[struct_type->tag]->members.size();
         ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        std::unique_ptr<CInitializer> zero_initializer = checktype_zero_initializer(member->member_type.get());
        node->initializers.push_back(std::move(zero_initializer));
    }
    node->init_type = init_type;
}

static void checktype_return_function_declaration(CFunctionDeclaration* node) {
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    errors->line_buffer = node->line;
    resolve_struct_type(fun_type->ret_type.get());
    is_valid_type(fun_type->ret_type.get());

    switch (fun_type->ret_type->type()) {
        case AST_T::Array_t:
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_returns_array,
                                            get_name_hr(node->name), get_type_hr(fun_type->ret_type.get())),
                node->line);
        case AST_T::Structure_t: {
            if (node->body && !is_struct_type_complete(static_cast<Structure*>(fun_type->ret_type.get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_returns_incomplete_structure_type,
                        get_name_hr(node->name), get_type_hr(fun_type->ret_type.get())),
                    node->line);
            }
            break;
        }
        default:
            break;
    }
}

static void checktype_params_function_declaration(CFunctionDeclaration* node) {
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    for (size_t i = 0; i < node->params.size(); ++i) {
        errors->line_buffer = node->line;
        resolve_struct_type(fun_type->param_types[i].get());
        if (fun_type->param_types[i]->type() == AST_T::Void_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::parameter_with_type_void,
                                            get_name_hr(node->name), get_name_hr(node->params[i])),
                node->line);
        }
        is_valid_type(fun_type->param_types[i].get());
        if (fun_type->param_types[i]->type() == AST_T::Array_t) {
            std::shared_ptr<Type> ref_type = static_cast<Array*>(fun_type->param_types[i].get())->elem_type;
            fun_type->param_types[i] = std::make_shared<Pointer>(std::move(ref_type));
        }

        if (node->body) {
            if (fun_type->param_types[i]->type() == AST_T::Structure_t
                && !is_struct_type_complete(static_cast<Structure*>(fun_type->param_types[i].get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::parameter_with_incomplete_structure_type,
                        get_name_hr(node->name), get_name_hr(node->params[i]),
                        get_type_hr(fun_type->param_types[i].get())),
                    node->line);
            }
            std::shared_ptr<Type> type_t = fun_type->param_types[i];
            std::unique_ptr<IdentifierAttr> param_attrs = std::make_unique<LocalAttr>();
            frontend->symbol_table[node->params[i]] =
                std::make_unique<Symbol>(std::move(type_t), std::move(param_attrs));
        }
    }
}

static void checktype_function_declaration(CFunctionDeclaration* node) {
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
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_redeclared_with_conflicting_type,
                    get_name_hr(node->name), get_type_hr(node->fun_type.get()), get_type_hr(fun_type)),
                node->line);
        }
        else if (is_defined && node->body) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_redefined,
                                            get_name_hr(node->name), get_type_hr(node->fun_type.get())),
                node->line);
        }

        FunAttr* fun_attrs = static_cast<FunAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if (!is_global && fun_attrs->is_global) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::non_static_function_redeclared_static,
                                            get_name_hr(node->name)),
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

static void push_zero_init_static_init(TLong&& byte) {
    if (!context->p_static_inits->empty() && context->p_static_inits->back()->type() == AST_T::ZeroInit_t) {
        static_cast<ZeroInit*>(context->p_static_inits->back().get())->byte += byte;
    }
    else {
        push_static_init(std::make_shared<ZeroInit>(std::move(byte)));
    }
}

static void checktype_initializer_static_init(CInitializer* node, Type* static_init_type);

static void checktype_no_initializer_static_init(Type* static_init_type, TLong size) {
    TLong byte = static_init_type == nullptr ? size : get_type_scale(static_init_type) * size;
    push_zero_init_static_init(std::move(byte));
}

static std::shared_ptr<Initial> checktype_no_initializer_initial(Type* static_init_type) {
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    {
        context->p_static_inits = &static_inits;
        checktype_no_initializer_static_init(static_init_type, 1l);
        context->p_static_inits = nullptr;
    }
    return std::make_shared<Initial>(std::move(static_inits));
}

static void checktype_constant_initializer_static_init(CConstant* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value = get_char_constant_value(node);
            if (value == 0) {
                push_zero_init_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<CharInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Int_t: {
            TInt value = get_int_constant_value(node);
            if (value == 0) {
                push_zero_init_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<IntInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Long_t: {
            TLong value = get_long_constant_value(node);
            if (value == 0l) {
                push_zero_init_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<LongInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Double_t: {
            TDouble value = get_double_constant_value(node);
            TULong binary = double_to_binary(value);
            if (binary == 0ul) {
                push_zero_init_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<DoubleInit>(std::move(value), std::move(binary)));
            }
            break;
        }
        case AST_T::UChar_t: {
            TUChar value = get_uchar_constant_value(node);
            if (value == 0u) {
                push_zero_init_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<UCharInit>(std::move(value)));
            }
            break;
        }
        case AST_T::UInt_t: {
            TUInt value = get_uint_constant_value(node);
            if (value == 0u) {
                push_zero_init_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<UIntInit>(std::move(value)));
            }
            break;
        }
        case AST_T::ULong_t: {
            TULong value = get_ulong_constant_value(node);
            if (value == 0ul) {
                push_zero_init_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<ULongInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Pointer_t: {
            TULong value;
            switch (node->constant->type()) {
                case AST_T::CConstInt_t: {
                    value = static_cast<TULong>(static_cast<CConstInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<TULong>(static_cast<CConstLong*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUInt_t: {
                    value = static_cast<TULong>(static_cast<CConstUInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstULong_t: {
                    value = static_cast<CConstULong*>(node->constant.get())->value;
                    break;
                }
                case AST_T::CConstChar_t:
                case AST_T::CConstDouble_t:
                case AST_T::CConstUChar_t:
                    RAISE_RUNTIME_ERROR_AT_LINE(
                        GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::static_pointer_initialized_from_non_integer,
                            get_type_hr(static_init_type), get_const_hr(node->constant.get())),
                        node->line);
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if (value != 0ul) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::static_pointer_initialized_from_non_null,
                        get_type_hr(static_init_type), std::to_string(value)),
                    node->line);
            }
            push_zero_init_static_init(8l);
            break;
        }
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::aggregate_initialized_with_single_initializer,
                    get_type_hr(static_init_type)),
                node->line);
    }
}

static void checktype_string_initializer_pointer_static_init(CString* node, Pointer* static_ptr_type) {
    if (static_ptr_type->ref_type->type() != AST_T::Char_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(
                ERROR_MESSAGE_SEMANTIC::static_non_char_pointer_initialized_from_string, get_type_hr(static_ptr_type)),
            node->line);
    }

    TIdentifier static_constant_label;
    {
        TIdentifier string_constant = string_literal_to_string_constant(node->literal->value);
        TIdentifier static_constant_hash = std::to_string(std::hash<std::string> {}(string_constant));
        if (frontend->static_constant_table.find(static_constant_hash) != frontend->static_constant_table.end()) {
            static_constant_label = frontend->static_constant_table[static_constant_hash];
        }
        else {
            static_constant_label = represent_label_identifier(LABEL_KIND::Lstring);
            frontend->static_constant_table[static_constant_hash] = static_constant_label;
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
                    TIdentifier string_constant = string_literal_to_string_constant(node->literal->value);
                    std::shared_ptr<CStringLiteral> literal = node->literal;
                    static_init = std::make_shared<StringInit>(true, std::move(string_constant), std::move(literal));
                }
                constant_attrs = std::make_unique<ConstantAttr>(std::move(static_init));
            }
            frontend->symbol_table[static_constant_label] =
                std::make_unique<Symbol>(std::move(constant_type), std::move(constant_attrs));
        }
    }
    push_static_init(std::make_shared<PointerInit>(std::move(static_constant_label)));
}

static void checktype_string_initializer_array_static_init(CString* node, Array* static_arr_type) {
    checktype_bound_array_single_init_string_initializer(node, static_arr_type);
    TLong byte = static_arr_type->size - static_cast<TLong>(node->literal->value.size()) - 1l;
    {
        bool is_null_terminated = byte >= 0l;
        TIdentifier string_constant = string_literal_to_string_constant(node->literal->value);
        std::shared_ptr<CStringLiteral> literal = node->literal;
        push_static_init(std::make_shared<StringInit>(
            std::move(is_null_terminated), std::move(string_constant), std::move(literal)));
    }
    if (byte > 0l) {
        push_zero_init_static_init(std::move(byte));
    }
}

static void checktype_string_initializer_static_init(CString* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_T::Pointer_t:
            checktype_string_initializer_pointer_static_init(node, static_cast<Pointer*>(static_init_type));
            break;
        case AST_T::Array_t:
            checktype_string_initializer_array_static_init(node, static_cast<Array*>(static_init_type));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void checktype_single_init_initializer_static_init(CSingleInit* node, Type* static_init_type) {
    switch (node->exp->type()) {
        case AST_T::CConstant_t:
            checktype_constant_initializer_static_init(static_cast<CConstant*>(node->exp.get()), static_init_type);
            break;
        case AST_T::CString_t:
            checktype_string_initializer_static_init(static_cast<CString*>(node->exp.get()), static_init_type);
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::static_initialized_with_non_constant,
                                            get_type_hr(static_init_type)),
                node->exp->line);
    }
}

static void checktype_array_compound_init_initializer_static_init(CCompoundInit* node, Array* arr_type) {
    checktype_bound_array_compound_init_initializer(node, arr_type);

    for (const auto& initializer : node->initializers) {
        checktype_initializer_static_init(initializer.get(), arr_type->elem_type.get());
    }
    if (static_cast<size_t>(arr_type->size) > node->initializers.size()) {
        checktype_no_initializer_static_init(arr_type->elem_type.get(), arr_type->size - node->initializers.size());
    }
}

static void checktype_structure_compound_init_initializer_static_init(CCompoundInit* node, Structure* struct_type) {
    checktype_bound_structure_compound_init_initializer(node, struct_type);

    TLong size = 0l;
    for (size_t i = 0; i < node->initializers.size(); ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        if (member->offset != size) {
            checktype_no_initializer_static_init(nullptr, member->offset - size);
            size = member->offset;
        }
        checktype_initializer_static_init(node->initializers[i].get(), member->member_type.get());
        size += get_type_scale(member->member_type.get());
    }
    if (frontend->struct_typedef_table[struct_type->tag]->size != size) {
        checktype_no_initializer_static_init(nullptr, frontend->struct_typedef_table[struct_type->tag]->size - size);
    }
}

static void checktype_compound_init_initializer_static_init(CCompoundInit* node, Type* static_init_type) {
    switch (static_init_type->type()) {
        case AST_T::Array_t:
            checktype_array_compound_init_initializer_static_init(node, static_cast<Array*>(static_init_type));
            break;
        case AST_T::Structure_t:
            checktype_structure_compound_init_initializer_static_init(node, static_cast<Structure*>(static_init_type));
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::scalar_initialized_with_compound_initializer,
                    get_type_hr(static_init_type)),
                get_compound_init_line(node));
    }
}

static void checktype_initializer_static_init(CInitializer* node, Type* static_init_type) {
    switch (node->type()) {
        case AST_T::CSingleInit_t:
            checktype_single_init_initializer_static_init(static_cast<CSingleInit*>(node), static_init_type);
            break;
        case AST_T::CCompoundInit_t:
            checktype_compound_init_initializer_static_init(static_cast<CCompoundInit*>(node), static_init_type);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<Initial> checktype_initializer_initial(CInitializer* node, Type* static_init_type) {
    std::vector<std::shared_ptr<StaticInit>> static_inits;
    {
        context->p_static_inits = &static_inits;
        checktype_initializer_static_init(node, static_init_type);
        context->p_static_inits = nullptr;
    }
    return std::make_shared<Initial>(std::move(static_inits));
}

static void checktype_file_scope_variable_declaration(CVariableDeclaration* node) {
    errors->line_buffer = node->line;
    resolve_struct_type(node->var_type.get());
    if (node->var_type->type() == AST_T::Void_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_declared_with_type_void, get_name_hr(node->name)),
            node->line);
    }
    is_valid_type(node->var_type.get());

    std::shared_ptr<InitialValue> initial_value;
    bool is_global = !(node->storage_class && node->storage_class->type() == AST_T::CStatic_t);

    if (node->init) {
        if (node->var_type->type() == AST_T::Structure_t
            && !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type,
                    get_name_hr(node->name), get_type_hr(node->var_type.get())),
                node->line);
        }
        initial_value = checktype_initializer_initial(node->init.get(), node->var_type.get());
    }
    else {
        if (node->storage_class && node->storage_class->type() == AST_T::CExtern_t) {
            initial_value = std::make_shared<NoInitializer>();
        }
        else {
            if (node->var_type->type() == AST_T::Structure_t
                && !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type,
                        get_name_hr(node->name), get_type_hr(node->var_type.get())),
                    node->line);
            }
            initial_value = std::make_shared<Tentative>();
        }
    }

    if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if (!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_type,
                    get_name_hr(node->name), get_type_hr(node->var_type.get()),
                    get_type_hr(frontend->symbol_table[node->name]->type_t.get())),
                node->line);
        }

        StaticAttr* global_var_attrs = static_cast<StaticAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if (node->storage_class && node->storage_class->type() == AST_T::CExtern_t) {
            is_global = global_var_attrs->is_global;
        }
        else if (is_global != global_var_attrs->is_global) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(
                    ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_storage, get_name_hr(node->name)),
                node->line);
        }

        if (global_var_attrs->init->type() == AST_T::Initial_t) {
            if (initial_value->type() == AST_T::Initial_t) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_ERROR_MESSAGE(
                        ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_storage, get_name_hr(node->name)),
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

static void checktype_extern_block_scope_variable_declaration(CVariableDeclaration* node) {
    if (node->init) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::extern_variable_defined, get_name_hr(node->name)), node->line);
    }
    else if (frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if (!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_redeclared_with_conflicting_type,
                    get_name_hr(node->name), get_type_hr(node->var_type.get()),
                    get_type_hr(frontend->symbol_table[node->name]->type_t.get())),
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

static void checktype_static_block_scope_variable_declaration(CVariableDeclaration* node) {
    if (node->var_type->type() == AST_T::Structure_t
        && !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
        RAISE_INTERNAL_ERROR;
    }

    std::shared_ptr<InitialValue> initial_value;
    if (node->init) {
        initial_value = checktype_initializer_initial(node->init.get(), node->var_type.get());
    }
    else {
        initial_value = checktype_no_initializer_initial(node->var_type.get());
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<StaticAttr>(false, std::move(initial_value));
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void checktype_automatic_block_scope_variable_declaration(CVariableDeclaration* node) {
    if (node->var_type->type() == AST_T::Structure_t
        && !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_declared_with_incomplete_structure_type,
                get_name_hr(node->name), get_type_hr(node->var_type.get())),
            node->line);
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<LocalAttr>();
    frontend->symbol_table[node->name] =
        std::make_unique<Symbol>(std::move(local_var_type), std::move(local_var_attrs));
}

static void checktype_block_scope_variable_declaration(CVariableDeclaration* node) {
    errors->line_buffer = node->line;
    resolve_struct_type(node->var_type.get());
    if (node->var_type->type() == AST_T::Void_t) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_declared_with_type_void, get_name_hr(node->name)),
            node->line);
    }
    is_valid_type(node->var_type.get());

    if (node->storage_class) {
        switch (node->storage_class->type()) {
            case AST_T::CExtern_t:
                checktype_extern_block_scope_variable_declaration(node);
                break;
            case AST_T::CStatic_t:
                checktype_static_block_scope_variable_declaration(node);
                break;
            default:
                break;
        }
    }
    else {
        checktype_automatic_block_scope_variable_declaration(node);
    }
}

static void checktype_members_structure_declaration(CStructDeclaration* node) {
    for (size_t i = 0; i < node->members.size(); ++i) {
        for (size_t j = i + 1; j < node->members.size(); ++j) {
            if (node->members[i]->member_name.compare(node->members[j]->member_name) == 0) {
                RAISE_RUNTIME_ERROR_AT_LINE(
                    GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::structure_declared_with_duplicate_member,
                        get_struct_name_hr(node->tag), get_name_hr(node->members[i]->member_name)),
                    node->members[i]->line);
            }
        }
        if (node->members[i].get()->member_type->type() == AST_T::FunType_t) {
            RAISE_INTERNAL_ERROR;
        }
        errors->line_buffer = node->members[i]->line;
        resolve_struct_type(node->members[i].get()->member_type.get());
        if (!is_type_complete(node->members[i].get()->member_type.get())) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::member_declared_with_incomplete_type,
                                            get_struct_name_hr(node->tag), get_name_hr(node->members[i]->member_name),
                                            get_type_hr(node->members[i].get()->member_type.get())),
                node->members[i]->line);
        }
        is_valid_type(node->members[i].get()->member_type.get());
    }
}

static void checktype_structure_declaration(CStructDeclaration* node) {
    if (frontend->struct_typedef_table.find(node->tag) != frontend->struct_typedef_table.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::structure_redeclared_in_scope, get_struct_name_hr(node->tag)),
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
            member_names.push_back(std::move(name));
        }
        TInt member_alignment = get_type_alignment(member->member_type.get());
        TLong member_size = get_type_scale(member->member_type.get());
        {
            TLong offset = 0l;
            switch (node->data_type->type()) {
                case AST_T::Struct_t: {
                    offset = size % member_alignment;
                    if (offset != 0l) {
                        size += member_alignment - offset;
                    }
                    offset = size;
                    size += member_size;
                    break;
                }
                case AST_T::Union_t: {
                    if (size < member_size) {
                        size = member_size;
                    }
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
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
    // TODO is DataStructureType needed in StructTypedef? => no?
    std::shared_ptr<DataStructureType> data_type = node->data_type;
    frontend->struct_typedef_table[node->tag] = std::make_unique<StructTypedef>(
        std::move(alignment), std::move(size), std::move(member_names), std::move(data_type), std::move(members));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loop labeling

static void annotate_goto_label(CLabel* node) {
    if (context->label_set.find(node->target) != context->label_set.end()) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::label_redefined_in_scope, get_name_hr(node->target)), node->line);
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
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::case_outside_of_switch), node->value->line);
    }
    node->target = std::to_string(context->p_switch_statement->cases.size());
    node->target += context->p_switch_statement->target;
}

static void annotate_default_jump(CDefault* node) {
    if (!context->p_switch_statement) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::default_outside_of_switch), node->line);
    }
    else if (context->p_switch_statement->is_default) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::more_than_one_default_in_switch), node->line);
    }
    node->target = context->p_switch_statement->target;
    context->p_switch_statement->is_default = true;
}

static void annotate_break_jump(CBreak* node) {
    if (context->break_loop_labels.empty()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::break_outside_of_loop), node->line);
    }
    node->target = context->break_loop_labels.back();
}

static void annotate_continue_jump(CContinue* node) {
    if (context->continue_loop_labels.empty()) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::continue_outside_of_loop), node->line);
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

static size_t current_scope_depth() { return context->scoped_identifier_maps.size(); }

static bool is_file_scope() { return current_scope_depth() == 1; }

static void enter_scope() {
    context->scoped_identifier_maps.emplace_back();
    context->scoped_structure_type_maps.emplace_back();
}

static void exit_scope() {
    for (const auto& identifier : context->scoped_identifier_maps.back()) {
        if (context->external_linkage_scope_map.find(identifier.first) != context->external_linkage_scope_map.end()
            && context->external_linkage_scope_map[identifier.first] == current_scope_depth()) {
            context->external_linkage_scope_map.erase(identifier.first);
        }
    }
    context->scoped_identifier_maps.pop_back();
    context->scoped_structure_type_maps.pop_back();
}

static void resolve_label(CFunctionDeclaration* node) {
    for (const auto& target : context->goto_map) {
        if (context->label_set.find(target.first) == context->label_set.end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::goto_with_undefined_target_label,
                                            get_name_hr(target.first), get_name_hr(node->name)),
                errors->line_buffer_map[target.second]);
        }
    }
}

static void resolve_pointer_struct_type(Pointer* ptr_type) { resolve_struct_type(ptr_type->ref_type.get()); }

static void resolve_array_struct_type(Array* arr_type) { resolve_struct_type(arr_type->elem_type.get()); }

static void resolve_structure_struct_type(Structure* struct_type) {
    switch (struct_type->data_type->type()) {
        case AST_T::Struct_t: {
            if (context->struct_definition_set.find(struct_type->tag) != context->struct_definition_set.end()) {
                return;
            }
            break;
        }
        case AST_T::Union_t: {
            if (context->union_definition_set.find(struct_type->tag) != context->union_definition_set.end()) {
                return;
            }
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    for (size_t i = current_scope_depth(); i-- > 0;) {
        if (context->scoped_structure_type_maps[i].find(struct_type->tag)
            != context->scoped_structure_type_maps[i].end()) {
            if (context->scoped_structure_type_maps[i][struct_type->tag].type != struct_type->data_type->type()) {
                // TODO RAISE_RUNTIME_ERROR_AT_LINE
                RAISE_INTERNAL_ERROR;
            }
            struct_type->tag = context->scoped_structure_type_maps[i][struct_type->tag].tag;
            return;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(
        GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::structure_not_defined_in_scope, get_type_hr(struct_type)),
        errors->line_buffer);
}

static void resolve_struct_type(Type* type) {
    switch (type->type()) {
        case AST_T::Pointer_t:
            resolve_pointer_struct_type(static_cast<Pointer*>(type));
            break;
        case AST_T::Array_t:
            resolve_array_struct_type(static_cast<Array*>(type));
            break;
        case AST_T::Structure_t:
            resolve_structure_struct_type(static_cast<Structure*>(type));
            break;
        case AST_T::FunType_t:
            RAISE_INTERNAL_ERROR;
        default:
            break;
    }
}

static void resolve_expression(CExp* node);
static std::unique_ptr<CExp> resolve_typed_expression(std::unique_ptr<CExp>&& node);

static void resolve_constant_expression(CConstant* node) { checktype_constant_expression(node); }

static void resolve_string_expression(CString* node) { checktype_string_expression(node); }

static void resolve_var_expression(CVar* node) {
    for (size_t i = current_scope_depth(); i-- > 0;) {
        if (context->scoped_identifier_maps[i].find(node->name) != context->scoped_identifier_maps[i].end()) {
            node->name = context->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(
        GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_not_declared_in_scope, get_name_hr(node->name)), node->line);
Lelse:

    checktype_var_expression(node);
}

static void resolve_cast_expression(CCast* node) {
    node->exp = resolve_typed_expression(std::move(node->exp));
    checktype_cast_expression(node);
}

static void resolve_unary_expression(CUnary* node) {
    node->exp = resolve_typed_expression(std::move(node->exp));
    checktype_unary_expression(node);
}

static void resolve_binary_expression(CBinary* node) {
    node->exp_left = resolve_typed_expression(std::move(node->exp_left));
    node->exp_right = resolve_typed_expression(std::move(node->exp_right));
    checktype_binary_expression(node);
}

static void resolve_assignment_expression(CAssignment* node) {
    if (node->exp_left) {
        node->exp_left = resolve_typed_expression(std::move(node->exp_left));
    }
    node->exp_right = resolve_typed_expression(std::move(node->exp_right));
    checktype_assignment_expression(node);
}

static void resolve_conditional_expression(CConditional* node) {
    node->condition = resolve_typed_expression(std::move(node->condition));
    node->exp_middle = resolve_typed_expression(std::move(node->exp_middle));
    node->exp_right = resolve_typed_expression(std::move(node->exp_right));
    checktype_conditional_expression(node);
}

static void resolve_function_call_expression(CFunctionCall* node) {
    for (size_t i = current_scope_depth(); i-- > 0;) {
        if (context->scoped_identifier_maps[i].find(node->name) != context->scoped_identifier_maps[i].end()) {
            node->name = context->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    RAISE_RUNTIME_ERROR_AT_LINE(
        GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_not_declared_in_scope, get_name_hr(node->name)), node->line);
Lelse:

    for (size_t i = 0; i < node->args.size(); ++i) {
        node->args[i] = resolve_typed_expression(std::move(node->args[i]));
    }
    checktype_function_call_expression(node);
}

static void resolve_dereference_expression(CDereference* node) {
    node->exp = resolve_typed_expression(std::move(node->exp));
    checktype_dereference_expression(node);
}

static void resolve_addrof_expression(CAddrOf* node) {
    resolve_expression(node->exp.get());
    checktype_addrof_expression(node);
}

static void resolve_subscript_expression(CSubscript* node) {
    node->primary_exp = resolve_typed_expression(std::move(node->primary_exp));
    node->subscript_exp = resolve_typed_expression(std::move(node->subscript_exp));
    checktype_subscript_expression(node);
}

static void resolve_sizeof_expression(CSizeOf* node) {
    resolve_expression(node->exp.get());
    checktype_sizeof_expression(node);
}

static void resolve_sizeoft_expression(CSizeOfT* node) { checktype_sizeoft_expression(node); }

static void resolve_dot_expression(CDot* node) {
    node->structure = resolve_typed_expression(std::move(node->structure));
    checktype_dot_expression(node);
}

static void resolve_arrow_expression(CArrow* node) {
    node->pointer = resolve_typed_expression(std::move(node->pointer));
    checktype_arrow_expression(node);
}

static void resolve_expression(CExp* node) {
    switch (node->type()) {
        case AST_T::CConstant_t:
            resolve_constant_expression(static_cast<CConstant*>(node));
            break;
        case AST_T::CString_t:
            resolve_string_expression(static_cast<CString*>(node));
            break;
        case AST_T::CVar_t:
            resolve_var_expression(static_cast<CVar*>(node));
            break;
        case AST_T::CCast_t:
            resolve_cast_expression(static_cast<CCast*>(node));
            break;
        case AST_T::CUnary_t:
            resolve_unary_expression(static_cast<CUnary*>(node));
            break;
        case AST_T::CBinary_t:
            resolve_binary_expression(static_cast<CBinary*>(node));
            break;
        case AST_T::CAssignment_t:
            resolve_assignment_expression(static_cast<CAssignment*>(node));
            break;
        case AST_T::CConditional_t:
            resolve_conditional_expression(static_cast<CConditional*>(node));
            break;
        case AST_T::CFunctionCall_t:
            resolve_function_call_expression(static_cast<CFunctionCall*>(node));
            break;
        case AST_T::CDereference_t:
            resolve_dereference_expression(static_cast<CDereference*>(node));
            break;
        case AST_T::CAddrOf_t:
            resolve_addrof_expression(static_cast<CAddrOf*>(node));
            break;
        case AST_T::CSubscript_t:
            resolve_subscript_expression(static_cast<CSubscript*>(node));
            break;
        case AST_T::CSizeOf_t:
            resolve_sizeof_expression(static_cast<CSizeOf*>(node));
            break;
        case AST_T::CSizeOfT_t:
            resolve_sizeoft_expression(static_cast<CSizeOfT*>(node));
            break;
        case AST_T::CDot_t:
            resolve_dot_expression(static_cast<CDot*>(node));
            break;
        case AST_T::CArrow_t:
            resolve_arrow_expression(static_cast<CArrow*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::unique_ptr<CExp> resolve_typed_expression(std::unique_ptr<CExp>&& node) {
    resolve_expression(node.get());
    return checktype_typed_expression(std::move(node));
}

static void resolve_block(CBlock* node);
static void resolve_block_scope_variable_declaration(CVariableDeclaration* node);

static void resolve_statement(CStatement* node);

static void resolve_init_decl_for_init(CInitDecl* node) {
    if (node->init->storage_class) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::for_initial_declared_with_non_automatic_storage,
                get_name_hr(node->init->name), get_storage_class_hr(node->init->storage_class.get())),
            node->init->line);
    }
    resolve_block_scope_variable_declaration(node->init.get());
}

static void resolve_init_exp_for_init(CInitExp* node) {
    if (node->init) {
        node->init = resolve_typed_expression(std::move(node->init));
    }
}

static void resolve_for_init(CForInit* node) {
    switch (node->type()) {
        case AST_T::CInitDecl_t:
            resolve_init_decl_for_init(static_cast<CInitDecl*>(node));
            break;
        case AST_T::CInitExp_t: {
            resolve_init_exp_for_init(static_cast<CInitExp*>(node));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_return_statement(CReturn* node) {
    if (node->exp) {
        node->exp = resolve_typed_expression(std::move(node->exp));
    }
    checktype_return_statement(node);
}

static void resolve_expression_statement(CExpression* node) {
    node->exp = resolve_typed_expression(std::move(node->exp));
}

static void resolve_if_statement(CIf* node) {
    node->condition = resolve_typed_expression(std::move(node->condition));
    resolve_statement(node->then.get());
    if (node->else_fi) {
        resolve_statement(node->else_fi.get());
    }
    checktype_if_statement(node);
}

static void resolve_goto_statement(CGoto* node) {
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

static void resolve_label_statement(CLabel* node) {
    annotate_goto_label(node);
    if (context->goto_map.find(node->target) != context->goto_map.end()) {
        node->target = context->goto_map[node->target];
    }
    else {
        context->goto_map[node->target] = resolve_label_identifier(node->target);
        node->target = context->goto_map[node->target];
    }
    resolve_statement(node->jump_to.get());
}

static void resolve_compound_statement(CCompound* node) {
    enter_scope();
    resolve_block(node->block.get());
    exit_scope();
}

static void resolve_while_statement(CWhile* node) {
    annotate_while_loop(node);
    node->condition = resolve_typed_expression(std::move(node->condition));
    resolve_statement(node->body.get());
    deannotate_loop();
    checktype_while_statement(node);
}

static void resolve_do_while_statement(CDoWhile* node) {
    annotate_do_while_loop(node);
    resolve_statement(node->body.get());
    node->condition = resolve_typed_expression(std::move(node->condition));
    deannotate_loop();
    checktype_do_while_statement(node);
}

static void resolve_for_statement(CFor* node) {
    annotate_for_loop(node);
    enter_scope();
    resolve_for_init(node->init.get());
    if (node->condition) {
        node->condition = resolve_typed_expression(std::move(node->condition));
    }
    if (node->post) {
        node->post = resolve_typed_expression(std::move(node->post));
    }
    resolve_statement(node->body.get());
    exit_scope();
    deannotate_loop();
    checktype_for_statement(node);
}

static void resolve_switch_statement(CSwitch* node) {
    annotate_switch_lookup(node);
    enter_scope();
    node->match = resolve_typed_expression(std::move(node->match));
    {
        CSwitch* p_switch_statement = context->p_switch_statement;
        context->p_switch_statement = node;
        resolve_statement(node->body.get());
        context->p_switch_statement = p_switch_statement;
    }
    exit_scope();
    deannotate_lookup();
    checktype_switch_statement(node);
}

static void resolve_case_statement(CCase* node) {
    annotate_case_jump(node);
    node->value = resolve_typed_expression(std::move(node->value));
    context->p_switch_statement->cases.push_back(std::move(node->value));
    resolve_statement(node->jump_to.get());
}

static void resolve_default_statement(CDefault* node) {
    annotate_default_jump(node);
    resolve_statement(node->jump_to.get());
}

static void resolve_break_statement(CBreak* node) { annotate_break_jump(node); }

static void resolve_continue_statement(CContinue* node) { annotate_continue_jump(node); }

static void resolve_statement(CStatement* node) {
    switch (node->type()) {
        case AST_T::CReturn_t:
            resolve_return_statement(static_cast<CReturn*>(node));
            break;
        case AST_T::CExpression_t:
            resolve_expression_statement(static_cast<CExpression*>(node));
            break;
        case AST_T::CIf_t:
            resolve_if_statement(static_cast<CIf*>(node));
            break;
        case AST_T::CGoto_t:
            resolve_goto_statement(static_cast<CGoto*>(node));
            break;
        case AST_T::CLabel_t:
            resolve_label_statement(static_cast<CLabel*>(node));
            break;
        case AST_T::CCompound_t:
            resolve_compound_statement(static_cast<CCompound*>(node));
            break;
        case AST_T::CWhile_t:
            resolve_while_statement(static_cast<CWhile*>(node));
            break;
        case AST_T::CDoWhile_t:
            resolve_do_while_statement(static_cast<CDoWhile*>(node));
            break;
        case AST_T::CFor_t:
            resolve_for_statement(static_cast<CFor*>(node));
            break;
        case AST_T::CSwitch_t:
            resolve_switch_statement(static_cast<CSwitch*>(node));
            break;
        case AST_T::CCase_t:
            resolve_case_statement(static_cast<CCase*>(node));
            break;
        case AST_T::CDefault_t:
            resolve_default_statement(static_cast<CDefault*>(node));
            break;
        case AST_T::CBreak_t:
            resolve_break_statement(static_cast<CBreak*>(node));
            break;
        case AST_T::CContinue_t:
            resolve_continue_statement(static_cast<CContinue*>(node));
            break;
        case AST_T::CNull_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_declaration(CDeclaration* node);

static void resolve_block_items(const std::vector<std::unique_ptr<CBlockItem>>& list_node) {
    for (const auto& block_item : list_node) {
        switch (block_item->type()) {
            case AST_T::CS_t:
                resolve_statement(static_cast<CS*>(block_item.get())->statement.get());
                break;
            case AST_T::CD_t:
                resolve_declaration(static_cast<CD*>(block_item.get())->declaration.get());
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

static void resolve_block(CBlock* node) {
    switch (node->type()) {
        case AST_T::CB_t:
            resolve_block_items(static_cast<CB*>(node)->block_items);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_initializer(CInitializer* node, std::shared_ptr<Type>& init_type);

static void resolve_single_init_initializer(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    if (node->exp->type() == AST_T::CString_t && init_type->type() == AST_T::Array_t) {
        checktype_bound_array_single_init_string_initializer(
            static_cast<CString*>(node->exp.get()), static_cast<Array*>(init_type.get()));
        checktype_array_single_init_string_initializer(node, init_type);
    }
    else {
        node->exp = resolve_typed_expression(std::move(node->exp));
        checktype_single_init_initializer(node, init_type);
    }
}

static void resolve_array_compound_init_initializer(
    CCompoundInit* node, Array* arr_type, std::shared_ptr<Type>& init_type) {
    checktype_bound_array_compound_init_initializer(node, arr_type);

    for (const auto& initializer : node->initializers) {
        resolve_initializer(initializer.get(), arr_type->elem_type);
    }
    checktype_array_compound_init_initializer(node, arr_type, init_type);
}

static void resolve_structure_compound_init_initializer(
    CCompoundInit* node, Structure* struct_type, std::shared_ptr<Type>& init_type) {
    checktype_bound_structure_compound_init_initializer(node, struct_type);

    for (size_t i = 0; i < node->initializers.size(); ++i) {
        const auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, i);
        resolve_initializer(node->initializers[i].get(), member->member_type);
    }
    checktype_structure_compound_init_initializer(node, struct_type, init_type);
}

static void resolve_compound_init_initializer(CCompoundInit* node, std::shared_ptr<Type>& init_type) {
    switch (init_type->type()) {
        case AST_T::Array_t:
            resolve_array_compound_init_initializer(node, static_cast<Array*>(init_type.get()), init_type);
            break;
        case AST_T::Structure_t:
            resolve_structure_compound_init_initializer(node, static_cast<Structure*>(init_type.get()), init_type);
            break;
        default:
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(
                    ERROR_MESSAGE_SEMANTIC::scalar_initialized_with_compound_initializer, get_type_hr(init_type.get())),
                get_compound_init_line(node));
    }
}

static void resolve_initializer(CInitializer* node, std::shared_ptr<Type>& init_type) {
    switch (node->type()) {
        case AST_T::CSingleInit_t:
            resolve_single_init_initializer(static_cast<CSingleInit*>(node), init_type);
            break;
        case AST_T::CCompoundInit_t:
            resolve_compound_init_initializer(static_cast<CCompoundInit*>(node), init_type);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_params_function_declaration(CFunctionDeclaration* node) {
    for (auto& param : node->params) {
        if (context->scoped_identifier_maps.back().find(param) != context->scoped_identifier_maps.back().end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_redeclared_in_scope, get_name_hr(param)),
                node->line);
        }
        context->scoped_identifier_maps.back()[param] = resolve_variable_identifier(param);
        param = context->scoped_identifier_maps.back()[param];
    }
    checktype_params_function_declaration(node);
}

static void resolve_function_declaration(CFunctionDeclaration* node) {
    if (!is_file_scope()) {
        if (node->body) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::nested_function_defined, get_name_hr(node->name)),
                node->line);
        }
        else if (node->storage_class && node->storage_class->type() == AST_T::CStatic_t) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::nested_static_function_declared, get_name_hr(node->name)),
                node->line);
        }
    }

    if (context->external_linkage_scope_map.find(node->name) == context->external_linkage_scope_map.end()) {
        if (context->scoped_identifier_maps.back().find(node->name) != context->scoped_identifier_maps.back().end()) {
            RAISE_RUNTIME_ERROR_AT_LINE(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::function_redeclared_in_scope, get_name_hr(node->name)),
                node->line);
        }
        context->external_linkage_scope_map[node->name] = current_scope_depth();
    }

    context->scoped_identifier_maps.back()[node->name] = node->name;
    checktype_return_function_declaration(node);

    enter_scope();
    if (!node->params.empty()) {
        resolve_params_function_declaration(node);
    }
    checktype_function_declaration(node);

    if (node->body) {
        resolve_block(node->body.get());
    }
    exit_scope();
}

static void resolve_file_scope_variable_declaration(CVariableDeclaration* node) {
    if (context->external_linkage_scope_map.find(node->name) == context->external_linkage_scope_map.end()) {
        context->external_linkage_scope_map[node->name] = current_scope_depth();
    }

    context->scoped_identifier_maps.back()[node->name] = node->name;
    if (is_file_scope()) {
        checktype_file_scope_variable_declaration(node);
    }
    else {
        checktype_block_scope_variable_declaration(node);
    }
}

static void resolve_block_scope_variable_declaration(CVariableDeclaration* node) {
    if (context->scoped_identifier_maps.back().find(node->name) != context->scoped_identifier_maps.back().end()
        && !(context->external_linkage_scope_map.find(node->name) != context->external_linkage_scope_map.end()
             && (node->storage_class && node->storage_class->type() == AST_T::CExtern_t))) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_ERROR_MESSAGE(ERROR_MESSAGE_SEMANTIC::variable_redeclared_in_scope, get_name_hr(node->name)),
            node->line);
    }
    else if (node->storage_class && node->storage_class->type() == AST_T::CExtern_t) {
        resolve_file_scope_variable_declaration(node);
        return;
    }

    context->scoped_identifier_maps.back()[node->name] = resolve_variable_identifier(node->name);
    node->name = context->scoped_identifier_maps.back()[node->name];
    checktype_block_scope_variable_declaration(node);

    if (node->init && !node->storage_class) {
        resolve_initializer(node->init.get(), node->var_type);
    }
}

static void resolve_members_structure_declaration(CStructDeclaration* node) {
    checktype_members_structure_declaration(node);
}

static void resolve_structure_declaration(CStructDeclaration* node) {
    if (context->scoped_structure_type_maps.back().find(node->tag)
        != context->scoped_structure_type_maps.back().end()) {
        node->tag = context->scoped_structure_type_maps.back()[node->tag].tag;
        switch (node->data_type->type()) {
            case AST_T::Struct_t: {
                if (context->struct_definition_set.find(node->tag) == context->struct_definition_set.end()) {
                    // TODO RAISE_RUNTIME_ERROR_AT_LINE
                    RAISE_INTERNAL_ERROR;
                }
                break;
            }
            case AST_T::Union_t: {
                if (context->union_definition_set.find(node->tag) == context->union_definition_set.end()) {
                    // TODO RAISE_RUNTIME_ERROR_AT_LINE
                    RAISE_INTERNAL_ERROR;
                }
                break;
            }
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    else {
        context->scoped_structure_type_maps.back()[node->tag] = {
            node->data_type->type(), resolve_structure_tag(node->tag)};
        node->tag = context->scoped_structure_type_maps.back()[node->tag].tag;
        switch (node->data_type->type()) {
            case AST_T::Struct_t:
                context->struct_definition_set.insert(node->tag);
                break;
            case AST_T::Union_t:
                context->union_definition_set.insert(node->tag);
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    if (!node->members.empty()) {
        resolve_members_structure_declaration(node);
        checktype_structure_declaration(node);
    }
}

static void resolve_fun_decl_declaration(CFunDecl* node) {
    if (is_file_scope()) {
        context->goto_map.clear();
        context->label_set.clear();
        context->break_loop_labels.clear();
        context->continue_loop_labels.clear();
        context->p_switch_statement = nullptr;
    }
    resolve_function_declaration(node->function_decl.get());
    if (is_file_scope()) {
        resolve_label(node->function_decl.get());
    }
}

static void resolve_var_decl_declaration(CVarDecl* node) {
    if (is_file_scope()) {
        resolve_file_scope_variable_declaration(node->variable_decl.get());
    }
    else {
        resolve_block_scope_variable_declaration(node->variable_decl.get());
    }
}

static void resolve_struct_decl_declaration(CStructDecl* node) {
    resolve_structure_declaration(node->struct_decl.get());
}

static void resolve_declaration(CDeclaration* node) {
    switch (node->type()) {
        case AST_T::CFunDecl_t:
            resolve_fun_decl_declaration(static_cast<CFunDecl*>(node));
            break;
        case AST_T::CVarDecl_t:
            resolve_var_decl_declaration(static_cast<CVarDecl*>(node));
            break;
        case AST_T::CStructDecl_t:
            resolve_struct_decl_declaration(static_cast<CStructDecl*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_identifiers(CProgram* node) {
    enter_scope();
    for (const auto& declaration : node->declarations) {
        resolve_declaration(declaration.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void analyze_semantic(CProgram* node) {
    context = std::make_unique<SemanticContext>();
    resolve_identifiers(node);
    context.reset();
}
