#include "lexer/lexer.hpp"
#include "util/fopen.hpp"

#include <string>
#include <array>
#include <regex>

#include <iostream>

namespace Lexer {

static std::array<std::string, TOKEN_KIND::error+1> TOKEN_REGEX = {
    R"(\()",
    R"(\))",
    R"({)",
    R"(})",
    R"(;)",
    R"(int\b)",
    R"(void\b)",
    R"(return\b)",
    R"([a-zA-Z_]\w*\b)",
    R"([0-9]+(?![\w.]))",
    R"([ \n\r\t\f\v])",
    R"(.)"
};

static void tokenize(const std::string& filename) {
    Fopen::file_open_read(filename);

    std::string regexp_string = "";
    for(size_t i = 0; i < TOKEN_KIND::error+1; i++) {
        // regexp_string += "(?P<" + std::to_string(i) + ">" + TOKEN_REGEX[i] + ")|";
        regexp_string += "(?P<" + std::to_string(i) + ">" + TOKEN_REGEX[i] + ")|";
    }
    regexp_string.pop_back();

    std::cout << regexp_string << std::endl;

    // std::regex regexp_to_match(regexp_string);

    std::string line;
    while(Fopen::read_line(line)) {
        std::cout << line << std::endl;
    }

    Fopen::file_close_read();
}

}

void Lexer::lexing(const std::string& filename) {
    tokenize(filename);
}
