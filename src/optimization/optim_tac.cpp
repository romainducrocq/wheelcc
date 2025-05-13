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
    FrontEndContext* frontend;
    // Constant folding
    // Unreachable code elimination
    // Copy propagation
    // Dead store elimination
    bool is_fixed_point;
    std::array<bool, 5> enabled_optims;
    std::unique_ptr<ControlFlowGraph> cfg;
    std::unique_ptr<DataFlowAnalysis> dfa;
    std::unique_ptr<DataFlowAnalysisO1> dfa_o1;
    std::vector<std::unique_ptr<TacInstruction>>* p_instrs;
};

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

static std::shared_ptr<CConst> fold_sign_extend_char_const(Ctx ctx, TacVariable* node, CConstChar* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_sign_extend_int_const(Ctx ctx, TacVariable* node, CConstInt* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<TacConstant> fold_sign_extend_const(Ctx ctx, TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstChar_t: {
            fold_constant = fold_sign_extend_char_const(ctx, node, static_cast<CConstChar*>(constant));
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_sign_extend_int_const(ctx, node, static_cast<CConstInt*>(constant));
            break;
        }
        default:
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_sign_extend_instr(Ctx ctx, TacSignExtend* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
        std::shared_ptr<TacValue> src = fold_sign_extend_const(ctx, static_cast<TacVariable*>(node->dst.get()),
            static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<CConst> fold_truncate_int_const(Ctx ctx, TacVariable* node, CConstInt* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_truncate_long_const(Ctx ctx, TacVariable* node, CConstLong* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_truncate_uint_const(Ctx ctx, TacVariable* node, CConstUInt* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_truncate_ulong_const(Ctx ctx, TacVariable* node, CConstULong* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<TacConstant> fold_truncate_const(Ctx ctx, TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstInt_t: {
            fold_constant = fold_truncate_int_const(ctx, node, static_cast<CConstInt*>(constant));
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_truncate_long_const(ctx, node, static_cast<CConstLong*>(constant));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_truncate_uint_const(ctx, node, static_cast<CConstUInt*>(constant));
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_truncate_ulong_const(ctx, node, static_cast<CConstULong*>(constant));
            break;
        }
        default:
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_truncate_instr(Ctx ctx, TacTruncate* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
        std::shared_ptr<TacValue> src = fold_truncate_const(ctx, static_cast<TacVariable*>(node->dst.get()),
            static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<CConst> fold_zero_extend_uchar_const(Ctx ctx, TacVariable* node, CConstUChar* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_zero_extend_uint_const(Ctx ctx, TacVariable* node, CConstUInt* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<TacConstant> fold_zero_extend_const(Ctx ctx, TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstUChar_t: {
            fold_constant = fold_zero_extend_uchar_const(ctx, node, static_cast<CConstUChar*>(constant));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_zero_extend_uint_const(ctx, node, static_cast<CConstUInt*>(constant));
            break;
        }
        default:
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_zero_extend_instr(Ctx ctx, TacZeroExtend* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
        std::shared_ptr<TacValue> src = fold_zero_extend_const(ctx, static_cast<TacVariable*>(node->dst.get()),
            static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<TacConstant> fold_dbl_to_signed_const(Ctx ctx, TacVariable* node, CConst* constant) {
    THROW_ABORT_IF(constant->type() != AST_CConstDouble_t);
    std::shared_ptr<CConst> fold_constant;
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_dbl_to_signed_instr(Ctx ctx, TacDoubleToInt* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
        std::shared_ptr<TacValue> src = fold_dbl_to_signed_const(ctx, static_cast<TacVariable*>(node->dst.get()),
            static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<TacConstant> fold_dbl_to_unsigned_const(Ctx ctx, TacVariable* node, CConst* constant) {
    THROW_ABORT_IF(constant->type() != AST_CConstDouble_t);
    std::shared_ptr<CConst> fold_constant;
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_dbl_to_unsigned_instr(Ctx ctx, TacDoubleToUInt* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
        std::shared_ptr<TacValue> src = fold_dbl_to_unsigned_const(ctx, static_cast<TacVariable*>(node->dst.get()),
            static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<TacConstant> fold_signed_to_dbl_const(CConst* constant) {
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
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_signed_to_dbl_instr(Ctx ctx, TacIntToDouble* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t
                       || ctx->frontend->symbol_table[static_cast<TacVariable*>(node->dst.get())->name]->type_t->type()
                              != AST_Double_t);
        std::shared_ptr<TacValue> src =
            fold_signed_to_dbl_const(static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<TacConstant> fold_unsigned_to_dbl_const(CConst* constant) {
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
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_unsigned_to_dbl_instr(Ctx ctx, TacUIntToDouble* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t
                       || ctx->frontend->symbol_table[static_cast<TacVariable*>(node->dst.get())->name]->type_t->type()
                              != AST_Double_t);
        std::shared_ptr<TacValue> src =
            fold_unsigned_to_dbl_const(static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<CConst> fold_unary_char_const(TacUnaryOp* node, CConstChar* constant) {
    if (node->type() == AST_TacNot_t) {
        TInt value = !constant->value ? 1 : 0;
        return std::make_shared<CConstInt>(std::move(value));
    }
    else {
        THROW_ABORT;
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
            THROW_ABORT;
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
            THROW_ABORT;
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_unary_uchar_const(TacUnaryOp* node, CConstUChar* constant) {
    if (node->type() == AST_TacNot_t) {
        TInt value = !constant->value ? 1 : 0;
        return std::make_shared<CConstInt>(std::move(value));
    }
    else {
        THROW_ABORT;
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
            THROW_ABORT;
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
            THROW_ABORT;
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
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_unary_instr(Ctx ctx, TacUnary* node, size_t instr_idx) {
    if (node->src->type() == AST_TacConstant_t) {
        std::shared_ptr<TacValue> src =
            fold_unary_const(node->unop.get(), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
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
            THROW_ABORT;
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
            THROW_ABORT;
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
            THROW_ABORT;
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
            THROW_ABORT;
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
            THROW_ABORT;
    }
}

static std::shared_ptr<TacConstant> fold_binary_const(TacBinaryOp* node, CConst* constant_1, CConst* constant_2) {
    THROW_ABORT_IF(constant_1->type() != constant_2->type());
    std::shared_ptr<CConst> fold_constant;
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
            THROW_ABORT;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static void fold_binary_instr(Ctx ctx, TacBinary* node, size_t instr_idx) {
    if (node->src1->type() == AST_TacConstant_t && node->src2->type() == AST_TacConstant_t) {
        std::shared_ptr<TacValue> src =
            fold_binary_const(node->binop.get(), static_cast<TacConstant*>(node->src1.get())->constant.get(),
                static_cast<TacConstant*>(node->src2.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instr(ctx, std::make_unique<TacCopy>(std::move(src), std::move(dst)), instr_idx);
    }
}

static std::shared_ptr<CConst> fold_copy_char_const(Ctx ctx, TacVariable* node, CConstChar* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t:
            return nullptr;
        case AST_UChar_t: {
            TUChar value = static_cast<TUChar>(constant->value);
            return std::make_shared<CConstUChar>(std::move(value));
        }
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_copy_int_const(Ctx ctx, TacVariable* node, CConstInt* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Int_t:
            return nullptr;
        case AST_UInt_t: {
            TUInt value = static_cast<TUInt>(constant->value);
            return std::make_shared<CConstUInt>(std::move(value));
        }
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_copy_long_const(Ctx ctx, TacVariable* node, CConstLong* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_copy_dbl_const(Ctx ctx, TacVariable* node, CConstDouble* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_copy_uchar_const(Ctx ctx, TacVariable* node, CConstUChar* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = static_cast<TChar>(constant->value);
            return std::make_shared<CConstChar>(std::move(value));
        }
        case AST_UChar_t:
            return nullptr;
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_copy_uint_const(Ctx ctx, TacVariable* node, CConstUInt* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Int_t: {
            TInt value = static_cast<TInt>(constant->value);
            return std::make_shared<CConstInt>(std::move(value));
        }
        case AST_UInt_t:
            return nullptr;
        default:
            THROW_ABORT;
    }
}

static std::shared_ptr<CConst> fold_copy_ulong_const(Ctx ctx, TacVariable* node, CConstULong* constant) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
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
            THROW_ABORT;
    }
}

static std::shared_ptr<TacConstant> fold_copy_const(Ctx ctx, TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_CConstChar_t: {
            fold_constant = fold_copy_char_const(ctx, node, static_cast<CConstChar*>(constant));
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_copy_int_const(ctx, node, static_cast<CConstInt*>(constant));
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_copy_long_const(ctx, node, static_cast<CConstLong*>(constant));
            break;
        }
        case AST_CConstDouble_t: {
            fold_constant = fold_copy_dbl_const(ctx, node, static_cast<CConstDouble*>(constant));
            break;
        }
        case AST_CConstUChar_t: {
            fold_constant = fold_copy_uchar_const(ctx, node, static_cast<CConstUChar*>(constant));
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_copy_uint_const(ctx, node, static_cast<CConstUInt*>(constant));
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_copy_ulong_const(ctx, node, static_cast<CConstULong*>(constant));
            break;
        }
        default:
            THROW_ABORT;
    }
    if (fold_constant) {
        return std::make_shared<TacConstant>(std::move(fold_constant));
    }
    else {
        return nullptr;
    }
}

static void fold_copy_instr(Ctx ctx, TacCopy* node) {
    if (node->src->type() == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
        std::shared_ptr<TacValue> src = fold_copy_const(ctx, static_cast<TacVariable*>(node->dst.get()),
            static_cast<TacConstant*>(node->src.get())->constant.get());
        if (src) {
            node->src = std::move(src);
            ctx->is_fixed_point = false;
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
            THROW_ABORT;
    }
}

static void fold_jmp_eq_0_instr(Ctx ctx, TacJumpIfZero* node, size_t instr_idx) {
    if (node->condition->type() == AST_TacConstant_t) {
        if (is_const_zero(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            TIdentifier target = node->target;
            set_instr(ctx, std::make_unique<TacJump>(std::move(target)), instr_idx);
        }
        else {
            set_instr(ctx, nullptr, instr_idx);
        }
    }
}

static void fold_jmp_ne_0_instr(Ctx ctx, TacJumpIfNotZero* node, size_t instr_idx) {
    if (node->condition->type() == AST_TacConstant_t) {
        if (is_const_zero(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            set_instr(ctx, nullptr, instr_idx);
        }
        else {
            TIdentifier target = node->target;
            set_instr(ctx, std::make_unique<TacJump>(std::move(target)), instr_idx);
        }
    }
}

static void fold_instr(Ctx ctx, size_t instr_idx) {
    TacInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_TacSignExtend_t:
            fold_sign_extend_instr(ctx, static_cast<TacSignExtend*>(node), instr_idx);
            break;
        case AST_TacTruncate_t:
            fold_truncate_instr(ctx, static_cast<TacTruncate*>(node), instr_idx);
            break;
        case AST_TacZeroExtend_t:
            fold_zero_extend_instr(ctx, static_cast<TacZeroExtend*>(node), instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            fold_dbl_to_signed_instr(ctx, static_cast<TacDoubleToInt*>(node), instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            fold_dbl_to_unsigned_instr(ctx, static_cast<TacDoubleToUInt*>(node), instr_idx);
            break;
        case AST_TacIntToDouble_t:
            fold_signed_to_dbl_instr(ctx, static_cast<TacIntToDouble*>(node), instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            fold_unsigned_to_dbl_instr(ctx, static_cast<TacUIntToDouble*>(node), instr_idx);
            break;
        case AST_TacUnary_t:
            fold_unary_instr(ctx, static_cast<TacUnary*>(node), instr_idx);
            break;
        case AST_TacBinary_t:
            fold_binary_instr(ctx, static_cast<TacBinary*>(node), instr_idx);
            break;
        case AST_TacCopy_t:
            fold_copy_instr(ctx, static_cast<TacCopy*>(node));
            break;
        case AST_TacJumpIfZero_t:
            fold_jmp_eq_0_instr(ctx, static_cast<TacJumpIfZero*>(node), instr_idx);
            break;
        case AST_TacJumpIfNotZero_t:
            fold_jmp_ne_0_instr(ctx, static_cast<TacJumpIfNotZero*>(node), instr_idx);
            break;
        default:
            break;
    }
}

static void fold_constants(Ctx ctx) {
    for (size_t instr_idx = 0; instr_idx < ctx->p_instrs->size(); ++instr_idx) {
        if (GET_INSTR(instr_idx)) {
            fold_instr(ctx, instr_idx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unreachable code elimination

static void unreach_reachable_block(Ctx ctx, size_t block_id);

static void unreach_succ_reachable_blocks(Ctx ctx, size_t block_id) {
    for (size_t succ_id : GET_CFG_BLOCK(block_id).succ_ids) {
        unreach_reachable_block(ctx, succ_id);
    }
}

static void unreach_reachable_block(Ctx ctx, size_t block_id) {
    if (block_id < ctx->cfg->exit_id && !ctx->cfg->reaching_code[block_id]) {
        ctx->cfg->reaching_code[block_id] = true;
        unreach_succ_reachable_blocks(ctx, block_id);
    }
}

static void unreach_empty_block(Ctx ctx, size_t block_id) {
    for (size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_front_idx;
         instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
        if (GET_INSTR(instr_idx)) {
            set_instr(ctx, nullptr, instr_idx);
        }
    }
    GET_CFG_BLOCK(block_id).size = 0;
    cfg_rm_empty_block(ctx, block_id, false);
    GET_CFG_BLOCK(block_id).succ_ids.clear();
    GET_CFG_BLOCK(block_id).pred_ids.clear();
}

static void unreach_jump_instr(Ctx ctx, size_t block_id) {
    TacInstruction* node = GET_INSTR(GET_CFG_BLOCK(block_id).instrs_back_idx).get();
    switch (node->type()) {
        case AST_TacJump_t:
        case AST_TacJumpIfZero_t:
        case AST_TacJumpIfNotZero_t:
            cfg_rm_block_instr(ctx, GET_CFG_BLOCK(block_id).instrs_back_idx, block_id);
            break;
        default:
            break;
    }
}

static void unreach_jump_block(Ctx ctx, size_t block_id, size_t next_block_id) {
    if (GET_CFG_BLOCK(block_id).succ_ids.size() == 1 && GET_CFG_BLOCK(block_id).succ_ids[0] == next_block_id) {
        unreach_jump_instr(ctx, block_id);
    }
}

static void unreach_label_instr(Ctx ctx, size_t block_id) {
    THROW_ABORT_IF(GET_INSTR(GET_CFG_BLOCK(block_id).instrs_front_idx)->type() != AST_TacLabel_t);
    cfg_rm_block_instr(ctx, GET_CFG_BLOCK(block_id).instrs_front_idx, block_id);
}

static void unreach_label_block(Ctx ctx, size_t block_id, size_t prev_block_id) {
    if (GET_CFG_BLOCK(block_id).pred_ids.size() == 1 && GET_CFG_BLOCK(block_id).pred_ids[0] == prev_block_id) {
        unreach_label_instr(ctx, block_id);
    }
}

static void eliminate_unreachable_code(Ctx ctx) {
    if (ctx->cfg->blocks.empty()) {
        return;
    }
    if (ctx->cfg->reaching_code.size() < ctx->cfg->blocks.size()) {
        ctx->cfg->reaching_code.resize(ctx->cfg->blocks.size());
    }
    std::fill(ctx->cfg->reaching_code.begin(), ctx->cfg->reaching_code.begin() + ctx->cfg->blocks.size(), false);
    for (size_t succ_id : ctx->cfg->entry_succ_ids) {
        unreach_reachable_block(ctx, succ_id);
    }

    size_t block_id = ctx->cfg->blocks.size();
    size_t next_block_id = ctx->cfg->exit_id;
    while (block_id-- > 0) {
        if (ctx->cfg->reaching_code[block_id]) {
            next_block_id = block_id;
            break;
        }
        else {
            unreach_empty_block(ctx, block_id);
        }
    }
    while (block_id-- > 0) {
        if (ctx->cfg->reaching_code[block_id]) {
            unreach_jump_block(ctx, block_id, next_block_id);
            next_block_id = block_id;
        }
        else {
            unreach_empty_block(ctx, block_id);
        }
    }

    for (auto& label_id : ctx->cfg->identifier_id_map) {
        if (ctx->cfg->reaching_code[label_id.second]) {
            for (block_id = label_id.second; block_id-- > 0;) {
                if (ctx->cfg->reaching_code[block_id]) {
                    next_block_id = block_id;
                    goto Lelse;
                }
            }
            next_block_id = ctx->cfg->entry_id;
        Lelse:
            unreach_label_block(ctx, label_id.second, next_block_id);
        }
        else {
            label_id.second = ctx->cfg->exit_id;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy propagation

static bool is_aliased_value(Ctx ctx, TacValue* node) {
    return node->type() == AST_TacVariable_t && is_aliased_name(ctx, static_cast<TacVariable*>(node)->name);
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

static bool is_var_signed(Ctx ctx, TacVariable* node) {
    switch (ctx->frontend->symbol_table[node->name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_Int_t:
        case AST_Long_t:
            return true;
        default:
            return false;
    }
}

static bool is_value_signed(Ctx ctx, TacValue* node) {
    switch (node->type()) {
        case AST_TacConstant_t:
            return is_const_signed(static_cast<TacConstant*>(node));
        case AST_TacVariable_t:
            return is_var_signed(ctx, static_cast<TacVariable*>(node));
        default:
            THROW_ABORT;
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
                THROW_ABORT;
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
                THROW_ABORT;
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
            THROW_ABORT;
    }
}

static bool is_copy_same_signedness(Ctx ctx, TacCopy* node) {
    return is_value_signed(ctx, node->src.get()) == is_value_signed(ctx, node->dst.get());
}

static bool is_copy_null_ptr(Ctx ctx, TacCopy* node) {
    if (node->src->type() == AST_TacConstant_t && node->dst->type() == AST_TacVariable_t
        && ctx->frontend->symbol_table[static_cast<TacVariable*>(node->dst.get())->name]->type_t->type()
               == AST_Pointer_t) {
        return is_const_null_ptr(static_cast<TacConstant*>(node->src.get()));
    }
    else {
        return false;
    }
}

static void prop_transfer_dst_value(Ctx ctx, TacValue* node, size_t next_instr_idx) {
    THROW_ABORT_IF(node->type() != AST_TacVariable_t);
    size_t i = 0;
    for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instr_idx, i)) {
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type() != AST_TacCopy_t);
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                if (is_same_value(node, copy->src.get()) || is_same_value(node, copy->dst.get())) {
                    SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static void prop_transfer_call(Ctx ctx, TacFunCall* node, size_t next_instr_idx) {
    THROW_ABORT_IF(node->dst && node->dst->type() != AST_TacVariable_t);
    size_t i = 0;
    for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instr_idx, i)) {
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type() != AST_TacCopy_t);
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                if (is_aliased_value(ctx, copy->src.get()) || is_aliased_value(ctx, copy->dst.get())
                    || (node->dst
                        && (is_same_value(node->dst.get(), copy->src.get())
                            || is_same_value(node->dst.get(), copy->dst.get())))) {
                    SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static bool prop_transfer_copy(Ctx ctx, TacCopy* node, size_t next_instr_idx) {
    THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
    for (size_t i = 0; i < ctx->dfa->set_size; ++i) {
        THROW_ABORT_IF(GET_DFA_INSTR(i)->type() != AST_TacCopy_t);
        TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
        THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
        if (is_same_value(node->dst.get(), copy->dst.get())) {
            if ((is_copy_same_signedness(ctx, copy) || is_copy_null_ptr(ctx, copy))
                && is_same_value(node->src.get(), copy->src.get())) {
                SET_DFA_INSTR_SET_AT(next_instr_idx, i, true);
            }
            else {
                SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
            }
        }
        else if (GET_DFA_INSTR_SET_AT(next_instr_idx, i)) {
            if (is_same_value(node->dst.get(), copy->src.get())) {
                if (is_same_value(node->src.get(), copy->dst.get())) {
                    return false;
                }
                else {
                    SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
                }
            }
        }
    }
    return true;
}

static void prop_transfer_store(Ctx ctx, size_t next_instr_idx) {
    size_t i = 0;
    for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instr_idx, i)) {
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type() != AST_TacCopy_t);
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                if (is_aliased_value(ctx, copy->src.get()) || is_aliased_value(ctx, copy->dst.get())) {
                    SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static void prop_transfer_cp_to_offset(Ctx ctx, TacCopyToOffset* node, size_t next_instr_idx) {
    size_t i = 0;
    for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(next_instr_idx, i)) {
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type() != AST_TacCopy_t);
                TacCopy* copy = static_cast<TacCopy*>(GET_DFA_INSTR(i).get());
                THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                if (is_same_name(copy->src.get(), node->dst_name) || is_same_name(copy->dst.get(), node->dst_name)) {
                    SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
                    if (GET_DFA_INSTR_SET_MASK(next_instr_idx, j) == MASK_FALSE) {
                        i = mask_set_size;
                    }
                }
            }
        }
    }
}

static bool prop_transfer_reach_copies(Ctx ctx, size_t instr_idx, size_t next_instr_idx) {
    TacInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_TacSignExtend_t:
            prop_transfer_dst_value(ctx, static_cast<TacSignExtend*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacTruncate_t:
            prop_transfer_dst_value(ctx, static_cast<TacTruncate*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacZeroExtend_t:
            prop_transfer_dst_value(ctx, static_cast<TacZeroExtend*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            prop_transfer_dst_value(ctx, static_cast<TacDoubleToInt*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            prop_transfer_dst_value(ctx, static_cast<TacDoubleToUInt*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacIntToDouble_t:
            prop_transfer_dst_value(ctx, static_cast<TacIntToDouble*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            prop_transfer_dst_value(ctx, static_cast<TacUIntToDouble*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacFunCall_t:
            prop_transfer_call(ctx, static_cast<TacFunCall*>(node), next_instr_idx);
            break;
        case AST_TacUnary_t:
            prop_transfer_dst_value(ctx, static_cast<TacUnary*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacBinary_t:
            prop_transfer_dst_value(ctx, static_cast<TacBinary*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacCopy_t:
            return prop_transfer_copy(ctx, static_cast<TacCopy*>(node), next_instr_idx);
        case AST_TacGetAddress_t:
            prop_transfer_dst_value(ctx, static_cast<TacGetAddress*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacLoad_t:
            prop_transfer_dst_value(ctx, static_cast<TacLoad*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacStore_t:
            prop_transfer_store(ctx, next_instr_idx);
            break;
        case AST_TacAddPtr_t:
            prop_transfer_dst_value(ctx, static_cast<TacAddPtr*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacCopyToOffset_t:
            prop_transfer_cp_to_offset(ctx, static_cast<TacCopyToOffset*>(node), next_instr_idx);
            break;
        case AST_TacCopyFromOffset_t:
            prop_transfer_dst_value(ctx, static_cast<TacCopyFromOffset*>(node)->dst.get(), next_instr_idx);
            break;
        default:
            THROW_ABORT;
    }
    return true;
}

static TacCopy* get_dfa_bak_copy_instr(Ctx ctx, size_t i) {
    TacInstruction* node = get_dfa_bak_instr(ctx, i);
    THROW_ABORT_IF(node->type() != AST_TacCopy_t);
    return static_cast<TacCopy*>(node);
}

static void set_dfa_bak_copy_instr(Ctx ctx, TacCopy* node, size_t instr_idx) {
    size_t i;
    if (set_dfa_bak_instr(ctx, instr_idx, i)) {
        std::shared_ptr<TacValue> src = node->src;
        std::shared_ptr<TacValue> dst = node->dst;
        ctx->dfa_o1->bak_instrs[i] = std::make_unique<TacCopy>(std::move(src), std::move(dst));
    }
}

static void prop_ret_instr(Ctx ctx, TacReturn* node, size_t incoming_idx, bool exit_block) {
    if (node->val && node->val->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if ((exit_block && GET_DFA_BLOCK_SET_MASK(incoming_idx, j) == MASK_FALSE)
                || (!exit_block && GET_DFA_INSTR_SET_MASK(incoming_idx, j) == MASK_FALSE)) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (((exit_block && GET_DFA_BLOCK_SET_AT(incoming_idx, i))
                        || (!exit_block && GET_DFA_INSTR_SET_AT(incoming_idx, i)))) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->val.get(), copy->dst.get())) {
                        node->val = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_sign_extend_instr(Ctx ctx, TacSignExtend* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_truncate_instr(Ctx ctx, TacTruncate* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_zero_extend_instr(Ctx ctx, TacZeroExtend* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_dbl_to_int_instr(Ctx ctx, TacDoubleToInt* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_dbl_to_uint_instr(Ctx ctx, TacDoubleToUInt* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_int_to_dbl_instr(Ctx ctx, TacIntToDouble* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_uint_to_dbl_instr(Ctx ctx, TacUIntToDouble* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_call_instr(Ctx ctx, TacFunCall* node, size_t instr_idx) {
    for (size_t i = 0; i < node->args.size(); ++i) {
        if (node->args[i]->type() == AST_TacVariable_t) {
            size_t j = 0;
            for (size_t k = 0; k < ctx->dfa->mask_size; ++k) {
                if (GET_DFA_INSTR_SET_MASK(instr_idx, k) == MASK_FALSE) {
                    j += 64;
                    continue;
                }
                size_t mask_set_size = j + 64;
                if (mask_set_size > ctx->dfa->set_size) {
                    mask_set_size = ctx->dfa->set_size;
                }
                for (; j < mask_set_size; ++j) {
                    if (GET_DFA_INSTR_SET_AT(instr_idx, j)) {
                        TacCopy* copy = get_dfa_bak_copy_instr(ctx, j);
                        THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                        if (is_same_value(node->args[i].get(), copy->dst.get())) {
                            node->args[i] = copy->src;
                            ctx->is_fixed_point = false;
                            goto Lbreak;
                        }
                    }
                }
            }
        Lbreak:;
        }
    }
}

static void prop_unary_instr(Ctx ctx, TacUnary* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_binary_instr(Ctx ctx, TacBinary* node, size_t instr_idx) {
    bool is_src1 = node->src1->type() == AST_TacVariable_t;
    bool is_src2 = node->src2->type() == AST_TacVariable_t;
    if (is_src1 || is_src2) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_src1 && is_same_value(node->src1.get(), copy->dst.get())) {
                        node->src1 = copy->src;
                        ctx->is_fixed_point = false;
                        is_src1 = false;
                        if (!is_src2) {
                            return;
                        }
                    }
                    if (is_src2 && is_same_value(node->src2.get(), copy->dst.get())) {
                        node->src2 = copy->src;
                        ctx->is_fixed_point = false;
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

static void prop_copy_instr(Ctx ctx, TacCopy* node, size_t instr_idx, size_t block_id) {
    THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
    size_t i = 0;
    for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                if (ctx->dfa_o1->data_idx_map[i] == instr_idx
                    || (is_same_value(node->src.get(), copy->dst.get())
                        && is_same_value(node->dst.get(), copy->src.get()))) {
                    set_dfa_bak_copy_instr(ctx, node, instr_idx);
                    cfg_rm_block_instr(ctx, instr_idx, block_id);
                    return;
                }
                else if (is_same_value(node->src.get(), copy->dst.get())) {
                    set_dfa_bak_copy_instr(ctx, node, instr_idx);
                    node->src = copy->src;
                    ctx->is_fixed_point = false;
                    return;
                }
            }
        }
    }
}

static void prop_load_instr(Ctx ctx, TacLoad* node, size_t instr_idx) {
    if (node->src_ptr->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src_ptr.get(), copy->dst.get())) {
                        node->src_ptr = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_store_instr(Ctx ctx, TacStore* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_add_ptr_instr(Ctx ctx, TacAddPtr* node, size_t instr_idx) {
    bool is_src_ptr = node->src_ptr->type() == AST_TacVariable_t;
    bool is_idx = node->idx->type() == AST_TacVariable_t;
    if (is_src_ptr || is_idx) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_src_ptr && is_same_value(node->src_ptr.get(), copy->dst.get())) {
                        node->src_ptr = copy->src;
                        ctx->is_fixed_point = false;
                        is_src_ptr = false;
                        if (!is_idx) {
                            return;
                        }
                    }
                    if (is_idx && is_same_value(node->idx.get(), copy->dst.get())) {
                        node->idx = copy->src;
                        ctx->is_fixed_point = false;
                        is_idx = false;
                        if (!is_src_ptr) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

static void prop_cp_to_offset_instr(Ctx ctx, TacCopyToOffset* node, size_t instr_idx) {
    if (node->src->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->src.get(), copy->dst.get())) {
                        node->src = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_cp_from_offset_instr(Ctx ctx, TacCopyFromOffset* node, size_t instr_idx) {
    size_t i = 0;
    for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instr_idx, i)) {
                TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                if (is_same_name(copy->dst.get(), node->src_name)) {
                    THROW_ABORT_IF(copy->src->type() != AST_TacVariable_t);
                    node->src_name = static_cast<TacVariable*>(copy->src.get())->name;
                    ctx->is_fixed_point = false;
                    return;
                }
            }
        }
    }
}

static void prop_jmp_eq_0_instr(Ctx ctx, TacJumpIfZero* node, size_t incoming_idx, size_t exit_block) {
    if (node->condition->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if ((exit_block && GET_DFA_BLOCK_SET_MASK(incoming_idx, j) == MASK_FALSE)
                || (!exit_block && GET_DFA_INSTR_SET_MASK(incoming_idx, j) == MASK_FALSE)) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (((exit_block && GET_DFA_BLOCK_SET_AT(incoming_idx, i))
                        || (!exit_block && GET_DFA_INSTR_SET_AT(incoming_idx, i)))) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->condition.get(), copy->dst.get())) {
                        node->condition = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_jmp_ne_0_instr(Ctx ctx, TacJumpIfNotZero* node, size_t incoming_idx, size_t exit_block) {
    if (node->condition->type() == AST_TacVariable_t) {
        size_t i = 0;
        for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
            if ((exit_block && GET_DFA_BLOCK_SET_MASK(incoming_idx, j) == MASK_FALSE)
                || (!exit_block && GET_DFA_INSTR_SET_MASK(incoming_idx, j) == MASK_FALSE)) {
                i += 64;
                continue;
            }
            size_t mask_set_size = i + 64;
            if (mask_set_size > ctx->dfa->set_size) {
                mask_set_size = ctx->dfa->set_size;
            }
            for (; i < mask_set_size; ++i) {
                if (((exit_block && GET_DFA_BLOCK_SET_AT(incoming_idx, i))
                        || (!exit_block && GET_DFA_INSTR_SET_AT(incoming_idx, i)))) {
                    TacCopy* copy = get_dfa_bak_copy_instr(ctx, i);
                    THROW_ABORT_IF(copy->dst->type() != AST_TacVariable_t);
                    if (is_same_value(node->condition.get(), copy->dst.get())) {
                        node->condition = copy->src;
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_instr(Ctx ctx, size_t instr_idx, size_t copy_instr_idx, size_t block_id) {
    TacInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_TacReturn_t:
            prop_ret_instr(ctx, static_cast<TacReturn*>(node), copy_instr_idx, block_id > 0);
            break;
        case AST_TacSignExtend_t:
            prop_sign_extend_instr(ctx, static_cast<TacSignExtend*>(node), copy_instr_idx);
            break;
        case AST_TacTruncate_t:
            prop_truncate_instr(ctx, static_cast<TacTruncate*>(node), copy_instr_idx);
            break;
        case AST_TacZeroExtend_t:
            prop_zero_extend_instr(ctx, static_cast<TacZeroExtend*>(node), copy_instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            prop_dbl_to_int_instr(ctx, static_cast<TacDoubleToInt*>(node), copy_instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            prop_dbl_to_uint_instr(ctx, static_cast<TacDoubleToUInt*>(node), copy_instr_idx);
            break;
        case AST_TacIntToDouble_t:
            prop_int_to_dbl_instr(ctx, static_cast<TacIntToDouble*>(node), copy_instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            prop_uint_to_dbl_instr(ctx, static_cast<TacUIntToDouble*>(node), copy_instr_idx);
            break;
        case AST_TacFunCall_t:
            prop_call_instr(ctx, static_cast<TacFunCall*>(node), copy_instr_idx);
            break;
        case AST_TacUnary_t:
            prop_unary_instr(ctx, static_cast<TacUnary*>(node), copy_instr_idx);
            break;
        case AST_TacBinary_t:
            prop_binary_instr(ctx, static_cast<TacBinary*>(node), copy_instr_idx);
            break;
        case AST_TacCopy_t:
            prop_copy_instr(ctx, static_cast<TacCopy*>(node), copy_instr_idx, block_id);
            break;
        case AST_TacLoad_t:
            prop_load_instr(ctx, static_cast<TacLoad*>(node), copy_instr_idx);
            break;
        case AST_TacStore_t:
            prop_store_instr(ctx, static_cast<TacStore*>(node), copy_instr_idx);
            break;
        case AST_TacAddPtr_t:
            prop_add_ptr_instr(ctx, static_cast<TacAddPtr*>(node), copy_instr_idx);
            break;
        case AST_TacCopyToOffset_t:
            prop_cp_to_offset_instr(ctx, static_cast<TacCopyToOffset*>(node), copy_instr_idx);
            break;
        case AST_TacCopyFromOffset_t:
            prop_cp_from_offset_instr(ctx, static_cast<TacCopyFromOffset*>(node), copy_instr_idx);
            break;
        case AST_TacJumpIfZero_t:
            prop_jmp_eq_0_instr(ctx, static_cast<TacJumpIfZero*>(node), copy_instr_idx, block_id > 0);
            break;
        case AST_TacJumpIfNotZero_t:
            prop_jmp_ne_0_instr(ctx, static_cast<TacJumpIfNotZero*>(node), copy_instr_idx, block_id > 0);
            break;
        default:
            THROW_ABORT;
    }
}

static void propagate_copies(Ctx ctx) {
    if (!init_data_flow_analysis(ctx, false, true)) {
        return;
    }
    dfa_forward_iter_alg(ctx);

    for (size_t block_id = 0; block_id < ctx->cfg->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            size_t incoming_idx = block_id;
            size_t exit_block = 1;
            for (size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_back_idx + 1;
                 instr_idx-- > GET_CFG_BLOCK(block_id).instrs_front_idx;) {
                if (GET_INSTR(instr_idx)) {
                    switch (GET_INSTR(instr_idx)->type()) {
                        case AST_TacReturn_t:
                        case AST_TacJumpIfZero_t:
                        case AST_TacJumpIfNotZero_t:
                            prop_instr(ctx, instr_idx, incoming_idx, exit_block);
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
                            prop_instr(ctx, instr_idx, instr_idx, block_id);
                            incoming_idx = instr_idx;
                            exit_block = 0;
                            break;
                        }
                        case AST_TacGetAddress_t: {
                            incoming_idx = instr_idx;
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

static void elim_transfer_addressed(Ctx ctx, size_t next_instr_idx) {
    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(next_instr_idx, i) |= GET_DFA_INSTR_SET_MASK(ctx->dfa_o1->addressed_idx, i);
    }
}

static void elim_transfer_aliased(Ctx ctx, size_t next_instr_idx) {
    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(next_instr_idx, i) |= GET_DFA_INSTR_SET_MASK(ctx->dfa->static_idx, i);
        GET_DFA_INSTR_SET_MASK(next_instr_idx, i) |= GET_DFA_INSTR_SET_MASK(ctx->dfa_o1->addressed_idx, i);
    }
}

static void elim_transfer_src_name(Ctx ctx, TIdentifier name, size_t next_instr_idx) {
    size_t i = ctx->cfg->identifier_id_map[name];
    SET_DFA_INSTR_SET_AT(next_instr_idx, i, true);
}

static void elim_transfer_src_value(Ctx ctx, TacValue* node, size_t next_instr_idx) {
    if (node->type() == AST_TacVariable_t) {
        elim_transfer_src_name(ctx, static_cast<TacVariable*>(node)->name, next_instr_idx);
    }
}

static void elim_transfer_dst_value(Ctx ctx, TacValue* node, size_t next_instr_idx) {
    THROW_ABORT_IF(node->type() != AST_TacVariable_t);
    size_t i = ctx->cfg->identifier_id_map[static_cast<TacVariable*>(node)->name];
    SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
}

static void elim_transfer_live_values(Ctx ctx, size_t instr_idx, size_t next_instr_idx) {
    TacInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_TacReturn_t: {
            TacReturn* p_node = static_cast<TacReturn*>(node);
            if (p_node->val) {
                elim_transfer_src_value(ctx, p_node->val.get(), next_instr_idx);
            }
            break;
        }
        case AST_TacSignExtend_t: {
            TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacTruncate_t: {
            TacTruncate* p_node = static_cast<TacTruncate*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacZeroExtend_t: {
            TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacDoubleToInt_t: {
            TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacDoubleToUInt_t: {
            TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacIntToDouble_t: {
            TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacUIntToDouble_t: {
            TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacFunCall_t: {
            TacFunCall* p_node = static_cast<TacFunCall*>(node);
            if (p_node->dst) {
                elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            }
            for (const auto& arg : p_node->args) {
                elim_transfer_src_value(ctx, arg.get(), next_instr_idx);
            }
            elim_transfer_aliased(ctx, next_instr_idx);
            break;
        }
        case AST_TacUnary_t: {
            TacUnary* p_node = static_cast<TacUnary*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacBinary_t: {
            TacBinary* p_node = static_cast<TacBinary*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src1.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src2.get(), next_instr_idx);
            break;
        }
        case AST_TacCopy_t: {
            TacCopy* p_node = static_cast<TacCopy*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_TacGetAddress_t:
            elim_transfer_dst_value(ctx, static_cast<TacGetAddress*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_TacLoad_t: {
            TacLoad* p_node = static_cast<TacLoad*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src_ptr.get(), next_instr_idx);
            elim_transfer_addressed(ctx, next_instr_idx);
            break;
        }
        case AST_TacStore_t: {
            TacStore* p_node = static_cast<TacStore*>(node);
            elim_transfer_src_value(ctx, p_node->src.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->dst_ptr.get(), next_instr_idx);
            break;
        }
        case AST_TacAddPtr_t: {
            TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src_ptr.get(), next_instr_idx);
            elim_transfer_src_value(ctx, p_node->idx.get(), next_instr_idx);
            break;
        }
        case AST_TacCopyToOffset_t:
            elim_transfer_src_value(ctx, static_cast<TacCopyToOffset*>(node)->src.get(), next_instr_idx);
            break;
        case AST_TacCopyFromOffset_t: {
            TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
            elim_transfer_dst_value(ctx, p_node->dst.get(), next_instr_idx);
            elim_transfer_src_name(ctx, p_node->src_name, next_instr_idx);
            break;
        }
        case AST_TacJumpIfZero_t:
            elim_transfer_src_value(ctx, static_cast<TacJumpIfZero*>(node)->condition.get(), next_instr_idx);
            break;
        case AST_TacJumpIfNotZero_t:
            elim_transfer_src_value(ctx, static_cast<TacJumpIfNotZero*>(node)->condition.get(), next_instr_idx);
            break;
        default:
            THROW_ABORT;
    }
}

static void elim_dst_name_instr(Ctx ctx, TIdentifier name, size_t instr_idx) {
    size_t i = ctx->cfg->identifier_id_map[name];
    if (!GET_DFA_INSTR_SET_AT(instr_idx, i)) {
        set_instr(ctx, nullptr, instr_idx);
    }
}

static void elim_dst_value_instr(Ctx ctx, TacValue* node, size_t instr_idx) {
    THROW_ABORT_IF(node->type() != AST_TacVariable_t);
    elim_dst_name_instr(ctx, static_cast<TacVariable*>(node)->name, instr_idx);
}

static void elim_instr(Ctx ctx, size_t instr_idx) {
    TacInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_TacSignExtend_t:
            elim_dst_value_instr(ctx, static_cast<TacSignExtend*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacTruncate_t:
            elim_dst_value_instr(ctx, static_cast<TacTruncate*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacZeroExtend_t:
            elim_dst_value_instr(ctx, static_cast<TacZeroExtend*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            elim_dst_value_instr(ctx, static_cast<TacDoubleToInt*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            elim_dst_value_instr(ctx, static_cast<TacDoubleToUInt*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacIntToDouble_t:
            elim_dst_value_instr(ctx, static_cast<TacIntToDouble*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            elim_dst_value_instr(ctx, static_cast<TacUIntToDouble*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacUnary_t:
            elim_dst_value_instr(ctx, static_cast<TacUnary*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacBinary_t:
            elim_dst_value_instr(ctx, static_cast<TacBinary*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacCopy_t:
            elim_dst_value_instr(ctx, static_cast<TacCopy*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacGetAddress_t:
            elim_dst_value_instr(ctx, static_cast<TacGetAddress*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacLoad_t:
            elim_dst_value_instr(ctx, static_cast<TacLoad*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacAddPtr_t:
            elim_dst_value_instr(ctx, static_cast<TacAddPtr*>(node)->dst.get(), instr_idx);
            break;
        case AST_TacCopyToOffset_t:
            elim_dst_name_instr(ctx, static_cast<TacCopyToOffset*>(node)->dst_name, instr_idx);
            break;
        case AST_TacCopyFromOffset_t:
            elim_dst_value_instr(ctx, static_cast<TacCopyFromOffset*>(node)->dst.get(), instr_idx);
            break;
        default:
            break;
    }
}

static void eliminate_dead_stores(Ctx ctx, bool is_addressed_set) {
    if (!init_data_flow_analysis(ctx, true, is_addressed_set)) {
        return;
    }
    dfa_iter_alg(ctx);

    for (size_t block_id = 0; block_id < ctx->cfg->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_front_idx;
                 instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
                if (GET_INSTR(instr_idx)) {
                    elim_instr(ctx, instr_idx);
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

static void optim_fun_toplvl(Ctx ctx, TacFunction* node) {
    ctx->p_instrs = &node->body;
    do {
        ctx->is_fixed_point = true;
        if (ctx->enabled_optims[CONSTANT_FOLDING]) {
            fold_constants(ctx);
        }
        if (ctx->enabled_optims[CONTROL_FLOW_GRAPH]) {
            init_control_flow_graph(ctx);
            if (ctx->enabled_optims[UNREACHABLE_CODE_ELIMINATION]) {
                eliminate_unreachable_code(ctx);
            }
            if (ctx->enabled_optims[COPY_PROPAGATION]) {
                propagate_copies(ctx);
            }
            if (ctx->enabled_optims[DEAD_STORE_ELIMINATION]) {
                eliminate_dead_stores(ctx, !ctx->enabled_optims[COPY_PROPAGATION]);
            }
        }
    }
    while (!ctx->is_fixed_point);
    ctx->p_instrs = nullptr;
}

static void optim_toplvl(Ctx ctx, TacTopLevel* node) {
    if (node->type() == AST_TacFunction_t) {
        optim_fun_toplvl(ctx, static_cast<TacFunction*>(node));
    }
    else {
        THROW_ABORT;
    }
}

static void optim_program(Ctx ctx, TacProgram* node) {
    for (const auto& top_level : node->fun_toplvls) {
        optim_toplvl(ctx, top_level.get());
    }
    ctx->frontend->addressed_set.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void optimize_three_address_code(TacProgram* node, uint8_t optim_1_mask) {
    OptimTacContext ctx;
    {
        ctx.frontend = frontend.get();
        ctx.is_fixed_point = true;

        ctx.enabled_optims[CONSTANT_FOLDING] = (optim_1_mask & (static_cast<uint8_t>(1u) << 0)) > 0;
        ctx.enabled_optims[COPY_PROPAGATION] = (optim_1_mask & (static_cast<uint8_t>(1u) << 1)) > 0;
        ctx.enabled_optims[UNREACHABLE_CODE_ELIMINATION] = (optim_1_mask & (static_cast<uint8_t>(1u) << 2)) > 0;
        ctx.enabled_optims[DEAD_STORE_ELIMINATION] = (optim_1_mask & (static_cast<uint8_t>(1u) << 3)) > 0;
        ctx.enabled_optims[CONTROL_FLOW_GRAPH] = (optim_1_mask & ~(static_cast<uint8_t>(1u) << 0)) > 0;
    }
    if (ctx.enabled_optims[CONTROL_FLOW_GRAPH]) {
        ctx.cfg = std::make_unique<ControlFlowGraph>();
        if (ctx.enabled_optims[COPY_PROPAGATION] || ctx.enabled_optims[DEAD_STORE_ELIMINATION]) {
            ctx.dfa = std::make_unique<DataFlowAnalysis>();
            ctx.dfa_o1 = std::make_unique<DataFlowAnalysisO1>();
        }
    }
    optim_program(&ctx, node);
}
