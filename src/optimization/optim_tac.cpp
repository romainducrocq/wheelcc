#include <inttypes.h>
#include <memory>
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding

static void set_instruction(std::unique_ptr<TacInstruction>&& instruction) {
    if (instruction) {
        (*context->p_instructions)[context->instruction_index] = std::move(instruction);
    }
    else {
        (*context->p_instructions)[context->instruction_index].reset();
    }
    context->is_fixed_point = false;
}

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

static void fold_constants_sign_extend_instructions(TacSignExtend* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_sign_extend_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_truncate_instructions(TacTruncate* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_truncate_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_zero_extend_instructions(TacZeroExtend* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_zero_extend_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_double_to_signed_instructions(TacDoubleToInt* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_double_to_signed_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_double_to_unsigned_instructions(TacDoubleToUInt* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_double_to_unsigned_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_signed_to_double_instructions(TacIntToDouble* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_signed_to_double_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_unsigned_to_double_instructions(TacUIntToDouble* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        if (node->dst->type() != AST_T::TacVariable_t) {
            RAISE_INTERNAL_ERROR;
        }
        std::shared_ptr<TacValue> src = fold_constants_unsigned_to_double_constant_value(
            static_cast<TacVariable*>(node->dst.get()), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_unary_instructions(TacUnary* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        std::shared_ptr<TacValue> src = fold_constants_unary_constant_value(
            node->unary_op.get(), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_binary_instructions(TacBinary* node) {
    if (node->src1->type() == AST_T::TacConstant_t && node->src2->type() == AST_T::TacConstant_t) {
        std::shared_ptr<TacValue> src = fold_constants_binary_constant_value(node->binary_op.get(),
            static_cast<TacConstant*>(node->src1.get())->constant.get(),
            static_cast<TacConstant*>(node->src2.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
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

static void fold_constants_jump_if_zero_instructions(TacJumpIfZero* node) {
    if (node->condition->type() == AST_T::TacConstant_t) {
        if (fold_constants_is_zero_constant_value(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            TIdentifier target = node->target;
            set_instruction(std::make_unique<TacJump>(std::move(target)));
        }
        else {
            set_instruction(nullptr);
        }
    }
}

static void fold_constants_jump_if_not_zero_instructions(TacJumpIfNotZero* node) {
    if (node->condition->type() == AST_T::TacConstant_t) {
        if (fold_constants_is_zero_constant_value(static_cast<TacConstant*>(node->condition.get())->constant.get())) {
            set_instruction(nullptr);
        }
        else {
            TIdentifier target = node->target;
            set_instruction(std::make_unique<TacJump>(std::move(target)));
        }
    }
}

static void fold_constants_instructions(TacInstruction* node) {
    switch (node->type()) {
        case AST_T::TacSignExtend_t:
            fold_constants_sign_extend_instructions(static_cast<TacSignExtend*>(node));
            break;
        case AST_T::TacTruncate_t:
            fold_constants_truncate_instructions(static_cast<TacTruncate*>(node));
            break;
        case AST_T::TacZeroExtend_t:
            fold_constants_zero_extend_instructions(static_cast<TacZeroExtend*>(node));
            break;
        case AST_T::TacDoubleToInt_t:
            fold_constants_double_to_signed_instructions(static_cast<TacDoubleToInt*>(node));
            break;
        case AST_T::TacDoubleToUInt_t:
            fold_constants_double_to_unsigned_instructions(static_cast<TacDoubleToUInt*>(node));
            break;
        case AST_T::TacIntToDouble_t:
            fold_constants_signed_to_double_instructions(static_cast<TacIntToDouble*>(node));
            break;
        case AST_T::TacUIntToDouble_t:
            fold_constants_unsigned_to_double_instructions(static_cast<TacUIntToDouble*>(node));
            break;
        case AST_T::TacUnary_t:
            fold_constants_unary_instructions(static_cast<TacUnary*>(node));
            break;
        case AST_T::TacBinary_t:
            fold_constants_binary_instructions(static_cast<TacBinary*>(node));
            break;
        case AST_T::TacJumpIfZero_t:
            fold_constants_jump_if_zero_instructions(static_cast<TacJumpIfZero*>(node));
            break;
        case AST_T::TacJumpIfNotZero_t:
            fold_constants_jump_if_not_zero_instructions(static_cast<TacJumpIfNotZero*>(node));
            break;
        default:
            break;
    }
}

static void fold_constants_list_instructions() {
    for (context->instruction_index = 0; context->instruction_index < context->p_instructions->size();
         ++context->instruction_index) {
        if ((*context->p_instructions)[context->instruction_index]) {
            fold_constants_instructions((*context->p_instructions)[context->instruction_index].get());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy propagation

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unreachable code elimination

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Dead store elimination

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void control_flow_graph_add_edge(size_t predecessor_id, size_t successor_id) {
    if (predecessor_id == context->control_flow_graph->entry_id) {
        // TODO
        // context->control_flow_graph->entry_sucessor_ids.push_back(successor_id);
        // context->control_flow_graph->blocks[successor_id].predecessor_ids.push_back(predecessor_id);
    }
    else if (successor_id == context->control_flow_graph->exit_id) {
        // TODO
    }
    else {
        // TODO
    }
}

static void control_flow_graph_initialize() {
    context->control_flow_graph->blocks.clear();
    context->control_flow_graph->label_id_map.clear();

    size_t instructions_back_index = context->p_instructions->size();
    for (context->instruction_index = 0; context->instruction_index < context->p_instructions->size();
         ++context->instruction_index) {
        if ((*context->p_instructions)[context->instruction_index]) {
            if (instructions_back_index == context->p_instructions->size()) {
                ControlFlowBlock block {context->instruction_index, 0, {}, {}};
                context->control_flow_graph->blocks.emplace_back(std::move(block));
            }
            switch ((*context->p_instructions)[context->instruction_index]->type()) {
                case AST_T::TacLabel_t: {
                    // TODO rm
                    // if current_block is not empty:
                    //     finished_blocks.append(current_block)
                    // current_block = []
                    // current_block = [instruction]
                    if (instructions_back_index != context->p_instructions->size()) {
                        context->control_flow_graph->blocks.back().instructions_size =
                            instructions_back_index
                            - context->control_flow_graph->blocks.back().instructions_front_index + 1;
                        ControlFlowBlock block {context->instruction_index, 0, {}, {}};
                        context->control_flow_graph->blocks.emplace_back(std::move(block));
                    }
                    context->control_flow_graph->label_id_map
                        [static_cast<TacLabel*>((*context->p_instructions)[context->instruction_index].get())->name] =
                        context->control_flow_graph->blocks.size() - 1;
                    instructions_back_index = context->instruction_index;
                    break;
                }
                case AST_T::TacReturn_t:
                case AST_T::TacJump_t:
                case AST_T::TacJumpIfZero_t:
                case AST_T::TacJumpIfNotZero_t: {
                    // TODO rm
                    // current_block.append(instruction)
                    // finished_blocks.append(current_block)
                    // current_block = []
                    context->control_flow_graph->blocks.back().instructions_size =
                        context->instruction_index - context->control_flow_graph->blocks.back().instructions_front_index
                        + 1;
                    instructions_back_index = context->p_instructions->size();
                    break;
                }
                default: {
                    instructions_back_index = context->instruction_index;
                    break;
                }
            }
        }
    }
    if (instructions_back_index != context->p_instructions->size()) {
        context->control_flow_graph->blocks.back().instructions_size =
            instructions_back_index - context->control_flow_graph->blocks.back().instructions_front_index + 1;
    }

    context->control_flow_graph->entry_id = context->control_flow_graph->blocks.size();
    context->control_flow_graph->exit_id = context->control_flow_graph->blocks.size() + 1;
    context->control_flow_graph->null_id = context->control_flow_graph->blocks.size() + 2;
    context->control_flow_graph->entry_sucessor_ids.clear();
    context->control_flow_graph->exit_predecessor_ids.clear();
    for (size_t i = 0; i < context->control_flow_graph->blocks.size(); ++i) {
        // TODO
    }
}

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
    context->p_instructions = nullptr;
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
    if (context->enabled_optimizations[CONTROL_FLOW_GRAPH]) {
        context->control_flow_graph = std::make_unique<ControlFlowGraph>();
    }
    optimize_program(node);
    context.reset();
}
