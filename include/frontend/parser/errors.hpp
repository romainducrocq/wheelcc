#ifndef _FRONTEND_PARSER_ERRORS_HPP
#define _FRONTEND_PARSER_ERRORS_HPP

#include <cstdio>
#include <string>

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/lexer.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum ERROR_MESSAGE {
    unhandled_runtime_error, // runtime_error

    // Main
    no_option_code_in_argument, // no_option_code
    invalid_option_code_in_argument, // invalid_option_code
    no_input_files_in_argument, // no_input_files

    // Util
    failed_to_read_output_file, // fail_open_rb_file
    failed_to_write_to_output_file, // fail_open_wb_file
    failed_to_interpret_string_to_integer, // string_not_integer
    failed_to_interpret_string_to_unsigned_integer, // string_not_unsigned
    failed_to_interpret_string_to_float, // string_not_float

    // Lexer
    invalid_token, // invalid_token

    // Parser
    unexpected_next_token, // invalid_next_token
    reached_end_of_file, // reach_end_of_input
    number_too_large_for_long_constant, // out_of_bound_constant
    number_too_large_for_unsigned_long_constant, // out_of_bound_unsigned
    array_size_not_a_constant_integer, // invalid_arr_size_type
    unexpected_unary_operator, // invalid_unary_op
    unexpected_binary_operator, // invalid_binary_op
    unexpected_abstract_declarator, // invalid_abstract_declarator
    unexpected_pointer_unary_factor, // invalid_pointer_unary_factor
    unexpected_primary_expression_factor, // invalid_primary_exp_factor
    unexpected_expression, // invalid_exp
    function_declared_in_for_initial, // invalid_for_loop_decl_type
    unexpected_type_specifier, // invalid_type_specifier
    unexpected_type_specifier_list, // invalid_type_specifier_list
    unexpected_storage_class, // invalid_storage_class
    empty_compound_initializer, // empty_compound_initializer
    type_derivation_on_function_declaration, // many_fun_type_derivation
    unexpected_simple_declarator, // invalid_simple_declarator
    unexpected_parameter_list, // invalid_param_list
    member_declared_with_non_automatic_storage, // invalid_member_decl_storage
    member_declared_as_function, // invalid_member_decl_fun_type

    // Semantic
    array_of_incomplete_type, // array_of_incomplete_type
    joint_pointer_type_mismatch, // joint_pointer_type_mismatch
    function_used_as_variable, // function_used_as_variable
    illegal_conversion_from_type_to, // cannot_convert_from_to
    unary_on_invalid_operand_type, // cannot_apply_unop_on_type
    binary_on_invalid_operand_type, // cannot_apply_binop_on_type
    binary_on_invalid_operand_types, // cannot_apply_binop_on_types
    assignment_to_void_type, // wrong_lhs_assignment_type
    assignment_to_rvalue, // invalid_lvalue_lhs_assignment
    conditional_on_invalid_condition_type, // wrong_cond_type_conditional
    ternary_on_invalid_operand_types, // conditional_type_mismatch
    variable_used_as_function, // variable_used_as_function
    function_called_with_wrong_number_of_arguments, // wrong_number_of_arguments
    dereference_non_pointer, // cannot_dereference_non_pointer
    address_of_rvalue, // invalid_lvalue_address_of
    subscript_array_with_invalid_types, // invalid_array_subscript_types
    get_size_of_incomplete_type, // size_of_incomplete_type
    dot_on_non_structure_type, // access_member_non_struct
    member_not_in_structure_type, // struct_has_no_member_named
    arrow_on_non_pointer_to_structure_type, // access_member_non_pointer
    arrow_on_incomplete_structure_type, // access_member_incomplete_type
    incomplete_structure_type_in_expression, // incomplete_struct_type
    return_value_in_void_function, // return_value_in_void_function
    no_return_value_in_non_void_function, // no_return_value_in_function
    if_used_with_condition_type, // cannot_use_if_with_type
    while_used_with_condition_type, // cannot_use_while_with_type
    do_while_used_with_condition_type, // cannot_use_do_while_with_type
    for_used_with_condition_type, // cannot_use_for_with_type
    non_char_array_initialized_from_string, // non_char_array_from_string
    string_initialized_with_too_many_characters, // wrong_string_literal_size
    array_initialized_with_too_many_initializers, // wrong_array_initializer_size
    structure_initialized_with_too_many_members, // wrong_struct_members_number
    function_returns_array, // function_returns_array
    function_returns_incomplete_structure_type, // function_returns_incomplete
    parameter_with_type_void, // function_has_void_param
    parameter_with_incomplete_structure_type, // function_has_incomplete_param
    function_redeclared_with_conflicting_type, // redeclaration_type_mismatch
    function_redefined, // redeclare_function_type
    non_static_function_redeclared_static, // redeclare_non_static_function
    static_pointer_initialized_from_non_integer, // pointer_type_from_constant
    static_pointer_initialized_from_non_null, // pointer_type_from_non_null
    static_non_char_pointer_initialized_from_string, // non_char_pointer_from_string
    static_initialized_with_non_constant, // static_variable_non_constant
    scalar_initialized_with_compound_initializer, // scalar_type_from_compound
    variable_declared_with_type_void, // variable_declared_void
    variable_declared_with_incomplete_structure_type, // variable_incomplete_structure
    variable_redeclared_with_conflicting_type, // redeclare_variable_mismatch
    variable_redeclared_with_conflicting_storage, // redeclare_variable_storage
    extern_variable_defined, // initialized_extern_variable
    structure_declared_with_duplicate_member, // structure_duplicate_member
    member_declared_with_incomplete_type, // structure_has_incomplete_member
    structure_redeclared_in_scope, // redeclare_structure_in_scope
    break_outside_of_loop, // break_outside_of_loop
    continue_outside_of_loop, // continue_outside_of_loop
    goto_with_undefined_target_label, // goto_without_target_label
    structure_not_defined_in_scope, // structure_not_defined_in_scope
    variable_not_declared_in_scope, // variable_not_declared_in_scope
    function_not_declared_in_scope, // function_not_declared_in_scope
    for_initial_declared_with_non_automatic_storage, // non_auto_variable_for_initial
    label_redefined_in_scope, // redeclare_label_in_scope
    variable_redeclared_in_scope, // redeclare_variable_in_scope
    nested_function_defined, // function_defined_nested
    nested_static_function_declared, // static_function_declared_nested
    function_redeclared_in_scope, // redeclare_function_in_scope  
};

std::string get_token_kind_hr(TOKEN_KIND token_kind);
std::string get_name_hr(const TIdentifier& name);
std::string get_struct_name_hr(const TIdentifier& name);
std::string get_type_hr(Type* type);
std::string get_const_hr(CConst* node);
std::string get_storage_class_hr(CStorageClass* node);
std::string get_unary_op_hr(CUnaryOp* node);
std::string get_binary_op_hr(CBinaryOp* node);
std::string get_assignment_hr(CBinaryOp* node);
std::string get_error_message(ERROR_MESSAGE message);
template <typename... TArgs> inline std::string get_error_message(ERROR_MESSAGE message, TArgs&&... args) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), get_error_message(message).c_str(), em(std::forward<TArgs>(args)).c_str()...);
    return std::string(buffer);
}
#define GET_ERROR_MESSAGE(...) get_error_message(__VA_ARGS__)

#endif
