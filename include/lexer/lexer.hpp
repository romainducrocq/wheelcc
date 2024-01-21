#ifndef _LEXER_LEXER_HPP
#define _LEXER_LEXER_HPP

#include <string>
#include <vector>

namespace lexer
{
    enum TOKEN_KIND {
        unop_decrement,

        parenthesis_open,
        parenthesis_close,
        brace_open,
        brace_close,
        semicolon,
        unop_complement,
        unop_negation,

        key_int,
        key_void,
        key_return,

        identifier,
        constant,

        skip,
        error
    };

    struct Token {
        std::string token;
        TOKEN_KIND token_kind;

        Token(std::string&& token, TOKEN_KIND token_kind)
            : token(std::move(token)), token_kind(token_kind) {}
    };

    void lexing(const std::string& filename, std::vector<Token>& tokens);
}

#endif