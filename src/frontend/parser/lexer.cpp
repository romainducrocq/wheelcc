#include "frontend/parser/lexer.hpp"
#include "util/error.hpp"
#include "util/fopen.hpp"

#include <string>
#include <array>
#include <vector>
#include <boost/regex.hpp>

constexpr size_t NUM_TOKEN = TOKEN_KIND::error + 1;

static std::array<std::string, NUM_TOKEN> TOKEN_REGEX = {
    R"(<<=)",
    R"(>>=)",

    R"(--)",
    R"(<<)",
    R"(>>)",
    R"(&&)",
    R"(\|\|)",
    R"(==)",
    R"(!=)",
    R"(<=)",
    R"(>=)",
    R"(\+=)",
    R"(-=)",
    R"(\*=)",
    R"(/=)",
    R"(%=)",
    R"(&=)",
    R"(\|=)",
    R"(\^=)",

    R"(//)",
    R"(/\*)",
    R"(\*/)",

    R"(\()",
    R"(\))",
    R"({)",
    R"(})",
    R"(;)",
    R"(~)",
    R"(-)",
    R"(!)",
    R"(\+)",
    R"(\*)",
    R"(/)",
    R"(%)",
    R"(&)",
    R"(\|)",
    R"(\^)",
    R"(<)",
    R"(>)",
    R"(=)",
    R"(\?)",
    R"(:)",
    R"(,)",

    R"(#[acdefgilmnoprsuw]+\b)",

    R"(int\b)",
    R"(long\b)",
    R"(double\b)",
    R"(signed\b)",
    R"(unsigned\b)",
    R"(void\b)",
    R"(return\b)",
    R"(if\b)",
    R"(else\b)",
    R"(goto\b)",
    R"(do\b)",
    R"(while\b)",
    R"(for\b)",
    R"(break\b)",
    R"(continue\b)",
    R"(static\b)",
    R"(extern\b)",

    R"([a-zA-Z_]\w*\b)",
    R"((([0-9]*\.[0-9]+|[0-9]+\.?)[Ee][+-]?[0-9]+|[0-9]*\.[0-9]+|[0-9]+\.)(?![\w.]))",
    R"([0-9]+([lL][uU]|[uU][lL])(?![\w.]))",
    R"([0-9]+[uU](?![\w.]))",
    R"([0-9]+[lL](?![\w.]))",
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

    // https://stackoverflow.com/questions/13612837/how-to-check-which-matching-group-was-used-to-match-boost-regex
    std::string line;
    bool is_comment = false;
    while(read_line(line)) {

        boost::sregex_iterator it_end;
        for(boost::sregex_iterator it_begin = boost::sregex_iterator(line.begin(), line.end(), token_pattern);
            it_begin != it_end; it_begin++) {

            size_t last_group;
            boost::smatch match = *it_begin;
            for(last_group = NUM_TOKEN; last_group-- > 0 ;) {
                if(match[groups[last_group]].matched) {
                    break;
                }
            }

            if(is_comment) {
                if(last_group == TOKEN_KIND::comment_multilineend) {
                    is_comment = false;
                }
                continue;
            }
            else {
                switch(last_group) {
                    case TOKEN_KIND::error:
                    case TOKEN_KIND::comment_multilineend:
                        raise_runtime_error_at_line("Found invalid token " +
                                                    em(match.get_last_closed_paren()),
                                                    get_line_number());
                    case TOKEN_KIND::skip:
                        goto Lcontinue;
                    case TOKEN_KIND::comment_multilinestart: {
                        is_comment = true;
                        goto Lcontinue;
                    }
                    case TOKEN_KIND::comment_singleline:
                    case TOKEN_KIND::preprocessor_directive:
                        goto Lbreak;
                    default:
                        goto Lpass;
                }
                Lbreak: break;
                Lcontinue: continue;
                Lpass: ;
            }

            Token token = { match.get_last_closed_paren(), static_cast<TOKEN_KIND>(last_group),
                            get_line_number() };
            tokens.emplace_back(std::move(token));
        }
    }

    file_close_read();
}

std::vector<Token> lexing(const std::string& filename) {
    std::vector<Token> tokens;
    tokenize(filename, tokens);
    return tokens;
}
