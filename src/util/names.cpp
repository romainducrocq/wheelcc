#include "util/names.hpp"

/** TODO
cdef int32 label_counter = 0
*/

/** TODO
cdef int32 variable_counter = 0
*/

/** TODO
cdef TIdentifier resolve_label_identifier(TIdentifier label):

    return represent_label_identifier(label.str_t)
*/

/** TODO
cdef TIdentifier resolve_variable_identifier(TIdentifier variable):
    global variable_counter

    variable_counter += 1
    cdef str name = f"{variable.str_t}.{variable_counter - 1}"

    return TIdentifier(name)
*/

/** TODO
cdef TIdentifier represent_label_identifier(str label):
    global label_counter

    label_counter += 1
    cdef str name = f"{label}.{label_counter - 1}"

    return TIdentifier(name)
*/

/** TODO
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
