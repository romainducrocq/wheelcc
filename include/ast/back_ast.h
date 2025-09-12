#ifndef _AST_BACK_AST_H
#define _AST_BACK_AST_H

#include "util/c_std.h"

#include "ast/ast.h"
#include "ast/back_symt.h"
#include "ast/front_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct AsmReg AsmReg;
typedef struct AsmCondCode AsmCondCode;
typedef struct AsmOperand AsmOperand;
typedef struct AsmBinaryOp AsmBinaryOp;
typedef struct AsmUnaryOp AsmUnaryOp;
typedef struct AsmInstruction AsmInstruction;
typedef struct AsmTopLevel AsmTopLevel;
typedef struct AsmProgram AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// reg = AX
//     | BX
//     | CX
//     | DX
//     | DI
//     | SI
//     | R8
//     | R9
//     | R10
//     | R11
//     | R12
//     | R13
//     | R14
//     | R15
//     | SP
//     | BP
//     | XMM0
//     | XMM1
//     | XMM2
//     | XMM3
//     | XMM4
//     | XMM5
//     | XMM6
//     | XMM7
//     | XMM8
//     | XMM9
//     | XMM10
//     | XMM11
//     | XMM12
//     | XMM13
//     | XMM14
//     | XMM15

typedef struct AsmAx {
    int8_t _empty;
} AsmAx;

typedef struct AsmBx {
    int8_t _empty;
} AsmBx;

typedef struct AsmCx {
    int8_t _empty;
} AsmCx;

typedef struct AsmDx {
    int8_t _empty;
} AsmDx;

typedef struct AsmDi {
    int8_t _empty;
} AsmDi;

typedef struct AsmSi {
    int8_t _empty;
} AsmSi;

typedef struct AsmR8 {
    int8_t _empty;
} AsmR8;

typedef struct AsmR9 {
    int8_t _empty;
} AsmR9;

typedef struct AsmR10 {
    int8_t _empty;
} AsmR10;

typedef struct AsmR11 {
    int8_t _empty;
} AsmR11;

typedef struct AsmR12 {
    int8_t _empty;
} AsmR12;

typedef struct AsmR13 {
    int8_t _empty;
} AsmR13;

typedef struct AsmR14 {
    int8_t _empty;
} AsmR14;

typedef struct AsmR15 {
    int8_t _empty;
} AsmR15;

typedef struct AsmSp {
    int8_t _empty;
} AsmSp;

typedef struct AsmBp {
    int8_t _empty;
} AsmBp;

typedef struct AsmXMM0 {
    int8_t _empty;
} AsmXMM0;

typedef struct AsmXMM1 {
    int8_t _empty;
} AsmXMM1;

typedef struct AsmXMM2 {
    int8_t _empty;
} AsmXMM2;

typedef struct AsmXMM3 {
    int8_t _empty;
} AsmXMM3;

typedef struct AsmXMM4 {
    int8_t _empty;
} AsmXMM4;

typedef struct AsmXMM5 {
    int8_t _empty;
} AsmXMM5;

typedef struct AsmXMM6 {
    int8_t _empty;
} AsmXMM6;

typedef struct AsmXMM7 {
    int8_t _empty;
} AsmXMM7;

typedef struct AsmXMM8 {
    int8_t _empty;
} AsmXMM8;

typedef struct AsmXMM9 {
    int8_t _empty;
} AsmXMM9;

typedef struct AsmXMM10 {
    int8_t _empty;
} AsmXMM10;

typedef struct AsmXMM11 {
    int8_t _empty;
} AsmXMM11;

typedef struct AsmXMM12 {
    int8_t _empty;
} AsmXMM12;

typedef struct AsmXMM13 {
    int8_t _empty;
} AsmXMM13;

typedef struct AsmXMM14 {
    int8_t _empty;
} AsmXMM14;

typedef struct AsmXMM15 {
    int8_t _empty;
} AsmXMM15;

