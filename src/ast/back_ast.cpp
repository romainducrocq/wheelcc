#include <memory>
#include <vector>

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend abstract syntax tree

AST_T AsmReg::type() { return AST_AsmReg_t; }
AST_T AsmAx::type() { return AST_AsmAx_t; }
AST_T AsmBx::type() { return AST_AsmBx_t; }
AST_T AsmCx::type() { return AST_AsmCx_t; }
AST_T AsmDx::type() { return AST_AsmDx_t; }
AST_T AsmDi::type() { return AST_AsmDi_t; }
AST_T AsmSi::type() { return AST_AsmSi_t; }
AST_T AsmR8::type() { return AST_AsmR8_t; }
AST_T AsmR9::type() { return AST_AsmR9_t; }
AST_T AsmR10::type() { return AST_AsmR10_t; }
AST_T AsmR11::type() { return AST_AsmR11_t; }
AST_T AsmR12::type() { return AST_AsmR12_t; }
AST_T AsmR13::type() { return AST_AsmR13_t; }
AST_T AsmR14::type() { return AST_AsmR14_t; }
AST_T AsmR15::type() { return AST_AsmR15_t; }
AST_T AsmSp::type() { return AST_AsmSp_t; }
AST_T AsmBp::type() { return AST_AsmBp_t; }
AST_T AsmXMM0::type() { return AST_AsmXMM0_t; }
AST_T AsmXMM1::type() { return AST_AsmXMM1_t; }
AST_T AsmXMM2::type() { return AST_AsmXMM2_t; }
AST_T AsmXMM3::type() { return AST_AsmXMM3_t; }
AST_T AsmXMM4::type() { return AST_AsmXMM4_t; }
AST_T AsmXMM5::type() { return AST_AsmXMM5_t; }
AST_T AsmXMM6::type() { return AST_AsmXMM6_t; }
AST_T AsmXMM7::type() { return AST_AsmXMM7_t; }
AST_T AsmXMM8::type() { return AST_AsmXMM8_t; }
AST_T AsmXMM9::type() { return AST_AsmXMM9_t; }
AST_T AsmXMM10::type() { return AST_AsmXMM10_t; }
AST_T AsmXMM11::type() { return AST_AsmXMM11_t; }
AST_T AsmXMM12::type() { return AST_AsmXMM12_t; }
AST_T AsmXMM13::type() { return AST_AsmXMM13_t; }
AST_T AsmXMM14::type() { return AST_AsmXMM14_t; }
AST_T AsmXMM15::type() { return AST_AsmXMM15_t; }
AST_T AsmCondCode::type() { return AST_AsmCondCode_t; }
AST_T AsmE::type() { return AST_AsmE_t; }
AST_T AsmNE::type() { return AST_AsmNE_t; }
AST_T AsmG::type() { return AST_AsmG_t; }
AST_T AsmGE::type() { return AST_AsmGE_t; }
AST_T AsmL::type() { return AST_AsmL_t; }
AST_T AsmLE::type() { return AST_AsmLE_t; }
AST_T AsmA::type() { return AST_AsmA_t; }
AST_T AsmAE::type() { return AST_AsmAE_t; }
AST_T AsmB::type() { return AST_AsmB_t; }
AST_T AsmBE::type() { return AST_AsmBE_t; }
AST_T AsmP::type() { return AST_AsmP_t; }
AST_T AsmOperand::type() { return AST_AsmOperand_t; }
AST_T AsmImm::type() { return AST_AsmImm_t; }
AST_T AsmRegister::type() { return AST_AsmRegister_t; }
AST_T AsmPseudo::type() { return AST_AsmPseudo_t; }
AST_T AsmMemory::type() { return AST_AsmMemory_t; }
AST_T AsmData::type() { return AST_AsmData_t; }
AST_T AsmPseudoMem::type() { return AST_AsmPseudoMem_t; }
AST_T AsmIndexed::type() { return AST_AsmIndexed_t; }
AST_T AsmBinaryOp::type() { return AST_AsmBinaryOp_t; }
AST_T AsmAdd::type() { return AST_AsmAdd_t; }
AST_T AsmSub::type() { return AST_AsmSub_t; }
AST_T AsmMult::type() { return AST_AsmMult_t; }
AST_T AsmDivDouble::type() { return AST_AsmDivDouble_t; }
AST_T AsmBitAnd::type() { return AST_AsmBitAnd_t; }
AST_T AsmBitOr::type() { return AST_AsmBitOr_t; }
AST_T AsmBitXor::type() { return AST_AsmBitXor_t; }
AST_T AsmBitShiftLeft::type() { return AST_AsmBitShiftLeft_t; }
AST_T AsmBitShiftRight::type() { return AST_AsmBitShiftRight_t; }
AST_T AsmBitShrArithmetic::type() { return AST_AsmBitShrArithmetic_t; }
AST_T AsmUnaryOp::type() { return AST_AsmUnaryOp_t; }
AST_T AsmNot::type() { return AST_AsmNot_t; }
AST_T AsmNeg::type() { return AST_AsmNeg_t; }
AST_T AsmShr::type() { return AST_AsmShr_t; }
AST_T AsmInstruction::type() { return AST_AsmInstruction_t; }
AST_T AsmMov::type() { return AST_AsmMov_t; }
AST_T AsmMovSx::type() { return AST_AsmMovSx_t; }
AST_T AsmMovZeroExtend::type() { return AST_AsmMovZeroExtend_t; }
AST_T AsmLea::type() { return AST_AsmLea_t; }
AST_T AsmCvttsd2si::type() { return AST_AsmCvttsd2si_t; }
AST_T AsmCvtsi2sd::type() { return AST_AsmCvtsi2sd_t; }
AST_T AsmUnary::type() { return AST_AsmUnary_t; }
AST_T AsmBinary::type() { return AST_AsmBinary_t; }
AST_T AsmCmp::type() { return AST_AsmCmp_t; }
AST_T AsmIdiv::type() { return AST_AsmIdiv_t; }
AST_T AsmDiv::type() { return AST_AsmDiv_t; }
AST_T AsmCdq::type() { return AST_AsmCdq_t; }
AST_T AsmJmp::type() { return AST_AsmJmp_t; }
AST_T AsmJmpCC::type() { return AST_AsmJmpCC_t; }
AST_T AsmSetCC::type() { return AST_AsmSetCC_t; }
AST_T AsmLabel::type() { return AST_AsmLabel_t; }
AST_T AsmPush::type() { return AST_AsmPush_t; }
AST_T AsmPop::type() { return AST_AsmPop_t; }
AST_T AsmCall::type() { return AST_AsmCall_t; }
AST_T AsmRet::type() { return AST_AsmRet_t; }
AST_T AsmTopLevel::type() { return AST_AsmTopLevel_t; }
AST_T AsmFunction::type() { return AST_AsmFunction_t; }
AST_T AsmStaticVariable::type() { return AST_AsmStaticVariable_t; }
AST_T AsmStaticConstant::type() { return AST_AsmStaticConstant_t; }
AST_T AsmProgram::type() { return AST_AsmProgram_t; }

