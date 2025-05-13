#ifndef _BACK_EMITTER_GAS_CODE_H
#define _BACK_EMITTER_GAS_CODE_H

#include <memory>
#include <string>

struct AsmProgram;
struct FileIoContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gnu assembler code emission

void emit_gas_code(std::unique_ptr<AsmProgram> asm_ast, std::string&& filename, FileIoContext* fileio);

#endif
