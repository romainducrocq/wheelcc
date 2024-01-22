#ifndef _PARSER_PRECEDENCE_HPP
#define _PARSER_PRECEDENCE_HPP

#include "parser/lexer.hpp"

#include <inttypes.h>

int32_t parse_token_precedence(TOKEN_KIND token_kind);

#endif
