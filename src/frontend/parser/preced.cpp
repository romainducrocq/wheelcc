#include "frontend/parser/preced.hpp"
#include "frontend/parser/lexer.hpp"

#include <inttypes.h>

int32_t parse_token_precedence(TOKEN_KIND token_kind) {
    switch(token_kind) {
        case TOKEN_KIND::binop_multiplication:
        case TOKEN_KIND::binop_division:
        case TOKEN_KIND::binop_remainder:
            return 50;
        case TOKEN_KIND::unop_negation:
        case TOKEN_KIND::binop_addition:
            return 45;
        case TOKEN_KIND::binop_bitshiftleft:
        case TOKEN_KIND::binop_bitshiftright:
            return 40;
        case TOKEN_KIND::binop_lessthan:
        case TOKEN_KIND::binop_lessthanorequal:
        case TOKEN_KIND::binop_greaterthan:
        case TOKEN_KIND::binop_greaterthanorequal:
            return 35;
        case TOKEN_KIND::binop_equalto:
        case TOKEN_KIND::binop_notequal:
            return 30;
        case TOKEN_KIND::binop_bitand:
            return 25;
        case TOKEN_KIND::binop_bitxor:
            return 20;
        case TOKEN_KIND::binop_bitor:
            return 15;
        case TOKEN_KIND::binop_and:
            return 10;
        case TOKEN_KIND::binop_or:
            return 5;
        case TOKEN_KIND::ternary_if:
            return 3;
        case TOKEN_KIND::assignment_simple:
        case TOKEN_KIND::assignment_plus:
        case TOKEN_KIND::assignment_difference:
        case TOKEN_KIND::assignment_product:
        case TOKEN_KIND::assignment_quotient:
        case TOKEN_KIND::assignment_remainder:
        case TOKEN_KIND::assignment_bitand:
        case TOKEN_KIND::assignment_bitor:
        case TOKEN_KIND::assignment_bitxor:
        case TOKEN_KIND::assignment_bitshiftleft:
        case TOKEN_KIND::assignment_bitshiftright:
            return 1;
        default:
            return -1;
    }
}
