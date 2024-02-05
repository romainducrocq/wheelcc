#ifndef _AST_ASM_AST_HPP
#define _AST_ASM_AST_HPP

#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class AsmReg(AST):
    pass
*/

/** TODO
cdef class AsmAx(AsmReg):
    pass
*/

/** TODO
cdef class AsmCx(AsmReg):
    pass
*/

/** TODO
cdef class AsmDx(AsmReg):
    pass
*/

/** TODO
cdef class AsmDi(AsmReg):
    pass
*/

/** TODO
cdef class AsmSi(AsmReg):
    pass
*/

/** TODO
cdef class AsmR8(AsmReg):
    pass
*/

/** TODO
cdef class AsmR9(AsmReg):
    pass
*/

/** TODO
cdef class AsmR10(AsmReg):
    pass
*/

/** TODO
cdef class AsmR11(AsmReg):
    pass
*/

/** TODO
cdef class AsmSp(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM0(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM1(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM2(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM3(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM4(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM5(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM6(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM7(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM14(AsmReg):
    pass
*/

/** TODO
cdef class AsmXMM15(AsmReg):
    pass
*/

/** TODO
cdef class AsmCondCode(AST):
    pass
*/

/** TODO
cdef class AsmE(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmNE(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmG(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmGE(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmL(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmLE(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmA(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmAE(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmB(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmBE(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmP(AsmCondCode):
    pass
*/

/** TODO
cdef class AsmOperand(AST):
    pass
*/

/** TODO
cdef class AsmImm(AsmOperand):
    cdef public TIdentifier value
*/

/** TODO
cdef class AsmRegister(AsmOperand):
    cdef public AsmReg reg
*/

/** TODO
cdef class AsmPseudo(AsmOperand):
    cdef public TIdentifier name
*/

/** TODO
cdef class AsmStack(AsmOperand):
    cdef public TInt value
*/

/** TODO
cdef class AsmData(AsmOperand):
    cdef public TIdentifier name
*/

/** TODO
cdef class AsmBinaryOp(AST):
    pass
*/

/** TODO
cdef class AsmAdd(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmSub(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmMult(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmDivDouble(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmBitAnd(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmBitOr(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmBitXor(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmBitShiftLeft(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmBitShiftRight(AsmBinaryOp):
    pass
*/

/** TODO
cdef class AsmUnaryOp(AST):
    pass
*/

/** TODO
cdef class AsmNot(AsmUnaryOp):
    pass
*/

/** TODO
cdef class AsmNeg(AsmUnaryOp):
    pass
*/

/** TODO
cdef class AsmShr(AsmUnaryOp):
    pass
*/

/** TODO
cdef class AsmInstruction(AST):
    pass
*/

/** TODO
cdef class AsmMov(AsmInstruction):
    cdef public AssemblyType assembly_type
    cdef public AsmOperand src
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmMovSx(AsmInstruction):
    cdef public AsmOperand src
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmMovZeroExtend(AsmInstruction):
    cdef public AsmOperand src
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmCvttsd2si(AsmInstruction):
    cdef public AssemblyType assembly_type
    cdef public AsmOperand src
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmCvtsi2sd(AsmInstruction):
    cdef public AssemblyType assembly_type
    cdef public AsmOperand src
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmUnary(AsmInstruction):
    cdef public AsmUnaryOp unary_op
    cdef public AssemblyType assembly_type
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmBinary(AsmInstruction):
    cdef public AsmBinaryOp binary_op
    cdef public AssemblyType assembly_type
    cdef public AsmOperand src
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmCmp(AsmInstruction):
    cdef public AssemblyType assembly_type
    cdef public AsmOperand src
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmIdiv(AsmInstruction):
    cdef public AssemblyType assembly_type
    cdef public AsmOperand src
*/

/** TODO
cdef class AsmDiv(AsmInstruction):
    cdef public AssemblyType assembly_type
    cdef public AsmOperand src
*/

/** TODO
cdef class AsmCdq(AsmInstruction):
    cdef public AssemblyType assembly_type
*/

/** TODO
cdef class AsmJmp(AsmInstruction):
    cdef public TIdentifier target
*/

/** TODO
cdef class AsmJmpCC(AsmInstruction):
    cdef public AsmCondCode cond_code
    cdef public TIdentifier target
*/

/** TODO
cdef class AsmSetCC(AsmInstruction):
    cdef public AsmCondCode cond_code
    cdef public AsmOperand dst
*/

/** TODO
cdef class AsmLabel(AsmInstruction):
    cdef public TIdentifier name
*/

/** TODO
cdef class AsmPush(AsmInstruction):
    cdef public AsmOperand src
*/

/** TODO
cdef class AsmCall(AsmInstruction):
    cdef public TIdentifier name
*/

/** TODO
cdef class AsmRet(AsmInstruction):
    pass
*/

/** TODO
cdef class AsmTopLevel(AST):
    pass
*/

/** TODO
cdef class AsmFunction(AsmTopLevel):
    cdef public TIdentifier name
    cdef public bint is_global
    cdef public list[AsmInstruction] instructions
*/

/** TODO
cdef class AsmStaticVariable(AsmTopLevel):
    cdef public TIdentifier name
    cdef public bint is_global
    cdef public TInt alignment
    cdef public StaticInit initial_value
*/

/** TODO
cdef class AsmStaticConstant(AsmTopLevel):
    cdef public TIdentifier name
    cdef public TInt alignment
    cdef public StaticInit initial_value
*/

/** TODO
cdef class AsmProgram(AST):
    cdef public list[AsmTopLevel] top_levels
*/

#endif
