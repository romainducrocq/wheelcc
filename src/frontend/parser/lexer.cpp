#include "ctre/ctre.hpp"
#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

static std::unique_ptr<LexerContext> context;

LexerContext::LexerContext(std::vector<Token>* p_tokens, std::vector<std::string>* p_includedirs) :
    total_line_number(0), p_tokens(p_tokens), p_includedirs(p_includedirs), stdlibdirs({
#ifdef __GNUC__
                                                                                "/usr/include/", "/usr/local/include/"
#endif
                                                                            }) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

static void tokenize_header(std::string include_match, size_t tokenize_header);

template <ctll::fixed_string regex_pattern, TOKEN_KIND token_kind> bool ctre_match_token() {
    ctre::regex_results match = ctre::starts_with<regex_pattern>(context->ctre_iterator_view_substr);
    if (match.size()) {
        context->ctre_match_token_kind = token_kind;
        context->ctre_match_token = std::string(match.template get<0>());
        return true;
    }
    else {
        return false;
    }
}
#define CTRE_MATCH_TOKEN(X, Y)      \
    if (ctre_match_token<X, Y>()) { \
        return;                     \
    }

static void ctre_match_current_token() {
    CTRE_MATCH_TOKEN(R"(<<=)", TOKEN_KIND::assignment_bitshiftleft)
    CTRE_MATCH_TOKEN(R"(>>=)", TOKEN_KIND::assignment_bitshiftright)

    CTRE_MATCH_TOKEN(R"(\+\+)", TOKEN_KIND::unop_increment)
    CTRE_MATCH_TOKEN(R"(--)", TOKEN_KIND::unop_decrement)
    CTRE_MATCH_TOKEN(R"(<<)", TOKEN_KIND::binop_bitshiftleft)
    CTRE_MATCH_TOKEN(R"(>>)", TOKEN_KIND::binop_bitshiftright)
    CTRE_MATCH_TOKEN(R"(&&)", TOKEN_KIND::binop_and)
    CTRE_MATCH_TOKEN(R"(\|\|)", TOKEN_KIND::binop_or)
    CTRE_MATCH_TOKEN(R"(==)", TOKEN_KIND::binop_equalto)
    CTRE_MATCH_TOKEN(R"(!=)", TOKEN_KIND::binop_notequal)
    CTRE_MATCH_TOKEN(R"(<=)", TOKEN_KIND::binop_lessthanorequal)
    CTRE_MATCH_TOKEN(R"(>=)", TOKEN_KIND::binop_greaterthanorequal)
    CTRE_MATCH_TOKEN(R"(\+=)", TOKEN_KIND::assignment_plus)
    CTRE_MATCH_TOKEN(R"(-=)", TOKEN_KIND::assignment_difference)
    CTRE_MATCH_TOKEN(R"(\*=)", TOKEN_KIND::assignment_product)
    CTRE_MATCH_TOKEN(R"(/=)", TOKEN_KIND::assignment_quotient)
    CTRE_MATCH_TOKEN(R"(%=)", TOKEN_KIND::assignment_remainder)
    CTRE_MATCH_TOKEN(R"(&=)", TOKEN_KIND::assignment_bitand)
    CTRE_MATCH_TOKEN(R"(\|=)", TOKEN_KIND::assignment_bitor)
    CTRE_MATCH_TOKEN(R"(\^=)", TOKEN_KIND::assignment_bitxor)
    // CTRE_MATCH_TOKEN(R"(->)", TOKEN_KIND::structop_pointer)

    CTRE_MATCH_TOKEN(R"(//)", TOKEN_KIND::comment_singleline)
    CTRE_MATCH_TOKEN(R"(/\*)", TOKEN_KIND::comment_multilinestart)
    CTRE_MATCH_TOKEN(R"(\*/)", TOKEN_KIND::comment_multilineend)

    CTRE_MATCH_TOKEN(R"(\()", TOKEN_KIND::parenthesis_open)
    CTRE_MATCH_TOKEN(R"(\))", TOKEN_KIND::parenthesis_close)
    CTRE_MATCH_TOKEN(R"(\{)", TOKEN_KIND::brace_open)
    CTRE_MATCH_TOKEN(R"(\})", TOKEN_KIND::brace_close)
    // CTRE_MATCH_TOKEN(R"(\[)", TOKEN_KIND::brackets_open)
    // CTRE_MATCH_TOKEN(R"(\])", TOKEN_KIND::brackets_close)
    CTRE_MATCH_TOKEN(R"(;)", TOKEN_KIND::semicolon)
    CTRE_MATCH_TOKEN(R"(~)", TOKEN_KIND::unop_complement)
    CTRE_MATCH_TOKEN(R"(-)", TOKEN_KIND::unop_negation)
    CTRE_MATCH_TOKEN(R"(!)", TOKEN_KIND::unop_not)
    CTRE_MATCH_TOKEN(R"(\+)", TOKEN_KIND::binop_addition)
    CTRE_MATCH_TOKEN(R"(\*)", TOKEN_KIND::binop_multiplication)
    CTRE_MATCH_TOKEN(R"(/)", TOKEN_KIND::binop_division)
    CTRE_MATCH_TOKEN(R"(%)", TOKEN_KIND::binop_remainder)
    CTRE_MATCH_TOKEN(R"(&)", TOKEN_KIND::binop_bitand)
    CTRE_MATCH_TOKEN(R"(\|)", TOKEN_KIND::binop_bitor)
    CTRE_MATCH_TOKEN(R"(\^)", TOKEN_KIND::binop_bitxor)
    CTRE_MATCH_TOKEN(R"(<)", TOKEN_KIND::binop_lessthan)
    CTRE_MATCH_TOKEN(R"(>)", TOKEN_KIND::binop_greaterthan)
    CTRE_MATCH_TOKEN(R"(=)", TOKEN_KIND::assignment_simple)
    CTRE_MATCH_TOKEN(R"(\?)", TOKEN_KIND::ternary_if)
    CTRE_MATCH_TOKEN(R"(:)", TOKEN_KIND::ternary_else)
    CTRE_MATCH_TOKEN(R"(,)", TOKEN_KIND::separator_comma)
    // CTRE_MATCH_TOKEN(R"(\.(?![0-9]+))", TOKEN_KIND::structop_member)

    // CTRE_MATCH_TOKEN(R"(char\b)", TOKEN_KIND::key_char)
    CTRE_MATCH_TOKEN(R"(int\b)", TOKEN_KIND::key_int)
    CTRE_MATCH_TOKEN(R"(long\b)", TOKEN_KIND::key_long)
    // CTRE_MATCH_TOKEN(R"(double\b)", TOKEN_KIND::key_double)
    CTRE_MATCH_TOKEN(R"(signed\b)", TOKEN_KIND::key_signed)
    CTRE_MATCH_TOKEN(R"(unsigned\b)", TOKEN_KIND::key_unsigned)
    CTRE_MATCH_TOKEN(R"(void\b)", TOKEN_KIND::key_void)
    // CTRE_MATCH_TOKEN(R"(struct\b)", TOKEN_KIND::key_struct)
    // CTRE_MATCH_TOKEN(R"(union\b)", TOKEN_KIND::key_union)
    // CTRE_MATCH_TOKEN(R"(sizeof\b)", TOKEN_KIND::key_sizeof)
    CTRE_MATCH_TOKEN(R"(return\b)", TOKEN_KIND::key_return)
    CTRE_MATCH_TOKEN(R"(if\b)", TOKEN_KIND::key_if)
    CTRE_MATCH_TOKEN(R"(else\b)", TOKEN_KIND::key_else)
    CTRE_MATCH_TOKEN(R"(goto\b)", TOKEN_KIND::key_goto)
    CTRE_MATCH_TOKEN(R"(do\b)", TOKEN_KIND::key_do)
    CTRE_MATCH_TOKEN(R"(while\b)", TOKEN_KIND::key_while)
    CTRE_MATCH_TOKEN(R"(for\b)", TOKEN_KIND::key_for)
    CTRE_MATCH_TOKEN(R"(switch\b)", TOKEN_KIND::key_switch)
    CTRE_MATCH_TOKEN(R"(case\b)", TOKEN_KIND::key_case)
    CTRE_MATCH_TOKEN(R"(default\b)", TOKEN_KIND::key_default)
    CTRE_MATCH_TOKEN(R"(break\b)", TOKEN_KIND::key_break)
    CTRE_MATCH_TOKEN(R"(continue\b)", TOKEN_KIND::key_continue)
    CTRE_MATCH_TOKEN(R"(static\b)", TOKEN_KIND::key_static)
    CTRE_MATCH_TOKEN(R"(extern\b)", TOKEN_KIND::key_extern)

    CTRE_MATCH_TOKEN(R"([a-zA-Z_]\w*\b)", TOKEN_KIND::identifier)
    // CTRE_MATCH_TOKEN(R"("([^"\\\n]|\\['"\\?abfnrtv])*")", TOKEN_KIND::string_literal)
    // CTRE_MATCH_TOKEN(R"('([^'\\\n]|\\['"?\\abfnrtv])')", TOKEN_KIND::char_constant)
    // CTRE_MATCH_TOKEN(R"((([0-9]*\.[0-9]+|[0-9]+\.?)[Ee][+-]?[0-9]+|[0-9]*\.[0-9]+|[0-9]+\.)(?![\w.]))",
    // TOKEN_KIND::float_constant)
    CTRE_MATCH_TOKEN(R"([0-9]+([lL][uU]|[uU][lL])(?![\w.]))", TOKEN_KIND::unsigned_long_constant)
    CTRE_MATCH_TOKEN(R"([0-9]+[uU](?![\w.]))", TOKEN_KIND::unsigned_constant)
    CTRE_MATCH_TOKEN(R"([0-9]+[lL](?![\w.]))", TOKEN_KIND::long_constant)
    CTRE_MATCH_TOKEN(R"([0-9]+(?![\w.]))", TOKEN_KIND::constant)

    // CTRE_MATCH_TOKEN(R"(^\s*#\s*include\b\s*(<[^/]+(/[^/]+)*\.h>|"[^/]+(/[^/]+)*\.h"))",
    // TOKEN_KIND::include_directive)
    CTRE_MATCH_TOKEN(R"(^\s*#\s*[_acdefgilmnoprstuwx]+\b)", TOKEN_KIND::preprocessor_directive)

    CTRE_MATCH_TOKEN(R"([ \n\r\t\f\v])", TOKEN_KIND::skip)

    CTRE_MATCH_TOKEN(R"(.)", TOKEN_KIND::error)
}

