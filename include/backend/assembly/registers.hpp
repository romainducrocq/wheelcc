#ifndef _BACK_ASSEMBLY_REGISTERS_H
#define _BACK_ASSEMBLY_REGISTERS_H

#include <memory>

#include "ast_t.hpp" // ast

#include "assembly/regs.hpp" // backend

struct AsmReg;
struct AsmRegister;
struct AsmMemory;
struct AsmIndexed;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registers

std::shared_ptr<AsmRegister> gen_register(REGISTER_KIND reg_kind);
std::shared_ptr<AsmMemory> gen_memory(REGISTER_KIND reg_kind, TLong value);
std::shared_ptr<AsmIndexed> gen_indexed(REGISTER_KIND reg_kind_base, REGISTER_KIND reg_kind_idx, TLong scale);
REGISTER_KIND register_mask_kind(AsmReg* node);
size_t register_mask_bit(REGISTER_KIND reg_kind);
bool register_mask_get(TULong reg_mask, REGISTER_KIND reg_kind);
void register_mask_set(TULong& reg_mask, REGISTER_KIND reg_kind, bool value);

#endif
