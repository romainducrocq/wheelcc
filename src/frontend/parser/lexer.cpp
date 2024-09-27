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

// // TODO make MATCH_PATTERN a macro
// static constexpr ctll::fixed_string MATCH_PATTERN(
//     // R"((<<=)|)" // assignment_bitshiftleft
//     // R"((>>=)|)" // assignment_bitshiftright

//     // R"((\+\+)|)" // unop_increment
//     // R"((--)|)"   // unop_decrement
//     // R"((<<)|)"   // binop_bitshiftleft
//     // R"((>>)|)"   // binop_bitshiftright
//     // R"((&&)|)"   // binop_and
//     // R"((\|\|)|)" // binop_or
//     // R"((==)|)"   // binop_equalto
//     // R"((!=)|)"   // binop_notequal
//     // R"((<=)|)"   // binop_lessthanorequal
//     // R"((>=)|)"   // binop_greaterthanorequal
//     // R"((\+=)|)"  // assignment_plus
//     // R"((-=)|)"   // assignment_difference
//     // R"((\*=)|)"  // assignment_product
//     // R"((/=)|)"   // assignment_quotient
//     // R"((%=)|)"   // assignment_remainder
//     // R"((&=)|)"   // assignment_bitand
//     // R"((\|=)|)"  // assignment_bitor
//     // R"((\^=)|)"  // assignment_bitxor
//     // R"((->)|)"   // structop_pointer

//     R"((//)|)"  // comment_singleline
//     R"((/\*)|)" // comment_multilinestart
//     R"((\*/)|)" // comment_multilineend

//     R"((\()|)" // parenthesis_open
//     R"((\))|)" // parenthesis_close
//     R"((\{)|)" // brace_open
//     R"((\})|)" // brace_close
//     // R"((\[)|)"           // brackets_open
//     // R"((\])|)"           // brackets_close
//     R"((;)|)" // semicolon
//     // R"((~)|)"  // unop_complement
//     // R"((-)|)"  // unop_negation
//     // R"((!)|)"  // unop_not
//     // R"((\+)|)" // binop_addition
//     // R"((\*)|)" // binop_multiplication
//     // R"((/)|)"  // binop_division
//     // R"((%)|)"  // binop_remainder
//     // R"((&)|)"  // binop_bitand
//     // R"((\|)|)" // binop_bitor
//     // R"((\^)|)" // binop_bitxor
//     // R"((<)|)"  // binop_lessthan
//     // R"((>)|)"  // binop_greaterthan
//     // R"((=)|)"  // assignment_simple
//     // R"((\?)|)"           // ternary_if
//     // R"((:)|)"            // ternary_else
//     // R"((,)|)"            // separator_comma
//     // R"((\.(?![0-9]+))|)" // structop_member

//     // R"((^\s*#\s*include\b\s*(<[^/]+(/[^/]+)*\.h>|"[^/]+(/[^/]+)*\.h"))|)" // include_directive
//     // R"((^\s*#\s*[_acdefgilmnoprstuwx]+\b)|)" // preprocessor_directive

//     // R"((char\b)|)"     // key_char
//     R"((int\b)|)" // key_int
//     // R"((long\b)|)"     // key_long
//     // R"((double\b)|)"   // key_double
//     // R"((signed\b)|)"   // key_signed
//     // R"((unsigned\b)|)" // key_unsigned
//     R"((void\b)|)" // key_void
//     // R"((struct\b)|)"   // key_struct
//     // R"((union\b)|)"    // key_union
//     // R"((sizeof\b)|)"   // key_sizeof
//     R"((return\b)|)" // key_return
//     // R"((if\b)|)"       // key_if
//     // R"((else\b)|)"     // key_else
//     // R"((goto\b)|)"     // key_goto
//     // R"((do\b)|)"       // key_do
//     // R"((while\b)|)"    // key_while
//     // R"((for\b)|)"      // key_for
//     // R"((switch\b)|)"   // key_switch
//     // R"((case\b)|)"     // key_case
//     // R"((default\b)|)"  // key_default
//     // R"((break\b)|)"    // key_break
//     // R"((continue\b)|)" // key_continue
//     // R"((static\b)|)"   // key_static
//     // R"((extern\b)|)"   // key_extern

//     R"(([a-zA-Z_]\w*\b)|)" // identifier
//     // R"(("([^"\\\n]|\\['"\\?abfnrtv])*")|)"                                               // string_literal
//     // R"(('([^'\\\n]|\\['"?\\abfnrtv])')|)"                                                // char_constant
//     // R"(((([0-9]*\.[0-9]+|[0-9]+\.?)[Ee][+-]?[0-9]+|[0-9]*\.[0-9]+|[0-9]+\.)(?![\w.]))|)" // float_constant
//     // R"(([0-9]+([lL][uU]|[uU][lL])(?![\w.]))|)"                                           // unsigned_long_constant
//     // R"(([0-9]+[uU](?![\w.]))|)"                                                          // unsigned_constant
//     // R"(([0-9]+[lL](?![\w.]))|)"                                                          // long_constant
//     R"(([0-9]+(?![\w.]))|)" // constant

//     R"(([ \n\r\t\f\v])|)" // skip
//     R"((.))"              // error
// );

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
    CTRE_MATCH_TOKEN(R"(//)", TOKEN_KIND::comment_singleline)
    CTRE_MATCH_TOKEN(R"(/\*)", TOKEN_KIND::comment_multilinestart)
    CTRE_MATCH_TOKEN(R"(\*/)", TOKEN_KIND::comment_multilineend)

    CTRE_MATCH_TOKEN(R"(\()", TOKEN_KIND::parenthesis_open)
    CTRE_MATCH_TOKEN(R"(\))", TOKEN_KIND::parenthesis_close)
    CTRE_MATCH_TOKEN(R"(\{)", TOKEN_KIND::brace_open)
    CTRE_MATCH_TOKEN(R"(\})", TOKEN_KIND::brace_close)
    CTRE_MATCH_TOKEN(R"(;)", TOKEN_KIND::semicolon)

    CTRE_MATCH_TOKEN(R"(int\b)", TOKEN_KIND::key_int)
    CTRE_MATCH_TOKEN(R"(void\b)", TOKEN_KIND::key_void)
    CTRE_MATCH_TOKEN(R"(return\b)", TOKEN_KIND::key_return)

    CTRE_MATCH_TOKEN(R"([a-zA-Z_]\w*\b)", TOKEN_KIND::identifier)
    CTRE_MATCH_TOKEN(R"([0-9]+(?![\w.]))", TOKEN_KIND::constant)

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
