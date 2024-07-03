#ifndef _FRONTEND_PARSER_ERRORS_HPP
#define _FRONTEND_PARSER_ERRORS_HPP

#include <cstdio>
#include <string>

#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"

#include "frontend/parser/lexer.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum ERROR_MESSAGE {
    // Main
    no_option_code,
    invalid_option_code,
    no_input_files,

    // Util
    fail_open_rb_file,
    fail_open_wb_file,
    string_not_integer,
    string_not_unsigned,
    string_not_float,

    // Lexer
    invalid_token,

    // Parser
    invalid_next_token,
    reach_end_of_input,
    out_of_bound_constant,
    out_of_bound_unsigned,
    invalid_arr_size_type,
    invalid_unary_op,
    invalid_binary_op,
    invalid_abstract_declarator,
    invalid_pointer_unary_factor,
    invalid_primary_exp_factor,
    invalid_exp,
    invalid_for_loop_decl_type,
    invalid_type_specifier,
    invalid_type_specifier_list,
    invalid_storage_class,
    empty_compound_initializer,
    many_fun_type_derivation,
    fun_ptr_param_derivation,
    invalid_simple_declarator,
    invalid_param_list,
    invalid_member_decl_storage,
    invalid_member_decl_fun_type,

    // Semantic
    joint_pointer_type_mismatch,
    function_used_as_variable,
    cannot_convert_from_to,
    cannot_apply_unop_on_type
};

std::string get_token_kind_hr(TOKEN_KIND token_kind);
std::string get_type_hr(Type* type);
std::string get_unary_op_hr(CUnaryOp* node);
std::string get_error_message(ERROR_MESSAGE message);
template <typename... TArgs> std::string get_error_message(ERROR_MESSAGE message, TArgs&&... args) {
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), get_error_message(message).c_str(), std::forward<TArgs>(args)...);
    return std::string(buffer);
}
#define GET_ERROR_MESSAGE(...) get_error_message(__VA_ARGS__)

#endif
