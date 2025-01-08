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

// Copy propagation

// TODO

// transfer(block, initial_reaching_copies):
//     current_reaching_copies = initial_reaching_copies
//     for instruction in block.instructions:
//         annotate_instruction(instruction, current_reaching_copies)
//         match instruction with
//         | Copy(src, dst) ->
//             if Copy(dst, src) is in current_reaching_copies:
//                 continue
//             for copy in current_reaching_copies:
//                 if copy.src == dst || copy.dst == dst:
//                     current_reaching_copies.remove(copy)
//             current_reaching_copies.add(instruction)
//         | FunCall(fun_name, args, dst) ->
//             for copy in current_reaching_copies:
//                 if copy.src is static || copy.dst is static
//                     || copy.src == dst || copy.dst == dst:
//                     current_reaching_copies.remove(copy)
//         | Unary(operator, src, dst) ->
//             for copy in current_reaching_copies:
//                 if copy.src == dst || copy.dst == dst:
//                     current_reaching_copies.remove(copy)
//         | Binary(operator, src1, src2, dst) ->
//             --same as Unary--
//         | _ -> continue
//     annotate_block(block.id, current_reaching_copies)

// meet(block, all_copies):
//     incoming_copies = all_copies
//     for pred_id in block.predecessors:
//         match pred_id with
//         | ENTRY -> return {}
//         | BlockId(id) ->
//             pred_out_copies = get_block_annotation(pred_id)
//             incoming_copies = intersection(incoming_copies, pred_out_copies)
//         | EXIT -> fail("Malformed control-flow graph")
//     return incoming_copies

// block = take_first(worklist)
//// get next current position from open_block_ids
// old_annotation = get_block_annotation(block.id)
//// get [block_id * size(all_copy_indices) + 0]
// incoming_copies = meet(block, all_copies)
//// meet(block_id, all_copy_indices)
// transfer(block, incoming_copies)
//// transfer(block_id, incoming_copies)
// if old_annotation != get_block_annotation(block.id):

// while worklist is not empty:
//     block = take_first(worklist)
//     old_annotation = get_block_annotation(block.id)
//     incoming_copies = meet(block, all_copies)
//     transfer(block, incoming_copies)
//     if old_annotation != get_block_annotation(block.id):
//         for successor_id in block.successors:
//             match successor_id with
//             | EXIT -> continue
//             | ENTRY -> fail("Malformed control-flow graph")
//             | BlockId(id) ->
//                 successor = get_block_by_id(successor_id)
//                 if successor is not in worklist:
//                     worklist.append(successor)

/*
all_copy_indices
= vector of indices of copy instructions

reaching_copy_block_masks
= vector of vector of bool (blocks vector of bitmask of all_copy_indices)
reaching_copy_instruction_masks
= vector of vector of bool (reaching copy instructions vector of all_copy_indices)

+ all_reaching_copy_instruction_indices (indices of all instructions copy/funcall/unary/binary to map for
reaching_copy_instruction_masks)
*/

