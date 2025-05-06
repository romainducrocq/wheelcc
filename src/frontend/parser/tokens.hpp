#ifndef _FRONT_PARSER_TOKENS_H
#define _FRONT_PARSER_TOKENS_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

enum TOKEN_KIND {
    TOK_skip,

    TOK_assign_shiftleft,
    TOK_assign_shiftright,

    TOK_unop_incr,
    TOK_unop_decr,
    TOK_binop_shiftleft,
    TOK_binop_shiftright,
    TOK_binop_and,
    TOK_binop_or,
    TOK_binop_eq,
    TOK_binop_ne,
    TOK_binop_le,
    TOK_binop_ge,
    TOK_assign_add,
    TOK_assign_subtract,
    TOK_assign_multiply,
    TOK_assign_divide,
    TOK_assign_remainder,
    TOK_assign_bitand,
    TOK_assign_bitor,
    TOK_assign_xor,
    TOK_structop_ptr,

    TOK_comment_line,
    TOK_comment_start,
    TOK_comment_end,

    TOK_open_paren,
    TOK_close_paren,
    TOK_open_brace,
    TOK_close_brace,
    TOK_open_bracket,
    TOK_close_bracket,
    TOK_semicolon,
    TOK_unop_complement,
    TOK_unop_neg,
    TOK_unop_not,
    TOK_binop_add,
    TOK_binop_multiply,
    TOK_binop_divide,
    TOK_binop_remainder,
    TOK_binop_bitand,
    TOK_binop_bitor,
    TOK_binop_xor,
    TOK_binop_lt,
    TOK_binop_gt,
    TOK_assign,
    TOK_ternary_if,
    TOK_ternary_else,
    TOK_comma_separator,
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
    TOK_char_const,
    TOK_int_const,
    TOK_long_const,
    TOK_uint_const,
    TOK_ulong_const,
    TOK_dbl_const,

    TOK_include_preproc,
    TOK_strip_preproc,

    TOK_error
};

#endif
