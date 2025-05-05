#include <algorithm>
#include <array>
#include <inttypes.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/interm_ast.hpp"
#include "ast_t.hpp" // ast

#include "optimization/optim_tac.hpp"

struct ControlFlowGraph;
struct DataFlowAnalysis;
struct DataFlowAnalysisO1;

struct OptimTacContext {
    OptimTacContext(uint8_t optim_1_mask);

    // Constant folding
    // Unreachable code elimination
    // Copy propagation
    // Dead store elimination
    bool is_fixed_point;
    std::array<bool, 5> enabled_optimizations;
    std::unique_ptr<ControlFlowGraph> control_flow_graph;
    std::unique_ptr<DataFlowAnalysis> data_flow_analysis;
    std::unique_ptr<DataFlowAnalysisO1> data_flow_analysis_o1;
    std::vector<std::unique_ptr<TacInstruction>>* p_instructions;
};

OptimTacContext::OptimTacContext(uint8_t optim_1_mask) :
    is_fixed_point(true),
    enabled_optimizations({
        (optim_1_mask & (static_cast<uint8_t>(1u) << 0)) > 0, // Enable constant folding
        (optim_1_mask & (static_cast<uint8_t>(1u) << 1)) > 0, // Enable copy propagation
        (optim_1_mask & (static_cast<uint8_t>(1u) << 2)) > 0, // Enable unreachable code elimination
        (optim_1_mask & (static_cast<uint8_t>(1u) << 3)) > 0, // Enable dead store elimination
        (optim_1_mask & ~(static_cast<uint8_t>(1u) << 0)) > 0 // Optimize with control flow graph
    }) {}

static std::unique_ptr<OptimTacContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code optimization

#ifndef __OPTIM_LEVEL__
#define __OPTIM_LEVEL__ 1
#undef _OPTIMIZATION_IMPL_OLVL_H
#include "impl_olvl.hpp" // optimization
#undef __OPTIM_LEVEL__
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding

