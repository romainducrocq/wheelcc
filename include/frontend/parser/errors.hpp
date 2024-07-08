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

// TODO clean error message enum
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
    cannot_apply_unop_on_type,
    cannot_apply_binop_on_type,
    cannot_apply_binop_on_types,
    wrong_lhs_assignment_type,
    invalid_lvalue_lhs_assignment,
    wrong_cond_type_conditional,
    conditional_type_mismatch,
    variable_used_as_function,
    wrong_number_of_arguments,
    cannot_dereference_non_pointer,
    invalid_lvalue_address_of,
    invalid_array_subscript_types,
    size_of_incomplete_type,
    access_member_non_struct,
    struct_has_no_member_named,
    access_member_non_pointer,
    access_member_incomplete_type,
    incomplete_struct_type,
    return_value_in_void_function,
    no_return_value_in_function,
    cannot_use_if_with_type,
    cannot_use_while_with_type,
    cannot_use_do_while_with_type,
    cannot_use_for_with_type,
    non_char_array_from_string,
    wrong_string_literal_size,
    wrong_array_initializer_size,
    wrong_struct_members_number,
    function_returns_array,
    function_returns_incomplete,
    function_has_void_param,
    function_has_incomplete_param,
    redeclaration_type_mismatch,
    redeclare_function_type,
    redefine_non_static_function,
    pointer_type_from_constant,
    pointer_type_from_non_null,
    non_char_pointer_from_string,
    static_variable_non_constant,
    scalar_type_from_compound
};

std::string get_token_kind_hr(TOKEN_KIND token_kind);
std::string get_name_hr(const TIdentifier& name);
std::string get_type_hr(Type* type);
std::string get_const_hr(CConst* node);
std::string get_unary_op_hr(CUnaryOp* node);
std::string get_binary_op_hr(CBinaryOp* node);
std::string get_error_message(ERROR_MESSAGE message);
template <typename... TArgs> std::string get_error_message(ERROR_MESSAGE message, TArgs&&... args) {
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), get_error_message(message).c_str(), (em(args).c_str(), ...));
    return std::string(buffer);
}
#define GET_ERROR_MESSAGE(...) get_error_message(__VA_ARGS__)
#define GET_COMPOUND_INIT_LINE(X)                                            \
    [](CInitializer* node) -> size_t {                                       \
        while (node->type() == AST_T::CCompoundInit_t) {                     \
            node = static_cast<CCompoundInit*>(node)->initializers[0].get(); \
        }                                                                    \
        if (node->type() != AST_T::CSingleInit_t) {                          \
            RAISE_INTERNAL_ERROR;                                            \
        }                                                                    \
        return static_cast<CSingleInit*>(node)->exp->line;                   \
    }((X))

#endif
