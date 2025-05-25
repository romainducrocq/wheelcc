#ifndef _AST_INTERM_AST_H
#define _AST_INTERM_AST_H

#include <memory>
#include <vector>

#include "ast/ast.hpp"
#include "ast/front_symt.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Intermediate abstract syntax tree

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
//                 | BitShrArithmetic
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

struct TacBitShrArithmetic : TacBinaryOp {
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

// exp_result = PlainOperand(val)
//            | DereferencedPointer(val)
//            | SubObject(identifier, int)
struct TacExpResult : Ast {
    AST_T type() override;
};

struct TacPlainOperand : TacExpResult {
    AST_T type() override;
    TacPlainOperand() = default;
    TacPlainOperand(std::shared_ptr<TacValue> val);

    std::shared_ptr<TacValue> val;
};

struct TacDereferencedPointer : TacExpResult {
    AST_T type() override;
    TacDereferencedPointer() = default;
    TacDereferencedPointer(std::shared_ptr<TacValue> val);

    std::shared_ptr<TacValue> val;
};

struct TacSubObject : TacExpResult {
    AST_T type() override;
    TacSubObject() = default;
    TacSubObject(TIdentifier base_name, TLong offset);

    TIdentifier base_name;
    TLong offset;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// instruction = Return(val?)
//             | SignExtend(val, val)
//             | Truncate(val, val)
//             | ZeroExtend(val, val)
//             | TacDoubleToInt(val, val)
//             | TacDoubleToUInt(val, val)
//             | TacIntToDouble(val, val)
//             | TacUIntToDouble(val, val)
//             | FunCall(identifier, val*, val?)
//             | Unary(unary_operator, val, val)
//             | Binary(binary_operator, val, val, val)
//             | Copy(val, val)
//             | GetAddress(val, val)
//             | Load(val, val)
//             | Store(val, val)
//             | AddPtr(int, val, val, val)
//             | CopyToOffset(identifier, int, val)
//             | CopyFromOffset(identifier, int, val)
//             | Jump(identifier)
//             | JumpIfZero(val, identifier)
//             | JumpIfNotZero(val, identifier)
//             | Label(identifier)
struct TacInstruction : Ast {
    AST_T type() override;
};

struct TacReturn : TacInstruction {
    AST_T type() override;
    TacReturn() = default;
    TacReturn(std::shared_ptr<TacValue> val);

    // Optional
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
    TacFunCall(TIdentifier name, std::vector<std::shared_ptr<TacValue>>&& args, std::shared_ptr<TacValue> dst);

    TIdentifier name;
    std::vector<std::shared_ptr<TacValue>> args;
    // Optional
    std::shared_ptr<TacValue> dst;
};

struct TacUnary : TacInstruction {
    AST_T type() override;
    TacUnary() = default;
    TacUnary(std::unique_ptr<TacUnaryOp>&& unop, std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::unique_ptr<TacUnaryOp> unop;
    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacBinary : TacInstruction {
    AST_T type() override;
    TacBinary() = default;
    TacBinary(std::unique_ptr<TacBinaryOp>&& binop, std::shared_ptr<TacValue> src1, std::shared_ptr<TacValue> src2,
        std::shared_ptr<TacValue> dst);

    std::unique_ptr<TacBinaryOp> binop;
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

struct TacGetAddress : TacInstruction {
    AST_T type() override;
    TacGetAddress() = default;
    TacGetAddress(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst;
};

struct TacLoad : TacInstruction {
    AST_T type() override;
    TacLoad() = default;
    TacLoad(std::shared_ptr<TacValue> src_ptr, std::shared_ptr<TacValue> dst);

    std::shared_ptr<TacValue> src_ptr;
    std::shared_ptr<TacValue> dst;
};

struct TacStore : TacInstruction {
    AST_T type() override;
    TacStore() = default;
    TacStore(std::shared_ptr<TacValue> src, std::shared_ptr<TacValue> dst_ptr);

    std::shared_ptr<TacValue> src;
    std::shared_ptr<TacValue> dst_ptr;
};

struct TacAddPtr : TacInstruction {
    AST_T type() override;
    TacAddPtr() = default;
    TacAddPtr(
        TLong scale, std::shared_ptr<TacValue> src_ptr, std::shared_ptr<TacValue> idx, std::shared_ptr<TacValue> dst);

    TLong scale;
    std::shared_ptr<TacValue> src_ptr;
    std::shared_ptr<TacValue> idx;
    std::shared_ptr<TacValue> dst;
};

struct TacCopyToOffset : TacInstruction {
    AST_T type() override;
    TacCopyToOffset() = default;
    TacCopyToOffset(TIdentifier dst_name, TLong offset, std::shared_ptr<TacValue> src);

    TIdentifier dst_name;
    TLong offset;
    std::shared_ptr<TacValue> src;
};

struct TacCopyFromOffset : TacInstruction {
    AST_T type() override;
    TacCopyFromOffset() = default;
    TacCopyFromOffset(TIdentifier src_name, TLong offset, std::shared_ptr<TacValue> dst);

    TIdentifier src_name;
    TLong offset;
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

// top_level = Function(identifier, bool, identifier*, instruction*)
//           | StaticVariable(identifier, bool, type, static_init*)
//           | StaticConstant(identifier, type, static_init)
struct TacTopLevel : Ast {
    AST_T type() override;
};

struct TacFunction : TacTopLevel {
    AST_T type() override;
    TacFunction() = default;
    TacFunction(TIdentifier name, bool is_glob, std::vector<TIdentifier>&& params,
        std::vector<std::unique_ptr<TacInstruction>>&& body);

    TIdentifier name;
    bool is_glob;
    std::vector<TIdentifier> params;
    std::vector<std::unique_ptr<TacInstruction>> body;
};

struct TacStaticVariable : TacTopLevel {
    AST_T type() override;
    TacStaticVariable() = default;
    TacStaticVariable(TIdentifier name, bool is_glob, std::shared_ptr<Type> static_init_type,
        std::vector<std::shared_ptr<StaticInit>>&& static_inits);

    TIdentifier name;
    bool is_glob;
    std::shared_ptr<Type> static_init_type;
    std::vector<std::shared_ptr<StaticInit>> static_inits;
};

struct TacStaticConstant : TacTopLevel {
    AST_T type() override;
    TacStaticConstant() = default;
    TacStaticConstant(
        TIdentifier name, std::shared_ptr<Type> static_init_type, std::shared_ptr<StaticInit> static_init);

    TIdentifier name;
    std::shared_ptr<Type> static_init_type;
    std::shared_ptr<StaticInit> static_init;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// AST = Program(top_level*, top_level*, top_level*)
struct TacProgram : Ast {
    AST_T type() override;
    TacProgram() = default;
    TacProgram(std::vector<std::unique_ptr<TacTopLevel>>&& static_const_toplvls,
        std::vector<std::unique_ptr<TacTopLevel>>&& static_var_toplvls,
        std::vector<std::unique_ptr<TacTopLevel>>&& fun_toplvls);

    std::vector<std::unique_ptr<TacTopLevel>> static_const_toplvls;
    std::vector<std::unique_ptr<TacTopLevel>> static_var_toplvls;
    std::vector<std::unique_ptr<TacTopLevel>> fun_toplvls;
};

/*
struct Dummy : Ast {
};
*/

#endif
