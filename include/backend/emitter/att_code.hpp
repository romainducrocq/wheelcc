#ifndef _BACKEND_EMITTER_ATT_CODE_HPP
#define _BACKEND_EMITTER_ATT_CODE_HPP

#include <ast/back_ast.hpp>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// At&t code emission

void code_emission(std::unique_ptr<AsmProgram> asm_ast, std::string&& filename);

#endif
