#include "lexer/lexer.hpp"

#include <string>
#include <vector>

#include <iostream>

int main(int, char **argv) {

    std::string filename(argv[2]);

    std::vector<lexer::Token> tokens;
    lexer::lexing(filename, tokens);

    for(const auto& token: tokens) {
        std::cout << token.token_kind << ", " << token.token << std::endl;
    }

    return 0;
}
