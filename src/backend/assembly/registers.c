#include "util/c_std.h"
#include "util/throw.h"

#include "ast/back_ast.h"

#include "backend/assembly/registers.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registers

// reg = AX | BX | CX | DX | DI | SI | R8 | R9 | R10 | R11 | R12 | R13 | R14 | R15 | SP | XMM0 | XMM1 | XMM2 | XMM3
//     | XMM4 | XMM5 | XMM6 | XMM7 | XMM8 | XMM9 | XMM10 | XMM11 | XMM12 | XMM13 | XMM14 | XMM15
static AsmReg gen_reg(REGISTER_KIND reg_kind) {
    switch (reg_kind) {
        case REG_Ax:
            return init_AsmAx();
        case REG_Bx:
            return init_AsmBx();
        case REG_Cx:
            return init_AsmCx();
        case REG_Dx:
            return init_AsmDx();
        case REG_Di:
            return init_AsmDi();
        case REG_Si:
            return init_AsmSi();
        case REG_R8:
            return init_AsmR8();
        case REG_R9:
            return init_AsmR9();
        case REG_R10:
            return init_AsmR10();
        case REG_R11:
            return init_AsmR11();
        case REG_R12:
            return init_AsmR12();
        case REG_R13:
            return init_AsmR13();
        case REG_R14:
            return init_AsmR14();
        case REG_R15:
            return init_AsmR15();
        case REG_Sp:
            return init_AsmSp();
        case REG_Bp:
            return init_AsmBp();
        case REG_Xmm0:
            return init_AsmXMM0();
        case REG_Xmm1:
            return init_AsmXMM1();
        case REG_Xmm2:
            return init_AsmXMM2();
        case REG_Xmm3:
            return init_AsmXMM3();
        case REG_Xmm4:
            return init_AsmXMM4();
        case REG_Xmm5:
            return init_AsmXMM5();
        case REG_Xmm6:
            return init_AsmXMM6();
        case REG_Xmm7:
            return init_AsmXMM7();
        case REG_Xmm8:
            return init_AsmXMM8();
        case REG_Xmm9:
            return init_AsmXMM9();
        case REG_Xmm10:
            return init_AsmXMM10();
        case REG_Xmm11:
            return init_AsmXMM11();
        case REG_Xmm12:
            return init_AsmXMM12();
        case REG_Xmm13:
            return init_AsmXMM13();
        case REG_Xmm14:
            return init_AsmXMM14();
        case REG_Xmm15:
            return init_AsmXMM15();
        default:
            THROW_ABORT;
    }
}

shared_ptr_t(AsmOperand) gen_register(REGISTER_KIND reg_kind) {
    AsmReg reg = gen_reg(reg_kind);
    return make_AsmRegister(&reg);
}

shared_ptr_t(AsmOperand) gen_memory(REGISTER_KIND reg_kind, TLong value) {
    AsmReg reg = gen_reg(reg_kind);
    return make_AsmMemory(value, &reg);
}

shared_ptr_t(AsmOperand) gen_indexed(REGISTER_KIND reg_kind_base, REGISTER_KIND reg_kind_idx, TLong scale) {
    AsmReg reg_base = gen_reg(reg_kind_base);
    AsmReg reg_index = gen_reg(reg_kind_idx);
    return make_AsmIndexed(scale, &reg_base, &reg_index);
}

REGISTER_KIND register_mask_kind(AsmReg* node) {
    switch (node->type) {
        case AST_AsmAx_t:
            return REG_Ax;
        case AST_AsmBx_t:
            return REG_Bx;
        case AST_AsmCx_t:
            return REG_Cx;
        case AST_AsmDx_t:
            return REG_Dx;
        case AST_AsmDi_t:
            return REG_Di;
        case AST_AsmSi_t:
            return REG_Si;
        case AST_AsmR8_t:
            return REG_R8;
        case AST_AsmR9_t:
            return REG_R9;
        case AST_AsmR12_t:
            return REG_R12;
        case AST_AsmR13_t:
            return REG_R13;
        case AST_AsmR14_t:
            return REG_R14;
        case AST_AsmR15_t:
            return REG_R15;
        case AST_AsmXMM0_t:
            return REG_Xmm0;
        case AST_AsmXMM1_t:
            return REG_Xmm1;
        case AST_AsmXMM2_t:
            return REG_Xmm2;
        case AST_AsmXMM3_t:
            return REG_Xmm3;
        case AST_AsmXMM4_t:
            return REG_Xmm4;
        case AST_AsmXMM5_t:
            return REG_Xmm5;
        case AST_AsmXMM6_t:
            return REG_Xmm6;
        case AST_AsmXMM7_t:
            return REG_Xmm7;
        case AST_AsmXMM8_t:
            return REG_Xmm8;
        case AST_AsmXMM9_t:
            return REG_Xmm9;
        case AST_AsmXMM10_t:
            return REG_Xmm10;
        case AST_AsmXMM11_t:
            return REG_Xmm11;
        case AST_AsmXMM12_t:
            return REG_Xmm12;
        case AST_AsmXMM13_t:
            return REG_Xmm13;
        case AST_AsmSp_t:
        case AST_AsmBp_t:
            return REG_Sp;
        case AST_AsmR10_t:
        case AST_AsmR11_t:
        case AST_AsmXMM14_t:
        case AST_AsmXMM15_t:
            THROW_ABORT;
        default:
            THROW_ABORT;
    }
}

size_t register_mask_bit(REGISTER_KIND reg_kind) {
    switch (reg_kind) {
        case REG_Ax:
            return 0;
        case REG_Bx:
            return 1;
        case REG_Cx:
            return 2;
        case REG_Dx:
            return 3;
        case REG_Di:
            return 4;
        case REG_Si:
            return 5;
        case REG_R8:
            return 6;
        case REG_R9:
            return 7;
        case REG_R12:
            return 8;
        case REG_R13:
            return 9;
        case REG_R14:
            return 10;
        case REG_R15:
            return 11;
        case REG_Xmm0:
            return 12;
        case REG_Xmm1:
            return 13;
        case REG_Xmm2:
            return 14;
        case REG_Xmm3:
            return 15;
        case REG_Xmm4:
            return 16;
        case REG_Xmm5:
            return 17;
        case REG_Xmm6:
            return 18;
        case REG_Xmm7:
            return 19;
        case REG_Xmm8:
            return 20;
        case REG_Xmm9:
            return 21;
        case REG_Xmm10:
            return 22;
        case REG_Xmm11:
            return 23;
        case REG_Xmm12:
            return 24;
        case REG_Xmm13:
            return 25;
        case REG_R10:
        case REG_R11:
        case REG_Sp:
        case REG_Bp:
        case REG_Xmm14:
        case REG_Xmm15:
            THROW_ABORT;
        default:
            THROW_ABORT;
    }
}

bool register_mask_get(TULong reg_mask, REGISTER_KIND reg_kind) {
    return (reg_mask & (((TULong)1ul) << register_mask_bit(reg_kind))) > 0;
}

void register_mask_set(TULong* reg_mask, REGISTER_KIND reg_kind, bool value) {
    if (value) {
        *reg_mask |= ((TULong)1ul) << register_mask_bit(reg_kind);
    }
    else {
        *reg_mask &= ~(((TULong)1ul) << register_mask_bit(reg_kind));
    }
}
