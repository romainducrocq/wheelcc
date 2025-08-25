#ifndef _BACK_ASSEMBLY_STACK_FIX_H
#define _BACK_ASSEMBLY_STACK_FIX_H

#include <memory>

#include "ast_t.h" // ast

typedef struct AsmBinary AsmBinary;
typedef struct AsmProgram AsmProgram;
typedef struct BackEndContext BackEndContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement
// Instruction fix up

std::unique_ptr<AsmBinary> alloc_stack_bytes(TLong byte);
void fix_stack(AsmProgram* node, BackEndContext* backend);

#endif
