#ifndef _BACKEND_ASSEMBLY_ASM_GEN_HPP
#define _BACKEND_ASSEMBLY_ASM_GEN_HPP

#include <memory>

struct TacProgram;
struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

std::unique_ptr<AsmProgram> assembly_generation(std::unique_ptr<TacProgram> tac_ast);

#endif
