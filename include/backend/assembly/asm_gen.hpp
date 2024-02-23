#ifndef _BACKEND_ASSEMBLY_ASM_GEN_HPP
#define _BACKEND_ASSEMBLY_ASM_GEN_HPP

#include "ast/tac_ast.hpp"
#include "ast/asm_ast.hpp"

#include <memory>

std::unique_ptr<AsmProgram> assembly_generation(std::unique_ptr<TacProgram> tac_ast);

#endif
