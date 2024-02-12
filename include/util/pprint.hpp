#ifndef _UTIL_PPRINT_HPP
#define _UTIL_PPRINT_HPP

#ifndef __NDEBUG__

#include "parser/lexer.hpp"
#include "ast/ast.hpp"

#include <vector>

void pretty_print_tokens(const std::vector<Token>& tokens);
void pretty_print_ast(Ast* node, const std::string& name);
void pretty_print_symbol_table();
void pretty_print_backend_symbol_table();

#endif

#endif
