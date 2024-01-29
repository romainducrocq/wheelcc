#include "semantic/type_check.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"

#include <inttypes.h>
#include <string>
#include <memory>
#include <set>

/**
cdef set[str] defined_set = set()
*/
static std::set<TIdentifier> defined_set;

/**
cdef str function_declaration_name_str = ""
*/
static TIdentifier function_declaration_name_str;

/**
cdef bint is_same_type(Type type1, Type type2):
    return isinstance(type1, type(type2))
*/
bool is_same_type(Type* type1, Type* type2) {
    return type1->type() == type2->type();
}

/**
cdef bint is_same_fun_type(FunType fun_type1, FunType fun_type2):
    if len(fun_type1.param_types) != len(fun_type2.param_types):
        return False
    if not is_same_type(fun_type1.ret_type, fun_type2.ret_type):
        return False
    cdef Py_ssize_t param_type
    for param_type in range(len(fun_type1.param_types)):
        if not is_same_type(fun_type1.param_types[param_type], fun_type2.param_types[param_type]):
            return False
    return True
*/
bool is_same_fun_type(FunType* fun_type1, FunType* fun_type2) {
    if(fun_type1->param_types.size() != fun_type2->param_types.size()) {
        return false;
    }
    if(!is_same_type(fun_type1->ret_type.get(), fun_type2->ret_type.get())) {
        return false;
    }
    for(size_t param_type = 0; param_type < fun_type1->param_types.size(); param_type++) {
        if(!is_same_type(fun_type1->param_types[param_type].get(), fun_type2->param_types[param_type].get())) {
            return false;
        }
    }
    return true;
}

