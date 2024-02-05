#ifndef _AST_ASM_AST_HPP
#define _AST_ASM_AST_HPP

#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class AsmReg(AST):
    pass
*/
struct AsmReg : Ast {
    AST_T type() override;
};

/** TODO
cdef class AsmAx(AsmReg):
    pass
*/
struct AsmAx : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmCx(AsmReg):
    pass
*/
struct AsmCx : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmDx(AsmReg):
    pass
*/
struct AsmDx : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmDi(AsmReg):
    pass
*/
struct AsmDi : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmSi(AsmReg):
    pass
*/
struct AsmSi : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmR8(AsmReg):
    pass
*/
struct AsmR8 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmR9(AsmReg):
    pass
*/
struct AsmR9 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmR10(AsmReg):
    pass
*/
struct AsmR10 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmR11(AsmReg):
    pass
*/
struct AsmR11 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmSp(AsmReg):
    pass
*/
struct AsmSp : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM0(AsmReg):
    pass
*/
struct AsmXMM0 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM1(AsmReg):
    pass
*/
struct AsmXMM1 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM2(AsmReg):
    pass
*/
struct AsmXMM2 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM3(AsmReg):
    pass
*/
struct AsmXMM3 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM4(AsmReg):
    pass
*/
struct AsmXMM4 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM5(AsmReg):
    pass
*/
struct AsmXMM5 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM6(AsmReg):
    pass
*/
struct AsmXMM6 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM7(AsmReg):
    pass
*/
struct AsmXMM7 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM14(AsmReg):
    pass
*/
struct AsmXMM14 : AsmReg {
    AST_T type() override;
};

/** TODO
cdef class AsmXMM15(AsmReg):
    pass
*/
struct AsmXMM15 : AsmReg {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class AsmCondCode(AST):
    pass
*/
struct AsmCondCode : Ast {
    AST_T type() override;
};

/** TODO
cdef class AsmE(AsmCondCode):
    pass
*/
struct AsmE : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmNE(AsmCondCode):
    pass
*/
struct AsmNE : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmG(AsmCondCode):
    pass
*/
struct AsmG : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmGE(AsmCondCode):
    pass
*/
struct AsmGE : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmL(AsmCondCode):
    pass
*/
struct AsmL : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmLE(AsmCondCode):
    pass
*/
struct AsmLE : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmA(AsmCondCode):
    pass
*/
struct AsmA : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmAE(AsmCondCode):
    pass
*/
struct AsmAE : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmB(AsmCondCode):
    pass
*/
struct AsmB : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmBE(AsmCondCode):
    pass
*/
struct AsmBE : AsmCondCode {
    AST_T type() override;
};

/** TODO
cdef class AsmP(AsmCondCode):
    pass
*/
struct AsmP : AsmCondCode {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class AsmOperand(AST):
    pass
*/
struct AsmOperand : Ast {
    AST_T type() override;
};

/** TODO
cdef class AsmImm(AsmOperand):
    cdef public TIdentifier value
*/
struct AsmImm : AsmOperand {
    AST_T type() override;
    AsmImm() = default;
    AsmImm(TIdentifier value);

    TIdentifier value;
};

/** TODO
cdef class AsmRegister(AsmOperand):
    cdef public AsmReg reg
*/
struct AsmRegister : AsmOperand {
    AST_T type() override;
    AsmRegister() = default;
    AsmRegister(std::unique_ptr<AsmReg> reg);

    std::unique_ptr<AsmReg> reg;
};

/** TODO
cdef class AsmPseudo(AsmOperand):
    cdef public TIdentifier name
*/
struct AsmPseudo : AsmOperand {
    AST_T type() override;
    AsmPseudo() = default;
    AsmPseudo(TIdentifier name);

    TIdentifier name;
};

/** TODO
cdef class AsmStack(AsmOperand):
    cdef public TInt value
*/
struct AsmStack : AsmOperand {
    AST_T type() override;
    AsmStack() = default;
    AsmStack(TInt value);

    TInt value;
};

/** TODO
cdef class AsmData(AsmOperand):
    cdef public TIdentifier name
*/
struct AsmData : AsmOperand {
    AST_T type() override;
    AsmData() = default;
    AsmData(TIdentifier name);

    TIdentifier name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class AsmBinaryOp(AST):
    pass
*/
struct AsmBinaryOp : Ast {
    AST_T type() override;
};

/** TODO
cdef class AsmAdd(AsmBinaryOp):
    pass
*/
struct AsmAdd : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmSub(AsmBinaryOp):
    pass
*/
struct AsmSub : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmMult(AsmBinaryOp):
    pass
*/
struct AsmMult : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmDivDouble(AsmBinaryOp):
    pass
*/
struct AsmDivDouble : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmBitAnd(AsmBinaryOp):
    pass
*/
struct AsmBitAnd : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmBitOr(AsmBinaryOp):
    pass
*/
struct AsmBitOr : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmBitXor(AsmBinaryOp):
    pass
*/
struct AsmBitXor : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmBitShiftLeft(AsmBinaryOp):
    pass
*/
struct AsmBitShiftLeft : AsmBinaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmBitShiftRight(AsmBinaryOp):
    pass
*/
struct AsmBitShiftRight : AsmBinaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** TODO
cdef class AsmUnaryOp(AST):
    pass
*/
struct AsmUnaryOp : Ast {
    AST_T type() override;
};

/** TODO
cdef class AsmNot(AsmUnaryOp):
    pass
*/
struct AsmNot : AsmUnaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmNeg(AsmUnaryOp):
    pass
*/
struct AsmNeg : AsmUnaryOp {
    AST_T type() override;
};

/** TODO
cdef class AsmShr(AsmUnaryOp):
    pass
*/
struct AsmShr : AsmUnaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
