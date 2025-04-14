#ifndef _UTIL_PPRINT_HPP
#define _UTIL_PPRINT_HPP

#ifndef __NDEBUG__
#include <string>
#include <vector>

#include "frontend/parser/lexer.hpp"

struct Ast;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

void pretty_print_tokens(const std::vector<Token>& tokens);
void pretty_print_ast(Ast* node, const std::string& name);
void pretty_print_addressed_set();
void pretty_print_string_constant_table();
void pretty_print_struct_typedef_table();
void pretty_print_symbol_table();
void pretty_print_backend_symbol_table();
void pretty_print_asm_code();

#endif
#endif
