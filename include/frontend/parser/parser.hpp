#ifndef _FRONT_PARSER_PARSER_H
#define _FRONT_PARSER_PARSER_H

#include <memory>
#include <vector>

#include "util/throw.hpp"

struct Token;
struct CProgram;
struct ErrorsContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

error_t parse_tokens(std::vector<Token>&& tokens, ErrorsContext* errors, IdentifierContext* identifiers,
    std::unique_ptr<CProgram>* c_ast);

#endif
