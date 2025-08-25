#ifndef _BACK_EMITTER_GAS_CODE_H
#define _BACK_EMITTER_GAS_CODE_H

#include <memory>

struct AsmProgram;
struct BackEndContext;
struct FileIoContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gnu assembler code emission

void emit_gas_code(std::unique_ptr<AsmProgram>* asm_ast, BackEndContext* backend, FileIoContext* fileio,
    IdentifierContext* identifiers);

#endif