typedef struct AsmReg {
    unique_ptr_impl(AST_T);

    union {
        AsmAx _AsmAx;
        AsmBx _AsmBx;
        AsmCx _AsmCx;
        AsmDx _AsmDx;
        AsmDi _AsmDi;
        AsmSi _AsmSi;
        AsmR8 _AsmR8;
        AsmR9 _AsmR9;
        AsmR10 _AsmR10;
        AsmR11 _AsmR11;
        AsmR12 _AsmR12;
        AsmR13 _AsmR13;
        AsmR14 _AsmR14;
        AsmR15 _AsmR15;
        AsmSp _AsmSp;
        AsmBp _AsmBp;
        AsmXMM0 _AsmXMM0;
        AsmXMM1 _AsmXMM1;
        AsmXMM2 _AsmXMM2;
        AsmXMM3 _AsmXMM3;
        AsmXMM4 _AsmXMM4;
        AsmXMM5 _AsmXMM5;
        AsmXMM6 _AsmXMM6;
        AsmXMM7 _AsmXMM7;
        AsmXMM8 _AsmXMM8;
        AsmXMM9 _AsmXMM9;
        AsmXMM10 _AsmXMM10;
        AsmXMM11 _AsmXMM11;
        AsmXMM12 _AsmXMM12;
        AsmXMM13 _AsmXMM13;
        AsmXMM14 _AsmXMM14;
        AsmXMM15 _AsmXMM15;
    } get;
} AsmReg;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmReg) make_AsmReg(void);
unique_ptr_t(AsmReg) make_AsmAx(void);
unique_ptr_t(AsmReg) make_AsmBx(void);
unique_ptr_t(AsmReg) make_AsmCx(void);
unique_ptr_t(AsmReg) make_AsmDx(void);
unique_ptr_t(AsmReg) make_AsmDi(void);
unique_ptr_t(AsmReg) make_AsmSi(void);
unique_ptr_t(AsmReg) make_AsmR8(void);
unique_ptr_t(AsmReg) make_AsmR9(void);
unique_ptr_t(AsmReg) make_AsmR10(void);
unique_ptr_t(AsmReg) make_AsmR11(void);
unique_ptr_t(AsmReg) make_AsmR12(void);
unique_ptr_t(AsmReg) make_AsmR13(void);
unique_ptr_t(AsmReg) make_AsmR14(void);
unique_ptr_t(AsmReg) make_AsmR15(void);
unique_ptr_t(AsmReg) make_AsmSp(void);
unique_ptr_t(AsmReg) make_AsmBp(void);
unique_ptr_t(AsmReg) make_AsmXMM0(void);
unique_ptr_t(AsmReg) make_AsmXMM1(void);
unique_ptr_t(AsmReg) make_AsmXMM2(void);
unique_ptr_t(AsmReg) make_AsmXMM3(void);
unique_ptr_t(AsmReg) make_AsmXMM4(void);
unique_ptr_t(AsmReg) make_AsmXMM5(void);
unique_ptr_t(AsmReg) make_AsmXMM6(void);
unique_ptr_t(AsmReg) make_AsmXMM7(void);
unique_ptr_t(AsmReg) make_AsmXMM8(void);
unique_ptr_t(AsmReg) make_AsmXMM9(void);
unique_ptr_t(AsmReg) make_AsmXMM10(void);
unique_ptr_t(AsmReg) make_AsmXMM11(void);
unique_ptr_t(AsmReg) make_AsmXMM12(void);
unique_ptr_t(AsmReg) make_AsmXMM13(void);
unique_ptr_t(AsmReg) make_AsmXMM14(void);
unique_ptr_t(AsmReg) make_AsmXMM15(void);
void free_AsmReg(unique_ptr_t(AsmReg) * self);
#ifdef __cplusplus
}
#endif
#define init_AsmReg() tagged_def_init(AST, AsmReg)
#define init_AsmAx() tagged_def_init(AST, AsmAx)
#define init_AsmBx() tagged_def_init(AST, AsmBx)
#define init_AsmCx() tagged_def_init(AST, AsmCx)
#define init_AsmDx() tagged_def_init(AST, AsmDx)
#define init_AsmDi() tagged_def_init(AST, AsmDi)
#define init_AsmSi() tagged_def_init(AST, AsmSi)
#define init_AsmR8() tagged_def_init(AST, AsmR8)
#define init_AsmR9() tagged_def_init(AST, AsmR9)
#define init_AsmR10() tagged_def_init(AST, AsmR10)
#define init_AsmR11() tagged_def_init(AST, AsmR11)
#define init_AsmR12() tagged_def_init(AST, AsmR12)
#define init_AsmR13() tagged_def_init(AST, AsmR13)
#define init_AsmR14() tagged_def_init(AST, AsmR14)
#define init_AsmR15() tagged_def_init(AST, AsmR15)
#define init_AsmSp() tagged_def_init(AST, AsmSp)
#define init_AsmBp() tagged_def_init(AST, AsmBp)
#define init_AsmXMM0() tagged_def_init(AST, AsmXMM0)
#define init_AsmXMM1() tagged_def_init(AST, AsmXMM1)
#define init_AsmXMM2() tagged_def_init(AST, AsmXMM2)
#define init_AsmXMM3() tagged_def_init(AST, AsmXMM3)
#define init_AsmXMM4() tagged_def_init(AST, AsmXMM4)
#define init_AsmXMM5() tagged_def_init(AST, AsmXMM5)
#define init_AsmXMM6() tagged_def_init(AST, AsmXMM6)
#define init_AsmXMM7() tagged_def_init(AST, AsmXMM7)
#define init_AsmXMM8() tagged_def_init(AST, AsmXMM8)
#define init_AsmXMM9() tagged_def_init(AST, AsmXMM9)
#define init_AsmXMM10() tagged_def_init(AST, AsmXMM10)
#define init_AsmXMM11() tagged_def_init(AST, AsmXMM11)
#define init_AsmXMM12() tagged_def_init(AST, AsmXMM12)
#define init_AsmXMM13() tagged_def_init(AST, AsmXMM13)
#define init_AsmXMM14() tagged_def_init(AST, AsmXMM14)
#define init_AsmXMM15() tagged_def_init(AST, AsmXMM15)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// cond_code = E
//           | NE
//           | G
//           | GE
//           | L
//           | LE
//           | A
//           | AE
//           | B
//           | BE
//           | P

