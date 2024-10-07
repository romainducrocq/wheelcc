#include <memory>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/interm_ast.hpp"

#include "optimization/optim_tac.hpp"

static std::unique_ptr<OptimTacContext> context;

OptimTacContext::OptimTacContext(uint8_t optim_1_mask) :
    is_fixed_point(true),
    enabled_optimizations({
        (optim_1_mask & (static_cast<uint8_t>(1) << 0)) > 0, // Enable constant folding
        (optim_1_mask & (static_cast<uint8_t>(1) << 1)) > 0, // Enable copy propagation
        (optim_1_mask & (static_cast<uint8_t>(1) << 2)) > 0, // Enable unreachable code elimination
        (optim_1_mask & (static_cast<uint8_t>(1) << 3)) > 0, // Enable dead store elimination
        (optim_1_mask & ~(static_cast<uint8_t>(1) << 0)) > 0 // Optimize with control flow graph
    }) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code optimization

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding

static void set_instruction(std::unique_ptr<TacInstruction>&& instruction) {
    if (instruction) {
        (*context->p_instructions)[context->instruction_index] = std::move(instruction);
    }
    else {
        (*context->p_instructions)[context->instruction_index].reset();
    }
}

static std::shared_ptr<TacConstant> fold_constants_unary_not_value(TacConstant* node) {
    std::shared_ptr<CConst> constant;
    switch (node->constant->type()) {
        case AST_T::CConstInt_t: {
            TInt value = !static_cast<CConstInt*>(node->constant.get())->value;
            constant = std::make_shared<CConstInt>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(constant));
}

static std::shared_ptr<TacConstant> fold_constants_unary_complement_value(TacConstant* node) {
    std::shared_ptr<CConst> constant;
    switch (node->constant->type()) {
        case AST_T::CConstInt_t: {
            TInt value = ~static_cast<CConstInt*>(node->constant.get())->value;
            constant = std::make_shared<CConstInt>(std::move(value));
            break;
        }
        case AST_T::CConstLong_t: {
            TLong value = ~static_cast<CConstLong*>(node->constant.get())->value;
            constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        case AST_T::CConstUInt_t: {
            TUInt value = ~static_cast<CConstUInt*>(node->constant.get())->value;
            constant = std::make_shared<CConstUInt>(std::move(value));
            break;
        }
        case AST_T::CConstULong_t: {
            TULong value = ~static_cast<CConstULong*>(node->constant.get())->value;
            constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(constant));
}

static std::shared_ptr<TacConstant> fold_constants_unary_negate_value(TacConstant* node) {
    std::shared_ptr<CConst> constant;
    switch (node->constant->type()) {
        case AST_T::CConstInt_t: {
            TInt value = -static_cast<CConstInt*>(node->constant.get())->value;
            constant = std::make_shared<CConstInt>(std::move(value));
            break;
        }
        case AST_T::CConstLong_t: {
            TLong value = -static_cast<CConstLong*>(node->constant.get())->value;
            constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        case AST_T::CConstDouble_t: {
            TDouble value = -static_cast<CConstDouble*>(node->constant.get())->value;
            constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_T::CConstUInt_t: {
            TUInt value = -static_cast<CConstUInt*>(node->constant.get())->value;
            constant = std::make_shared<CConstUInt>(std::move(value));
            break;
        }
        case AST_T::CConstULong_t: {
            TULong value = -static_cast<CConstULong*>(node->constant.get())->value;
            constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(constant));
}

static std::shared_ptr<TacConstant> fold_constants_unary_value(TacUnary* node) {
    switch (node->unary_op->type()) {
        case AST_T::TacNot_t:
            return fold_constants_unary_not_value(static_cast<TacConstant*>(node->src.get()));
        case AST_T::TacComplement_t:
            return fold_constants_unary_complement_value(static_cast<TacConstant*>(node->src.get()));
        case AST_T::TacNegate_t:
            return fold_constants_unary_negate_value(static_cast<TacConstant*>(node->src.get()));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void fold_constants_unary_instructions(TacUnary* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        std::shared_ptr<TacValue> src = fold_constants_unary_value(node);
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
    }
}

static void fold_constants_binary_instructions(TacBinary* node) {
    // if (node->src1->type() == AST_T::TacConstant_t && node->src2->type() == AST_T::TacConstant_t) {

    // }
}

static void fold_constants_instructions(TacInstruction* node) {
    switch (node->type()) {
        case AST_T::TacSignExtend_t: {
            // fold_constants_sign_extend_instructions(static_cast<TacSignExtend*>(node)); // TODO
            break;
        }
        case AST_T::TacTruncate_t: {
            // fold_constants_truncate_instructions(static_cast<TacTruncate*>(node)); // TODO
            break;
        }
        case AST_T::TacZeroExtend_t: {
            // fold_constants_zero_extend_instructions(static_cast<TacZeroExtend*>(node)); // TODO
            break;
        }
        case AST_T::TacDoubleToInt_t: {
            // fold_constants_double_to_signed_instructions(static_cast<TacDoubleToInt*>(node)); // TODO
            break;
        }
        case AST_T::TacDoubleToUInt_t: {
            // fold_constants_double_to_unsigned_instructions(static_cast<TacDoubleToUInt*>(node)); // TODO
            break;
        }
        case AST_T::TacIntToDouble_t: {
            // fold_constants_signed_to_double_instructions(static_cast<TacIntToDouble*>(node)); // TODO
            break;
        }
        case AST_T::TacUIntToDouble_t: {
            // fold_constants_unsigned_to_double_instructions(static_cast<TacUIntToDouble*>(node)); // TODO
            break;
        }
        case AST_T::TacUnary_t: {
            fold_constants_unary_instructions(static_cast<TacUnary*>(node));
            break;
        }
        case AST_T::TacBinary_t: {
            fold_constants_binary_instructions(static_cast<TacBinary*>(node));
            break;
        }
        case AST_T::TacJumpIfZero_t: {
            // fold_constants_jump_if_zero_instructions(static_cast<TacJumpIfZero*>(node)); // TODO
            break;
        }
        case AST_T::TacJumpIfNotZero_t: {
            // fold_constants_jump_if_not_zero_instructions(static_cast<TacJumpIfNotZero*>(node)); // TODO
            break;
        }
        default:
            break;
    }
}

static void fold_constants_list_instructions(std::vector<std::unique_ptr<TacInstruction>>& list_node) {
    context->p_instructions = &list_node;
    for (context->instruction_index = 0; context->instruction_index < list_node.size(); ++context->instruction_index) {
        fold_constants_instructions((*context->p_instructions)[context->instruction_index].get());
    }
    context->p_instructions = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy propagation

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unreachable code elimination

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Dead store elimination

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CONSTANT_FOLDING 0
#define COPY_PROPAGATION 1
#define UNREACHABLE_CODE_ELIMINATION 2
#define DEAD_STORE_ELMININATION 3
#define CONTROL_FLOW_GRAPH 4

// #include <stdio.h> // TODO rm
static void optimize_function_top_level(TacFunction* node) {
    context->is_fixed_point = true;
    do {
        if (context->enabled_optimizations[CONSTANT_FOLDING]) {
            // printf("--fold-constants\n"); // TODO rm
            fold_constants_list_instructions(node->body);
        }
        if (context->enabled_optimizations[CONTROL_FLOW_GRAPH]) {
            if (context->enabled_optimizations[COPY_PROPAGATION]) {
                // printf("--propagate-copies\n"); // TODO rm
            }
            if (context->enabled_optimizations[UNREACHABLE_CODE_ELIMINATION]) {
                // printf("--eliminate-unreachable-code\n"); // TODO rm
            }
            if (context->enabled_optimizations[DEAD_STORE_ELMININATION]) {
                // printf("--eliminate-dead-stores\n"); // TODO rm
            }
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

void three_address_code_optimization(TacProgram* node, uint8_t optim_1_mask) {
    context = std::make_unique<OptimTacContext>(optim_1_mask);
    optimize_program(node);
    context.reset();
}
