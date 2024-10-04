#include <memory>

#include "util/throw.hpp"

#include "ast/interm_ast.hpp"

#include "optimization/optim_tac.hpp"

static std::unique_ptr<OptimTacContext> context;

OptimTacContext::OptimTacContext(int32_t optim_1_mask) :
    is_fixed_point(true), enabled_optimizations({
                              (optim_1_mask & 2 << 0) > 0, // Constant folding
                              (optim_1_mask & 2 << 1) > 0, // Copy propagation
                              (optim_1_mask & 2 << 2) > 0, // Unreachable code elimination
                              (optim_1_mask & 2 << 3) > 0  // Dead store elimination
                          }) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code optimization

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding

static void fold_constants_list_instructions(const std::vector<std::unique_ptr<TacInstruction>>& list_node) {
    for (const auto& instruction : list_node) {
        // TODO
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy propagation

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unreachable code elimination

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Dead store elimination

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void optimize_function_top_level(TacFunction* node) {
    context->is_fixed_point = true;
    do {
        if (context->enabled_optimizations[0]) {
            fold_constants_list_instructions(node->body);
        }
        if (context->enabled_optimizations[1]) {
        }
        if (context->enabled_optimizations[2]) {
        }
        if (context->enabled_optimizations[3]) {
        }
    }
    while (!context->is_fixed_point);
}

static void optimize_top_level(TacTopLevel* node) {
    switch (node->type()) {
        case AST_T::TacFunction_t:
            optimize_function_top_level(static_cast<TacFunction*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void optimize_program(TacProgram* node) {
    for (const auto& top_level : node->function_top_levels) {
        optimize_top_level(top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void three_address_code_optimization(TacProgram* node, int32_t optim_1_mask) {
    context = std::make_unique<OptimTacContext>(optim_1_mask);
    optimize_program(node);
    context.reset();
}