typedef struct AsmE {
    int8_t _empty;
} AsmE;

typedef struct AsmNE {
    int8_t _empty;
} AsmNE;

typedef struct AsmG {
    int8_t _empty;
} AsmG;

typedef struct AsmGE {
    int8_t _empty;
} AsmGE;

typedef struct AsmL {
    int8_t _empty;
} AsmL;

typedef struct AsmLE {
    int8_t _empty;
} AsmLE;

typedef struct AsmA {
    int8_t _empty;
} AsmA;

typedef struct AsmAE {
    int8_t _empty;
} AsmAE;

typedef struct AsmB {
    int8_t _empty;
} AsmB;

typedef struct AsmBE {
    int8_t _empty;
} AsmBE;

typedef struct AsmP {
    int8_t _empty;
} AsmP;

typedef struct AsmCondCode {
    unique_ptr_impl(AST_T);

    union {
        AsmE _AsmE;
        AsmNE _AsmNE;
        AsmG _AsmG;
        AsmGE _AsmGE;
        AsmL _AsmL;
        AsmLE _AsmLE;
        AsmA _AsmA;
        AsmAE _AsmAE;
        AsmB _AsmB;
        AsmBE _AsmBE;
        AsmP _AsmP;
    } get;
} AsmCondCode;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmCondCode) make_AsmCondCode(void);
unique_ptr_t(AsmCondCode) make_AsmE(void);
unique_ptr_t(AsmCondCode) make_AsmNE(void);
unique_ptr_t(AsmCondCode) make_AsmG(void);
unique_ptr_t(AsmCondCode) make_AsmGE(void);
unique_ptr_t(AsmCondCode) make_AsmL(void);
unique_ptr_t(AsmCondCode) make_AsmLE(void);
unique_ptr_t(AsmCondCode) make_AsmA(void);
unique_ptr_t(AsmCondCode) make_AsmAE(void);
unique_ptr_t(AsmCondCode) make_AsmB(void);
unique_ptr_t(AsmCondCode) make_AsmBE(void);
unique_ptr_t(AsmCondCode) make_AsmP(void);
void free_AsmCondCode(unique_ptr_t(AsmCondCode) * self);
#ifdef __cplusplus
}
#endif
#define init_AsmCondCode() tagged_def_init(AST, AsmCondCode)
#define init_AsmE() tagged_def_init(AST, AsmE)
#define init_AsmNE() tagged_def_init(AST, AsmNE)
#define init_AsmG() tagged_def_init(AST, AsmG)
#define init_AsmGE() tagged_def_init(AST, AsmGE)
#define init_AsmL() tagged_def_init(AST, AsmL)
#define init_AsmLE() tagged_def_init(AST, AsmLE)
#define init_AsmA() tagged_def_init(AST, AsmA)
#define init_AsmAE() tagged_def_init(AST, AsmAE)
#define init_AsmB() tagged_def_init(AST, AsmB)
#define init_AsmBE() tagged_def_init(AST, AsmBE)
#define init_AsmP() tagged_def_init(AST, AsmP)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// operand = Imm(int, bool, bool, bool)
//         | Reg(reg)
//         | Pseudo(identifier)
//         | Memory(int, reg)
//         | Data(identifier, int)
//         | PseudoMem(identifier, int)
//         | Indexed(int, reg, reg)

