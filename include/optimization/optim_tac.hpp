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

struct UnreachableCode {
    std::vector<bool> reachable_blocks;
};

struct CopyPropagation {
    // queue that starts with all block ids and pushes new open blocks at the end
    // -> need to keep track of current position locally
    std::vector<size_t> open_block_ids;
    // size = (all copies)
    // list of copy instruction indices
    std::vector<size_t> all_copy_indices;
    // number of blocks that can be modified by a reaching copy (all except removed)
    size_t reaching_copy_block_set_size;
    // number of instructions that can be modified by a reaching copy (part 1 = copy, unary, binary)
    size_t reaching_copy_instruction_set_size;
    // size = context->control_flow_graph->bloks.size()
    // indices of 1D reaching copy block flat mask by block indices
    std::vector<size_t> reaching_copy_block_by_indices;
    // size = context->p_instructions->size()
    // indices of 1D reaching copy instruction flat mask by instruction indices
    std::vector<size_t> reaching_copy_instruction_by_indices;
    // size = (reaching_copy_block_set_size) * (all_copy_indices.size())
    // flat array of boolean masks on all copy indices for all blocks that can be modified by a reaching copy
    // example: reaching_copy_block_flatmasks[j * n + i]:
    //   i element of mask array for block j = reaching_copy_block_by_indices[instruction_index],
    //    with n = size of all_copy_indices and i in [0, reaching_copy_block_set_size[
    std::vector<bool> reaching_copy_block_flat_sets;
    // size = (reaching_copy_instruction_set_size) * (all_copy_indices.size())
    // flat array of boolean masks on all copy indices for all instructions that can be modified by a reaching copy
    // example: reaching_copy_instruction_flatmasks[j * n + i]
    //   i element of mask array for instruction j = reaching_copy_instruction_by_indices[instruction_index],
    //   with n = size of all_copy_indices and i in [0, reaching_copy_instruction_set_size[
    std::vector<bool> reaching_copy_instruction_flat_sets;
};

struct OptimTacContext {
    OptimTacContext(uint8_t optim_1_mask);

    bool is_fixed_point;
    std::array<bool, 5> enabled_optimizations;
    std::unique_ptr<ControlFlowGraph> control_flow_graph;
    std::vector<std::unique_ptr<TacInstruction>>* p_instructions;
    // Constant folding
    // Copy propagation
    std::unique_ptr<CopyPropagation> copy_propagation;
    // Unreachable code elimination
    std::unique_ptr<UnreachableCode> unreachable_code;
    // Dead store elimination
};

void three_address_code_optimization(TacProgram* node, uint8_t optim_1_mask);

#endif
