#ifndef _FRONT_INTERMEDIATE_SEMANTIC_H
#define _FRONT_INTERMEDIATE_SEMANTIC_H

#include "util/throw.hpp"

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

error_t analyze_semantic(
    CProgram* node, ErrorsContext* errors, FrontEndContext* frontend, IdentifierContext* identifiers);

#endif
