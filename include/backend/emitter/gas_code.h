#ifndef _BACK_EMITTER_GAS_CODE_H
#define _BACK_EMITTER_GAS_CODE_H

#include "util/c_std.h"

typedef struct AsmProgram AsmProgram;
typedef struct BackEndContext BackEndContext;
typedef struct FileIoContext FileIoContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gnu assembler code emission

#ifdef __cplusplus
extern "C" {
#endif
void emit_gas_code(
    unique_ptr_t(AsmProgram) * asm_ast, BackEndContext* backend, FileIoContext* fileio, IdentifierContext* identifiers);
#ifdef __cplusplus
}
#endif

#endif