typedef struct AsmImm {
    TULong value;
    bool is_byte;
    bool is_quad;
    bool is_neg;
} AsmImm;

typedef struct AsmRegister {
    unique_ptr_t(AsmReg) reg;
} AsmRegister;

typedef struct AsmPseudo {
    TIdentifier name;
} AsmPseudo;

typedef struct AsmMemory {
    TLong value;
    unique_ptr_t(AsmReg) reg;
} AsmMemory;

typedef struct AsmData {
    TIdentifier name;
    TLong offset;
} AsmData;

typedef struct AsmPseudoMem {
    TIdentifier name;
    TLong offset;
} AsmPseudoMem;

typedef struct AsmIndexed {
    TLong scale;
    unique_ptr_t(AsmReg) reg_base;
    unique_ptr_t(AsmReg) reg_index;
} AsmIndexed;

typedef struct AsmOperand {
    shared_ptr_impl(AST_T);

    union {
        AsmImm _AsmImm;
        AsmRegister _AsmRegister;
        AsmPseudo _AsmPseudo;
        AsmMemory _AsmMemory;
        AsmData _AsmData;
        AsmPseudoMem _AsmPseudoMem;
        AsmIndexed _AsmIndexed;
    } get;
} AsmOperand;

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(AsmOperand) make_AsmOperand(void);
shared_ptr_t(AsmOperand) make_AsmImm(TULong value, bool is_byte, bool is_quad, bool is_neg);
shared_ptr_t(AsmOperand) make_AsmRegister(unique_ptr_t(AsmReg) * reg);
shared_ptr_t(AsmOperand) make_AsmPseudo(TIdentifier name);
shared_ptr_t(AsmOperand) make_AsmMemory(TLong value, unique_ptr_t(AsmReg) * reg);
shared_ptr_t(AsmOperand) make_AsmData(TIdentifier name, TLong offset);
shared_ptr_t(AsmOperand) make_AsmPseudoMem(TIdentifier name, TLong offset);
shared_ptr_t(AsmOperand)
    make_AsmIndexed(TLong scale, unique_ptr_t(AsmReg) * reg_base, unique_ptr_t(AsmReg) * reg_index);
