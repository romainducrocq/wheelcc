#include "lexer/lexer.hpp"
#include "util/fopen.hpp"

#include <string>
#include <array>
#include <boost/regex.hpp>

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
        // regexp_string += "(?<" + std::to_string(i) + ">" + TOKEN_REGEX[i] + ")|";
        regexp_string += "(" + TOKEN_REGEX[i] + ")|";
    }
    regexp_string.pop_back();

    std::cout << regexp_string << std::endl;

    const boost::regex token_pattern(regexp_string);

    // https://stackoverflow.com/questions/13612837/how-to-check-which-matching-group-was-used-to-match-boost-regex
    std::string line;
    while(Fopen::read_line(line)) {

        boost::smatch match;

//            regex.assign(regex_string, boost::regex_constants::icase);
            boost::sregex_iterator  res(line.begin(), line.end(), token_pattern);
            //pattern in line above is string which I'm parsing
            boost::sregex_iterator  end;
            for(; res != end; ++res) {
                match = *res;
                std::cout <<  match.get_last_closed_paren() << std::endl;
                //I want to know if the thing that was just written to output is from group describing time string
                for(int i = 0; i < 12; i++) {
                    if(match[i+1].matched) {
                        std::cout <<  "match is: " << std::to_string(i) << std::endl;
                    }
                }

            }
//        while (boost::regex_search(start, end, what, r))
//        {
//            string stest(what[1].first, what[1].second);
//            cout << stest << endl;
//            // Update the beginning of the range to the character
//            // following the whole match
//            start = what[0].second;
//        }



        // std::cout << line << std::endl;
    }

    Fopen::file_close_read();
}

}

void Lexer::lexing(const std::string& filename) {
    tokenize(filename);
}
