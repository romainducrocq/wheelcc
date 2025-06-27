#ifndef _BACK_EMITTER_GAS_CODE_H
#define _BACK_EMITTER_GAS_CODE_H

#include <memory>
#include <string>

struct AsmProgram;
struct BackEndContext;
struct FileIoContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gnu assembler code emission

int emit_gas_code(std::unique_ptr<AsmProgram>&& asm_ast, std::string&& filename, BackEndContext* backend,
    FileIoContext* fileio, IdentifierContext* identifiers);

#endif
