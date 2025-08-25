#ifndef _FRONT_PARSER_PARSER_H
#define _FRONT_PARSER_PARSER_H

#include <memory>

#include "util/c_std.h"
#include "util/throw.h"

typedef struct Token Token;
typedef struct CProgram CProgram;
typedef struct ErrorsContext ErrorsContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Parser

#ifdef __cplusplus
extern "C" {
#endif
error_t parse_tokens(
    vector_t(Token) * tokens, ErrorsContext* errors, IdentifierContext* identifiers, std::unique_ptr<CProgram>* c_ast);
#ifdef __cplusplus
}
#endif

#endif
