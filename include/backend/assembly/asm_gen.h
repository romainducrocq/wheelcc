#ifndef _BACK_ASSEMBLY_ASM_GEN_H
#define _BACK_ASSEMBLY_ASM_GEN_H

#include <memory>

typedef struct TacProgram TacProgram;
typedef struct AsmProgram AsmProgram;
typedef struct FrontEndContext FrontEndContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

std::unique_ptr<AsmProgram> generate_assembly(
    std::unique_ptr<TacProgram>* tac_ast, FrontEndContext* frontend, IdentifierContext* identifiers);

#endif
