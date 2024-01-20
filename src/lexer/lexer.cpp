#include "lexer/lexer.hpp"
#include "util/fopen.hpp"

#include <string>
#include <iostream>

namespace Lexer {

static void tokenize(const std::string& filename) {
    Fopen::file_open_read(filename);

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
