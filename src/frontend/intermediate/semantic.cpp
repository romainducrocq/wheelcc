#include "frontend/intermediate/semantic.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/front_ast.hpp"
#include "frontend/intermediate/names.hpp"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

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
    return arr_type_1->size == arr_type_2->size &&
           is_same_type(arr_type_1->elem_type.get(), arr_type_2->elem_type.get());
}

static bool is_structure_same_type(Structure* struct_type_1, Structure* struct_type_2) {
    return struct_type_1->tag.compare(struct_type_2->tag) == 0;
}

static bool is_same_type(Type* type_1, Type* type_2) {
    if(type_1->type() == type_2->type()) {
        switch(type_1->type()) {
            case AST_T::Pointer_t:
                return is_pointer_same_type(static_cast<Pointer*>(type_1),
                                            static_cast<Pointer*>(type_2));
            case AST_T::Array_t:
                return is_array_same_type(static_cast<Array*>(type_1),
                                          static_cast<Array*>(type_2));
            case AST_T::Structure_t:
                return is_structure_same_type(static_cast<Structure*>(type_1),
                                              static_cast<Structure*>(type_2));
            case AST_T::FunType_t:
                RAISE_INTERNAL_ERROR;
            default:
                return true;
        }
    }
    return false;
}

static bool is_same_fun_type(FunType* fun_type_1, FunType* fun_type_2) {
    if(fun_type_1->param_types.size() != fun_type_2->param_types.size()) {
        return false;
    }
    else if(!is_same_type(fun_type_1->ret_type.get(), fun_type_2->ret_type.get())) {
        return false;
    }
    for(size_t param_type = 0; param_type < fun_type_1->param_types.size(); param_type++) {
        if(!is_same_type(fun_type_1->param_types[param_type].get(),
                         fun_type_2->param_types[param_type].get())) {
            return false;
        }
    }
    return true;
}

