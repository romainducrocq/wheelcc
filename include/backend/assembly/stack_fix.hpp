#ifndef _BACKEND_ASSEMBLY_STACK_FIX_HPP
#define _BACKEND_ASSEMBLY_STACK_FIX_HPP

#include <memory>

#include "ast/ast.hpp"

struct AsmBinary;
struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement
// Instruction fix up

std::unique_ptr<AsmBinary> allocate_stack_bytes(TLong byte);
std::unique_ptr<AsmBinary> deallocate_stack_bytes(TLong byte);
void fix_stack(AsmProgram* node);

#endif
