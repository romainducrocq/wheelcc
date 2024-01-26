#ifndef _UTIL_PPRINT_HPP
#define _UTIL_PPRINT_HPP

#include "parser/lexer.hpp"
#include "ast/ast.hpp"

#include <vector>

void pretty_print_tokens(const std::vector<Token>& tokens);
void pretty_print_ast(Ast* node, const std::string& name);

#endif
