#ifndef _BACK_ASSEMBLY_SYMT_CVT_H
#define _BACK_ASSEMBLY_SYMT_CVT_H

#include <memory>

#include "ast_t.h" // ast

typedef struct Type Type;
typedef struct AssemblyType AssemblyType;
typedef struct AsmProgram AsmProgram;
typedef struct BackEndContext BackEndContext;
typedef struct FrontEndContext FrontEndContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

TInt gen_type_alignment(FrontEndContext* ctx, Type* type);
std::shared_ptr<AssemblyType> cvt_backend_asm_type(FrontEndContext* ctx, TIdentifier name);
void convert_symbol_table(AsmProgram* node, BackEndContext* backend, FrontEndContext* frontend);

#endif