void free_AsmOperand(shared_ptr_t(AsmOperand) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// binary_operator = Add
//                 | Sub
//                 | Mult
//                 | DivDouble
//                 | BitAnd
//                 | BitOr
//                 | BitXor
//                 | BitShiftLeft
//                 | BitShiftRight
//                 | BitShrArithmetic

typedef struct AsmAdd {
    int8_t _empty;
} AsmAdd;

typedef struct AsmSub {
    int8_t _empty;
} AsmSub;

typedef struct AsmMult {
    int8_t _empty;
} AsmMult;

typedef struct AsmDivDouble {
    int8_t _empty;
} AsmDivDouble;

typedef struct AsmBitAnd {
    int8_t _empty;
} AsmBitAnd;

typedef struct AsmBitOr {
    int8_t _empty;
} AsmBitOr;

typedef struct AsmBitXor {
    int8_t _empty;
} AsmBitXor;

typedef struct AsmBitShiftLeft {
    int8_t _empty;
} AsmBitShiftLeft;

typedef struct AsmBitShiftRight {
    int8_t _empty;
} AsmBitShiftRight;

typedef struct AsmBitShrArithmetic {
    int8_t _empty;
} AsmBitShrArithmetic;

typedef struct AsmBinaryOp {
    unique_ptr_impl(AST_T);

    union {
        AsmAdd _AsmAdd;
        AsmSub _AsmSub;
        AsmMult _AsmMult;
        AsmDivDouble _AsmDivDouble;
        AsmBitAnd _AsmBitAnd;
        AsmBitOr _AsmBitOr;
        AsmBitXor _AsmBitXor;
        AsmBitShiftLeft _AsmBitShiftLeft;
        AsmBitShiftRight _AsmBitShiftRight;
        AsmBitShrArithmetic _AsmBitShrArithmetic;
    } get;
} AsmBinaryOp;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmBinaryOp) make_AsmBinaryOp(void);
unique_ptr_t(AsmBinaryOp) make_AsmAdd(void);
unique_ptr_t(AsmBinaryOp) make_AsmSub(void);
unique_ptr_t(AsmBinaryOp) make_AsmMult(void);
unique_ptr_t(AsmBinaryOp) make_AsmDivDouble(void);
unique_ptr_t(AsmBinaryOp) make_AsmBitAnd(void);
unique_ptr_t(AsmBinaryOp) make_AsmBitOr(void);
unique_ptr_t(AsmBinaryOp) make_AsmBitXor(void);
unique_ptr_t(AsmBinaryOp) make_AsmBitShiftLeft(void);
unique_ptr_t(AsmBinaryOp) make_AsmBitShiftRight(void);
unique_ptr_t(AsmBinaryOp) make_AsmBitShrArithmetic(void);
void free_AsmBinaryOp(unique_ptr_t(AsmBinaryOp) * self);
#ifdef __cplusplus
}
#endif
#define init_AsmBinaryOp() tagged_def_init(AST, AsmBinaryOp)
#define init_AsmAdd() tagged_def_init(AST, AsmAdd)
#define init_AsmSub() tagged_def_init(AST, AsmSub)
#define init_AsmMult() tagged_def_init(AST, AsmMult)
#define init_AsmDivDouble() tagged_def_init(AST, AsmDivDouble)
#define init_AsmBitAnd() tagged_def_init(AST, AsmBitAnd)
#define init_AsmBitOr() tagged_def_init(AST, AsmBitOr)
#define init_AsmBitXor() tagged_def_init(AST, AsmBitXor)
#define init_AsmBitShiftLeft() tagged_def_init(AST, AsmBitShiftLeft)
#define init_AsmBitShiftRight() tagged_def_init(AST, AsmBitShiftRight)
#define init_AsmBitShrArithmetic() tagged_def_init(AST, AsmBitShrArithmetic)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Not
//                | Neg
//                | Shr

typedef struct AsmNot {
    int8_t _empty;
} AsmNot;

typedef struct AsmNeg {
    int8_t _empty;
} AsmNeg;

typedef struct AsmShr {
    int8_t _empty;
} AsmShr;

typedef struct AsmUnaryOp {
    unique_ptr_impl(AST_T);

    union {
        AsmNot _AsmNot;
        AsmNeg _AsmNeg;
        AsmShr _AsmShr;
    } get;
} AsmUnaryOp;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmUnaryOp) make_AsmUnaryOp(void);
unique_ptr_t(AsmUnaryOp) make_AsmNot(void);
unique_ptr_t(AsmUnaryOp) make_AsmNeg(void);
unique_ptr_t(AsmUnaryOp) make_AsmShr(void);
void free_AsmUnaryOp(unique_ptr_t(AsmUnaryOp) * self);
#ifdef __cplusplus
}
#endif
#define init_AsmUnaryOp() tagged_def_init(AST, AsmUnaryOp)
#define init_AsmNot() tagged_def_init(AST, AsmNot)
#define init_AsmNeg() tagged_def_init(AST, AsmNeg)
#define init_AsmShr() tagged_def_init(AST, AsmShr)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// instruction = Mov(assembly_type, operand, operand)
//             | MovSx(assembly_type, assembly_type, operand, operand)
//             | MovZeroExtend(assembly_type, assembly_type, operand, operand)
//             | Lea(operand, operand)
//             | Cvttsd2si(assembly_type, operand, operand)
//             | Cvtsi2sd(assembly_type, operand, operand)
//             | Unary(unary_operator, assembly_type, operand)
//             | Binary(binary_operator, assembly_type, operand, operand)
//             | Cmp(assembly_type, operand, operand)
//             | Idiv(assembly_type, operand)
//             | Div(assembly_type, operand)
//             | Cdq(assembly_type)
//             | Jmp(identifier)
//             | JmpCC(cond_code, identifier)
//             | SetCC(cond_code, operand)
//             | Label(identifier)
//             | Push(operand)
//             | Pop(reg)
//             | Call(identifier)
//             | Ret

