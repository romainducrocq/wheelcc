#include "lexer/lexer.hpp"

#include <string>

int main(int, char **argv) {

    std::string filename(argv[1]);
    Lexer::lexing(filename);

    return 0;
}
