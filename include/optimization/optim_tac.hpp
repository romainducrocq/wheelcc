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

struct ControlFlowBlock {
    size_t instructions_front_index;
    size_t instructions_size;
    std::vector<size_t> predecessor_ids;
    std::vector<size_t> successor_ids;
};

struct ControlFlowGraph {
    size_t entry_id;
    size_t exit_id;
    std::vector<size_t> entry_sucessor_ids;
    std::vector<size_t> exit_predecessor_ids;
    std::vector<ControlFlowBlock> blocks;
};

struct DataFlowAnalysis {
    ControlFlowGraph control_flow_graph;
};

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
    std::unique_ptr<DataFlowAnalysis> data_flow_analysis;
};

void three_address_code_optimization(TacProgram* node, uint8_t optim_1_mask);

#endif
