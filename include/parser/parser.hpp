#ifndef _PARSER_PARSER_HPP
#define _PARSER_PARSER_HPP

#include "ast/c_ast.hpp"
#include "parser/lexer.hpp"

#include <memory>
#include <vector>

std::unique_ptr<CProgram> parsing(std::vector<Token>&& tokens);

#endif
