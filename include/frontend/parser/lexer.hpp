#ifndef _FRONTEND_PARSER_LEXER_HPP
#define _FRONTEND_PARSER_LEXER_HPP

#include "boost/regex.hpp"
#include <array>
#include <memory>
#include <string>
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
    std::vector<Token>* p_tokens;
    std::vector<std::string>* p_includedirs;
    std::vector<std::string> stdlibdirs;
    std::array<std::string, TOKEN_KIND_SIZE> TOKEN_REGEXPS;
    std::string token_groups[TOKEN_KIND_SIZE];
    std::unique_ptr<const boost::regex> token_pattern;
    std::unordered_set<std::string> filename_include_set;
};

std::unique_ptr<std::vector<Token>> lexing(std::string& filename, std::vector<std::string>&& includedirs);

#endif
