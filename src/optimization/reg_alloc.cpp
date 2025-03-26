#include <algorithm>
#include <inttypes.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/front_symt.hpp"

#include "optimization/reg_alloc.hpp"

struct ControlFlowGraph;
struct DataFlowAnalysis;

struct RegAllocContext {
    RegAllocContext(uint8_t optim_2_code);

    // Register allocation
    // Register coalescing
    bool is_with_coalescing;
    std::unique_ptr<ControlFlowGraph> control_flow_graph;
    std::unique_ptr<DataFlowAnalysis> data_flow_analysis;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instructions;
};

RegAllocContext::RegAllocContext(uint8_t optim_2_code) :
    is_with_coalescing(optim_2_code > 1u) // With register coalescing
{}

static std::unique_ptr<RegAllocContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

#ifndef __OPTIM_LEVEL__
#define __OPTIM_LEVEL__ 2
#undef _OPTIMIZATION_CFG_IMPL_HPP
#include "optimization/cfg_impl.hpp"
#undef __OPTIM_LEVEL__
#endif

static void set_instruction(std::unique_ptr<AsmInstruction>&& instruction, size_t instruction_index) {
    if (instruction) {
        GET_INSTRUCTION(instruction_index) = std::move(instruction);
    }
    else {
        GET_INSTRUCTION(instruction_index).reset();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

static void regalloc_inference_graph() {
    // TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register coalescing

static void coalesce_list_instructions() {
    // TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void regalloc_function_top_level(AsmFunction* node) {
    context->p_instructions = &node->instructions;
    regalloc_inference_graph();
    if (context->is_with_coalescing) {
        coalesce_list_instructions();
    }
    context->p_instructions = nullptr;
}

static void regalloc_top_level(AsmTopLevel* node) {
    switch (node->type()) {
        case AST_T::AsmFunction_t:
            regalloc_function_top_level(static_cast<AsmFunction*>(node));
            break;
        case AST_T::AsmStaticVariable_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void regalloc_program(AsmProgram* node) {
    for (const auto& top_level : node->top_levels) {
        regalloc_top_level(top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void register_allocation(AsmProgram* node, uint8_t optim_2_code) {
    context = std::make_unique<RegAllocContext>(optim_2_code);
    context->control_flow_graph = std::make_unique<ControlFlowGraph>();
    context->data_flow_analysis = std::make_unique<DataFlowAnalysis>();
    regalloc_program(node);
    context.reset();
}
