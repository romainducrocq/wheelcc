#include <string>

#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

const std::string get_token_kind_hr(TOKEN_KIND token_kind) {
    switch (token_kind) {
        case TOKEN_KIND::assignment_bitshiftleft:
            return "<<=";
        case TOKEN_KIND::assignment_bitshiftright:
            return ">>=";
        case TOKEN_KIND::unop_decrement:
            return "--";
        case TOKEN_KIND::binop_bitshiftleft:
            return "<<";
        case TOKEN_KIND::binop_bitshiftright:
            return ">>";
        case TOKEN_KIND::binop_and:
            return "&&";
        case TOKEN_KIND::binop_or:
            return "||";
        case TOKEN_KIND::binop_equalto:
            return "==";
        case TOKEN_KIND::binop_notequal:
            return "!=";
        case TOKEN_KIND::binop_lessthanorequal:
            return "<=";
        case TOKEN_KIND::binop_greaterthanorequal:
            return ">=";
        case TOKEN_KIND::assignment_plus:
            return "+=";
        case TOKEN_KIND::assignment_difference:
            return "-=";
        case TOKEN_KIND::assignment_product:
            return "*=";
        case TOKEN_KIND::assignment_quotient:
            return "/=";
        case TOKEN_KIND::assignment_remainder:
            return "%=";
        case TOKEN_KIND::assignment_bitand:
            return "&=";
        case TOKEN_KIND::assignment_bitor:
            return "|=";
        case TOKEN_KIND::assignment_bitxor:
            return "^=";
        case TOKEN_KIND::structop_pointer:
            return "->";
        case TOKEN_KIND::parenthesis_open:
            return "(";
        case TOKEN_KIND::parenthesis_close:
            return ")";
        case TOKEN_KIND::brace_open:
            return "{";
        case TOKEN_KIND::brace_close:
            return "}";
        case TOKEN_KIND::brackets_open:
            return "[";
        case TOKEN_KIND::brackets_close:
            return "]";
        case TOKEN_KIND::semicolon:
            return ";";
        case TOKEN_KIND::unop_complement:
            return "~";
        case TOKEN_KIND::unop_negation:
            return "-";
        case TOKEN_KIND::unop_not:
            return "!";
        case TOKEN_KIND::binop_addition:
            return "+";
        case TOKEN_KIND::binop_multiplication:
            return "*";
        case TOKEN_KIND::binop_division:
            return "/";
        case TOKEN_KIND::binop_remainder:
            return "%";
        case TOKEN_KIND::binop_bitand:
            return "&";
        case TOKEN_KIND::binop_bitor:
            return "|";
        case TOKEN_KIND::binop_bitxor:
            return "^";
        case TOKEN_KIND::binop_lessthan:
            return "<";
        case TOKEN_KIND::binop_greaterthan:
            return ">";
        case TOKEN_KIND::assignment_simple:
            return "=";
        case TOKEN_KIND::ternary_if:
            return "?";
        case TOKEN_KIND::ternary_else:
            return ":";
        case TOKEN_KIND::separator_comma:
            return ",";
        case TOKEN_KIND::structop_member:
            return ".";
        case TOKEN_KIND::key_char:
            return "char";
        case TOKEN_KIND::key_int:
            return "int";
        case TOKEN_KIND::key_long:
            return "long";
        case TOKEN_KIND::key_double:
            return "double";
        case TOKEN_KIND::key_signed:
            return "signed";
        case TOKEN_KIND::key_unsigned:
            return "unsigned";
        case TOKEN_KIND::key_void:
            return "void";
        case TOKEN_KIND::key_struct:
            return "struct";
        case TOKEN_KIND::key_sizeof:
            return "sizeof";
        case TOKEN_KIND::key_return:
            return "return";
        case TOKEN_KIND::key_if:
            return "if";
        case TOKEN_KIND::key_else:
            return "else";
        case TOKEN_KIND::key_goto:
            return "goto";
        case TOKEN_KIND::key_do:
            return "do";
        case TOKEN_KIND::key_while:
            return "while";
        case TOKEN_KIND::key_for:
            return "for";
        case TOKEN_KIND::key_break:
            return "break";
        case TOKEN_KIND::key_continue:
            return "continue";
        case TOKEN_KIND::key_static:
            return "static";
        case TOKEN_KIND::key_extern:
            return "extern";
        case TOKEN_KIND::identifier:
            return "identifier";
        case TOKEN_KIND::string_literal:
            return "string literal";
        case TOKEN_KIND::char_constant:
            return "const char8";
        case TOKEN_KIND::float_constant:
            return "const float64";
        case TOKEN_KIND::unsigned_long_constant:
            return "const uint64";
        case TOKEN_KIND::unsigned_constant:
            return "const uint32";
        case TOKEN_KIND::long_constant:
            return "const int64";
        case TOKEN_KIND::constant:
            return "const int32";
        default:
            RAISE_INTERNAL_ERROR;
    }
}
