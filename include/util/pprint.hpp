#ifndef _UTIL_PPRINT_H
#define _UTIL_PPRINT_H

#ifndef __NDEBUG__
#include <vector>

struct Token;
struct Ast;
struct BackEndContext;
struct FrontEndContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

void pprint_toks(IdentifierContext* ctx, std::vector<Token>& tokens);
void pprint_ast(IdentifierContext* ctx, Ast* node, const char* name);
void pprint_addressed_set(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_string_const_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_struct_typedef_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_symbol_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_backend_symbol_table(IdentifierContext* ctx, BackEndContext* backend);

#endif
#endif
