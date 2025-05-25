#ifndef _AST_BACK_AST_H
#define _AST_BACK_AST_H

#include <memory>
#include <vector>

#include "ast/ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend abstract syntax tree

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
struct AsmReg : Ast {
    AST_T type() override;
};

struct AsmAx : AsmReg {
    AST_T type() override;
};

struct AsmBx : AsmReg {
    AST_T type() override;
};

struct AsmCx : AsmReg {
    AST_T type() override;
};

struct AsmDx : AsmReg {
    AST_T type() override;
};

struct AsmDi : AsmReg {
    AST_T type() override;
};

struct AsmSi : AsmReg {
    AST_T type() override;
};

struct AsmR8 : AsmReg {
    AST_T type() override;
};

struct AsmR9 : AsmReg {
    AST_T type() override;
};

struct AsmR10 : AsmReg {
    AST_T type() override;
};

struct AsmR11 : AsmReg {
    AST_T type() override;
};

struct AsmR12 : AsmReg {
    AST_T type() override;
};

struct AsmR13 : AsmReg {
    AST_T type() override;
};

struct AsmR14 : AsmReg {
    AST_T type() override;
};

struct AsmR15 : AsmReg {
    AST_T type() override;
};

struct AsmSp : AsmReg {
    AST_T type() override;
};

struct AsmBp : AsmReg {
    AST_T type() override;
};

struct AsmXMM0 : AsmReg {
    AST_T type() override;
};

struct AsmXMM1 : AsmReg {
    AST_T type() override;
};

struct AsmXMM2 : AsmReg {
    AST_T type() override;
};

struct AsmXMM3 : AsmReg {
    AST_T type() override;
};

struct AsmXMM4 : AsmReg {
    AST_T type() override;
};

struct AsmXMM5 : AsmReg {
    AST_T type() override;
};

struct AsmXMM6 : AsmReg {
    AST_T type() override;
};

struct AsmXMM7 : AsmReg {
    AST_T type() override;
};

struct AsmXMM8 : AsmReg {
    AST_T type() override;
};

struct AsmXMM9 : AsmReg {
    AST_T type() override;
};

struct AsmXMM10 : AsmReg {
    AST_T type() override;
};

struct AsmXMM11 : AsmReg {
    AST_T type() override;
};

struct AsmXMM12 : AsmReg {
    AST_T type() override;
};

struct AsmXMM13 : AsmReg {
    AST_T type() override;
};

struct AsmXMM14 : AsmReg {
    AST_T type() override;
};

struct AsmXMM15 : AsmReg {
    AST_T type() override;
};

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
struct AsmCondCode : Ast {
    AST_T type() override;
};

struct AsmE : AsmCondCode {
    AST_T type() override;
};

struct AsmNE : AsmCondCode {
    AST_T type() override;
};

struct AsmG : AsmCondCode {
    AST_T type() override;
};

struct AsmGE : AsmCondCode {
    AST_T type() override;
};

struct AsmL : AsmCondCode {
    AST_T type() override;
};

struct AsmLE : AsmCondCode {
    AST_T type() override;
};

struct AsmA : AsmCondCode {
    AST_T type() override;
};

struct AsmAE : AsmCondCode {
    AST_T type() override;
};

struct AsmB : AsmCondCode {
    AST_T type() override;
};

struct AsmBE : AsmCondCode {
    AST_T type() override;
};

struct AsmP : AsmCondCode {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// operand = Imm(int, bool, bool, bool)
//         | Reg(reg)
//         | Pseudo(identifier)
//         | Memory(int, reg)
//         | Data(identifier, int)
//         | PseudoMem(identifier, int)
//         | Indexed(int, reg, reg)
struct AsmOperand : Ast {
    AST_T type() override;
};

struct AsmImm : AsmOperand {
    AST_T type() override;
    AsmImm() = default;
    AsmImm(TULong value, bool is_byte, bool is_quad, bool is_neg);

    TULong value;
    bool is_byte;
    bool is_quad;
    bool is_neg;
};

struct AsmRegister : AsmOperand {
    AST_T type() override;
    AsmRegister() = default;
    AsmRegister(std::unique_ptr<AsmReg>@@ reg);

    std::unique_ptr<AsmReg> reg;
};

struct AsmPseudo : AsmOperand {
    AST_T type() override;
    AsmPseudo() = default;
    AsmPseudo(TIdentifier name);

