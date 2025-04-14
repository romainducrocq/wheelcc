#ifndef _BACKEND_ASSEMBLY_REGS_HPP
#define _BACKEND_ASSEMBLY_REGS_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registers

enum REGISTER_KIND {
    Ax,
    Bx,
    Cx,
    Dx,
    Di,
    Si,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    Sp,
    Bp,
    Xmm0,
    Xmm1,
    Xmm2,
    Xmm3,
    Xmm4,
    Xmm5,
    Xmm6,
    Xmm7,
    Xmm8,
    Xmm9,
    Xmm10,
    Xmm11,
    Xmm12,
    Xmm13,
    Xmm14,
    Xmm15
};

#define REGISTER_MASK_SIZE 26
#define REGISTER_MASK_FALSE 0ul
#define NULL_REGISTER_MASK static_cast<uint8_t>(1u) << REGISTER_MASK_SIZE

#endif
