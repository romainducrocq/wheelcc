#include "parser/precedence.hpp"
#include "parser/lexer.hpp"

#include <inttypes.h>
#include <unordered_map>

static std::unordered_map<TOKEN_KIND, int32_t> TOKEN_PRECEDENCE = {
    {TOKEN_KIND::binop_multiplication, 50},
    {TOKEN_KIND::binop_division, 50},
    {TOKEN_KIND::binop_remainder, 50},
    {TOKEN_KIND::unop_negation, 45},
    {TOKEN_KIND::binop_addition, 45},
    {TOKEN_KIND::binop_bitshiftleft, 40},
    {TOKEN_KIND::binop_bitshiftright, 40},
    {TOKEN_KIND::binop_lessthan, 35},
    {TOKEN_KIND::binop_lessthanorequal, 35},
    {TOKEN_KIND::binop_greaterthan, 35},
    {TOKEN_KIND::binop_greaterthanorequal, 35},
    {TOKEN_KIND::binop_equalto, 30},
    {TOKEN_KIND::binop_notequal, 30},
    {TOKEN_KIND::binop_bitand, 25},
    {TOKEN_KIND::binop_bitxor, 20},
    {TOKEN_KIND::binop_bitor, 15},
    {TOKEN_KIND::binop_and, 10},
    {TOKEN_KIND::binop_or, 5},
    {TOKEN_KIND::ternary_if, 3},
    {TOKEN_KIND::assignment_simple, 1},
    {TOKEN_KIND::assignment_plus, 1},
    {TOKEN_KIND::assignment_difference, 1},
    {TOKEN_KIND::assignment_product, 1},
    {TOKEN_KIND::assignment_quotient, 1},
    {TOKEN_KIND::assignment_remainder, 1},
    {TOKEN_KIND::assignment_bitand, 1},
    {TOKEN_KIND::assignment_bitor, 1},
    {TOKEN_KIND::assignment_bitxor, 1},
    {TOKEN_KIND::assignment_bitshiftleft, 1},
    {TOKEN_KIND::assignment_bitshiftright, 1}
};

int32_t parse_token_precedence(TOKEN_KIND token_kind) {
    if(TOKEN_PRECEDENCE.find(token_kind) == TOKEN_PRECEDENCE.end()) {
        return -1;
    }
    return TOKEN_PRECEDENCE[token_kind];
}