    TIdentifier name;
};

struct AsmMemory : AsmOperand {
    AST_T type() override;
    AsmMemory() = default;
    AsmMemory(TLong value, std::unique_ptr<AsmReg>@@ reg);

    TLong value;
    std::unique_ptr<AsmReg> reg;
};

struct AsmData : AsmOperand {
    AST_T type() override;
    AsmData() = default;
    AsmData(TIdentifier name, TLong offset);

    TIdentifier name;
    TLong offset;
};

struct AsmPseudoMem : AsmOperand {
    AST_T type() override;
    AsmPseudoMem() = default;
    AsmPseudoMem(TIdentifier name, TLong offset);

    TIdentifier name;
    TLong offset;
};

struct AsmIndexed : AsmOperand {
    AST_T type() override;
    AsmIndexed() = default;
    AsmIndexed(TLong scale, std::unique_ptr<AsmReg>@@ reg_base, std::unique_ptr<AsmReg>@@ reg_index);

    TLong scale;
    std::unique_ptr<AsmReg> reg_base;
    std::unique_ptr<AsmReg> reg_index;
};

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
struct AsmBinaryOp : Ast {
    AST_T type() override;
};

struct AsmAdd : AsmBinaryOp {
    AST_T type() override;
};

struct AsmSub : AsmBinaryOp {
    AST_T type() override;
};

struct AsmMult : AsmBinaryOp {
    AST_T type() override;
};

struct AsmDivDouble : AsmBinaryOp {
    AST_T type() override;
};

struct AsmBitAnd : AsmBinaryOp {
    AST_T type() override;
};

struct AsmBitOr : AsmBinaryOp {
    AST_T type() override;
};

struct AsmBitXor : AsmBinaryOp {
    AST_T type() override;
};

struct AsmBitShiftLeft : AsmBinaryOp {
    AST_T type() override;
};

struct AsmBitShiftRight : AsmBinaryOp {
    AST_T type() override;
};

struct AsmBitShrArithmetic : AsmBinaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Not
//                | Neg
//                | Shr
struct AsmUnaryOp : Ast {
    AST_T type() override;
};

struct AsmNot : AsmUnaryOp {
    AST_T type() override;
};

struct AsmNeg : AsmUnaryOp {
    AST_T type() override;
};

struct AsmShr : AsmUnaryOp {
    AST_T type() override;
};

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
struct AsmInstruction : Ast {
    AST_T type() override;
};

struct AsmMov : AsmInstruction {
    AST_T type() override;
    AsmMov() = default;
    AsmMov(std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmMovSx : AsmInstruction {
    AST_T type() override;
    AsmMovSx() = default;
    AsmMovSx(std::shared_ptr<AssemblyType> asm_type_src, std::shared_ptr<AssemblyType> asm_type_dst, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::shared_ptr<AssemblyType> asm_type_src;
    std::shared_ptr<AssemblyType> asm_type_dst;
    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmMovZeroExtend : AsmInstruction {
    AST_T type() override;
    AsmMovZeroExtend() = default;
    AsmMovZeroExtend(std::shared_ptr<AssemblyType> asm_type_src, std::shared_ptr<AssemblyType> asm_type_dst, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::shared_ptr<AssemblyType> asm_type_src;
    std::shared_ptr<AssemblyType> asm_type_dst;
    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmLea : AsmInstruction {
    AST_T type() override;
    AsmLea() = default;
    AsmLea(std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmCvttsd2si : AsmInstruction {
    AST_T type() override;
    AsmCvttsd2si() = default;
    AsmCvttsd2si(std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmCvtsi2sd : AsmInstruction {
    AST_T type() override;
    AsmCvtsi2sd() = default;
    AsmCvtsi2sd(std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmUnary : AsmInstruction {
    AST_T type() override;
    AsmUnary() = default;
    AsmUnary(std::unique_ptr<AsmUnaryOp>@@ unop, std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> dst);

    std::unique_ptr<AsmUnaryOp> unop;
    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmBinary : AsmInstruction {
    AST_T type() override;
    AsmBinary() = default;
    AsmBinary(std::unique_ptr<AsmBinaryOp>@@ binop, std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::unique_ptr<AsmBinaryOp> binop;
    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmCmp : AsmInstruction {
    AST_T type() override;
    AsmCmp() = default;
    AsmCmp(std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst);

    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> src;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmIdiv : AsmInstruction {
    AST_T type() override;
    AsmIdiv() = default;
    AsmIdiv(std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> src);

    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> src;
};

struct AsmDiv : AsmInstruction {
    AST_T type() override;
    AsmDiv() = default;
    AsmDiv(std::shared_ptr<AssemblyType> asm_type, std::shared_ptr<AsmOperand> src);

    std::shared_ptr<AssemblyType> asm_type;
    std::shared_ptr<AsmOperand> src;
};

struct AsmCdq : AsmInstruction {
    AST_T type() override;
    AsmCdq() = default;
    AsmCdq(std::shared_ptr<AssemblyType> asm_type);

    std::shared_ptr<AssemblyType> asm_type;
};

struct AsmJmp : AsmInstruction {
    AST_T type() override;
    AsmJmp() = default;
    AsmJmp(TIdentifier target);

    TIdentifier target;
};

struct AsmJmpCC : AsmInstruction {
    AST_T type() override;
    AsmJmpCC() = default;
    AsmJmpCC(TIdentifier target, std::unique_ptr<AsmCondCode>@@ cond_code);

    TIdentifier target;
    std::unique_ptr<AsmCondCode> cond_code;
};

struct AsmSetCC : AsmInstruction {
    AST_T type() override;
    AsmSetCC() = default;
    AsmSetCC(std::unique_ptr<AsmCondCode>@@ cond_code, std::shared_ptr<AsmOperand> dst);

    std::unique_ptr<AsmCondCode> cond_code;
    std::shared_ptr<AsmOperand> dst;
};

struct AsmLabel : AsmInstruction {
    AST_T type() override;
    AsmLabel() = default;
    AsmLabel(TIdentifier name);

    TIdentifier name;
};

struct AsmPush : AsmInstruction {
    AST_T type() override;
    AsmPush() = default;
    AsmPush(std::shared_ptr<AsmOperand> src);

    std::shared_ptr<AsmOperand> src;
};

struct AsmPop : AsmInstruction {
    AST_T type() override;
    AsmPop() = default;
    AsmPop(std::unique_ptr<AsmReg>@@ reg);

    std::unique_ptr<AsmReg> reg;
};

struct AsmCall : AsmInstruction {
    AST_T type() override;
    AsmCall() = default;
    AsmCall(TIdentifier name);

    TIdentifier name;
};

struct AsmRet : AsmInstruction {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// top_level = Function(identifier, bool, bool, instruction*)
//           | StaticVariable(identifier, bool, int, static_init*)
//           | StaticConstant(identifier, int, static_init)
struct AsmTopLevel : Ast {
    AST_T type() override;
};

struct AsmFunction : AsmTopLevel {
    AST_T type() override;
    AsmFunction() = default;
    AsmFunction(TIdentifier name, bool is_glob, bool is_ret_memory, std::vector<std::unique_ptr<AsmInstruction>> instructions);

    TIdentifier name;
    bool is_glob;
    bool is_ret_memory;
    std::vector<std::unique_ptr<AsmInstruction>> instructions;
};

struct AsmStaticVariable : AsmTopLevel {
    AST_T type() override;
    AsmStaticVariable() = default;
    AsmStaticVariable(TIdentifier name, TInt alignment, bool is_glob, std::vector<std::shared_ptr<StaticInit>> static_inits);

    TIdentifier name;
    TInt alignment;
    bool is_glob;
    std::vector<std::shared_ptr<StaticInit>> static_inits;
};

struct AsmStaticConstant : AsmTopLevel {
    AST_T type() override;
    AsmStaticConstant() = default;
    AsmStaticConstant(TIdentifier name, TInt alignment, std::shared_ptr<StaticInit> static_init);

    TIdentifier name;
    TInt alignment;
    std::shared_ptr<StaticInit> static_init;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// AST = Program(top_level*, top_level*)
struct AsmProgram : Ast {
    AST_T type() override;
    AsmProgram() = default;
    AsmProgram(std::vector<std::unique_ptr<AsmTopLevel>> static_const_toplvls, std::vector<std::unique_ptr<AsmTopLevel>> top_levels);

    std::vector<std::unique_ptr<AsmTopLevel>> static_const_toplvls;
    std::vector<std::unique_ptr<AsmTopLevel>> top_levels;
};

/*
struct Dummy : Ast {
};
*/

#endif
