#ifndef _FRONTEND_PARSER_ERRORS_HPP
#define _FRONTEND_PARSER_ERRORS_HPP

#include <cstdio>
#include <string>

#include "frontend/parser/lexer.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

enum ERROR_MESSAGE
{
    // Main
    NO_OPTION_CODE,
    INVALID_OPTION_CODE,
    NO_INPUT_FILES,

    // Util


    //
    TEST_ERROR_0,
    TEST_ERROR_1,
    TEST_ERROR_2
};

std::string get_token_kind_hr(TOKEN_KIND token_kind);
std::string get_error_message(ERROR_MESSAGE message);
template <typename... TArgs> std::string get_error_message(ERROR_MESSAGE message, TArgs&&... args) {
    char buf[256];
    snprintf(buf, sizeof(buf), get_error_message(message).c_str(), std::forward<TArgs>(args)...);
    return std::string(buf);
}
#define GET_ERROR_MESSAGE(...) get_error_message(__VA_ARGS__)

#endif
