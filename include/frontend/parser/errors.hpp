#ifndef _FRONTEND_PARSER_ERRORS_HPP
#define _FRONTEND_PARSER_ERRORS_HPP

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/lexer.hpp"
#include "messages.hpp" // frontend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum ERROR_MESSAGE_ARGUMENT {
    unhandled_argument_error = 100,
    no_debug_code_in_argument,
    invalid_debug_code_in_argument,
    no_input_files_in_argument,
    no_include_directories_in_argument
};

enum ERROR_MESSAGE_UTIL {
    unhandled_util_error = 200,
    failed_to_read_input_file,
    failed_to_write_to_output_file,
    failed_to_interpret_string_to_integer,
    failed_to_interpret_string_to_unsigned_integer,
    failed_to_interpret_string_to_float
};

enum ERROR_MESSAGE_SEMANTIC {
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
    break_outside_of_loop,
    continue_outside_of_loop,
    goto_with_undefined_target_label,
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

struct FileOpenLine {
    size_t line_number;
    size_t total_line_number;
    std::string filename;
};

struct ErrorsContext {
    size_t line_buffer;
    std::vector<FileOpenLine> file_open_lines;
};

extern std::unique_ptr<ErrorsContext> errors;
#define INIT_ERRORS_CONTEXT errors = std::make_unique<ErrorsContext>()
#define FREE_ERRORS_CONTEXT errors.reset()

std::string get_token_kind_hr(TOKEN_KIND token_kind);
std::string get_name_hr(const TIdentifier& name);
std::string get_struct_name_hr(const TIdentifier& name);
std::string get_type_hr(Type* type);
std::string get_const_hr(CConst* node);
std::string get_storage_class_hr(CStorageClass* node);
std::string get_unary_op_hr(CUnaryOp* node);
std::string get_binary_op_hr(CBinaryOp* node);
std::string get_assignment_hr(CBinaryOp* node);
std::string get_what_message(ERROR_MESSAGE_ARGUMENT message);
std::string get_what_message(ERROR_MESSAGE_UTIL message);
std::string get_what_message(ERROR_MESSAGE_LEXER message);
std::string get_what_message(ERROR_MESSAGE_PARSER message);
std::string get_what_message(ERROR_MESSAGE_SEMANTIC message);
template <typename TErrorMessage> inline std::string get_error_message(TErrorMessage message) {
    std::string error_message = "(no. ";
    error_message += std::to_string(static_cast<int>(message));
    error_message += ") ";
    error_message += get_what_message(message);
    return error_message;
}
template <typename TErrorMessage, typename... TArgs>
inline std::string get_error_message(TErrorMessage message, TArgs&&... args) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), get_error_message(message).c_str(), em(std::forward<TArgs>(args)).c_str()...);
    return std::string(buffer);
}
size_t handle_error_at_line(size_t total_line_number);
#define GET_ERROR_MESSAGE(...) get_error_message(__VA_ARGS__)
#define RAISE_RUNTIME_ERROR_AT_LINE(X, Y) raise_runtime_error_at_line((X), handle_error_at_line(Y))

#endif
