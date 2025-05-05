#ifndef _UTIL_PPRINT_H
#define _UTIL_PPRINT_H

#ifndef __NDEBUG__
#include <string>
#include <vector>

#include "frontend/parser/lexer.hpp"

struct Ast;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

void pprint_toks(const std::vector<Token>& tokens);
void pprint_ast(Ast* node, const std::string& name);
void pprint_addressed_set();
void pprint_string_const_table();
void pprint_struct_typedef_table();
void pprint_symbol_table();
void pprint_backend_symbol_table();
void pprint_asm_code();

#endif
#endif
