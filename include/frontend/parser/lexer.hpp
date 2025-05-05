#ifndef _FRONT_PARSER_LEXER_H
#define _FRONT_PARSER_LEXER_H

#include <string>
#include <vector>

#include "parser/tokens.hpp" // frontend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

struct Token {
    TOKEN_KIND token_kind;
    std::string token;
    size_t line;
};

std::vector<Token> lex_c_code(std::string& filename, std::vector<std::string>&& includedirs);

#endif
