#ifndef _FRONTEND_PARSER_LEXER_HPP
#define _FRONTEND_PARSER_LEXER_HPP

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "tokens.hpp" // frontend

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

constexpr size_t TOKEN_KIND_SIZE = TOKEN_KIND::error + 1;

struct Token {
    std::string token;
    TOKEN_KIND token_kind;
    size_t line;
};

struct LexerContext {
    LexerContext();

    std::array<std::string, TOKEN_KIND_SIZE> TOKEN_REGEXPS;
};

std::unique_ptr<std::vector<Token>> lexing(const std::string& filename, std::vector<std::string>&& includedirs);

#endif
