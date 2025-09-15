#ifndef _FRONT_INTERMEDIATE_SEMANTIC_H
#define _FRONT_INTERMEDIATE_SEMANTIC_H

#include "util/throw.h"

typedef struct CProgram CProgram;
typedef struct ErrorsContext ErrorsContext;
typedef struct FrontEndContext FrontEndContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Type checking
// Loop labeling
// Identifier resolution

#ifdef __cplusplus
extern "C" {
#endif
error_t analyze_semantic(
    const CProgram* node, ErrorsContext* errors, FrontEndContext* frontend, IdentifierContext* identifiers);
#ifdef __cplusplus
}
#endif

#endif
