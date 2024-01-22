#include "util/pprint.hpp"
#include "parser/lexer.hpp"

#include <vector>
#include <iostream>

static std::string pretty_string = "";

static void header_string(const std::string& header) {
    pretty_string = "+\n+\n@@ " + header + " @@\n";
}

void pretty_print_tokens(const std::vector<Token>& tokens) {
    header_string("Tokens");
    for(size_t token = 0; token < tokens.size(); token++) {
        pretty_string += std::to_string(token) + ": (\"" + tokens[token].token + "\", "
                       + std::to_string(tokens[token].token_kind) + ")\n";
    }
    pretty_string.pop_back();
    std::cout << pretty_string << std::endl;
}