/**
cdef int32 get_type_size(Type type1):
    if isinstance(type1, (Int, UInt)):
        return 32
    elif isinstance(type1, (Long, Double, ULong)):
        return 64
    else:
        return -1
*/
int32_t get_type_size(Type* type1) {
    switch(type1->type()) {
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

/**
cdef bint is_type_signed(Type type1):
    return isinstance(type1, (Int, Long))
*/
bool is_type_signed(Type* type1) {
    switch(type1->type()) {
        case AST_T::Int_t:
        case AST_T::Long_t:
            return true;
        default:
            return false;
    }
}

/**
cdef bint is_const_signed(CConst node):
    return isinstance(node, (CConstInt, CConstLong))
*/
bool is_const_signed(CConst* node) {
    switch(node->type()) {
        case AST_T::CConstInt_t:
        case AST_T::CConstLong_t:
            return true;
        default:
            return false;
    }
}

/**
cdef Type get_joint_type(Type type1, Type type2):
    if is_same_type(type1, type2):
        return type1
    elif isinstance(type1, Double) or \
         isinstance(type2, Double):
        return Double()
    cdef int32 type1_size = get_type_size(type1)
    cdef int32 type2_size = get_type_size(type2)
    if type1_size == type2_size:
        if is_type_signed(type1):
            return type2
        else:
            return type1
    if type1_size > type2_size:
        return type1
    else:
        return type2
*/
std::shared_ptr<Type> get_joint_type(std::shared_ptr<Type>& type1, std::shared_ptr<Type>& type2) {
    if(is_same_type(type1.get(), type2.get())) {
        return type1;
    }
    else if(type1->type() == AST_T::Double_t ||
            type2->type() == AST_T::Double_t) {
        return std::make_shared<Double>();
    }
    int32_t type1_size = get_type_size(type1.get());
    int32_t type2_size = get_type_size(type2.get());
    if(type1_size == type2_size) {
        if(is_type_signed(type1.get())) {
            return type2;
        } else {
            return type1;
        }
    }
    if(type1_size > type2_size) {
        return type1;
    } else {
        return type2;
    }
}

/**
cdef void checktype_cast_expression(CCast node):
    node.exp_type = node.target_type
*/
void checktype_cast_expression(CCast* node) {
    node->exp_type = node->target_type;
}

/**
cdef CCast cast_expression(CExp node, Type exp_type):
    cdef CExp exp = CCast(node, exp_type)
    checktype_cast_expression(exp)
    return exp
*/
std::unique_ptr<CCast> cast_expression(std::unique_ptr<CExp> node, std::shared_ptr<Type>& exp_type) {
    std::unique_ptr<CCast> exp = std::make_unique<CCast>(std::move(node), exp_type);
    checktype_cast_expression(exp.get());
    return exp;
}

/**
cdef void checktype_function_call_expression(CFunctionCall node):

    if not isinstance(symbol_table[node.name.str_t].type_t, FunType):

        raise RuntimeError(
            f"Variable {node.name.str_t} was used as a function")

    if len(symbol_table[node.name.str_t].type_t.param_types) != len(node.args):

        raise RuntimeError(
            f"""Function {node.name.str_t} has {len(symbol_table[node.name.str_t].type_t.param_types)} arguments
                but was called with {len(node.args)}""")

    cdef Py_ssize_t i
    for i in range(len(node.args)):
        if not is_same_type(node.args[i].exp_type, symbol_table[node.name.str_t].type_t.param_types[i]):
            node.args[i] = cast_expression(node.args[i], symbol_table[node.name.str_t].type_t.param_types[i])

    node.exp_type = symbol_table[node.name.str_t].type_t.ret_type
*/
void checktype_function_call_expression(CFunctionCall* node) {
    if(symbol_table[node->name]->type_t->type() != AST_T::FunType_t) {
        raise_runtime_error("Variable " + em(node->name) + " was used as a function");
    }
    FunType* symbol = static_cast<FunType*>(symbol_table[node->name]->type_t.get());
    if(symbol->param_types.size() != node->args.size()) {
        raise_runtime_error("Function " + em(node->name) + " has " +
                            em(std::to_string(symbol->param_types.size())) +
                            " arguments but was called with " + em(std::to_string(node->args.size())));
    }
    for(size_t i = 0; i < node->args.size(); i ++) {
        if(!is_same_type(node->args[i]->exp_type.get(), symbol->param_types[i].get())) {
            std::unique_ptr<CExp> exp = cast_expression(std::move(node->args[i]), symbol->param_types[i]);
            node->args[i] = std::move(exp);
        }
    }
    node->exp_type = symbol->ret_type;
}

/**
cdef void checktype_var_expression(CVar node):
    if isinstance(symbol_table[node.name.str_t].type_t, FunType):

        raise RuntimeError(
            f"Function {node.name.str_t} was used as a variable")

    node.exp_type = symbol_table[node.name.str_t].type_t
*/
void checktype_var_expression(CVar* node) {
    if(symbol_table[node->name]->type_t->type() == AST_T::FunType_t) {
        raise_runtime_error("Function " + em(node->name) + " was used as a variable");
    }
    node->exp_type = symbol_table[node->name]->type_t;
}

/**
cdef void checktype_constant_expression(CConstant node):
    if isinstance(node.constant, CConstInt):
        node.exp_type = Int()
    elif isinstance(node.constant, CConstLong):
        node.exp_type = Long()
    elif isinstance(node.constant, CConstDouble):
        node.exp_type = Double()
    elif isinstance(node.constant, CConstUInt):
        node.exp_type = UInt()
    elif isinstance(node.constant, CConstULong):
        node.exp_type = ULong()
*/
void checktype_constant_expression(CConstant* node) {
    switch(node->type()) {
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
            break;
    }
}

/**
cdef void checktype_assignment_expression(CAssignment node):
    if not is_same_type(node.exp_right.exp_type, node.exp_left.exp_type):
        node.exp_right = cast_expression(node.exp_right, node.exp_left.exp_type)
    node.exp_type = node.exp_left.exp_type
*/
void checktype_assignment_expression(CAssignment* node) {
    if(!is_same_type(node->exp_right->exp_type.get(), node->exp_left->exp_type.get())) {
        std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_right), node->exp_left->exp_type);
        node->exp_right = std::move(exp);
    }
    node->exp_type = node->exp_left->exp_type;
}

/**
cdef void checktype_unary_expression(CUnary node):
    if isinstance(node.unary_op, CNot):
        node.exp_type = Int()
    else:
        if isinstance(node.unary_op, CComplement) and \
           isinstance(node.exp.exp_type, Double):

            raise RuntimeError(
                "An error occurred in type checking, unary operator can not be used on floating-point number")

        node.exp_type = node.exp.exp_type
*/
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

