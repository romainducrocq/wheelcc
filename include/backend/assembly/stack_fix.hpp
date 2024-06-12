#ifndef _BACKEND_ASSEMBLY_STACK_FIX_HPP
#define _BACKEND_ASSEMBLY_STACK_FIX_HPP

#include <memory>
#include <unordered_map>
#include <vector>

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stack fix

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pseudo register replacement
// Instruction fix up

struct StackFixContext {
    StackFixContext();

    // Pseudo register replacement
    TLong stack_bytes;
    std::unordered_map<TIdentifier, TLong> pseudo_stack_bytes_map;
    // Instruction fix up
    std::vector<std::unique_ptr<AsmInstruction>>* p_fix_instructions;
};

std::unique_ptr<AsmBinary> deallocate_stack_bytes(TLong byte);
std::unique_ptr<AsmBinary> allocate_stack_bytes(TLong byte);
void fix_stack(AsmProgram* node);

#endif
