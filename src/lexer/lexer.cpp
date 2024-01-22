#include "lexer/lexer.hpp"
#include "util/error.hpp"
#include "util/fopen.hpp"

#include <string>
#include <array>
#include <vector>
#include <boost/regex.hpp>

constexpr int NUM_TOKEN = TOKEN_KIND::error + 1;

static std::array<std::string, NUM_TOKEN> TOKEN_REGEX = {
    R"(--)",

    R"(\()",
    R"(\))",
    R"({)",
    R"(})",
    R"(;)",
    R"(~)",
    R"(-)",

    R"(int\b)",
    R"(void\b)",
    R"(return\b)",
    R"([a-zA-Z_]\w*\b)",
    R"([0-9]+(?![\w.]))",

    R"([ \n\r\t\f\v])",
    R"(.)"
};

static void tokenize(const std::string& filename, std::vector<Token>& tokens) {
    file_open_read(filename);

    std::string groups[NUM_TOKEN];
    std::string regexp_string = "";
    for(size_t i = 0; i < NUM_TOKEN; i++) {
        groups[i] = std::to_string(i);
        regexp_string += "(?<" + groups[i] + ">" + TOKEN_REGEX[i] + ")|";
    }
    regexp_string.pop_back();
    const boost::regex token_pattern(regexp_string);

    boost::smatch match;
    boost::sregex_iterator it_begin;
    boost::sregex_iterator it_end;

    size_t last_group;

    // https://stackoverflow.com/questions/13612837/how-to-check-which-matching-group-was-used-to-match-boost-regex
    std::string line;
    while(read_line(line)) {

        for(it_begin = boost::sregex_iterator(line.begin(), line.end(), token_pattern);
            it_begin != it_end; it_begin++) {

            match = *it_begin;
            for(last_group = NUM_TOKEN; last_group-- > 0 ;) {
                if(match[groups[last_group]].matched) {
                    break;
                }
            }

            if(last_group == 0) {
                raise_runtime_error(
                        "No token found in line: " + line);
            }

            if(last_group == TOKEN_KIND::error) {
                raise_runtime_error(
                    "Invalid token \"" + match.get_last_closed_paren() + "\" found in line: " + line);
            }

            if(last_group == TOKEN_KIND::skip) {
                continue;
            }

            tokens.emplace_back(match.get_last_closed_paren(), static_cast<TOKEN_KIND>(last_group));
        }
    }

    file_close_read();
}

void lexing(const std::string& filename, std::vector<Token>& tokens) {
    tokenize(filename, tokens);
}
