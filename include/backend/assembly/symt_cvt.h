#ifndef _BACK_ASSEMBLY_SYMT_CVT_H
#define _BACK_ASSEMBLY_SYMT_CVT_H

#include "util/c_std.h"

#include "ast_t.h" // ast

typedef struct Type Type;
typedef struct AssemblyType AssemblyType;
typedef struct AsmProgram AsmProgram;
typedef struct BackEndContext BackEndContext;
typedef struct FrontEndContext FrontEndContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

#ifdef __cplusplus
extern "C" {
#endif
TInt gen_type_alignment(FrontEndContext* ctx, const Type* type);
shared_ptr_t(AssemblyType) cvt_backend_asm_type(FrontEndContext* ctx, TIdentifier name);
void convert_symbol_table(const AsmProgram* node, BackEndContext* backend, FrontEndContext* frontend);
#ifdef __cplusplus
}
#endif

#endif
