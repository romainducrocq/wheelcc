#ifndef _FRONT_PARSER_MESSAGES_H
#define _FRONT_PARSER_MESSAGES_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum MESSAGE_LEXER {
    MSG_unhandled_lexer_error = 300,
    MSG_invalid_tok,
    MSG_failed_include
};

enum MESSAGE_PARSER {
    MSG_unhandled_parser_error = 400,
    MSG_unexpected_next_tok,
    MSG_reached_eof,
    MSG_overflow_long_const,
    MSG_overflow_ulong_const,
    MSG_arr_size_not_int_const,
    MSG_case_value_not_int_const,
    MSG_expect_unop,
    MSG_expect_binop,
    MSG_expect_abstract_decltor,
    MSG_expect_ptr_unary_factor,
    MSG_expect_primary_exp_factor,
    MSG_expect_exp,
    MSG_for_init_decl_as_fun,
    MSG_expect_specifier,
    MSG_expect_specifier_list,
    MSG_expect_storage_class,
    MSG_empty_compound_init,
    MSG_derived_fun_decl,
    MSG_expect_simple_decltor,
    MSG_expect_param_list,
    MSG_non_auto_member_decl,
    MSG_member_decl_as_fun
};

#endif
