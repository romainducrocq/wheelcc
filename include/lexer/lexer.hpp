#ifndef _LEXER_LEXER_HPP
#define _LEXER_LEXER_HPP

#include <string>

namespace Lexer
{
    enum TOKEN_KIND {
        parenthesis_open,
        parenthesis_close,
        brace_open,
        brace_close,
        semicolon,

        key_int,
        key_void,
        key_return,

        identifier,
        constant,

        skip,
        error
    };

    void lexing(const std::string& filename);
}

#endif