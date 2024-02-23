#ifndef _FRONTEND_PARSER_PRECEDENCE_HPP
#define _FRONTEND_PARSER_PRECEDENCE_HPP

#include "frontend/parser/lexer.hpp"

#include <inttypes.h>

int32_t parse_token_precedence(TOKEN_KIND token_kind);

#endif
