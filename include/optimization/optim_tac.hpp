#ifndef _OPTIMIZATION_OPTIM_TAC_HPP
#define _OPTIMIZATION_OPTIM_TAC_HPP

#include <array>
#include <inttypes.h>
#include <memory>
#include <vector>

#include "ast/interm_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code optimization

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding
// Copy propagation
// Unreachable code elimination
// Dead store elimination

struct OptimTacContext {
    OptimTacContext(uint8_t optim_1_mask);

    bool is_fixed_point;
    std::array<bool, 5> enabled_optimizations;
    // Constant folding
    size_t instruction_index;
    std::vector<std::unique_ptr<TacInstruction>>* p_instructions;
    // Copy propagation
    // Unreachable code elimination
    // Dead store elimination
};

void three_address_code_optimization(TacProgram* node, uint8_t optim_1_mask);

#endif
