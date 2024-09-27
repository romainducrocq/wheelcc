#ifndef _FRONTEND_PARSER_LEXER_HPP
#define _FRONTEND_PARSER_LEXER_HPP

#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
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
    LexerContext(std::vector<Token>* p_tokens, std::vector<std::string>* p_includedirs);

    size_t total_line_number;
    TOKEN_KIND ctre_match_token_kind;
    std::string ctre_match_token;
    std::string_view ctre_iterator_view_substr;
    std::vector<Token>* p_tokens;
    std::vector<std::string>* p_includedirs;
    std::vector<std::string> stdlibdirs;
    std::unordered_set<std::string> filename_include_set;
};

std::unique_ptr<std::vector<Token>> lexing(std::string& filename, std::vector<std::string>&& includedirs);

#endif
