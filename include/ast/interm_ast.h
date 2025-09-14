#ifndef _AST_INTERM_AST_H
#define _AST_INTERM_AST_H

#include "util/c_std.h"

#include "ast/ast.h"
#include "ast/front_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Intermediate abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct TacUnaryOp TacUnaryOp;
typedef struct TacBinaryOp TacBinaryOp;
typedef struct TacValue TacValue;
typedef struct TacExpResult TacExpResult;
typedef struct TacInstruction TacInstruction;
typedef struct TacTopLevel TacTopLevel;
typedef struct TacProgram TacProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unary_operator = Complement
//                | Negate
//                | Not

typedef struct TacUnaryOp {
    tagged_def_impl(AST_T);
} TacUnaryOp;

#define init_TacUnaryOp() tagged_def_init(AST, TacUnaryOp, TacUnaryOp)
#define init_TacComplement() tagged_def_init(AST, TacUnaryOp, TacComplement)
#define init_TacNegate() tagged_def_init(AST, TacUnaryOp, TacNegate)
#define init_TacNot() tagged_def_init(AST, TacUnaryOp, TacNot)

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

typedef struct TacAdd {
    int8_t _empty;
} TacAdd;

typedef struct TacSubtract {
    int8_t _empty;
} TacSubtract;

typedef struct TacMultiply {
    int8_t _empty;
} TacMultiply;

typedef struct TacDivide {
    int8_t _empty;
} TacDivide;

typedef struct TacRemainder {
    int8_t _empty;
} TacRemainder;

typedef struct TacBitAnd {
    int8_t _empty;
} TacBitAnd;

typedef struct TacBitOr {
    int8_t _empty;
} TacBitOr;

typedef struct TacBitXor {
    int8_t _empty;
} TacBitXor;

typedef struct TacBitShiftLeft {
    int8_t _empty;
} TacBitShiftLeft;

typedef struct TacBitShiftRight {
    int8_t _empty;
} TacBitShiftRight;

typedef struct TacBitShrArithmetic {
    int8_t _empty;
} TacBitShrArithmetic;

typedef struct TacEqual {
    int8_t _empty;
} TacEqual;

typedef struct TacNotEqual {
    int8_t _empty;
} TacNotEqual;

typedef struct TacLessThan {
    int8_t _empty;
} TacLessThan;

typedef struct TacLessOrEqual {
    int8_t _empty;
} TacLessOrEqual;

typedef struct TacGreaterThan {
    int8_t _empty;
} TacGreaterThan;

typedef struct TacGreaterOrEqual {
    int8_t _empty;
} TacGreaterOrEqual;