typedef struct AsmMov {
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmMov;

typedef struct AsmMovSx {
    shared_ptr_t(AssemblyType) asm_type_src;
    shared_ptr_t(AssemblyType) asm_type_dst;
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmMovSx;

typedef struct AsmMovZeroExtend {
    shared_ptr_t(AssemblyType) asm_type_src;
    shared_ptr_t(AssemblyType) asm_type_dst;
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmMovZeroExtend;

typedef struct AsmLea {
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmLea;

typedef struct AsmCvttsd2si {
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmCvttsd2si;

typedef struct AsmCvtsi2sd {
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmCvtsi2sd;

typedef struct AsmUnary {
    unique_ptr_t(AsmUnaryOp) unop;
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) dst;
} AsmUnary;

typedef struct AsmBinary {
    unique_ptr_t(AsmBinaryOp) binop;
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmBinary;

typedef struct AsmCmp {
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) src;
    shared_ptr_t(AsmOperand) dst;
} AsmCmp;

typedef struct AsmIdiv {
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) src;
} AsmIdiv;

typedef struct AsmDiv {
    shared_ptr_t(AssemblyType) asm_type;
    shared_ptr_t(AsmOperand) src;
} AsmDiv;

typedef struct AsmCdq {
    shared_ptr_t(AssemblyType) asm_type;
} AsmCdq;

typedef struct AsmJmp {
    TIdentifier target;
} AsmJmp;

typedef struct AsmJmpCC {
    TIdentifier target;
    unique_ptr_t(AsmCondCode) cond_code;
} AsmJmpCC;

typedef struct AsmSetCC {
    unique_ptr_t(AsmCondCode) cond_code;
    shared_ptr_t(AsmOperand) dst;
} AsmSetCC;

typedef struct AsmLabel {
    TIdentifier name;
} AsmLabel;

typedef struct AsmPush {
    shared_ptr_t(AsmOperand) src;
} AsmPush;

typedef struct AsmPop {
    unique_ptr_t(AsmReg) reg;
} AsmPop;

typedef struct AsmCall {
    TIdentifier name;
} AsmCall;

typedef struct AsmRet {
    int8_t _empty;
} AsmRet;

typedef struct AsmInstruction {
    unique_ptr_impl(AST_T);

    union {
        AsmMov _AsmMov;
        AsmMovSx _AsmMovSx;
        AsmMovZeroExtend _AsmMovZeroExtend;
        AsmLea _AsmLea;
        AsmCvttsd2si _AsmCvttsd2si;
        AsmCvtsi2sd _AsmCvtsi2sd;
        AsmUnary _AsmUnary;
        AsmBinary _AsmBinary;
        AsmCmp _AsmCmp;
        AsmIdiv _AsmIdiv;
        AsmDiv _AsmDiv;
        AsmCdq _AsmCdq;
        AsmJmp _AsmJmp;
        AsmJmpCC _AsmJmpCC;
        AsmSetCC _AsmSetCC;
        AsmLabel _AsmLabel;
        AsmPush _AsmPush;
        AsmPop _AsmPop;
        AsmCall _AsmCall;
        AsmRet _AsmRet;
    } get;
} AsmInstruction;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmInstruction) make_AsmInstruction(void);
unique_ptr_t(AsmInstruction)
    make_AsmMov(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmMovSx(shared_ptr_t(AssemblyType) * asm_type_src,
    shared_ptr_t(AssemblyType) * asm_type_dst, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmMovZeroExtend(shared_ptr_t(AssemblyType) * asm_type_src,
    shared_ptr_t(AssemblyType) * asm_type_dst, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmLea(shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmCvttsd2si(
    shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmCvtsi2sd(
    shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmUnary(
    unique_ptr_t(AsmUnaryOp) * unop, shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmBinary(unique_ptr_t(AsmBinaryOp) * binop, shared_ptr_t(AssemblyType) * asm_type,
    shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction)
    make_AsmCmp(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmIdiv(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src);
unique_ptr_t(AsmInstruction) make_AsmDiv(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src);
unique_ptr_t(AsmInstruction) make_AsmCdq(shared_ptr_t(AssemblyType) * asm_type);
unique_ptr_t(AsmInstruction) make_AsmJmp(TIdentifier target);
unique_ptr_t(AsmInstruction) make_AsmJmpCC(TIdentifier target, unique_ptr_t(AsmCondCode) * cond_code);
unique_ptr_t(AsmInstruction) make_AsmSetCC(unique_ptr_t(AsmCondCode) * cond_code, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmLabel(TIdentifier name);
unique_ptr_t(AsmInstruction) make_AsmPush(shared_ptr_t(AsmOperand) * src);
unique_ptr_t(AsmInstruction) make_AsmPop(unique_ptr_t(AsmReg) * reg);
unique_ptr_t(AsmInstruction) make_AsmCall(TIdentifier name);
unique_ptr_t(AsmInstruction) make_AsmRet(void);
void free_AsmInstruction(unique_ptr_t(AsmInstruction) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// top_level = Function(identifier, bool, bool, instruction*)
//           | StaticVariable(identifier, bool, int, static_init*)
//           | StaticConstant(identifier, int, static_init)

typedef struct AsmFunction {
    TIdentifier name;
    bool is_glob;
    bool is_ret_memory;
    vector_t(unique_ptr_t(AsmInstruction)) instructions;
} AsmFunction;

typedef struct AsmStaticVariable {
    TIdentifier name;
    TInt alignment;
    bool is_glob;
    vector_t(shared_ptr_t(StaticInit)) static_inits;
} AsmStaticVariable;

typedef struct AsmStaticConstant {
    TIdentifier name;
    TInt alignment;
    shared_ptr_t(StaticInit) static_init;
} AsmStaticConstant;

typedef struct AsmTopLevel {
    unique_ptr_impl(AST_T);

    union {
        AsmFunction _AsmFunction;
        AsmStaticVariable _AsmStaticVariable;
        AsmStaticConstant _AsmStaticConstant;
    } get;
} AsmTopLevel;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmTopLevel) make_AsmTopLevel(void);
unique_ptr_t(AsmTopLevel) make_AsmFunction(
    TIdentifier name, bool is_glob, bool is_ret_memory, vector_t(unique_ptr_t(AsmInstruction)) * instructions);
unique_ptr_t(AsmTopLevel) make_AsmStaticVariable(
    TIdentifier name, TInt alignment, bool is_glob, vector_t(shared_ptr_t(StaticInit)) * static_inits);
unique_ptr_t(AsmTopLevel)
    make_AsmStaticConstant(TIdentifier name, TInt alignment, shared_ptr_t(StaticInit) * static_init);
void free_AsmTopLevel(unique_ptr_t(AsmTopLevel) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// AST = Program(top_level*, top_level*)

typedef struct AsmProgram {
    unique_ptr_impl(AST_T);
    vector_t(unique_ptr_t(AsmTopLevel)) static_const_toplvls;
    vector_t(unique_ptr_t(AsmTopLevel)) top_levels;
} AsmProgram;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmProgram) make_AsmProgram(
    vector_t(unique_ptr_t(AsmTopLevel)) * static_const_toplvls, vector_t(unique_ptr_t(AsmTopLevel)) * top_levels);
void free_AsmProgram(unique_ptr_t(AsmProgram) * self);
#ifdef __cplusplus
}
#endif

#endif
