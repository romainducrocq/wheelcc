#ifndef _FRONT_PARSER_PARSER_H
#define _FRONT_PARSER_PARSER_H

#include <memory>
#include <vector>

#include "frontend/parser/lexer.hpp"

struct CProgram;
struct ErrorsContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

std::unique_ptr<CProgram> parse_tokens(std::vector<Token>&& tokens, ErrorsContext* errors);

#endif
