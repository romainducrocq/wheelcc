#ifndef _BACKEND_EMITTER_ATT_CODE_HPP
#define _BACKEND_EMITTER_ATT_CODE_HPP

#include <memory>
#include <string>

struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Gnu assembler code emission

void emit_gas_code(std::unique_ptr<AsmProgram> asm_ast, std::string&& filename);

#endif
