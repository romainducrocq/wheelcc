#ifndef _UTIL_PPRINT_H
#define _UTIL_PPRINT_H

#ifndef __NDEBUG__
#include "util/c_std.hpp"

typedef struct Token Token;
typedef struct Ast Ast;
typedef struct CProgram CProgram;
typedef struct TacProgram TacProgram;
typedef struct AsmProgram AsmProgram;
typedef struct BackEndContext BackEndContext;
typedef struct FrontEndContext FrontEndContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

void pprint_toks(IdentifierContext* ctx, vector_t(Token) tokens);
void pprint_c_ast(IdentifierContext* ctx, CProgram* node);
void pprint_tac_ast(IdentifierContext* ctx, TacProgram* node);
void pprint_asm_ast(IdentifierContext* ctx, AsmProgram* node);
void pprint_addressed_set(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_string_const_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_struct_typedef_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_symbol_table(IdentifierContext* ctx, FrontEndContext* frontend);
void pprint_backend_symbol_table(IdentifierContext* ctx, BackEndContext* backend);

#endif
#endif
