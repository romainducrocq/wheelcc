#ifndef _BACK_ASSEMBLY_REGISTERS_H
#define _BACK_ASSEMBLY_REGISTERS_H

#include "util/c_std.h"

#include "ast_t.h" // ast

#include "assembly/regs.h" // backend

typedef struct AsmReg AsmReg;
typedef struct AsmOperand AsmOperand;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registers

#ifdef __cplusplus
extern "C" {
#endif
shared_ptr_t(AsmOperand) gen_register(REGISTER_KIND reg_kind);
shared_ptr_t(AsmOperand) gen_memory(REGISTER_KIND reg_kind, TLong value);
shared_ptr_t(AsmOperand) gen_indexed(REGISTER_KIND reg_kind_base, REGISTER_KIND reg_kind_idx, TLong scale);
REGISTER_KIND register_mask_kind(AsmReg* node);
size_t register_mask_bit(REGISTER_KIND reg_kind);
bool register_mask_get(TULong reg_mask, REGISTER_KIND reg_kind);
void register_mask_set(TULong* reg_mask, REGISTER_KIND reg_kind, bool value);
#ifdef __cplusplus
}
#endif

#endif
