#ifndef _BACK_ASSEMBLY_STACK_FIX_H
#define _BACK_ASSEMBLY_STACK_FIX_H

#include "util/c_std.h"

#include "ast_t.h" // ast

typedef struct AsmInstruction AsmInstruction;
typedef struct AsmProgram AsmProgram;
typedef struct BackEndContext BackEndContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement
// Instruction fix up

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmInstruction) alloc_stack_bytes(TLong byte);
void fix_stack(const AsmProgram* node, BackEndContext* backend);
#ifdef __cplusplus
}
#endif

#endif