AsmImm::AsmImm(TULong value, bool is_byte, bool is_quad, bool is_neg) :
    value(value), is_byte(is_byte), is_quad(is_quad), is_neg(is_neg) {}

AsmRegister::AsmRegister(std::unique_ptr<AsmReg>&& reg) : reg(std::move(reg)) {}

AsmPseudo::AsmPseudo(TIdentifier name) : name(name) {}

AsmMemory::AsmMemory(TLong value, std::unique_ptr<AsmReg>&& reg) : value(value), reg(std::move(reg)) {}

AsmData::AsmData(TIdentifier name, TLong offset) : name(name), offset(offset) {}

AsmPseudoMem::AsmPseudoMem(TIdentifier name, TLong offset) : name(name), offset(offset) {}

AsmIndexed::AsmIndexed(TLong scale, std::unique_ptr<AsmReg>&& reg_base, std::unique_ptr<AsmReg>&& reg_index) :
    scale(scale), reg_base(std::move(reg_base)), reg_index(std::move(reg_index)) {}

AsmMov::AsmMov(
    std::shared_ptr<AssemblyType>&& asm_type, std::shared_ptr<AsmOperand>&& src, std::shared_ptr<AsmOperand>&& dst) :
    asm_type(std::move(asm_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmMovSx::AsmMovSx(std::shared_ptr<AssemblyType>&& asm_type_src, std::shared_ptr<AssemblyType>&& asm_type_dst,
    std::shared_ptr<AsmOperand>&& src, std::shared_ptr<AsmOperand>&& dst) :
    asm_type_src(std::move(asm_type_src)),
    asm_type_dst(std::move(asm_type_dst)), src(std::move(src)), dst(std::move(dst)) {}

AsmMovZeroExtend::AsmMovZeroExtend(std::shared_ptr<AssemblyType>&& asm_type_src,
    std::shared_ptr<AssemblyType>&& asm_type_dst, std::shared_ptr<AsmOperand>&& src,
    std::shared_ptr<AsmOperand>&& dst) :
    asm_type_src(std::move(asm_type_src)),
    asm_type_dst(std::move(asm_type_dst)), src(std::move(src)), dst(std::move(dst)) {}

AsmLea::AsmLea(std::shared_ptr<AsmOperand>&& src, std::shared_ptr<AsmOperand>&& dst) :
    src(std::move(src)), dst(std::move(dst)) {}

AsmCvttsd2si::AsmCvttsd2si(
    std::shared_ptr<AssemblyType>&& asm_type, std::shared_ptr<AsmOperand>&& src, std::shared_ptr<AsmOperand>&& dst) :
    asm_type(std::move(asm_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmCvtsi2sd::AsmCvtsi2sd(
    std::shared_ptr<AssemblyType>&& asm_type, std::shared_ptr<AsmOperand>&& src, std::shared_ptr<AsmOperand>&& dst) :
    asm_type(std::move(asm_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmUnary::AsmUnary(
    std::unique_ptr<AsmUnaryOp>&& unop, std::shared_ptr<AssemblyType>&& asm_type, std::shared_ptr<AsmOperand>&& dst) :
    unop(std::move(unop)),
    asm_type(std::move(asm_type)), dst(std::move(dst)) {}

AsmBinary::AsmBinary(std::unique_ptr<AsmBinaryOp>&& binop, std::shared_ptr<AssemblyType>&& asm_type,
    std::shared_ptr<AsmOperand>&& src, std::shared_ptr<AsmOperand>&& dst) :
    binop(std::move(binop)),
    asm_type(std::move(asm_type)), src(std::move(src)), dst(std::move(dst)) {}

AsmCmp::AsmCmp(
    std::shared_ptr<AssemblyType>&& asm_type, std::shared_ptr<AsmOperand>&& src, std::shared_ptr<AsmOperand>&& dst) :
    asm_type(std::move(asm_type)),
    src(std::move(src)), dst(std::move(dst)) {}

AsmIdiv::AsmIdiv(std::shared_ptr<AssemblyType>&& asm_type, std::shared_ptr<AsmOperand>&& src) :
    asm_type(std::move(asm_type)), src(std::move(src)) {}

AsmDiv::AsmDiv(std::shared_ptr<AssemblyType>&& asm_type, std::shared_ptr<AsmOperand>&& src) :
    asm_type(std::move(asm_type)), src(std::move(src)) {}

AsmCdq::AsmCdq(std::shared_ptr<AssemblyType>&& asm_type) : asm_type(std::move(asm_type)) {}

AsmJmp::AsmJmp(TIdentifier target) : target(target) {}

AsmJmpCC::AsmJmpCC(TIdentifier target, std::unique_ptr<AsmCondCode>&& cond_code) :
    target(target), cond_code(std::move(cond_code)) {}

AsmSetCC::AsmSetCC(std::unique_ptr<AsmCondCode>&& cond_code, std::shared_ptr<AsmOperand>&& dst) :
    cond_code(std::move(cond_code)), dst(std::move(dst)) {}

AsmLabel::AsmLabel(TIdentifier name) : name(name) {}

AsmPush::AsmPush(std::shared_ptr<AsmOperand>&& src) : src(std::move(src)) {}

AsmPop::AsmPop(std::unique_ptr<AsmReg>&& reg) : reg(std::move(reg)) {}

AsmCall::AsmCall(TIdentifier name) : name(name) {}

AsmFunction::AsmFunction(
    TIdentifier name, bool is_glob, bool is_ret_memory, std::vector<std::unique_ptr<AsmInstruction>>&& instructions) :
    name(name),
    is_glob(is_glob), is_ret_memory(is_ret_memory), instructions(std::move(instructions)) {}

AsmStaticVariable::AsmStaticVariable(
    TIdentifier name, TInt alignment, bool is_glob, std::vector<std::shared_ptr<StaticInit>>&& static_inits) :
    name(name),
    alignment(alignment), is_glob(is_glob), static_inits(std::move(static_inits)) {}

AsmStaticConstant::AsmStaticConstant(TIdentifier name, TInt alignment, std::shared_ptr<StaticInit>&& static_init) :
    name(name), alignment(alignment), static_init(std::move(static_init)) {}

AsmProgram::AsmProgram(std::vector<std::unique_ptr<AsmTopLevel>>&& static_const_toplvls,
    std::vector<std::unique_ptr<AsmTopLevel>>&& top_levels) :
    static_const_toplvls(std::move(static_const_toplvls)),
    top_levels(std::move(top_levels)) {}
