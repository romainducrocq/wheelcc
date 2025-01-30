#include <algorithm>
#include <inttypes.h>
#include <memory>
#include <unordered_set>
#include <vector>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"
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

#ifndef __OPTIM_LEVEL__
#define __OPTIM_LEVEL__ 1
#undef _OPTIMIZATION_CFG_IMPL_HPP
#include "optimization/cfg_impl.hpp"
#undef __OPTIM_LEVEL__
#endif

static void set_instruction(std::unique_ptr<TacInstruction>&& instruction, size_t instruction_index) {
    if (instruction) {
        GET_INSTRUCTION(instruction_index) = std::move(instruction);
    }
    else {
        GET_INSTRUCTION(instruction_index).reset();
    }
    context->is_fixed_point = false;
}

// // TODO rm
// #include "util/pprint.hpp"
// static void print_control_flow_graph() {
//     printf("\n\n----------------------------------------\nControlFlowGraph[%lu]:\n",
//         context->control_flow_graph->blocks.size());
//     printf("entry_id: %lu\n"
//            "exit_id: %lu\n"
//            "entry_successor_ids: ",
//         context->control_flow_graph->entry_id, context->control_flow_graph->exit_id);
//     for (size_t successor_id : context->control_flow_graph->entry_successor_ids) {
//         if (successor_id == context->control_flow_graph->entry_id) {
//             printf("ENTRY, ");
//         }
//         else if (successor_id == context->control_flow_graph->exit_id) {
//             printf("EXIT, ");
//         }
//         else {
//             printf("%lu, ", successor_id);
//         }
//     }
//     printf("\nexit_predecessor_ids: ");
//     for (size_t predecessor_id : context->control_flow_graph->exit_predecessor_ids) {
//         if (predecessor_id == context->control_flow_graph->entry_id) {
//             printf("ENTRY, ");
//         }
//         else if (predecessor_id == context->control_flow_graph->exit_id) {
//             printf("EXIT, ");
//         }
//         else {
//             printf("%lu, ", predecessor_id);
//         }
//     }
//     printf("\nblocks: \n");
//     for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
//         printf("\n--- ControlFlowBlock[%lu]\n"
//                "    size: %lu\n"
//                "    instructions_front_index: %lu\n"
//                "    instructions_back_index: %lu\n"
//                "    predecessor_ids: ",
//             block_id, GET_CFG_BLOCK(block_id).size, GET_CFG_BLOCK(block_id).instructions_front_index,
//             GET_CFG_BLOCK(block_id).instructions_back_index);
//         for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
//             if (predecessor_id == context->control_flow_graph->entry_id) {
//                 printf("ENTRY, ");
//             }
//             else if (predecessor_id == context->control_flow_graph->exit_id) {
//                 printf("EXIT, ");
//             }
//             else {
//                 printf("%lu, ", predecessor_id);
//             }
//         }
//         printf("\n    successor_ids: ");
//         for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
//             if (successor_id == context->control_flow_graph->entry_id) {
//                 printf("ENTRY, ");
//             }
//             else if (successor_id == context->control_flow_graph->exit_id) {
//                 printf("EXIT, ");
//             }
//             else {
//                 printf("%lu, ", successor_id);
//             }
//         }
//         printf("\n    instructions: \n");
//         std::unique_ptr<TacTopLevel> print_ast;
//         {
//             std::vector<std::unique_ptr<TacInstruction>> print_instructions;
//             print_instructions.reserve(
//                 GET_CFG_BLOCK(block_id).instructions_back_index - GET_CFG_BLOCK(block_id).instructions_front_index +
//                 1);
//             for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
//                  instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
//                 print_instructions.push_back(std::move(GET_INSTRUCTION(instruction_index)));
//             }
//             print_ast = std::make_unique<TacFunction>("CFG BLOCK ID " + std::to_string(block_id), false,
//                 std::vector<std::string>(0), std::move(print_instructions));
//         }
//         pretty_print_ast(print_ast.get(), std::to_string(block_id));
//         {
//             std::vector<std::unique_ptr<TacInstruction>>& print_instructions =
//                 static_cast<TacFunction*>(print_ast.get())->body;
//             for (size_t instruction_index = 0; instruction_index < print_instructions.size(); ++instruction_index) {
//                 GET_INSTRUCTION(GET_CFG_BLOCK(block_id).instructions_front_index + instruction_index) =
//                     std::move(print_instructions[instruction_index]);
//             }
//         }
//     }
//     // RAISE_INTERNAL_ERROR;
// }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding

