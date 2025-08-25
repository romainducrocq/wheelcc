#ifndef _FRONT_PARSER_LEXER_H
#define _FRONT_PARSER_LEXER_H

#include "util/c_std.h"
#include "util/throw.h"

#include "ast_t.h" // ast

#include "parser/tokens.h" // frontend

typedef struct ErrorsContext ErrorsContext;
typedef struct FileIoContext FileIoContext;
typedef struct IdentifierContext IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

typedef struct Token {
    TOKEN_KIND tok_kind;
    TIdentifier tok;
    size_t line;
} Token;

error_t lex_c_code(const string_t filename, vector_t(const char*) * includedirs, ErrorsContext* errors,
    FileIoContext* fileio, IdentifierContext* identifiers, vector_t(Token) * tokens);

#endif
