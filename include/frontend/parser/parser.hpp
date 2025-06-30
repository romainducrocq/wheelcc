#ifndef _FRONT_PARSER_PARSER_H
#define _FRONT_PARSER_PARSER_H

#include <memory>
#include <vector>

#include "frontend/parser/lexer.hpp"

struct CProgram;
struct ErrorsContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

int parse_tokens(std::vector<Token>&& tokens, ErrorsContext* errors, IdentifierContext* identifiers,
    std::unique_ptr<CProgram>* c_ast);

#endif
