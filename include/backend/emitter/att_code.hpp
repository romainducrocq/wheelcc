#ifndef _BACKEND_EMITTER_ATT_CODE_HPP
#define _BACKEND_EMITTER_ATT_CODE_HPP

#include <ast/asm_ast.hpp>

#include <string>
#include <memory>

void code_emission(std::unique_ptr<AsmProgram> asm_ast, std::string&& filename);

#endif