static bool is_type_signed(Type* type) {
    switch(type->type()) {
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
    switch(type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_integer(Type* type) {
    switch(type->type()) {
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
    switch(type->type()) {
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
    switch(type->type()) {
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
    switch(type->type()) {
        case AST_T::Void_t:
            return false;
        case AST_T::Structure_t:
            return is_struct_type_complete(static_cast<Structure*>(type));
        default:
            return true;
    }
}

static void is_valid_type(Type* type);

static void is_pointer_valid_type(Pointer* ptr_type) {
    is_valid_type(ptr_type->ref_type.get());
}

static void is_array_valid_type(Array* arr_type) {
    if(!is_type_complete(arr_type->elem_type.get())) {
        raise_runtime_error("Array must be of complete type");
    }
    is_valid_type(arr_type->elem_type.get());
}

static void is_valid_type(Type* type) {
    switch(type->type()) {
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

static bool is_dot_exp_lvalue(CDot* node) {
    return is_exp_lvalue(node->structure.get());
}

static bool is_exp_lvalue(CExp* node) {
    switch(node->type()) {
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
    switch(node->constant->type()) {
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
    switch(type->type()) {
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
    while(arr_type->elem_type->type() == AST_T::Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    return get_type_scale(arr_type->elem_type.get()) * size;
}

static TLong get_structure_aggregate_type_scale(Structure* struct_type) {
    if(frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        raise_runtime_error("Structure type " + em(struct_type->tag) + "was not declared in this scope");
    }
    return frontend->struct_typedef_table[struct_type->tag]->size;
}

static TLong get_type_scale(Type* type) {
    switch(type->type()) {
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
    if(frontend->struct_typedef_table.find(struct_type->tag) == frontend->struct_typedef_table.end()) {
        raise_runtime_error("Structure type " + em(struct_type->tag) + "was not declared in this scope");
    }
    return frontend->struct_typedef_table[struct_type->tag]->alignment;
}

static TInt get_type_alignment(Type* type) {
    switch(type->type()) {
        case AST_T::Array_t:
            return get_array_aggregate_type_alignment(static_cast<Array*>(type));
        case AST_T::Structure_t:
            return get_structure_aggregate_type_alignment(static_cast<Structure*>(type));
        default:
            return get_scalar_type_size(type);
    }
}

static std::shared_ptr<Type> get_joint_type(CExp* node_1, CExp* node_2) {
    if(is_type_character(node_1->exp_type.get())) {
        std::shared_ptr<Type> exp_type = std::move(node_1->exp_type);
        node_1->exp_type = std::make_shared<Int>();
        std::shared_ptr<Type> joint_type = get_joint_type(node_1, node_2);
        node_1->exp_type = std::move(exp_type);
        return joint_type;
    }
    else if(is_type_character(node_2->exp_type.get())) {
        std::shared_ptr<Type> exp_type_2 = std::move(node_2->exp_type);
        node_2->exp_type = std::make_shared<Int>();
        std::shared_ptr<Type> joint_type = get_joint_type(node_1, node_2);
        node_2->exp_type = std::move(exp_type_2);
        return joint_type;
    }
    else if(is_same_type(node_1->exp_type.get(), node_2->exp_type.get())) {
        return node_1->exp_type;
    }
    else if(node_1->exp_type->type() == AST_T::Double_t ||
            node_2->exp_type->type() == AST_T::Double_t) {
        return std::make_shared<Double>();
    }

    TInt type_size_1 = get_scalar_type_size(node_1->exp_type.get());
    TInt type_size_2 = get_scalar_type_size(node_2->exp_type.get());
    if(type_size_1 == type_size_2) {
        if(is_type_signed(node_1->exp_type.get())) {
            return node_2->exp_type;
        }
        else {
            return node_1->exp_type;
        }
    }
    else if(type_size_1 > type_size_2) {
        return node_1->exp_type;
    }
    else {
        return node_2->exp_type;
    }
}

static std::shared_ptr<Type> get_joint_pointer_type(CExp* node_1, CExp* node_2) {
    if(is_same_type(node_1->exp_type.get(), node_2->exp_type.get())) {
        return node_1->exp_type;
    }
    else if(node_1->type() == AST_T::CConstant_t &&
            is_constant_null_pointer(static_cast<CConstant*>(node_1))) {
        return node_2->exp_type;
    }
    else if(node_2->type() == AST_T::CConstant_t &&
            is_constant_null_pointer(static_cast<CConstant*>(node_2))) {
        return node_1->exp_type;
    }
    else if(node_1->exp_type->type() == AST_T::Pointer_t &&
            static_cast<Pointer*>(node_1->exp_type.get())->ref_type->type() == AST_T::Void_t &&
            node_2->exp_type->type() == AST_T::Pointer_t) {
        return node_1->exp_type;
    }
    else if(node_2->exp_type->type() == AST_T::Pointer_t &&
            static_cast<Pointer*>(node_2->exp_type.get())->ref_type->type() == AST_T::Void_t &&
            node_1->exp_type->type() == AST_T::Pointer_t) {
        return node_2->exp_type;
    }
    raise_runtime_error("Maybe-pointer expressions have incompatible types");
}

static void resolve_struct_type(Type* type);

static void checktype_constant_expression(CConstant* node) {
    switch(node->constant->type()) {
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
    if(frontend->symbol_table[node->name]->type_t->type() == AST_T::FunType_t) {
        raise_runtime_error("Function " + em(node->name) + " was used as a variable");
    }
    node->exp_type = frontend->symbol_table[node->name]->type_t;
}

static void checktype_cast_expression(CCast* node) {
    resolve_struct_type(node->target_type.get());
    if(node->target_type->type() != AST_T::Void_t) {
        if(node->exp->exp_type->type() == AST_T::Double_t &&
           node->target_type->type() == AST_T::Pointer_t) {
            raise_runtime_error("Types can not be converted from floating-point number to pointer type");
        }
        else if(node->exp->exp_type->type() == AST_T::Pointer_t &&
                node->target_type->type() == AST_T::Double_t) {
            raise_runtime_error("Types can not be converted from pointer type to floating-point number");
        }
        else if(!is_type_scalar(node->exp->exp_type.get())) {
            raise_runtime_error("Types can not be converted from non-scalar type");
        }
        else if(!is_type_scalar(node->target_type.get())) {
            raise_runtime_error("Types can not be converted to non-scalar and non-void type");
        }
    }
    is_valid_type(node->target_type.get());
    node->exp_type = node->target_type;
}

static std::unique_ptr<CCast> cast_expression(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    std::unique_ptr<CCast> exp = std::make_unique<CCast>(std::move(node), exp_type);
    checktype_cast_expression(exp.get());
    return exp;
}

static std::unique_ptr<CCast> cast_by_assignment(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    if(is_type_arithmetic(node->exp_type.get()) &&
       is_type_arithmetic(exp_type.get())) {
        return cast_expression(std::move(node), exp_type);
    }
    else if(node->type() == AST_T::CConstant_t &&
            exp_type->type() == AST_T::Pointer_t &&
            is_constant_null_pointer(static_cast<CConstant*>(node.get()))) {
        return cast_expression(std::move(node), exp_type);
    }
    else if(exp_type->type() == AST_T::Pointer_t &&
            static_cast<Pointer*>(exp_type.get())->ref_type->type() == AST_T::Void_t &&
            node->exp_type->type() == AST_T::Pointer_t) {
        return cast_expression(std::move(node), exp_type);
    }
    else if(node->exp_type->type() == AST_T::Pointer_t &&
            static_cast<Pointer*>(node->exp_type.get())->ref_type->type() == AST_T::Void_t &&
            exp_type->type() == AST_T::Pointer_t) {
        return cast_expression(std::move(node), exp_type);
    }
    raise_runtime_error("Assignment expressions have incompatible types");
}

static void checktype_unary_not_expression(CUnary* node) {
    if(!is_type_scalar(node->exp->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("unary operator") +
                            " can not be used on " + em("non-scalar type"));
    }

    node->exp_type = std::make_shared<Int>();
}

static void checktype_unary_complement_expression(CUnary* node) {
    if(!is_type_arithmetic(node->exp->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("unary operator") +
                            " can not be used on " + em("non-arithmetic type"));
    }

    switch(node->exp->exp_type->type()) {
        case AST_T::Double_t:
            raise_runtime_error("An error occurred in type checking, " + em("unary operator") +
                                " can not be used on " + em("floating-point number"));
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
    if(!is_type_arithmetic(node->exp->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("unary operator") +
                            " can not be used on " + em("non-arithmetic type"));
    }

    switch(node->exp->exp_type->type()) {
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
    switch(node->unary_op->type()) {
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
    if(is_type_arithmetic(node->exp_left->exp_type.get()) &&
       is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if(node->exp_left->exp_type->type() == AST_T::Pointer_t &&
            is_type_complete(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get()) &&
            is_type_integer(node->exp_right->exp_type.get())) {
        common_type = std::make_shared<Long>();
        if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
            node->exp_right = cast_expression(std::move(node->exp_right), common_type);
        }
        node->exp_type = node->exp_left->exp_type;
        return;
    }
    else if(is_type_integer(node->exp_left->exp_type.get()) &&
            node->exp_right->exp_type->type() == AST_T::Pointer_t &&
            is_type_complete(static_cast<Pointer*>(node->exp_right->exp_type.get())->ref_type.get())) {
        common_type = std::make_shared<Long>();
        if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
            node->exp_left = cast_expression(std::move(node->exp_left), common_type);
        }
        node->exp_type = node->exp_right->exp_type;
        return;
    }
    else {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used with " + em("non-arithmetic") + " and "
                            + em("pointer to incomplete type"));
    }

    if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_binary_arithmetic_subtract_expression(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if(is_type_arithmetic(node->exp_left->exp_type.get()) &&
       is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else if(node->exp_left->exp_type->type() == AST_T::Pointer_t &&
            is_type_complete(static_cast<Pointer*>(node->exp_left->exp_type.get())->ref_type.get())) {
        if(is_type_integer(node->exp_right->exp_type.get())) {
            common_type = std::make_shared<Long>();
            if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
                node->exp_right = cast_expression(std::move(node->exp_right), common_type);
            }
            node->exp_type = node->exp_left->exp_type;
            return;
        }
        else if(is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get()) &&
                !(node->exp_left->type() == AST_T::CConstant_t &&
                  is_constant_null_pointer(static_cast<CConstant*>(node->exp_left.get())))) {
            common_type = std::make_shared<Long>();
            node->exp_type = std::move(common_type);
            return;
        }
    }
    else {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used with " + em("non-integer") + " or "
                            + em("constant null pointer") + " and " + em("pointer to incomplete type"));
    }

    if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_binary_arithmetic_multiply_divide_expression(CBinary* node) {
    if(!is_type_arithmetic(node->exp_left->exp_type.get()) ||
       !is_type_arithmetic(node->exp_right->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("non-arithmetic type"));
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_binary_arithmetic_remainder_bitwise_expression(CBinary* node) {
    if(!is_type_arithmetic(node->exp_left->exp_type.get()) ||
       !is_type_arithmetic(node->exp_right->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("non-arithmetic type"));
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
    if(node->exp_type->type() == AST_T::Double_t) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("floating-point number"));
    }
}

static void checktype_binary_arithmetic_bitshift_expression(CBinary* node) {
    // Note: https://stackoverflow.com/a/70130146
    // if the value of the right operand is negative or is greater than or equal
    // to the width of the promoted left operand, the behavior is undefined
    if(!is_type_arithmetic(node->exp_left->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("non-arithmetic type"));
    }
    else if(!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())){
        node->exp_right = cast_expression(std::move(node->exp_right), node->exp_left->exp_type);
    }
    node->exp_type = node->exp_left->exp_type;
    if(node->exp_type->type() == AST_T::Double_t) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("floating-point number"));
    }
}

static void checktype_binary_logical_expression(CBinary* node) {
    if(!is_type_scalar(node->exp_left->exp_type.get()) ||
       !is_type_scalar(node->exp_right->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("non-scalar type"));
    }

    node->exp_type = std::make_shared<Int>();
}

static void checktype_binary_comparison_equality_expression(CBinary* node) {
    std::shared_ptr<Type> common_type;
    if(node->exp_left->exp_type->type() == AST_T::Pointer_t ||
       node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        common_type = get_joint_pointer_type(node->exp_left.get(), node->exp_right.get());
    }
    else if(is_type_arithmetic(node->exp_left->exp_type.get()) &&
            is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    }
    else {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("non-scalar type"));
    }

    if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::make_shared<Int>();
}

static void checktype_binary_comparison_relational_expression(CBinary* node) {
    if(!is_type_scalar(node->exp_left->exp_type.get()) ||
       !is_type_scalar(node->exp_right->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used on " + em("non-scalar type"));
    }

    else if(node->exp_left->exp_type->type() == AST_T::Pointer_t &&
       (!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get()) ||
        (node->exp_left->type() == AST_T::CConstant_t &&
         is_constant_null_pointer(static_cast<CConstant*>(node->exp_left.get()))) ||
        (node->exp_right->type() == AST_T::CConstant_t &&
         is_constant_null_pointer(static_cast<CConstant*>(node->exp_right.get()))))) {
        raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                            " can not be used with " + em("non-pointer type") + " or "
                            + em("constant null pointer") + " and " + em("pointer type"));
    }

    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
    if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        node->exp_left = cast_expression(std::move(node->exp_left), common_type);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::make_shared<Int>();
}

static void checktype_binary_expression(CBinary* node) {
    switch(node->binary_op->type()) {
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
        case AST_T::CBitShiftRight_t:
            checktype_binary_arithmetic_bitshift_expression(node);
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
    if(node->exp_left) {
        if(node->exp_left->exp_type->type() == AST_T::Void_t) {
            raise_runtime_error("Assignment left expression can not have void type");
        }
        else if(!is_exp_lvalue(node->exp_left.get())) {
            raise_runtime_error("Assignment left expression is an invalid lvalue");
        }
        else if(!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
            node->exp_right = cast_by_assignment(std::move(node->exp_right), node->exp_left->exp_type);
        }
        node->exp_type = node->exp_left->exp_type;
    }
    else {
        if(node->exp_right->type() != AST_T::CBinary_t) {
            raise_runtime_error("Right expression is an invalid compound assignment");
        }
        CExp* exp_left = static_cast<CBinary*>(node->exp_right.get())->exp_left.get();
        if(exp_left->type() == AST_T::CCast_t) {
            exp_left = static_cast<CCast*>(exp_left)->exp.get();
        }
        if(!is_exp_lvalue(exp_left)) {
            raise_runtime_error("Left expression is an invalid lvalue");
        }
        else if(!is_same_type(node->exp_right->exp_type.get(), exp_left->exp_type.get())) {
            node->exp_right = cast_by_assignment(std::move(node->exp_right), exp_left->exp_type);
        }
        node->exp_type = exp_left->exp_type;
    }
}

static void checktype_conditional_expression(CConditional* node) {
    if(!is_type_scalar(node->condition->exp_type.get())) {
        raise_runtime_error("Ternary operator must have a conditional expression of scalar type");
    }
    else if(node->exp_middle->exp_type->type() == AST_T::Void_t &&
            node->exp_right->exp_type->type() == AST_T::Void_t) {
        node->exp_type = node->exp_middle->exp_type;
        return;
    }
    else if(node->exp_middle->exp_type->type() == AST_T::Structure_t ||
            node->exp_right->exp_type->type() == AST_T::Structure_t) {
        if(!is_same_type(node->exp_middle->exp_type.get(), node->exp_right->exp_type.get())) {
            raise_runtime_error("Ternary operator must have matching structure type expressions");
        }
        node->exp_type = node->exp_middle->exp_type;
        return;
    }

    std::shared_ptr<Type> common_type;
    if(is_type_arithmetic(node->exp_middle->exp_type.get()) &&
       is_type_arithmetic(node->exp_right->exp_type.get())) {
        common_type = get_joint_type(node->exp_middle.get(), node->exp_right.get());
    }
    else if(node->exp_middle->exp_type->type() == AST_T::Pointer_t ||
            node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        common_type = get_joint_pointer_type(node->exp_middle.get(), node->exp_right.get());
    }
    else if(node->exp_middle->exp_type->type() == AST_T::Void_t ||
            node->exp_right->exp_type->type() == AST_T::Void_t) {
        raise_runtime_error("Ternary operator must have both void type expressions");
    }
    else {
        raise_runtime_error("Ternary operator must have both scalar type expressions");
    }
    if(!is_same_type(node->exp_middle->exp_type.get(), common_type.get())) {
        node->exp_middle = cast_expression(std::move(node->exp_middle), common_type);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        node->exp_right = cast_expression(std::move(node->exp_right), common_type);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_function_call_expression(CFunctionCall* node) {
    if(frontend->symbol_table[node->name]->type_t->type() != AST_T::FunType_t) {
        raise_runtime_error("Variable " + em(node->name) + " was used as a function");
    }
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    if(fun_type->param_types.size() != node->args.size()) {
        raise_runtime_error("Function " + em(node->name) + " has " +
                            em(std::to_string(fun_type->param_types.size())) +
                            " arguments but was called with " + em(std::to_string(node->args.size())));
    }
    for(size_t i = 0; i < node->args.size(); i ++) {
        if(!is_same_type(node->args[i]->exp_type.get(), fun_type->param_types[i].get())) {
            node->args[i] = cast_by_assignment(std::move(node->args[i]), fun_type->param_types[i]);
        }
    }
    node->exp_type = fun_type->ret_type;
}

static void checktype_dereference_expression(CDereference* node) {
    if(node->exp->exp_type->type() != AST_T::Pointer_t) {
        raise_runtime_error("Non-pointer type can not be de-referenced");
    }
    node->exp_type = static_cast<Pointer*>(node->exp->exp_type.get())->ref_type;
}

static void checktype_addrof_expression(CAddrOf* node) {
    if(!is_exp_lvalue(node->exp.get())) {
        raise_runtime_error("Non-lvalue type can not be addressed");
    }
    std::shared_ptr<Type> ref_type = node->exp->exp_type;
    node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
}

static void checktype_subscript_expression(CSubscript* node) {
    std::shared_ptr<Type> ref_type;
    if(node->primary_exp->exp_type->type() == AST_T::Pointer_t &&
       is_type_complete(static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type.get()) &&
       is_type_integer(node->subscript_exp->exp_type.get())) {
        std::shared_ptr<Type> subscript_type = std::make_shared<Long>();
        if(!is_same_type(node->subscript_exp->exp_type.get(), subscript_type.get())) {
            node->subscript_exp = cast_expression(std::move(node->subscript_exp), subscript_type);
        }
        ref_type = static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type;
    }
    else if(is_type_integer(node->primary_exp->exp_type.get()) &&
            node->subscript_exp->exp_type->type() == AST_T::Pointer_t &&
            is_type_complete(static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type.get())) {
        std::shared_ptr<Type> primary_type = std::make_shared<Long>();
        if(!is_same_type(node->primary_exp->exp_type.get(), primary_type.get())) {
            node->primary_exp = cast_expression(std::move(node->primary_exp), primary_type);
        }
        ref_type = static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type;
    }
    else {
        raise_runtime_error("Subscript must consist of an integer operand and a pointer to complete type "
                            "operand");
    }
    node->exp_type = std::move(ref_type);
}

static void checktype_sizeof_expression(CSizeOf* node) {
    if(!is_type_complete(node->exp->exp_type.get())) {
        raise_runtime_error("Can not get the size of an incomplete type");
    }
    node->exp_type = std::make_shared<ULong>();
}

static void checktype_sizeoft_expression(CSizeOfT* node) {
    resolve_struct_type(node->target_type.get());
    if(!is_type_complete(node->target_type.get())) {
        raise_runtime_error("Can not get the size of an incomplete type");
    }
    is_valid_type(node->target_type.get());
    node->exp_type = std::make_shared<ULong>();
}

static void checktype_dot_expression(CDot* node) {
    if(node->structure->exp_type->type() != AST_T::Structure_t) {
        raise_runtime_error("Can not access member on expression with non-structure type");
    }
    Structure* struct_type = static_cast<Structure*>(node->structure->exp_type.get());
    if(frontend->struct_typedef_table[struct_type->tag]->members.find(node->member) ==
                                                      frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        raise_runtime_error("Structure does not have a member named " + em(node->member));
    }
    node->exp_type = frontend->struct_typedef_table[struct_type->tag]->members[node->member]->member_type;
}

static void checktype_arrow_expression(CArrow* node) {
    if(node->pointer->exp_type->type() != AST_T::Pointer_t) {
        raise_runtime_error("Can not access member on expression with non-pointer to structure type");
    }
    Pointer* ptr_type = static_cast<Pointer*>(node->pointer->exp_type.get());
    if(ptr_type->ref_type->type() != AST_T::Structure_t) {
        raise_runtime_error("Can not access member on expression with non-pointer to structure type");
    }
    Structure* struct_type = static_cast<Structure*>(ptr_type->ref_type.get());
    if(frontend->struct_typedef_table[struct_type->tag]->members.find(node->member) ==
                                                      frontend->struct_typedef_table[struct_type->tag]->members.end()) {
        raise_runtime_error("Structure does not have a member named " + em(node->member));
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
    std::unique_ptr<CAddrOf> addrof = std::make_unique<CAddrOf>(std::move(node));
    addrof->exp_type = addrof->exp->exp_type;
    return addrof;
}

static std::unique_ptr<CExp> checktype_structure_aggregate_typed_expression(std::unique_ptr<CExp>&& node) {
    if(!is_struct_type_complete(static_cast<Structure*>(node->exp_type.get()))) {
        raise_runtime_error("Expression was declared with incomplete structure type");
    }

    std::unique_ptr<CExp> exp = std::move(node);
    return exp;
}

static std::unique_ptr<CExp> checktype_typed_expression(std::unique_ptr<CExp>&& node) {
    switch(node->exp_type->type()) {
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
    if(fun_type->ret_type->type() == AST_T::Void_t) {
        if(node->exp) {
            raise_runtime_error("Void type function can not return a value");
        }
        return;
    }
    else if(!node->exp) {
        raise_runtime_error("Non-void type function must return a value");
    }

    else if(!is_same_type(node->exp->exp_type.get(), fun_type->ret_type.get())) {
        node->exp = cast_by_assignment(std::move(node->exp), fun_type->ret_type);
    }
    node->exp = checktype_typed_expression(std::move(node->exp));
}

static void checktype_if_statement(CIf* node) {
    if(node->condition && 
       !is_type_scalar(node->condition->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("if statement") +
                            " can not be used on " + em("non-scalar type"));
    }
}

static void checktype_while_statement(CWhile* node) {
    if(node->condition &&
       !is_type_scalar(node->condition->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("while statement") +
                            " can not be used on " + em("non-scalar type"));
    }
}

static void checktype_do_while_statement(CDoWhile* node) {
    if(node->condition &&
       !is_type_scalar(node->condition->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("do while statement") +
                            " can not be used on " + em("non-scalar type"));
    }
}

static void checktype_for_statement(CFor* node) {
    if(node->condition &&
       !is_type_scalar(node->condition->exp_type.get())) {
        raise_runtime_error("An error occurred in type checking, " + em("for statement") +
                            " can not be used on " + em("non-scalar type"));
    }
}

static void checktype_bound_array_single_init_string_initializer(CString* node, Array* arr_type) {
    if(!is_type_character(arr_type->elem_type.get())) {
        raise_runtime_error("Array of non-character type was initialized with string literal");
    }
    else if(node->literal->value.size() > static_cast<size_t>(arr_type->size)) {
        raise_runtime_error("String literal of size " +
                            em(std::to_string(node->literal->value.size())) + " was initialized with " +
                            em(std::to_string(arr_type->size)) + " initializers");
    }
}

static void checktype_single_init_initializer(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    if(!is_same_type(node->exp->exp_type.get(), init_type.get())) {
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
        switch(elem_type->type()) {
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
        exp = std::make_unique<CConstant>(std::move(constant));
    }
    return std::make_unique<CSingleInit>(std::move(exp));
}

static std::unique_ptr<CCompoundInit> checktype_array_compound_init_zero_initializer(Array* arr_type) {
    std::vector<std::unique_ptr<CInitializer>> zero_initializers;
    for(size_t throwaway = 0; throwaway < static_cast<size_t>(arr_type->size); throwaway++) {
        std::unique_ptr<CInitializer> initializer = checktype_zero_initializer(arr_type->elem_type.get());
        zero_initializers.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_initializers));
}

static std::unique_ptr<CCompoundInit> checktype_structure_compound_init_zero_initializer(Structure* struct_type) {
    std::vector<std::unique_ptr<CInitializer>> zero_initializers;
    for(const auto& member_name: frontend->struct_typedef_table[struct_type->tag]->member_names) {
        auto& member = frontend->struct_typedef_table[struct_type->tag]->members[member_name];
        std::unique_ptr<CInitializer> initializer = checktype_zero_initializer(member->member_type.get());
        zero_initializers.push_back(std::move(initializer));
    }
    return std::make_unique<CCompoundInit>(std::move(zero_initializers));
}

static std::unique_ptr<CInitializer> checktype_zero_initializer(Type* init_type) {
    switch(init_type->type()) {
        case AST_T::Array_t:
            return checktype_array_compound_init_zero_initializer(static_cast<Array*>(init_type));
        case AST_T::Structure_t:
            return checktype_structure_compound_init_zero_initializer(static_cast<Structure*>(init_type));
        default:
            return checktype_single_init_zero_initializer(init_type);
    }
}

static void checktype_bound_array_compound_init_initializer(CCompoundInit* node, Array* arr_type) {
    if(node->initializers.size() > static_cast<size_t>(arr_type->size)) {
        raise_runtime_error("Array of size " + em(std::to_string(arr_type->size)) +
                            " was initialized with " + em(std::to_string(node->initializers.size())) +
                            " initializers");
    }
}

static void checktype_bound_structure_compound_init_initializer(CCompoundInit* node, Structure* struct_type) {
    if(node->initializers.size() > frontend->struct_typedef_table[struct_type->tag]->members.size()) {
        raise_runtime_error("Structure with " +
                            em(std::to_string(frontend->struct_typedef_table[struct_type->tag]->members.size())) +
                            " members was initialized with " + em(std::to_string(node->initializers.size())) +
                            " initializers");
    }
}

static void checktype_array_compound_init_initializer(CCompoundInit* node, Array* arr_type,
                                                      std::shared_ptr<Type>& init_type) {
    while(node->initializers.size() < static_cast<size_t>(arr_type->size)) {
        std::unique_ptr<CInitializer> zero_initializer = checktype_zero_initializer(arr_type->elem_type.get());
        node->initializers.push_back(std::move(zero_initializer));
    }
    node->init_type = init_type;
}

static void checktype_structure_compound_init_initializer(CCompoundInit* node, Structure* struct_type,
                                                          std::shared_ptr<Type>& init_type) {
    for(size_t initializer = node->initializers.size();
        initializer < frontend->struct_typedef_table[struct_type->tag]->members.size(); initializer++) {
        auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, initializer);
        std::unique_ptr<CInitializer> zero_initializer = checktype_zero_initializer(member->member_type.get());
        node->initializers.push_back(std::move(zero_initializer));

    }
    node->init_type = init_type;
}

static void checktype_return_function_declaration(CFunctionDeclaration* node) {
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    resolve_struct_type(fun_type->ret_type.get());
    is_valid_type(fun_type->ret_type.get());

    switch(fun_type->ret_type->type()) {
        case AST_T::Array_t:
            raise_runtime_error("Function " + em(node->name) + " was declared with array return type");
        case AST_T::Structure_t: {
            if(node->body &&
               !is_struct_type_complete(static_cast<Structure*>(fun_type->ret_type.get()))) {
                raise_runtime_error("Function " + em(node->name) + " was declared with incomplete "
                                    " structure return type");
            }
            break;
        }
        default:
            break;
    }
}

static void checktype_params_function_declaration(CFunctionDeclaration* node) {
    FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
    for(size_t param_type = 0; param_type < node->params.size(); param_type++) {
        resolve_struct_type(fun_type->param_types[param_type].get());
        if(fun_type->param_types[param_type]->type() == AST_T::Void_t) {
            raise_runtime_error("Function parameters can not have void type");
        }
        is_valid_type(fun_type->param_types[param_type].get());
        if(fun_type->param_types[param_type]->type() == AST_T::Array_t) {
            std::shared_ptr<Type> ref_type = static_cast<Array*>(fun_type->param_types[param_type].get())->elem_type;
            fun_type->param_types[param_type] = std::make_shared<Pointer>(std::move(ref_type));
        }

        if(node->body) {
            if(fun_type->param_types[param_type]->type() == AST_T::Structure_t &&
               !is_struct_type_complete(static_cast<Structure*>(fun_type->param_types[param_type].get()))) {
                raise_runtime_error("Function parameter was declared with incomplete structure type");
            }
            std::shared_ptr<Type> type_t = fun_type->param_types[param_type];
            std::unique_ptr<IdentifierAttr> param_attrs = std::make_unique<LocalAttr>();
            frontend->symbol_table[node->params[param_type]] = std::make_unique<Symbol>(std::move(type_t),
                                                                                        std::move(param_attrs));
        }
    }
}

static void checktype_function_declaration(CFunctionDeclaration* node) {
    if(node->fun_type->type() == AST_T::Void_t) {
        raise_runtime_error("Function declaration can not have void type");
    }

    bool is_defined = context->function_definition_set.find(node->name) != context->function_definition_set.end();
    bool is_global = !(node->storage_class && 
                       node->storage_class->type() == AST_T::CStatic_t);

    if(frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {

        FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
        if(!(frontend->symbol_table[node->name]->type_t->type() == AST_T::FunType_t &&
             fun_type->param_types.size() == node->params.size() &&
             is_same_fun_type(static_cast<FunType*>(node->fun_type.get()), fun_type))) {
            raise_runtime_error("Function declaration " + em(node->name) +
                                " was redeclared with conflicting type");
        }

        else if(is_defined &&
           node->body) {
            raise_runtime_error("Function declaration " + em(node->name) +
                                " was already defined");
        }

        FunAttr* fun_attrs = static_cast<FunAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if(!is_global &&
           fun_attrs->is_global) {
            raise_runtime_error("Static function " + em(node->name) +
                                " was already defined non-static");
        }
        is_global = fun_attrs->is_global;
    }

    if(node->body) {
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
    if(!context->p_static_inits->empty() &&
       context->p_static_inits->back()->type() == AST_T::ZeroInit_t) {
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
    switch(static_init_type->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value;
            switch(node->constant->type()) {
                case AST_T::CConstChar_t: {
                    value = static_cast<CConstChar*>(node->constant.get())->value;
                    break;
                }
                case AST_T::CConstInt_t: {
                    value = static_cast<TChar>(static_cast<CConstInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<TChar>(static_cast<CConstLong*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstDouble_t: {
                    value = static_cast<TChar>(static_cast<CConstDouble*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUChar_t: {
                    value =  static_cast<TChar>(static_cast<CConstUChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUInt_t: {
                    value = static_cast<TChar>(static_cast<CConstUInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstULong_t: {
                    value = static_cast<TChar>(static_cast<CConstULong*>(node->constant.get())->value);
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(value == 0) {
                push_zero_init_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<CharInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Int_t: {
            TInt value;
            switch(node->constant->type()) {
                case AST_T::CConstChar_t: {
                    value = static_cast<TInt>(static_cast<CConstChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstInt_t: {
                    value = static_cast<CConstInt*>(node->constant.get())->value;
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<TInt>(static_cast<CConstLong*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstDouble_t: {
                    value = static_cast<TInt>(static_cast<CConstDouble*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUChar_t: {
                    value =  static_cast<TInt>(static_cast<CConstUChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUInt_t: {
                    value = static_cast<TInt>(static_cast<CConstUInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstULong_t: {
                    value = static_cast<TInt>(static_cast<CConstULong*>(node->constant.get())->value);
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(value == 0) {
                push_zero_init_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<IntInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Long_t: {
            TLong value;
            switch(node->constant->type()) {
                case AST_T::CConstChar_t: {
                    value = static_cast<TLong>(static_cast<CConstChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstInt_t: {
                    value = static_cast<TLong>(static_cast<CConstInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<CConstLong*>(node->constant.get())->value;
                    break;
                }
                case AST_T::CConstDouble_t: {
                    value = static_cast<TLong>(static_cast<CConstDouble*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUChar_t: {
                    value =  static_cast<TLong>(static_cast<CConstUChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUInt_t: {
                    value = static_cast<TLong>(static_cast<CConstUInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstULong_t: {
                    value = static_cast<TLong>(static_cast<CConstULong*>(node->constant.get())->value);
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(value == 0l) {
                push_zero_init_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<LongInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Double_t: {
            TDouble value;
            switch(node->constant->type()) {
                case AST_T::CConstChar_t: {
                    value = static_cast<TDouble>(static_cast<CConstChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstInt_t: {
                    value = static_cast<TDouble>(static_cast<CConstInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<TDouble>(static_cast<CConstLong*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstDouble_t: {
                    value = static_cast<CConstDouble*>(node->constant.get())->value;
                    break;
                }
                case AST_T::CConstUChar_t: {
                    value =  static_cast<TDouble>(static_cast<CConstUChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUInt_t: {
                    value = static_cast<TDouble>(static_cast<CConstUInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstULong_t: {
                    value = static_cast<TDouble>(static_cast<CConstULong*>(node->constant.get())->value);
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            TULong binary = double_to_binary(value);
            if(binary == 0ul) {
                push_zero_init_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<DoubleInit>(std::move(value), std::move(binary)));
            }
            break;
        }
        case AST_T::UChar_t: {
            TUChar value;
            switch(node->constant->type()) {
                case AST_T::CConstChar_t: {
                    value = static_cast<TUChar>(static_cast<CConstChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstInt_t: {
                    value = static_cast<TUChar>(static_cast<CConstInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<TUChar>(static_cast<CConstLong*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstDouble_t: {
                    value = static_cast<TUChar>(static_cast<CConstDouble*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUChar_t: {
                    value =  static_cast<CConstUChar*>(node->constant.get())->value;
                    break;
                }
                case AST_T::CConstUInt_t: {
                    value = static_cast<TUChar>(static_cast<CConstUInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstULong_t: {
                    value = static_cast<TUChar>(static_cast<CConstULong*>(node->constant.get())->value);
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(value == 0u) {
                push_zero_init_static_init(1l);
            }
            else {
                push_static_init(std::make_shared<UCharInit>(std::move(value)));
            }
            break;
        }
        case AST_T::UInt_t: {
            TUInt value;
            switch(node->constant->type()) {
                case AST_T::CConstChar_t: {
                    value = static_cast<TUInt>(static_cast<CConstChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstInt_t: {
                    value = static_cast<TUInt>(static_cast<CConstInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<TUInt>(static_cast<CConstLong*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstDouble_t: {
                    value = static_cast<TUInt>(static_cast<CConstDouble*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUChar_t: {
                    value =  static_cast<TUInt>(static_cast<CConstUChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUInt_t: {
                    value = static_cast<CConstUInt*>(node->constant.get())->value;
                    break;
                }
                case AST_T::CConstULong_t: {
                    value = static_cast<TUInt>(static_cast<CConstULong*>(node->constant.get())->value);
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(value == 0u) {
                push_zero_init_static_init(4l);
            }
            else {
                push_static_init(std::make_shared<UIntInit>(std::move(value)));
            }
            break;
        }
        case AST_T::ULong_t: {
            TULong value;
            switch(node->constant->type()) {
                case AST_T::CConstChar_t: {
                    value = static_cast<TULong>(static_cast<CConstChar*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstInt_t: {
                    value = static_cast<TULong>(static_cast<CConstInt*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstLong_t: {
                    value = static_cast<TULong>(static_cast<CConstLong*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstDouble_t: {
                    value = static_cast<TULong>(static_cast<CConstDouble*>(node->constant.get())->value);
                    break;
                }
                case AST_T::CConstUChar_t: {
                    value =  static_cast<TULong>(static_cast<CConstUChar*>(node->constant.get())->value);
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
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(value == 0ul) {
                push_zero_init_static_init(8l);
            }
            else {
                push_static_init(std::make_shared<ULongInit>(std::move(value)));
            }
            break;
        }
        case AST_T::Pointer_t: {
            TULong value;
            switch(node->constant->type()) {
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
                    raise_runtime_error("Static pointer type can only be initialized to integer constant");
                default:
                    RAISE_INTERNAL_ERROR;
            }
            if(value != 0ul) {
                raise_runtime_error("Static pointer type can only be initialized to null integer constant");
            }
            push_zero_init_static_init(8l);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void checktype_string_initializer_pointer_static_init(CString* node, Pointer* static_ptr_type) {
    if(static_ptr_type->ref_type->type() != AST_T::Char_t) {
        raise_runtime_error("Pointer of non-character type was initialized with string literal");
    }

    TIdentifier static_constant_label;
    {
        TIdentifier string_constant = string_literal_to_string_constant(node->literal->value);
        TIdentifier static_constant_hash = std::to_string(std::hash<std::string>{}(string_constant));
        if(frontend->static_constant_table.find(static_constant_hash) != frontend->static_constant_table.end()) {
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
                    static_init = std::make_shared<StringInit>(true, std::move(string_constant),
                                                               std::move(literal));
                }
                constant_attrs = std::make_unique<ConstantAttr>(std::move(static_init));
            }
            frontend->symbol_table[static_constant_label] = std::make_unique<Symbol>(std::move(constant_type),
                                                                                     std::move(constant_attrs));
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
        push_static_init(std::make_shared<StringInit>(std::move(is_null_terminated), std::move(string_constant),
                                                               std::move(literal)));
    }
    if(byte > 0l) {
        push_zero_init_static_init(std::move(byte));
    }
}

static void checktype_string_initializer_static_init(CString* node, Type* static_init_type) {
    switch(static_init_type->type()) {
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
    switch(node->exp->type()) {
        case AST_T::CConstant_t:
            checktype_constant_initializer_static_init(static_cast<CConstant*>(node->exp.get()), static_init_type);
            break;
        case AST_T::CString_t:
            checktype_string_initializer_static_init(static_cast<CString*>(node->exp.get()), static_init_type);
            break;
        default:
            raise_runtime_error("Variable with static linkage was initialized to a non-constant");
    }
}

static void checktype_array_compound_init_initializer_static_init(CCompoundInit* node, Array* arr_type) {
    checktype_bound_array_compound_init_initializer(node, arr_type);

    for(size_t initializer = 0; initializer < node->initializers.size(); initializer++) {
        checktype_initializer_static_init(node->initializers[initializer].get(),
                                          arr_type->elem_type.get());
    }
    if(static_cast<size_t>(arr_type->size) > node->initializers.size()) {
        checktype_no_initializer_static_init(arr_type->elem_type.get(),
                                             arr_type->size - node->initializers.size());
    }
}

static void checktype_structure_compound_init_initializer_static_init(CCompoundInit* node, Structure* struct_type) {
    checktype_bound_structure_compound_init_initializer(node, struct_type);

    TLong size = 0l;
    for(size_t initializer = 0; initializer < node->initializers.size(); initializer++) {
        auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, initializer);
        if(member->offset != size) {
            checktype_no_initializer_static_init(nullptr, member->offset - size);
            size = member->offset;
        }
        checktype_initializer_static_init(node->initializers[initializer].get(),
                                          member->member_type.get());
        size += get_type_scale(member->member_type.get());
    }
    if(frontend->struct_typedef_table[struct_type->tag]->size != size) {
        checktype_no_initializer_static_init(nullptr,
                                             frontend->struct_typedef_table[struct_type->tag]->size - size);
    }
}

static void checktype_compound_init_initializer_static_init(CCompoundInit* node, Type* static_init_type) {
    switch(static_init_type->type()) {
        case AST_T::Array_t:
            checktype_array_compound_init_initializer_static_init(node, static_cast<Array*>(static_init_type));
            break;
        case AST_T::Structure_t:
            checktype_structure_compound_init_initializer_static_init(node,
                                                                    static_cast<Structure*>(static_init_type));
            break;
        default:
            raise_runtime_error("Compound initializer can not be initialized with scalar type");
    }
}

static void checktype_initializer_static_init(CInitializer* node, Type* static_init_type) {
    switch(node->type()) {
        case AST_T::CSingleInit_t:
            checktype_single_init_initializer_static_init(static_cast<CSingleInit*>(node), static_init_type);
            break;
        case AST_T::CCompoundInit_t:
            checktype_compound_init_initializer_static_init(static_cast<CCompoundInit*>(node),
                                                            static_init_type);
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
    resolve_struct_type(node->var_type.get());
    if(node->var_type->type() == AST_T::Void_t) {
        raise_runtime_error("Variable declaration can not have void type");
    }
    is_valid_type(node->var_type.get());

    std::shared_ptr<InitialValue> initial_value;
    bool is_global = !(node->storage_class && 
                       node->storage_class->type() == AST_T::CStatic_t);

    if(node->init) {
        if(node->var_type->type() == AST_T::Structure_t &&
           !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
            raise_runtime_error("Non-extern file scope variable " + em(node->name) + " was declared "
                                " with incomplete structure type");
        }
        initial_value = checktype_initializer_initial(node->init.get(), node->var_type.get());
    }
    else {
        if(node->storage_class && 
           node->storage_class->type() == AST_T::CExtern_t) {
            initial_value = std::make_shared<NoInitializer>();
        }
        else {
            if(node->var_type->type() == AST_T::Structure_t &&
               !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
                raise_runtime_error("Non-extern file scope variable " + em(node->name) + " was declared "
                                    " with incomplete structure type");
            }
            initial_value = std::make_shared<Tentative>();
        }
    }

    if(frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if(!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            raise_runtime_error("File scope variable " + em(node->name) +
                                " was redeclared with conflicting type");
        }

        StaticAttr* global_var_attrs = static_cast<StaticAttr*>(frontend->symbol_table[node->name]->attrs.get());
        if(node->storage_class &&
           node->storage_class->type() == AST_T::CExtern_t) {
            is_global = global_var_attrs->is_global;
        }
        else if(is_global != global_var_attrs->is_global) {
            raise_runtime_error("File scope variable " + em(node->name) +
                                " was redeclared with conflicting linkage");
        }

        if(global_var_attrs->init->type() == AST_T::Initial_t) {
            if(initial_value->type() == AST_T::Initial_t) {
                raise_runtime_error("File scope variable " + em(node->name) +
                                    " was defined with conflicting linkage");
            }
            else {
                initial_value = global_var_attrs->init;
            }
        }
    }

    std::shared_ptr<Type> global_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> global_var_attrs = std::make_unique<StaticAttr>(std::move(is_global),
                                                                                    std::move(initial_value));
    frontend->symbol_table[node->name] = std::make_unique<Symbol>(std::move(global_var_type),
                                                                  std::move(global_var_attrs));
}

static void checktype_extern_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(node->init) {
        raise_runtime_error("Block scope variable " + em(node->name) +
                            " with external linkage was defined");
    }
    else if(frontend->symbol_table.find(node->name) != frontend->symbol_table.end()) {
        if(!is_same_type(frontend->symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            raise_runtime_error("Block scope variable " + em(node->name) +
                                " was redeclared with conflicting type");
        }
        return;
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs;
    {
        std::shared_ptr<InitialValue> initial_value = std::make_shared<NoInitializer>();
        local_var_attrs = std::make_unique<StaticAttr>(true, std::move(initial_value));
    }
    frontend->symbol_table[node->name] = std::make_unique<Symbol>(std::move(local_var_type),
                                                                  std::move(local_var_attrs));
}

static void checktype_static_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(node->var_type->type() == AST_T::Structure_t &&
       !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
        raise_runtime_error("Non-extern file scope variable " + em(node->name) + " was declared "
                            " with incomplete structure type");
    }

    std::shared_ptr<InitialValue> initial_value;
    if(node->init) {
        initial_value = checktype_initializer_initial(node->init.get(), node->var_type.get());
    }
    else {
        initial_value = checktype_no_initializer_initial(node->var_type.get());
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<StaticAttr>(false,
                                                                                   std::move(initial_value));
    frontend->symbol_table[node->name] = std::make_unique<Symbol>(std::move(local_var_type),
                                                                  std::move(local_var_attrs));
}

static void checktype_automatic_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(node->var_type->type() == AST_T::Structure_t &&
       !is_struct_type_complete(static_cast<Structure*>(node->var_type.get()))) {
        raise_runtime_error("Non-extern file scope variable " + em(node->name) + " was declared "
                            " with incomplete structure type");
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<LocalAttr>();
    frontend->symbol_table[node->name] = std::make_unique<Symbol>(std::move(local_var_type),
                                                                  std::move(local_var_attrs));
}

static void checktype_block_scope_variable_declaration(CVariableDeclaration* node) {
    resolve_struct_type(node->var_type.get());
    if(node->var_type->type() == AST_T::Void_t) {
        raise_runtime_error("Variable declaration can not have void type");
    }
    is_valid_type(node->var_type.get());

    if(node->storage_class) {
        switch(node->storage_class->type()) {
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
    for(size_t member = 0; member < node->members.size(); member++) {
        for(size_t next_member = member + 1; next_member < node->members.size(); next_member++) {
            if(node->members[member]->member_name.compare(node->members[next_member]->member_name) == 0) {
                raise_runtime_error("Structure member was already declared in this scope");
            }
        }
        if(node->members[member].get()->member_type->type() == AST_T::FunType_t) {
            RAISE_INTERNAL_ERROR;
        }
        resolve_struct_type(node->members[member].get()->member_type.get());
        if(!is_type_complete(node->members[member].get()->member_type.get())) {
            raise_runtime_error("Structure member must be declared with a complete type");
        }
        is_valid_type(node->members[member].get()->member_type.get());
    }
}

static void checktype_structure_declaration(CStructDeclaration* node) {
    if(frontend->struct_typedef_table.find(node->tag) != frontend->struct_typedef_table.end()) {
        raise_runtime_error("Structure type " + em(node->tag) + " was already declared in this scope");
    }
    TInt alignment = 0;
    TLong size = 0l;
    std::vector<TIdentifier> member_names;
    std::unordered_map<TIdentifier, std::unique_ptr<StructMember>> members;
    for(size_t member = 0; member < node->members.size(); member++) {
        {
            TIdentifier name = node->members[member]->member_name;
            member_names.push_back(std::move(name));
        }
        TInt member_alignment = get_type_alignment(node->members[member]->member_type.get());
        {
            TLong offset = size % member_alignment;
            offset = offset == 0l ? size : size + member_alignment - offset;
            std::shared_ptr<Type> member_type = node->members[member]->member_type;
            members[member_names.back()] = std::make_unique<StructMember>(std::move(offset), std::move(member_type));
        }
        if(alignment < member_alignment) {
            alignment = member_alignment;
        }
        size += get_type_scale(node->members[member]->member_type.get());
    }
    {
        TLong offset = size % alignment;
        if(offset != 0l) {
            size += alignment - offset;
        }
    }
    frontend->struct_typedef_table[node->tag] = std::make_unique<StructTypedef>(std::move(alignment), std::move(size),
                                                                                std::move(member_names),
                                                                                std::move(members));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loop labeling

static void annotate_while_loop(CWhile* node) {
    node->target = represent_label_identifier(LABEL_KIND::Lwhile);
    context->loop_labels.push_back(node->target);
}

static void annotate_do_while_loop(CDoWhile* node) {
    node->target = represent_label_identifier(LABEL_KIND::Ldo_while);
    context->loop_labels.push_back(node->target);
}

static void annotate_for_loop(CFor* node) {
    node->target = represent_label_identifier(LABEL_KIND::Lfor);
    context->loop_labels.push_back(node->target);
}

static void annotate_break_loop(CBreak* node) {
    if(context->loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("break") +
                            "is outside of loop");
    }
    node->target = context->loop_labels.back();
}

static void annotate_continue_loop(CContinue* node) {
    if(context->loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("continue") +
                            "is outside of loop");
    }
    node->target = context->loop_labels.back();
}

static void deannotate_loop() {
    context->loop_labels.pop_back();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifier resolution

static size_t current_scope_depth() {
    return context->scoped_identifier_maps.size();
}

static bool is_file_scope() {
    return current_scope_depth() == 1;
}

static void enter_scope() {
    context->scoped_identifier_maps.emplace_back();
    context->scoped_structure_tag_maps.emplace_back();
}

static void exit_scope() {
    for(const auto& identifier: context->scoped_identifier_maps.back()) {
        if(context->external_linkage_scope_map.find(identifier.first) != context->external_linkage_scope_map.end() &&
           context->external_linkage_scope_map[identifier.first] == current_scope_depth()) {
            context->external_linkage_scope_map.erase(identifier.first);
        }
    }
    context->scoped_identifier_maps.pop_back();
    context->scoped_structure_tag_maps.pop_back();
}

static void resolve_label() {
    for(const auto& target: context->goto_map) {
        if(context->label_set.find(target.first) == context->label_set.end()) {
            raise_runtime_error("An error occurred in variable resolution, goto " + em(target.first) +
                                " has no target label");
        }
    }
}

static void resolve_pointer_struct_type(Pointer* ptr_type) {
    resolve_struct_type(ptr_type->ref_type.get());
}

static void resolve_array_struct_type(Array* arr_type) {
    resolve_struct_type(arr_type->elem_type.get());
}

static void resolve_structure_struct_type(Structure* struct_type) {
    if(context->structure_definition_set.find(struct_type->tag) != context->structure_definition_set.end()) {
        return;
    }
    for(size_t i = current_scope_depth(); i-- > 0;) {
        if(context->scoped_structure_tag_maps[i].find(struct_type->tag) !=
                                                                          context->scoped_structure_tag_maps[i].end()) {
            struct_type->tag = context->scoped_structure_tag_maps[i][struct_type->tag];
            return;
        }
    }
    raise_runtime_error("Structure type " + em(struct_type->tag) + " was not declared in this scope");
}

static void resolve_struct_type(Type* type) {
    switch(type->type()) {
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

static void resolve_constant_expression(CConstant* node) {
    checktype_constant_expression(node);
}

static void resolve_string_expression(CString* node) {
    checktype_string_expression(node);
}

static void resolve_var_expression(CVar* node) {
    for(size_t i = current_scope_depth(); i-- > 0;) {
        if(context->scoped_identifier_maps[i].find(node->name) != context->scoped_identifier_maps[i].end()) {
            node->name = context->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    raise_runtime_error("Variable " + em(node->name) + " was not declared in this scope");
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
    if(node->exp_left) {
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
    for(size_t i = current_scope_depth(); i-- > 0;) {
        if(context->scoped_identifier_maps[i].find(node->name) != context->scoped_identifier_maps[i].end()) {
            node->name = context->scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    raise_runtime_error("Function " + em(node->name) + " was not declared in this scope");
    Lelse:

    for(size_t i = 0; i < node->args.size(); i++) {
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

static void resolve_sizeoft_expression(CSizeOfT* node) {
    checktype_sizeoft_expression(node);
}

static void resolve_dot_expression(CDot* node) {
    node->structure = resolve_typed_expression(std::move(node->structure));
    checktype_dot_expression(node);
}

static void resolve_arrow_expression(CArrow* node) {
    node->pointer = resolve_typed_expression(std::move(node->pointer));
    checktype_arrow_expression(node);
}

static void resolve_expression(CExp* node) {
    switch(node->type()) {
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
    if(node->init->storage_class) {
        raise_runtime_error("Variable " + em(node->init->name) +
                            " was not declared with automatic linkage in for loop initializer");
    }
    resolve_block_scope_variable_declaration(node->init.get());
}

static void resolve_init_exp_for_init(CInitExp* node) {
    if(node->init) {
        node->init = resolve_typed_expression(std::move(node->init));
    }
}

static void resolve_for_init(CForInit* node) {
    switch(node->type()) {
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
    if(node->exp) {
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
    if(node->else_fi) {
        resolve_statement(node->else_fi.get());
    }
    checktype_if_statement(node);
}

static void resolve_goto_statement(CGoto* node) {
    if(context->goto_map.find(node->target) != context->goto_map.end()) {
        node->target = context->goto_map[node->target];
    }
    else {
        context->goto_map[node->target] = resolve_label_identifier(node->target);
        node->target = context->goto_map[node->target];
    }
}

static void resolve_label_statement(CLabel* node) {
    if(context->label_set.find(node->target) != context->label_set.end()) {
        raise_runtime_error("Label " + em(node->target) + " was already declared in this scope");
    }
    context->label_set.insert(node->target);

    if(context->goto_map.find(node->target) != context->goto_map.end()) {
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
    if(node->condition) {
        node->condition = resolve_typed_expression(std::move(node->condition));
    }
    if(node->post) {
        node->post = resolve_typed_expression(std::move(node->post));
    }
    resolve_statement(node->body.get());
    exit_scope();
    deannotate_loop();
    checktype_for_statement(node);
}

static void resolve_break_statement(CBreak* node) {
    annotate_break_loop(node);
}

static void resolve_continue_statement(CContinue* node) {
    annotate_continue_loop(node);
}

static void resolve_statement(CStatement* node) {
    switch(node->type()) {
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

static void resolve_block_items(std::vector<std::unique_ptr<CBlockItem>>& list_node) {
    for(size_t block_item = 0; block_item < list_node.size(); block_item++) {
        switch(list_node[block_item]->type()) {
            case AST_T::CS_t:
                resolve_statement(static_cast<CS*>(list_node[block_item].get())->statement.get());
                break;
            case AST_T::CD_t:
                resolve_declaration(static_cast<CD*>(list_node[block_item].get())->declaration.get());
                break;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
}

static void resolve_block(CBlock* node) {
    switch(node->type()) {
        case AST_T::CB_t:
            resolve_block_items(static_cast<CB*>(node)->block_items);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_initializer(CInitializer* node, std::shared_ptr<Type>& init_type);

static void resolve_single_init_initializer(CSingleInit* node, std::shared_ptr<Type>& init_type) {
    if(node->exp->type() == AST_T::CString_t &&
       init_type->type() == AST_T::Array_t) {
        checktype_bound_array_single_init_string_initializer(static_cast<CString*>(node->exp.get()),
                                                             static_cast<Array*>(init_type.get()));
        checktype_array_single_init_string_initializer(node, init_type);
    }
    else {
        node->exp = resolve_typed_expression(std::move(node->exp));
        checktype_single_init_initializer(node, init_type);
    }
}

static void resolve_array_compound_init_initializer(CCompoundInit* node, Array* arr_type,
                                                    std::shared_ptr<Type>& init_type) {
    checktype_bound_array_compound_init_initializer(node, arr_type);

    for(size_t initializer = 0; initializer < node->initializers.size(); initializer++) {
        resolve_initializer(node->initializers[initializer].get(), arr_type->elem_type);
    }
    checktype_array_compound_init_initializer(node, arr_type, init_type);
}

static void resolve_structure_compound_init_initializer(CCompoundInit* node, Structure* struct_type,
                                                        std::shared_ptr<Type>& init_type) {
    checktype_bound_structure_compound_init_initializer(node, struct_type);

    for(size_t initializer = 0; initializer < node->initializers.size(); initializer++) {
        auto& member = GET_STRUCT_TYPEDEF_MEMBER(struct_type->tag, initializer);
        resolve_initializer(node->initializers[initializer].get(), member->member_type);
    }
    checktype_structure_compound_init_initializer(node, struct_type, init_type);
}

static void resolve_compound_init_initializer(CCompoundInit* node, std::shared_ptr<Type>& init_type) {
    switch(init_type->type()) {
        case AST_T::Array_t:
            resolve_array_compound_init_initializer(node, static_cast<Array*>(init_type.get()), init_type);
            break;
        case AST_T::Structure_t:
            resolve_structure_compound_init_initializer(node, static_cast<Structure*>(init_type.get()),
                                                        init_type);
            break;
        default:
            raise_runtime_error("Compound initializer can not be initialized with scalar type");
    }
}

static void resolve_initializer(CInitializer* node, std::shared_ptr<Type>& init_type) {
    switch(node->type()) {
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
    for(size_t param = 0; param < node->params.size(); param++) {
        if(context->scoped_identifier_maps.back().find(node->params[param]) !=
                                                                         context->scoped_identifier_maps.back().end()) {
            raise_runtime_error("Variable " + node->params[param] + " was already declared in this scope");
        }
        context->scoped_identifier_maps.back()[node->params[param]] =
                                                                resolve_variable_identifier(node->params[param]);
        node->params[param] = context->scoped_identifier_maps.back()[node->params[param]];
    }
    checktype_params_function_declaration(node);
}

static void resolve_function_declaration(CFunctionDeclaration* node) {
    if(!is_file_scope()) {
        if(node->body) {
            raise_runtime_error("Block scoped function definition " + em(node->name) +
                                " can not be nested");
        }
        else if(node->storage_class &&
           node->storage_class->type() == AST_T::CStatic_t) {
            raise_runtime_error("Block scoped function definition " + em(node->name) +
                                " can not be static");
        }
    }

    if(context->external_linkage_scope_map.find(node->name) == context->external_linkage_scope_map.end()) {
        if(context->scoped_identifier_maps.back().find(node->name) != context->scoped_identifier_maps.back().end()) {
            raise_runtime_error("Function " + em(node->name) + " was already declared in this scope");
        }
        context->external_linkage_scope_map[node->name] = current_scope_depth();
    }

    context->scoped_identifier_maps.back()[node->name] = node->name;
    checktype_return_function_declaration(node);

    enter_scope();
    if(!node->params.empty()) {
        resolve_params_function_declaration(node);
    }
    checktype_function_declaration(node);

    if(node->body) {
        resolve_block(node->body.get());
    }
    exit_scope();
}

static void resolve_file_scope_variable_declaration(CVariableDeclaration* node) {
    if(context->external_linkage_scope_map.find(node->name) == context->external_linkage_scope_map.end()) {
        context->external_linkage_scope_map[node->name] = current_scope_depth();
    }

    context->scoped_identifier_maps.back()[node->name] = node->name;
    if(is_file_scope()) {
        checktype_file_scope_variable_declaration(node);
    }
    else {
        checktype_block_scope_variable_declaration(node);
    }
}

static void resolve_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(context->scoped_identifier_maps.back().find(node->name) != context->scoped_identifier_maps.back().end() &&
       !(context->external_linkage_scope_map.find(node->name) != context->external_linkage_scope_map.end() &&
         (node->storage_class && 
          node->storage_class->type() == AST_T::CExtern_t))) {
       raise_runtime_error("Variable " + em(node->name) + " was already declared in this scope");
    }
    else if(node->storage_class &&
       node->storage_class->type() == AST_T::CExtern_t) {
        resolve_file_scope_variable_declaration(node);
        return;
    }

    context->scoped_identifier_maps.back()[node->name] = resolve_variable_identifier(node->name);
    node->name = context->scoped_identifier_maps.back()[node->name];
    checktype_block_scope_variable_declaration(node);

    if(node->init &&
       !node->storage_class) {
        resolve_initializer(node->init.get(), node->var_type);
    }
}

static void resolve_members_structure_declaration(CStructDeclaration* node) {
    checktype_members_structure_declaration(node);
}

static void resolve_structure_declaration(CStructDeclaration* node) {
    if(context->scoped_structure_tag_maps.back().find(node->tag) != context->scoped_structure_tag_maps.back().end()) {
        node->tag = context->scoped_structure_tag_maps.back()[node->tag];
    }
    else {
        context->scoped_structure_tag_maps.back()[node->tag] = resolve_structure_tag(node->tag);
        node->tag = context->scoped_structure_tag_maps.back()[node->tag];
        context->structure_definition_set.insert(node->tag);
    }
    if(!node->members.empty()) {
        resolve_members_structure_declaration(node);
        checktype_structure_declaration(node);
    }
}

static void resolve_fun_decl_declaration(CFunDecl* node) {
    if(is_file_scope()) {
        context->goto_map.clear();
        context->label_set.clear();
        context->loop_labels.clear();
    }
    resolve_function_declaration(node->function_decl.get());
    if(is_file_scope()) {
        resolve_label();
    }
}

static void resolve_var_decl_declaration(CVarDecl* node) {
    if(is_file_scope()) {
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
    switch(node->type()) {
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
    for(size_t declaration = 0; declaration < node->declarations.size(); declaration++) {
        resolve_declaration(node->declarations[declaration].get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void analyze_semantic(CProgram* node) {
    context = std::make_unique<SemanticContext>();
    resolve_identifiers(node);
    context.reset();
}
