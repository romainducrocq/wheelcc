#include <math.h>
#include <memory>
#include <stdint.h>

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

/*
-- sign extend
int -> long
    -> unsigned long

-- zero extend
unsigned int -> long
             -> unsigned long

-- truncate
long -> int
     -> unsigned int
unsigned long -> int
              -> unsigned int


unsigned int -> long
             -> unsigned long
             -> double


int -> long
    -> unsigned long
    -> double


long -> int
     -> unsigned int
     -> double


unsigned long -> int
              -> unsigned int
              -> double


double -> int
       -> long
       -> unsigned int
       -> unsigned long
*/

/*
long -> int
double -> int
unsigned long -> int
*/

// static TInt get_int_constant_value(CConstant* node) {
//     switch (node->constant->type()) {
//         case AST_T::CConstLong_t:
//             return static_cast<TInt>(static_cast<CConstLong*>(node->constant.get())->value);
//         case AST_T::CConstDouble_t:
//             return static_cast<TInt>(static_cast<CConstDouble*>(node->constant.get())->value);
//         case AST_T::CConstULong_t:
//             return static_cast<TInt>(static_cast<CConstULong*>(node->constant.get())->value);
//         default:
//             RAISE_INTERNAL_ERROR;
//     }
// }

/*
int -> long
double -> long
unsigned int -> long
*/

// static TLong get_long_constant_value(CConstant* node) {
//     switch (node->constant->type()) {
//         case AST_T::CConstInt_t:
//             return static_cast<TLong>(static_cast<CConstInt*>(node->constant.get())->value);
//         case AST_T::CConstDouble_t:
//             return static_cast<TLong>(static_cast<CConstDouble*>(node->constant.get())->value);
//         case AST_T::CConstUInt_t:
//             return static_cast<TLong>(static_cast<CConstUInt*>(node->constant.get())->value);
//         default:
//             RAISE_INTERNAL_ERROR;
//     }
// }


/*
int -> double
long -> double
unsigned int -> double
unsigned long -> double
*/

// static TDouble get_double_constant_value(CConstant* node) {
//     switch (node->constant->type()) {
//         case AST_T::CConstInt_t:
//             return static_cast<TDouble>(static_cast<CConstInt*>(node->constant.get())->value);
//         case AST_T::CConstLong_t:
//             return static_cast<TDouble>(static_cast<CConstLong*>(node->constant.get())->value);
//         case AST_T::CConstUInt_t:
//             return static_cast<TDouble>(static_cast<CConstUInt*>(node->constant.get())->value);
//         case AST_T::CConstULong_t:
//             return static_cast<TDouble>(static_cast<CConstULong*>(node->constant.get())->value);
//         default:
//             RAISE_INTERNAL_ERROR;
//     }
// }

/*
long -> unsigned int
double -> unsigned int
unsigned long -> unsigned int
*/

// static TUInt get_uint_constant_value(CConstant* node) {
//     switch (node->constant->type()) {
//         case AST_T::CConstLong_t:
//             return static_cast<TUInt>(static_cast<CConstLong*>(node->constant.get())->value);
//         case AST_T::CConstDouble_t:
//             return static_cast<TUInt>(static_cast<CConstDouble*>(node->constant.get())->value);
//         case AST_T::CConstULong_t:
//             return static_cast<TUInt>(static_cast<CConstULong*>(node->constant.get())->value);
//         default:
//             RAISE_INTERNAL_ERROR;
//     }
// }

/*
int -> unsigned long
double -> unsigned long
unsigned int -> unsigned long
*/

// static TULong get_ulong_constant_value(CConstant* node) {
//     switch (node->constant->type()) {
//         case AST_T::CConstInt_t:
//             return static_cast<TULong>(static_cast<CConstInt*>(node->constant.get())->value);
//         case AST_T::CConstDouble_t:
//             return static_cast<TULong>(static_cast<CConstDouble*>(node->constant.get())->value);
//         case AST_T::CConstUInt_t:
//             return static_cast<TULong>(static_cast<CConstUInt*>(node->constant.get())->value);
//         default:
//             RAISE_INTERNAL_ERROR;
//     }
// }

