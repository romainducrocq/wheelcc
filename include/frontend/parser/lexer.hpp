#ifndef _FRONTEND_PARSER_LEXER_HPP
#define _FRONTEND_PARSER_LEXER_HPP

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

std::vector<Token> lexing(std::string& filename, std::vector<std::string>&& includedirs);

#endif
