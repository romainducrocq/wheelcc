#ifndef _FRONT_PARSER_PARSER_H
#define _FRONT_PARSER_PARSER_H

#include <memory>

#include "util/c_std.hpp"
#include "util/throw.hpp"

struct Token;
struct CProgram;
struct ErrorsContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

error_t parse_tokens(vector_t(Token) * tokens, ErrorsContext* errors, IdentifierContext* identifiers,
    return_t(std::unique_ptr<CProgram>) c_ast);

#endif