/**
cdef void checktype_binary_expression(CBinary node):
    if isinstance(node.binary_op, (CAnd, COr)):
        node.exp_type = Int()
        return
    elif isinstance(node.binary_op, (CBitShiftLeft, CBitShiftRight)):
        # Note: https://stackoverflow.com/a/70130146
        # if the value of the right operand is negative or is greater than or equal
        # to the width of the promoted left operand, the behavior is undefined
        if not is_same_type(node.exp_left.exp_type, node.exp_right.exp_type):
            node.exp_right = cast_expression(node.exp_right, node.exp_left.exp_type)
        node.exp_type = node.exp_left.exp_type
        if isinstance(node.exp_type, Double):

            raise RuntimeError(
                "An error occurred in type checking, binary operator can not be used on floating-point number")

        return
    cdef Type common_type = get_joint_type(node.exp_left.exp_type, node.exp_right.exp_type)
    if not is_same_type(node.exp_left.exp_type, common_type):
        node.exp_left = cast_expression(node.exp_left, common_type)
    if not is_same_type(node.exp_right.exp_type, common_type):
        node.exp_right = cast_expression(node.exp_right, common_type)
    if isinstance(node.binary_op, (CAdd, CSubtract, CMultiply, CDivide)):
        node.exp_type = common_type
    elif isinstance(node.binary_op, (CRemainder, CBitAnd, CBitOr, CBitXor)):
        node.exp_type = common_type
        if isinstance(node.exp_type, Double):

            raise RuntimeError(
                "An error occurred in type checking, binary operator can not be used on floating-point number")

    else:
        node.exp_type = Int()
*/
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
                std::unique_ptr<CExp> exp = cast_expression(std::move(node->exp_right), node->exp_left->exp_type);
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
    std::shared_ptr<Type> common_type = get_joint_type(node->exp_left->exp_type, node->exp_right->exp_type);
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

/** TODO
cdef void checktype_conditional_expression(CConditional node):
    cdef Type common_type = get_joint_type(node.exp_middle.exp_type, node.exp_right.exp_type)
    if not is_same_type(node.exp_middle.exp_type, common_type):
        node.exp_middle = cast_expression(node.exp_middle, common_type)
    if not is_same_type(node.exp_right.exp_type, common_type):
        node.exp_right = cast_expression(node.exp_right, common_type)
    node.exp_type = common_type
*/

/** TODO
cdef void checktype_return_statement(CReturn node):
    if not is_same_type(node.exp.exp_type, symbol_table[function_declaration_name_str].type_t.ret_type):
        node.exp = cast_expression(node.exp, symbol_table[function_declaration_name_str].type_t.ret_type)
*/

/** TODO
cdef Symbol checktype_param(FunType fun_type, Py_ssize_t param):
    cdef Type type_t = fun_type.param_types[param]
    cdef IdentifierAttr param_attrs = LocalAttr()
    return Symbol(type_t, param_attrs)
*/

/** TODO
cdef void checktype_params(CFunctionDeclaration node):
    cdef Py_ssize_t param
    if node.body:
        for param in range(len(node.params)):
            symbol_table[node.params[param].str_t] = checktype_param(node.fun_type, param)
*/

/** TODO
cdef void checktype_function_declaration(CFunctionDeclaration node):
    global function_declaration_name_str

    cdef bint is_defined = node.name.str_t in defined_set
    cdef bint is_global = not isinstance(node.storage_class, CStatic)

    if node.name.str_t in symbol_table:
        if not (isinstance(symbol_table[node.name.str_t].type_t, FunType) and
                len(symbol_table[node.name.str_t].type_t.param_types) == len(node.params) and
                is_same_fun_type(symbol_table[node.name.str_t].type_t, node.fun_type)):

            raise RuntimeError(
                f"Function declaration {node.name.str_t} was redeclared with conflicting type")

        if is_defined and \
           node.body:

                raise RuntimeError(
                    f"Function {node.name.str_t} was already defined")

        if not is_global and \
           symbol_table[node.name.str_t].attrs.is_global:

            raise RuntimeError(
                f"Static function {node.name.str_t} was already defined non-static")

        is_global = symbol_table[node.name.str_t].attrs.is_global

    if node.body:
        defined_set.add(node.name.str_t)
        is_defined = True

    cdef Type fun_type = node.fun_type
    cdef IdentifierAttr fun_attrs = FunAttr(is_defined, is_global)
    symbol_table[node.name.str_t] = Symbol(fun_type, fun_attrs)

    function_declaration_name_str = node.name.str_t
*/