typedef struct TacBinaryOp {
    unique_ptr_impl(AST_T);

    union {
        TacAdd _TacAdd;
        TacSubtract _TacSubtract;
        TacMultiply _TacMultiply;
        TacDivide _TacDivide;
        TacRemainder _TacRemainder;
        TacBitAnd _TacBitAnd;
        TacBitOr _TacBitOr;
        TacBitXor _TacBitXor;
        TacBitShiftLeft _TacBitShiftLeft;
        TacBitShiftRight _TacBitShiftRight;
        TacBitShrArithmetic _TacBitShrArithmetic;
        TacEqual _TacEqual;
        TacNotEqual _TacNotEqual;
        TacLessThan _TacLessThan;
        TacLessOrEqual _TacLessOrEqual;
        TacGreaterThan _TacGreaterThan;
        TacGreaterOrEqual _TacGreaterOrEqual;
    } get;
} TacBinaryOp;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(TacBinaryOp) make_TacBinaryOp(void);
unique_ptr_t(TacBinaryOp) make_TacAdd(void);
unique_ptr_t(TacBinaryOp) make_TacSubtract(void);
unique_ptr_t(TacBinaryOp) make_TacMultiply(void);
unique_ptr_t(TacBinaryOp) make_TacDivide(void);
unique_ptr_t(TacBinaryOp) make_TacRemainder(void);
unique_ptr_t(TacBinaryOp) make_TacBitAnd(void);
unique_ptr_t(TacBinaryOp) make_TacBitOr(void);
unique_ptr_t(TacBinaryOp) make_TacBitXor(void);
unique_ptr_t(TacBinaryOp) make_TacBitShiftLeft(void);
unique_ptr_t(TacBinaryOp) make_TacBitShiftRight(void);
unique_ptr_t(TacBinaryOp) make_TacBitShrArithmetic(void);
unique_ptr_t(TacBinaryOp) make_TacEqual(void);
unique_ptr_t(TacBinaryOp) make_TacNotEqual(void);
unique_ptr_t(TacBinaryOp) make_TacLessThan(void);
unique_ptr_t(TacBinaryOp) make_TacLessOrEqual(void);
unique_ptr_t(TacBinaryOp) make_TacGreaterThan(void);
unique_ptr_t(TacBinaryOp) make_TacGreaterOrEqual(void);
void free_TacBinaryOp(unique_ptr_t(TacBinaryOp) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// val = Constant(int)
//     | Var(identifier)

typedef struct TacConstant {
    shared_ptr_t(CConst) constant;
} TacConstant;

typedef struct TacVariable {
    TIdentifier name;
} TacVariable;

typedef struct TacValue {
    shared_ptr_impl(AST_T);

    union {
        TacConstant _TacConstant;
        TacVariable _TacVariable;
    } get;
} TacValue;

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(TacValue) make_TacValue(void);
shared_ptr_t(TacValue) make_TacConstant(shared_ptr_t(CConst) * constant);
shared_ptr_t(TacValue) make_TacVariable(TIdentifier name);
void free_TacValue(shared_ptr_t(TacValue) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// exp_result = PlainOperand(val)
//            | DereferencedPointer(val)
//            | SubObject(identifier, int)

typedef struct TacPlainOperand {
    shared_ptr_t(TacValue) val;
} TacPlainOperand;

typedef struct TacDereferencedPointer {
    shared_ptr_t(TacValue) val;
} TacDereferencedPointer;

typedef struct TacSubObject {
    TIdentifier base_name;
    TLong offset;
} TacSubObject;

typedef struct TacExpResult {
    unique_ptr_impl(AST_T);

    union {
        TacPlainOperand _TacPlainOperand;
        TacDereferencedPointer _TacDereferencedPointer;
        TacSubObject _TacSubObject;
    } get;
} TacExpResult;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(TacExpResult) make_TacExpResult(void);
unique_ptr_t(TacExpResult) make_TacPlainOperand(shared_ptr_t(TacValue) * val);
unique_ptr_t(TacExpResult) make_TacDereferencedPointer(shared_ptr_t(TacValue) * val);
unique_ptr_t(TacExpResult) make_TacSubObject(TIdentifier base_name, TLong offset);
void free_TacExpResult(unique_ptr_t(TacExpResult) * self);
#ifdef __cplusplus
}
#endif

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

typedef struct TacReturn {
    shared_ptr_t(TacValue) val;
} TacReturn;

typedef struct TacSignExtend {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacSignExtend;

typedef struct TacTruncate {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacTruncate;

typedef struct TacZeroExtend {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacZeroExtend;

typedef struct TacDoubleToInt {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacDoubleToInt;

typedef struct TacDoubleToUInt {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacDoubleToUInt;

typedef struct TacIntToDouble {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacIntToDouble;

typedef struct TacUIntToDouble {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacUIntToDouble;

typedef struct TacFunCall {
    TIdentifier name;
    vector_t(shared_ptr_t(TacValue)) args;
    shared_ptr_t(TacValue) dst;
} TacFunCall;

typedef struct TacUnary {
    TacUnaryOp unop;
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacUnary;

typedef struct TacBinary {
    unique_ptr_t(TacBinaryOp) binop;
    shared_ptr_t(TacValue) src1;
    shared_ptr_t(TacValue) src2;
    shared_ptr_t(TacValue) dst;
} TacBinary;

typedef struct TacCopy {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacCopy;

typedef struct TacGetAddress {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst;
} TacGetAddress;

typedef struct TacLoad {
    shared_ptr_t(TacValue) src_ptr;
    shared_ptr_t(TacValue) dst;
} TacLoad;

typedef struct TacStore {
    shared_ptr_t(TacValue) src;
    shared_ptr_t(TacValue) dst_ptr;
} TacStore;

typedef struct TacAddPtr {
    TLong scale;
    shared_ptr_t(TacValue) src_ptr;
    shared_ptr_t(TacValue) idx;
    shared_ptr_t(TacValue) dst;
} TacAddPtr;

typedef struct TacCopyToOffset {
    TIdentifier dst_name;
    TLong offset;
    shared_ptr_t(TacValue) src;
} TacCopyToOffset;

typedef struct TacCopyFromOffset {
    TIdentifier src_name;
    TLong offset;
    shared_ptr_t(TacValue) dst;
} TacCopyFromOffset;

typedef struct TacJump {
    TIdentifier target;
} TacJump;

typedef struct TacJumpIfZero {
    TIdentifier target;
    shared_ptr_t(TacValue) condition;
} TacJumpIfZero;

typedef struct TacJumpIfNotZero {
    TIdentifier target;
    shared_ptr_t(TacValue) condition;
} TacJumpIfNotZero;

typedef struct TacLabel {
    TIdentifier name;
} TacLabel;

typedef struct TacInstruction {
    unique_ptr_impl(AST_T);

    union {
        TacReturn _TacReturn;
        TacSignExtend _TacSignExtend;
        TacTruncate _TacTruncate;
        TacZeroExtend _TacZeroExtend;
        TacDoubleToInt _TacDoubleToInt;
        TacDoubleToUInt _TacDoubleToUInt;
        TacIntToDouble _TacIntToDouble;
        TacUIntToDouble _TacUIntToDouble;
        TacFunCall _TacFunCall;
        TacUnary _TacUnary;
        TacBinary _TacBinary;
        TacCopy _TacCopy;
        TacGetAddress _TacGetAddress;
        TacLoad _TacLoad;
        TacStore _TacStore;
        TacAddPtr _TacAddPtr;
        TacCopyToOffset _TacCopyToOffset;
        TacCopyFromOffset _TacCopyFromOffset;
        TacJump _TacJump;
        TacJumpIfZero _TacJumpIfZero;
        TacJumpIfNotZero _TacJumpIfNotZero;
        TacLabel _TacLabel;
    } get;
} TacInstruction;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(TacInstruction) make_TacInstruction(void);
unique_ptr_t(TacInstruction) make_TacReturn(shared_ptr_t(TacValue) * val);
unique_ptr_t(TacInstruction) make_TacSignExtend(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacTruncate(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacZeroExtend(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacDoubleToInt(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacDoubleToUInt(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacIntToDouble(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacUIntToDouble(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction)
    make_TacFunCall(TIdentifier name, vector_t(shared_ptr_t(TacValue)) * args, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction)
    make_TacUnary(TacUnaryOp* unop, shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacBinary(unique_ptr_t(TacBinaryOp) * binop, shared_ptr_t(TacValue) * src1,
    shared_ptr_t(TacValue) * src2, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacCopy(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacGetAddress(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacLoad(shared_ptr_t(TacValue) * src_ptr, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacStore(shared_ptr_t(TacValue) * src, shared_ptr_t(TacValue) * dst_ptr);
unique_ptr_t(TacInstruction) make_TacAddPtr(
    TLong scale, shared_ptr_t(TacValue) * src_ptr, shared_ptr_t(TacValue) * idx, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacCopyToOffset(TIdentifier dst_name, TLong offset, shared_ptr_t(TacValue) * src);
unique_ptr_t(TacInstruction) make_TacCopyFromOffset(TIdentifier src_name, TLong offset, shared_ptr_t(TacValue) * dst);
unique_ptr_t(TacInstruction) make_TacJump(TIdentifier target);
unique_ptr_t(TacInstruction) make_TacJumpIfZero(TIdentifier target, shared_ptr_t(TacValue) * condition);
unique_ptr_t(TacInstruction) make_TacJumpIfNotZero(TIdentifier target, shared_ptr_t(TacValue) * condition);
unique_ptr_t(TacInstruction) make_TacLabel(TIdentifier name);
void free_TacInstruction(unique_ptr_t(TacInstruction) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// top_level = Function(identifier, bool, identifier*, instruction*)
//           | StaticVariable(identifier, bool, type, static_init*)
//           | StaticConstant(identifier, type, static_init)

typedef struct TacFunction {
    TIdentifier name;
    bool is_glob;
    vector_t(TIdentifier) params;
    vector_t(unique_ptr_t(TacInstruction)) body;
} TacFunction;

typedef struct TacStaticVariable {
    TIdentifier name;
    bool is_glob;
    shared_ptr_t(Type) static_init_type;
    vector_t(shared_ptr_t(StaticInit)) static_inits;
} TacStaticVariable;

typedef struct TacStaticConstant {
    TIdentifier name;
    shared_ptr_t(Type) static_init_type;
    shared_ptr_t(StaticInit) static_init;
} TacStaticConstant;

typedef struct TacTopLevel {
    unique_ptr_impl(AST_T);

    union {
        TacFunction _TacFunction;
        TacStaticVariable _TacStaticVariable;
        TacStaticConstant _TacStaticConstant;
    } get;
} TacTopLevel;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(TacTopLevel) make_TacTopLevel(void);
unique_ptr_t(TacTopLevel) make_TacFunction(
    TIdentifier name, bool is_glob, vector_t(TIdentifier) * params, vector_t(unique_ptr_t(TacInstruction)) * body);
unique_ptr_t(TacTopLevel) make_TacStaticVariable(TIdentifier name, bool is_glob, shared_ptr_t(Type) * static_init_type,
    vector_t(shared_ptr_t(StaticInit)) * static_inits);
unique_ptr_t(TacTopLevel) make_TacStaticConstant(
    TIdentifier name, shared_ptr_t(Type) * static_init_type, shared_ptr_t(StaticInit) * static_init);
void free_TacTopLevel(unique_ptr_t(TacTopLevel) * self);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// AST = Program(top_level*, top_level*, top_level*)

typedef struct TacProgram {
    unique_ptr_impl(AST_T);
    vector_t(unique_ptr_t(TacTopLevel)) static_const_toplvls;
    vector_t(unique_ptr_t(TacTopLevel)) static_var_toplvls;
    vector_t(unique_ptr_t(TacTopLevel)) fun_toplvls;
} TacProgram;

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(TacProgram) make_TacProgram(vector_t(unique_ptr_t(TacTopLevel)) * static_const_toplvls,
    vector_t(unique_ptr_t(TacTopLevel)) * static_var_toplvls, vector_t(unique_ptr_t(TacTopLevel)) * fun_toplvls);
void free_TacProgram(unique_ptr_t(TacProgram) * self);
#ifdef __cplusplus
}
#endif

#endif
