#ifndef _ASSEMBLY_STACK_FIX_HPP
#define _ASSEMBLY_STACK_FIX_HPP

#include "ast/ast.hpp"
#include "ast/asm_ast.hpp"

#include <memory>

std::unique_ptr<AsmBinary> deallocate_stack_bytes(TInt byte);
std::unique_ptr<AsmBinary> allocate_stack_bytes(TInt byte);
void fix_stack(AsmProgram* node);

#endif
