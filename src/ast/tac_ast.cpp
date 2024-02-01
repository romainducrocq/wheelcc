#include "ast/tac_ast.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"

AST_T TacUnaryOp::type() { return AST_T::TacUnaryOp_t; }
AST_T TacComplement::type() { return AST_T::TacComplement_t; }
AST_T TacNegate::type() { return AST_T::TacNegate_t; }
AST_T TacNot::type() { return AST_T::TacNot_t; }
AST_T TacBinaryOp::type() { return AST_T::TacBinaryOp_t; }
AST_T TacAdd::type() { return AST_T::TacAdd_t; }
AST_T TacSubtract::type() { return AST_T::TacSubtract_t; }
AST_T TacMultiply::type() { return AST_T::TacMultiply_t; }
AST_T TacDivide::type() { return AST_T::TacDivide_t; }
AST_T TacRemainder::type() { return AST_T::TacRemainder_t; }
AST_T TacBitAnd::type() { return AST_T::TacBitAnd_t; }
AST_T TacBitOr::type() { return AST_T::TacBitOr_t; }
AST_T TacBitXor::type() { return AST_T::TacBitXor_t; }
AST_T TacBitShiftLeft::type() { return AST_T::TacBitShiftLeft_t; }
AST_T TacBitShiftRight::type() { return AST_T::TacBitShiftRight_t; }
AST_T TacEqual::type() { return AST_T::TacEqual_t; }
AST_T TacNotEqual::type() { return AST_T::TacNotEqual_t; }
AST_T TacLessThan::type() { return AST_T::TacLessThan_t; }
AST_T TacLessOrEqual::type() { return AST_T::TacLessOrEqual_t; }
AST_T TacGreaterThan::type() { return AST_T::TacGreaterThan_t; }
AST_T TacGreaterOrEqual::type() { return AST_T::TacGreaterOrEqual_t; }
AST_T TacValue::type() { return AST_T::TacValue_t; }
AST_T TacConstant::type() { return AST_T::TacConstant_t; }
AST_T TacVariable::type() { return AST_T::TacVariable_t; }
AST_T TacInstruction::type() { return AST_T::TacInstruction_t; }
AST_T TacReturn::type() { return AST_T::TacReturn_t; }
AST_T TacSignExtend::type() { return AST_T::TacSignExtend_t; }
AST_T TacTruncate::type() { return AST_T::TacTruncate_t; }
AST_T TacZeroExtend::type() { return AST_T::TacZeroExtend_t; }
AST_T TacDoubleToInt::type() { return AST_T::TacDoubleToInt_t; }
AST_T TacDoubleToUInt::type() { return AST_T::TacDoubleToUInt_t; }
AST_T TacIntToDouble::type() { return AST_T::TacIntToDouble_t; }
AST_T TacUIntToDouble::type() { return AST_T::TacUIntToDouble_t; }
AST_T TacFunCall::type() { return AST_T::TacFunCall_t; }
AST_T TacUnary::type() { return AST_T::TacUnary_t; }
AST_T TacBinary::type() { return AST_T::TacBinary_t; }
AST_T TacCopy::type() { return AST_T::TacCopy_t; }
AST_T TacJump::type() { return AST_T::TacJump_t; }
AST_T TacJumpIfZero::type() { return AST_T::TacJumpIfZero_t; }
AST_T TacJumpIfNotZero::type() { return AST_T::TacJumpIfNotZero_t; }
AST_T TacLabel::type() { return AST_T::TacLabel_t; }
AST_T TacTopLevel::type() { return AST_T::TacTopLevel_t; }
AST_T TacFunction::type() { return AST_T::TacFunction_t; }
AST_T TacStaticVariable::type() { return AST_T::TacStaticVariable_t; }
AST_T TacProgram::type() { return AST_T::TacProgram_t; }

TacConstant::TacConstant(std::shared_ptr<CConst> constant)
    : constant(std::move(constant)) {}

TacVariable::TacVariable(TIdentifier name)
    : name(std::move(name)) {}

TacReturn::TacReturn(std::unique_ptr<TacValue> val)
    : val(std::move(val)) {}

TacSignExtend::TacSignExtend(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacTruncate::TacTruncate(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacZeroExtend::TacZeroExtend(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacDoubleToInt::TacDoubleToInt(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacDoubleToUInt::TacDoubleToUInt(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacIntToDouble::TacIntToDouble(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacUIntToDouble::TacUIntToDouble(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacFunCall::TacFunCall(TIdentifier name, std::vector<std::unique_ptr<TacValue>> args,
                       std::unique_ptr<TacValue> dst)
    : name(std::move(name)), args(std::move(args)), dst(std::move(dst)) {}

TacUnary::TacUnary(std::unique_ptr<TacUnaryOp> unary_op, std::unique_ptr<TacValue> src,
                   std::unique_ptr<TacValue> dst)
    : unary_op(std::move(unary_op)), src(std::move(src)), dst(std::move(dst)) {}

TacBinary::TacBinary(std::unique_ptr<TacBinaryOp> binary_op, std::unique_ptr<TacValue> src1,
                     std::unique_ptr<TacValue> src2, std::unique_ptr<TacValue> dst)
    : binary_op(std::move(binary_op)), src1(std::move(src1)), src2(std::move(src2)),
      dst(std::move(dst)) {}

TacCopy::TacCopy(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst)
    : src(std::move(src)), dst(std::move(dst)) {}

TacJump::TacJump(TIdentifier target)
    : target(std::move(target)) {}

TacJumpIfZero::TacJumpIfZero(TIdentifier target, std::unique_ptr<TacValue> condition)
    : target(std::move(target)), condition(std::move(condition)) {}

TacJumpIfNotZero::TacJumpIfNotZero(TIdentifier target, std::unique_ptr<TacValue> condition)
    : target(std::move(target)), condition(std::move(condition)) {}

TacLabel::TacLabel(TIdentifier name)
    : name(std::move(name)) {}

TacFunction::TacFunction(TIdentifier name, bool is_global, std::vector<TIdentifier> params,
                         std::vector<std::unique_ptr<TacInstruction>> body)
    : name(std::move(name)), is_global(is_global), params(std::move(params)), body(std::move(body)) {}

TacStaticVariable::TacStaticVariable(TIdentifier name, bool is_global, std::shared_ptr<Type> static_init_type,
                                     std::shared_ptr<StaticInit> initial_value)
    : name(std::move(name)), is_global(is_global), static_init_type(std::move(static_init_type)),
      initial_value(std::move(initial_value)) {}

TacProgram::TacProgram(std::vector<std::unique_ptr<TacTopLevel>> top_levels)
    : top_levels(std::move(top_levels)) {}
