#ifndef _FRONTEND_PARSER_LEXER_HPP
#define _FRONTEND_PARSER_LEXER_HPP

#ifdef __GNUC__
#if __cplusplus > 201703L
#define __WITH_CTRE__
#endif
#endif

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "tokens.hpp" // frontend

#ifdef __WITH_CTRE__
#include <string_view>
#else
#include "boost/regex.hpp"
#endif

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
#ifdef __WITH_CTRE__
    std::string_view re_iterator_view_slice;
#else
    std::string re_capture_groups[TOKEN_KIND::error + 1];
    std::unique_ptr<const boost::regex> re_compiled_pattern;
#endif
    std::vector<Token>* p_tokens;
    std::vector<std::string>* p_includedirs;
    std::vector<std::string> stdlibdirs;
    std::unordered_set<std::string> filename_include_set;
    size_t total_line_number;
};

std::unique_ptr<std::vector<Token>> lexing(std::string& filename, std::vector<std::string>&& includedirs);

#endif
