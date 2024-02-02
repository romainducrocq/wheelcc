#ifndef _AST_TAC_AST_HPP
#define _AST_TAC_AST_HPP

#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Complement
//                | Negate
//                | Not
struct TacUnaryOp : Ast {
    AST_T type() override;
};

struct TacComplement : TacUnaryOp {
    AST_T type() override;
};

struct TacNegate : TacUnaryOp {
    AST_T type() override;
};

struct TacNot : TacUnaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// binary_operator = Add
//                 | Subtract
//                 | Multiply
//                 | Divide
//                 | Remainder
//                 | BitAnd
//                 | BitOr
//                 | BitXor
//                 | BitShiftLeft
//                 | BitShiftRight
//                 | Equal
//                 | NotEqual
//                 | LessThan
//                 | LessOrEqual
//                 | GreaterThan
//                 | GreaterOrEqual
struct TacBinaryOp : Ast {
    AST_T type() override;
};

struct TacAdd : TacBinaryOp {
    AST_T type() override;
};

struct TacSubtract : TacBinaryOp {
    AST_T type() override;
};

struct TacMultiply : TacBinaryOp {
    AST_T type() override;
};

struct TacDivide : TacBinaryOp {
    AST_T type() override;
};

struct TacRemainder : TacBinaryOp {
    AST_T type() override;
};

struct TacBitAnd : TacBinaryOp {
    AST_T type() override;
};

struct TacBitOr : TacBinaryOp {
    AST_T type() override;
};

struct TacBitXor : TacBinaryOp {
    AST_T type() override;
};

struct TacBitShiftLeft : TacBinaryOp {
    AST_T type() override;
};

struct TacBitShiftRight : TacBinaryOp {
    AST_T type() override;
};

struct TacEqual : TacBinaryOp {
    AST_T type() override;
};

struct TacNotEqual : TacBinaryOp {
    AST_T type() override;
};

struct TacLessThan : TacBinaryOp {
    AST_T type() override;
};

struct TacLessOrEqual : TacBinaryOp {
    AST_T type() override;
};

struct TacGreaterThan : TacBinaryOp {
    AST_T type() override;
};

struct TacGreaterOrEqual : TacBinaryOp {
    AST_T type() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// val = Constant(int)
//     | Var(identifier)
struct TacValue : Ast {
    AST_T type() override;
};

struct TacConstant : TacValue {
    AST_T type() override;
    TacConstant() = default;
    TacConstant(std::shared_ptr<CConst> constant);

    std::shared_ptr<CConst> constant;
};

struct TacVariable : TacValue {
    AST_T type() override;
    TacVariable() = default;
    TacVariable(TIdentifier name);

    TIdentifier name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// instruction = Return(val)
//             | SignExtend(val src, val dst)
//             | Truncate(val src, val dst)
//             | ZeroExtend(val src, val dst)
//             | TacDoubleToInt(val src, val dst)
//             | TacDoubleToUInt(val src, val dst)
//             | TacIntToDouble(val src, val dst)
//             | TacUIntToDouble(val src, val dst)
//             | FunCall(identifier fun_name, val* args, val dst)
//             | Unary(unary_operator, val src, val dst)
//             | Binary(binary_operator, val src1, val src2, val dst)
//             | Copy(val src, val dst)
//             | Jump(identifier target)
//             | JumpIfZero(val condition, identifier target)
//             | JumpIfNotZero(val condition, identifier target)
//             | Label(identifier name)
struct TacInstruction : Ast {
    AST_T type() override;
};

struct TacReturn : TacInstruction {
    AST_T type() override;
    TacReturn() = default;
    TacReturn(std::shared_ptr<TacValue> val);

    std::shared_ptr<TacValue> val;
};

struct TacSignExtend : TacInstruction {
    AST_T type() override;
    TacSignExtend() = default;
    TacSignExtend(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacTruncate : TacInstruction {
    AST_T type() override;
    TacTruncate() = default;
    TacTruncate(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacZeroExtend : TacInstruction {
    AST_T type() override;
    TacZeroExtend() = default;
    TacZeroExtend(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacDoubleToInt : TacInstruction {
    AST_T type() override;
    TacDoubleToInt() = default;
    TacDoubleToInt(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacDoubleToUInt : TacInstruction {
    AST_T type() override;
    TacDoubleToUInt() = default;
    TacDoubleToUInt(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacIntToDouble : TacInstruction {
    AST_T type() override;
    TacIntToDouble() = default;
    TacIntToDouble(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacUIntToDouble : TacInstruction {
    AST_T type() override;
    TacUIntToDouble() = default;
    TacUIntToDouble(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacFunCall : TacInstruction {
    AST_T type() override;
    TacFunCall() = default;
    TacFunCall(TIdentifier name, std::vector<std::shared_ptr<TacValue>> args,
               std::shared_ptr<TacValue> dst);

    TIdentifier name;
    std::vector<std::shared_ptr<TacValue>> args;
    std::shared_ptr<TacValue> dst;
};

struct TacUnary : TacInstruction {
    AST_T type() override;
    TacUnary() = default;
    TacUnary(std::unique_ptr<TacUnaryOp> unary_op, std::shared_ptr<TacValue> src,
             std::shared_ptr<TacValue> dst);

    std::unique_ptr<TacUnaryOp> unary_op;
    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacBinary : TacInstruction {
    AST_T type() override;
    TacBinary() = default;
    TacBinary(std::unique_ptr<TacBinaryOp> binary_op, std::shared_ptr<TacValue> src1,
              std::shared_ptr<TacValue> src2, std::shared_ptr<TacValue> dst);

    std::unique_ptr<TacBinaryOp> binary_op;
    std::shared_ptr<TacValue> src1;
    std::shared_ptr<TacValue> src2;
    std::shared_ptr<TacValue> dst;
};

struct TacCopy : TacInstruction {
    AST_T type() override;
    TacCopy() = default;
    TacCopy(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacJump : TacInstruction {
    AST_T type() override;
    TacJump() = default;
    TacJump(TIdentifier target);

    TIdentifier target;
};

struct TacJumpIfZero : TacInstruction {
    AST_T type() override;
    TacJumpIfZero() = default;
    TacJumpIfZero(TIdentifier target, std::shared_ptr<TacValue> condition);

    TIdentifier target;
    std::shared_ptr<TacValue> condition;
};

struct TacJumpIfNotZero : TacInstruction {
    AST_T type() override;
    TacJumpIfNotZero() = default;
    TacJumpIfNotZero(TIdentifier target, std::shared_ptr<TacValue> condition);

    TIdentifier target;
    std::shared_ptr<TacValue> condition;
};

struct TacLabel : TacInstruction {
    AST_T type() override;
    TacLabel() = default;
    TacLabel(TIdentifier name);

    TIdentifier name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// top_level = Function(identifier, bool global, identifier* params, instruction* body)
//           | StaticVariable(identifier, bool global, type t, static_init init)
struct TacTopLevel : Ast {
    AST_T type() override;
};

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

// AST = Program(top_level*)
struct TacProgram : Ast {
    AST_T type() override;
    TacProgram() = default;
    TacProgram(std::vector<std::unique_ptr<TacTopLevel>> top_levels);

    std::vector<std::unique_ptr<TacTopLevel>> top_levels;
};

/*
struct Dummy : Ast {
};
*/

#endif
