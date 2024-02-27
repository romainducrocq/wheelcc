#include "frontend/intermediate/semantic.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/front_ast.hpp"
#include "frontend/intermediate/names.hpp"

#include <inttypes.h>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Type checking

static std::unordered_set<TIdentifier> defined_set;

static TIdentifier function_declaration_name;

static bool is_same_type(Type* type_1, Type* type_2) {
    while(type_1->type() == AST_T::Pointer_t &&
          type_2->type() == AST_T::Pointer_t) {
        type_1 = static_cast<Pointer*>(type_1)->ref_type.get();
        type_2 = static_cast<Pointer*>(type_2)->ref_type.get();
    }
    return type_1->type() == type_2->type();
}

static bool is_same_fun_type(FunType* fun_type_1, FunType* fun_type_2) {
    if(fun_type_1->param_types.size() != fun_type_2->param_types.size()) {
        return false;
    }
    if(!is_same_type(fun_type_1->ret_type.get(), fun_type_2->ret_type.get())) {
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

static bool is_type_signed(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::Double_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_integer(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::UInt_t:
        case AST_T::ULong_t:
            return true;
        default:
            return false;
    }
}

static bool is_type_arithmetic(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::UInt_t:
        case AST_T::ULong_t:
            return true;
        default:
            return false;
    }
}

static int32_t get_type_size(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return 32;
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::ULong_t:
        case AST_T::Pointer_t:
            return 64;
        default:
            RAISE_INTERNAL_ERROR;
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

static bool is_exp_lvalue(CExp* node) {
    switch(node->type()) {
        case AST_T::CVar_t:
        case AST_T::CDereference_t:
        case AST_T::CSubscript_t:
            return true;
        default:
            return false;
    }
}

static std::shared_ptr<Type> get_joint_type(CExp* node_1, CExp* node_2) {
    if(is_same_type(node_1->exp_type.get(), node_2->exp_type.get())) {
        return node_1->exp_type;
    }
    else if(node_1->exp_type->type() == AST_T::Double_t ||
            node_2->exp_type->type() == AST_T::Double_t) {
        return std::make_shared<Double>();
    }
    int32_t type1_size = get_type_size(node_1->exp_type.get());
    int32_t type2_size = get_type_size(node_2->exp_type.get());
    if(type1_size == type2_size) {
        if(is_type_signed(node_1->exp_type.get())) {
            return node_2->exp_type;
        } else {
            return node_1->exp_type;
        }
    }
    if(type1_size > type2_size) {
        return node_1->exp_type;
    } else {
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
    raise_runtime_error("Maybe-pointer expressions have incompatible types");
}

static void checktype_constant_expression(CConstant* node) {
    switch(node->constant->type()) {
        case AST_T::CConstInt_t:
            node->exp_type = std::make_shared<Int>();
            break;
        case AST_T::CConstLong_t:
            node->exp_type = std::make_shared<Long>();
            break;
        case AST_T::CConstDouble_t:
            node->exp_type = std::make_shared<Double>();
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

static void checktype_var_expression(CVar* node) {
    if(symbol_table[node->name]->type_t->type() == AST_T::FunType_t) {
        raise_runtime_error("Function " + em(node->name) + " was used as a variable");
    }
    node->exp_type = symbol_table[node->name]->type_t;
}

static void checktype_cast_expression(CCast* node) {
    node->exp_type = node->target_type;
    if((node->exp_type->type() == AST_T::Double_t && node->exp->exp_type->type() == AST_T::Pointer_t) ||
       (node->exp_type->type() == AST_T::Pointer_t && node->exp->exp_type->type() == AST_T::Double_t)) {
        raise_runtime_error("Types can not be converted between floating-point number and pointer type");
    }
    if(node->exp_type->type() == AST_T::Array_t) {
        raise_runtime_error("Type can not be converted to array type");
    }
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
    raise_runtime_error("Assignment expressions have incompatible types");
}

static void checktype_unary_expression(CUnary* node) {
    switch(node->unary_op->type()) {
        case AST_T::CNot_t: {
            node->exp_type = std::make_shared<Int>();
            break;
        }
        case AST_T::CComplement_t: {
            switch(node->exp->exp_type->type()) {
                case AST_T::Double_t:
                case AST_T::Pointer_t:
                    raise_runtime_error("An error occurred in type checking, " + em("unary operator") +
                                        " can not be used on " + em("floating-point number") + " or " +
                                        em("pointer type"));
                default:
                    break;
            }
            node->exp_type = node->exp->exp_type;
            break;
        }
        case AST_T::CNegate_t: {
            if(node->exp->exp_type->type() == AST_T::Pointer_t) {
                raise_runtime_error("An error occurred in type checking, " + em("unary operator") +
                                    " can not be used on " + em("pointer type"));
            }
            node->exp_type = node->exp->exp_type;
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void checktype_binary_expression(CBinary* node) {
    switch(node->binary_op->type()) {
        case AST_T::CAnd_t:
        case AST_T::COr_t:
            node->exp_type = std::make_shared<Int>();
            return;
        case AST_T::CMultiply_t:
        case AST_T::CDivide_t:
        case AST_T::CRemainder_t:
        case AST_T::CBitAnd_t:
        case AST_T::CBitOr_t:
        case AST_T::CBitXor_t: {
            if(node->exp_left->exp_type->type() == AST_T::Pointer_t ||
               node->exp_right->exp_type->type() == AST_T::Pointer_t) {
                raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                                    " can not be used on " + em("pointer type"));
            }
            break;
        }
        case AST_T::CBitShiftLeft_t:
        case AST_T::CBitShiftRight_t: {
            // Note: https://stackoverflow.com/a/70130146
            // if the value of the right operand is negative or is greater than or equal
            // to the width of the promoted left operand, the behavior is undefined
            if(!is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())){
                std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_right),
                                                            node->exp_left->exp_type);
                node->exp_right = std::move(exp);
            }
            node->exp_type = node->exp_left->exp_type;
            switch(node->exp_type->type()) {
                case AST_T::Double_t:
                case AST_T::Pointer_t:
                    raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                                        " can not be used on " + em("floating-point number") + " or " +
                                        em("pointer type"));
                default:
                    break;
            }
            return;
        }
        default:
            break;
    }
    std::shared_ptr<Type> common_type;
    switch(node->binary_op->type()) {
        case AST_T::CAdd_t:
        case AST_T::CSubtract_t: {
            if(is_type_arithmetic(node->exp_left->exp_type.get()) &&
               is_type_arithmetic(node->exp_right->exp_type.get())) {
                common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
            }
            else if(node->exp_left->exp_type->type() == AST_T::Pointer_t &&
                    is_type_integer(node->exp_right->exp_type.get())) {
                common_type = std::make_shared<Long>();
                if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
                    std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_right), common_type);
                    node->exp_right = std::move(exp);
                }
                node->exp_type = std::move(common_type);
                return;
            }
            else if(node->binary_op->type() == AST_T::CSubtract_t &&
                    node->exp_left->exp_type->type() == AST_T::Pointer_t &&
                    is_same_type(node->exp_left->exp_type.get(), node->exp_right->exp_type.get())) {
                common_type = std::make_shared<Long>();
                node->exp_type = std::move(common_type);
                return;
            }
            else {
                raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                                    " can not be used with " + em("floating-point number") + " and "
                                    + em("pointer type"));
            }
            break;
        }
        case AST_T::CEqual_t:
        case AST_T::CNotEqual_t: {
            if(node->exp_left->exp_type->type() == AST_T::Pointer_t ||
               node->exp_right->exp_type->type() == AST_T::Pointer_t) {
                common_type = get_joint_pointer_type(node->exp_left.get(), node->exp_right.get());
            }
            else {
                common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
            }
            break;
        }
        default: {
            common_type = get_joint_type(node->exp_left.get(), node->exp_right.get());
            break;
        }
    }
    if(!is_same_type(node->exp_left->exp_type.get(), common_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_left), common_type);
        node->exp_left = std::move(exp);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_right), common_type);
        node->exp_right = std::move(exp);
    }
    switch(node->binary_op->type()) {
        case AST_T::CAdd_t:
        case AST_T::CSubtract_t:
        case AST_T::CMultiply_t:
        case AST_T::CDivide_t: {
            node->exp_type = std::move(common_type);
            return;
        }
        case AST_T::CRemainder_t:
        case AST_T::CBitAnd_t:
        case AST_T::CBitOr_t:
        case AST_T::CBitXor_t: {
            node->exp_type = std::move(common_type);
            if(node->exp_type->type() == AST_T::Double_t) {
                raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                                    " can not be used on " + em("floating-point number"));
            }
            return;
        }
        case AST_T::CEqual_t:
        case AST_T::CNotEqual_t:
        case AST_T::CLessThan_t:
        case AST_T::CLessOrEqual_t:
        case AST_T::CGreaterThan_t:
        case AST_T::CGreaterOrEqual_t: {
            node->exp_type = std::make_shared<Int>();
            return;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void checktype_assignment_expression(CAssignment* node) {
    if(node->exp_left) {
        if(!is_exp_lvalue(node->exp_left.get())) {
            raise_runtime_error("Left expression is an invalid lvalue");
        }
        if(!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
            std::unique_ptr<CExp> exp = cast_by_assignment(std::move(node->exp_right), node->exp_left->exp_type);
            node->exp_right = std::move(exp);
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
        if(!is_same_type(node->exp_right->exp_type.get(), exp_left->exp_type.get())) {
            std::unique_ptr<CExp> exp = cast_by_assignment(std::move(node->exp_right), exp_left->exp_type);
            node->exp_right = std::move(exp);
        }
        node->exp_type = exp_left->exp_type;
    }
}

static void checktype_conditional_expression(CConditional* node) {
    std::shared_ptr<Type> common_type;
    if(node->exp_middle->exp_type->type() == AST_T::Pointer_t ||
       node->exp_right->exp_type->type() == AST_T::Pointer_t) {
        common_type = get_joint_pointer_type(node->exp_middle.get(), node->exp_right.get());
    }
    else {
        common_type = get_joint_type(node->exp_middle.get(), node->exp_right.get());
    }
    if(!is_same_type(node->exp_middle->exp_type.get(), common_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_middle), common_type);
        node->exp_middle = std::move(exp);
    }
    if(!is_same_type(node->exp_right->exp_type.get(), common_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_right), common_type);
        node->exp_right = std::move(exp);
    }
    node->exp_type = std::move(common_type);
}