static void tokenize_file() {
    std::string line;
    bool is_comment = false;
    for (size_t line_number = 1; read_line(line); ++line_number) {
        context->total_line_number++;

        const std::string_view ctre_iterator_view(line);
        for (size_t i = 0; i < line.size(); i += context->ctre_match_token.size()) {
            context->ctre_iterator_view_substr = ctre_iterator_view.substr(i);
            ctre_match_current_token();

            if (is_comment) {
                if (context->ctre_match_token_kind == TOKEN_KIND::comment_multilineend) {
                    is_comment = false;
                }
                continue;
            }
            else {
                switch (context->ctre_match_token_kind) {
                    case TOKEN_KIND::error:
                    case TOKEN_KIND::comment_multilineend:
                        raise_runtime_error_at_line(
                            GET_ERROR_MESSAGE(ERROR_MESSAGE_LEXER::invalid_token, context->ctre_match_token),
                            line_number);
                    case TOKEN_KIND::skip:
                        goto Lcontinue;
                    case TOKEN_KIND::comment_multilinestart: {
                        is_comment = true;
                        goto Lcontinue;
                    }
                    case TOKEN_KIND::include_directive:
                        tokenize_header(context->ctre_match_token, line_number);
                        goto Lcontinue;
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

            Token token = {context->ctre_match_token, context->ctre_match_token_kind, context->total_line_number};
            context->p_tokens->emplace_back(std::move(token));
        }
    }
}

static bool find_header(std::vector<std::string>& dirnames, std::string& filename) {
    for (auto dirname : dirnames) {
        dirname += filename;
        if (find_file(dirname)) {
            filename = std::move(dirname);
            return true;
        }
    }
    return false;
}

static void tokenize_header(std::string filename, size_t line_number) {
    if (filename.back() == '>') {
        filename = filename.substr(filename.find('<') + 1);
        filename.pop_back();
        if (context->filename_include_set.find(filename) != context->filename_include_set.end()) {
            return;
        }
        context->filename_include_set.insert(filename);
        if (!find_header(context->stdlibdirs, filename)) {
            if (!find_header(*context->p_includedirs, filename)) {
                raise_runtime_error_at_line(
                    GET_ERROR_MESSAGE(ERROR_MESSAGE_LEXER::failed_to_include_header_file, filename), line_number);
            }
        }
    }
    else {
        filename = filename.substr(filename.find('"') + 1);
        filename.pop_back();
        if (context->filename_include_set.find(filename) != context->filename_include_set.end()) {
            return;
        }
        context->filename_include_set.insert(filename);
        if (!find_header(*context->p_includedirs, filename)) {
            raise_runtime_error_at_line(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_LEXER::failed_to_include_header_file, filename), line_number);
        }
    }

    std::string include_filename = errors->file_open_lines.back().filename;
    file_open_read(filename);
    {
        FileOpenLine file_open_line = {1, context->total_line_number + 1, std::move(filename)};
        errors->file_open_lines.emplace_back(std::move(file_open_line));
    }
    tokenize_file();
    file_close_read(line_number);
    {
        FileOpenLine file_open_line = {line_number + 1, context->total_line_number + 1, std::move(include_filename)};
        errors->file_open_lines.emplace_back(std::move(file_open_line));
    }
}

static void tokenize_source() { tokenize_file(); }

static void strip_filename_extension(std::string& filename) { filename = filename.substr(0, filename.size() - 2); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<std::vector<Token>> lexing(std::string& filename, std::vector<std::string>&& includedirs) {
    file_open_read(filename);
    {
        FileOpenLine file_open_line = {1, 1, filename};
        errors->file_open_lines.emplace_back(std::move(file_open_line));
    }

    std::vector<Token> tokens;
    context = std::make_unique<LexerContext>(&tokens, &includedirs);
    tokenize_source();
    context.reset();

    file_close_read(0);
    includedirs.clear();
    std::vector<std::string>().swap(includedirs);
    set_filename(filename);
    strip_filename_extension(filename);
    return std::make_unique<std::vector<Token>>(std::move(tokens));
}