static void propagate_copies_control_flow_graph() {
    if (context->copy_propagation->open_block_ids.size() < context->control_flow_graph->blocks.size()) {
        context->copy_propagation->open_block_ids.resize(context->control_flow_graph->blocks.size());
    }
    if (context->copy_propagation->reaching_copy_block_by_is.size() < context->control_flow_graph->blocks.size()) {
        context->copy_propagation->reaching_copy_block_by_is.resize(context->control_flow_graph->blocks.size());
    }
    if (context->copy_propagation->reaching_copy_instruction_by_is.size() < context->p_instructions->size()) {
        context->copy_propagation->reaching_copy_instruction_by_is.resize(context->p_instructions->size());
    }
    size_t all_copy_size = 0;
    {
        size_t reaching_copy_block_size = 0;
        size_t reaching_copy_instruction_size = 0;
        // TODO fill copy_propagation->open_block_ids in reverse postorder
        for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
            if (GET_CFG_BLOCK(block_id).size > 0) {
                context->copy_propagation->open_block_ids[block_id] = block_id; // TODO
                for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
                     instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                    if (GET_INSTRUCTION(instruction_index)) {
                        switch (GET_INSTRUCTION(instruction_index)->type()) {
                            case AST_T::TacCopy_t: {
                                if (all_copy_size < context->copy_propagation->all_copy_indices.size()) {
                                    context->copy_propagation->all_copy_indices[all_copy_size] = instruction_index;
                                }
                                else {
                                    context->copy_propagation->all_copy_indices.push_back(instruction_index);
                                }
                                context->copy_propagation->reaching_copy_instruction_by_is[instruction_index] =
                                    reaching_copy_instruction_size;
                                all_copy_size++;
                                reaching_copy_instruction_size++;
                                break;
                            }
                            case AST_T::TacUnary_t:
                            case AST_T::TacBinary_t: {
                                context->copy_propagation->reaching_copy_instruction_by_is[instruction_index] =
                                    reaching_copy_instruction_size;
                                reaching_copy_instruction_size++;
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                context->copy_propagation->reaching_copy_block_by_is[block_id] = reaching_copy_block_size;
                reaching_copy_block_size++;
            }
            else {
                context->copy_propagation->open_block_ids[block_id] = context->control_flow_graph->exit_id; // TODO
            }
        }
        reaching_copy_block_size *= all_copy_size;
        reaching_copy_instruction_size *= all_copy_size;
        if (context->copy_propagation->reaching_copy_block_sets.size() < reaching_copy_block_size) {
            context->copy_propagation->reaching_copy_block_sets.resize(reaching_copy_block_size);
        }
        if (context->copy_propagation->reaching_copy_instruction_sets.size() < reaching_copy_instruction_size) {
            context->copy_propagation->reaching_copy_instruction_sets.resize(reaching_copy_instruction_size);
        }
        std::fill(context->copy_propagation->reaching_copy_block_sets.begin(),
            context->copy_propagation->reaching_copy_block_sets.begin() + reaching_copy_block_size, true);
        std::fill(context->copy_propagation->reaching_copy_instruction_sets.begin(),
            context->copy_propagation->reaching_copy_instruction_sets.begin() + reaching_copy_instruction_size, false);
    }
    size_t open_block_ids_back_index = context->control_flow_graph->blocks.size() - 1;

    for (size_t i = 0; i <= open_block_ids_back_index; ++i) {
        //     // block = take_first(worklist)
        size_t block_id = context->copy_propagation->open_block_ids[i];
        if (block_id == context->control_flow_graph->exit_id) {
            continue;
        }
        bool is_fixed_point = false;
        //     //  old_annotation = get_block_annotation(block.id)
        //     //  incoming_copies = meet(block, all_copies)
        //     //  transfer(block, incoming_copies)
        if (!is_fixed_point) {
            for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
                if (successor_id < context->control_flow_graph->exit_id) {
                    for (size_t j = i; j <= open_block_ids_back_index; ++j) {
                        if (successor_id == context->copy_propagation->open_block_ids[j]) {
                            goto Lelse;
                        }
                    }
                    open_block_ids_back_index++;
                    if (open_block_ids_back_index < context->copy_propagation->open_block_ids.size()) {
                        context->copy_propagation->open_block_ids[open_block_ids_back_index] = successor_id;
                    }
                    else {
                        context->copy_propagation->open_block_ids.push_back(successor_id);
                    }
                Lelse:;
                }
                else if (successor_id != context->control_flow_graph->exit_id) {
                    RAISE_INTERNAL_ERROR;
                }
            }
        }
    }


    // size_t all_copy_size = 0;
    // size_t all_reaching_copy_instruction_indices_size = 0;
    // for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
    //     if (GET_INSTRUCTION(instruction_index)) {
    // switch (GET_INSTRUCTION(instruction_index)->type()) {
    //     case AST_T::TacCopy_t: {
    //         if (all_copy_size < context->copy_propagation->all_copy_indices.size()) {
    //             context->copy_propagation->all_copy_indices[all_copy_size] = instruction_index;
    //         }
    //         else {
    //             context->copy_propagation->all_copy_indices.push_back(instruction_index);
    //         }
    //         if (all_reaching_copy_instruction_indices_size
    //             < context->copy_propagation->all_reaching_copy_instruction_indices.size()) {
    //             context->copy_propagation
    //                 ->all_reaching_copy_instruction_indices[all_reaching_copy_instruction_indices_size] =
    //                 instruction_index;
    //         }
    //         else {
    //             context->copy_propagation->all_reaching_copy_instruction_indices.push_back(instruction_index);
    //         }
    //         all_copy_size++;
    //         all_reaching_copy_instruction_indices_size++;
    //         break;
    //     }
    //     case AST_T::TacUnary_t:
    //     case AST_T::TacBinary_t: {
    //         if (all_reaching_copy_instruction_indices_size
    //             < context->copy_propagation->all_reaching_copy_instruction_indices.size()) {
    //             context->copy_propagation
    //                 ->all_reaching_copy_instruction_indices[all_reaching_copy_instruction_indices_size] =
    //                 instruction_index;
    //         }
    //         else {
    //             context->copy_propagation->all_reaching_copy_instruction_indices.push_back(instruction_index);
    //         }
    //         all_reaching_copy_instruction_indices_size++;
    //         break;
    //     }
    //     default:
    //         break;
    // }
    //     }
    // }
    // {
    //     size_t flat_masks_size = all_copy_size * context->control_flow_graph->blocks.size();
    //     if (context->copy_propagation->reaching_copy_block_flat_masks.size() < flat_masks_size) {
    //         context->copy_propagation->reaching_copy_block_flat_masks.resize(flat_masks_size);
    //     }
    //     std::fill(context->copy_propagation->reaching_copy_block_flat_masks.begin(),
    //         context->copy_propagation->reaching_copy_block_flat_masks.begin() + flat_masks_size, true);
    // }
    // {
    //     size_t flat_masks_size = all_copy_size * all_reaching_copy_instruction_indices_size;
    //     if (context->copy_propagation->reaching_copy_instruction_flat_masks.size() < flat_masks_size) {
    //         context->copy_propagation->reaching_copy_instruction_flat_masks.resize(flat_masks_size);
    //     }
    //     std::fill(context->copy_propagation->reaching_copy_instruction_flat_masks.begin(),
    //         context->copy_propagation->reaching_copy_instruction_flat_masks.begin() + flat_masks_size, false);
    // }


    // TODO fill copy_propagation->open_block_ids in reverse postorder


    // context->copy_propagation->all_copy_indices.clear();
    // for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
    //     //     if (GET_INSTRUCTION(instruction_index)) {
    //     //         if (GET_INSTRUCTION(instruction_index)->type() == AST_T::TacCopy_t) {
    //     //             context->copy_propagation->all_copy_indices.insert(instruction_index);
    //     //         }
    //     //         context->copy_propagation->reaching_copy_instruction_masks[instruction_index].clear();
    //     //     }
    // }


    // if (context->copy_propagation->reaching_copy_block_masks.size() < context->control_flow_graph->blocks.size()) {
    //     context->copy_propagation->reaching_copy_block_masks.resize(context->control_flow_graph->blocks.size());
    // }
    // if (context->copy_propagation->reaching_copy_instruction_masks.size() < context->p_instructions->size()) {
    //     context->copy_propagation->reaching_copy_instruction_masks.resize(context->p_instructions->size());
    // }
    // context->copy_propagation->all_copy_indices.clear();
    // for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
    //     if (GET_INSTRUCTION(instruction_index)) {
    //         if (GET_INSTRUCTION(instruction_index)->type() == AST_T::TacCopy_t) {
    //             context->copy_propagation->all_copy_indices.insert(instruction_index);
    //         }
    //         context->copy_propagation->reaching_copy_instruction_masks[instruction_index].clear();
    //     }
    // }
    // // TODO fill copy_propagation->open_block_ids in reverse postorder
    // for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
    //     if (GET_CFG_BLOCK(block_id).size > 0) {
    //         context->copy_propagation->open_block_ids[block_id] = block_id;
    //         {
    //             context->copy_propagation->reaching_copy_block_masks[block_id].clear();
    //             std::unordered_set<size_t> reaching_index_set_block(
    //                 context->copy_propagation->all_copy_indices.begin(),
    //                 context->copy_propagation->all_copy_indices.end());

    //         }
    //     }
    //     else {
    //         context->copy_propagation->open_block_ids[block_id] = context->control_flow_graph->exit_id;
    //     }
    // }
    // size_t open_block_ids_back_index = context->control_flow_graph->blocks.size() - 1;


    // // for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
    // //     if (GET_CFG_BLOCK(block_id).size > 0) {
    // //         for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
    // //              instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
    // //             if (GET_INSTRUCTION(instruction_index)
    // //                 && GET_INSTRUCTION(instruction_index)->type() == AST_T::TacCopy_t) {
    // //                 context->copy_propagation->all_copy_indices.insert(instruction_index);
    // //                 // context->copy_propagation->reaching_index_set_instructions[instruction_index].clear();
    // //             }
    // //         }
    // //         // context->copy_propagation->open_block_ids[block_id] = block_id;
    // //         // context->copy_propagation->reaching_index_set_blocks[block_id].clear();
    // //     }
    // // }
    // // size_t open_block_ids_back_index = context->control_flow_graph->blocks.size() - 1;

    // // TODO annotate all blocks with all copies
    // for (size_t i = 0; i <= open_block_ids_back_index; ++i) {
    //     size_t block_id = context->copy_propagation->open_block_ids[i];
    //     if (block_id == context->control_flow_graph->exit_id) {
    //         continue;
    //     }
    //     // block = take_first(worklist)
    //     bool is_fixed_point = false;
    //     //  old_annotation = get_block_annotation(block.id)
    //     //  incoming_copies = meet(block, all_copies)
    //     //  transfer(block, incoming_copies)
    //     if (!is_fixed_point) {
    //         for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
    //             if (successor_id < context->control_flow_graph->exit_id) {
    //                 for (size_t j = i; j <= open_block_ids_back_index; ++j) {
    //                     if (successor_id == context->copy_propagation->open_block_ids[j]) {
    //                         goto Lelse;
    //                     }
    //                 }
    //                 open_block_ids_back_index++;
    //                 if (open_block_ids_back_index == context->copy_propagation->open_block_ids.size()) {
    //                     context->copy_propagation->open_block_ids.push_back(successor_id);
    //                 }
    //                 else {
    //                     context->copy_propagation->open_block_ids[open_block_ids_back_index] = successor_id;
    //                 }
    //             Lelse:;
    //             }
    //             else if (successor_id != context->control_flow_graph->exit_id) {
    //                 RAISE_INTERNAL_ERROR;
    //             }
    //         }
    //     }
    // }
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
    if (block_id < context->control_flow_graph->exit_id && !context->unreachable_code->reachable_blocks[block_id]) {
        context->unreachable_code->reachable_blocks[block_id] = true;
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
    if (context->unreachable_code->reachable_blocks.size() < context->control_flow_graph->blocks.size()) {
        context->unreachable_code->reachable_blocks.resize(context->control_flow_graph->blocks.size());
    }
    std::fill(context->unreachable_code->reachable_blocks.begin(),
        context->unreachable_code->reachable_blocks.begin() + context->control_flow_graph->blocks.size(), false);
    for (size_t successor_id : context->control_flow_graph->entry_successor_ids) {
        eliminate_unreachable_code_reachable_block(successor_id);
    }

    size_t block_id = context->control_flow_graph->blocks.size();
    size_t next_block_id = context->control_flow_graph->exit_id;
    while (block_id-- > 0) {
        if (context->unreachable_code->reachable_blocks[block_id]) {
            next_block_id = block_id;
            break;
        }
        else {
            eliminate_unreachable_code_empty_block(block_id);
        }
    }
    while (block_id-- > 0) {
        if (context->unreachable_code->reachable_blocks[block_id]) {
            eliminate_unreachable_code_jump_block(block_id, next_block_id);
            next_block_id = block_id;
        }
        else {
            eliminate_unreachable_code_empty_block(block_id);
        }
    }

    for (auto& label_id : context->control_flow_graph->label_id_map) {
        if (context->unreachable_code->reachable_blocks[label_id.second]) {
            for (block_id = label_id.second; block_id-- > 0;) {
                if (context->unreachable_code->reachable_blocks[block_id]) {
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
        if (context->enabled_optimizations[COPY_PROPAGATION]) {
            context->copy_propagation = std::make_unique<CopyPropagation>();
        }
        if (context->enabled_optimizations[UNREACHABLE_CODE_ELIMINATION]) {
            context->unreachable_code = std::make_unique<UnreachableCode>();
        }
        if (context->enabled_optimizations[DEAD_STORE_ELMININATION]) {
        }
    }
    optimize_program(node);
    context.reset();
}