static void checktype_function_call_expression(CFunctionCall* node) {
    if(symbol_table[node->name]->type_t->type() != AST_T::FunType_t) {
        raise_runtime_error("Variable " + em(node->name) + " was used as a function");
    }
    FunType* fun_type = static_cast<FunType*>(symbol_table[node->name]->type_t.get());
    if(fun_type->param_types.size() != node->args.size()) {
        raise_runtime_error("Function " + em(node->name) + " has " +
                            em(std::to_string(fun_type->param_types.size())) +
                            " arguments but was called with " + em(std::to_string(node->args.size())));
    }
    for(size_t i = 0; i < node->args.size(); i ++) {
        if(!is_same_type(node->args[i]->exp_type.get(), fun_type->param_types[i].get())) {
            std::unique_ptr<CExp> exp = cast_by_assignment(std::move(node->args[i]), fun_type->param_types[i]);
            node->args[i] = std::move(exp);
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
       is_type_integer(node->subscript_exp->exp_type.get())) {
        std::shared_ptr<Type> subscript_type = std::make_shared<Long>();
        if(!is_same_type(node->subscript_exp->exp_type.get(), subscript_type.get())) {
            std::unique_ptr<CExp> exp = cast_expression(std::move(node->subscript_exp), subscript_type);
            node->subscript_exp = std::move(exp);
        }
        ref_type = static_cast<Pointer*>(node->primary_exp->exp_type.get())->ref_type;
    }
    else if(is_type_integer(node->primary_exp->exp_type.get()) &&
            node->subscript_exp->exp_type->type() == AST_T::Pointer_t) {
        std::shared_ptr<Type> primary_type = std::make_shared<Long>();
        if(!is_same_type(node->primary_exp->exp_type.get(), primary_type.get())) {
            std::unique_ptr<CExp> exp = cast_expression(std::move(node->primary_exp), primary_type);
            node->primary_exp = std::move(exp);
        }
        ref_type = static_cast<Pointer*>(node->subscript_exp->exp_type.get())->ref_type;
    }
    else {
        raise_runtime_error("Subscript must consist of an integer operand and a pointer type operand");
    }
    node->exp_type = std::move(ref_type);
}

static std::unique_ptr<CExp> checktype_array_typed_expression(std::unique_ptr<CExp>&& node) {
    std::shared_ptr<Type> ref_type = static_cast<Array*>(node->exp_type.get())->elem_type;
    std::unique_ptr<CExp> exp = std::make_unique<CAddrOf>(std::move(node));
    exp->exp_type = std::make_shared<Pointer>(std::move(ref_type));
    return exp;
}

static std::unique_ptr<CExp> checktype_pass_typed_expression(std::unique_ptr<CExp>&& node) {
    std::unique_ptr<CExp> exp = std::move(node);
    return exp;
}

static std::unique_ptr<CExp> checktype_typed_expression(std::unique_ptr<CExp>&& node) {
    switch(node->exp_type->type()) {
        case Array_t:
            return checktype_array_typed_expression(std::move(node));
        default:
            return checktype_pass_typed_expression(std::move(node));
    }
}

static void checktype_return_statement(CReturn* node) {
    FunType* fun_type = static_cast<FunType*>(symbol_table[function_declaration_name]->type_t.get());
    if(!is_same_type(node->exp->exp_type.get(), fun_type->ret_type.get())) {
        std::unique_ptr<CExp> exp = cast_by_assignment(std::move(node->exp), fun_type->ret_type);
        node->exp = std::move(exp);
    }
    node->exp = checktype_typed_expression(std::move(node->exp));
}

static std::unique_ptr<Symbol> checktype_param(FunType* fun_type, size_t param) {
    std::shared_ptr<Type> type_t = fun_type->param_types[param];
    std::unique_ptr<IdentifierAttr> param_attrs = std::make_unique<LocalAttr>();
    return std::make_unique<Symbol>(std::move(type_t), std::move(param_attrs));
}

static void checktype_params(CFunctionDeclaration* node) {
    if(node->body) {
        FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
        for(size_t param = 0; param < node->params.size(); param++) {
            std::unique_ptr<Symbol> symbol = checktype_param(fun_type, param);
            symbol_table[node->params[param]] = std::move(symbol);
        }
    }
}

static void checktype_function_declaration(CFunctionDeclaration* node) {
    bool is_defined = defined_set.find(node->name) != defined_set.end();
    bool is_global = !(node->storage_class && 
                       node->storage_class->type() == AST_T::CStatic_t);

    if(symbol_table.find(node->name) != symbol_table.end()) {

        FunType* fun_type = static_cast<FunType*>(symbol_table[node->name]->type_t.get());
        if(!(symbol_table[node->name]->type_t->type() == AST_T::FunType_t &&
            fun_type->param_types.size() == node->params.size() &&
            is_same_fun_type(fun_type, static_cast<FunType*>(node->fun_type.get())))) {
            raise_runtime_error("Function declaration " + em(node->name) +
                                "was redeclared with conflicting type");
        }

        if(is_defined &&
           node->body) {
            raise_runtime_error("Function declaration " + em(node->name) +
                                "was already defined");
        }

        FunAttr* fun_attrs = static_cast<FunAttr*>(symbol_table[node->name]->attrs.get());
        if(!is_global &&
           fun_attrs->is_global) {
            raise_runtime_error("Static function " + em(node->name) +
                                "was already defined non-static");
        }
        is_global = fun_attrs->is_global;
    }

    if(node->body) {
        defined_set.insert(node->name);
        is_defined = true;
    }

    std::shared_ptr<Type> fun_type = node->fun_type;
    std::unique_ptr<IdentifierAttr> fun_attrs = std::make_unique<FunAttr>(std::move(is_defined), std::move(is_global));
    std::unique_ptr<Symbol> symbol = std::make_unique<Symbol>(std::move(fun_type), std::move(fun_attrs));
    symbol_table[node->name] = std::move(symbol);

    function_declaration_name = node->name;
}

static std::unique_ptr<Initial> checktype_constant_initial(CConstant* node, Type* static_init_type) {
    std::shared_ptr<StaticInit> static_init;
    switch(static_init_type->type()) {
        case AST_T::Int_t: {
            TInt value;
            switch(node->constant->type()) {
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
            static_init = std::make_shared<IntInit>(std::move(value));
            break;
        }
        case AST_T::Long_t: {
            TLong value;
            switch(node->constant->type()) {
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
            static_init = std::make_shared<LongInit>(std::move(value));
            break;
        }
        case AST_T::Double_t: {
            TDouble value;
            switch(node->constant->type()) {
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
            static_init = std::make_shared<DoubleInit>(std::move(value));
            break;
        }
        case AST_T::UInt_t: {
            TUInt value;
            switch(node->constant->type()) {
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
            static_init = std::make_shared<UIntInit>(std::move(value));
            break;
        }
        case AST_T::ULong_t: {
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
                case AST_T::CConstDouble_t: {
                    value = static_cast<TULong>(static_cast<CConstDouble*>(node->constant.get())->value);
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
            static_init = std::make_shared<ULongInit>(std::move(value));
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
                case AST_T::CConstDouble_t: {
                    raise_runtime_error("Static pointer type can only be initialized to integer constant");
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
            if(value != 0ul) {
                raise_runtime_error("Static pointer type can only be initialized to null integer constant");
            }
            static_init = std::make_shared<ULongInit>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    // TODO
    // return std::make_unique<Initial>(std::move(static_init));
    return nullptr;
}

static std::unique_ptr<Initial> checktype_no_init_initial(Type* static_init_type) {
    std::shared_ptr<StaticInit> static_init;
    switch(static_init_type->type()) {
        case AST_T::Int_t: {
            static_init = std::make_shared<IntInit>(0);
            break;
        }
        case AST_T::Long_t: {
            static_init = std::make_shared<LongInit>(0l);
            break;
        }
        case AST_T::Double_t: {
            static_init = std::make_shared<DoubleInit>(0.0);
            break;
        }
        case AST_T::UInt_t: {
            static_init = std::make_shared<UIntInit>(0u);
            break;
        }
        case AST_T::ULong_t:
        case AST_T::Pointer_t: {
            static_init = std::make_shared<ULongInit>(0ul);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    // TODO
    // return std::make_unique<Initial>(std::move(static_init));
    return nullptr;
}

static void checktype_file_scope_variable_declaration(CVariableDeclaration* node) {
    std::shared_ptr<InitialValue> initial_value;
    bool is_global = !(node->storage_class && 
                       node->storage_class->type() == AST_T::CStatic_t);

    if(node->init &&
       node->init->type() == AST_T::CConstant_t) {
// TODO
//        initial_value = checktype_constant_initial(static_cast<CConstant*>(node->init.get()),
//                                                   node->var_type.get());
;
    }
    else if(!node->init) {
        if(node->storage_class && 
           node->storage_class->type() == AST_T::CExtern_t) {
            initial_value = std::make_shared<NoInitializer>();
        } else {
            initial_value = std::make_shared<Tentative>();
        }
    }
    else {
        raise_runtime_error("File scope variable " + em(node->name) +
                            " was initialized to a non-constant");
    }

    if(symbol_table.find(node->name) != symbol_table.end()) {
        if(!is_same_type(symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            raise_runtime_error("File scope variable " + em(node->name) +
                                " was redeclared with conflicting type");
        }

        StaticAttr* global_var_attrs = static_cast<StaticAttr*>(symbol_table[node->name]->attrs.get());
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
            } else {
                initial_value = global_var_attrs->init;
            }
        }
    }

    std::shared_ptr<Type> global_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> global_var_attrs = std::make_unique<StaticAttr>(std::move(is_global),
                                                                                    std::move(initial_value));
    std::unique_ptr<Symbol> symbol = std::make_unique<Symbol>(std::move(global_var_type),
                                                              std::move(global_var_attrs));
    symbol_table[node->name] = std::move(symbol);
}

static void checktype_extern_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(node->init) {
        raise_runtime_error("Block scope variable " + em(node->name) +
                            " with external linkage was defined");
    }
    if(symbol_table.find(node->name) != symbol_table.end()) {
        if(!is_same_type(symbol_table[node->name]->type_t.get(), node->var_type.get())) {
            raise_runtime_error("Block scope variable " + em(node->name) +
                                " was redeclared with conflicting type");
        }
        return;
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::shared_ptr<InitialValue> initial_value = std::make_shared<NoInitializer>();
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<StaticAttr>(true,
                                                                                   std::move(initial_value));
    std::unique_ptr<Symbol> symbol = std::make_unique<Symbol>(std::move(local_var_type),
                                                              std::move(local_var_attrs));
    symbol_table[node->name] = std::move(symbol);
}

static void checktype_static_block_scope_variable_declaration(CVariableDeclaration* node) {
    std::shared_ptr<InitialValue> initial_value;

    if(node->init &&
       node->init->type() == AST_T::CConstant_t) {
// TODO
//        initial_value = checktype_constant_initial(static_cast<CConstant*>(node->init.get()),
//                                                   node->var_type.get());
;
    }
    else if(!node->init) {
        initial_value = checktype_no_init_initial(node->var_type.get());
    }
    else {
        raise_runtime_error("Block scope variable " + em(node->name) +
                            " with static linkage was initialized to a non-constant");
    }

    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<StaticAttr>(false,
                                                                                   std::move(initial_value));
    std::unique_ptr<Symbol> symbol = std::make_unique<Symbol>(std::move(local_var_type),
                                                              std::move(local_var_attrs));
    symbol_table[node->name] = std::move(symbol);
}

static void checktype_automatic_block_scope_variable_declaration(CVariableDeclaration* node) {
    std::shared_ptr<Type> local_var_type = node->var_type;
    std::unique_ptr<IdentifierAttr> local_var_attrs = std::make_unique<LocalAttr>();
    std::unique_ptr<Symbol> symbol = std::make_unique<Symbol>(std::move(local_var_type),
                                                              std::move(local_var_attrs));
    symbol_table[node->name] = std::move(symbol);
}

static void checktype_init_block_scope_variable_declaration(CVariableDeclaration* node) {
// TODO
//    if(node->init &&
//       !node->storage_class &&
//       !is_same_type(node->var_type.get(), node->init->exp_type.get())) {
//        std::unique_ptr<CExp> exp = cast_by_assignment(std::move(node->init), node->var_type);
//        node->init = std::move(exp);
//    }
}

static void checktype_block_scope_variable_declaration(CVariableDeclaration* node) {
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loop labeling

static std::vector<TIdentifier> loop_labels;

static void annotate_while_loop(CWhile* node) {
    node->target = represent_label_identifier("while");
    loop_labels.push_back(node->target);
}

static void annotate_do_while_loop(CDoWhile* node) {
    node->target = represent_label_identifier("do_while");
    loop_labels.push_back(node->target);
}

static void annotate_for_loop(CFor* node) {
    node->target = represent_label_identifier("for");
    loop_labels.push_back(node->target);
}

static void annotate_break_loop(CBreak* node) {
    if(loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("break") +
                            "is outside of loop");
    }
    node->target = loop_labels.back();
}

static void annotate_continue_loop(CContinue* node) {
    if(loop_labels.empty()) {
        raise_runtime_error("An error occurred in loop annotation, " + em("continue") +
                            "is outside of loop");
    }
    node->target = loop_labels.back();
}

static void deannotate_loop() {
    loop_labels.pop_back();
}

static void clear_annotate_loops() {
    loop_labels.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Identifier resolution

static std::unordered_map<TIdentifier, size_t> external_linkage_scope_map;

static std::vector<std::unordered_map<TIdentifier, TIdentifier>> scoped_identifier_maps;

static std::unordered_map<TIdentifier, TIdentifier> goto_map;

static std::unordered_set<TIdentifier> label_set;

static size_t current_scope_depth() {
    return scoped_identifier_maps.size();
}

static bool is_file_scope() {
    return current_scope_depth() == 1;
}

static void enter_scope() {
    scoped_identifier_maps.emplace_back();
}

static void exit_scope() {
    for(const auto& identifier: scoped_identifier_maps.back()) {
        if(external_linkage_scope_map.find(identifier.first) != external_linkage_scope_map.end() &&
           external_linkage_scope_map[identifier.first] == current_scope_depth()) {
            external_linkage_scope_map.erase(identifier.first);
        }
    }
    scoped_identifier_maps.pop_back();
}

static void resolve_label() {
    for(const auto& target: goto_map) {
        if(label_set.find(target.first) == label_set.end()) {
            raise_runtime_error("An error occurred in variable resolution, goto " + em(target.first) +
                                " has no target label");
        }
    }
}

static std::unique_ptr<CExp> resolve_expression(std::unique_ptr<CExp>&& node);

static void resolve_var_expression(CVar* node) {
    for(size_t i = current_scope_depth(); i-- > 0;) {
        if(scoped_identifier_maps[i].find(node->name) != scoped_identifier_maps[i].end()) {
            node->name = scoped_identifier_maps[i][node->name];
            return;
        }
    }
    raise_runtime_error("Variable " + em(node->name) + " was not declared in this scope");
}

static void resolve_cast_expression(CCast* node) {
    node->exp = resolve_expression(std::move(node->exp));
}

static void resolve_unary_expression(CUnary* node) {
    node->exp = resolve_expression(std::move(node->exp));
}

static void resolve_binary_expression(CBinary* node) {
    node->exp_left = resolve_expression(std::move(node->exp_left));
    node->exp_right = resolve_expression(std::move(node->exp_right));
}

static void resolve_assignment_expression(CAssignment* node) {
    if(node->exp_left) {
        node->exp_left = resolve_expression(std::move(node->exp_left));
    }
    node->exp_right = resolve_expression(std::move(node->exp_right));
}

static void resolve_conditional_expression(CConditional* node) {
    node->condition = resolve_expression(std::move(node->condition));
    node->exp_middle = resolve_expression(std::move(node->exp_middle));
    node->exp_right = resolve_expression(std::move(node->exp_right));
}

static void resolve_function_call_expression(CFunctionCall* node) {
    for(size_t i = current_scope_depth(); i-- > 0;) {
        if(scoped_identifier_maps[i].find(node->name) != scoped_identifier_maps[i].end()) {
            node->name = scoped_identifier_maps[i][node->name];
            goto Lelse;
        }
    }
    raise_runtime_error("Function " + em(node->name) + " was not declared in this scope");
    Lelse:

    for(size_t i = 0; i < node->args.size(); i++) {
        node->args[i] = resolve_expression(std::move(node->args[i]));
    }
}

static void resolve_dereference_expression(CDereference* node) {
    node->exp = resolve_expression(std::move(node->exp));
}

static void resolve_addrof_expression(CAddrOf* node) {
    node->exp = resolve_expression(std::move(node->exp));
}

static void resolve_subscript_expression(CSubscript* node) {
    node->primary_exp = resolve_expression(std::move(node->primary_exp));
    node->subscript_exp = resolve_expression(std::move(node->subscript_exp));
}

static std::unique_ptr<CExp> resolve_expression(std::unique_ptr<CExp>&& node) {
    switch(node->type()) {
        case AST_T::CConstant_t:
            checktype_constant_expression(static_cast<CConstant*>(node.get()));
            break;
        case AST_T::CVar_t: {
            CVar* p_node = static_cast<CVar*>(node.get());
            resolve_var_expression(p_node);
            checktype_var_expression(p_node);
            break;
        }
        case AST_T::CCast_t: {
            CCast* p_node = static_cast<CCast*>(node.get());
            resolve_cast_expression(p_node);
            checktype_cast_expression(p_node);
            break;
        }
        case AST_T::CUnary_t: {
            CUnary* p_node = static_cast<CUnary*>(node.get());
            resolve_unary_expression(p_node);
            checktype_unary_expression(p_node);
            break;
        }
        case AST_T::CBinary_t: {
            CBinary* p_node = static_cast<CBinary*>(node.get());
            resolve_binary_expression(p_node);
            checktype_binary_expression(p_node);
            break;
        }
        case AST_T::CAssignment_t: {
            CAssignment* p_node = static_cast<CAssignment*>(node.get());
            resolve_assignment_expression(p_node);
            checktype_assignment_expression(p_node);
            break;
        }
        case AST_T::CConditional_t: {
            CConditional* p_node = static_cast<CConditional*>(node.get());
            resolve_conditional_expression(p_node);
            checktype_conditional_expression(p_node);
            break;
        }
        case AST_T::CFunctionCall_t: {
            CFunctionCall* p_node = static_cast<CFunctionCall*>(node.get());
            resolve_function_call_expression(p_node);
            checktype_function_call_expression(p_node);
            break;
        }
        case AST_T::CDereference_t: {
            CDereference* p_node = static_cast<CDereference*>(node.get());
            resolve_dereference_expression(p_node);
            checktype_dereference_expression(p_node);
            break;
        }
        case AST_T::CAddrOf_t: {
            CAddrOf* p_node = static_cast<CAddrOf*>(node.get());
            resolve_addrof_expression(p_node);
            checktype_addrof_expression(p_node);
            return checktype_pass_typed_expression(std::move(node));
        }
        case AST_T::CSubscript_t: {
            CSubscript* p_node = static_cast<CSubscript*>(node.get());
            resolve_subscript_expression(p_node);
            checktype_subscript_expression(p_node);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return checktype_typed_expression(std::move(node));
}

static void resolve_block(CBlock* node);
static void resolve_block_scope_variable_declaration(CVariableDeclaration* node);

static void resolve_statement(CStatement* node);

static void resolve_for_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(node->storage_class) {
        raise_runtime_error("Variable " + em(node->name) +
                            " was not declared with automatic linkage in for loop initializer");
    }
    resolve_block_scope_variable_declaration(node);
}

static void resolve_for_init(CForInit* node) {
    switch(node->type()) {
        case AST_T::CInitDecl_t:
            resolve_for_block_scope_variable_declaration(static_cast<CInitDecl*>(node)->init.get());
            break;
        case AST_T::CInitExp_t: {
            CInitExp* init_decl = static_cast<CInitExp*>(node);
            if(init_decl->init) {
                init_decl->init = resolve_expression(std::move(init_decl->init));
            }
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_return_statement(CReturn* node) {
    node->exp = resolve_expression(std::move(node->exp));
}

static void resolve_expression_statement(CExpression* node) {
    node->exp = resolve_expression(std::move(node->exp));
}

static void resolve_if_statement(CIf* node) {
    node->condition = resolve_expression(std::move(node->condition));
    resolve_statement(node->then.get());
    if(node->else_fi) {
        resolve_statement(node->else_fi.get());
    }
}

static void resolve_goto_statement(CGoto* node) {
    if(goto_map.find(node->target) != goto_map.end()) {
        node->target = goto_map[node->target];
    }
    else {
        goto_map[node->target] = resolve_label_identifier(node->target);
        node->target = goto_map[node->target];
    }
}

static void resolve_label_statement(CLabel* node) {
    if(label_set.find(node->target) != label_set.end()) {
        raise_runtime_error("Label " + em(node->target) + " was already declared in this scope");
    }
    label_set.insert(node->target);

    if(goto_map.find(node->target) != goto_map.end()) {
        node->target = goto_map[node->target];
    }
    else {
        goto_map[node->target] = resolve_label_identifier(node->target);
        node->target = goto_map[node->target];
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
    node->condition = resolve_expression(std::move(node->condition));
    resolve_statement(node->body.get());
    deannotate_loop();
}

static void resolve_do_while_statement(CDoWhile* node) {
    annotate_do_while_loop(node);
    resolve_statement(node->body.get());
    node->condition = resolve_expression(std::move(node->condition));
    deannotate_loop();
}

static void resolve_for_statement(CFor* node) {
    annotate_for_loop(node);
    enter_scope();
    resolve_for_init(node->init.get());
    if(node->condition) {
        node->condition = resolve_expression(std::move(node->condition));
    }
    if(node->post) {
        node->post = resolve_expression(std::move(node->post));
    }
    resolve_statement(node->body.get());
    exit_scope();
    deannotate_loop();
}

static void resolve_break_statement(CBreak* node) {
    annotate_break_loop(node);
}

static void resolve_continue_statement(CContinue* node) {
    annotate_continue_loop(node);
}

static void resolve_statement(CStatement* node) {
    switch(node->type()) {
        case AST_T::CReturn_t: {
            CReturn* p_node = static_cast<CReturn*>(node);
            resolve_return_statement(p_node);
            checktype_return_statement(p_node);
            break;
        }
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

static void resolve_params(CFunctionDeclaration* node) {
    for(size_t param = 0; param < node->params.size(); param++) {
        if(scoped_identifier_maps.back().find(node->params[param]) != scoped_identifier_maps.back().end()) {
            raise_runtime_error("Variable " + node->params[param] + " was already declared in this scope");
        }
        scoped_identifier_maps.back()[node->params[param]] = resolve_variable_identifier(node->params[param]);
        node->params[param] = scoped_identifier_maps.back()[node->params[param]];
    }

    if(node->body) {
        checktype_params(node);
    }
}

static void resolve_function_declaration(CFunctionDeclaration* node) {
    if(!is_file_scope()) {
        if(node->body) {
            raise_runtime_error("Block scoped function definition " + em(node->name) +
                                " can not be nested");
        }
        if(node->storage_class && 
           node->storage_class->type() == AST_T::CStatic_t) {
            raise_runtime_error("Block scoped function definition " + em(node->name) +
                                " can not be static");
        }
    }

    if(external_linkage_scope_map.find(node->name) == external_linkage_scope_map.end()) {
        if(scoped_identifier_maps.back().find(node->name) != scoped_identifier_maps.back().end()) {
            raise_runtime_error("Function " + em(node->name) + " was already declared in this scope");
        }
        external_linkage_scope_map[node->name] = current_scope_depth();
    }

    scoped_identifier_maps.back()[node->name] = node->name;
    checktype_function_declaration(node);

    enter_scope();
    if(!node->params.empty()) {
        resolve_params(node);
    }
    if(node->body) {
        resolve_block(node->body.get());
    }
    exit_scope();
}

static void resolve_file_scope_variable_declaration(CVariableDeclaration* node) {
    if(external_linkage_scope_map.find(node->name) == external_linkage_scope_map.end()) {
        external_linkage_scope_map[node->name] = current_scope_depth();
    }

    scoped_identifier_maps.back()[node->name] = node->name;
    if(is_file_scope()) {
        checktype_file_scope_variable_declaration(node);
    }
    else {
        checktype_block_scope_variable_declaration(node);
    }
}

static void resolve_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(scoped_identifier_maps.back().find(node->name) != scoped_identifier_maps.back().end() &&
       !(external_linkage_scope_map.find(node->name) != external_linkage_scope_map.end() &&
         (node->storage_class && 
          node->storage_class->type() == AST_T::CExtern_t))) {
       raise_runtime_error("Variable " + em(node->name) + " was already declared in this scope");
    }
    if(node->storage_class && 
       node->storage_class->type() == AST_T::CExtern_t) {
        resolve_file_scope_variable_declaration(node);
        return;
    }

    scoped_identifier_maps.back()[node->name] = resolve_variable_identifier(node->name);
    node->name = scoped_identifier_maps.back()[node->name];
    checktype_block_scope_variable_declaration(node);

    if(node->init &&
       !node->storage_class) {
// TODO
//        resolve_expression(node->init.get());
    }
    checktype_init_block_scope_variable_declaration(node);
}

static void clear_resolve_labels() {
    goto_map.clear();
    label_set.clear();
}

static void resolve_fun_decl_declaration(CFunDecl* node) {
    if(is_file_scope()) {
        clear_resolve_labels();
        clear_annotate_loops();
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

static void resolve_declaration(CDeclaration* node) {
    switch(node->type()) {
        case AST_T::CFunDecl_t:
            resolve_fun_decl_declaration(static_cast<CFunDecl*>(node));
            break;
        case AST_T::CVarDecl_t:
            resolve_var_decl_declaration(static_cast<CVarDecl*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void resolve_identifiers(CProgram* node) {
    enter_scope();

    for(size_t declaration = 0; declaration < node->declarations.size(); declaration++) {
        resolve_declaration(node->declarations[declaration].get());
        resolve_label();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

void analyze_semantic(CProgram* node) {
    resolve_identifiers(node);
}
