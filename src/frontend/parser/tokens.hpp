#ifndef _FRONT_PARSER_TOKENS_H
#define _FRONT_PARSER_TOKENS_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

enum TOKEN_KIND {
    TOK_skip,

    TOK_assignment_bitshiftleft,
    TOK_assignment_bitshiftright,

    TOK_unop_increment,
    TOK_unop_decrement,
    TOK_binop_bitshiftleft,
    TOK_binop_bitshiftright,
    TOK_binop_and,
    TOK_binop_or,
    TOK_binop_equalto,
    TOK_binop_notequal,
    TOK_binop_lessthanorequal,
    TOK_binop_greaterthanorequal,
    TOK_assignment_plus,
    TOK_assignment_difference,
    TOK_assignment_product,
    TOK_assignment_quotient,
    TOK_assignment_remainder,
    TOK_assignment_bitand,
    TOK_assignment_bitor,
    TOK_assignment_bitxor,
    TOK_structop_pointer,

    TOK_comment_singleline,
    TOK_comment_multilinestart,
    TOK_comment_multilineend,

    TOK_parenthesis_open,
    TOK_parenthesis_close,
    TOK_brace_open,
    TOK_brace_close,
    TOK_brackets_open,
    TOK_brackets_close,
    TOK_semicolon,
    TOK_unop_complement,
    TOK_unop_negation,
    TOK_unop_not,
    TOK_binop_addition,
    TOK_binop_multiplication,
    TOK_binop_division,
    TOK_binop_remainder,
    TOK_binop_bitand,
    TOK_binop_bitor,
    TOK_binop_bitxor,
    TOK_binop_lessthan,
    TOK_binop_greaterthan,
    TOK_assignment_simple,
    TOK_ternary_if,
    TOK_ternary_else,
    TOK_separator_comma,
    TOK_structop_member,

    TOK_key_char,
    TOK_key_int,
    TOK_key_long,
    TOK_key_double,
    TOK_key_signed,
    TOK_key_unsigned,
    TOK_key_void,
    TOK_key_struct,
    TOK_key_union,
    TOK_key_sizeof,
    TOK_key_return,
    TOK_key_if,
    TOK_key_else,
    TOK_key_goto,
    TOK_key_do,
    TOK_key_while,
    TOK_key_for,
    TOK_key_switch,
    TOK_key_case,
    TOK_key_default,
    TOK_key_break,
    TOK_key_continue,
    TOK_key_static,
    TOK_key_extern,

    TOK_identifier,
    TOK_string_literal,
    TOK_char_constant,
    TOK_constant,
    TOK_long_constant,
    TOK_unsigned_constant,
    TOK_unsigned_long_constant,
    TOK_float_constant,

    TOK_include_directive,
    TOK_preprocessor_directive,

    TOK_error
};

#endif
