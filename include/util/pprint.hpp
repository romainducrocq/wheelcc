#ifndef _UTIL_PPRINT_H
#define _UTIL_PPRINT_H

#ifndef __NDEBUG__
#include "util/c_std.hpp"

struct Token;
struct Ast;
struct CProgram;
struct TacProgram;
struct AsmProgram;
struct BackEndContext;
struct FrontEndContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

void pprint_toks(IdentifierContext* ctx, vector_t(Token) tokens);
void pprint_c_ast(IdentifierContext* ctx, CProgram* node);
void pprint_tac_ast(IdentifierContext* ctx, TacProgram* node);
void pprint_asm_ast(IdentifierContext* ctx, AsmProgram* node);
void pprint_ast(IdentifierContext* ctx, Ast* node, const char* name);
void pprint_addressed_set(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_string_const_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_struct_typedef_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_symbol_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_backend_symbol_table(IdentifierContext* ctx, BackEndContext* backend);

#endif
#endif
