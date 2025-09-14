#include "util/c_std.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/back_ast.h"
#include "ast/back_symt.h"
#include "ast/front_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend abstract syntax tree

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unique_ptr_t(AsmCondCode) make_AsmCondCode(void) {
    unique_ptr_t(AsmCondCode) self = uptr_new();
    uptr_alloc(AsmCondCode, self);
    self->type = AST_AsmCondCode_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmE(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmE_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmNE(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmNE_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmG(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmG_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmGE(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmGE_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmL(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmL_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmLE(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmLE_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmA(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmA_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmAE(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmAE_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmB(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmB_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmBE(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmBE_t;
    return self;
}

unique_ptr_t(AsmCondCode) make_AsmP(void) {
    unique_ptr_t(AsmCondCode) self = make_AsmCondCode();
    self->type = AST_AsmP_t;
    return self;
}

void free_AsmCondCode(unique_ptr_t(AsmCondCode) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_AsmCondCode_t:
        case AST_AsmE_t:
        case AST_AsmNE_t:
        case AST_AsmG_t:
        case AST_AsmGE_t:
        case AST_AsmL_t:
        case AST_AsmLE_t:
        case AST_AsmA_t:
        case AST_AsmAE_t:
        case AST_AsmB_t:
        case AST_AsmBE_t:
        case AST_AsmP_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

shared_ptr_t(AsmOperand) make_AsmOperand(void) {
    shared_ptr_t(AsmOperand) self = sptr_new();
    sptr_alloc(AsmOperand, self);
    self->type = AST_AsmOperand_t;
    return self;
}

shared_ptr_t(AsmOperand) make_AsmImm(TULong value, bool is_byte, bool is_quad, bool is_neg) {
    shared_ptr_t(AsmOperand) self = make_AsmOperand();
    self->type = AST_AsmImm_t;
    self->get._AsmImm.value = value;
    self->get._AsmImm.is_byte = is_byte;
    self->get._AsmImm.is_quad = is_quad;
    self->get._AsmImm.is_neg = is_neg;
    return self;
}

shared_ptr_t(AsmOperand) make_AsmRegister(AsmReg* reg) {
    shared_ptr_t(AsmOperand) self = make_AsmOperand();
    self->type = AST_AsmRegister_t;
    self->get._AsmRegister.reg = *reg;
    return self;
}

shared_ptr_t(AsmOperand) make_AsmPseudo(TIdentifier name) {
    shared_ptr_t(AsmOperand) self = make_AsmOperand();
    self->type = AST_AsmPseudo_t;
    self->get._AsmPseudo.name = name;
    return self;
}

shared_ptr_t(AsmOperand) make_AsmMemory(TLong value, AsmReg* reg) {
    shared_ptr_t(AsmOperand) self = make_AsmOperand();
    self->type = AST_AsmMemory_t;
    self->get._AsmMemory.value = value;
    self->get._AsmMemory.reg = *reg;
    return self;
}

shared_ptr_t(AsmOperand) make_AsmData(TIdentifier name, TLong offset) {
    shared_ptr_t(AsmOperand) self = make_AsmOperand();
    self->type = AST_AsmData_t;
    self->get._AsmData.name = name;
    self->get._AsmData.offset = offset;
    return self;
}

shared_ptr_t(AsmOperand) make_AsmPseudoMem(TIdentifier name, TLong offset) {
    shared_ptr_t(AsmOperand) self = make_AsmOperand();
    self->type = AST_AsmPseudoMem_t;
    self->get._AsmPseudoMem.name = name;
    self->get._AsmPseudoMem.offset = offset;
    return self;
}

shared_ptr_t(AsmOperand) make_AsmIndexed(TLong scale, AsmReg* reg_base, AsmReg* reg_index) {
    shared_ptr_t(AsmOperand) self = make_AsmOperand();
    self->type = AST_AsmIndexed_t;
    self->get._AsmIndexed.scale = scale;
    self->get._AsmIndexed.reg_base = *reg_base;
    self->get._AsmIndexed.reg_index = *reg_index;
    return self;
}

void free_AsmOperand(shared_ptr_t(AsmOperand) * self) {
    sptr_delete(*self);
    switch ((*self)->type) {
        case AST_AsmOperand_t:
        case AST_AsmImm_t:
            break;
        case AST_AsmRegister_t:
            break;
        case AST_AsmPseudo_t:
            break;
        case AST_AsmMemory_t:
            break;
        case AST_AsmData_t:
            break;
        case AST_AsmPseudoMem_t:
            break;
        case AST_AsmIndexed_t:
            break;
        default:
            THROW_ABORT;
    }
    sptr_free(*self);
}

unique_ptr_t(AsmBinaryOp) make_AsmBinaryOp(void) {
    unique_ptr_t(AsmBinaryOp) self = uptr_new();
    uptr_alloc(AsmBinaryOp, self);
    self->type = AST_AsmBinaryOp_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmAdd(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmAdd_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmSub(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmSub_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmMult(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmMult_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmDivDouble(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmDivDouble_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmBitAnd(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmBitAnd_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmBitOr(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmBitOr_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmBitXor(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmBitXor_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmBitShiftLeft(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmBitShiftLeft_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmBitShiftRight(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmBitShiftRight_t;
    return self;
}

unique_ptr_t(AsmBinaryOp) make_AsmBitShrArithmetic(void) {
    unique_ptr_t(AsmBinaryOp) self = make_AsmBinaryOp();
    self->type = AST_AsmBitShrArithmetic_t;
    return self;
}

void free_AsmBinaryOp(unique_ptr_t(AsmBinaryOp) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_AsmBinaryOp_t:
        case AST_AsmAdd_t:
        case AST_AsmSub_t:
        case AST_AsmMult_t:
        case AST_AsmDivDouble_t:
        case AST_AsmBitAnd_t:
        case AST_AsmBitOr_t:
        case AST_AsmBitXor_t:
        case AST_AsmBitShiftLeft_t:
        case AST_AsmBitShiftRight_t:
        case AST_AsmBitShrArithmetic_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(AsmUnaryOp) make_AsmUnaryOp(void) {
    unique_ptr_t(AsmUnaryOp) self = uptr_new();
    uptr_alloc(AsmUnaryOp, self);
    self->type = AST_AsmUnaryOp_t;
    return self;
}

unique_ptr_t(AsmUnaryOp) make_AsmNot(void) {
    unique_ptr_t(AsmUnaryOp) self = make_AsmUnaryOp();
    self->type = AST_AsmNot_t;
    return self;
}

unique_ptr_t(AsmUnaryOp) make_AsmNeg(void) {
    unique_ptr_t(AsmUnaryOp) self = make_AsmUnaryOp();
    self->type = AST_AsmNeg_t;
    return self;
}

unique_ptr_t(AsmUnaryOp) make_AsmShr(void) {
    unique_ptr_t(AsmUnaryOp) self = make_AsmUnaryOp();
    self->type = AST_AsmShr_t;
    return self;
}

void free_AsmUnaryOp(unique_ptr_t(AsmUnaryOp) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_AsmUnaryOp_t:
        case AST_AsmNot_t:
        case AST_AsmNeg_t:
        case AST_AsmShr_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(AsmInstruction) make_AsmInstruction(void) {
    unique_ptr_t(AsmInstruction) self = uptr_new();
    uptr_alloc(AsmInstruction, self);
    self->type = AST_AsmInstruction_t;
    return self;
}

unique_ptr_t(AsmInstruction)
    make_AsmMov(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmMov_t;
    self->get._AsmMov.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmMov.asm_type);
    self->get._AsmMov.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmMov.src);
    self->get._AsmMov.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmMov.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmMovSx(shared_ptr_t(AssemblyType) * asm_type_src,
    shared_ptr_t(AssemblyType) * asm_type_dst, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmMovSx_t;
    self->get._AsmMovSx.asm_type_src = sptr_new();
    sptr_move(AssemblyType, *asm_type_src, self->get._AsmMovSx.asm_type_src);
    self->get._AsmMovSx.asm_type_dst = sptr_new();
    sptr_move(AssemblyType, *asm_type_dst, self->get._AsmMovSx.asm_type_dst);
    self->get._AsmMovSx.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmMovSx.src);
    self->get._AsmMovSx.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmMovSx.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmMovZeroExtend(shared_ptr_t(AssemblyType) * asm_type_src,
    shared_ptr_t(AssemblyType) * asm_type_dst, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmMovZeroExtend_t;
    self->get._AsmMovZeroExtend.asm_type_src = sptr_new();
    sptr_move(AssemblyType, *asm_type_src, self->get._AsmMovZeroExtend.asm_type_src);
    self->get._AsmMovZeroExtend.asm_type_dst = sptr_new();
    sptr_move(AssemblyType, *asm_type_dst, self->get._AsmMovZeroExtend.asm_type_dst);
    self->get._AsmMovZeroExtend.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmMovZeroExtend.src);
    self->get._AsmMovZeroExtend.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmMovZeroExtend.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmLea(shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmLea_t;
    self->get._AsmLea.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmLea.src);
    self->get._AsmLea.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmLea.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmCvttsd2si(
    shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmCvttsd2si_t;
    self->get._AsmCvttsd2si.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmCvttsd2si.asm_type);
    self->get._AsmCvttsd2si.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmCvttsd2si.src);
    self->get._AsmCvttsd2si.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmCvttsd2si.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmCvtsi2sd(
    shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmCvtsi2sd_t;
    self->get._AsmCvtsi2sd.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmCvtsi2sd.asm_type);
    self->get._AsmCvtsi2sd.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmCvtsi2sd.src);
    self->get._AsmCvtsi2sd.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmCvtsi2sd.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmUnary(
    unique_ptr_t(AsmUnaryOp) * unop, shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmUnary_t;
    self->get._AsmUnary.unop = uptr_new();
    uptr_move(AsmUnaryOp, *unop, self->get._AsmUnary.unop);
    self->get._AsmUnary.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmUnary.asm_type);
    self->get._AsmUnary.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmUnary.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmBinary(unique_ptr_t(AsmBinaryOp) * binop, shared_ptr_t(AssemblyType) * asm_type,
    shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmBinary_t;
    self->get._AsmBinary.binop = uptr_new();
    uptr_move(AsmBinaryOp, *binop, self->get._AsmBinary.binop);
    self->get._AsmBinary.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmBinary.asm_type);
    self->get._AsmBinary.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmBinary.src);
    self->get._AsmBinary.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmBinary.dst);
    return self;
}

unique_ptr_t(AsmInstruction)
    make_AsmCmp(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmCmp_t;
    self->get._AsmCmp.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmCmp.asm_type);
    self->get._AsmCmp.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmCmp.src);
    self->get._AsmCmp.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmCmp.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmIdiv(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmIdiv_t;
    self->get._AsmIdiv.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmIdiv.asm_type);
    self->get._AsmIdiv.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmIdiv.src);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmDiv(shared_ptr_t(AssemblyType) * asm_type, shared_ptr_t(AsmOperand) * src) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmDiv_t;
    self->get._AsmDiv.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmDiv.asm_type);
    self->get._AsmDiv.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmDiv.src);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmCdq(shared_ptr_t(AssemblyType) * asm_type) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmCdq_t;
    self->get._AsmCdq.asm_type = sptr_new();
    sptr_move(AssemblyType, *asm_type, self->get._AsmCdq.asm_type);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmJmp(TIdentifier target) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmJmp_t;
    self->get._AsmJmp.target = target;
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmJmpCC(TIdentifier target, unique_ptr_t(AsmCondCode) * cond_code) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmJmpCC_t;
    self->get._AsmJmpCC.target = target;
    self->get._AsmJmpCC.cond_code = uptr_new();
    uptr_move(AsmCondCode, *cond_code, self->get._AsmJmpCC.cond_code);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmSetCC(unique_ptr_t(AsmCondCode) * cond_code, shared_ptr_t(AsmOperand) * dst) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmSetCC_t;
    self->get._AsmSetCC.cond_code = uptr_new();
    uptr_move(AsmCondCode, *cond_code, self->get._AsmSetCC.cond_code);
    self->get._AsmSetCC.dst = sptr_new();
    sptr_move(AsmOperand, *dst, self->get._AsmSetCC.dst);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmLabel(TIdentifier name) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmLabel_t;
    self->get._AsmLabel.name = name;
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmPush(shared_ptr_t(AsmOperand) * src) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmPush_t;
    self->get._AsmPush.src = sptr_new();
    sptr_move(AsmOperand, *src, self->get._AsmPush.src);
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmPop(AsmReg* reg) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmPop_t;
    self->get._AsmPop.reg = *reg;
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmCall(TIdentifier name) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmCall_t;
    self->get._AsmCall.name = name;
    return self;
}

unique_ptr_t(AsmInstruction) make_AsmRet(void) {
    unique_ptr_t(AsmInstruction) self = make_AsmInstruction();
    self->type = AST_AsmRet_t;
    return self;
}

void free_AsmInstruction(unique_ptr_t(AsmInstruction) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_AsmInstruction_t:
            break;
        case AST_AsmMov_t:
            free_AssemblyType(&(*self)->get._AsmMov.asm_type);
            free_AsmOperand(&(*self)->get._AsmMov.src);
            free_AsmOperand(&(*self)->get._AsmMov.dst);
            break;
        case AST_AsmMovSx_t:
            free_AssemblyType(&(*self)->get._AsmMovSx.asm_type_src);
            free_AssemblyType(&(*self)->get._AsmMovSx.asm_type_dst);
            free_AsmOperand(&(*self)->get._AsmMovSx.src);
            free_AsmOperand(&(*self)->get._AsmMovSx.dst);
            break;
        case AST_AsmMovZeroExtend_t:
            free_AssemblyType(&(*self)->get._AsmMovZeroExtend.asm_type_src);
            free_AssemblyType(&(*self)->get._AsmMovZeroExtend.asm_type_dst);
            free_AsmOperand(&(*self)->get._AsmMovZeroExtend.src);
            free_AsmOperand(&(*self)->get._AsmMovZeroExtend.dst);
            break;
        case AST_AsmLea_t:
            free_AsmOperand(&(*self)->get._AsmLea.src);
            free_AsmOperand(&(*self)->get._AsmLea.dst);
            break;
        case AST_AsmCvttsd2si_t:
            free_AssemblyType(&(*self)->get._AsmCvttsd2si.asm_type);
            free_AsmOperand(&(*self)->get._AsmCvttsd2si.src);
            free_AsmOperand(&(*self)->get._AsmCvttsd2si.dst);
            break;
        case AST_AsmCvtsi2sd_t:
            free_AssemblyType(&(*self)->get._AsmCvtsi2sd.asm_type);
            free_AsmOperand(&(*self)->get._AsmCvtsi2sd.src);
            free_AsmOperand(&(*self)->get._AsmCvtsi2sd.dst);
            break;
        case AST_AsmUnary_t:
            free_AsmUnaryOp(&(*self)->get._AsmUnary.unop);
            free_AssemblyType(&(*self)->get._AsmUnary.asm_type);
            free_AsmOperand(&(*self)->get._AsmUnary.dst);
            break;
        case AST_AsmBinary_t:
            free_AsmBinaryOp(&(*self)->get._AsmBinary.binop);
            free_AssemblyType(&(*self)->get._AsmBinary.asm_type);
            free_AsmOperand(&(*self)->get._AsmBinary.src);
            free_AsmOperand(&(*self)->get._AsmBinary.dst);
            break;
        case AST_AsmCmp_t:
            free_AssemblyType(&(*self)->get._AsmCmp.asm_type);
            free_AsmOperand(&(*self)->get._AsmCmp.src);
            free_AsmOperand(&(*self)->get._AsmCmp.dst);
            break;
        case AST_AsmIdiv_t:
            free_AssemblyType(&(*self)->get._AsmIdiv.asm_type);
            free_AsmOperand(&(*self)->get._AsmIdiv.src);
            break;
        case AST_AsmDiv_t:
            free_AssemblyType(&(*self)->get._AsmDiv.asm_type);
            free_AsmOperand(&(*self)->get._AsmDiv.src);
            break;
        case AST_AsmCdq_t:
            free_AssemblyType(&(*self)->get._AsmCdq.asm_type);
            break;
        case AST_AsmJmp_t:
            break;
        case AST_AsmJmpCC_t:
            free_AsmCondCode(&(*self)->get._AsmJmpCC.cond_code);
            break;
        case AST_AsmSetCC_t:
            free_AsmCondCode(&(*self)->get._AsmSetCC.cond_code);
            free_AsmOperand(&(*self)->get._AsmSetCC.dst);
            break;
        case AST_AsmLabel_t:
            break;
        case AST_AsmPush_t:
            free_AsmOperand(&(*self)->get._AsmPush.src);
            break;
        case AST_AsmPop_t:
            break;
        case AST_AsmCall_t:
            break;
        case AST_AsmRet_t:
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(AsmTopLevel) make_AsmTopLevel(void) {
    unique_ptr_t(AsmTopLevel) self = uptr_new();
    uptr_alloc(AsmTopLevel, self);
    self->type = AST_AsmTopLevel_t;
    return self;
}

unique_ptr_t(AsmTopLevel) make_AsmFunction(
    TIdentifier name, bool is_glob, bool is_ret_memory, vector_t(unique_ptr_t(AsmInstruction)) * instructions) {
    unique_ptr_t(AsmTopLevel) self = make_AsmTopLevel();
    self->type = AST_AsmFunction_t;
    self->get._AsmFunction.name = name;
    self->get._AsmFunction.is_glob = is_glob;
    self->get._AsmFunction.is_ret_memory = is_ret_memory;
    self->get._AsmFunction.instructions = vec_new();
    vec_move(*instructions, self->get._AsmFunction.instructions);
    return self;
}

unique_ptr_t(AsmTopLevel) make_AsmStaticVariable(
    TIdentifier name, TInt alignment, bool is_glob, vector_t(shared_ptr_t(StaticInit)) * static_inits) {
    unique_ptr_t(AsmTopLevel) self = make_AsmTopLevel();
    self->type = AST_AsmStaticVariable_t;
    self->get._AsmStaticVariable.name = name;
    self->get._AsmStaticVariable.alignment = alignment;
    self->get._AsmStaticVariable.is_glob = is_glob;
    self->get._AsmStaticVariable.static_inits = vec_new();
    vec_move(*static_inits, self->get._AsmStaticVariable.static_inits);
    return self;
}

unique_ptr_t(AsmTopLevel)
    make_AsmStaticConstant(TIdentifier name, TInt alignment, shared_ptr_t(StaticInit) * static_init) {
    unique_ptr_t(AsmTopLevel) self = make_AsmTopLevel();
    self->type = AST_AsmStaticConstant_t;
    self->get._AsmStaticConstant.name = name;
    self->get._AsmStaticConstant.alignment = alignment;
    self->get._AsmStaticConstant.static_init = sptr_new();
    sptr_move(StaticInit, *static_init, self->get._AsmStaticConstant.static_init);
    return self;
}

void free_AsmTopLevel(unique_ptr_t(AsmTopLevel) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_AsmTopLevel_t:
            break;
        case AST_AsmFunction_t:
            for (size_t i = 0; i < vec_size((*self)->get._AsmFunction.instructions); ++i) {
                free_AsmInstruction(&(*self)->get._AsmFunction.instructions[i]);
            }
            vec_delete((*self)->get._AsmFunction.instructions);
            break;
        case AST_AsmStaticVariable_t:
            for (size_t i = 0; i < vec_size((*self)->get._AsmStaticVariable.static_inits); ++i) {
                free_StaticInit(&(*self)->get._AsmStaticVariable.static_inits[i]);
            }
            vec_delete((*self)->get._AsmStaticVariable.static_inits);
            break;
        case AST_AsmStaticConstant_t:
            free_StaticInit(&(*self)->get._AsmStaticConstant.static_init);
            break;
        default:
            THROW_ABORT;
    }
    uptr_free(*self);
}

unique_ptr_t(AsmProgram) make_AsmProgram(
    vector_t(unique_ptr_t(AsmTopLevel)) * static_const_toplvls, vector_t(unique_ptr_t(AsmTopLevel)) * top_levels) {
    unique_ptr_t(AsmProgram) self = uptr_new();
    uptr_alloc(AsmProgram, self);
    self->type = AST_AsmProgram_t;
    self->static_const_toplvls = vec_new();
    vec_move(*static_const_toplvls, self->static_const_toplvls);
    self->top_levels = vec_new();
    vec_move(*top_levels, self->top_levels);
    return self;
}

void free_AsmProgram(unique_ptr_t(AsmProgram) * self) {
    uptr_delete(*self);
    switch ((*self)->type) {
        case AST_AsmProgram_t:
            break;
        default:
            THROW_ABORT;
    }
    for (size_t i = 0; i < vec_size((*self)->static_const_toplvls); ++i) {
        free_AsmTopLevel(&(*self)->static_const_toplvls[i]);
    }
    vec_delete((*self)->static_const_toplvls);
    for (size_t i = 0; i < vec_size((*self)->top_levels); ++i) {
        free_AsmTopLevel(&(*self)->top_levels[i]);
    }
    vec_delete((*self)->top_levels);
    uptr_free(*self);
}
