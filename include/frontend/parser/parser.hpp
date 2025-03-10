#ifndef _FRONTEND_PARSER_PARSER_HPP
#define _FRONTEND_PARSER_PARSER_HPP

#include <memory>
#include <vector>

#include "frontend/parser/lexer.hpp"

struct CProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

std::unique_ptr<CProgram> parsing(std::vector<Token>&& tokens);

#endif
