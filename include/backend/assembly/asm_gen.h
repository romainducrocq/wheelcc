#ifndef _BACK_ASSEMBLY_ASM_GEN_H
#define _BACK_ASSEMBLY_ASM_GEN_H

#include "util/c_std.h"

typedef struct TacProgram TacProgram;
typedef struct AsmProgram AsmProgram;
typedef struct FrontEndContext FrontEndContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Assembly generation

#ifdef __cplusplus
extern "C" {
#endif
unique_ptr_t(AsmProgram) generate_assembly(
    unique_ptr_t(TacProgram)* tac_ast, FrontEndContext* frontend, IdentifierContext* identifiers);
#ifdef __cplusplus
}
#endif

#endif
