#include <boost/regex.hpp>
#include <memory>
#include <string>
#include <vector>

#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

static std::unique_ptr<LexerContext> context;

LexerContext::LexerContext() :
    TOKEN_REGEXPS({
        R"(<<=)", // assignment_bitshiftleft
        R"(>>=)", // assignment_bitshiftright

        R"(--)",   // unop_decrement
        R"(<<)",   // binop_bitshiftleft
        R"(>>)",   // binop_bitshiftright
        R"(&&)",   // binop_and
        R"(\|\|)", // binop_or
        R"(==)",   // binop_equalto
        R"(!=)",   // binop_notequal
        R"(<=)",   // binop_lessthanorequal
        R"(>=)",   // binop_greaterthanorequal
        R"(\+=)",  // assignment_plus
        R"(-=)",   // assignment_difference
        R"(\*=)",  // assignment_product
        R"(/=)",   // assignment_quotient
        R"(%=)",   // assignment_remainder
        R"(&=)",   // assignment_bitand
        R"(\|=)",  // assignment_bitor
        R"(\^=)",  // assignment_bitxor
        R"(->)",   // structop_pointer

        R"(//)",  // comment_singleline
        R"(/\*)", // comment_multilinestart
        R"(\*/)", // comment_multilineend

        R"(\()",           // parenthesis_open
        R"(\))",           // parenthesis_close
        R"({)",            // brace_open
        R"(})",            // brace_close
        R"(\[)",           // brackets_open
        R"(\])",           // brackets_close
        R"(;)",            // semicolon
        R"(~)",            // unop_complement
        R"(-)",            // unop_negation
        R"(!)",            // unop_not
        R"(\+)",           // binop_addition
        R"(\*)",           // binop_multiplication
        R"(/)",            // binop_division
        R"(%)",            // binop_remainder
        R"(&)",            // binop_bitand
        R"(\|)",           // binop_bitor
        R"(\^)",           // binop_bitxor
        R"(<)",            // binop_lessthan
        R"(>)",            // binop_greaterthan
        R"(=)",            // assignment_simple
        R"(\?)",           // ternary_if
        R"(:)",            // ternary_else
        R"(,)",            // separator_comma
        R"(\.(?![0-9]+))", // structop_member

        R"(#[acdefgilmnoprsuw]+\b)", // preprocessor_directive

        R"(char\b)",     // key_char
        R"(int\b)",      // key_int
        R"(long\b)",     // key_long
        R"(double\b)",   // key_double
        R"(signed\b)",   // key_signed
        R"(unsigned\b)", // key_unsigned
        R"(void\b)",     // key_void
        R"(struct\b)",   // key_struct
        R"(sizeof\b)",   // key_sizeof
        R"(return\b)",   // key_return
        R"(if\b)",       // key_if
        R"(else\b)",     // key_else
        R"(goto\b)",     // key_goto
        R"(do\b)",       // key_do
        R"(while\b)",    // key_while
        R"(for\b)",      // key_for
        R"(break\b)",    // key_break
        R"(continue\b)", // key_continue
        R"(static\b)",   // key_static
        R"(extern\b)",   // key_extern

        R"([a-zA-Z_]\w*\b)",                                                               // identifier
        R"("([^"\\\n]|\\['"\\?abfnrtv])*")",                                               // string_literal
        R"('([^'\\\n]|\\['"?\\abfnrtv])')",                                                // char_constant
        R"((([0-9]*\.[0-9]+|[0-9]+\.?)[Ee][+-]?[0-9]+|[0-9]*\.[0-9]+|[0-9]+\.)(?![\w.]))", // float_constant
        R"([0-9]+([lL][uU]|[uU][lL])(?![\w.]))",                                           // unsigned_long_constant
        R"([0-9]+[uU](?![\w.]))",                                                          // unsigned_constant
        R"([0-9]+[lL](?![\w.]))",                                                          // long_constant
        R"([0-9]+(?![\w.]))",                                                              // constant

        R"([ \n\r\t\f\v])", // skip
        R"(.)"              // error
    }) {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

static std::vector<Token> tokenize() {
    std::string regexp_string = "";
    std::string groups[TOKEN_KIND_SIZE];
    for (size_t i = 0; i < TOKEN_KIND_SIZE; ++i) {
        groups[i] = std::to_string(i);
        regexp_string += "(?<" + groups[i] + ">" + context->TOKEN_REGEXPS[i] + ")|";
    }
    regexp_string.pop_back();

    const boost::regex token_pattern(regexp_string);
    std::vector<Token> tokens;

    // https://stackoverflow.com/questions/13612837/how-to-check-which-matching-group-was-used-to-match-boost-regex
    std::string line;
    bool is_comment = false;
    for (size_t line_number = 1; read_line(line); ++line_number) {

        boost::sregex_iterator it_end;
        for (boost::sregex_iterator it_begin = boost::sregex_iterator(line.begin(), line.end(), token_pattern);
             it_begin != it_end; it_begin++) {

            size_t last_group;
            boost::smatch match = *it_begin;
            for (last_group = TOKEN_KIND_SIZE; last_group-- > 0;) {
                if (match[groups[last_group]].matched) {
                    break;
                }
            }

            if (is_comment) {
                if (last_group == TOKEN_KIND::comment_multilineend) {
                    is_comment = false;
                }
                continue;
            }
            else {
                switch (last_group) {
                    case TOKEN_KIND::error:
                    case TOKEN_KIND::comment_multilineend:
                        raise_runtime_error_at_line(
                            GET_ERROR_MESSAGE(ERROR_MESSAGE::invalid_token, match.get_last_closed_paren()),
                            line_number);
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
            Lbreak:
                break;
            Lcontinue:
                continue;
            Lpass:;
            }

            Token token = {match.get_last_closed_paren(), static_cast<TOKEN_KIND>(last_group), line_number};
            tokens.emplace_back(std::move(token));
        }
    }
    return tokens;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<std::vector<Token>> lexing(const std::string& filename) {
    file_open_read(filename);

    context = std::make_unique<LexerContext>();
    std::vector<Token> tokens = tokenize();
    context.reset();

    file_close_read();
    return std::make_unique<std::vector<Token>>(std::move(tokens));
}
