#include <memory>
#include <vector>

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend abstract syntax tree

AST_T AsmReg::type() { return AST_T::AsmReg_t; }
AST_T AsmAx::type() { return AST_T::AsmAx_t; }
AST_T AsmCx::type() { return AST_T::AsmCx_t; }
AST_T AsmDx::type() { return AST_T::AsmDx_t; }
AST_T AsmDi::type() { return AST_T::AsmDi_t; }
AST_T AsmSi::type() { return AST_T::AsmSi_t; }
AST_T AsmR8::type() { return AST_T::AsmR8_t; }
AST_T AsmR9::type() { return AST_T::AsmR9_t; }
AST_T AsmR10::type() { return AST_T::AsmR10_t; }
AST_T AsmR11::type() { return AST_T::AsmR11_t; }
AST_T AsmSp::type() { return AST_T::AsmSp_t; }
AST_T AsmBp::type() { return AST_T::AsmBp_t; }
AST_T AsmXMM0::type() { return AST_T::AsmXMM0_t; }
AST_T AsmXMM1::type() { return AST_T::AsmXMM1_t; }
AST_T AsmXMM2::type() { return AST_T::AsmXMM2_t; }
AST_T AsmXMM3::type() { return AST_T::AsmXMM3_t; }
AST_T AsmXMM4::type() { return AST_T::AsmXMM4_t; }
AST_T AsmXMM5::type() { return AST_T::AsmXMM5_t; }
AST_T AsmXMM6::type() { return AST_T::AsmXMM6_t; }
AST_T AsmXMM7::type() { return AST_T::AsmXMM7_t; }
AST_T AsmXMM14::type() { return AST_T::AsmXMM14_t; }
AST_T AsmXMM15::type() { return AST_T::AsmXMM15_t; }
AST_T AsmCondCode::type() { return AST_T::AsmCondCode_t; }
AST_T AsmE::type() { return AST_T::AsmE_t; }
AST_T AsmNE::type() { return AST_T::AsmNE_t; }
AST_T AsmG::type() { return AST_T::AsmG_t; }
AST_T AsmGE::type() { return AST_T::AsmGE_t; }
AST_T AsmL::type() { return AST_T::AsmL_t; }
AST_T AsmLE::type() { return AST_T::AsmLE_t; }
AST_T AsmA::type() { return AST_T::AsmA_t; }
AST_T AsmAE::type() { return AST_T::AsmAE_t; }
AST_T AsmB::type() { return AST_T::AsmB_t; }
AST_T AsmBE::type() { return AST_T::AsmBE_t; }
AST_T AsmP::type() { return AST_T::AsmP_t; }
AST_T AsmOperand::type() { return AST_T::AsmOperand_t; }
AST_T AsmImm::type() { return AST_T::AsmImm_t; }
AST_T AsmRegister::type() { return AST_T::AsmRegister_t; }
AST_T AsmPseudo::type() { return AST_T::AsmPseudo_t; }
AST_T AsmMemory::type() { return AST_T::AsmMemory_t; }
AST_T AsmData::type() { return AST_T::AsmData_t; }
AST_T AsmPseudoMem::type() { return AST_T::AsmPseudoMem_t; }
AST_T AsmIndexed::type() { return AST_T::AsmIndexed_t; }
AST_T AsmBinaryOp::type() { return AST_T::AsmBinaryOp_t; }
AST_T AsmAdd::type() { return AST_T::AsmAdd_t; }
AST_T AsmSub::type() { return AST_T::AsmSub_t; }
AST_T AsmMult::type() { return AST_T::AsmMult_t; }
AST_T AsmDivDouble::type() { return AST_T::AsmDivDouble_t; }
AST_T AsmBitAnd::type() { return AST_T::AsmBitAnd_t; }
AST_T AsmBitOr::type() { return AST_T::AsmBitOr_t; }
AST_T AsmBitXor::type() { return AST_T::AsmBitXor_t; }
AST_T AsmBitShiftLeft::type() { return AST_T::AsmBitShiftLeft_t; }
AST_T AsmBitShiftRight::type() { return AST_T::AsmBitShiftRight_t; }
AST_T AsmBitShrArithmetic::type() { return AST_T::AsmBitShrArithmetic_t; }
AST_T AsmUnaryOp::type() { return AST_T::AsmUnaryOp_t; }
AST_T AsmNot::type() { return AST_T::AsmNot_t; }
AST_T AsmNeg::type() { return AST_T::AsmNeg_t; }
AST_T AsmShr::type() { return AST_T::AsmShr_t; }
AST_T AsmInstruction::type() { return AST_T::AsmInstruction_t; }
AST_T AsmMov::type() { return AST_T::AsmMov_t; }
AST_T AsmMovSx::type() { return AST_T::AsmMovSx_t; }
AST_T AsmMovZeroExtend::type() { return AST_T::AsmMovZeroExtend_t; }
AST_T AsmLea::type() { return AST_T::AsmLea_t; }
AST_T AsmCvttsd2si::type() { return AST_T::AsmCvttsd2si_t; }
AST_T AsmCvtsi2sd::type() { return AST_T::AsmCvtsi2sd_t; }
AST_T AsmUnary::type() { return AST_T::AsmUnary_t; }
AST_T AsmBinary::type() { return AST_T::AsmBinary_t; }
AST_T AsmCmp::type() { return AST_T::AsmCmp_t; }
AST_T AsmIdiv::type() { return AST_T::AsmIdiv_t; }
AST_T AsmDiv::type() { return AST_T::AsmDiv_t; }
AST_T AsmCdq::type() { return AST_T::AsmCdq_t; }
AST_T AsmJmp::type() { return AST_T::AsmJmp_t; }
AST_T AsmJmpCC::type() { return AST_T::AsmJmpCC_t; }
AST_T AsmSetCC::type() { return AST_T::AsmSetCC_t; }
AST_T AsmLabel::type() { return AST_T::AsmLabel_t; }
AST_T AsmPush::type() { return AST_T::AsmPush_t; }
AST_T AsmCall::type() { return AST_T::AsmCall_t; }
AST_T AsmRet::type() { return AST_T::AsmRet_t; }
AST_T AsmTopLevel::type() { return AST_T::AsmTopLevel_t; }
AST_T AsmFunction::type() { return AST_T::AsmFunction_t; }
AST_T AsmStaticVariable::type() { return AST_T::AsmStaticVariable_t; }
AST_T AsmStaticConstant::type() { return AST_T::AsmStaticConstant_t; }
AST_T AsmProgram::type() { return AST_T::AsmProgram_t; }

