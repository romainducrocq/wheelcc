#ifndef _PARSER_LEXER_HPP
#define _PARSER_LEXER_HPP

#include <string>
#include <vector>

enum TOKEN_KIND {
    assignment_bitshiftleft,
    assignment_bitshiftright,

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

    comment_singleline,
    comment_multilinestart,
    comment_multilineend,

    parenthesis_open,
    parenthesis_close,
    brace_open,
    brace_close,
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

    preprocessor_directive,

    key_int,
    key_long,
    key_double,
    key_signed,
    key_unsigned,
    key_void,
    key_return,
    key_if,
    key_else,
    key_goto,
    key_do,
    key_while,
    key_for,
    key_break,
    key_continue,
    key_static,
    key_extern,

    identifier,
    float_constant,
    unsigned_long_constant,
    unsigned_constant,
    long_constant,
    constant,

    skip,
    error
};

struct Token {
    std::string token;
    TOKEN_KIND token_kind;
    size_t line;
};

std::vector<Token> lexing(std::string&& filename);

#endif
