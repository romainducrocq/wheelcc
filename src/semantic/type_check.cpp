#include "semantic/type_check.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"

#include <inttypes.h>
#include <string>
#include <memory>
#include <unordered_set>

static std::unordered_set<TIdentifier> defined_set;

static TIdentifier function_declaration_name;

bool is_same_type(Type* type_1, Type* type_2) {
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

bool is_type_signed(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Int_t:
        case AST_T::Long_t:
            return true;
        default:
            return false;
    }
}

bool is_const_signed(CConst* node) {
    switch(node->type()) {
        case AST_T::CConstInt_t:
        case AST_T::CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_exp_lvalue(CExp* node) {
    switch(node->exp_type->type()) {
        case AST_T::CVar_t:
        case AST_T::CDereference_t:
            return true;
        default:
            return false;
    }
}

static bool is_const_null_pointer(CConstant* node) {
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

int32_t get_type_size(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return 32;
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::ULong_t:
            return 64;
        default:
            return -1;
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
            is_const_null_pointer(static_cast<CConstant*>(node_1))) {
        return node_2->exp_type;
    }
    else if(node_2->type() == AST_T::CConstant_t &&
            is_const_null_pointer(static_cast<CConstant*>(node_2))) {
        return node_1->exp_type;
    }
    raise_runtime_error("Maybe-pointer expressions have incompatible types");
}

void checktype_cast_expression(CCast* node) {
    node->exp_type = node->target_type;
}

static std::unique_ptr<CCast> cast_expression(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    std::unique_ptr<CCast> exp = std::make_unique<CCast>(std::move(node), exp_type);
    checktype_cast_expression(exp.get());
    return exp;
}

void checktype_function_call_expression(CFunctionCall* node) {
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
            std::unique_ptr<CExp> exp = cast_expression(std::move(node->args[i]), fun_type->param_types[i]);
            node->args[i] = std::move(exp);
        }
    }
    node->exp_type = fun_type->ret_type;
}

void checktype_var_expression(CVar* node) {
    if(symbol_table[node->name]->type_t->type() == AST_T::FunType_t) {
        raise_runtime_error("Function " + em(node->name) + " was used as a variable");
    }
    node->exp_type = symbol_table[node->name]->type_t;
}

void checktype_constant_expression(CConstant* node) {
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

void checktype_assignment_expression(CAssignment* node) {
    if(!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_right), node->exp_left->exp_type);
        node->exp_right = std::move(exp);
    }
    node->exp_type = node->exp_left->exp_type;
}

void checktype_unary_expression(CUnary* node) {
    if(node->unary_op->type() == AST_T::CNot_t) {
        node->exp_type = std::make_shared<Int>();
        return;
    }
    if(node->unary_op->type() == AST_T::CComplement_t &&
       node->exp->exp_type->type() == AST_T::Double_t) {
        raise_runtime_error("An error occurred in type checking, " + em("unary operator") +
                            " can not be used on " + em("floating-point number"));
    }
    node->exp_type = node->exp->exp_type;
}

void checktype_binary_expression(CBinary* node) {
    switch(node->binary_op->type()) {
        case AST_T::CAnd_t:
        case AST_T::COr_t:
            node->exp_type = std::make_shared<Int>();
            return;
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
            if(node->exp_type->type() == AST_T::Double_t) {
                raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                                    " can not be used on " + em("floating-point number"));
            }
            return;
        }
        default:
            break;
    }
    std::shared_ptr<Type> common_type;
    switch(node->binary_op->type()) {
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
        case AST_T::CDivide_t:
            node->exp_type = std::move(common_type);
            return;
        case AST_T::CRemainder_t:
        case AST_T::CBitAnd_t:
        case AST_T::CBitOr_t:
        case AST_T::CBitXor_t:
            node->exp_type = std::move(common_type);
            if(node->exp_type->type() == AST_T::Double_t) {
                raise_runtime_error("An error occurred in type checking, " + em("binary operator") +
                                    " can not be used on " + em("floating-point number"));
            }
            return;
        default:
            node->exp_type = std::make_shared<Int>();
            return;
    }
}

void checktype_conditional_expression(CConditional* node) {
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

void checktype_dereference_expression(CDereference* node) {
    if(node->exp->exp_type->type() != AST_T::Pointer_t) {
        raise_runtime_error("Non-pointer type can not be de-referenced");
    }
    node->exp_type = static_cast<Pointer*>(node->exp->exp_type.get())->ref_type;
}

void checktype_addrof_expression(CAddrOf* node) {
    if(!is_exp_lvalue(node->exp.get())) {
        raise_runtime_error("Non-lvalue type can not be addressed");
    }
    std::shared_ptr<Type> ref_type = node->exp->exp_type;
    node->exp_type = std::make_shared<Pointer>(std::move(ref_type));
}

void checktype_return_statement(CReturn* node) {
    FunType* fun_type = static_cast<FunType*>(symbol_table[function_declaration_name]->type_t.get());
    if(!is_same_type(node->exp->exp_type.get(), fun_type->ret_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp), fun_type->ret_type);
        node->exp = std::move(exp);
    }
}

static std::unique_ptr<Symbol> checktype_param(FunType* fun_type, size_t param) {
    std::shared_ptr<Type> type_t = fun_type->param_types[param];
    std::unique_ptr<IdentifierAttr> param_attrs = std::make_unique<LocalAttr>();
    return std::make_unique<Symbol>(std::move(type_t), std::move(param_attrs));
}

void checktype_params(CFunctionDeclaration* node) {
    if(node->body) {
        FunType* fun_type = static_cast<FunType*>(node->fun_type.get());
        for(size_t param = 0; param < node->params.size(); param++) {
            std::unique_ptr<Symbol> symbol = checktype_param(fun_type, param);
            symbol_table[node->params[param]] = std::move(symbol);
        }
    }
}

void checktype_function_declaration(CFunctionDeclaration* node) {
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
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_unique<Initial>(std::move(static_init));
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
        case AST_T::ULong_t: {
            static_init = std::make_shared<ULongInit>(0ul);
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_unique<Initial>(std::move(static_init));
}

void checktype_file_scope_variable_declaration(CVariableDeclaration* node) {
    std::shared_ptr<InitialValue> initial_value;
    bool is_global = !(node->storage_class && 
                       node->storage_class->type() == AST_T::CStatic_t);

    if(node->init &&
       node->init->type() == AST_T::CConstant_t) {
        initial_value = checktype_constant_initial(static_cast<CConstant*>(node->init.get()),
                                                   node->var_type.get());
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
        initial_value = checktype_constant_initial(static_cast<CConstant*>(node->init.get()),
                                                   node->var_type.get());
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

void checktype_init_block_scope_variable_declaration(CVariableDeclaration* node) {
    if(node->init &&
       !node->storage_class &&
       !is_same_type(node->var_type.get(), node->init->exp_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->init), node->var_type);
        node->init = std::move(exp);
    }
}

void checktype_block_scope_variable_declaration(CVariableDeclaration* node) {
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
