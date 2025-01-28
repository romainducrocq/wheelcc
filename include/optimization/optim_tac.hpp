#ifndef _OPTIMIZATION_OPTIM_TAC_HPP
#define _OPTIMIZATION_OPTIM_TAC_HPP

#include <array>
#include <inttypes.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>
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
    size_t size;
    size_t instructions_front_index;
    size_t instructions_back_index;
    std::vector<size_t> predecessor_ids;
    std::vector<size_t> successor_ids;
};

struct ControlFlowGraph {
    size_t entry_id;
    size_t exit_id;
    std::vector<size_t> entry_successor_ids;
    std::vector<size_t> exit_predecessor_ids;
    std::vector<ControlFlowBlock> blocks;
    std::unordered_map<std::string, size_t> label_id_map;
};

struct DataFlowAnalysis {
    size_t set_size;
    size_t incoming_index;
    std::vector<size_t> open_block_ids;
    // std::vector<size_t> redundant_data;
    std::vector<size_t> data_index_map;
    std::vector<size_t> block_index_map;
    std::vector<size_t> instruction_index_map;
    std::vector<bool> blocks_flat_sets;
    std::vector<bool> instructions_flat_sets;
};

struct OptimTacContext {
    OptimTacContext(uint8_t optim_1_mask);

    bool is_fixed_point;
    std::array<bool, 5> enabled_optimizations;
    std::unique_ptr<ControlFlowGraph> control_flow_graph;
    std::unique_ptr<DataFlowAnalysis> data_flow_analysis;
    std::vector<std::unique_ptr<TacInstruction>>* p_instructions;
    // Constant folding
    // Copy propagation
    std::unique_ptr<std::vector<bool>> reaching_code;
    std::unique_ptr<std::vector<std::unique_ptr<TacInstruction>>> bak_instructions;
    // Unreachable code elimination
    std::unique_ptr<std::vector<bool>> reachable_blocks;
    // Dead store elimination
};

void three_address_code_optimization(TacProgram* node, uint8_t optim_1_mask);

#endif
