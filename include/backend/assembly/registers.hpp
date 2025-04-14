#ifndef _BACKEND_ASSEMBLY_REGISTERS_HPP
#define _BACKEND_ASSEMBLY_REGISTERS_HPP

#include <memory>

#include "ast/ast.hpp"

#include "assembly/regs.hpp" // backend

struct AsmReg;
struct AsmRegister;
struct AsmMemory;
struct AsmIndexed;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registers

std::shared_ptr<AsmRegister> generate_register(REGISTER_KIND register_kind);
std::shared_ptr<AsmMemory> generate_memory(REGISTER_KIND register_kind, TLong value);
std::shared_ptr<AsmIndexed> generate_indexed(
    REGISTER_KIND register_kind_base, REGISTER_KIND register_kind_index, TLong scale);
REGISTER_KIND register_mask_kind(AsmReg* node);
size_t register_mask_bit(REGISTER_KIND register_kind);
bool register_mask_get(TULong register_mask, REGISTER_KIND register_kind);
void register_mask_set(TULong& register_mask, REGISTER_KIND register_kind, bool value);

#endif
