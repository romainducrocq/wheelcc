#ifndef _BACKEND_ASSEMBLY_STACK_FIX_HPP
#define _BACKEND_ASSEMBLY_STACK_FIX_HPP

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"

#include <memory>

std::unique_ptr<AsmBinary> deallocate_stack_bytes(TULong byte);
std::unique_ptr<AsmBinary> allocate_stack_bytes(TULong byte);
void fix_stack(AsmProgram* node);

#endif
