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

typedef struct AsmReg {
    tagged_def_impl(AST_T);
} AsmReg;

#define init_AsmReg() tagged_def_init(AST, AsmReg, AsmReg)
#define init_AsmAx() tagged_def_init(AST, AsmReg, AsmAx)
#define init_AsmBx() tagged_def_init(AST, AsmReg, AsmBx)
#define init_AsmCx() tagged_def_init(AST, AsmReg, AsmCx)
#define init_AsmDx() tagged_def_init(AST, AsmReg, AsmDx)
#define init_AsmDi() tagged_def_init(AST, AsmReg, AsmDi)
#define init_AsmSi() tagged_def_init(AST, AsmReg, AsmSi)
#define init_AsmR8() tagged_def_init(AST, AsmReg, AsmR8)
#define init_AsmR9() tagged_def_init(AST, AsmReg, AsmR9)
#define init_AsmR10() tagged_def_init(AST, AsmReg, AsmR10)
#define init_AsmR11() tagged_def_init(AST, AsmReg, AsmR11)
#define init_AsmR12() tagged_def_init(AST, AsmReg, AsmR12)
#define init_AsmR13() tagged_def_init(AST, AsmReg, AsmR13)
#define init_AsmR14() tagged_def_init(AST, AsmReg, AsmR14)
#define init_AsmR15() tagged_def_init(AST, AsmReg, AsmR15)
#define init_AsmSp() tagged_def_init(AST, AsmReg, AsmSp)
#define init_AsmBp() tagged_def_init(AST, AsmReg, AsmBp)
#define init_AsmXMM0() tagged_def_init(AST, AsmReg, AsmXMM0)
#define init_AsmXMM1() tagged_def_init(AST, AsmReg, AsmXMM1)
#define init_AsmXMM2() tagged_def_init(AST, AsmReg, AsmXMM2)
#define init_AsmXMM3() tagged_def_init(AST, AsmReg, AsmXMM3)
#define init_AsmXMM4() tagged_def_init(AST, AsmReg, AsmXMM4)
#define init_AsmXMM5() tagged_def_init(AST, AsmReg, AsmXMM5)
#define init_AsmXMM6() tagged_def_init(AST, AsmReg, AsmXMM6)
#define init_AsmXMM7() tagged_def_init(AST, AsmReg, AsmXMM7)
#define init_AsmXMM8() tagged_def_init(AST, AsmReg, AsmXMM8)
#define init_AsmXMM9() tagged_def_init(AST, AsmReg, AsmXMM9)
#define init_AsmXMM10() tagged_def_init(AST, AsmReg, AsmXMM10)
#define init_AsmXMM11() tagged_def_init(AST, AsmReg, AsmXMM11)
#define init_AsmXMM12() tagged_def_init(AST, AsmReg, AsmXMM12)
#define init_AsmXMM13() tagged_def_init(AST, AsmReg, AsmXMM13)
#define init_AsmXMM14() tagged_def_init(AST, AsmReg, AsmXMM14)
#define init_AsmXMM15() tagged_def_init(AST, AsmReg, AsmXMM15)

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

typedef struct AsmCondCode {
    tagged_def_impl(AST_T);
} AsmCondCode;

#define init_AsmCondCode() tagged_def_init(AST, AsmCondCode, AsmCondCode)
#define init_AsmE() tagged_def_init(AST, AsmCondCode, AsmE)
#define init_AsmNE() tagged_def_init(AST, AsmCondCode, AsmNE)
#define init_AsmG() tagged_def_init(AST, AsmCondCode, AsmG)
#define init_AsmGE() tagged_def_init(AST, AsmCondCode, AsmGE)
#define init_AsmL() tagged_def_init(AST, AsmCondCode, AsmL)
#define init_AsmLE() tagged_def_init(AST, AsmCondCode, AsmLE)
#define init_AsmA() tagged_def_init(AST, AsmCondCode, AsmA)
#define init_AsmAE() tagged_def_init(AST, AsmCondCode, AsmAE)
#define init_AsmB() tagged_def_init(AST, AsmCondCode, AsmB)
#define init_AsmBE() tagged_def_init(AST, AsmCondCode, AsmBE)
#define init_AsmP() tagged_def_init(AST, AsmCondCode, AsmP)

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
    AsmReg reg;
} AsmRegister;

typedef struct AsmPseudo {
    TIdentifier name;
} AsmPseudo;

typedef struct AsmMemory {
    TLong value;
    AsmReg reg;
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
    AsmReg reg_base;
    AsmReg reg_index;
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
shared_ptr_t(AsmOperand) make_AsmRegister(AsmReg* reg);
shared_ptr_t(AsmOperand) make_AsmPseudo(TIdentifier name);
shared_ptr_t(AsmOperand) make_AsmMemory(TLong value, AsmReg* reg);
shared_ptr_t(AsmOperand) make_AsmData(TIdentifier name, TLong offset);
shared_ptr_t(AsmOperand) make_AsmPseudoMem(TIdentifier name, TLong offset);
shared_ptr_t(AsmOperand) make_AsmIndexed(TLong scale, AsmReg* reg_base, AsmReg* reg_index);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Not
//                | Neg
//                | Shr

typedef struct AsmUnaryOp {
    tagged_def_impl(AST_T);
} AsmUnaryOp;

#define init_AsmUnaryOp() tagged_def_init(AST, AsmUnaryOp, AsmUnaryOp)
#define init_AsmNot() tagged_def_init(AST, AsmUnaryOp, AsmNot)
#define init_AsmNeg() tagged_def_init(AST, AsmUnaryOp, AsmNeg)
#define init_AsmShr() tagged_def_init(AST, AsmUnaryOp, AsmShr)

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
    AsmUnaryOp unop;
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
    AsmCondCode cond_code;
} AsmJmpCC;

typedef struct AsmSetCC {
    AsmCondCode cond_code;
    shared_ptr_t(AsmOperand) dst;
} AsmSetCC;

typedef struct AsmLabel {
    TIdentifier name;
} AsmLabel;

typedef struct AsmPush {
    shared_ptr_t(AsmOperand) src;
} AsmPush;

typedef struct AsmPop {
    AsmReg reg;
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
unique_ptr_t(AsmInstruction)
    make_AsmUnary(AsmUnaryOp* unop, shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmBinary(unique_ptr_t(AsmBinaryOp) * binop, shared_ptr_t(AssemblyType) * asm_type,
    shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction)
    make_AsmCmp(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmIdiv(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src);
unique_ptr_t(AsmInstruction) make_AsmDiv(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src);
unique_ptr_t(AsmInstruction) make_AsmCdq(shared_ptr_t(AssemblyType) * asm_type);
unique_ptr_t(AsmInstruction) make_AsmJmp(TIdentifier target);
unique_ptr_t(AsmInstruction) make_AsmJmpCC(TIdentifier target, AsmCondCode* cond_code);
unique_ptr_t(AsmInstruction) make_AsmSetCC(AsmCondCode* cond_code, shared_ptr_t(AsmOperand) * dst);
unique_ptr_t(AsmInstruction) make_AsmLabel(TIdentifier name);
unique_ptr_t(AsmInstruction) make_AsmPush(shared_ptr_t(AsmOperand) * src);
unique_ptr_t(AsmInstruction) make_AsmPop(AsmReg* reg);
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
