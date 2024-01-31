#ifndef _AST_TAC_AST_HPP
#define _AST_TAC_AST_HPP

/** TODO
cdef class TacUnaryOp(AST):
    pass
*/

/** TODO
cdef class TacComplement(TacUnaryOp):
    pass
*/

/** TODO
cdef class TacNegate(TacUnaryOp):
    pass
*/

/** TODO
cdef class TacNot(TacUnaryOp):
    pass
*/

/** TODO
cdef class TacBinaryOp(AST):
    pass
*/

/** TODO
cdef class TacAdd(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacSubtract(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacMultiply(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacDivide(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacRemainder(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacBitAnd(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacBitOr(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacBitXor(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacBitShiftLeft(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacBitShiftRight(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacEqual(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacNotEqual(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacLessThan(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacLessOrEqual(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacGreaterThan(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacGreaterOrEqual(TacBinaryOp):
    pass
*/

/** TODO
cdef class TacValue(AST):
    pass
*/

/** TODO
cdef class TacConstant(TacValue):
    cdef public CConst constant
*/

/** TODO
cdef class TacVariable(TacValue):
    cdef public TIdentifier name
*/

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