/** TODO
cdef Initial checktype_constant_initial(CConstant node, Type static_init_type):
    if isinstance(static_init_type, Int):
        if isinstance(node.constant, CConstInt):
            return Initial(IntInit(copy_int(node.constant.value)))
        elif isinstance(node.constant, CConstLong):
            return Initial(IntInit(copy_long_to_int(node.constant.value)))
        elif isinstance(node.constant, CConstDouble):
            return Initial(IntInit(copy_double_to_int(node.constant.value)))
        elif isinstance(node.constant, CConstUInt):
            return Initial(IntInit(copy_uint_to_int(node.constant.value)))
        elif isinstance(node.constant, CConstULong):
            return Initial(IntInit(copy_ulong_to_int(node.constant.value)))
    elif isinstance(static_init_type, Long):
        if isinstance(node.constant, CConstInt):
            return Initial(LongInit(copy_int_to_long(node.constant.value)))
        elif isinstance(node.constant, CConstLong):
            return Initial(LongInit(copy_long(node.constant.value)))
        elif isinstance(node.constant, CConstDouble):
            return Initial(LongInit(copy_double_to_long(node.constant.value)))
        elif isinstance(node.constant, CConstUInt):
            return Initial(LongInit(copy_uint_to_long(node.constant.value)))
        elif isinstance(node.constant, CConstULong):
            return Initial(LongInit(copy_ulong_to_long(node.constant.value)))
    elif isinstance(static_init_type, Double):
        if isinstance(node.constant, CConstInt):
            return Initial(DoubleInit(copy_int_to_double(node.constant.value)))
        elif isinstance(node.constant, CConstLong):
            return Initial(DoubleInit(copy_long_to_double(node.constant.value)))
        elif isinstance(node.constant, CConstDouble):
            return Initial(DoubleInit(copy_double(node.constant.value)))
        elif isinstance(node.constant, CConstUInt):
            return Initial(DoubleInit(copy_uint_to_double(node.constant.value)))
        elif isinstance(node.constant, CConstULong):
            return Initial(DoubleInit(copy_ulong_to_double(node.constant.value)))
    elif isinstance(static_init_type, UInt):
        if isinstance(node.constant, CConstInt):
            return Initial(UIntInit(copy_int_to_uint(node.constant.value)))
        elif isinstance(node.constant, CConstLong):
            return Initial(UIntInit(copy_long_to_uint(node.constant.value)))
        elif isinstance(node.constant, CConstDouble):
            return Initial(UIntInit(copy_double_to_uint(node.constant.value)))
        elif isinstance(node.constant, CConstUInt):
            return Initial(UIntInit(copy_uint(node.constant.value)))
        elif isinstance(node.constant, CConstULong):
            return Initial(UIntInit(copy_ulong_to_uint(node.constant.value)))
    elif isinstance(static_init_type, ULong):
        if isinstance(node.constant, CConstInt):
            return Initial(ULongInit(copy_int_to_ulong(node.constant.value)))
        elif isinstance(node.constant, CConstLong):
            return Initial(ULongInit(copy_long_to_ulong(node.constant.value)))
        elif isinstance(node.constant, CConstDouble):
            return Initial(ULongInit(copy_double_to_ulong(node.constant.value)))
        elif isinstance(node.constant, CConstUInt):
            return Initial(ULongInit(copy_uint_to_ulong(node.constant.value)))
        elif isinstance(node.constant, CConstULong):
            return Initial(ULongInit(copy_ulong(node.constant.value)))
*/

/** TODO
cdef Initial checktype_no_init_initial(Type static_init_type):
    if isinstance(static_init_type, Int):
        return Initial(IntInit(TInt(0)))
    elif isinstance(static_init_type, Long):
        return Initial(LongInit(TLong(0)))
    elif isinstance(static_init_type, Double):
        return Initial(DoubleInit(TDouble(0.0)))
    elif isinstance(static_init_type, UInt):
        return Initial(UIntInit(TUInt(0)))
    elif isinstance(static_init_type, ULong):
        return Initial(ULongInit(TULong(0)))
*/