AsmImm::AsmImm(TULong value, bool is_byte, bool is_quad, bool is_neg) :
    value(value), is_byte(is_byte), is_quad(is_quad), is_neg(is_neg) {}

AsmRegister::AsmRegister(std::unique_ptr<AsmReg> reg) : reg(std::move(reg)) {}

AsmPseudo::AsmPseudo(TIdentifier name) : name(name) {}

AsmMemory::AsmMemory(TLong value, std::unique_ptr<AsmReg> reg) : value(value), reg(std::move(reg)) {}

AsmData::AsmData(TIdentifier name, TLong offset) : name(name), offset(offset) {}

AsmPseudoMem::AsmPseudoMem(TIdentifier name, TLong offset) : name(name), offset(offset) {}

AsmIndexed::AsmIndexed(TLong scale, std::unique_ptr<AsmReg> reg_base, std::unique_ptr<AsmReg> reg_index) :
    scale(scale), reg_base(std::move(reg_base)), reg_index(std::move(reg_index)) {}

AsmMov::AsmMov(
    std::shared_ptr<AssemblyType> assembly_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    assembly_type(std::move(assembly_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmMovSx::AsmMovSx(std::shared_ptr<AssemblyType> assembly_type_src, std::shared_ptr<AssemblyType> assembly_type_dst,
    std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    assembly_type_src(std::move(assembly_type_src)),
    assembly_type_dst(std::move(assembly_type_dst)), src(std::move(src)), dst(std::move(dst)) {}

AsmMovZeroExtend::AsmMovZeroExtend(std::shared_ptr<AssemblyType> assembly_type_src,
    std::shared_ptr<AssemblyType> assembly_type_dst, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    assembly_type_src(std::move(assembly_type_src)),
    assembly_type_dst(std::move(assembly_type_dst)), src(std::move(src)), dst(std::move(dst)) {}

AsmLea::AsmLea(std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    src(std::move(src)), dst(std::move(dst)) {}

AsmCvttsd2si::AsmCvttsd2si(
    std::shared_ptr<AssemblyType> assembly_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    assembly_type(std::move(assembly_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmCvtsi2sd::AsmCvtsi2sd(
    std::shared_ptr<AssemblyType> assembly_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    assembly_type(std::move(assembly_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmUnary::AsmUnary(std::unique_ptr<AsmUnaryOp> unary_op, std::shared_ptr<AssemblyType> assembly_type,
    std::shared_ptr<AsmOperand> dst) :
    unary_op(std::move(unary_op)),
    assembly_type(std::move(assembly_type)), dst(std::move(dst)) {}

AsmBinary::AsmBinary(std::unique_ptr<AsmBinaryOp> binary_op, std::shared_ptr<AssemblyType> assembly_type,
    std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    binary_op(std::move(binary_op)),
    assembly_type(std::move(assembly_type)), src(std::move(src)), dst(std::move(dst)) {}

AsmCmp::AsmCmp(
    std::shared_ptr<AssemblyType> assembly_type, std::shared_ptr<AsmOperand> src, std::shared_ptr<AsmOperand> dst) :
    assembly_type(std::move(assembly_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmIdiv::AsmIdiv(std::shared_ptr<AssemblyType> assembly_type, std::shared_ptr<AsmOperand> src) :
    assembly_type(std::move(assembly_type)), src(std::move(src)) {}

AsmDiv::AsmDiv(std::shared_ptr<AssemblyType> assembly_type, std::shared_ptr<AsmOperand> src) :
    assembly_type(std::move(assembly_type)), src(std::move(src)) {}

AsmCdq::AsmCdq(std::shared_ptr<AssemblyType> assembly_type) : assembly_type(std::move(assembly_type)) {}

AsmJmp::AsmJmp(TIdentifier target) : target(target) {}

AsmJmpCC::AsmJmpCC(TIdentifier target, std::unique_ptr<AsmCondCode> cond_code) :
    target(target), cond_code(std::move(cond_code)) {}

AsmSetCC::AsmSetCC(std::unique_ptr<AsmCondCode> cond_code, std::shared_ptr<AsmOperand> dst) :
    cond_code(std::move(cond_code)), dst(std::move(dst)) {}

AsmLabel::AsmLabel(TIdentifier name) : name(name) {}

AsmPush::AsmPush(std::shared_ptr<AsmOperand> src) : src(std::move(src)) {}

AsmCall::AsmCall(TIdentifier name) : name(name) {}

AsmFunction::AsmFunction(TIdentifier name, bool is_global, bool is_return_memory,
    std::vector<std::unique_ptr<AsmInstruction>> instructions) :
    name(name),
    is_global(is_global), is_return_memory(is_return_memory), instructions(std::move(instructions)) {}

AsmStaticVariable::AsmStaticVariable(
    TIdentifier name, TInt alignment, bool is_global, std::vector<std::shared_ptr<StaticInit>> static_inits) :
    name(name),
    alignment(alignment), is_global(is_global), static_inits(std::move(static_inits)) {}

AsmStaticConstant::AsmStaticConstant(TIdentifier name, TInt alignment, std::shared_ptr<StaticInit> static_init) :
    name(name), alignment(alignment), static_init(std::move(static_init)) {}

AsmProgram::AsmProgram(std::vector<std::unique_ptr<AsmTopLevel>> static_constant_top_levels,
    std::vector<std::unique_ptr<AsmTopLevel>> top_levels) :
    static_constant_top_levels(std::move(static_constant_top_levels)),
    top_levels(std::move(top_levels)) {}
