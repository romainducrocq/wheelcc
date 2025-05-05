#ifndef _BACKEND_ASSEMBLY_STACK_FIX_HPP
#define _BACKEND_ASSEMBLY_STACK_FIX_HPP

#include <memory>

#include "ast_t.hpp" // ast

struct AsmBinary;
struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement
// Instruction fix up

std::unique_ptr<AsmBinary> alloc_stack_bytes(TLong byte);
std::unique_ptr<AsmBinary> dealloc_stack_bytes(TLong byte);
void fix_stack(AsmProgram* node);

#endif
