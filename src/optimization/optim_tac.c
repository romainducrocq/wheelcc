#include <string.h>

#include "util/c_std.h"
#include "util/str2t.h"
#include "util/throw.h"

#include "ast/front_ast.h"
#include "ast/front_symt.h"
#include "ast/interm_ast.h"
#include "ast_t.h" // ast

#include "optimization/optim_tac.h"

typedef struct ControlFlowGraph ControlFlowGraph;
typedef struct DataFlowAnalysis DataFlowAnalysis;
typedef struct DataFlowAnalysisO1 DataFlowAnalysisO1;

typedef struct OptimTacContext {
    FrontEndContext* frontend;
    // Constant folding
    // Unreachable code elimination
    // Copy propagation
    // Dead store elimination
    bool is_fixed_point;
    bool enabled_optims[5];
    unique_ptr_t(ControlFlowGraph) cfg;
    unique_ptr_t(DataFlowAnalysis) dfa;
    unique_ptr_t(DataFlowAnalysisO1) dfa_o1;
    vector_t(unique_ptr_t(TacInstruction)) * p_instrs;
} OptimTacContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Three address code optimization

#ifndef __OPTIM_LEVEL__
#define __OPTIM_LEVEL__ 1
#undef _OPTIMIZATION_IMPL_OLVL_H
#include "impl_olvl.h" // optimization
#undef __OPTIM_LEVEL__
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constant folding

