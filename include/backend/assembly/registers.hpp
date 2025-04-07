#ifndef _BACKEND_ASSEMBLY_REGISTERS_HPP
#define _BACKEND_ASSEMBLY_REGISTERS_HPP

#include <memory>

#include "ast/ast.hpp"

struct AsmRegister;
struct AsmMemory;
struct AsmIndexed;

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

std::shared_ptr<AsmRegister> generate_register(REGISTER_KIND register_kind);
std::shared_ptr<AsmMemory> generate_memory(REGISTER_KIND register_kind, TLong value);
std::shared_ptr<AsmIndexed> generate_indexed(
    REGISTER_KIND register_kind_base, REGISTER_KIND register_kind_index, TLong scale);
REGISTER_KIND register_mask_kind(AsmRegister* node);
size_t register_mask_bit(REGISTER_KIND register_kind);
bool register_mask_get(TULong register_mask, REGISTER_KIND register_kind);
void register_mask_set(TULong& register_mask, REGISTER_KIND register_kind, bool value);
#define REGISTER_MASK_SIZE 26
#define REGISTER_MASK_FALSE 0ul

#endif
