#include <memory>
#include <vector>

#include "util/c_std.hpp"

#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/interm_ast.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Intermediate abstract syntax tree

AST_T TacUnaryOp::type() { return AST_TacUnaryOp_t; }
AST_T TacComplement::type() { return AST_TacComplement_t; }
AST_T TacNegate::type() { return AST_TacNegate_t; }
AST_T TacNot::type() { return AST_TacNot_t; }
AST_T TacBinaryOp::type() { return AST_TacBinaryOp_t; }
AST_T TacAdd::type() { return AST_TacAdd_t; }
AST_T TacSubtract::type() { return AST_TacSubtract_t; }
AST_T TacMultiply::type() { return AST_TacMultiply_t; }
AST_T TacDivide::type() { return AST_TacDivide_t; }
AST_T TacRemainder::type() { return AST_TacRemainder_t; }
AST_T TacBitAnd::type() { return AST_TacBitAnd_t; }
AST_T TacBitOr::type() { return AST_TacBitOr_t; }
AST_T TacBitXor::type() { return AST_TacBitXor_t; }
AST_T TacBitShiftLeft::type() { return AST_TacBitShiftLeft_t; }
AST_T TacBitShiftRight::type() { return AST_TacBitShiftRight_t; }
AST_T TacBitShrArithmetic::type() { return AST_TacBitShrArithmetic_t; }
AST_T TacEqual::type() { return AST_TacEqual_t; }
AST_T TacNotEqual::type() { return AST_TacNotEqual_t; }
AST_T TacLessThan::type() { return AST_TacLessThan_t; }
AST_T TacLessOrEqual::type() { return AST_TacLessOrEqual_t; }
AST_T TacGreaterThan::type() { return AST_TacGreaterThan_t; }
AST_T TacGreaterOrEqual::type() { return AST_TacGreaterOrEqual_t; }
AST_T TacValue::type() { return AST_TacValue_t; }
AST_T TacConstant::type() { return AST_TacConstant_t; }
AST_T TacVariable::type() { return AST_TacVariable_t; }
AST_T TacExpResult::type() { return AST_TacExpResult_t; }
AST_T TacPlainOperand::type() { return AST_TacPlainOperand_t; }
AST_T TacDereferencedPointer::type() { return AST_TacDereferencedPointer_t; }
AST_T TacSubObject::type() { return AST_TacSubObject_t; }
AST_T TacInstruction::type() { return AST_TacInstruction_t; }
AST_T TacReturn::type() { return AST_TacReturn_t; }
AST_T TacSignExtend::type() { return AST_TacSignExtend_t; }
AST_T TacTruncate::type() { return AST_TacTruncate_t; }
AST_T TacZeroExtend::type() { return AST_TacZeroExtend_t; }
AST_T TacDoubleToInt::type() { return AST_TacDoubleToInt_t; }
AST_T TacDoubleToUInt::type() { return AST_TacDoubleToUInt_t; }
AST_T TacIntToDouble::type() { return AST_TacIntToDouble_t; }
AST_T TacUIntToDouble::type() { return AST_TacUIntToDouble_t; }
AST_T TacFunCall::type() { return AST_TacFunCall_t; }
AST_T TacUnary::type() { return AST_TacUnary_t; }
AST_T TacBinary::type() { return AST_TacBinary_t; }
AST_T TacCopy::type() { return AST_TacCopy_t; }
AST_T TacGetAddress::type() { return AST_TacGetAddress_t; }
AST_T TacLoad::type() { return AST_TacLoad_t; }
AST_T TacStore::type() { return AST_TacStore_t; }
AST_T TacAddPtr::type() { return AST_TacAddPtr_t; }
AST_T TacCopyToOffset::type() { return AST_TacCopyToOffset_t; }
AST_T TacCopyFromOffset::type() { return AST_TacCopyFromOffset_t; }
AST_T TacJump::type() { return AST_TacJump_t; }
AST_T TacJumpIfZero::type() { return AST_TacJumpIfZero_t; }
AST_T TacJumpIfNotZero::type() { return AST_TacJumpIfNotZero_t; }
AST_T TacLabel::type() { return AST_TacLabel_t; }
AST_T TacTopLevel::type() { return AST_TacTopLevel_t; }
AST_T TacFunction::type() { return AST_TacFunction_t; }
AST_T TacStaticVariable::type() { return AST_TacStaticVariable_t; }
AST_T TacStaticConstant::type() { return AST_TacStaticConstant_t; }
AST_T TacProgram::type() { return AST_TacProgram_t; }

TacConstant::TacConstant(std::shared_ptr<CConst>&& constant) : constant(std::move(constant)) {}

TacVariable::TacVariable(TIdentifier name) : name(name) {}

TacPlainOperand::TacPlainOperand(std::shared_ptr<TacValue>&& val) : val(std::move(val)) {}

TacDereferencedPointer::TacDereferencedPointer(std::shared_ptr<TacValue>&& val) : val(std::move(val)) {}

