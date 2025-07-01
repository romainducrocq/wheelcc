#ifndef _FRONT_INTERMEDIATE_SEMANTIC_H
#define _FRONT_INTERMEDIATE_SEMANTIC_H

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

int analyze_semantic(CProgram* node, ErrorsContext* errors, FrontEndContext* frontend, IdentifierContext* identifiers);

#endif
