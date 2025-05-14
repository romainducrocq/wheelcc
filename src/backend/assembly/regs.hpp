#ifndef _BACK_ASSEMBLY_REGS_H
#define _BACK_ASSEMBLY_REGS_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registers

enum REGISTER_KIND {
    REG_Ax,
    REG_Bx,
    REG_Cx,
    REG_Dx,
    REG_Di,
    REG_Si,
    REG_R8,
    REG_R9,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
    REG_Sp,
    REG_Bp,
    REG_Xmm0,
    REG_Xmm1,
    REG_Xmm2,
    REG_Xmm3,
    REG_Xmm4,
    REG_Xmm5,
    REG_Xmm6,
    REG_Xmm7,
    REG_Xmm8,
    REG_Xmm9,
    REG_Xmm10,
    REG_Xmm11,
    REG_Xmm12,
    REG_Xmm13,
    REG_Xmm14,
    REG_Xmm15
};

#define REGISTER_MASK_SIZE 26
#define REGISTER_MASK_FALSE 0ul
#define NULL_REGISTER_MASK ((uint8_t)1u) << REGISTER_MASK_SIZE

#endif
