#ifndef _FRONT_PARSER_TOKENS_H
#define _FRONT_PARSER_TOKENS_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

enum TOKEN_KIND {
    skip,

    assignment_bitshiftleft,
    assignment_bitshiftright,

    unop_increment,
    unop_decrement,
    binop_bitshiftleft,
    binop_bitshiftright,
    binop_and,
    binop_or,
    binop_equalto,
    binop_notequal,
    binop_lessthanorequal,
    binop_greaterthanorequal,
    assignment_plus,
    assignment_difference,
    assignment_product,
    assignment_quotient,
    assignment_remainder,
    assignment_bitand,
    assignment_bitor,
    assignment_bitxor,
    structop_pointer,

    comment_singleline,
    comment_multilinestart,
    comment_multilineend,

    parenthesis_open,
    parenthesis_close,
    brace_open,
    brace_close,
    brackets_open,
    brackets_close,
    semicolon,
    unop_complement,
    unop_negation,
    unop_not,
    binop_addition,
    binop_multiplication,
    binop_division,
    binop_remainder,
    binop_bitand,
    binop_bitor,
    binop_bitxor,
    binop_lessthan,
    binop_greaterthan,
    assignment_simple,
    ternary_if,
    ternary_else,
    separator_comma,
    structop_member,

    key_char,
    key_int,
    key_long,
    key_double,
    key_signed,
    key_unsigned,
    key_void,
    key_struct,
    key_union,
    key_sizeof,
    key_return,
    key_if,
    key_else,
    key_goto,
    key_do,
    key_while,
    key_for,
    key_switch,
    key_case,
    key_default,
    key_break,
    key_continue,
    key_static,
    key_extern,

    identifier,
    string_literal,
    char_constant,
    constant,
    long_constant,
    unsigned_constant,
    unsigned_long_constant,
    float_constant,

    include_directive,
    preprocessor_directive,

    error
};

#endif
