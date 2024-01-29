#ifndef _SEMANTIC_TYPE_CHECK_HPP
#define _SEMANTIC_TYPE_CHECK_HPP

/** TODO
cdef bint is_same_type(Type type1, Type type2)
*/

/** TODO
cdef bint is_type_signed(Type type1)
*/

/** TODO
cdef bint is_const_signed(CConst node)
*/

/** TODO
cdef int32 get_type_size(Type type1)
*/

/** TODO
cdef void checktype_cast_expression(CCast node)
*/

/** TODO
cdef void checktype_function_call_expression(CFunctionCall node)
*/

/** TODO
cdef void checktype_var_expression(CVar node)
*/

/** TODO
cdef void checktype_constant_expression(CConstant node)
*/

/** TODO
cdef void checktype_assignment_expression(CAssignment node)
*/

/** TODO
cdef void checktype_assignment_compound_expression(CAssignmentCompound node)
*/

/** TODO
cdef void checktype_unary_expression(CUnary node)
*/

/** TODO
cdef void checktype_binary_expression(CBinary node)
*/

/** TODO
cdef void checktype_conditional_expression(CConditional node)
*/

/** TODO
cdef void checktype_return_statement(CReturn node)
*/

/** TODO
cdef void checktype_params(CFunctionDeclaration node)
*/

/** TODO
cdef void checktype_function_declaration(CFunctionDeclaration node)
*/

/** TODO
cdef void checktype_file_scope_variable_declaration(CVariableDeclaration node)
*/

/** TODO
cdef void checktype_block_scope_variable_declaration(CVariableDeclaration node)
*/

/** TODO
cdef void checktype_init_block_scope_variable_declaration(CVariableDeclaration node)
*/

/** TODO
cdef void init_check_types()
*/

#endif
