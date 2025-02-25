#ifndef _BACKEND_ASSEMBLY_STACK_FIX_HPP
#define _BACKEND_ASSEMBLY_STACK_FIX_HPP

#include <memory>
#include <string>

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement
// Instruction fix up

std::shared_ptr<AsmImm> generate_imm_operand(std::string&& value, bool is_byte, bool is_quad);
std::unique_ptr<AsmBinary> deallocate_stack_bytes(TLong byte);
std::unique_ptr<AsmBinary> allocate_stack_bytes(TLong byte);
void fix_stack(AsmProgram* node);

#endif
