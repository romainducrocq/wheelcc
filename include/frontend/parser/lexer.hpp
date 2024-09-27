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

struct Token {
    TOKEN_KIND token_kind;
    std::string token;
    size_t line;
};

struct LexerContext {
    LexerContext(std::vector<Token>* p_tokens, std::vector<std::string>* p_includedirs);

    TOKEN_KIND re_match_token_kind;
    std::string re_match_token;
    std::string_view re_iterator_view_substr;
    std::vector<Token>* p_tokens;
    std::vector<std::string>* p_includedirs;
    std::vector<std::string> stdlibdirs;
    std::unordered_set<std::string> filename_include_set;
    size_t total_line_number;
};

std::unique_ptr<std::vector<Token>> lexing(std::string& filename, std::vector<std::string>&& includedirs);

#endif
