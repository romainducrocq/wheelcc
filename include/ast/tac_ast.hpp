#ifndef _AST_TAC_AST_HPP
#define _AST_TAC_AST_HPP

#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class TacUnaryOp(AST):
    pass
*/
struct TacUnaryOp : Ast {
    AST_T type() override;
};

/** TODO
cdef class TacComplement(TacUnaryOp):
    pass
*/
struct TacComplement : TacUnaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacNegate(TacUnaryOp):
    pass
*/
struct TacNegate : TacUnaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacNot(TacUnaryOp):
    pass
*/
struct TacNot : TacUnaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class TacBinaryOp(AST):
    pass
*/
struct TacBinaryOp : Ast {
    AST_T type() override;
};

/** TODO
cdef class TacAdd(TacBinaryOp):
    pass
*/
struct TacAdd : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacSubtract(TacBinaryOp):
    pass
*/
struct TacSubtract : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacMultiply(TacBinaryOp):
    pass
*/
struct TacMultiply : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacDivide(TacBinaryOp):
    pass
*/
struct TacDivide : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacRemainder(TacBinaryOp):
    pass
*/
struct TacRemainder : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacBitAnd(TacBinaryOp):
    pass
*/
struct TacBitAnd : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacBitOr(TacBinaryOp):
    pass
*/
struct TacBitOr : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacBitXor(TacBinaryOp):
    pass
*/
struct TacBitXor : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacBitShiftLeft(TacBinaryOp):
    pass
*/
struct TacBitShiftLeft : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacBitShiftRight(TacBinaryOp):
    pass
*/
struct TacBitShiftRight : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacEqual(TacBinaryOp):
    pass
*/
struct TacEqual : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacNotEqual(TacBinaryOp):
    pass
*/
struct TacNotEqual : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacLessThan(TacBinaryOp):
    pass
*/
struct TacLessThan : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacLessOrEqual(TacBinaryOp):
    pass
*/
struct TacLessOrEqual : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacGreaterThan(TacBinaryOp):
    pass
*/
struct TacGreaterThan : TacBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class TacGreaterOrEqual(TacBinaryOp):
    pass
*/
struct TacGreaterOrEqual : TacBinaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class TacValue(AST):
    pass
*/
struct TacValue : Ast {
    AST_T type() override;
};

/** TODO
cdef class TacConstant(TacValue):
    cdef public CConst constant
*/
struct TacConstant : TacValue {
    AST_T type() override;
    TacConstant() = default;
    TacConstant(std::shared_ptr<CConst> constant);

    std::shared_ptr<CConst> constant;
};

/** TODO
cdef class TacVariable(TacValue):
    cdef public TIdentifier name
*/
struct TacVariable : TacValue {
    AST_T type() override;
    TacVariable() = default;
    TacVariable(TIdentifier name);

    TIdentifier name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class TacInstruction(AST):
    pass
*/
struct TacInstruction : Ast {
    AST_T type() override;
};

/** TODO
cdef class TacReturn(TacInstruction):
    cdef public TacValue val
*/
struct TacReturn : TacInstruction {
    AST_T type() override;
    TacReturn() = default;
    TacReturn(std::unique_ptr<TacValue> val);

    std::unique_ptr<TacValue> val;
};

/** TODO
cdef class TacSignExtend(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacSignExtend : TacInstruction {
    AST_T type() override;
    TacSignExtend() = default;
    TacSignExtend(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacTruncate(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacTruncate : TacInstruction {
    AST_T type() override;
    TacTruncate() = default;
    TacTruncate(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacZeroExtend(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacZeroExtend : TacInstruction {
    AST_T type() override;
    TacZeroExtend() = default;
    TacZeroExtend(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacDoubleToInt(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacDoubleToInt : TacInstruction {
    AST_T type() override;
    TacDoubleToInt() = default;
    TacDoubleToInt(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacDoubleToUInt(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacDoubleToUInt : TacInstruction {
    AST_T type() override;
    TacDoubleToUInt() = default;
    TacDoubleToUInt(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacIntToDouble(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacIntToDouble : TacInstruction {
    AST_T type() override;
    TacIntToDouble() = default;
    TacIntToDouble(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacUIntToDouble(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacUIntToDouble : TacInstruction {
    AST_T type() override;
    TacUIntToDouble() = default;
    TacUIntToDouble(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacFunCall(TacInstruction):
    cdef public TIdentifier name
    cdef public list[TacValue] args
    cdef public TacValue dst
*/
struct TacFunCall : TacInstruction {
    AST_T type() override;
    TacFunCall() = default;
    TacFunCall(TIdentifier name, std::vector<std::unique_ptr<TacValue>> args,
               std::unique_ptr<TacValue> dst);