/** TODO
cdef void checktype_file_scope_variable_declaration(CVariableDeclaration node):
    cdef InitialValue initial_value
    cdef bint is_global = not isinstance(node.storage_class, CStatic)

    if isinstance(node.init, CConstant):
        initial_value = checktype_constant_initial(node.init, node.var_type)
    elif not node.init:
        if isinstance(node.storage_class, CExtern):
            initial_value = NoInitializer()
        else:
            initial_value = Tentative()
    else:

        raise RuntimeError(
            f"File scope variable {node.name.str_t} was initialized to a non-constant")

    if node.name.str_t in symbol_table:
        if not is_same_type(symbol_table[node.name.str_t].type_t, node.var_type):

            raise RuntimeError(
                f"File scope variable {node.name.str_t} was redeclared with conflicting type")

        if isinstance(node.storage_class, CExtern):
            is_global = symbol_table[node.name.str_t].attrs.is_global
        elif is_global != symbol_table[node.name.str_t].attrs.is_global:

            raise RuntimeError(
                f"File scope variable {node.name.str_t} was redeclared with conflicting linkage")

        if isinstance(symbol_table[node.name.str_t].attrs.init, Initial):
            if isinstance(initial_value, Initial):

                raise RuntimeError(
                    f"File scope variable {node.name.str_t} was defined with conflicting linkage")

            else:
                initial_value = symbol_table[node.name.str_t].attrs.init

    cdef Type global_var_type = node.var_type
    cdef IdentifierAttr global_var_attrs = StaticAttr(initial_value, is_global)
    symbol_table[node.name.str_t] = Symbol(global_var_type, global_var_attrs)
*/

/** TODO
cdef void checktype_extern_block_scope_variable_declaration(CVariableDeclaration node):
    if node.init:
        raise RuntimeError(
            f"Block scope variable {node.name.str_t} with external linkage was defined")

    if node.name.str_t in symbol_table:
        if not is_same_type(symbol_table[node.name.str_t].type_t, node.var_type):

            raise RuntimeError(
                f"Block scope variable {node.name.str_t} was redeclared with conflicting type")

        return

    cdef Type local_var_type = node.var_type
    cdef IdentifierAttr local_var_attrs = StaticAttr(NoInitializer(), True)
    symbol_table[node.name.str_t] = Symbol(local_var_type, local_var_attrs)
*/

/** TODO
cdef void checktype_static_block_scope_variable_declaration(CVariableDeclaration node):
    cdef InitialValue initial_value

    if isinstance(node.init, CConstant):
        initial_value = checktype_constant_initial(node.init, node.var_type)
    elif not node.init:
        initial_value = checktype_no_init_initial(node.var_type)
    else:

        raise RuntimeError(
            f"Block scope variable {node.name.str_t} with static linkage was initialized to a non-constant")

    cdef Type local_var_type = node.var_type
    cdef IdentifierAttr local_var_attrs = StaticAttr(initial_value, False)
    symbol_table[node.name.str_t] = Symbol(local_var_type, local_var_attrs)
*/

/** TODO
cdef void checktype_automatic_block_scope_variable_declaration(CVariableDeclaration node):
    cdef Type local_var_type = node.var_type
    cdef IdentifierAttr local_var_attrs = LocalAttr()
    symbol_table[node.name.str_t] = Symbol(local_var_type, local_var_attrs)
*/

/** TODO
cdef void checktype_init_block_scope_variable_declaration(CVariableDeclaration node):
    if node.init and \
       not node.storage_class and \
       not is_same_type(node.var_type, node.init.exp_type):

        node.init = cast_expression(node.init, node.var_type)
*/

/** TODO
cdef void checktype_block_scope_variable_declaration(CVariableDeclaration node):
    if isinstance(node.storage_class, CExtern):
        checktype_extern_block_scope_variable_declaration(node)
    elif isinstance(node.storage_class, CStatic):
        checktype_static_block_scope_variable_declaration(node)
    else:
        checktype_automatic_block_scope_variable_declaration(node)
*/

/** TODO
cdef void init_check_types():
    global function_declaration_name_str
    defined_set.clear()
    function_declaration_name_str = ""
*/
