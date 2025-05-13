#ifndef _FRONT_PARSER_LEXER_H
#define _FRONT_PARSER_LEXER_H

#include <string>
#include <vector>

#include "parser/tokens.hpp" // frontend

struct ErrorsContext;
struct FileIoContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

struct Token {
    TOKEN_KIND tok_kind;
    std::string tok;
    size_t line;
};

std::vector<Token> lex_c_code(
    std::string& filename, std::vector<std::string>&& includedirs, ErrorsContext* errors, FileIoContext* fileio);

#endif
