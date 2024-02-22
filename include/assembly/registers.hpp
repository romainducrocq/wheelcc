#ifndef _ASSEMBLY_REGISTERS_HPP
#define _ASSEMBLY_REGISTERS_HPP

#include "ast/asm_ast.hpp"

#include <memory>

enum REGISTER_KIND {
    Ax,
    Cx,
    Dx,
    Di,
    Si,
    R8,
    R9,
    R10,
    R11,
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
    Xmm14,
    Xmm15
};

std::shared_ptr<AsmRegister> generate_register(REGISTER_KIND register_kind);

#endif
