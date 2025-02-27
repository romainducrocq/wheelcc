#ifndef _FRONTEND_PARSER_ERRORS_HPP
#define _FRONTEND_PARSER_ERRORS_HPP

#include <cstdio>
#include <string>

#include "ast/ast.hpp"

#include "messages.hpp" // frontend
#include "tokens.hpp"   // frontend

struct Type;
struct CConst;
struct CUnaryOp;
struct CBinaryOp;
struct CStorageClass;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum MESSAGE_ARGUMENT {
    unhandled_argument_error = 100,
    no_debug_code_in_argument,
    invalid_debug_code_in_argument,
    no_optim_1_mask_in_argument,
    invalid_optim_1_mask_in_argument,
    no_optim_2_code_in_argument,
    invalid_optim_2_code_in_argument,
    no_input_files_in_argument,
    no_include_directories_in_argument
};

enum MESSAGE_UTIL {
    unhandled_util_error = 200,
    failed_to_read_input_file,
    failed_to_write_to_output_file,
    failed_to_interpret_string_to_integer,
    failed_to_interpret_string_to_unsigned_integer,
    failed_to_interpret_string_to_float
};

enum MESSAGE_SEMANTIC {
    unhandled_semantic_error = 500,
    array_of_incomplete_type,
    joint_pointer_type_mismatch,
    function_used_as_variable,
    illegal_conversion_from_type_to,
    unary_on_invalid_operand_type,
    binary_on_invalid_operand_type,
    binary_on_invalid_operand_types,
    assignment_to_void_type,
    assignment_to_rvalue,
    conditional_on_invalid_condition_type,
    ternary_on_invalid_operand_types,
    variable_used_as_function,
    function_called_with_wrong_number_of_arguments,
    dereference_non_pointer,
    address_of_rvalue,
    subscript_array_with_invalid_types,
    get_size_of_incomplete_type,
    dot_on_non_structure_type,
    member_not_in_structure_type,
    arrow_on_non_pointer_to_structure_type,
    arrow_on_incomplete_structure_type,
    incomplete_structure_type_in_expression,
    return_value_in_void_function,
    no_return_value_in_non_void_function,
    if_used_with_condition_type,
    while_used_with_condition_type,
    do_while_used_with_condition_type,
    for_used_with_condition_type,
    switch_used_with_match_type,
    duplicate_case_value_in_switch,
    non_char_array_initialized_from_string,
    string_initialized_with_too_many_characters,
    array_initialized_with_too_many_initializers,
    structure_initialized_with_too_many_members,
    function_returns_array,
    function_returns_incomplete_structure_type,
    parameter_with_type_void,
    parameter_with_incomplete_structure_type,
    function_redeclared_with_conflicting_type,
    function_redefined,
    non_static_function_redeclared_static,
    static_pointer_initialized_from_non_integer,
    static_pointer_initialized_from_non_null,
    aggregate_initialized_with_single_initializer,
    static_non_char_pointer_initialized_from_string,
    static_initialized_with_non_constant,
    scalar_initialized_with_compound_initializer,
    variable_declared_with_type_void,
    variable_declared_with_incomplete_structure_type,
    variable_redeclared_with_conflicting_type,
    variable_redeclared_with_conflicting_storage,
    extern_variable_defined,
    structure_declared_with_duplicate_member,
    member_declared_with_incomplete_type,
    structure_redeclared_in_scope,
    case_outside_of_switch,
    default_outside_of_switch,
    more_than_one_default_in_switch,
    break_outside_of_loop,
    continue_outside_of_loop,
    goto_with_undefined_target_label,
    structure_conflicts_with_previously_declared,
    structure_not_defined_in_scope,
    variable_not_declared_in_scope,
    function_not_declared_in_scope,
    for_initial_declared_with_non_automatic_storage,
    label_redefined_in_scope,
    variable_redeclared_in_scope,
    nested_function_defined,
    nested_static_function_declared,
    function_redeclared_in_scope,
};

std::string get_token_kind_hr(TOKEN_KIND token_kind);
std::string get_name_hr(TIdentifier name);
std::string get_struct_name_hr(TIdentifier name, bool is_union);
std::string get_type_hr(Type* type);
std::string get_const_hr(CConst* node);
std::string get_storage_class_hr(CStorageClass* node);
std::string get_unary_op_hr(CUnaryOp* node);
std::string get_binary_op_hr(CBinaryOp* node);
std::string get_assignment_hr(CBinaryOp* node, CUnaryOp* unary_op);
#define get_token_kind_hr_c_str(X) get_token_kind_hr(X).c_str()
#define get_name_hr_c_str(X) get_name_hr(X).c_str()
#define get_struct_name_hr_c_str(X, Y) get_struct_name_hr(X, Y).c_str()
#define get_type_hr_c_str(X) get_type_hr(X).c_str()
#define get_const_hr_c_str(X) get_const_hr(X).c_str()
#define get_storage_class_hr_c_str(X) get_storage_class_hr(X).c_str()
#define get_unary_op_hr_c_str(X) get_unary_op_hr(X).c_str()
#define get_binary_op_hr_c_str(X) get_binary_op_hr(X).c_str()
#define get_assignment_hr_c_str(X, Y) get_assignment_hr(X, Y).c_str()

const char* get_argument_message(MESSAGE_ARGUMENT message);
const char* get_util_message(MESSAGE_UTIL message);
const char* get_lexer_message(MESSAGE_LEXER message);
const char* get_parser_message(MESSAGE_PARSER message);
const char* get_semantic_message(MESSAGE_SEMANTIC message);
#define GET_ARGUMENT_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_argument_message(X), (int)X, __VA_ARGS__)
#define GET_UTIL_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_util_message(X), (int)X, __VA_ARGS__)
#define GET_LEXER_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_lexer_message(X), (int)X, __VA_ARGS__)
#define GET_PARSER_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_parser_message(X), (int)X, __VA_ARGS__)
#define GET_SEMANTIC_MESSAGE(X, ...) GET_ERROR_MESSAGE(get_semantic_message(X), (int)X, __VA_ARGS__)
#define GET_ARGUMENT_MESSAGE_0(X) GET_ERROR_MESSAGE(get_argument_message(X), (int)X)
#define GET_PARSER_MESSAGE_0(X) GET_ERROR_MESSAGE(get_parser_message(X), (int)X)
#define GET_SEMANTIC_MESSAGE_0(X) GET_ERROR_MESSAGE(get_semantic_message(X), (int)X)

#endif