    TIdentifier name;
    std::vector<std::unique_ptr<TacValue>> args;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacUnary(TacInstruction):
    cdef public TacUnaryOp unary_op
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacUnary : TacInstruction {
    AST_T type() override;
    TacUnary() = default;
    TacUnary(std::unique_ptr<TacUnaryOp> unary_op, std::unique_ptr<TacValue> src,
             std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacUnaryOp> unary_op;
    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacBinary(TacInstruction):
    cdef public TacBinaryOp binary_op
    cdef public TacValue src1
    cdef public TacValue src2
    cdef public TacValue dst
*/
struct TacBinary : TacInstruction {
    AST_T type() override;
    TacBinary() = default;
    TacBinary(std::unique_ptr<TacBinaryOp> binary_op, std::unique_ptr<TacValue> src1,
              std::unique_ptr<TacValue> src2, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacBinaryOp> binary_op;
    std::unique_ptr<TacValue> src1;
    std::unique_ptr<TacValue> src2;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacCopy(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/
struct TacCopy : TacInstruction {
    AST_T type() override;
    TacCopy() = default;
    TacCopy(std::unique_ptr<TacValue> src, std::unique_ptr<TacValue> dst);

    std::unique_ptr<TacValue> src;
    std::unique_ptr<TacValue> dst;
};

/** TODO
cdef class TacJump(TacInstruction):
    cdef public TIdentifier target
*/
struct TacJump : TacInstruction {
    AST_T type() override;
    TacJump() = default;
    TacJump(TIdentifier target);

    TIdentifier target;
};

/** TODO
cdef class TacJumpIfZero(TacInstruction):
    cdef public TacValue condition
    cdef public TIdentifier target
*/
struct TacJumpIfZero : TacInstruction {
    AST_T type() override;
    TacJumpIfZero() = default;
    TacJumpIfZero(TIdentifier target, std::unique_ptr<TacValue> condition);

    TIdentifier target;
    std::unique_ptr<TacValue> condition;
};

/** TODO
cdef class TacJumpIfNotZero(TacInstruction):
    cdef public TacValue condition
    cdef public TIdentifier target
*/
struct TacJumpIfNotZero : TacInstruction {
    AST_T type() override;
    TacJumpIfNotZero() = default;
    TacJumpIfNotZero(TIdentifier target, std::unique_ptr<TacValue> condition);

    TIdentifier target;
    std::unique_ptr<TacValue> condition;
};

/** TODO
cdef class TacLabel(TacInstruction):
    cdef public TIdentifier name
*/
struct TacLabel : TacInstruction {
    AST_T type() override;
    TacLabel() = default;
    TacLabel(TIdentifier name);

    TIdentifier name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class TacTopLevel(AST):
    pass
*/
struct TacTopLevel : Ast {
    AST_T type() override;
};

/** TODO
cdef class TacFunction(TacTopLevel):
    cdef public TIdentifier name
    cdef public bint is_global
    cdef public list[TIdentifier] params
    cdef public list[TacInstruction] body
*/
struct TacFunction : TacTopLevel {
    AST_T type() override;
    TacFunction() = default;
    TacFunction(TIdentifier name, bool is_global, std::vector<TIdentifier> params,
                std::vector<std::unique_ptr<TacInstruction>> body);

    TIdentifier name;
    bool is_global;
    std::vector<TIdentifier> params;
    std::vector<std::unique_ptr<TacInstruction>> body;
};

/** TODO
cdef class TacStaticVariable(TacTopLevel):
    cdef public TIdentifier name
    cdef public bint is_global
    cdef public Type static_init_type
    cdef public StaticInit initial_value
*/
struct TacStaticVariable : TacTopLevel {
    AST_T type() override;
    TacStaticVariable() = default;
    TacStaticVariable(TIdentifier name, bool is_global, std::shared_ptr<Type> static_init_type,
                      std::shared_ptr<StaticInit> initial_value);

    TIdentifier name;
    bool is_global;
    std::shared_ptr<Type> static_init_type;
    std::shared_ptr<StaticInit> initial_value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class TacProgram(AST):
    cdef public list[TacTopLevel] top_levels
*/
struct TacProgram : Ast {
    AST_T type() override;
    TacProgram() = default;
    TacProgram(std::vector<std::unique_ptr<TacTopLevel>> top_levels);

    std::vector<std::unique_ptr<TacTopLevel>> top_levels;
};

#endif
