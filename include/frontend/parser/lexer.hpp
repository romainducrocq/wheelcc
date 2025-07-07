#ifndef _FRONT_PARSER_LEXER_H
#define _FRONT_PARSER_LEXER_H

#include <string>
#include <vector>

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

int lex_c_code(char* filename, std::vector<const char*>&& includedirs, ErrorsContext* errors, FileIoContext* fileio,
    IdentifierContext* identifiers, std::vector<Token>* tokens);

#endif
