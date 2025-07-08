#ifndef _FRONT_INTERMEDIATE_SEMANTIC_H
#define _FRONT_INTERMEDIATE_SEMANTIC_H

#include "util/throw.hpp"

struct CProgram;
struct ErrorsContext;
struct FrontEndContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Semantic analysis

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Type checking
// Loop labeling
// Identifier resolution

error_t analyze_semantic(
    CProgram* node, ErrorsContext* errors, FrontEndContext* frontend, IdentifierContext* identifiers);

#endif