static std::shared_ptr<CConst> fold_constants_sign_extend_int_constant(TacVariable* node, CConstInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Long_t:
        case AST_T::Pointer_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_constants_sign_extend_constant_value(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstInt_t: {
            fold_constant = fold_constants_sign_extend_int_constant(node, static_cast<CConstInt*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_sign_extend_instructions(TacSignExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_sign_extend_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_constants_truncate_int_constant(TacVariable* node, CConstInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_T::UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_truncate_long_constant(TacVariable* node, CConstLong* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_T::Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        case AST_T::UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_truncate_uint_constant(TacVariable* node, CConstUInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_T::UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_truncate_ulong_constant(TacVariable* node, CConstULong* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_T::Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        case AST_T::UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_constants_truncate_constant_value(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstInt_t: {
            fold_constant = fold_constants_truncate_int_constant(node, static_cast<CConstInt*>(constant));
            break;
        }
        case AST_T::CConstLong_t: {
            fold_constant = fold_constants_truncate_long_constant(node, static_cast<CConstLong*>(constant));
            break;
        }
        case AST_T::CConstUInt_t: {
            fold_constant = fold_constants_truncate_uint_constant(node, static_cast<CConstUInt*>(constant));
            break;
        }
        case AST_T::CConstULong_t: {
            fold_constant = fold_constants_truncate_ulong_constant(node, static_cast<CConstULong*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_truncate_instructions(TacTruncate* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_truncate_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_constants_zero_extend_uint_constant(TacVariable* node, CConstUInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Long_t:
        case AST_T::Pointer_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_constants_zero_extend_constant_value(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstUInt_t: {
            fold_constant = fold_constants_zero_extend_uint_constant(node, static_cast<CConstUInt*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_zero_extend_instructions(TacZeroExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_zero_extend_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_constants_double_to_signed_constant_value(
    TacVariable* node, CConst* constant) {
    if (constant->type() != AST_T::CConstDouble_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t: {
            TChar value = static_cast<TChar>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstChar>(std::move(value));
            break;
        }
        case AST_T::Int_t: {
            TInt value = static_cast<TInt>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstInt>(std::move(value));
            break;
        }
        case AST_T::Long_t: {
            TLong value = static_cast<TLong>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_double_to_signed_instructions(TacDoubleToInt* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_double_to_signed_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_constants_double_to_unsigned_constant_value(
    TacVariable* node, CConst* constant) {
    if (constant->type() != AST_T::CConstDouble_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::UChar_t: {
            TUChar value = static_cast<TUChar>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstUChar>(std::move(value));
            break;
        }
        case AST_T::UInt_t: {
            TUInt value = static_cast<TUInt>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstUInt>(std::move(value));
            break;
        }
        case AST_T::ULong_t: {
            TULong value = static_cast<TULong>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_double_to_unsigned_instructions(TacDoubleToUInt* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_double_to_unsigned_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_constants_signed_to_double_constant_value(
    TacVariable* node, CConst* constant) {
    if (frontend->symbol_table[node->name]->type_t->type() != AST_T::Double_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstInt_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstInt*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_T::CConstLong_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstLong*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_signed_to_double_instructions(TacIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_signed_to_double_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_constants_unsigned_to_double_constant_value(
    TacVariable* node, CConst* constant) {
    if (frontend->symbol_table[node->name]->type_t->type() != AST_T::Double_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstUInt_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstUInt*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_T::CConstULong_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstULong*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_unsigned_to_double_instructions(TacUIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_unsigned_to_double_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_constants_unary_int_constant(TacUnaryOp* node, CConstInt* constant) {
    switch (node->type()) {
        case AST_T::TacComplement_t: {
            TInt value = ~constant->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacNegate_t: {
            TInt value = -constant->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_unary_long_constant(TacUnaryOp* node, CConstLong* constant) {
    switch (node->type()) {
        case AST_T::TacComplement_t: {
            TLong value = ~constant->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacNegate_t: {
            TLong value = -constant->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_unary_double_constant(TacUnaryOp* node, CConstDouble* constant) {
    switch (node->type()) {
        case AST_T::TacNegate_t: {
            TDouble value = -constant->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_T::TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_unary_uint_constant(TacUnaryOp* node, CConstUInt* constant) {
    switch (node->type()) {
        case AST_T::TacComplement_t: {
            TUInt value = ~constant->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacNegate_t: {
            TUInt value = -constant->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_unary_ulong_constant(TacUnaryOp* node, CConstULong* constant) {
    switch (node->type()) {
        case AST_T::TacComplement_t: {
            TULong value = ~constant->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacNegate_t: {
            TULong value = -constant->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_constants_unary_constant_value(TacUnaryOp* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstInt_t: {
            fold_constant = fold_constants_unary_int_constant(node, static_cast<CConstInt*>(constant));
            break;
        }
        case AST_T::CConstLong_t: {
            fold_constant = fold_constants_unary_long_constant(node, static_cast<CConstLong*>(constant));
            break;
        }
        case AST_T::CConstDouble_t: {
            fold_constant = fold_constants_unary_double_constant(node, static_cast<CConstDouble*>(constant));
            break;
        }
        case AST_T::CConstUInt_t: {
            fold_constant = fold_constants_unary_uint_constant(node, static_cast<CConstUInt*>(constant));
            break;
        }
        case AST_T::CConstULong_t: {
            fold_constant = fold_constants_unary_ulong_constant(node, static_cast<CConstULong*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_unary_instructions(TacUnary* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacConstant_t) {
        std::shared_ptr<TacValue> src = fold_constants_unary_constant_value(
            node->unary_op.get(), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_constants_binary_int_constant(
    TacBinaryOp* node, CConstInt* constant_1, CConstInt* constant_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t: {
            TInt value = constant_1->value + constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacSubtract_t: {
            TInt value = constant_1->value - constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacMultiply_t: {
            TInt value = constant_1->value * constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacDivide_t: {
            TInt value = constant_2->value != 0 ? constant_1->value / constant_2->value : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacRemainder_t: {
            TInt value = constant_2->value != 0 ? constant_1->value % constant_2->value : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacBitAnd_t: {
            TInt value = constant_1->value & constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacBitOr_t: {
            TInt value = constant_1->value | constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacBitXor_t: {
            TInt value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacBitShiftLeft_t: {
            TInt value = constant_1->value << constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t: {
            TInt value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_binary_long_constant(
    TacBinaryOp* node, CConstLong* constant_1, CConstLong* constant_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t: {
            TLong value = constant_1->value + constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacSubtract_t: {
            TLong value = constant_1->value - constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacMultiply_t: {
            TLong value = constant_1->value * constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacDivide_t: {
            TLong value = constant_2->value != 0l ? constant_1->value / constant_2->value : 0l;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacRemainder_t: {
            TLong value = constant_2->value != 0l ? constant_1->value % constant_2->value : 0l;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacBitAnd_t: {
            TLong value = constant_1->value & constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacBitOr_t: {
            TLong value = constant_1->value | constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacBitXor_t: {
            TLong value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacBitShiftLeft_t: {
            TLong value = constant_1->value << constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t: {
            TLong value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_T::TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_binary_double_constant(
    TacBinaryOp* node, CConstDouble* constant_1, CConstDouble* constant_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t: {
            TDouble value = constant_1->value + constant_2->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_T::TacSubtract_t: {
            TDouble value = constant_1->value - constant_2->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_T::TacMultiply_t: {
            TDouble value = constant_1->value * constant_2->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_T::TacDivide_t: {
            TDouble value = constant_2->value != 0.0 ? constant_1->value / constant_2->value : 0.0 / 0.0;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_T::TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_binary_uint_constant(
    TacBinaryOp* node, CConstUInt* constant_1, CConstUInt* constant_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t: {
            TUInt value = constant_1->value + constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacSubtract_t: {
            TUInt value = constant_1->value - constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacMultiply_t: {
            TUInt value = constant_1->value * constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacDivide_t: {
            TUInt value = constant_2->value != 0u ? constant_1->value / constant_2->value : 0u;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacRemainder_t: {
            TUInt value = constant_2->value != 0u ? constant_1->value % constant_2->value : 0u;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacBitAnd_t: {
            TUInt value = constant_1->value & constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacBitOr_t: {
            TUInt value = constant_1->value | constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacBitXor_t: {
            TUInt value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacBitShiftLeft_t: {
            TUInt value = constant_1->value << constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t: {
            TUInt value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_T::TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_constants_binary_ulong_constant(
    TacBinaryOp* node, CConstULong* constant_1, CConstULong* constant_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t: {
            TULong value = constant_1->value + constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacSubtract_t: {
            TULong value = constant_1->value - constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacMultiply_t: {
            TULong value = constant_1->value * constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacDivide_t: {
            TULong value = constant_2->value != 0ul ? constant_1->value / constant_2->value : 0ul;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacRemainder_t: {
            TULong value = constant_2->value != 0ul ? constant_1->value % constant_2->value : 0ul;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacBitAnd_t: {
            TULong value = constant_1->value & constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacBitOr_t: {
            TULong value = constant_1->value | constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacBitXor_t: {
            TULong value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacBitShiftLeft_t: {
            TULong value = constant_1->value << constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t: {
            TULong value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_T::TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_T::TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_constants_binary_constant_value(
    TacBinaryOp* node, CConst* constant_1, CConst* constant_2) {
    std::shared_ptr<CConst> fold_constant;
    if (constant_1->type() != constant_2->type()) {
        RAISE_INTERNAL_ERROR;
    }
    switch (constant_1->type()) {
        case AST_T::CConstInt_t: {
            fold_constant = fold_constants_binary_int_constant(
                node, static_cast<CConstInt*>(constant_1), static_cast<CConstInt*>(constant_2));
            break;
        }
        case AST_T::CConstLong_t: {
            fold_constant = fold_constants_binary_long_constant(
                node, static_cast<CConstLong*>(constant_1), static_cast<CConstLong*>(constant_2));
            break;
        }
        case AST_T::CConstDouble_t: {
            fold_constant = fold_constants_binary_double_constant(
                node, static_cast<CConstDouble*>(constant_1), static_cast<CConstDouble*>(constant_2));
            break;
        }
        case AST_T::CConstUInt_t: {
            fold_constant = fold_constants_binary_uint_constant(
                node, static_cast<CConstUInt*>(constant_1), static_cast<CConstUInt*>(constant_2));
            break;
        }
        case AST_T::CConstULong_t: {
            fold_constant = fold_constants_binary_ulong_constant(
                node, static_cast<CConstULong*>(constant_1), static_cast<CConstULong*>(constant_2));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_constants_binary_instructions(TacBinary* node, size_t instruction_index) {
    if (node->src1->type() == AST_T::TacConstant_t && node->src2->type() == AST_T::TacConstant_t) {
        std::shared_ptr<TacValue> src = fold_constants_binary_constant_value(node->binary_op.get(),
            static_cast<TacConstant*>(node->src1.get())->constant.get(),
            static_cast<TacConstant*>(node->src2.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static bool fold_constants_is_zero_constant_value(CConst* constant) {
    switch (constant->type()) {
        case AST_T::CConstInt_t:
            return static_cast<CConstInt*>(constant)->value == 0;
        case AST_T::CConstLong_t:
            return static_cast<CConstLong*>(constant)->value == 0l;
        case AST_T::CConstDouble_t:
            return static_cast<CConstDouble*>(constant)->value == 0.0;
        case AST_T::CConstUInt_t:
            return static_cast<CConstUInt*>(constant)->value == 0u;
        case AST_T::CConstULong_t:
            return static_cast<CConstULong*>(constant)->value == 0ul;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void fold_constants_jump_if_zero_instructions(TacJumpIfZero* node, size_t instruction_index) {
    if (node->condition->type() == AST_T::TacConstant_t) {
        if (fold_constants_is_zero_constant_value(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            TIdentifier target = node->target;
            set_instruction(std::make_unique<TacJump>(std::move(target)), instruction_index);
        }
        else {
            set_instruction(nullptr, instruction_index);
        }
    }
}

static void fold_constants_jump_if_not_zero_instructions(TacJumpIfNotZero* node, size_t instruction_index) {
    if (node->condition->type() == AST_T::TacConstant_t) {
        if (fold_constants_is_zero_constant_value(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            set_instruction(nullptr, instruction_index);
        }
        else {
            TIdentifier target = node->target;
            set_instruction(std::make_unique<TacJump>(std::move(target)), instruction_index);
        }
    }
}

static void fold_constants_instructions(TacInstruction* node, size_t instruction_index) {
    switch (node->type()) {
        case AST_T::TacSignExtend_t:
            fold_constants_sign_extend_instructions(static_cast<TacSignExtend*>(node), instruction_index);
            break;
        case AST_T::TacTruncate_t:
            fold_constants_truncate_instructions(static_cast<TacTruncate*>(node), instruction_index);
            break;
        case AST_T::TacZeroExtend_t:
            fold_constants_zero_extend_instructions(static_cast<TacZeroExtend*>(node), instruction_index);
            break;
        case AST_T::TacDoubleToInt_t:
            fold_constants_double_to_signed_instructions(static_cast<TacDoubleToInt*>(node), instruction_index);
            break;
        case AST_T::TacDoubleToUInt_t:
            fold_constants_double_to_unsigned_instructions(static_cast<TacDoubleToUInt*>(node), instruction_index);
            break;
        case AST_T::TacIntToDouble_t:
            fold_constants_signed_to_double_instructions(static_cast<TacIntToDouble*>(node), instruction_index);
            break;
        case AST_T::TacUIntToDouble_t:
            fold_constants_unsigned_to_double_instructions(static_cast<TacUIntToDouble*>(node), instruction_index);
            break;
        case AST_T::TacUnary_t:
            fold_constants_unary_instructions(static_cast<TacUnary*>(node), instruction_index);
            break;
        case AST_T::TacBinary_t:
            fold_constants_binary_instructions(static_cast<TacBinary*>(node), instruction_index);
            break;
        case AST_T::TacJumpIfZero_t:
            fold_constants_jump_if_zero_instructions(static_cast<TacJumpIfZero*>(node), instruction_index);
            break;
        case AST_T::TacJumpIfNotZero_t:
            fold_constants_jump_if_not_zero_instructions(static_cast<TacJumpIfNotZero*>(node), instruction_index);
            break;
        default:
            break;
    }
}

static void fold_constants_list_instructions() {
    for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
        if (GET_INSTRUCTION(instruction_index)) {
            fold_constants_instructions(GET_INSTRUCTION(instruction_index).get(), instruction_index);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unreachable code elimination

static void eliminate_unreachable_code_reachable_block(size_t block_id);

static void eliminate_unreachable_code_successor_reachable_blocks(size_t block_id) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        eliminate_unreachable_code_reachable_block(successor_id);
    }
}

static void eliminate_unreachable_code_reachable_block(size_t block_id) {
    if (block_id < context->control_flow_graph->exit_id && !context->control_flow_graph->reaching_code[block_id]) {
        context->control_flow_graph->reaching_code[block_id] = true;
        eliminate_unreachable_code_successor_reachable_blocks(block_id);
    }
}

static void eliminate_unreachable_code_empty_block(size_t block_id) {
    for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
         instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
        if (GET_INSTRUCTION(instruction_index)) {
            set_instruction(nullptr, instruction_index);
        }
    }
    GET_CFG_BLOCK(block_id).size = 0;
    control_flow_graph_remove_empty_block(block_id, false);
    GET_CFG_BLOCK(block_id).successor_ids.clear();
    GET_CFG_BLOCK(block_id).predecessor_ids.clear();
}

static void eliminate_unreachable_code_jump_instructions(size_t block_id) {
    TacInstruction* node = GET_INSTRUCTION(GET_CFG_BLOCK(block_id).instructions_back_index).get();
    switch (node->type()) {
        case AST_T::TacJump_t:
        case AST_T::TacJumpIfZero_t:
        case AST_T::TacJumpIfNotZero_t:
            control_flow_graph_remove_block_instruction(GET_CFG_BLOCK(block_id).instructions_back_index, block_id);
            break;
        default:
            break;
    }
}

static void eliminate_unreachable_code_jump_block(size_t block_id, size_t next_block_id) {
    if (GET_CFG_BLOCK(block_id).successor_ids.size() == 1
        && GET_CFG_BLOCK(block_id).successor_ids[0] == next_block_id) {
        eliminate_unreachable_code_jump_instructions(block_id);
    }
}

static void eliminate_unreachable_code_label_instructions(size_t block_id) {
    TacInstruction* node = GET_INSTRUCTION(GET_CFG_BLOCK(block_id).instructions_front_index).get();
    if (node->type() != AST_T::TacLabel_t) {
        RAISE_INTERNAL_ERROR;
    }
    control_flow_graph_remove_block_instruction(GET_CFG_BLOCK(block_id).instructions_front_index, block_id);
}

static void eliminate_unreachable_code_label_block(size_t block_id, size_t previous_block_id) {
    if (GET_CFG_BLOCK(block_id).predecessor_ids.size() == 1
        && GET_CFG_BLOCK(block_id).predecessor_ids[0] == previous_block_id) {
        eliminate_unreachable_code_label_instructions(block_id);
    }
}

static void eliminate_unreachable_code_control_flow_graph() {
    if (context->control_flow_graph->reaching_code.size() < context->control_flow_graph->blocks.size()) {
        context->control_flow_graph->reaching_code.resize(context->control_flow_graph->blocks.size());
    }
    std::fill(context->control_flow_graph->reaching_code.begin(),
        context->control_flow_graph->reaching_code.begin() + context->control_flow_graph->blocks.size(), false);
    for (size_t successor_id : context->control_flow_graph->entry_successor_ids) {
        eliminate_unreachable_code_reachable_block(successor_id);
    }

    size_t block_id = context->control_flow_graph->blocks.size();
    size_t next_block_id = context->control_flow_graph->exit_id;
    while (block_id-- > 0) {
        if (context->control_flow_graph->reaching_code[block_id]) {
            next_block_id = block_id;
            break;
        }
        else {
            eliminate_unreachable_code_empty_block(block_id);
        }
    }
    while (block_id-- > 0) {
        if (context->control_flow_graph->reaching_code[block_id]) {
            eliminate_unreachable_code_jump_block(block_id, next_block_id);
            next_block_id = block_id;
        }
        else {
            eliminate_unreachable_code_empty_block(block_id);
        }
    }

    for (auto& label_id : context->control_flow_graph->label_id_map) {
        if (context->control_flow_graph->reaching_code[label_id.second]) {
            for (block_id = label_id.second; block_id-- > 0;) {
                if (context->control_flow_graph->reaching_code[block_id]) {
                    next_block_id = block_id;
                    goto Lelse;
                }
            }
            next_block_id = context->control_flow_graph->entry_id;
        Lelse:
            eliminate_unreachable_code_label_block(label_id.second, next_block_id);
        }
        else {
            label_id.second = context->control_flow_graph->exit_id;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy propagation

// TODO

#define GET_DFA_INSTRUCTION(X) GET_INSTRUCTION(context->data_flow_analysis->data_index_map[X])

#define GET_DFA_BLOCK_SET_INDEX(X, Y) \
    context->data_flow_analysis->block_index_map[X] * context->data_flow_analysis->set_size + (Y)
#define GET_DFA_INSTRUCTION_SET_INDEX(X, Y) \
    context->data_flow_analysis->instruction_index_map[X] * context->data_flow_analysis->set_size + (Y)

#define GET_DFA_BLOCK_SET_AT(X, Y) context->data_flow_analysis->blocks_flat_sets[GET_DFA_BLOCK_SET_INDEX(X, Y)]
#define GET_DFA_INSTRUCTION_SET_AT(X, Y) \
    context->data_flow_analysis->instructions_flat_sets[GET_DFA_INSTRUCTION_SET_INDEX(X, Y)]

#define GET_DFA_INSTRUCTION_SET_RANGE(X)                                                                  \
    context->data_flow_analysis->instructions_flat_sets.begin() + GET_DFA_INSTRUCTION_SET_INDEX(X, 0),    \
        context->data_flow_analysis->instructions_flat_sets.begin() + GET_DFA_INSTRUCTION_SET_INDEX(X, 0) \
            + context->data_flow_analysis->set_size

// TODO remove
// static TIdentifier func_name = "";

// static void print_copy_propagation() {
//     if (func_name.compare("target") != 0) {
//         return;
//     }
//     size_t blocks_flat_sets_size = 0;
//     size_t instructions_flat_sets_size = 0;
//     for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
//         if (GET_CFG_BLOCK(block_id).size > 0) {
//             blocks_flat_sets_size++;

//             for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
//                  instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
//                 if (GET_INSTRUCTION(instruction_index)) {
//                     switch (GET_INSTRUCTION(instruction_index)->type()) {
//                         case AST_T::TacFunCall_t:
//                         case AST_T::TacUnary_t:
//                         case AST_T::TacBinary_t: {
//                             instructions_flat_sets_size++;
//                             break;
//                         }
//                         case AST_T::TacCopy_t: {
//                             instructions_flat_sets_size++;
//                             break;
//                         }
//                         default:
//                             break;
//                     }
//                 }
//             }
//         }
//     }
//     instructions_flat_sets_size++;

//     printf("blocks_flat_sets_size: (%zu, %zu)\n", blocks_flat_sets_size, context->data_flow_analysis->set_size);
//     for (size_t i = 0; i < blocks_flat_sets_size; ++i) {
//         for (size_t j = 0; j < context->data_flow_analysis->set_size; ++j) {
//             printf("%i ", GET_DFA_BLOCK_SET_AT(i, j) ? 1 : 0);
//         }
//         printf("\n");
//     }

//     printf("instructions_flat_sets_size: (%zu, %zu)\n", instructions_flat_sets_size,
//         context->data_flow_analysis->set_size);
//     for (size_t i = 0; i < instructions_flat_sets_size; ++i) {
//         for (size_t j = 0; j < context->data_flow_analysis->set_size; ++j) {
//             printf("%i ", GET_DFA_INSTRUCTION_SET_AT(i, j) ? 1 : 0);
//         }
//         printf("\n");
//     }

//     // printf("data_index_map\n");
//     // for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
//     //     printf("%zu ", context->data_flow_analysis->data_index_map[i]);
//     // }
//     // printf("\n\n");

//     // for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
//     //     if (GET_INSTRUCTION(instruction_index)) {
//     //         if (GET_INSTRUCTION(instruction_index)->type() == AST_T::TacCopy_t) {
//     //             printf("Copy %zu\n", instruction_index);
//     //         }
//     //     }
//     // }
//     // printf("\n\n");

//     // printf("instruction_index_map\n");
//     // for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
//     //     if (GET_INSTRUCTION(instruction_index)) {
//     //         switch (GET_INSTRUCTION(instruction_index)->type()) {
//     //             case AST_T::TacFunCall_t:
//     //             case AST_T::TacUnary_t:
//     //             case AST_T::TacBinary_t:
//     //             case AST_T::TacCopy_t: {
//     //                 printf("%zu, %zu\n", instruction_index,
//     //                     context->data_flow_analysis->instruction_index_map[instruction_index]);
//     //                 break;
//     //             }
//     //             default:
//     //                 break;
//     //         }
//     //     }
//     // }
//     // printf("%zu, %zu\n", context->data_flow_analysis->incoming_index,
//     //     context->data_flow_analysis->instruction_index_map[context->data_flow_analysis->incoming_index]);
// }

static bool is_aliased_value(TacValue* node) {
    return node->type() == AST_T::TacVariable_t
           && context->data_flow_analysis->alias_set.find(static_cast<TacVariable*>(node)->name)
                  != context->data_flow_analysis->alias_set.end();
}

static bool is_constant_value_signed(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_T::CConstChar_t:
        case AST_T::CConstInt_t:
        case AST_T::CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_variable_value_signed(TacVariable* node) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::Int_t:
        case AST_T::Long_t:
        case AST_T::Double_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_signed(TacValue* node) {
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return is_constant_value_signed(static_cast<TacConstant*>(node));
        case AST_T::TacVariable_t:
            return is_variable_value_signed(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_constant_same_value(TacConstant* node_1, TacConstant* node_2) {
    if (node_1->constant->type() == node_2->constant->type()) {
        switch (node_1->constant->type()) {
            case AST_T::CConstInt_t:
                return static_cast<CConstInt*>(node_1->constant.get())->value
                       == static_cast<CConstInt*>(node_2->constant.get())->value;
            case AST_T::CConstLong_t:
                return static_cast<CConstLong*>(node_1->constant.get())->value
                       == static_cast<CConstLong*>(node_2->constant.get())->value;
            case AST_T::CConstUInt_t:
                return static_cast<CConstUInt*>(node_1->constant.get())->value
                       == static_cast<CConstUInt*>(node_2->constant.get())->value;
            case AST_T::CConstULong_t:
                return static_cast<CConstULong*>(node_1->constant.get())->value
                       == static_cast<CConstULong*>(node_2->constant.get())->value;
            case AST_T::CConstDouble_t:
                return static_cast<CConstDouble*>(node_1->constant.get())->value
                       == static_cast<CConstDouble*>(node_2->constant.get())->value;
            case AST_T::CConstChar_t:
                return static_cast<CConstChar*>(node_1->constant.get())->value
                       == static_cast<CConstChar*>(node_2->constant.get())->value;
            case AST_T::CConstUChar_t:
                return static_cast<CConstUChar*>(node_1->constant.get())->value
                       == static_cast<CConstUChar*>(node_2->constant.get())->value;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    return false;
}

static bool is_variable_same_value(TacVariable* node_1, TacVariable* node_2) {
    return node_1->name.compare(node_2->name) == 0;
}

static bool is_same_value(TacValue* node_1, TacValue* node_2) {
    if (node_1->type() == node_2->type()) {
        switch (node_1->type()) {
            case AST_T::TacConstant_t:
                return is_constant_same_value(static_cast<TacConstant*>(node_1), static_cast<TacConstant*>(node_2));
            case AST_T::TacVariable_t:
                return is_variable_same_value(static_cast<TacVariable*>(node_1), static_cast<TacVariable*>(node_2));
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    return false;
}

static bool is_variable_name_same_value(TacVariable* node, const TIdentifier& name) {
    return node->name.compare(name) == 0;
}

static bool is_name_same_value(TacValue* node, const TIdentifier& name) {
    switch (node->type()) {
        case AST_T::TacConstant_t:
            return false;
        case AST_T::TacVariable_t:
            return is_variable_name_same_value(static_cast<TacVariable*>(node), name);
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_transfer_instruction(size_t instruction_index) {
    switch (GET_INSTRUCTION(instruction_index)->type()) {
        case AST_T::TacSignExtend_t:
        case AST_T::TacTruncate_t:
        case AST_T::TacZeroExtend_t:
        case AST_T::TacDoubleToInt_t:
        case AST_T::TacDoubleToUInt_t:
        case AST_T::TacIntToDouble_t:
        case AST_T::TacUIntToDouble_t:
        case AST_T::TacFunCall_t:
        case AST_T::TacUnary_t:
        case AST_T::TacBinary_t:
        case AST_T::TacCopy_t:
        case AST_T::TacGetAddress_t:
        case AST_T::TacLoad_t:
        case AST_T::TacStore_t:
        case AST_T::TacAddPtr_t:
        case AST_T::TacCopyToOffset_t:
        case AST_T::TacCopyFromOffset_t:
            return true;
        default:
            return false;
    }
}

static void copy_propagation_transfer_dst_value_reaching_copies(
    TacValue* node, size_t instruction_index, size_t next_instruction_index) {
    if (node->type() != AST_T::TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
            if (GET_DFA_INSTRUCTION(i)->type() != AST_T::TacCopy_t) {
                RAISE_INTERNAL_ERROR;
            }
            TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTRUCTION(i).get());
            if (copy->dst->type() != AST_T::TacVariable_t) {
                RAISE_INTERNAL_ERROR;
            }
            else if (is_same_value(node, copy->src.get()) || is_same_value(node, copy->dst.get())) {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
            }
            else {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = true;
            }
        }
        else {
            GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
        }
    }
}

static void copy_propagation_transfer_fun_call_reaching_copies(
    TacFunCall* node, size_t instruction_index, size_t next_instruction_index) {
    if (node->dst && node->dst->type() != AST_T::TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
            if (GET_DFA_INSTRUCTION(i)->type() != AST_T::TacCopy_t) {
                RAISE_INTERNAL_ERROR;
            }
            TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTRUCTION(i).get());
            if (copy->dst->type() != AST_T::TacVariable_t) {
                RAISE_INTERNAL_ERROR;
            }
            else if (is_aliased_value(copy->src.get()) || is_aliased_value(copy->dst.get())
                     || (node->dst
                         && (is_same_value(node->dst.get(), copy->src.get())
                             || is_same_value(node->dst.get(), copy->dst.get())))) {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
            }
            else {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = true;
            }
        }
        else {
            GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
        }
    }
}

static void copy_propagation_transfer_copy_reaching_copies(
    TacCopy* node, size_t instruction_index, size_t next_instruction_index) {
    if (node->dst->type() != AST_T::TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTRUCTION(i)->type() != AST_T::TacCopy_t) {
            RAISE_INTERNAL_ERROR;
        }
        TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTRUCTION(i).get());
        if (copy->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        else if (is_same_value(node->dst.get(), copy->dst.get())) {
            if (is_value_signed(copy->src.get()) == is_value_signed(copy->dst.get())
                && is_same_value(node->src.get(), copy->src.get())) {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = true;
            }
            else {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
            }
        }
        else if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
            if (is_same_value(node->dst.get(), copy->src.get())) {
                if (is_same_value(node->src.get(), copy->dst.get())) {
                    for (i = 0; i < context->data_flow_analysis->set_size; ++i) {
                        // TBD? : copy range
                        GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) =
                            GET_DFA_INSTRUCTION_SET_AT(instruction_index, i);
                    }
                    return; // TODO return or break ?
                }
                else {
                    GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
                }
            }
            else {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = true;
            }
        }
        else {
            GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
        }
    }
}

static void copy_propagation_transfer_store_reaching_copies(size_t instruction_index, size_t next_instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
            if (GET_DFA_INSTRUCTION(i)->type() != AST_T::TacCopy_t) {
                RAISE_INTERNAL_ERROR;
            }
            TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTRUCTION(i).get());
            if (copy->dst->type() != AST_T::TacVariable_t) {
                RAISE_INTERNAL_ERROR;
            }
            else if (is_aliased_value(copy->src.get()) || is_aliased_value(copy->dst.get())) {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
            }
            else {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = true;
            }
        }
        else {
            GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
        }
    }
}

static void copy_propagation_transfer_copy_to_offset_reaching_copies(
    TacCopyToOffset* node, size_t instruction_index, size_t next_instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
            if (GET_DFA_INSTRUCTION(i)->type() != AST_T::TacCopy_t) {
                RAISE_INTERNAL_ERROR;
            }
            TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTRUCTION(i).get());
            if (copy->dst->type() != AST_T::TacVariable_t) {
                RAISE_INTERNAL_ERROR;
            }
            else if (is_name_same_value(copy->src.get(), node->dst_name)
                     || is_name_same_value(copy->dst.get(), node->dst_name)) {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
            }
            else {
                GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = true;
            }
        }
        else {
            GET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i) = false;
        }
    }
}

static void copy_propagation_transfer_reaching_copies(
    TacInstruction* node, size_t instruction_index, size_t next_instruction_index) {
    switch (node->type()) {
        case AST_T::TacSignExtend_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacSignExtend*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacTruncate_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacTruncate*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacZeroExtend_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacZeroExtend*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacDoubleToInt_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacDoubleToInt*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacDoubleToUInt_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacDoubleToUInt*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacIntToDouble_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacIntToDouble*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacUIntToDouble_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacUIntToDouble*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacFunCall_t:
            copy_propagation_transfer_fun_call_reaching_copies(
                static_cast<TacFunCall*>(node), instruction_index, next_instruction_index);
            break;
        case AST_T::TacUnary_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacUnary*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacBinary_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacBinary*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacCopy_t:
            copy_propagation_transfer_copy_reaching_copies(
                static_cast<TacCopy*>(node), instruction_index, next_instruction_index);
            break;
        case AST_T::TacGetAddress_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacGetAddress*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacLoad_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacLoad*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacStore_t:
            copy_propagation_transfer_store_reaching_copies(instruction_index, next_instruction_index);
            break;
        case AST_T::TacAddPtr_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacAddPtr*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        case AST_T::TacCopyToOffset_t:
            copy_propagation_transfer_copy_to_offset_reaching_copies(
                static_cast<TacCopyToOffset*>(node), instruction_index, next_instruction_index);
            break;
        case AST_T::TacCopyFromOffset_t:
            copy_propagation_transfer_dst_value_reaching_copies(
                static_cast<TacCopyFromOffset*>(node)->dst.get(), instruction_index, next_instruction_index);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static size_t data_flow_analysis_transfer_block(size_t instruction_index, size_t block_id) {
    for (size_t next_instruction_index = instruction_index + 1;
         next_instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++next_instruction_index) {
        if (GET_INSTRUCTION(next_instruction_index) && is_transfer_instruction(next_instruction_index)) {
            copy_propagation_transfer_reaching_copies(
                GET_INSTRUCTION(instruction_index).get(), instruction_index, next_instruction_index);
            instruction_index = next_instruction_index;
        }
    }
    copy_propagation_transfer_reaching_copies(
        GET_INSTRUCTION(instruction_index).get(), instruction_index, context->data_flow_analysis->incoming_index);
    return instruction_index;
}

static bool data_flow_analysis_meet_block(size_t block_id) {
    size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
    for (; instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
        if (GET_INSTRUCTION(instruction_index) && is_transfer_instruction(instruction_index)) {
            goto Lelse;
        }
    }
    instruction_index = context->data_flow_analysis->incoming_index;
Lelse:
    std::fill(GET_DFA_INSTRUCTION_SET_RANGE(instruction_index), true);

    for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
        if (predecessor_id < context->control_flow_graph->exit_id) {
            for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
                if (!GET_DFA_BLOCK_SET_AT(predecessor_id, i)) {
                    GET_DFA_INSTRUCTION_SET_AT(instruction_index, i) = false;
                }
            }
        }
        else if (predecessor_id == context->control_flow_graph->entry_id) {
            std::fill(GET_DFA_INSTRUCTION_SET_RANGE(instruction_index), false);
            break;
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }

    if (instruction_index < context->data_flow_analysis->incoming_index) {
        data_flow_analysis_transfer_block(instruction_index, block_id);
    }
    else if (instruction_index != context->data_flow_analysis->incoming_index) {
        RAISE_INTERNAL_ERROR;
    }

    bool is_fixed_point = true;
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_BLOCK_SET_AT(block_id, i)
            != GET_DFA_INSTRUCTION_SET_AT(context->data_flow_analysis->incoming_index, i)) {
            // TBD? : after first difference copy range instruction_index -> next_instruction_index, and break
            // memcpy = std::copy
            // memset = std::fill
            // memcmp = std::equal
            GET_DFA_BLOCK_SET_AT(block_id, i) =
                GET_DFA_INSTRUCTION_SET_AT(context->data_flow_analysis->incoming_index, i);
            is_fixed_point = false;
        }
    }
    return is_fixed_point;
}

static void data_flow_analysis_iterative_algorithm() {
    size_t open_block_ids_size = context->control_flow_graph->blocks.size();
    for (size_t i = 0; i < open_block_ids_size; ++i) {
        size_t block_id = context->data_flow_analysis->open_block_ids[i];
        if (block_id == context->control_flow_graph->exit_id) {
            continue;
        }

        bool is_fixed_point = data_flow_analysis_meet_block(block_id);
        if (!is_fixed_point) {
            for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
                if (successor_id < context->control_flow_graph->exit_id) {
                    for (size_t j = i + 1; j < open_block_ids_size; ++j) {
                        if (successor_id == context->data_flow_analysis->open_block_ids[j]) {
                            goto Lelse;
                        }
                    }
                    if (open_block_ids_size < context->data_flow_analysis->open_block_ids.size()) {
                        context->data_flow_analysis->open_block_ids[open_block_ids_size] = successor_id;
                    }
                    else {
                        context->data_flow_analysis->open_block_ids.push_back(successor_id);
                    }
                    open_block_ids_size++;
                Lelse:;
                }
                else if (successor_id != context->control_flow_graph->exit_id) {
                    RAISE_INTERNAL_ERROR;
                }
            }
        }
    }
}

static bool data_flow_analysis_initialize() {
    context->data_flow_analysis->set_size = 0;
    context->data_flow_analysis->incoming_index = context->p_instructions->size();

    if (context->data_flow_analysis->open_block_ids.size() < context->control_flow_graph->blocks.size()) {
        context->data_flow_analysis->open_block_ids.resize(context->control_flow_graph->blocks.size());
    }
    if (context->data_flow_analysis->block_index_map.size() < context->control_flow_graph->blocks.size()) {
        context->data_flow_analysis->block_index_map.resize(context->control_flow_graph->blocks.size());
    }
    if (context->data_flow_analysis->instruction_index_map.size() < context->p_instructions->size() + 1) {
        context->data_flow_analysis->instruction_index_map.resize(context->p_instructions->size() + 1);
    }

    size_t blocks_flat_sets_size = 0;
    size_t instructions_flat_sets_size = 0;
    bool is_aliased = true;                         // TODO based on optim stage
    context->data_flow_analysis->alias_set.clear(); // TODO based on optim stage
    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            context->data_flow_analysis->open_block_ids[block_id] = block_id; // TODO
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
                 instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTRUCTION(instruction_index)) {
                    switch (GET_INSTRUCTION(instruction_index)->type()) {
                        case AST_T::TacSignExtend_t:
                        case AST_T::TacTruncate_t:
                        case AST_T::TacZeroExtend_t:
                        case AST_T::TacDoubleToInt_t:
                        case AST_T::TacDoubleToUInt_t:
                        case AST_T::TacIntToDouble_t:
                        case AST_T::TacUIntToDouble_t:
                        case AST_T::TacFunCall_t:
                        case AST_T::TacUnary_t:
                        case AST_T::TacBinary_t:
                        case AST_T::TacLoad_t:
                        case AST_T::TacStore_t:
                        case AST_T::TacAddPtr_t:
                        case AST_T::TacCopyToOffset_t:
                        case AST_T::TacCopyFromOffset_t: {
                            context->data_flow_analysis->instruction_index_map[instruction_index] =
                                instructions_flat_sets_size;
                            instructions_flat_sets_size++;
                            break;
                        }
                        case AST_T::TacCopy_t: {
                            if (context->data_flow_analysis->set_size
                                < context->data_flow_analysis->data_index_map.size()) {
                                context->data_flow_analysis->data_index_map[context->data_flow_analysis->set_size] =
                                    instruction_index;
                            }
                            else {
                                context->data_flow_analysis->data_index_map.push_back(instruction_index);
                            }
                            context->data_flow_analysis->instruction_index_map[instruction_index] =
                                instructions_flat_sets_size;
                            if (is_aliased) {
                                TacCopy* copy = static_cast<TacCopy*>(GET_INSTRUCTION(instruction_index).get());
                                if (copy->dst->type() != AST_T::TacVariable_t) {
                                    RAISE_INTERNAL_ERROR;
                                }
                                if (copy->src->type() == AST_T::TacVariable_t) {
                                    TacVariable* copy_src = static_cast<TacVariable*>(copy->src.get());
                                    if (frontend->symbol_table[copy_src->name]->attrs->type() == AST_T::StaticAttr_t) {
                                        context->data_flow_analysis->alias_set.insert(copy_src->name);
                                    }
                                }
                                TacVariable* copy_dst = static_cast<TacVariable*>(copy->dst.get());
                                if (frontend->symbol_table[copy_dst->name]->attrs->type() == AST_T::StaticAttr_t) {
                                    context->data_flow_analysis->alias_set.insert(copy_dst->name);
                                }
                            }
                            context->data_flow_analysis->set_size++;
                            instructions_flat_sets_size++;
                            break;
                        }
                        case AST_T::TacGetAddress_t: {
                            context->data_flow_analysis->instruction_index_map[instruction_index] =
                                instructions_flat_sets_size;
                            if (is_aliased) {
                                TacGetAddress* get_address =
                                    static_cast<TacGetAddress*>(GET_INSTRUCTION(instruction_index).get());
                                if (get_address->src->type() == AST_T::TacVariable_t) {
                                    context->data_flow_analysis->alias_set.insert(
                                        static_cast<TacVariable*>(get_address->src.get())->name);
                                }
                            }
                            instructions_flat_sets_size++;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
            context->data_flow_analysis->block_index_map[block_id] = blocks_flat_sets_size;
            blocks_flat_sets_size++;
        }
        else {
            context->data_flow_analysis->open_block_ids[block_id] = context->control_flow_graph->exit_id; // TODO
        }
    }
    if (context->data_flow_analysis->set_size == 0) {
        return false;
    }
    context->data_flow_analysis->instruction_index_map[context->data_flow_analysis->incoming_index] =
        instructions_flat_sets_size;
    instructions_flat_sets_size++;
    blocks_flat_sets_size *= context->data_flow_analysis->set_size;
    instructions_flat_sets_size *= context->data_flow_analysis->set_size;

    if (context->control_flow_graph->reaching_code.size() < context->data_flow_analysis->set_size) {
        context->control_flow_graph->reaching_code.resize(context->data_flow_analysis->set_size);
    }
    if (context->data_flow_analysis->bak_instructions.size() < context->data_flow_analysis->set_size) {
        context->data_flow_analysis->bak_instructions.resize(context->data_flow_analysis->set_size);
    }
    if (context->data_flow_analysis->blocks_flat_sets.size() < blocks_flat_sets_size) {
        context->data_flow_analysis->blocks_flat_sets.resize(blocks_flat_sets_size);
    }
    if (context->data_flow_analysis->instructions_flat_sets.size() < instructions_flat_sets_size) {
        context->data_flow_analysis->instructions_flat_sets.resize(instructions_flat_sets_size);
    }
    std::fill(context->control_flow_graph->reaching_code.begin(),
        context->control_flow_graph->reaching_code.begin() + context->data_flow_analysis->set_size, false);
    std::fill(context->data_flow_analysis->blocks_flat_sets.begin(),
        context->data_flow_analysis->blocks_flat_sets.begin() + blocks_flat_sets_size, true);

    return true;
}

static size_t get_dfa_data_index(size_t instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (context->data_flow_analysis->data_index_map[i] == instruction_index) {
            return i;
        }
    }
    RAISE_INTERNAL_ERROR;
}

static TacInstruction* get_dfa_bak_instruction(size_t i) {
    if (context->control_flow_graph->reaching_code[i]) {
        if (context->data_flow_analysis->bak_instructions[i]) {
            return context->data_flow_analysis->bak_instructions[i].get();
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }
    else if (GET_DFA_INSTRUCTION(i)) {
        return GET_DFA_INSTRUCTION(i).get();
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static TacCopy* get_dfa_bak_copy_instruction(size_t i) {
    TacInstruction* node = get_dfa_bak_instruction(i);
    if (node->type() != AST_T::TacCopy_t) {
        RAISE_INTERNAL_ERROR;
    }
    return static_cast<TacCopy*>(node);
}

static bool set_dfa_bak_instruction(size_t instruction_index, size_t& i) {
    i = get_dfa_data_index(instruction_index);
    if (!context->control_flow_graph->reaching_code[i]) {
        context->control_flow_graph->reaching_code[i] = true;
        return true;
    }
    else {
        return false;
    }
}

static void set_dfa_bak_copy_instruction(TacCopy* node, size_t instruction_index) {
    size_t i;
    if (set_dfa_bak_instruction(instruction_index, i)) {
        std::shared_ptr<TacValue> src = node->src;
        std::shared_ptr<TacValue> dst = node->dst;
        context->data_flow_analysis->bak_instructions[i] = std::make_unique<TacCopy>(std::move(src), std::move(dst));
    }
}

static void propagate_copies_return_instructions(TacReturn* node, size_t incoming_index, size_t exit_block) {
    if (node->val && node->val->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (((exit_block != 0 && GET_DFA_BLOCK_SET_AT(incoming_index, i))
                    || (exit_block == 0 && GET_DFA_INSTRUCTION_SET_AT(incoming_index, i)))) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->val.get(), copy->dst.get())) {
                    node->val = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_sign_extend_instructions(TacSignExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_truncate_instructions(TacTruncate* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_zero_extend_instructions(TacZeroExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_double_to_int_instructions(TacDoubleToInt* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_double_to_uint_instructions(TacDoubleToUInt* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_int_to_double_instructions(TacIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_uint_to_double_instructions(TacUIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_fun_call_instructions(TacFunCall* node, size_t instruction_index) {
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (node->args[i]->type() == AST_T::TacVariable_t) {
            for (size_t j = 0; j < context->data_flow_analysis->set_size; ++j) {
                if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, j)) {
                    TacCopy* copy = get_dfa_bak_copy_instruction(j);
                    if (copy->dst->type() != AST_T::TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->args[i].get(), copy->dst.get())) {
                        node->args[i] = copy->src;
                        context->is_fixed_point = false;
                        break;
                    }
                }
            }
        }
    }
}

static void propagate_copies_unary_instructions(TacUnary* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_binary_instructions(TacBinary* node, size_t instruction_index) {
    bool is_src1 = node->src1->type() == AST_T::TacVariable_t;
    bool is_src2 = node->src2->type() == AST_T::TacVariable_t;
    if (is_src1 || is_src2) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_src1 && is_same_value(node->src1.get(), copy->dst.get())) {
                    node->src1 = copy->src;
                    context->is_fixed_point = false;
                    is_src1 = false;
                    if (!is_src2) {
                        break;
                    }
                }
                if (is_src2 && is_same_value(node->src2.get(), copy->dst.get())) {
                    node->src2 = copy->src;
                    context->is_fixed_point = false;
                    is_src2 = false;
                    if (!is_src1) {
                        break;
                    }
                }
            }
        }
    }
}

static void propagate_copies_copy_instructions(TacCopy* node, size_t instruction_index, size_t block_id) {
    if (node->dst->type() != AST_T::TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
            TacCopy* copy = get_dfa_bak_copy_instruction(i);
            if (copy->dst->type() != AST_T::TacVariable_t) {
                RAISE_INTERNAL_ERROR;
            }
            else if (context->data_flow_analysis->data_index_map[i] == instruction_index
                     || (is_same_value(node->src.get(), copy->dst.get())
                         && is_same_value(node->dst.get(), copy->src.get()))) {
                set_dfa_bak_copy_instruction(node, instruction_index);
                control_flow_graph_remove_block_instruction(instruction_index, block_id);
                break;
            }
            else if (is_same_value(node->src.get(), copy->dst.get())) {
                set_dfa_bak_copy_instruction(node, instruction_index);
                node->src = copy->src;
                context->is_fixed_point = false;
                break;
            }
        }
    }
}

static void propagate_copies_load_instructions(TacLoad* node, size_t instruction_index) {
    if (node->src_ptr->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src_ptr.get(), copy->dst.get())) {
                    node->src_ptr = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_store_instructions(TacStore* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_add_ptr_instructions(TacAddPtr* node, size_t instruction_index) {
    bool is_src_ptr = node->src_ptr->type() == AST_T::TacVariable_t;
    bool is_index = node->index->type() == AST_T::TacVariable_t;
    if (is_src_ptr || is_index) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_src_ptr && is_same_value(node->src_ptr.get(), copy->dst.get())) {
                    node->src_ptr = copy->src;
                    context->is_fixed_point = false;
                    is_src_ptr = false;
                    if (!is_index) {
                        break;
                    }
                }
                if (is_index && is_same_value(node->index.get(), copy->dst.get())) {
                    node->index = copy->src;
                    context->is_fixed_point = false;
                    is_index = false;
                    if (!is_src_ptr) {
                        break;
                    }
                }
            }
        }
    }
}

static void propagate_copies_copy_to_offset_instructions(TacCopyToOffset* node, size_t instruction_index) {
    if (node->src->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_copy_from_offset_instructions(TacCopyFromOffset* node, size_t instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
            TacCopy* copy = get_dfa_bak_copy_instruction(i);
            if (copy->dst->type() != AST_T::TacVariable_t) {
                RAISE_INTERNAL_ERROR;
            }
            else if (is_name_same_value(copy->dst.get(), node->src_name)) {
                if (copy->src->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                node->src_name = static_cast<TacVariable*>(copy->src.get())->name;
                context->is_fixed_point = false;
                break;
            }
        }
    }
}

static void propagate_copies_jump_if_zero_instructions(TacJumpIfZero* node, size_t incoming_index, size_t exit_block) {
    if (node->condition->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (((exit_block != 0 && GET_DFA_BLOCK_SET_AT(incoming_index, i))
                    || (exit_block == 0 && GET_DFA_INSTRUCTION_SET_AT(incoming_index, i)))) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->condition.get(), copy->dst.get())) {
                    node->condition = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_jump_if_not_zero_instructions(
    TacJumpIfNotZero* node, size_t incoming_index, size_t exit_block) {
    if (node->condition->type() == AST_T::TacVariable_t) {
        for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
            if (((exit_block != 0 && GET_DFA_BLOCK_SET_AT(incoming_index, i))
                    || (exit_block == 0 && GET_DFA_INSTRUCTION_SET_AT(incoming_index, i)))) {
                TacCopy* copy = get_dfa_bak_copy_instruction(i);
                if (copy->dst->type() != AST_T::TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node->condition.get(), copy->dst.get())) {
                    node->condition = copy->src;
                    context->is_fixed_point = false;
                    break;
                }
            }
        }
    }
}

static void propagate_copies_instructions(TacInstruction* node, size_t instruction_index, size_t block_id) {
    switch (node->type()) {
        case AST_T::TacReturn_t:
            propagate_copies_return_instructions(static_cast<TacReturn*>(node), instruction_index, block_id);
            break;
        case AST_T::TacSignExtend_t:
            propagate_copies_sign_extend_instructions(static_cast<TacSignExtend*>(node), instruction_index);
            break;
        case AST_T::TacTruncate_t:
            propagate_copies_truncate_instructions(static_cast<TacTruncate*>(node), instruction_index);
            break;
        case AST_T::TacZeroExtend_t:
            propagate_copies_zero_extend_instructions(static_cast<TacZeroExtend*>(node), instruction_index);
            break;
        case AST_T::TacDoubleToInt_t:
            propagate_copies_double_to_int_instructions(static_cast<TacDoubleToInt*>(node), instruction_index);
            break;
        case AST_T::TacDoubleToUInt_t:
            propagate_copies_double_to_uint_instructions(static_cast<TacDoubleToUInt*>(node), instruction_index);
            break;
        case AST_T::TacIntToDouble_t:
            propagate_copies_int_to_double_instructions(static_cast<TacIntToDouble*>(node), instruction_index);
            break;
        case AST_T::TacUIntToDouble_t:
            propagate_copies_uint_to_double_instructions(static_cast<TacUIntToDouble*>(node), instruction_index);
            break;
        case AST_T::TacFunCall_t:
            propagate_copies_fun_call_instructions(static_cast<TacFunCall*>(node), instruction_index);
            break;
        case AST_T::TacUnary_t:
            propagate_copies_unary_instructions(static_cast<TacUnary*>(node), instruction_index);
            break;
        case AST_T::TacBinary_t:
            propagate_copies_binary_instructions(static_cast<TacBinary*>(node), instruction_index);
            break;
        case AST_T::TacCopy_t:
            propagate_copies_copy_instructions(static_cast<TacCopy*>(node), instruction_index, block_id);
            break;
        case AST_T::TacLoad_t:
            propagate_copies_load_instructions(static_cast<TacLoad*>(node), instruction_index);
            break;
        case AST_T::TacStore_t:
            propagate_copies_store_instructions(static_cast<TacStore*>(node), instruction_index);
            break;
        case AST_T::TacAddPtr_t:
            propagate_copies_add_ptr_instructions(static_cast<TacAddPtr*>(node), instruction_index);
            break;
        case AST_T::TacCopyToOffset_t:
            propagate_copies_copy_to_offset_instructions(static_cast<TacCopyToOffset*>(node), instruction_index);
            break;
        case AST_T::TacCopyFromOffset_t:
            propagate_copies_copy_from_offset_instructions(static_cast<TacCopyFromOffset*>(node), instruction_index);
            break;
        case AST_T::TacJumpIfZero_t:
            propagate_copies_jump_if_zero_instructions(static_cast<TacJumpIfZero*>(node), instruction_index, block_id);
            break;
        case AST_T::TacJumpIfNotZero_t:
            propagate_copies_jump_if_not_zero_instructions(
                static_cast<TacJumpIfNotZero*>(node), instruction_index, block_id);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void propagate_copies_control_flow_graph() {
    if (!data_flow_analysis_initialize()) {
        return;
    }
    data_flow_analysis_iterative_algorithm();
    // print_copy_propagation();

    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            size_t incoming_index = block_id;
            size_t exit_block = 1;
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_back_index + 1;
                 instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index;) {
                if (GET_INSTRUCTION(instruction_index)) {
                    switch (GET_INSTRUCTION(instruction_index)->type()) {
                        case AST_T::TacReturn_t:
                        case AST_T::TacJumpIfZero_t:
                        case AST_T::TacJumpIfNotZero_t:
                            propagate_copies_instructions(
                                GET_INSTRUCTION(instruction_index).get(), incoming_index, exit_block);
                            break;
                        case AST_T::TacSignExtend_t:
                        case AST_T::TacTruncate_t:
                        case AST_T::TacZeroExtend_t:
                        case AST_T::TacDoubleToInt_t:
                        case AST_T::TacDoubleToUInt_t:
                        case AST_T::TacIntToDouble_t:
                        case AST_T::TacUIntToDouble_t:
                        case AST_T::TacFunCall_t:
                        case AST_T::TacUnary_t:
                        case AST_T::TacBinary_t:
                        case AST_T::TacCopy_t:
                        case AST_T::TacLoad_t:
                        case AST_T::TacStore_t:
                        case AST_T::TacAddPtr_t:
                        case AST_T::TacCopyToOffset_t:
                        case AST_T::TacCopyFromOffset_t: {
                            propagate_copies_instructions(
                                GET_INSTRUCTION(instruction_index).get(), instruction_index, block_id);
                            incoming_index = instruction_index;
                            exit_block = 0;
                            break;
                        }
                        case AST_T::TacGetAddress_t: {
                            incoming_index = instruction_index;
                            exit_block = 0;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }
    }
}

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
    context->p_instructions = &node->body;
    // func_name = node->name;
    do {
        context->is_fixed_point = true;
        if (context->enabled_optimizations[CONSTANT_FOLDING]) {
            // printf("--fold-constants\n"); // TODO rm
            fold_constants_list_instructions();
        }
        if (context->enabled_optimizations[CONTROL_FLOW_GRAPH]) {
            control_flow_graph_initialize();
            if (context->enabled_optimizations[UNREACHABLE_CODE_ELIMINATION]) {
                eliminate_unreachable_code_control_flow_graph();
                // printf("--eliminate-unreachable-code\n"); // TODO rm
            }
            if (context->enabled_optimizations[COPY_PROPAGATION]) {
                // printf("--propagate-copies\n"); // TODO rm
                propagate_copies_control_flow_graph();
            }
            if (context->enabled_optimizations[DEAD_STORE_ELMININATION]) {
                // printf("--eliminate-dead-stores\n"); // TODO rm
            }
        }
    }
    while (!context->is_fixed_point);
    context->p_instructions = nullptr;
}

static void optimize_top_level(TacTopLevel* node) {
    if (node->type() == AST_T::TacFunction_t) {
        optimize_function_top_level(static_cast<TacFunction*>(node));
    }
    else {
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
    if (context->enabled_optimizations[CONTROL_FLOW_GRAPH]) {
        context->control_flow_graph = std::make_unique<ControlFlowGraph>();
        if (context->enabled_optimizations[COPY_PROPAGATION]
            || context->enabled_optimizations[DEAD_STORE_ELMININATION]) {
            context->data_flow_analysis = std::make_unique<DataFlowAnalysis>();
        }
    }
    optimize_program(node);
    context.reset();
}