/*
-- sign extend
int -> long
    -> unsigned long
*/
static std::shared_ptr<TacConstant> fold_constants_sign_extend_constant_value(TacVariable* node, CConst* constant) {
    if (constant->type() != AST_T::CConstInt_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Long_t: {
            TLong value = static_cast<TLong>(static_cast<CConstInt*>(constant)->value);
            fold_constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        case AST_T::ULong_t: {
            TULong value = static_cast<TULong>(static_cast<CConstInt*>(constant)->value);
            fold_constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

/*
-- truncate
long -> int
     -> unsigned int
unsigned long -> int
              -> unsigned int
*/
static std::shared_ptr<TacConstant> fold_constants_truncate_constant_value(TacVariable* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstLong_t: {
            switch (frontend->symbol_table[node->name]->type_t->type()) {
                case AST_T::Int_t: {
                    TInt value = static_cast<TInt>(static_cast<CConstLong*>(constant)->value);
                    fold_constant = std::make_shared<CConstInt>(std::move(value));
                    break;
                }
                case AST_T::UInt_t: {
                    TUInt value = static_cast<TUInt>(static_cast<CConstLong*>(constant)->value);
                    fold_constant = std::make_shared<CConstUInt>(std::move(value));
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            break;
        }
        case AST_T::CConstULong_t: {
            switch (frontend->symbol_table[node->name]->type_t->type()) {
                case AST_T::Int_t: {
                    TInt value = static_cast<TInt>(static_cast<CConstULong*>(constant)->value);
                    fold_constant = std::make_shared<CConstInt>(std::move(value));
                    break;
                }
                case AST_T::UInt_t: {
                    TUInt value = static_cast<TUInt>(static_cast<CConstULong*>(constant)->value);
                    fold_constant = std::make_shared<CConstUInt>(std::move(value));
                    break;
                }
                default:
                    RAISE_INTERNAL_ERROR;
            }
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

/*
-- zero extend
unsigned int -> long
             -> unsigned long
*/
static std::shared_ptr<TacConstant> fold_constants_zero_extend_constant_value(TacVariable* node, CConst* constant) {
    if (constant->type() != AST_T::CConstUInt_t) {
        RAISE_INTERNAL_ERROR;
    }
    std::shared_ptr<CConst> fold_constant;
    switch (frontend->symbol_table[node->name]->type_t->type()) {
        case AST_T::Long_t: {
            TLong value = static_cast<TLong>(static_cast<CConstUInt*>(constant)->value);
            fold_constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        case AST_T::ULong_t: {
            TULong value = static_cast<TULong>(static_cast<CConstUInt*>(constant)->value);
            fold_constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static TInt fold_constants_unary_int_value(TacUnaryOp* node, TInt value) {
    switch (node->type()) {
        case AST_T::TacNot_t:
            return !value;
        case AST_T::TacComplement_t:
            return ~value;
        case AST_T::TacNegate_t:
            return -value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TLong fold_constants_unary_long_value(TacUnaryOp* node, TLong value) {
    switch (node->type()) {
        case AST_T::TacNot_t:
            return !value;
        case AST_T::TacComplement_t:
            return ~value;
        case AST_T::TacNegate_t:
            return -value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TDouble fold_constants_unary_double_value(TacUnaryOp* node, TDouble value) {
    switch (node->type()) {
        case AST_T::TacNot_t:
            return !value;
        case AST_T::TacNegate_t:
            return -value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TUInt fold_constants_unary_uint_value(TacUnaryOp* node, TUInt value) {
    switch (node->type()) {
        case AST_T::TacNot_t:
            return !value;
        case AST_T::TacComplement_t:
            return ~value;
        case AST_T::TacNegate_t:
            return -value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TULong fold_constants_unary_ulong_value(TacUnaryOp* node, TULong value) {
    switch (node->type()) {
        case AST_T::TacNot_t:
            return !value;
        case AST_T::TacComplement_t:
            return ~value;
        case AST_T::TacNegate_t:
            return -value;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static std::shared_ptr<TacConstant> fold_constants_unary_constant_value(TacUnaryOp* node, CConst* constant) {
    std::shared_ptr<CConst> fold_constant;
    switch (constant->type()) {
        case AST_T::CConstInt_t: {
            TInt value = fold_constants_unary_int_value(node, static_cast<CConstInt*>(constant)->value);
            fold_constant = std::make_shared<CConstInt>(std::move(value));
            break;
        }
        case AST_T::CConstLong_t: {
            TLong value = fold_constants_unary_long_value(node, static_cast<CConstLong*>(constant)->value);
            fold_constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        case AST_T::CConstDouble_t: {
            TDouble value = fold_constants_unary_double_value(node, static_cast<CConstDouble*>(constant)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_T::CConstUInt_t: {
            TUInt value = fold_constants_unary_uint_value(node, static_cast<CConstUInt*>(constant)->value);
            fold_constant = std::make_shared<CConstUInt>(std::move(value));
            break;
        }
        case AST_T::CConstULong_t: {
            TULong value = fold_constants_unary_ulong_value(node, static_cast<CConstULong*>(constant)->value);
            fold_constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
}

static TInt fold_constants_binary_int_value(TacBinaryOp* node, TInt value_1, TInt value_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t:
            return value_1 + value_2;
        case AST_T::TacSubtract_t:
            return value_1 - value_2;
        case AST_T::TacMultiply_t:
            return value_1 * value_2;
        case AST_T::TacDivide_t:
            return value_2 == 0 ? INT32_MAX : value_1 / value_2;
        case AST_T::TacRemainder_t:
            return value_2 == 0 ? 0 : value_1 % value_2;
        case AST_T::TacBitAnd_t:
            return value_1 & value_2;
        case AST_T::TacBitOr_t:
            return value_1 | value_2;
        case AST_T::TacBitXor_t:
            return value_1 ^ value_2;
        case AST_T::TacBitShiftLeft_t:
            return value_1 << value_2;
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t:
            return value_1 >> value_2;
        case AST_T::TacEqual_t:
            return value_1 == value_2;
        case AST_T::TacNotEqual_t:
            return value_1 != value_2;
        case AST_T::TacLessThan_t:
            return value_1 < value_2;
        case AST_T::TacLessOrEqual_t:
            return value_1 <= value_2;
        case AST_T::TacGreaterThan_t:
            return value_1 > value_2;
        case AST_T::TacGreaterOrEqual_t:
            return value_1 >= value_2;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TLong fold_constants_binary_long_value(TacBinaryOp* node, TLong value_1, TLong value_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t:
            return value_1 + value_2;
        case AST_T::TacSubtract_t:
            return value_1 - value_2;
        case AST_T::TacMultiply_t:
            return value_1 * value_2;
        case AST_T::TacDivide_t:
            return value_1 / value_2;
        case AST_T::TacRemainder_t:
            return value_1 % value_2;
        case AST_T::TacBitAnd_t:
            return value_1 & value_2;
        case AST_T::TacBitOr_t:
            return value_1 | value_2;
        case AST_T::TacBitXor_t:
            return value_1 ^ value_2;
        case AST_T::TacBitShiftLeft_t:
            return value_1 << value_2;
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t:
            return value_1 >> value_2;
        case AST_T::TacEqual_t:
            return value_1 == value_2;
        case AST_T::TacNotEqual_t:
            return value_1 != value_2;
        case AST_T::TacLessThan_t:
            return value_1 < value_2;
        case AST_T::TacLessOrEqual_t:
            return value_1 <= value_2;
        case AST_T::TacGreaterThan_t:
            return value_1 > value_2;
        case AST_T::TacGreaterOrEqual_t:
            return value_1 >= value_2;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TDouble fold_constants_binary_double_value(TacBinaryOp* node, TDouble value_1, TDouble value_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t:
            return value_1 + value_2;
        case AST_T::TacSubtract_t:
            return value_1 - value_2;
        case AST_T::TacMultiply_t:
            return value_1 * value_2;
        case AST_T::TacDivide_t:
            return value_1 / value_2;
        case AST_T::TacEqual_t:
            return value_1 == value_2;
        case AST_T::TacNotEqual_t:
            return value_1 != value_2;
        case AST_T::TacLessThan_t:
            return value_1 < value_2;
        case AST_T::TacLessOrEqual_t:
            return value_1 <= value_2;
        case AST_T::TacGreaterThan_t:
            return value_1 > value_2;
        case AST_T::TacGreaterOrEqual_t:
            return value_1 >= value_2;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TUInt fold_constants_binary_uint_value(TacBinaryOp* node, TUInt value_1, TUInt value_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t:
            return value_1 + value_2;
        case AST_T::TacSubtract_t:
            return value_1 - value_2;
        case AST_T::TacMultiply_t:
            return value_1 * value_2;
        case AST_T::TacDivide_t:
            return value_1 / value_2;
        case AST_T::TacRemainder_t:
            return value_1 % value_2;
        case AST_T::TacBitAnd_t:
            return value_1 & value_2;
        case AST_T::TacBitOr_t:
            return value_1 | value_2;
        case AST_T::TacBitXor_t:
            return value_1 ^ value_2;
        case AST_T::TacBitShiftLeft_t:
            return value_1 << value_2;
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t:
            return value_1 >> value_2;
        case AST_T::TacEqual_t:
            return value_1 == value_2;
        case AST_T::TacNotEqual_t:
            return value_1 != value_2;
        case AST_T::TacLessThan_t:
            return value_1 < value_2;
        case AST_T::TacLessOrEqual_t:
            return value_1 <= value_2;
        case AST_T::TacGreaterThan_t:
            return value_1 > value_2;
        case AST_T::TacGreaterOrEqual_t:
            return value_1 >= value_2;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static TULong fold_constants_binary_ulong_value(TacBinaryOp* node, TULong value_1, TULong value_2) {
    switch (node->type()) {
        case AST_T::TacAdd_t:
            return value_1 + value_2;
        case AST_T::TacSubtract_t:
            return value_1 - value_2;
        case AST_T::TacMultiply_t:
            return value_1 * value_2;
        case AST_T::TacDivide_t:
            return value_1 / value_2;
        case AST_T::TacRemainder_t:
            return value_1 % value_2;
        case AST_T::TacBitAnd_t:
            return value_1 & value_2;
        case AST_T::TacBitOr_t:
            return value_1 | value_2;
        case AST_T::TacBitXor_t:
            return value_1 ^ value_2;
        case AST_T::TacBitShiftLeft_t:
            return value_1 << value_2;
        case AST_T::TacBitShiftRight_t:
        case AST_T::TacBitShrArithmetic_t:
            return value_1 >> value_2;
        case AST_T::TacEqual_t:
            return value_1 == value_2;
        case AST_T::TacNotEqual_t:
            return value_1 != value_2;
        case AST_T::TacLessThan_t:
            return value_1 < value_2;
        case AST_T::TacLessOrEqual_t:
            return value_1 <= value_2;
        case AST_T::TacGreaterThan_t:
            return value_1 > value_2;
        case AST_T::TacGreaterOrEqual_t:
            return value_1 >= value_2;
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
            TInt value = fold_constants_binary_int_value(
                node, static_cast<CConstInt*>(constant_1)->value, static_cast<CConstInt*>(constant_2)->value);
            fold_constant = std::make_shared<CConstInt>(std::move(value));
            break;
        }
        case AST_T::CConstLong_t: {
            TLong value = fold_constants_binary_long_value(
                node, static_cast<CConstLong*>(constant_1)->value, static_cast<CConstLong*>(constant_2)->value);
            fold_constant = std::make_shared<CConstLong>(std::move(value));
            break;
        }
        case AST_T::CConstDouble_t: {
            TDouble value = fold_constants_binary_double_value(
                node, static_cast<CConstDouble*>(constant_1)->value, static_cast<CConstDouble*>(constant_2)->value);
            fold_constant = std::make_shared<CConstDouble>(std::move(value));
            break;
        }
        case AST_T::CConstUInt_t: {
            TUInt value = fold_constants_binary_uint_value(
                node, static_cast<CConstUInt*>(constant_1)->value, static_cast<CConstUInt*>(constant_2)->value);
            fold_constant = std::make_shared<CConstUInt>(std::move(value));
            break;
        }
        case AST_T::CConstULong_t: {
            TULong value = fold_constants_binary_ulong_value(
                node, static_cast<CConstULong*>(constant_1)->value, static_cast<CConstULong*>(constant_2)->value);
            fold_constant = std::make_shared<CConstULong>(std::move(value));
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
    return std::make_shared<TacConstant>(std::move(fold_constant));
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

static void set_instruction(std::unique_ptr<TacInstruction>&& instruction) {
    if (instruction) {
        (*context->p_instructions)[context->instruction_index] = std::move(instruction);
    }
    else {
        (*context->p_instructions)[context->instruction_index].reset();
    }
    context->is_fixed_point = false;
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

static void fold_constants_unary_instructions(TacUnary* node) {
    if (node->src->type() == AST_T::TacConstant_t) {
        std::shared_ptr<TacValue> src = fold_constants_unary_constant_value(
            node->unary_op.get(), static_cast<TacConstant*>(node->src.get())->constant.get());
        std::shared_ptr<TacValue> dst = node->dst;
        set_instruction(std::make_unique<TacCopy>(std::move(src), std::move(dst)));
    }
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
        // case AST_T::TacDoubleToInt_t:
        //     // fold_constants_double_to_signed_instructions(static_cast<TacDoubleToInt*>(node)); // TODO
        //     break;
        // case AST_T::TacDoubleToUInt_t:
        //     // fold_constants_double_to_unsigned_instructions(static_cast<TacDoubleToUInt*>(node)); // TODO
        //     break;
        // case AST_T::TacIntToDouble_t:
        //     // fold_constants_signed_to_double_instructions(static_cast<TacIntToDouble*>(node)); // TODO
        //     break;
        // case AST_T::TacUIntToDouble_t:
        //     // fold_constants_unsigned_to_double_instructions(static_cast<TacUIntToDouble*>(node)); // TODO
        //     break;
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

static void fold_constants_list_instructions(std::vector<std::unique_ptr<TacInstruction>>& list_node) {
    context->p_instructions = &list_node;
    for (context->instruction_index = 0; context->instruction_index < list_node.size(); ++context->instruction_index) {
        if ((*context->p_instructions)[context->instruction_index]) {
            fold_constants_instructions((*context->p_instructions)[context->instruction_index].get());
        }
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
    do {
        context->is_fixed_point = true;
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
