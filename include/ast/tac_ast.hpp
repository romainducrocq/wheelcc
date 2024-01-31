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

/** TODO
cdef class TacReturn(TacInstruction):
    cdef public TacValue val
*/

/** TODO
cdef class TacSignExtend(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacTruncate(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacZeroExtend(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacDoubleToInt(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacDoubleToUInt(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacIntToDouble(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacUIntToDouble(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacFunCall(TacInstruction):
    cdef public TIdentifier name
    cdef public list[TacValue] args
    cdef public TacValue dst
*/

/** TODO
cdef class TacUnary(TacInstruction):
    cdef public TacUnaryOp unary_op
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacBinary(TacInstruction):
    cdef public TacBinaryOp binary_op
    cdef public TacValue src1
    cdef public TacValue src2
    cdef public TacValue dst
*/

/** TODO
cdef class TacCopy(TacInstruction):
    cdef public TacValue src
    cdef public TacValue dst
*/

/** TODO
cdef class TacJump(TacInstruction):
    cdef public TIdentifier target
*/

/** TODO
cdef class TacJumpIfZero(TacInstruction):
    cdef public TacValue condition
    cdef public TIdentifier target
*/

/** TODO
cdef class TacJumpIfNotZero(TacInstruction):
    cdef public TacValue condition
    cdef public TIdentifier target
*/

/** TODO
cdef class TacLabel(TacInstruction):
    cdef public TIdentifier name
*/

/** TODO
cdef class TacTopLevel(AST):
    pass
*/

/** TODO
cdef class TacFunction(TacTopLevel):
    cdef public TIdentifier name
    cdef public bint is_global
    cdef public list[TIdentifier] params
    cdef public list[TacInstruction] body
*/

/** TODO
cdef class TacStaticVariable(TacTopLevel):
    cdef public TIdentifier name
    cdef public bint is_global
    cdef public Type static_init_type
    cdef public StaticInit initial_value
*/

/** TODO
cdef class TacProgram(AST):
    cdef public list[TacTopLevel] top_levels
*/


#endif