static std::shared_ptr<CConst> fold_sign_extend_char_const(TacVariable* node, CConstChar* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_sign_extend_int_const(TacVariable* node, CConstInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_sign_extend_const(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstChar_t: {
            fold_constant = fold_sign_extend_char_const(node, static_cast<CConstChar*>(constant));
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_sign_extend_int_const(node, static_cast<CConstInt*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_sign_extend_instr(TacSignExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_sign_extend_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_truncate_int_const(TacVariable* node, CConstInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_truncate_long_const(TacVariable* node, CConstLong* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        case AST_UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_truncate_uint_const(TacVariable* node, CConstUInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_truncate_ulong_const(TacVariable* node, CConstULong* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        case AST_UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_truncate_const(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstInt_t: {
            fold_constant = fold_truncate_int_const(node, static_cast<CConstInt*>(constant));
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_truncate_long_const(node, static_cast<CConstLong*>(constant));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_truncate_uint_const(node, static_cast<CConstUInt*>(constant));
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_truncate_ulong_const(node, static_cast<CConstULong*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_truncate_instr(TacTruncate* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_truncate_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_zero_extend_uchar_const(TacVariable* node, CConstUChar* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_zero_extend_uint_const(TacVariable* node, CConstUInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_zero_extend_const(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstUChar_t: {
            fold_constant = fold_zero_extend_uchar_const(node, static_cast<CConstUChar*>(constant));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_zero_extend_uint_const(node, static_cast<CConstUInt*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_zero_extend_instr(TacZeroExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_zero_extend_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_dbl_to_signed_const(TacVariable* node, CConst* constant) {
    if (constant->type() != AST_CConstDouble_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = static_cast<TChar>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstChar>(std::move(value));
            break;
        }
        case AST_Int_t: {
            TInt value = static_cast<TInt>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstInt>(std::move(value));
            break;
        }
        case AST_Long_t: {
            TLong value = static_cast<TLong>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_dbl_to_signed_instr(TacDoubleToInt* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_dbl_to_signed_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_dbl_to_unsigned_const(TacVariable* node, CConst* constant) {
    if (constant->type() != AST_CConstDouble_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_UChar_t: {
            TUChar value = static_cast<TUChar>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstUChar>(std::move(value));
            break;
        }
        case AST_UInt_t: {
            TUInt value = static_cast<TUInt>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstUInt>(std::move(value));
            break;
        }
        case AST_ULong_t: {
            TULong value = static_cast<TULong>(static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_dbl_to_unsigned_instr(TacDoubleToUInt* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_dbl_to_unsigned_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_signed_to_dbl_const(TacVariable* node, CConst* constant) {
    if (frontend->symbol_table[node->name]->type_t->type() != AST_Double_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstChar_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstChar*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_CConstInt_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstInt*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_CConstLong_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstLong*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_signed_to_dbl_instr(TacIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_signed_to_dbl_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<TacConstant> fold_unsigned_to_dbl_const(TacVariable* node, CConst* constant) {
    if (frontend->symbol_table[node->name]->type_t->type() != AST_Double_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstUChar_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstUChar*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_CConstUInt_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstUInt*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_CConstULong_t: {
            TDouble value = static_cast<TDouble>(static_cast<CConstULong*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_unsigned_to_dbl_instr(TacUIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_unsigned_to_dbl_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_unary_char_const(TacUnaryOp* node, CConstChar* constant) {
    switch (node->type()) {
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_unary_int_const(TacUnaryOp* node, CConstInt* constant) {
    switch (node->type()) {
        case AST_TacComplement_t: {
            TInt value = ~constant->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacNegate_t: {
            TInt value = -constant->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_unary_long_const(TacUnaryOp* node, CConstLong* constant) {
    switch (node->type()) {
        case AST_TacComplement_t: {
            TLong value = ~constant->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacNegate_t: {
            TLong value = -constant->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_unary_dbl_const(TacUnaryOp* node, CConstDouble* constant) {
    switch (node->type()) {
        case AST_TacNegate_t: {
            TDouble value = -constant->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_unary_uchar_const(TacUnaryOp* node, CConstUChar* constant) {
    switch (node->type()) {
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_unary_uint_const(TacUnaryOp* node, CConstUInt* constant) {
    switch (node->type()) {
        case AST_TacComplement_t: {
            TUInt value = ~constant->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacNegate_t: {
            TUInt value = -constant->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_unary_ulong_const(TacUnaryOp* node, CConstULong* constant) {
    switch (node->type()) {
        case AST_TacComplement_t: {
            TULong value = ~constant->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacNegate_t: {
            TULong value = -constant->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_unary_const(TacUnaryOp* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstChar_t: {
            fold_constant = fold_unary_char_const(node, static_cast<CConstChar*>(constant));
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_unary_int_const(node, static_cast<CConstInt*>(constant));
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_unary_long_const(node, static_cast<CConstLong*>(constant));
            break;
        }
        case AST_CConstDouble_t: {
            fold_constant = fold_unary_dbl_const(node, static_cast<CConstDouble*>(constant));
            break;
        }
        case AST_CConstUChar_t: {
            fold_constant = fold_unary_uchar_const(node, static_cast<CConstUChar*>(constant));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_unary_uint_const(node, static_cast<CConstUInt*>(constant));
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_unary_ulong_const(node, static_cast<CConstULong*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_unary_instr(TacUnary* node, size_t instruction_index) {
    if (node->src->type() == AST_TacConstant_t) {
        std::shared_ptr<TacValue> src =
            fold_unary_const(node->unary_op.get(), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_binary_int_const(TacBinaryOp* node, CConstInt* constant_1, CConstInt* constant_2) {
    switch (node->type()) {
        case AST_TacAdd_t: {
            TInt value = constant_1->value + constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacSubtract_t: {
            TInt value = constant_1->value - constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacMultiply_t: {
            TInt value = constant_1->value * constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacDivide_t: {
            TInt value = constant_2->value != 0 ? constant_1->value / constant_2->value : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacRemainder_t: {
            TInt value = constant_2->value != 0 ? constant_1->value % constant_2->value : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacBitAnd_t: {
            TInt value = constant_1->value & constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacBitOr_t: {
            TInt value = constant_1->value | constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacBitXor_t: {
            TInt value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacBitShiftLeft_t: {
            TInt value = constant_1->value << constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TInt value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_binary_long_const(
    TacBinaryOp* node, CConstLong* constant_1, CConstLong* constant_2) {
    switch (node->type()) {
        case AST_TacAdd_t: {
            TLong value = constant_1->value + constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacSubtract_t: {
            TLong value = constant_1->value - constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacMultiply_t: {
            TLong value = constant_1->value * constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacDivide_t: {
            TLong value = constant_2->value != 0l ? constant_1->value / constant_2->value : 0l;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacRemainder_t: {
            TLong value = constant_2->value != 0l ? constant_1->value % constant_2->value : 0l;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacBitAnd_t: {
            TLong value = constant_1->value & constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacBitOr_t: {
            TLong value = constant_1->value | constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacBitXor_t: {
            TLong value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacBitShiftLeft_t: {
            TLong value = constant_1->value << constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TLong value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_binary_dbl_const(
    TacBinaryOp* node, CConstDouble* constant_1, CConstDouble* constant_2) {
    switch (node->type()) {
        case AST_TacAdd_t: {
            TDouble value = constant_1->value + constant_2->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_TacSubtract_t: {
            TDouble value = constant_1->value - constant_2->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_TacMultiply_t: {
            TDouble value = constant_1->value * constant_2->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_TacDivide_t: {
            TDouble value = constant_1->value / constant_2->value;
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_binary_uint_const(
    TacBinaryOp* node, CConstUInt* constant_1, CConstUInt* constant_2) {
    switch (node->type()) {
        case AST_TacAdd_t: {
            TUInt value = constant_1->value + constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacSubtract_t: {
            TUInt value = constant_1->value - constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacMultiply_t: {
            TUInt value = constant_1->value * constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacDivide_t: {
            TUInt value = constant_2->value != 0u ? constant_1->value / constant_2->value : 0u;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacRemainder_t: {
            TUInt value = constant_2->value != 0u ? constant_1->value % constant_2->value : 0u;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacBitAnd_t: {
            TUInt value = constant_1->value & constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacBitOr_t: {
            TUInt value = constant_1->value | constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacBitXor_t: {
            TUInt value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacBitShiftLeft_t: {
            TUInt value = constant_1->value << constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TUInt value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstUInt>(std::move(value));
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_binary_ulong_const(
    TacBinaryOp* node, CConstULong* constant_1, CConstULong* constant_2) {
    switch (node->type()) {
        case AST_TacAdd_t: {
            TULong value = constant_1->value + constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacSubtract_t: {
            TULong value = constant_1->value - constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacMultiply_t: {
            TULong value = constant_1->value * constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacDivide_t: {
            TULong value = constant_2->value != 0ul ? constant_1->value / constant_2->value : 0ul;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacRemainder_t: {
            TULong value = constant_2->value != 0ul ? constant_1->value % constant_2->value : 0ul;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacBitAnd_t: {
            TULong value = constant_1->value & constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacBitOr_t: {
            TULong value = constant_1->value | constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacBitXor_t: {
            TULong value = constant_1->value ^ constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacBitShiftLeft_t: {
            TULong value = constant_1->value << constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TULong value = constant_1->value >> constant_2->value;
            return std::make_shared<CConstULong>(std::move(value));
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return std::make_shared<CConstInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_binary_const(TacBinaryOp* node, CConst* constant_1, CConst* constant_2) {
    std::shared_ptr<CConst> fold_constant;
    if (constant_1->type() != constant_2->type()) {
        RAISE_INTERNAL_ERROR;
    }
    switch (constant_1->type()) {
        case AST_CConstInt_t: {
            fold_constant =
                fold_binary_int_const(node, static_cast<CConstInt*>(constant_1), static_cast<CConstInt*>(constant_2));
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_binary_long_const(
                node, static_cast<CConstLong*>(constant_1), static_cast<CConstLong*>(constant_2));
            break;
        }
        case AST_CConstDouble_t: {
            fold_constant = fold_binary_dbl_const(
                node, static_cast<CConstDouble*>(constant_1), static_cast<CConstDouble*>(constant_2));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_binary_uint_const(
                node, static_cast<CConstUInt*>(constant_1), static_cast<CConstUInt*>(constant_2));
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_binary_ulong_const(
                node, static_cast<CConstULong*>(constant_1), static_cast<CConstULong*>(constant_2));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_binary_instr(TacBinary* node, size_t instruction_index) {
    if (node->src1->type() == AST_TacConstant_t && node->src2->type() == AST_TacConstant_t) {
        std::shared_ptr<TacValue> src =
            fold_binary_const(node->binary_op.get(), static_cast<TacConstant*>(node->src1.get())->constant.get(),
                static_cast<TacConstant*>(node->src2.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(std::make_unique<TacCopy>(std::move(src), std::move(dst)), instruction_index);
    }
}

static std::shared_ptr<CConst> fold_copy_char_const(TacVariable* node, CConstChar* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t:
            return nullptr;
        case AST_UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_copy_int_const(TacVariable* node, CConstInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Int_t:
            return nullptr;
        case AST_UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_copy_long_const(TacVariable* node, CConstLong* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Long_t:
        case AST_Pointer_t:
            return nullptr;
        case AST_Double_t: {
            TDouble value = static_cast<TDouble>(constant->value);
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_copy_dbl_const(TacVariable* node, CConstDouble* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Long_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_Double_t:
            return nullptr;
        case AST_ULong_t: {
            TULong value = static_cast<TULong>(constant->value);
            return std::make_shared<CConstULong>(std::move(value));
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_copy_uchar_const(TacVariable* node, CConstUChar* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_UChar_t:
            return nullptr;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_copy_uint_const(TacVariable* node, CConstUInt* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_UInt_t:
            return nullptr;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<CConst> fold_copy_ulong_const(TacVariable* node, CConstULong* constant) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Long_t: {
            TLong value = static_cast<TLong>(constant->value);
            return std::make_shared<CConstLong>(std::move(value));
        }
        case AST_Double_t: {
            TDouble value = static_cast<TDouble>(constant->value);
            return std::make_shared<CConstDouble>(std::move(value));
        }
        case AST_Pointer_t:
        case AST_ULong_t:
            return nullptr;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_copy_const(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstChar_t: {
            fold_constant = fold_copy_char_const(node, static_cast<CConstChar*>(constant));
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_copy_int_const(node, static_cast<CConstInt*>(constant));
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_copy_long_const(node, static_cast<CConstLong*>(constant));
            break;
        }
        case AST_CConstDouble_t: {
            fold_constant = fold_copy_dbl_const(node, static_cast<CConstDouble*>(constant));
            break;
        }
        case AST_CConstUChar_t: {
            fold_constant = fold_copy_uchar_const(node, static_cast<CConstUChar*>(constant));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_copy_uint_const(node, static_cast<CConstUInt*>(constant));
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_copy_ulong_const(node, static_cast<CConstULong*>(constant));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    if (fold_constant) {
        return std::make_shared<TacConstant>(std::move(fold_constant));
    }
    else {
        return nullptr;
    }
}

static void fold_copy_instr(TacCopy* node) {
    if (node->src->type() == AST_TacConstant_t) {
        if (node->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_copy_const(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        if (src) {
            node->src = std::move(src);
            context->is_fixed_point = false;
        }
    }
}

static bool is_const_zero(CConst* constant) {
    switch (constant->type()) {
        case AST_CConstChar_t:
            return static_cast<CConstChar*>(constant)->value == 0;
        case AST_CConstInt_t:
            return static_cast<CConstInt*>(constant)->value == 0;
        case AST_CConstLong_t:
            return static_cast<CConstLong*>(constant)->value == 0l;
        case AST_CConstDouble_t:
            return static_cast<CConstDouble*>(constant)->value == 0.0;
        case AST_CConstUChar_t:
            return static_cast<CConstUChar*>(constant)->value == 0;
        case AST_CConstUInt_t:
            return static_cast<CConstUInt*>(constant)->value == 0u;
        case AST_CConstULong_t:
            return static_cast<CConstULong*>(constant)->value == 0ul;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void fold_jmp_eq_0_instr(TacJumpIfZero* node, size_t instruction_index) {
    if (node->condition->type() == AST_TacConstant_t) {
        if (is_const_zero(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            TIdentifier target = node->target;
            set_instr(std::make_unique<TacJump>(std::move(target)), instruction_index);
        }
        else {
            set_instr(nullptr, instruction_index);
        }
    }
}

static void fold_jmp_ne_0_instr(TacJumpIfNotZero* node, size_t instruction_index) {
    if (node->condition->type() == AST_TacConstant_t) {
        if (is_const_zero(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            set_instr(nullptr, instruction_index);
        }
        else {
            TIdentifier target = node->target;
            set_instr(std::make_unique<TacJump>(std::move(target)), instruction_index);
        }
    }
}

static void fold_instr(size_t instruction_index) {
    TacInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_TacSignExtend_t:
            fold_sign_extend_instr(static_cast<TacSignExtend*>(node), instruction_index);
            break;
        case AST_TacTruncate_t:
            fold_truncate_instr(static_cast<TacTruncate*>(node), instruction_index);
            break;
        case AST_TacZeroExtend_t:
            fold_zero_extend_instr(static_cast<TacZeroExtend*>(node), instruction_index);
            break;
        case AST_TacDoubleToInt_t:
            fold_dbl_to_signed_instr(static_cast<TacDoubleToInt*>(node), instruction_index);
            break;
        case AST_TacDoubleToUInt_t:
            fold_dbl_to_unsigned_instr(static_cast<TacDoubleToUInt*>(node), instruction_index);
            break;
        case AST_TacIntToDouble_t:
            fold_signed_to_dbl_instr(static_cast<TacIntToDouble*>(node), instruction_index);
            break;
        case AST_TacUIntToDouble_t:
            fold_unsigned_to_dbl_instr(static_cast<TacUIntToDouble*>(node), instruction_index);
            break;
        case AST_TacUnary_t:
            fold_unary_instr(static_cast<TacUnary*>(node), instruction_index);
            break;
        case AST_TacBinary_t:
            fold_binary_instr(static_cast<TacBinary*>(node), instruction_index);
            break;
        case AST_TacCopy_t:
            fold_copy_instr(static_cast<TacCopy*>(node));
            break;
        case AST_TacJumpIfZero_t:
            fold_jmp_eq_0_instr(static_cast<TacJumpIfZero*>(node), instruction_index);
            break;
        case AST_TacJumpIfNotZero_t:
            fold_jmp_ne_0_instr(static_cast<TacJumpIfNotZero*>(node), instruction_index);
            break;
        default:
            break;
    }
}

static void fold_constants() {
    for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
        if (GET_INSTR(instruction_index)) {
            fold_instr(instruction_index);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unreachable code elimination

static void unreach_reachable_block(size_t block_id);

static void unreach_succ_reachable_blocks(size_t block_id) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        unreach_reachable_block(successor_id);
    }
}

static void unreach_reachable_block(size_t block_id) {
    if (block_id < context->control_flow_graph->exit_id && !context->control_flow_graph->reaching_code[block_id]) {
        context->control_flow_graph->reaching_code[block_id] = true;
        unreach_succ_reachable_blocks(block_id);
    }
}

static void unreach_empty_block(size_t block_id) {
    for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
         instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
        if (GET_INSTR(instruction_index)) {
            set_instr(nullptr, instruction_index);
        }
    }
    GET_CFG_BLOCK(block_id).size = 0;
    cfg_rm_empty_block(block_id, false);
    GET_CFG_BLOCK(block_id).successor_ids.clear();
    GET_CFG_BLOCK(block_id).predecessor_ids.clear();
}

static void unreach_jump_instr(size_t block_id) {
    TacInstruction* node = GET_INSTR(GET_CFG_BLOCK(block_id).instructions_back_index).get();
    switch (node->type()) {
        case AST_TacJump_t:
        case AST_TacJumpIfZero_t:
        case AST_TacJumpIfNotZero_t:
            cfg_rm_block_instr(GET_CFG_BLOCK(block_id).instructions_back_index, block_id);
            break;
        default:
            break;
    }
}

static void unreach_jump_block(size_t block_id, size_t next_block_id) {
    if (GET_CFG_BLOCK(block_id).successor_ids.size() == 1
        && GET_CFG_BLOCK(block_id).successor_ids[0] == next_block_id) {
        unreach_jump_instr(block_id);
    }
}

static void unreach_label_instr(size_t block_id) {
    TacInstruction* node = GET_INSTR(GET_CFG_BLOCK(block_id).instructions_front_index).get();
    if (node->type() != AST_TacLabel_t) {
        RAISE_INTERNAL_ERROR;
    }
    cfg_rm_block_instr(GET_CFG_BLOCK(block_id).instructions_front_index, block_id);
}

static void unreach_label_block(size_t block_id, size_t previous_block_id) {
    if (GET_CFG_BLOCK(block_id).predecessor_ids.size() == 1
        && GET_CFG_BLOCK(block_id).predecessor_ids[0] == previous_block_id) {
        unreach_label_instr(block_id);
    }
}

static void eliminate_unreachable_code() {
    if (context->control_flow_graph->blocks.empty()) {
        return;
    }
    if (context->control_flow_graph->reaching_code.size() < context->control_flow_graph->blocks.size()) {
        context->control_flow_graph->reaching_code.resize(context->control_flow_graph->blocks.size());
    }
    std::fill(context->control_flow_graph->reaching_code.begin(),
        context->control_flow_graph->reaching_code.begin() + context->control_flow_graph->blocks.size(), false);
    for (size_t successor_id : context->control_flow_graph->entry_successor_ids) {
        unreach_reachable_block(successor_id);
    }

    size_t block_id = context->control_flow_graph->blocks.size();
    size_t next_block_id = context->control_flow_graph->exit_id;
    while (block_id-- > 0) {
        if (context->control_flow_graph->reaching_code[block_id]) {
            next_block_id = block_id;
            break;
        }
        else {
            unreach_empty_block(block_id);
        }
    }
    while (block_id-- > 0) {
        if (context->control_flow_graph->reaching_code[block_id]) {
            unreach_jump_block(block_id, next_block_id);
            next_block_id = block_id;
        }
        else {
            unreach_empty_block(block_id);
        }
    }

    for (auto& label_id : context->control_flow_graph->identifier_id_map) {
        if (context->control_flow_graph->reaching_code[label_id.second]) {
            for (block_id = label_id.second; block_id-- > 0;) {
                if (context->control_flow_graph->reaching_code[block_id]) {
                    next_block_id = block_id;
                    goto Lelse;
                }
            }
            next_block_id = context->control_flow_graph->entry_id;
        Lelse:
            unreach_label_block(label_id.second, next_block_id);
        }
        else {
            label_id.second = context->control_flow_graph->exit_id;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy propagation

static bool is_aliased_value(TacValue* node) {
    return node->type() == AST_TacVariable_t && is_aliased_name(static_cast<TacVariable*>(node)->name);
}

static bool is_const_signed(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstChar_t:
        case AST_CConstInt_t:
        case AST_CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_signed(TacVariable* node) {
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_Int_t:
        case AST_Long_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_signed(TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return is_const_signed(static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return is_var_signed(static_cast<TacVariable*>(node));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_const_null_ptr(TacConstant* node) {
    switch (node->constant->type()) {
        case AST_CConstInt_t:
            return static_cast<CConstInt*>(node->constant.get())->value == 0;
        case AST_CConstLong_t:
            return static_cast<CConstLong*>(node->constant.get())->value == 0l;
        case AST_CConstUInt_t:
            return static_cast<CConstUInt*>(node->constant.get())->value == 0u;
        case AST_CConstULong_t:
            return static_cast<CConstULong*>(node->constant.get())->value == 0ul;
        default:
            return false;
    }
}

static bool is_dbl_same_const(CConstDouble* constant_1, CConstDouble* constant_2) {
    if (constant_1->value == constant_2->value) {
        if (constant_1->value != 0.0) {
            return true;
        }
        else {
            return dbl_to_binary(constant_1->value) == dbl_to_binary(constant_2->value);
        }
    }
    else if (constant_1->value != constant_1->value && constant_2->value != constant_2->value) {
        return true;
    }
    else {
        return false;
    }
}

static bool is_same_const(TacConstant* node_1, TacConstant* node_2) {
    if (node_1->constant->type() == node_2->constant->type()) {
        switch (node_1->constant->type()) {
            case AST_CConstChar_t:
                return static_cast<CConstChar*>(node_1->constant.get())->value
                       == static_cast<CConstChar*>(node_2->constant.get())->value;
            case AST_CConstInt_t:
                return static_cast<CConstInt*>(node_1->constant.get())->value
                       == static_cast<CConstInt*>(node_2->constant.get())->value;
            case AST_CConstLong_t:
                return static_cast<CConstLong*>(node_1->constant.get())->value
                       == static_cast<CConstLong*>(node_2->constant.get())->value;
            case AST_CConstDouble_t:
                return is_dbl_same_const(static_cast<CConstDouble*>(node_1->constant.get()),
                    static_cast<CConstDouble*>(node_2->constant.get()));
            case AST_CConstUChar_t:
                return static_cast<CConstUChar*>(node_1->constant.get())->value
                       == static_cast<CConstUChar*>(node_2->constant.get())->value;
            case AST_CConstUInt_t:
                return static_cast<CConstUInt*>(node_1->constant.get())->value
                       == static_cast<CConstUInt*>(node_2->constant.get())->value;
            case AST_CConstULong_t:
                return static_cast<CConstULong*>(node_1->constant.get())->value
                       == static_cast<CConstULong*>(node_2->constant.get())->value;
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    return false;
}

static bool is_same_var(TacVariable* node_1, TacVariable* node_2) { return node_1->name == node_2->name; }

static bool is_same_value(TacValue* node_1, TacValue* node_2) {
    if (node_1->type() == node_2->type()) {
        switch (node_1->type()) {
            case AST_TacConstant_t:
                return is_same_const(static_cast<TacConstant*>(node_1), static_cast<TacConstant*>(node_2));
            case AST_TacVariable_t:
                return is_same_var(static_cast<TacVariable*>(node_1), static_cast<TacVariable*>(node_2));
            default:
                RAISE_INTERNAL_ERROR;
        }
    }
    return false;
}

static bool is_same_name(TacValue* node, TIdentifier name) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return false;
        case AST_TacVariable_t:
            return static_cast<TacVariable*>(node)->name == name;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool is_copy_same_signedness(TacCopy* node) {
    return is_value_signed(node->src.get()) == is_value_signed(node->dst.get());
}

static bool is_copy_null_ptr(TacCopy* node) {
    if (node->src->type() == AST_TacConstant_t && node->dst->type() == AST_TacVariable_t
        && frontend->symbol_table[static_cast<TacVariable*>(node->dst.get())->name]->type_t->type() == AST_Pointer_t) {
        return is_const_null_ptr(static_cast<TacConstant*>(node->src.get()));
    }
    else {
        return false;
    }
}

static void prop_transfer_dst_value(TacValue* node, size_t next_instruction_index) {
    if (node->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    size_t i = 0;
    for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instruction_index, i)) {
                if (GET_DFA_INSTR(i)->type() != AST_TacCopy_t) {
                    RAISE_INTERNAL_ERROR;
                }
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                if (copy->dst->type() != AST_TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_value(node, copy->src.get()) || is_same_value(node, copy->dst.get())) {
                    SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static void prop_transfer_call(TacFunCall* node, size_t next_instruction_index) {
    if (node->dst && node->dst->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    size_t i = 0;
    for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instruction_index, i)) {
                if (GET_DFA_INSTR(i)->type() != AST_TacCopy_t) {
                    RAISE_INTERNAL_ERROR;
                }
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                if (copy->dst->type() != AST_TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_aliased_value(copy->src.get()) || is_aliased_value(copy->dst.get())
                         || (node->dst
                             && (is_same_value(node->dst.get(), copy->src.get())
                                 || is_same_value(node->dst.get(), copy->dst.get())))) {
                    SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static bool prop_transfer_copy(TacCopy* node, size_t next_instruction_index) {
    if (node->dst->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (GET_DFA_INSTR(i)->type() != AST_TacCopy_t) {
            RAISE_INTERNAL_ERROR;
        }
        TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
        if (copy->dst->type() != AST_TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        else if (is_same_value(node->dst.get(), copy->dst.get())) {
            if ((is_copy_same_signedness(copy) || is_copy_null_ptr(copy))
                && is_same_value(node->src.get(), copy->src.get())) {
                SET_DFA_INSTR_SET_AT(next_instruction_index, i, true);
            }
            else {
                SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
            }
        }
        else if (GET_DFA_INSTR_SET_AT(next_instruction_index, i)) {
            if (is_same_value(node->dst.get(), copy->src.get())) {
                if (is_same_value(node->src.get(), copy->dst.get())) {
                    return false;
                }
                else {
                    SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
                }
            }
        }
    }
    return true;
}

static void prop_transfer_store(size_t next_instruction_index) {
    size_t i = 0;
    for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instruction_index, i)) {
                if (GET_DFA_INSTR(i)->type() != AST_TacCopy_t) {
                    RAISE_INTERNAL_ERROR;
                }
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                if (copy->dst->type() != AST_TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_aliased_value(copy->src.get()) || is_aliased_value(copy->dst.get())) {
                    SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static void prop_transfer_cp_to_offset(TacCopyToOffset* node, size_t next_instruction_index) {
    size_t i = 0;
    for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instruction_index, i)) {
                if (GET_DFA_INSTR(i)->type() != AST_TacCopy_t) {
                    RAISE_INTERNAL_ERROR;
                }
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                if (copy->dst->type() != AST_TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_name(copy->src.get(), node->dst_name)
                         || is_same_name(copy->dst.get(), node->dst_name)) {
                    SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instruction_index, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static bool prop_transfer_reach_copies(size_t instruction_index, size_t next_instruction_index) {
    TacInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_TacSignExtend_t:
            prop_transfer_dst_value(static_cast<TacSignExtend*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacTruncate_t:
            prop_transfer_dst_value(static_cast<TacTruncate*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacZeroExtend_t:
            prop_transfer_dst_value(static_cast<TacZeroExtend*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacDoubleToInt_t:
            prop_transfer_dst_value(static_cast<TacDoubleToInt*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacDoubleToUInt_t:
            prop_transfer_dst_value(static_cast<TacDoubleToUInt*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacIntToDouble_t:
            prop_transfer_dst_value(static_cast<TacIntToDouble*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacUIntToDouble_t:
            prop_transfer_dst_value(static_cast<TacUIntToDouble*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacFunCall_t:
            prop_transfer_call(static_cast<TacFunCall*>(node), next_instruction_index);
            break;
        case AST_TacUnary_t:
            prop_transfer_dst_value(static_cast<TacUnary*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacBinary_t:
            prop_transfer_dst_value(static_cast<TacBinary*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacCopy_t:
            return prop_transfer_copy(static_cast<TacCopy*>(node), next_instruction_index);
        case AST_TacGetAddress_t:
            prop_transfer_dst_value(static_cast<TacGetAddress*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacLoad_t:
            prop_transfer_dst_value(static_cast<TacLoad*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacStore_t:
            prop_transfer_store(next_instruction_index);
            break;
        case AST_TacAddPtr_t:
            prop_transfer_dst_value(static_cast<TacAddPtr*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacCopyToOffset_t:
            prop_transfer_cp_to_offset(static_cast<TacCopyToOffset*>(node), next_instruction_index);
            break;
        case AST_TacCopyFromOffset_t:
            prop_transfer_dst_value(static_cast<TacCopyFromOffset*>(node)->dst.get(), next_instruction_index);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
    return true;
}

static TacCopy* get_dfa_bak_copy_instr(size_t i) {
    TacInstruction* node = get_dfa_bak_instr(i);
    if (node->type() != AST_TacCopy_t) {
        RAISE_INTERNAL_ERROR;
    }
    return static_cast<TacCopy*>(node);
}

static void set_dfa_bak_copy_instr(TacCopy* node, size_t instruction_index) {
    size_t i;
    if (set_dfa_bak_instr(instruction_index, i)) {
        std::shared_ptr<TacValue> src = node->src;
        std::shared_ptr<TacValue> dst = node->dst;
        context->data_flow_analysis_o1->bak_instructions[i] = std::make_unique<TacCopy>(std::move(src), std::move(dst));
    }
}

static void prop_ret_instr(TacReturn* node, size_t incoming_index, bool exit_block) {
    if (node->val && node->val->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if ((exit_block && GET_DFA_BLOCK_SET_MASK(incoming_index, j) == MASK_FALSE)
                || (!exit_block && GET_DFA_INSTR_SET_MASK(incoming_index, j) == MASK_FALSE)) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (((exit_block && GET_DFA_BLOCK_SET_AT(incoming_index, i))
                        || (!exit_block && GET_DFA_INSTR_SET_AT(incoming_index, i)))) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->val.get(), copy->dst.get())) {
                        node->val = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_sign_extend_instr(TacSignExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_truncate_instr(TacTruncate* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_zero_extend_instr(TacZeroExtend* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_dbl_to_int_instr(TacDoubleToInt* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_dbl_to_uint_instr(TacDoubleToUInt* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_int_to_dbl_instr(TacIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_uint_to_dbl_instr(TacUIntToDouble* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_call_instr(TacFunCall* node, size_t instruction_index) {
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (node->args[i]->type() == AST_TacVariable_t) {
            size_t j = 0;
            for (size_t k = 0; k < context->data_flow_analysis->mask_size; ++k) {
                if (GET_DFA_INSTR_SET_MASK(instruction_index, k) == MASK_FALSE) {
                    j += 64;
                    continue;
                }
                size_t mask_set_size = j + 64;
                if (mask_set_size > context->data_flow_analysis->set_size) {
                    mask_set_size = context->data_flow_analysis->set_size;
                }
                for (; j < mask_set_size; ++j) {
                    if (GET_DFA_INSTR_SET_AT(instruction_index, j)) {
                        TacCopy* copy = get_dfa_bak_copy_instr(j);
                        if (copy->dst->type() != AST_TacVariable_t) {
                            RAISE_INTERNAL_ERROR;
                        }
                        else if (is_same_value(node->args[i].get(), copy->dst.get())) {
                            node->args[i] = copy->src;
                            context->is_fixed_point = false;
                            goto Lbreak;
                        }
                    }
                }
            }
        Lbreak:;
        }
    }
}

static void prop_unary_instr(TacUnary* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_binary_instr(TacBinary* node, size_t instruction_index) {
    bool is_src1 = node->src1->type() == AST_TacVariable_t;
    bool is_src2 = node->src2->type() == AST_TacVariable_t;
    if (is_src1 || is_src2) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_src1 && is_same_value(node->src1.get(), copy->dst.get())) {
                        node->src1 = copy->src;
                        context->is_fixed_point = false;
                        is_src1 = false;
                        if (!is_src2) {
                            return;
                        }
                    }
                    if (is_src2 && is_same_value(node->src2.get(), copy->dst.get())) {
                        node->src2 = copy->src;
                        context->is_fixed_point = false;
                        is_src2 = false;
                        if (!is_src1) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

static void prop_copy_instr(TacCopy* node, size_t instruction_index, size_t block_id) {
    if (node->dst->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    size_t i = 0;
    for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instr(i);
                if (copy->dst->type() != AST_TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (context->data_flow_analysis_o1->data_index_map[i] == instruction_index
                         || (is_same_value(node->src.get(), copy->dst.get())
                             && is_same_value(node->dst.get(), copy->src.get()))) {
                    set_dfa_bak_copy_instr(node, instruction_index);
                    cfg_rm_block_instr(instruction_index, block_id);
                    return;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    set_dfa_bak_copy_instr(node, instruction_index);
                    node->src = copy->src;
                    context->is_fixed_point = false;
                    return;
                }
            }
        }
    }
}

static void prop_load_instr(TacLoad* node, size_t instruction_index) {
    if (node->src_ptr->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src_ptr.get(), copy->dst.get())) {
                        node->src_ptr = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_store_instr(TacStore* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_add_ptr_instr(TacAddPtr* node, size_t instruction_index) {
    bool is_src_ptr = node->src_ptr->type() == AST_TacVariable_t;
    bool is_index = node->index->type() == AST_TacVariable_t;
    if (is_src_ptr || is_index) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_src_ptr && is_same_value(node->src_ptr.get(), copy->dst.get())) {
                        node->src_ptr = copy->src;
                        context->is_fixed_point = false;
                        is_src_ptr = false;
                        if (!is_index) {
                            return;
                        }
                    }
                    if (is_index && is_same_value(node->index.get(), copy->dst.get())) {
                        node->index = copy->src;
                        context->is_fixed_point = false;
                        is_index = false;
                        if (!is_src_ptr) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

static void prop_cp_to_offset_instr(TacCopyToOffset* node, size_t instruction_index) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_cp_from_offset_instr(TacCopyFromOffset* node, size_t instruction_index) {
    size_t i = 0;
    for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instruction_index, i)) {
                TacCopy* copy = get_dfa_bak_copy_instr(i);
                if (copy->dst->type() != AST_TacVariable_t) {
                    RAISE_INTERNAL_ERROR;
                }
                else if (is_same_name(copy->dst.get(), node->src_name)) {
                    if (copy->src->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    node->src_name = static_cast<TacVariable*>(copy->src.get())->name;
                    context->is_fixed_point = false;
                    return;
                }
            }
        }
    }
}

static void prop_jmp_eq_0_instr(TacJumpIfZero* node, size_t incoming_index, size_t exit_block) {
    if (node->condition->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if ((exit_block && GET_DFA_BLOCK_SET_MASK(incoming_index, j) == MASK_FALSE)
                || (!exit_block && GET_DFA_INSTR_SET_MASK(incoming_index, j) == MASK_FALSE)) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (((exit_block && GET_DFA_BLOCK_SET_AT(incoming_index, i))
                        || (!exit_block && GET_DFA_INSTR_SET_AT(incoming_index, i)))) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->condition.get(), copy->dst.get())) {
                        node->condition = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_jmp_ne_0_instr(TacJumpIfNotZero* node, size_t incoming_index, size_t exit_block) {
    if (node->condition->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < context->data_flow_analysis->mask_size; ++j) {
            if ((exit_block && GET_DFA_BLOCK_SET_MASK(incoming_index, j) == MASK_FALSE)
                || (!exit_block && GET_DFA_INSTR_SET_MASK(incoming_index, j) == MASK_FALSE)) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > context->data_flow_analysis->set_size) {
                mask_set_size = context->data_flow_analysis->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (((exit_block && GET_DFA_BLOCK_SET_AT(incoming_index, i))
                        || (!exit_block && GET_DFA_INSTR_SET_AT(incoming_index, i)))) {
                    TacCopy* copy = get_dfa_bak_copy_instr(i);
                    if (copy->dst->type() != AST_TacVariable_t) {
                        RAISE_INTERNAL_ERROR;
                    }
                    else if (is_same_value(node->condition.get(), copy->dst.get())) {
                        node->condition = copy->src;
                        context->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_instr(size_t instruction_index, size_t copy_instruction_index, size_t block_id) {
    TacInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_TacReturn_t:
            prop_ret_instr(static_cast<TacReturn*>(node), copy_instruction_index, block_id > 0);
            break;
        case AST_TacSignExtend_t:
            prop_sign_extend_instr(static_cast<TacSignExtend*>(node), copy_instruction_index);
            break;
        case AST_TacTruncate_t:
            prop_truncate_instr(static_cast<TacTruncate*>(node), copy_instruction_index);
            break;
        case AST_TacZeroExtend_t:
            prop_zero_extend_instr(static_cast<TacZeroExtend*>(node), copy_instruction_index);
            break;
        case AST_TacDoubleToInt_t:
            prop_dbl_to_int_instr(static_cast<TacDoubleToInt*>(node), copy_instruction_index);
            break;
        case AST_TacDoubleToUInt_t:
            prop_dbl_to_uint_instr(static_cast<TacDoubleToUInt*>(node), copy_instruction_index);
            break;
        case AST_TacIntToDouble_t:
            prop_int_to_dbl_instr(static_cast<TacIntToDouble*>(node), copy_instruction_index);
            break;
        case AST_TacUIntToDouble_t:
            prop_uint_to_dbl_instr(static_cast<TacUIntToDouble*>(node), copy_instruction_index);
            break;
        case AST_TacFunCall_t:
            prop_call_instr(static_cast<TacFunCall*>(node), copy_instruction_index);
            break;
        case AST_TacUnary_t:
            prop_unary_instr(static_cast<TacUnary*>(node), copy_instruction_index);
            break;
        case AST_TacBinary_t:
            prop_binary_instr(static_cast<TacBinary*>(node), copy_instruction_index);
            break;
        case AST_TacCopy_t:
            prop_copy_instr(static_cast<TacCopy*>(node), copy_instruction_index, block_id);
            break;
        case AST_TacLoad_t:
            prop_load_instr(static_cast<TacLoad*>(node), copy_instruction_index);
            break;
        case AST_TacStore_t:
            prop_store_instr(static_cast<TacStore*>(node), copy_instruction_index);
            break;
        case AST_TacAddPtr_t:
            prop_add_ptr_instr(static_cast<TacAddPtr*>(node), copy_instruction_index);
            break;
        case AST_TacCopyToOffset_t:
            prop_cp_to_offset_instr(static_cast<TacCopyToOffset*>(node), copy_instruction_index);
            break;
        case AST_TacCopyFromOffset_t:
            prop_cp_from_offset_instr(static_cast<TacCopyFromOffset*>(node), copy_instruction_index);
            break;
        case AST_TacJumpIfZero_t:
            prop_jmp_eq_0_instr(static_cast<TacJumpIfZero*>(node), copy_instruction_index, block_id > 0);
            break;
        case AST_TacJumpIfNotZero_t:
            prop_jmp_ne_0_instr(static_cast<TacJumpIfNotZero*>(node), copy_instruction_index, block_id > 0);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void propagate_copies() {
    if (!init_data_flow_analysis(false, true)) {
        return;
    }
    dfa_forward_iter_alg();

    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            size_t incoming_index = block_id;
            size_t exit_block = 1;
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_back_index + 1;
                 instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index;) {
                if (GET_INSTR(instruction_index)) {
                    switch (GET_INSTR(instruction_index)->type()) {
                        case AST_TacReturn_t:
                        case AST_TacJumpIfZero_t:
                        case AST_TacJumpIfNotZero_t:
                            prop_instr(instruction_index, incoming_index, exit_block);
                            break;
                        case AST_TacSignExtend_t:
                        case AST_TacTruncate_t:
                        case AST_TacZeroExtend_t:
                        case AST_TacDoubleToInt_t:
                        case AST_TacDoubleToUInt_t:
                        case AST_TacIntToDouble_t:
                        case AST_TacUIntToDouble_t:
                        case AST_TacFunCall_t:
                        case AST_TacUnary_t:
                        case AST_TacBinary_t:
                        case AST_TacCopy_t:
                        case AST_TacLoad_t:
                        case AST_TacStore_t:
                        case AST_TacAddPtr_t:
                        case AST_TacCopyToOffset_t:
                        case AST_TacCopyFromOffset_t: {
                            prop_instr(instruction_index, instruction_index, block_id);
                            incoming_index = instruction_index;
                            exit_block = 0;
                            break;
                        }
                        case AST_TacGetAddress_t: {
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

static void elim_transfer_addressed(size_t next_instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(next_instruction_index, i) |=
            GET_DFA_INSTR_SET_MASK(context->data_flow_analysis_o1->addressed_index, i);
    }
}

static void elim_transfer_aliased(size_t next_instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(next_instruction_index, i) |=
            GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->static_index, i);
        GET_DFA_INSTR_SET_MASK(next_instruction_index, i) |=
            GET_DFA_INSTR_SET_MASK(context->data_flow_analysis_o1->addressed_index, i);
    }
}

static void elim_transfer_src_name(TIdentifier name, size_t next_instruction_index) {
    size_t i = context->control_flow_graph->identifier_id_map[name];
    SET_DFA_INSTR_SET_AT(next_instruction_index, i, true);
}

static void elim_transfer_src_value(TacValue* node, size_t next_instruction_index) {
    if (node->type() == AST_TacVariable_t) {
        elim_transfer_src_name(static_cast<TacVariable*>(node)->name, next_instruction_index);
    }
}

static void elim_transfer_dst_value(TacValue* node, size_t next_instruction_index) {
    if (node->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    size_t i = context->control_flow_graph->identifier_id_map[static_cast<TacVariable*>(node)->name];
    SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
}

static void elim_transfer_live_values(size_t instruction_index, size_t next_instruction_index) {
    TacInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_TacReturn_t: {
            TacReturn* p_node = static_cast<TacReturn*>(node);
            if (p_node->val) {
                elim_transfer_src_value(p_node->val.get(), next_instruction_index);
            }
            break;
        }
        case AST_TacSignExtend_t: {
            TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacTruncate_t: {
            TacTruncate* p_node = static_cast<TacTruncate*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacZeroExtend_t: {
            TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacDoubleToInt_t: {
            TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacDoubleToUInt_t: {
            TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacIntToDouble_t: {
            TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacUIntToDouble_t: {
            TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacFunCall_t: {
            TacFunCall* p_node = static_cast<TacFunCall*>(node);
            if (p_node->dst) {
                elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            }
            for (const auto& arg : p_node->args) {
                elim_transfer_src_value(arg.get(), next_instruction_index);
            }
            elim_transfer_aliased(next_instruction_index);
            break;
        }
        case AST_TacUnary_t: {
            TacUnary* p_node = static_cast<TacUnary*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacBinary_t: {
            TacBinary* p_node = static_cast<TacBinary*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src1.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src2.get(), next_instruction_index);
            break;
        }
        case AST_TacCopy_t: {
            TacCopy* p_node = static_cast<TacCopy*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_TacGetAddress_t:
            elim_transfer_dst_value(static_cast<TacGetAddress*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_TacLoad_t: {
            TacLoad* p_node = static_cast<TacLoad*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src_ptr.get(), next_instruction_index);
            elim_transfer_addressed(next_instruction_index);
            break;
        }
        case AST_TacStore_t: {
            TacStore* p_node = static_cast<TacStore*>(node);
            elim_transfer_src_value(p_node->src.get(), next_instruction_index);
            elim_transfer_src_value(p_node->dst_ptr.get(), next_instruction_index);
            break;
        }
        case AST_TacAddPtr_t: {
            TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_value(p_node->src_ptr.get(), next_instruction_index);
            elim_transfer_src_value(p_node->index.get(), next_instruction_index);
            break;
        }
        case AST_TacCopyToOffset_t:
            elim_transfer_src_value(static_cast<TacCopyToOffset*>(node)->src.get(), next_instruction_index);
            break;
        case AST_TacCopyFromOffset_t: {
            TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
            elim_transfer_dst_value(p_node->dst.get(), next_instruction_index);
            elim_transfer_src_name(p_node->src_name, next_instruction_index);
            break;
        }
        case AST_TacJumpIfZero_t:
            elim_transfer_src_value(static_cast<TacJumpIfZero*>(node)->condition.get(), next_instruction_index);
            break;
        case AST_TacJumpIfNotZero_t:
            elim_transfer_src_value(static_cast<TacJumpIfNotZero*>(node)->condition.get(), next_instruction_index);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void elim_dst_name_instr(TIdentifier name, size_t instruction_index) {
    size_t i = context->control_flow_graph->identifier_id_map[name];
    if (!GET_DFA_INSTR_SET_AT(instruction_index, i)) {
        set_instr(nullptr, instruction_index);
    }
}

static void elim_dst_value_instr(TacValue* node, size_t instruction_index) {
    if (node->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    elim_dst_name_instr(static_cast<TacVariable*>(node)->name, instruction_index);
}

static void elim_instr(size_t instruction_index) {
    TacInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_TacSignExtend_t:
            elim_dst_value_instr(static_cast<TacSignExtend*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacTruncate_t:
            elim_dst_value_instr(static_cast<TacTruncate*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacZeroExtend_t:
            elim_dst_value_instr(static_cast<TacZeroExtend*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacDoubleToInt_t:
            elim_dst_value_instr(static_cast<TacDoubleToInt*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacDoubleToUInt_t:
            elim_dst_value_instr(static_cast<TacDoubleToUInt*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacIntToDouble_t:
            elim_dst_value_instr(static_cast<TacIntToDouble*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacUIntToDouble_t:
            elim_dst_value_instr(static_cast<TacUIntToDouble*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacUnary_t:
            elim_dst_value_instr(static_cast<TacUnary*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacBinary_t:
            elim_dst_value_instr(static_cast<TacBinary*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacCopy_t:
            elim_dst_value_instr(static_cast<TacCopy*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacGetAddress_t:
            elim_dst_value_instr(static_cast<TacGetAddress*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacLoad_t:
            elim_dst_value_instr(static_cast<TacLoad*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacAddPtr_t:
            elim_dst_value_instr(static_cast<TacAddPtr*>(node)->dst.get(), instruction_index);
            break;
        case AST_TacCopyToOffset_t:
            elim_dst_name_instr(static_cast<TacCopyToOffset*>(node)->dst_name, instruction_index);
            break;
        case AST_TacCopyFromOffset_t:
            elim_dst_value_instr(static_cast<TacCopyFromOffset*>(node)->dst.get(), instruction_index);
            break;
        default:
            break;
    }
}

static void eliminate_dead_stores(bool is_addressed_set) {
    if (!init_data_flow_analysis(true, is_addressed_set)) {
        return;
    }
    dfa_iter_alg();

    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
                 instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTR(instruction_index)) {
                    elim_instr(instruction_index);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CONSTANT_FOLDING 0
#define COPY_PROPAGATION 1
#define UNREACHABLE_CODE_ELIMINATION 2
#define DEAD_STORE_ELIMINATION 3
#define CONTROL_FLOW_GRAPH 4

static void optim_fun_toplvl(TacFunction* node) {
    context->p_instructions = &node->body;
    do {
        context->is_fixed_point = true;
        if (context->enabled_optimizations[CONSTANT_FOLDING]) {
            fold_constants();
        }
        if (context->enabled_optimizations[CONTROL_FLOW_GRAPH]) {
            init_control_flow_graph();
            if (context->enabled_optimizations[UNREACHABLE_CODE_ELIMINATION]) {
                eliminate_unreachable_code();
            }
            if (context->enabled_optimizations[COPY_PROPAGATION]) {
                propagate_copies();
            }
            if (context->enabled_optimizations[DEAD_STORE_ELIMINATION]) {
                eliminate_dead_stores(!context->enabled_optimizations[COPY_PROPAGATION]);
            }
        }
    }
    while (!context->is_fixed_point);
    context->p_instructions = nullptr;
}

static void optim_toplvl(TacTopLevel* node) {
    if (node->type() == AST_TacFunction_t) {
        optim_fun_toplvl(static_cast<TacFunction*>(node));
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void optim_program(TacProgram* node) {
    for (const auto& top_level : node->function_top_levels) {
        optim_toplvl(top_level.get());
    }
    frontend->addressed_set.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void optimize_three_address_code(TacProgram* node, uint8_t optim_1_mask) {
    context = std::make_unique<OptimTacContext>(std::move(optim_1_mask));
    if (context->enabled_optimizations[CONTROL_FLOW_GRAPH]) {
        context->control_flow_graph = std::make_unique<ControlFlowGraph>();
        if (context->enabled_optimizations[COPY_PROPAGATION]
            || context->enabled_optimizations[DEAD_STORE_ELIMINATION]) {
            context->data_flow_analysis = std::make_unique<DataFlowAnalysis>();
            context->data_flow_analysis_o1 = std::make_unique<DataFlowAnalysisO1>();
        }
    }
    optim_program(node);
    context.reset();
}