static shared_ptr_t(CConst) fold_sign_extend_char_const(Ctx ctx, TacVariable* node, CConstChar* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Int_t: {
            TInt value = (TInt)constant->value;
            return make_CConstInt(value);
        }
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = (TLong)constant->value;
            return make_CConstLong(value);
        }
        case AST_UInt_t: {
            TUInt value = (TUInt)constant->value;
            return make_CConstUInt(value);
        }
        case AST_ULong_t: {
            TULong value = (TULong)constant->value;
            return make_CConstULong(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_sign_extend_int_const(Ctx ctx, TacVariable* node, CConstInt* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = (TLong)constant->value;
            return make_CConstLong(value);
        }
        case AST_ULong_t: {
            TULong value = (TULong)constant->value;
            return make_CConstULong(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) fold_sign_extend_const(Ctx ctx, TacVariable* node, CConst* constant) {
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant->type) {
        case AST_CConstChar_t: {
            fold_constant = fold_sign_extend_char_const(ctx, node, &constant->get._CConstChar);
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_sign_extend_int_const(ctx, node, &constant->get._CConstInt);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_sign_extend_instr(Ctx ctx, TacSignExtend* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
        shared_ptr_t(TacValue) src =
            fold_sign_extend_const(ctx, &node->dst->get._TacVariable, node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(CConst) fold_truncate_int_const(Ctx ctx, TacVariable* node, CConstInt* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = (TChar)constant->value;
            return make_CConstChar(value);
        }
        case AST_UChar_t: {
            TUChar value = (TUChar)constant->value;
            return make_CConstUChar(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_truncate_long_const(Ctx ctx, TacVariable* node, CConstLong* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = (TChar)constant->value;
            return make_CConstChar(value);
        }
        case AST_Int_t: {
            TInt value = (TInt)constant->value;
            return make_CConstInt(value);
        }
        case AST_UChar_t: {
            TUChar value = (TUChar)constant->value;
            return make_CConstUChar(value);
        }
        case AST_UInt_t: {
            TUInt value = (TUInt)constant->value;
            return make_CConstUInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_truncate_uint_const(Ctx ctx, TacVariable* node, CConstUInt* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = (TChar)constant->value;
            return make_CConstChar(value);
        }
        case AST_UChar_t: {
            TUChar value = (TUChar)constant->value;
            return make_CConstUChar(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_truncate_ulong_const(Ctx ctx, TacVariable* node, CConstULong* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = (TChar)constant->value;
            return make_CConstChar(value);
        }
        case AST_Int_t: {
            TInt value = (TInt)constant->value;
            return make_CConstInt(value);
        }
        case AST_UChar_t: {
            TUChar value = (TUChar)constant->value;
            return make_CConstUChar(value);
        }
        case AST_UInt_t: {
            TUInt value = (TUInt)constant->value;
            return make_CConstUInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) fold_truncate_const(Ctx ctx, TacVariable* node, CConst* constant) {
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant->type) {
        case AST_CConstInt_t: {
            fold_constant = fold_truncate_int_const(ctx, node, &constant->get._CConstInt);
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_truncate_long_const(ctx, node, &constant->get._CConstLong);
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_truncate_uint_const(ctx, node, &constant->get._CConstUInt);
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_truncate_ulong_const(ctx, node, &constant->get._CConstULong);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_truncate_instr(Ctx ctx, TacTruncate* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
        shared_ptr_t(TacValue) src =
            fold_truncate_const(ctx, &node->dst->get._TacVariable, node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(CConst) fold_zero_extend_uchar_const(Ctx ctx, TacVariable* node, CConstUChar* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Int_t: {
            TInt value = (TInt)constant->value;
            return make_CConstInt(value);
        }
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = (TLong)constant->value;
            return make_CConstLong(value);
        }
        case AST_UInt_t: {
            TUInt value = (TUInt)constant->value;
            return make_CConstUInt(value);
        }
        case AST_ULong_t: {
            TULong value = (TULong)constant->value;
            return make_CConstULong(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_zero_extend_uint_const(Ctx ctx, TacVariable* node, CConstUInt* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Long_t:
        case AST_Pointer_t: {
            TLong value = (TLong)constant->value;
            return make_CConstLong(value);
        }
        case AST_ULong_t: {
            TULong value = (TULong)constant->value;
            return make_CConstULong(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) fold_zero_extend_const(Ctx ctx, TacVariable* node, CConst* constant) {
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant->type) {
        case AST_CConstUChar_t: {
            fold_constant = fold_zero_extend_uchar_const(ctx, node, &constant->get._CConstUChar);
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_zero_extend_uint_const(ctx, node, &constant->get._CConstUInt);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_zero_extend_instr(Ctx ctx, TacZeroExtend* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
        shared_ptr_t(TacValue) src =
            fold_zero_extend_const(ctx, &node->dst->get._TacVariable, node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(TacValue) fold_dbl_to_signed_const(Ctx ctx, TacVariable* node, CConst* constant) {
    THROW_ABORT_IF(constant->type != AST_CConstDouble_t);
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = (TChar)constant->get._CConstDouble.value;
            fold_constant = make_CConstChar(value);
            break;
        }
        case AST_Int_t: {
            TInt value = (TInt)constant->get._CConstDouble.value;
            fold_constant = make_CConstInt(value);
            break;
        }
        case AST_Long_t: {
            TLong value = (TLong)constant->get._CConstDouble.value;
            fold_constant = make_CConstLong(value);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_dbl_to_signed_instr(Ctx ctx, TacDoubleToInt* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
        shared_ptr_t(TacValue) src =
            fold_dbl_to_signed_const(ctx, &node->dst->get._TacVariable, node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(TacValue) fold_dbl_to_unsigned_const(Ctx ctx, TacVariable* node, CConst* constant) {
    THROW_ABORT_IF(constant->type != AST_CConstDouble_t);
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_UChar_t: {
            TUChar value = (TUChar)constant->get._CConstDouble.value;
            fold_constant = make_CConstUChar(value);
            break;
        }
        case AST_UInt_t: {
            TUInt value = (TUInt)constant->get._CConstDouble.value;
            fold_constant = make_CConstUInt(value);
            break;
        }
        case AST_ULong_t: {
            TULong value = (TULong)constant->get._CConstDouble.value;
            fold_constant = make_CConstULong(value);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_dbl_to_unsigned_instr(Ctx ctx, TacDoubleToUInt* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
        shared_ptr_t(TacValue) src =
            fold_dbl_to_unsigned_const(ctx, &node->dst->get._TacVariable, node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(TacValue) fold_signed_to_dbl_const(CConst* constant) {
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant->type) {
        case AST_CConstChar_t: {
            TDouble value = (TDouble)constant->get._CConstChar.value;
            fold_constant = make_CConstDouble(value);
            break;
        }
        case AST_CConstInt_t: {
            TDouble value = (TDouble)constant->get._CConstInt.value;
            fold_constant = make_CConstDouble(value);
            break;
        }
        case AST_CConstLong_t: {
            TDouble value = (TDouble)constant->get._CConstLong.value;
            fold_constant = make_CConstDouble(value);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_signed_to_dbl_instr(Ctx ctx, TacIntToDouble* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(
            node->dst->type != AST_TacVariable_t
            || map_get(ctx->frontend->symbol_table, node->dst->get._TacVariable.name)->type_t->type != AST_Double_t);
        shared_ptr_t(TacValue) src = fold_signed_to_dbl_const(node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(TacValue) fold_unsigned_to_dbl_const(CConst* constant) {
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant->type) {
        case AST_CConstUChar_t: {
            TDouble value = (TDouble)constant->get._CConstUChar.value;
            fold_constant = make_CConstDouble(value);
            break;
        }
        case AST_CConstUInt_t: {
            TDouble value = (TDouble)constant->get._CConstUInt.value;
            fold_constant = make_CConstDouble(value);
            break;
        }
        case AST_CConstULong_t: {
            TDouble value = (TDouble)constant->get._CConstULong.value;
            fold_constant = make_CConstDouble(value);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_unsigned_to_dbl_instr(Ctx ctx, TacUIntToDouble* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(
            node->dst->type != AST_TacVariable_t
            || map_get(ctx->frontend->symbol_table, node->dst->get._TacVariable.name)->type_t->type != AST_Double_t);
        shared_ptr_t(TacValue) src = fold_unsigned_to_dbl_const(node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(CConst) fold_unary_char_const(TacUnaryOp* node, CConstChar* constant) {
    if (node->type == AST_TacNot_t) {
        TInt value = !constant->value ? 1 : 0;
        return make_CConstInt(value);
    }
    else {
        THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_unary_int_const(TacUnaryOp* node, CConstInt* constant) {
    switch (node->type) {
        case AST_TacComplement_t: {
            TInt value = ~constant->value;
            return make_CConstInt(value);
        }
        case AST_TacNegate_t: {
            TInt value = -constant->value;
            return make_CConstInt(value);
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_unary_long_const(TacUnaryOp* node, CConstLong* constant) {
    switch (node->type) {
        case AST_TacComplement_t: {
            TLong value = ~constant->value;
            return make_CConstLong(value);
        }
        case AST_TacNegate_t: {
            TLong value = -constant->value;
            return make_CConstLong(value);
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_unary_dbl_const(TacUnaryOp* node, CConstDouble* constant) {
    switch (node->type) {
        case AST_TacNegate_t: {
            TDouble value = -constant->value;
            return make_CConstDouble(value);
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_unary_uchar_const(TacUnaryOp* node, CConstUChar* constant) {
    if (node->type == AST_TacNot_t) {
        TInt value = !constant->value ? 1 : 0;
        return make_CConstInt(value);
    }
    else {
        THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_unary_uint_const(TacUnaryOp* node, CConstUInt* constant) {
    switch (node->type) {
        case AST_TacComplement_t: {
            TUInt value = ~constant->value;
            return make_CConstUInt(value);
        }
        case AST_TacNegate_t: {
            TUInt value = -constant->value;
            return make_CConstUInt(value);
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_unary_ulong_const(TacUnaryOp* node, CConstULong* constant) {
    switch (node->type) {
        case AST_TacComplement_t: {
            TULong value = ~constant->value;
            return make_CConstULong(value);
        }
        case AST_TacNegate_t: {
            TULong value = -constant->value;
            return make_CConstULong(value);
        }
        case AST_TacNot_t: {
            TInt value = !constant->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) fold_unary_const(TacUnaryOp* node, CConst* constant) {
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant->type) {
        case AST_CConstChar_t: {
            fold_constant = fold_unary_char_const(node, &constant->get._CConstChar);
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_unary_int_const(node, &constant->get._CConstInt);
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_unary_long_const(node, &constant->get._CConstLong);
            break;
        }
        case AST_CConstDouble_t: {
            fold_constant = fold_unary_dbl_const(node, &constant->get._CConstDouble);
            break;
        }
        case AST_CConstUChar_t: {
            fold_constant = fold_unary_uchar_const(node, &constant->get._CConstUChar);
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_unary_uint_const(node, &constant->get._CConstUInt);
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_unary_ulong_const(node, &constant->get._CConstULong);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_unary_instr(Ctx ctx, TacUnary* node, size_t instr_idx) {
    if (node->src->type == AST_TacConstant_t) {
        shared_ptr_t(TacValue) src = fold_unary_const(node->unop, node->src->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(CConst) fold_binary_int_const(TacBinaryOp* node, CConstInt* constant_1, CConstInt* constant_2) {
    switch (node->type) {
        case AST_TacAdd_t: {
            TInt value = constant_1->value + constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacSubtract_t: {
            TInt value = constant_1->value - constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacMultiply_t: {
            TInt value = constant_1->value * constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacDivide_t: {
            TInt value = constant_2->value != 0 ? constant_1->value / constant_2->value : 0;
            return make_CConstInt(value);
        }
        case AST_TacRemainder_t: {
            TInt value = constant_2->value != 0 ? constant_1->value % constant_2->value : 0;
            return make_CConstInt(value);
        }
        case AST_TacBitAnd_t: {
            TInt value = constant_1->value & constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacBitOr_t: {
            TInt value = constant_1->value | constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacBitXor_t: {
            TInt value = constant_1->value ^ constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacBitShiftLeft_t: {
            TInt value = constant_1->value << constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TInt value = constant_1->value >> constant_2->value;
            return make_CConstInt(value);
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_binary_long_const(TacBinaryOp* node, CConstLong* constant_1, CConstLong* constant_2) {
    switch (node->type) {
        case AST_TacAdd_t: {
            TLong value = constant_1->value + constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacSubtract_t: {
            TLong value = constant_1->value - constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacMultiply_t: {
            TLong value = constant_1->value * constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacDivide_t: {
            TLong value = constant_2->value != 0l ? constant_1->value / constant_2->value : 0l;
            return make_CConstLong(value);
        }
        case AST_TacRemainder_t: {
            TLong value = constant_2->value != 0l ? constant_1->value % constant_2->value : 0l;
            return make_CConstLong(value);
        }
        case AST_TacBitAnd_t: {
            TLong value = constant_1->value & constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacBitOr_t: {
            TLong value = constant_1->value | constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacBitXor_t: {
            TLong value = constant_1->value ^ constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacBitShiftLeft_t: {
            TLong value = constant_1->value << constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TLong value = constant_1->value >> constant_2->value;
            return make_CConstLong(value);
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst)
    fold_binary_dbl_const(TacBinaryOp* node, CConstDouble* constant_1, CConstDouble* constant_2) {
    switch (node->type) {
        case AST_TacAdd_t: {
            TDouble value = constant_1->value + constant_2->value;
            return make_CConstDouble(value);
        }
        case AST_TacSubtract_t: {
            TDouble value = constant_1->value - constant_2->value;
            return make_CConstDouble(value);
        }
        case AST_TacMultiply_t: {
            TDouble value = constant_1->value * constant_2->value;
            return make_CConstDouble(value);
        }
        case AST_TacDivide_t: {
            TDouble value = constant_1->value / constant_2->value;
            return make_CConstDouble(value);
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_binary_uint_const(TacBinaryOp* node, CConstUInt* constant_1, CConstUInt* constant_2) {
    switch (node->type) {
        case AST_TacAdd_t: {
            TUInt value = constant_1->value + constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacSubtract_t: {
            TUInt value = constant_1->value - constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacMultiply_t: {
            TUInt value = constant_1->value * constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacDivide_t: {
            TUInt value = constant_2->value != 0u ? constant_1->value / constant_2->value : 0u;
            return make_CConstUInt(value);
        }
        case AST_TacRemainder_t: {
            TUInt value = constant_2->value != 0u ? constant_1->value % constant_2->value : 0u;
            return make_CConstUInt(value);
        }
        case AST_TacBitAnd_t: {
            TUInt value = constant_1->value & constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacBitOr_t: {
            TUInt value = constant_1->value | constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacBitXor_t: {
            TUInt value = constant_1->value ^ constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacBitShiftLeft_t: {
            TUInt value = constant_1->value << constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TUInt value = constant_1->value >> constant_2->value;
            return make_CConstUInt(value);
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst)
    fold_binary_ulong_const(TacBinaryOp* node, CConstULong* constant_1, CConstULong* constant_2) {
    switch (node->type) {
        case AST_TacAdd_t: {
            TULong value = constant_1->value + constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacSubtract_t: {
            TULong value = constant_1->value - constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacMultiply_t: {
            TULong value = constant_1->value * constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacDivide_t: {
            TULong value = constant_2->value != 0ul ? constant_1->value / constant_2->value : 0ul;
            return make_CConstULong(value);
        }
        case AST_TacRemainder_t: {
            TULong value = constant_2->value != 0ul ? constant_1->value % constant_2->value : 0ul;
            return make_CConstULong(value);
        }
        case AST_TacBitAnd_t: {
            TULong value = constant_1->value & constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacBitOr_t: {
            TULong value = constant_1->value | constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacBitXor_t: {
            TULong value = constant_1->value ^ constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacBitShiftLeft_t: {
            TULong value = constant_1->value << constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacBitShiftRight_t:
        case AST_TacBitShrArithmetic_t: {
            TULong value = constant_1->value >> constant_2->value;
            return make_CConstULong(value);
        }
        case AST_TacEqual_t: {
            TInt value = constant_1->value == constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacNotEqual_t: {
            TInt value = constant_1->value != constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessThan_t: {
            TInt value = constant_1->value < constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacLessOrEqual_t: {
            TInt value = constant_1->value <= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterThan_t: {
            TInt value = constant_1->value > constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        case AST_TacGreaterOrEqual_t: {
            TInt value = constant_1->value >= constant_2->value ? 1 : 0;
            return make_CConstInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) fold_binary_const(TacBinaryOp* node, CConst* constant_1, CConst* constant_2) {
    THROW_ABORT_IF(constant_1->type != constant_2->type);
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant_1->type) {
        case AST_CConstInt_t: {
            fold_constant = fold_binary_int_const(node, &constant_1->get._CConstInt, &constant_2->get._CConstInt);
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_binary_long_const(node, &constant_1->get._CConstLong, &constant_2->get._CConstLong);
            break;
        }
        case AST_CConstDouble_t: {
            fold_constant = fold_binary_dbl_const(node, &constant_1->get._CConstDouble, &constant_2->get._CConstDouble);
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_binary_uint_const(node, &constant_1->get._CConstUInt, &constant_2->get._CConstUInt);
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_binary_ulong_const(node, &constant_1->get._CConstULong, &constant_2->get._CConstULong);
            break;
        }
        default:
            THROW_ABORT;
    }
    return make_TacConstant(&fold_constant);
}

static void fold_binary_instr(Ctx ctx, TacBinary* node, size_t instr_idx) {
    if (node->src1->type == AST_TacConstant_t && node->src2->type == AST_TacConstant_t) {
        shared_ptr_t(TacValue) src = fold_binary_const(
            node->binop, node->src1->get._TacConstant.constant, node->src2->get._TacConstant.constant);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        set_instr(ctx, make_TacCopy(&src, &dst), instr_idx);
    }
}

static shared_ptr_t(CConst) fold_copy_char_const(Ctx ctx, TacVariable* node, CConstChar* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t:
            return sptr_new();
        case AST_UChar_t: {
            TUChar value = (TUChar)constant->value;
            return make_CConstUChar(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_copy_int_const(Ctx ctx, TacVariable* node, CConstInt* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Int_t:
            return sptr_new();
        case AST_UInt_t: {
            TUInt value = (TUInt)constant->value;
            return make_CConstUInt(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_copy_long_const(Ctx ctx, TacVariable* node, CConstLong* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Long_t:
        case AST_Pointer_t:
            return sptr_new();
        case AST_Double_t: {
            TDouble value = (TDouble)constant->value;
            return make_CConstDouble(value);
        }
        case AST_ULong_t: {
            TULong value = (TULong)constant->value;
            return make_CConstULong(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_copy_dbl_const(Ctx ctx, TacVariable* node, CConstDouble* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Long_t: {
            TLong value = (TLong)constant->value;
            return make_CConstLong(value);
        }
        case AST_Double_t:
            return sptr_new();
        case AST_ULong_t: {
            TULong value = (TULong)constant->value;
            return make_CConstULong(value);
        }
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_copy_uchar_const(Ctx ctx, TacVariable* node, CConstUChar* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Char_t:
        case AST_SChar_t: {
            TChar value = (TChar)constant->value;
            return make_CConstChar(value);
        }
        case AST_UChar_t:
            return sptr_new();
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_copy_uint_const(Ctx ctx, TacVariable* node, CConstUInt* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Int_t: {
            TInt value = (TInt)constant->value;
            return make_CConstInt(value);
        }
        case AST_UInt_t:
            return sptr_new();
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(CConst) fold_copy_ulong_const(Ctx ctx, TacVariable* node, CConstULong* constant) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
        case AST_Long_t: {
            TLong value = (TLong)constant->value;
            return make_CConstLong(value);
        }
        case AST_Double_t: {
            TDouble value = (TDouble)constant->value;
            return make_CConstDouble(value);
        }
        case AST_Pointer_t:
        case AST_ULong_t:
            return sptr_new();
        default:
            THROW_ABORT;
    }
}

static shared_ptr_t(TacValue) fold_copy_const(Ctx ctx, TacVariable* node, CConst* constant) {
    shared_ptr_t(CConst) fold_constant = sptr_new();
    switch (constant->type) {
        case AST_CConstChar_t: {
            fold_constant = fold_copy_char_const(ctx, node, &constant->get._CConstChar);
            break;
        }
        case AST_CConstInt_t: {
            fold_constant = fold_copy_int_const(ctx, node, &constant->get._CConstInt);
            break;
        }
        case AST_CConstLong_t: {
            fold_constant = fold_copy_long_const(ctx, node, &constant->get._CConstLong);
            break;
        }
        case AST_CConstDouble_t: {
            fold_constant = fold_copy_dbl_const(ctx, node, &constant->get._CConstDouble);
            break;
        }
        case AST_CConstUChar_t: {
            fold_constant = fold_copy_uchar_const(ctx, node, &constant->get._CConstUChar);
            break;
        }
        case AST_CConstUInt_t: {
            fold_constant = fold_copy_uint_const(ctx, node, &constant->get._CConstUInt);
            break;
        }
        case AST_CConstULong_t: {
            fold_constant = fold_copy_ulong_const(ctx, node, &constant->get._CConstULong);
            break;
        }
        default:
            THROW_ABORT;
    }
    if (fold_constant) {
        return make_TacConstant(&fold_constant);
    }
    else {
        return sptr_new();
    }
}

static void fold_copy_instr(Ctx ctx, TacCopy* node) {
    if (node->src->type == AST_TacConstant_t) {
        THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
        shared_ptr_t(TacValue) src =
            fold_copy_const(ctx, &node->dst->get._TacVariable, node->src->get._TacConstant.constant);
        if (src) {
            sptr_move(TacValue, src, node->src);
            ctx->is_fixed_point = false;
        }
    }
}

static bool is_const_zero(CConst* constant) {
    switch (constant->type) {
        case AST_CConstChar_t:
            return constant->get._CConstChar.value == 0;
        case AST_CConstInt_t:
            return constant->get._CConstInt.value == 0;
        case AST_CConstLong_t:
            return constant->get._CConstLong.value == 0l;
        case AST_CConstDouble_t:
            return constant->get._CConstDouble.value == 0.0;
        case AST_CConstUChar_t:
            return constant->get._CConstUChar.value == 0u;
        case AST_CConstUInt_t:
            return constant->get._CConstUInt.value == 0u;
        case AST_CConstULong_t:
            return constant->get._CConstULong.value == 0ul;
        default:
            THROW_ABORT;
    }
}

static void fold_jmp_eq_0_instr(Ctx ctx, TacJumpIfZero* node, size_t instr_idx) {
    if (node->condition->type == AST_TacConstant_t) {
        if (is_const_zero(node->condition->get._TacConstant.constant)) {
            TIdentifier target = node->target;
            set_instr(ctx, make_TacJump(target), instr_idx);
        }
        else {
            set_instr(ctx, uptr_new(), instr_idx);
        }
    }
}

static void fold_jmp_ne_0_instr(Ctx ctx, TacJumpIfNotZero* node, size_t instr_idx) {
    if (node->condition->type == AST_TacConstant_t) {
        if (is_const_zero(node->condition->get._TacConstant.constant)) {
            set_instr(ctx, uptr_new(), instr_idx);
        }
        else {
            TIdentifier target = node->target;
            set_instr(ctx, make_TacJump(target), instr_idx);
        }
    }
}

static void fold_instr(Ctx ctx, size_t instr_idx) {
    TacInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_TacSignExtend_t:
            fold_sign_extend_instr(ctx, &node->get._TacSignExtend, instr_idx);
            break;
        case AST_TacTruncate_t:
            fold_truncate_instr(ctx, &node->get._TacTruncate, instr_idx);
            break;
        case AST_TacZeroExtend_t:
            fold_zero_extend_instr(ctx, &node->get._TacZeroExtend, instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            fold_dbl_to_signed_instr(ctx, &node->get._TacDoubleToInt, instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            fold_dbl_to_unsigned_instr(ctx, &node->get._TacDoubleToUInt, instr_idx);
            break;
        case AST_TacIntToDouble_t:
            fold_signed_to_dbl_instr(ctx, &node->get._TacIntToDouble, instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            fold_unsigned_to_dbl_instr(ctx, &node->get._TacUIntToDouble, instr_idx);
            break;
        case AST_TacUnary_t:
            fold_unary_instr(ctx, &node->get._TacUnary, instr_idx);
            break;
        case AST_TacBinary_t:
            fold_binary_instr(ctx, &node->get._TacBinary, instr_idx);
            break;
        case AST_TacCopy_t:
            fold_copy_instr(ctx, &node->get._TacCopy);
            break;
        case AST_TacJumpIfZero_t:
            fold_jmp_eq_0_instr(ctx, &node->get._TacJumpIfZero, instr_idx);
            break;
        case AST_TacJumpIfNotZero_t:
            fold_jmp_ne_0_instr(ctx, &node->get._TacJumpIfNotZero, instr_idx);
            break;
        default:
            break;
    }
}

static void fold_constants(Ctx ctx) {
    for (size_t instr_idx = 0; instr_idx < vec_size(*ctx->p_instrs); ++instr_idx) {
        if (GET_INSTR(instr_idx)) {
            fold_instr(ctx, instr_idx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Unreachable code elimination

static void unreach_reachable_block(Ctx ctx, size_t block_id);

static void unreach_succ_reachable_blocks(Ctx ctx, size_t block_id) {
    for (size_t i = 0; i < vec_size(GET_CFG_BLOCK(block_id).succ_ids); ++i) {
        unreach_reachable_block(ctx, GET_CFG_BLOCK(block_id).succ_ids[i]);
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
            set_instr(ctx, uptr_new(), instr_idx);
        }
    }
    GET_CFG_BLOCK(block_id).size = 0;
    cfg_rm_empty_block(ctx, block_id, false);
    vec_clear(GET_CFG_BLOCK(block_id).succ_ids);
    vec_clear(GET_CFG_BLOCK(block_id).pred_ids);
}

static void unreach_jump_instr(Ctx ctx, size_t block_id) {
    TacInstruction* node = GET_INSTR(GET_CFG_BLOCK(block_id).instrs_back_idx);
    switch (node->type) {
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
    if (vec_size(GET_CFG_BLOCK(block_id).succ_ids) == 1 && GET_CFG_BLOCK(block_id).succ_ids[0] == next_block_id) {
        unreach_jump_instr(ctx, block_id);
    }
}

static void unreach_label_instr(Ctx ctx, size_t block_id) {
    THROW_ABORT_IF(GET_INSTR(GET_CFG_BLOCK(block_id).instrs_front_idx)->type != AST_TacLabel_t);
    cfg_rm_block_instr(ctx, GET_CFG_BLOCK(block_id).instrs_front_idx, block_id);
}

static void unreach_label_block(Ctx ctx, size_t block_id, size_t prev_block_id) {
    if (vec_size(GET_CFG_BLOCK(block_id).pred_ids) == 1 && GET_CFG_BLOCK(block_id).pred_ids[0] == prev_block_id) {
        unreach_label_instr(ctx, block_id);
    }
}

static void eliminate_unreachable_code(Ctx ctx) {
    if (vec_empty(ctx->cfg->blocks)) {
        return;
    }
    if (vec_size(ctx->cfg->reaching_code) < vec_size(ctx->cfg->blocks)) {
        vec_resize(ctx->cfg->reaching_code, vec_size(ctx->cfg->blocks));
    }
    memset(ctx->cfg->reaching_code, false, sizeof(bool) * vec_size(ctx->cfg->blocks));
    for (size_t i = 0; i < vec_size(ctx->cfg->entry_succ_ids); ++i) {
        unreach_reachable_block(ctx, ctx->cfg->entry_succ_ids[i]);
    }

    size_t block_id = vec_size(ctx->cfg->blocks);
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

    for (size_t i = 0; i < map_size(ctx->cfg->identifier_id_map); ++i) {
        size_t label_id = pair_second(ctx->cfg->identifier_id_map[i]);
        if (ctx->cfg->reaching_code[label_id]) {
            for (block_id = label_id; block_id-- > 0;) {
                if (ctx->cfg->reaching_code[block_id]) {
                    next_block_id = block_id;
                    goto Lelse;
                }
            }
            next_block_id = ctx->cfg->entry_id;
        Lelse:
            unreach_label_block(ctx, label_id, next_block_id);
        }
        else {
            pair_second(ctx->cfg->identifier_id_map[i]) = ctx->cfg->exit_id;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy propagation

static bool is_aliased_value(Ctx ctx, TacValue* node) {
    return node->type == AST_TacVariable_t && is_aliased_name(ctx, node->get._TacVariable.name);
}

static bool is_const_signed(TacConstant* node) {
    switch (node->constant->type) {
        case AST_CConstChar_t:
        case AST_CConstInt_t:
        case AST_CConstLong_t:
            return true;
        default:
            return false;
    }
}

static bool is_var_signed(Ctx ctx, TacVariable* node) {
    switch (map_get(ctx->frontend->symbol_table, node->name)->type_t->type) {
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
    switch (node->type) {
        case AST_TacConstant_t:
            return is_const_signed(&node->get._TacConstant);
        case AST_TacVariable_t:
            return is_var_signed(ctx, &node->get._TacVariable);
        default:
            THROW_ABORT;
    }
}

static bool is_const_null_ptr(TacConstant* node) {
    switch (node->constant->type) {
        case AST_CConstInt_t:
            return node->constant->get._CConstInt.value == 0;
        case AST_CConstLong_t:
            return node->constant->get._CConstLong.value == 0l;
        case AST_CConstUInt_t:
            return node->constant->get._CConstUInt.value == 0u;
        case AST_CConstULong_t:
            return node->constant->get._CConstULong.value == 0ul;
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
    if (node_1->constant->type == node_2->constant->type) {
        switch (node_1->constant->type) {
            case AST_CConstChar_t:
                return node_1->constant->get._CConstChar.value == node_2->constant->get._CConstChar.value;
            case AST_CConstInt_t:
                return node_1->constant->get._CConstInt.value == node_2->constant->get._CConstInt.value;
            case AST_CConstLong_t:
                return node_1->constant->get._CConstLong.value == node_2->constant->get._CConstLong.value;
            case AST_CConstDouble_t:
                return is_dbl_same_const(&node_1->constant->get._CConstDouble, &node_2->constant->get._CConstDouble);
            case AST_CConstUChar_t:
                return node_1->constant->get._CConstUChar.value == node_2->constant->get._CConstUChar.value;
            case AST_CConstUInt_t:
                return node_1->constant->get._CConstUInt.value == node_2->constant->get._CConstUInt.value;
            case AST_CConstULong_t:
                return node_1->constant->get._CConstULong.value == node_2->constant->get._CConstULong.value;
            default:
                THROW_ABORT;
        }
    }
    return false;
}

static bool is_same_var(TacVariable* node_1, TacVariable* node_2) { return node_1->name == node_2->name; }

static bool is_same_value(TacValue* node_1, TacValue* node_2) {
    if (node_1->type == node_2->type) {
        switch (node_1->type) {
            case AST_TacConstant_t:
                return is_same_const(&node_1->get._TacConstant, &node_2->get._TacConstant);
            case AST_TacVariable_t:
                return is_same_var(&node_1->get._TacVariable, &node_2->get._TacVariable);
            default:
                THROW_ABORT;
        }
    }
    return false;
}

static bool is_same_name(TacValue* node, TIdentifier name) {
    switch (node->type) {
        case AST_TacConstant_t:
            return false;
        case AST_TacVariable_t:
            return node->get._TacVariable.name == name;
        default:
            THROW_ABORT;
    }
}

static bool is_copy_same_signedness(Ctx ctx, TacCopy* node) {
    return is_value_signed(ctx, node->src) == is_value_signed(ctx, node->dst);
}

static bool is_copy_null_ptr(Ctx ctx, TacCopy* node) {
    if (node->src->type == AST_TacConstant_t && node->dst->type == AST_TacVariable_t
        && map_get(ctx->frontend->symbol_table, node->dst->get._TacVariable.name)->type_t->type == AST_Pointer_t) {
        return is_const_null_ptr(&node->src->get._TacConstant);
    }
    else {
        return false;
    }
}

static void prop_transfer_dst_value(Ctx ctx, TacValue* node, size_t next_instr_idx) {
    THROW_ABORT_IF(node->type != AST_TacVariable_t);
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
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type != AST_TacCopy_t);
                TacCopy* copy = &GET_DFA_INSTR(i)->get._TacCopy;
                THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                if (is_same_value(node, copy->src) || is_same_value(node, copy->dst)) {
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
    THROW_ABORT_IF(node->dst && node->dst->type != AST_TacVariable_t);
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
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type != AST_TacCopy_t);
                TacCopy* copy = &GET_DFA_INSTR(i)->get._TacCopy;
                THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                if (is_aliased_value(ctx, copy->src) || is_aliased_value(ctx, copy->dst)
                    || (node->dst && (is_same_value(node->dst, copy->src) || is_same_value(node->dst, copy->dst)))) {
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
    THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
    for (size_t i = 0; i < ctx->dfa->set_size; ++i) {
        THROW_ABORT_IF(GET_DFA_INSTR(i)->type != AST_TacCopy_t);
        TacCopy* copy = &GET_DFA_INSTR(i)->get._TacCopy;
        THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
        if (is_same_value(node->dst, copy->dst)) {
            if ((is_copy_same_signedness(ctx, copy) || is_copy_null_ptr(ctx, copy))
                && is_same_value(node->src, copy->src)) {
                SET_DFA_INSTR_SET_AT(next_instr_idx, i, true);
            }
            else {
                SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
            }
        }
        else if (GET_DFA_INSTR_SET_AT(next_instr_idx, i)) {
            if (is_same_value(node->dst, copy->src)) {
                if (is_same_value(node->src, copy->dst)) {
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
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type != AST_TacCopy_t);
                TacCopy* copy = &GET_DFA_INSTR(i)->get._TacCopy;
                THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                if (is_aliased_value(ctx, copy->src) || is_aliased_value(ctx, copy->dst)) {
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
                THROW_ABORT_IF(GET_DFA_INSTR(i)->type != AST_TacCopy_t);
                TacCopy* copy = &GET_DFA_INSTR(i)->get._TacCopy;
                THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                if (is_same_name(copy->src, node->dst_name) || is_same_name(copy->dst, node->dst_name)) {
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
    TacInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_TacSignExtend_t:
            prop_transfer_dst_value(ctx, node->get._TacSignExtend.dst, next_instr_idx);
            break;
        case AST_TacTruncate_t:
            prop_transfer_dst_value(ctx, node->get._TacTruncate.dst, next_instr_idx);
            break;
        case AST_TacZeroExtend_t:
            prop_transfer_dst_value(ctx, node->get._TacZeroExtend.dst, next_instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            prop_transfer_dst_value(ctx, node->get._TacDoubleToInt.dst, next_instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            prop_transfer_dst_value(ctx, node->get._TacDoubleToUInt.dst, next_instr_idx);
            break;
        case AST_TacIntToDouble_t:
            prop_transfer_dst_value(ctx, node->get._TacIntToDouble.dst, next_instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            prop_transfer_dst_value(ctx, node->get._TacUIntToDouble.dst, next_instr_idx);
            break;
        case AST_TacFunCall_t:
            prop_transfer_call(ctx, &node->get._TacFunCall, next_instr_idx);
            break;
        case AST_TacUnary_t:
            prop_transfer_dst_value(ctx, node->get._TacUnary.dst, next_instr_idx);
            break;
        case AST_TacBinary_t:
            prop_transfer_dst_value(ctx, node->get._TacBinary.dst, next_instr_idx);
            break;
        case AST_TacCopy_t:
            return prop_transfer_copy(ctx, &node->get._TacCopy, next_instr_idx);
        case AST_TacGetAddress_t:
            prop_transfer_dst_value(ctx, node->get._TacGetAddress.dst, next_instr_idx);
            break;
        case AST_TacLoad_t:
            prop_transfer_dst_value(ctx, node->get._TacLoad.dst, next_instr_idx);
            break;
        case AST_TacStore_t:
            prop_transfer_store(ctx, next_instr_idx);
            break;
        case AST_TacAddPtr_t:
            prop_transfer_dst_value(ctx, node->get._TacAddPtr.dst, next_instr_idx);
            break;
        case AST_TacCopyToOffset_t:
            prop_transfer_cp_to_offset(ctx, &node->get._TacCopyToOffset, next_instr_idx);
            break;
        case AST_TacCopyFromOffset_t:
            prop_transfer_dst_value(ctx, node->get._TacCopyFromOffset.dst, next_instr_idx);
            break;
        default:
            THROW_ABORT;
    }
    return true;
}

static TacCopy* get_dfa_bak_copy_instr(Ctx ctx, size_t i) {
    TacInstruction* node = get_dfa_bak_instr(ctx, i);
    THROW_ABORT_IF(node->type != AST_TacCopy_t);
    return &node->get._TacCopy;
}

static void set_dfa_bak_copy_instr(Ctx ctx, TacCopy* node, size_t instr_idx) {
    size_t i;
    if (set_dfa_bak_instr(ctx, instr_idx, &i)) {
        shared_ptr_t(TacValue) src = sptr_new();
        sptr_copy(TacValue, node->src, src);
        shared_ptr_t(TacValue) dst = sptr_new();
        sptr_copy(TacValue, node->dst, dst);
        free_TacInstruction(&ctx->dfa_o1->bak_instrs[i]);
        ctx->dfa_o1->bak_instrs[i] = make_TacCopy(&src, &dst);
    }
}

static void prop_ret_instr(Ctx ctx, TacReturn* node, size_t incoming_idx, bool exit_block) {
    if (node->val && node->val->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->val, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->val);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_sign_extend_instr(Ctx ctx, TacSignExtend* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_truncate_instr(Ctx ctx, TacTruncate* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_zero_extend_instr(Ctx ctx, TacZeroExtend* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_dbl_to_int_instr(Ctx ctx, TacDoubleToInt* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_dbl_to_uint_instr(Ctx ctx, TacDoubleToUInt* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_int_to_dbl_instr(Ctx ctx, TacIntToDouble* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_uint_to_dbl_instr(Ctx ctx, TacUIntToDouble* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_call_instr(Ctx ctx, TacFunCall* node, size_t instr_idx) {
    for (size_t i = 0; i < vec_size(node->args); ++i) {
        if (node->args[i]->type == AST_TacVariable_t) {
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
                        THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                        if (is_same_value(node->args[i], copy->dst)) {
                            sptr_copy(TacValue, copy->src, node->args[i]);
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
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_binary_instr(Ctx ctx, TacBinary* node, size_t instr_idx) {
    bool is_src1 = node->src1->type == AST_TacVariable_t;
    bool is_src2 = node->src2->type == AST_TacVariable_t;
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_src1 && is_same_value(node->src1, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src1);
                        ctx->is_fixed_point = false;
                        is_src1 = false;
                        if (!is_src2) {
                            return;
                        }
                    }
                    if (is_src2 && is_same_value(node->src2, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src2);
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
    THROW_ABORT_IF(node->dst->type != AST_TacVariable_t);
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
                THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                if (ctx->dfa_o1->data_idx_map[i] == instr_idx
                    || (is_same_value(node->src, copy->dst) && is_same_value(node->dst, copy->src))) {
                    set_dfa_bak_copy_instr(ctx, node, instr_idx);
                    cfg_rm_block_instr(ctx, instr_idx, block_id);
                    return;
                }
                else if (is_same_value(node->src, copy->dst)) {
                    set_dfa_bak_copy_instr(ctx, node, instr_idx);
                    sptr_copy(TacValue, copy->src, node->src);
                    ctx->is_fixed_point = false;
                    return;
                }
            }
        }
    }
}

static void prop_load_instr(Ctx ctx, TacLoad* node, size_t instr_idx) {
    if (node->src_ptr->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src_ptr, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src_ptr);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_store_instr(Ctx ctx, TacStore* node, size_t instr_idx) {
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_add_ptr_instr(Ctx ctx, TacAddPtr* node, size_t instr_idx) {
    bool is_src_ptr = node->src_ptr->type == AST_TacVariable_t;
    bool is_idx = node->idx->type == AST_TacVariable_t;
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_src_ptr && is_same_value(node->src_ptr, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src_ptr);
                        ctx->is_fixed_point = false;
                        is_src_ptr = false;
                        if (!is_idx) {
                            return;
                        }
                    }
                    if (is_idx && is_same_value(node->idx, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->idx);
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
    if (node->src->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->src, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->src);
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
                THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                if (is_same_name(copy->dst, node->src_name)) {
                    THROW_ABORT_IF(copy->src->type != AST_TacVariable_t);
                    node->src_name = copy->src->get._TacVariable.name;
                    ctx->is_fixed_point = false;
                    return;
                }
            }
        }
    }
}

static void prop_jmp_eq_0_instr(Ctx ctx, TacJumpIfZero* node, size_t incoming_idx, size_t exit_block) {
    if (node->condition->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->condition, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->condition);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_jmp_ne_0_instr(Ctx ctx, TacJumpIfNotZero* node, size_t incoming_idx, size_t exit_block) {
    if (node->condition->type == AST_TacVariable_t) {
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
                    THROW_ABORT_IF(copy->dst->type != AST_TacVariable_t);
                    if (is_same_value(node->condition, copy->dst)) {
                        sptr_copy(TacValue, copy->src, node->condition);
                        ctx->is_fixed_point = false;
                        return;
                    }
                }
            }
        }
    }
}

static void prop_instr(Ctx ctx, size_t instr_idx, size_t copy_instr_idx, size_t block_id) {
    TacInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_TacReturn_t:
            prop_ret_instr(ctx, &node->get._TacReturn, copy_instr_idx, block_id > 0);
            break;
        case AST_TacSignExtend_t:
            prop_sign_extend_instr(ctx, &node->get._TacSignExtend, copy_instr_idx);
            break;
        case AST_TacTruncate_t:
            prop_truncate_instr(ctx, &node->get._TacTruncate, copy_instr_idx);
            break;
        case AST_TacZeroExtend_t:
            prop_zero_extend_instr(ctx, &node->get._TacZeroExtend, copy_instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            prop_dbl_to_int_instr(ctx, &node->get._TacDoubleToInt, copy_instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            prop_dbl_to_uint_instr(ctx, &node->get._TacDoubleToUInt, copy_instr_idx);
            break;
        case AST_TacIntToDouble_t:
            prop_int_to_dbl_instr(ctx, &node->get._TacIntToDouble, copy_instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            prop_uint_to_dbl_instr(ctx, &node->get._TacUIntToDouble, copy_instr_idx);
            break;
        case AST_TacFunCall_t:
            prop_call_instr(ctx, &node->get._TacFunCall, copy_instr_idx);
            break;
        case AST_TacUnary_t:
            prop_unary_instr(ctx, &node->get._TacUnary, copy_instr_idx);
            break;
        case AST_TacBinary_t:
            prop_binary_instr(ctx, &node->get._TacBinary, copy_instr_idx);
            break;
        case AST_TacCopy_t:
            prop_copy_instr(ctx, &node->get._TacCopy, copy_instr_idx, block_id);
            break;
        case AST_TacLoad_t:
            prop_load_instr(ctx, &node->get._TacLoad, copy_instr_idx);
            break;
        case AST_TacStore_t:
            prop_store_instr(ctx, &node->get._TacStore, copy_instr_idx);
            break;
        case AST_TacAddPtr_t:
            prop_add_ptr_instr(ctx, &node->get._TacAddPtr, copy_instr_idx);
            break;
        case AST_TacCopyToOffset_t:
            prop_cp_to_offset_instr(ctx, &node->get._TacCopyToOffset, copy_instr_idx);
            break;
        case AST_TacCopyFromOffset_t:
            prop_cp_from_offset_instr(ctx, &node->get._TacCopyFromOffset, copy_instr_idx);
            break;
        case AST_TacJumpIfZero_t:
            prop_jmp_eq_0_instr(ctx, &node->get._TacJumpIfZero, copy_instr_idx, block_id > 0);
            break;
        case AST_TacJumpIfNotZero_t:
            prop_jmp_ne_0_instr(ctx, &node->get._TacJumpIfNotZero, copy_instr_idx, block_id > 0);
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

    for (size_t block_id = 0; block_id < vec_size(ctx->cfg->blocks); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            size_t incoming_idx = block_id;
            size_t exit_block = 1;
            for (size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_back_idx + 1;
                 instr_idx-- > GET_CFG_BLOCK(block_id).instrs_front_idx;) {
                if (GET_INSTR(instr_idx)) {
                    switch (GET_INSTR(instr_idx)->type) {
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
    size_t i = map_get(ctx->cfg->identifier_id_map, name);
    SET_DFA_INSTR_SET_AT(next_instr_idx, i, true);
}

static void elim_transfer_src_value(Ctx ctx, TacValue* node, size_t next_instr_idx) {
    if (node->type == AST_TacVariable_t) {
        elim_transfer_src_name(ctx, node->get._TacVariable.name, next_instr_idx);
    }
}

static void elim_transfer_dst_value(Ctx ctx, TacValue* node, size_t next_instr_idx) {
    THROW_ABORT_IF(node->type != AST_TacVariable_t);
    size_t i = map_get(ctx->cfg->identifier_id_map, node->get._TacVariable.name);
    SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
}

static void elim_transfer_live_values(Ctx ctx, size_t instr_idx, size_t next_instr_idx) {
    TacInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_TacReturn_t: {
            TacReturn* p_node = &node->get._TacReturn;
            if (p_node->val) {
                elim_transfer_src_value(ctx, p_node->val, next_instr_idx);
            }
            break;
        }
        case AST_TacSignExtend_t: {
            TacSignExtend* p_node = &node->get._TacSignExtend;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacTruncate_t: {
            TacTruncate* p_node = &node->get._TacTruncate;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacZeroExtend_t: {
            TacZeroExtend* p_node = &node->get._TacZeroExtend;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacDoubleToInt_t: {
            TacDoubleToInt* p_node = &node->get._TacDoubleToInt;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacDoubleToUInt_t: {
            TacDoubleToUInt* p_node = &node->get._TacDoubleToUInt;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacIntToDouble_t: {
            TacIntToDouble* p_node = &node->get._TacIntToDouble;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacUIntToDouble_t: {
            TacUIntToDouble* p_node = &node->get._TacUIntToDouble;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacFunCall_t: {
            TacFunCall* p_node = &node->get._TacFunCall;
            if (p_node->dst) {
                elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            }
            for (size_t i = 0; i < vec_size(p_node->args); ++i) {
                elim_transfer_src_value(ctx, p_node->args[i], next_instr_idx);
            }
            elim_transfer_aliased(ctx, next_instr_idx);
            break;
        }
        case AST_TacUnary_t: {
            TacUnary* p_node = &node->get._TacUnary;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacBinary_t: {
            TacBinary* p_node = &node->get._TacBinary;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src1, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src2, next_instr_idx);
            break;
        }
        case AST_TacCopy_t: {
            TacCopy* p_node = &node->get._TacCopy;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_TacGetAddress_t:
            elim_transfer_dst_value(ctx, node->get._TacGetAddress.dst, next_instr_idx);
            break;
        case AST_TacLoad_t: {
            TacLoad* p_node = &node->get._TacLoad;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src_ptr, next_instr_idx);
            elim_transfer_addressed(ctx, next_instr_idx);
            break;
        }
        case AST_TacStore_t: {
            TacStore* p_node = &node->get._TacStore;
            elim_transfer_src_value(ctx, p_node->src, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->dst_ptr, next_instr_idx);
            break;
        }
        case AST_TacAddPtr_t: {
            TacAddPtr* p_node = &node->get._TacAddPtr;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->src_ptr, next_instr_idx);
            elim_transfer_src_value(ctx, p_node->idx, next_instr_idx);
            break;
        }
        case AST_TacCopyToOffset_t:
            elim_transfer_src_value(ctx, node->get._TacCopyToOffset.src, next_instr_idx);
            break;
        case AST_TacCopyFromOffset_t: {
            TacCopyFromOffset* p_node = &node->get._TacCopyFromOffset;
            elim_transfer_dst_value(ctx, p_node->dst, next_instr_idx);
            elim_transfer_src_name(ctx, p_node->src_name, next_instr_idx);
            break;
        }
        case AST_TacJumpIfZero_t:
            elim_transfer_src_value(ctx, node->get._TacJumpIfZero.condition, next_instr_idx);
            break;
        case AST_TacJumpIfNotZero_t:
            elim_transfer_src_value(ctx, node->get._TacJumpIfNotZero.condition, next_instr_idx);
            break;
        default:
            THROW_ABORT;
    }
}

static void elim_dst_name_instr(Ctx ctx, TIdentifier name, size_t instr_idx) {
    size_t i = map_get(ctx->cfg->identifier_id_map, name);
    if (!GET_DFA_INSTR_SET_AT(instr_idx, i)) {
        set_instr(ctx, uptr_new(), instr_idx);
    }
}

static void elim_dst_value_instr(Ctx ctx, TacValue* node, size_t instr_idx) {
    THROW_ABORT_IF(node->type != AST_TacVariable_t);
    elim_dst_name_instr(ctx, node->get._TacVariable.name, instr_idx);
}

static void elim_instr(Ctx ctx, size_t instr_idx) {
    TacInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_TacSignExtend_t:
            elim_dst_value_instr(ctx, node->get._TacSignExtend.dst, instr_idx);
            break;
        case AST_TacTruncate_t:
            elim_dst_value_instr(ctx, node->get._TacTruncate.dst, instr_idx);
            break;
        case AST_TacZeroExtend_t:
            elim_dst_value_instr(ctx, node->get._TacZeroExtend.dst, instr_idx);
            break;
        case AST_TacDoubleToInt_t:
            elim_dst_value_instr(ctx, node->get._TacDoubleToInt.dst, instr_idx);
            break;
        case AST_TacDoubleToUInt_t:
            elim_dst_value_instr(ctx, node->get._TacDoubleToUInt.dst, instr_idx);
            break;
        case AST_TacIntToDouble_t:
            elim_dst_value_instr(ctx, node->get._TacIntToDouble.dst, instr_idx);
            break;
        case AST_TacUIntToDouble_t:
            elim_dst_value_instr(ctx, node->get._TacUIntToDouble.dst, instr_idx);
            break;
        case AST_TacUnary_t:
            elim_dst_value_instr(ctx, node->get._TacUnary.dst, instr_idx);
            break;
        case AST_TacBinary_t:
            elim_dst_value_instr(ctx, node->get._TacBinary.dst, instr_idx);
            break;
        case AST_TacCopy_t:
            elim_dst_value_instr(ctx, node->get._TacCopy.dst, instr_idx);
            break;
        case AST_TacGetAddress_t:
            elim_dst_value_instr(ctx, node->get._TacGetAddress.dst, instr_idx);
            break;
        case AST_TacLoad_t:
            elim_dst_value_instr(ctx, node->get._TacLoad.dst, instr_idx);
            break;
        case AST_TacAddPtr_t:
            elim_dst_value_instr(ctx, node->get._TacAddPtr.dst, instr_idx);
            break;
        case AST_TacCopyToOffset_t:
            elim_dst_name_instr(ctx, node->get._TacCopyToOffset.dst_name, instr_idx);
            break;
        case AST_TacCopyFromOffset_t:
            elim_dst_value_instr(ctx, node->get._TacCopyFromOffset.dst, instr_idx);
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

    for (size_t block_id = 0; block_id < vec_size(ctx->cfg->blocks); ++block_id) {
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
    ctx->p_instrs = NULL;
}

static void optim_toplvl(Ctx ctx, TacTopLevel* node) {
    if (node->type == AST_TacFunction_t) {
        optim_fun_toplvl(ctx, &node->get._TacFunction);
    }
    else {
        THROW_ABORT;
    }
}

static void optim_program(Ctx ctx, TacProgram* node) {
    for (size_t i = 0; i < vec_size(node->fun_toplvls); ++i) {
        optim_toplvl(ctx, node->fun_toplvls[i]);
    }
    set_clear(ctx->frontend->addressed_set);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void optimize_three_address_code(TacProgram* node, FrontEndContext* frontend, uint8_t optim_1_mask) {
    OptimTacContext ctx;
    {
        ctx.frontend = frontend;
        ctx.is_fixed_point = true;

        ctx.enabled_optims[CONSTANT_FOLDING] = (optim_1_mask & (((uint8_t)1u) << 0)) > 0;
        ctx.enabled_optims[COPY_PROPAGATION] = (optim_1_mask & (((uint8_t)1u) << 1)) > 0;
        ctx.enabled_optims[UNREACHABLE_CODE_ELIMINATION] = (optim_1_mask & (((uint8_t)1u) << 2)) > 0;
        ctx.enabled_optims[DEAD_STORE_ELIMINATION] = (optim_1_mask & (((uint8_t)1u) << 3)) > 0;
        ctx.enabled_optims[CONTROL_FLOW_GRAPH] = (optim_1_mask & ~(((uint8_t)1u) << 0)) > 0;

        ctx.cfg = uptr_new();
        ctx.dfa = uptr_new();
        ctx.dfa_o1 = uptr_new();

        if (ctx.enabled_optims[CONTROL_FLOW_GRAPH]) {
            ctx.cfg = make_ControlFlowGraph();

            if (ctx.enabled_optims[COPY_PROPAGATION] || ctx.enabled_optims[DEAD_STORE_ELIMINATION]) {
                ctx.dfa = make_DataFlowAnalysis();
                ctx.dfa_o1 = make_DataFlowAnalysisO1();
            }
        }
    }
    optim_program(&ctx, node);

    free_ControlFlowGraph(&ctx.cfg);
    free_DataFlowAnalysis(&ctx.dfa);
    free_DataFlowAnalysisO1(&ctx.dfa_o1);
}
