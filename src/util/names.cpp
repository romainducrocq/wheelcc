#include "util/names.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/c_ast.hpp"

#include <inttypes.h>
#include <string>

/**
cdef int32 label_counter = 0
*/
static int32_t label_counter = 0;

/**
cdef int32 variable_counter = 0
*/
static int32_t variable_counter = 0;

/**
cdef TIdentifier resolve_label_identifier(TIdentifier label):

    return represent_label_identifier(label.str_t)
*/
TIdentifier represent_label_identifier(const TIdentifier& label);
TIdentifier resolve_label_identifier(const TIdentifier& label) {
    return represent_label_identifier(label);
}

/**
cdef TIdentifier resolve_variable_identifier(TIdentifier variable):
    global variable_counter

    variable_counter += 1
    cdef str name = f"{variable.str_t}.{variable_counter - 1}"

    return TIdentifier(name)
*/
TIdentifier resolve_variable_identifier(const TIdentifier& variable) {
    variable_counter += 1;
    TIdentifier name = variable + "." + std::to_string(variable_counter - 1);
    return name;
}

/**
cdef TIdentifier represent_label_identifier(str label):
    global label_counter

    label_counter += 1
    cdef str name = f"{label}.{label_counter - 1}"

    return TIdentifier(name)
*/
TIdentifier represent_label_identifier(const TIdentifier& label) {
    label_counter += 1;
    TIdentifier name = label + "." + std::to_string(label_counter - 1);
    return name;
}

/**
cdef TIdentifier represent_variable_identifier(CExp node):
    global variable_counter

    cdef str variable
    if isinstance(node, CFunctionCall):
        variable = "funcall"
    elif isinstance(node, CVar):
        variable = "var"
    elif isinstance(node, CConstant):
        variable = "constant"
    elif isinstance(node, CCast):
        variable = "cast"
    elif isinstance(node, CAssignment):
        variable = "assignment"
    elif isinstance(node, CAssignmentCompound):
        variable = "compound"
    elif isinstance(node, CUnary):
        variable = "unary"
    elif isinstance(node, CBinary):
        variable = "binary"
    elif isinstance(node, CConditional):
        variable = "ternary"
    else:

        raise RuntimeError(
            f"An error occurred in name management, unmanaged type {type(node)}")

    variable_counter += 1
    cdef str name = f"{variable}.{variable_counter - 1}"

    return TIdentifier(name)
*/
TIdentifier represent_variable_identifier(CExp* node) {
    TIdentifier name;
    switch(node->type()) {
        case AST_T::CFunctionCall_t:
            name = "funcall";
            break;
        case AST_T::CVar_t:
            name = "var";
            break;
        case AST_T::CConstant_t:
            name = "constant";
            break;
        case AST_T::CCast_t:
            name = "cast";
            break;
        case AST_T::CAssignment_t:
            name = "assignment";
            break;
        case AST_T::CAssignmentCompound_t:
            name = "compound";
            break;
        case AST_T::CUnary_t:
            name = "unary";
            break;
        case AST_T::CBinary_t:
            name = "binary";
            break;
        case AST_T::CConditional_t:
            name = "ternary";
            break;
        default:
            raise_internal_error("An error occurred in name management, unmanaged type " +
                                 std::to_string(node->type()));
    }

    variable_counter += 1;
    name += "." + std::to_string(variable_counter - 1);

    return name;
}
