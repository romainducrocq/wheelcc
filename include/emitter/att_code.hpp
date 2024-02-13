#ifndef _EMITTER_ATT_CODE_HPP
#define _EMITTER_ATT_CODE_HPP

#include <ast/asm_ast.hpp>

#include <string>
#include <memory>

void code_emission(const std::string& /*filename*/, std::unique_ptr<AsmProgram> asm_ast);

#endif
