#ifndef _BACK_ASSEMBLY_ASM_GEN_H
#define _BACK_ASSEMBLY_ASM_GEN_H

#include <memory>

struct TacProgram;
struct AsmProgram;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

std::unique_ptr<AsmProgram> generate_assembly(std::unique_ptr<TacProgram> tac_ast);

#endif
