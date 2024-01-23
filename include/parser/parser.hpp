#ifndef _PARSER_PARSER_HPP
#define _PARSER_PARSER_HPP

#include "ast/c_ast.hpp"
#include "parser/lexer.hpp"

void parsing(std::vector<Token>& tokens, CProgram& c_ast);

#endif