TacSubObject::TacSubObject(TIdentifier base_name, TLong offset) : base_name(base_name), offset(offset) {}

TacReturn::TacReturn(std::shared_ptr<TacValue>&& val) : val(std::move(val)) {}

TacSignExtend::TacSignExtend(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacTruncate::TacTruncate(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacZeroExtend::TacZeroExtend(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacDoubleToInt::TacDoubleToInt(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacDoubleToUInt::TacDoubleToUInt(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacIntToDouble::TacIntToDouble(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacUIntToDouble::TacUIntToDouble(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacFunCall::TacFunCall() : args(vec_new()) {}
TacFunCall::TacFunCall(TIdentifier name, vector_t(std::shared_ptr<TacValue>) * args, std::shared_ptr<TacValue>&& dst) :
    name(name), args(vec_new()), dst(std::move(dst)) {
    vec_move(args, &this->args);
}
TacFunCall::~TacFunCall() {
    for (size_t i = 0; i < vec_size(this->args); ++i) {
        this->args[i].reset();
    }
    vec_delete(this->args);
}

TacUnary::TacUnary(
    std::unique_ptr<TacUnaryOp>&& unop, std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    unop(std::move(unop)),
    src(std::move(src)), dst(std::move(dst)) {}

TacBinary::TacBinary(std::unique_ptr<TacBinaryOp>&& binop, std::shared_ptr<TacValue>&& src1,
    std::shared_ptr<TacValue>&& src2, std::shared_ptr<TacValue>&& dst) :
    binop(std::move(binop)),
    src1(std::move(src1)), src2(std::move(src2)), dst(std::move(dst)) {}

TacCopy::TacCopy(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacGetAddress::TacGetAddress(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

TacLoad::TacLoad(std::shared_ptr<TacValue>&& src_ptr, std::shared_ptr<TacValue>&& dst) :
    src_ptr(std::move(src_ptr)), dst(std::move(dst)) {}

TacStore::TacStore(std::shared_ptr<TacValue>&& src, std::shared_ptr<TacValue>&& dst_ptr) :
    src(std::move(src)), dst_ptr(std::move(dst_ptr)) {}

TacAddPtr::TacAddPtr(TLong scale, std::shared_ptr<TacValue>&& src_ptr, std::shared_ptr<TacValue>&& idx,
    std::shared_ptr<TacValue>&& dst) :
    scale(scale),
    src_ptr(std::move(src_ptr)), idx(std::move(idx)), dst(std::move(dst)) {}

TacCopyToOffset::TacCopyToOffset(TIdentifier dst_name, TLong offset, std::shared_ptr<TacValue>&& src) :
    dst_name(dst_name), offset(offset), src(std::move(src)) {}

TacCopyFromOffset::TacCopyFromOffset(TIdentifier src_name, TLong offset, std::shared_ptr<TacValue>&& dst) :
    src_name(src_name), offset(offset), dst(std::move(dst)) {}

TacJump::TacJump(TIdentifier target) : target(target) {}

TacJumpIfZero::TacJumpIfZero(TIdentifier target, std::shared_ptr<TacValue>&& condition) :
    target(target), condition(std::move(condition)) {}

TacJumpIfNotZero::TacJumpIfNotZero(TIdentifier target, std::shared_ptr<TacValue>&& condition) :
    target(target), condition(std::move(condition)) {}

TacLabel::TacLabel(TIdentifier name) : name(name) {}

TacFunction::TacFunction() : params(vec_new()) {}
TacFunction::TacFunction(TIdentifier name, bool is_glob, vector_t(TIdentifier) * params,
    std::vector<std::unique_ptr<TacInstruction>>&& body) :
    name(name),
    is_glob(is_glob), params(vec_new()), body(std::move(body)) {
    vec_move(params, &this->params);
}
TacFunction::~TacFunction() { vec_delete(this->params); }

TacStaticVariable::TacStaticVariable(TIdentifier name, bool is_glob, std::shared_ptr<Type>&& static_init_type,
    std::vector<std::shared_ptr<StaticInit>>&& static_inits) :
    name(name),
    is_glob(is_glob), static_init_type(std::move(static_init_type)), static_inits(std::move(static_inits)) {}

TacStaticConstant::TacStaticConstant(
    TIdentifier name, std::shared_ptr<Type>&& static_init_type, std::shared_ptr<StaticInit>&& static_init) :
    name(name),
    static_init_type(std::move(static_init_type)), static_init(std::move(static_init)) {}

TacProgram::TacProgram(std::vector<std::unique_ptr<TacTopLevel>>&& static_const_toplvls,
    std::vector<std::unique_ptr<TacTopLevel>>&& static_var_toplvls,
    std::vector<std::unique_ptr<TacTopLevel>>&& fun_toplvls) :
    static_const_toplvls(std::move(static_const_toplvls)),
    static_var_toplvls(std::move(static_var_toplvls)), fun_toplvls(std::move(fun_toplvls)) {}
