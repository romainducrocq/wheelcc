#ifndef _UTIL_PPRINT_H
#define _UTIL_PPRINT_H

#ifndef __NDEBUG__
#include <string>
#include <vector>

#include "frontend/parser/lexer.hpp"

struct Ast;
struct BackEndContext;
struct FrontEndContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

void pprint_toks(const std::vector<Token>& tokens);
void pprint_ast(Ast* node, const std::string& name);
void pprint_addressed_set(void);
void pprint_string_const_table(void);
void pprint_struct_typedef_table(void);
void pprint_symbol_table(void);
void pprint_backend_symbol_table(void);
void pprint_asm_code(void);
void pprint_p_backend(BackEndContext* ctx);
void pprint_p_frontend(FrontEndContext* ctx);
void pprint_p_identifiers(IdentifierContext* ctx);

#endif
#endif
