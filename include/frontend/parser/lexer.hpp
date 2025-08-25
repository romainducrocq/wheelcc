#ifndef _FRONT_PARSER_LEXER_H
#define _FRONT_PARSER_LEXER_H

#include "util/c_std.hpp"
#include "util/throw.hpp"

#include "ast_t.hpp" // ast

#include "parser/tokens.hpp" // frontend

struct ErrorsContext;
struct FileIoContext;
struct IdentifierContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

struct Token {
    TOKEN_KIND tok_kind;
    TIdentifier tok;
    size_t line;
};

error_t lex_c_code(const string_t filename, vector_t(const char*) * includedirs, ErrorsContext* errors,
    FileIoContext* fileio, IdentifierContext* identifiers, vector_t(Token) * tokens);

#endif
