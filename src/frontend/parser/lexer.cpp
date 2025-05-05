#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "util/fileio.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

#ifdef __GNUC__
#if __cplusplus > 201703L
#define __WITH_CTRE__
#endif
#endif

#ifdef __WITH_CTRE__
#include "ctre/ctre.hpp"
#include <string_view>
#else
#include "boost/regex.hpp"
#endif

#ifndef __WITH_CTRE__
#define TOKEN_KIND_SIZE TOK_error + 1
#endif

struct LexerContext {
    LexerContext(std::vector<Token>* p_tokens, std::vector<std::string>* p_includedirs);

    TOKEN_KIND re_match_token_kind;
    std::string re_match_token;
#ifdef __WITH_CTRE__
    std::string_view re_iterator_view_slice;
#else
    std::string re_capture_groups[TOKEN_KIND_SIZE];
    std::unique_ptr<const boost::regex> re_compiled_pattern;
#endif
    std::vector<Token>* p_tokens;
    std::vector<std::string>* p_includedirs;
    std::vector<std::string> stdlibdirs;
    std::unordered_set<hash_t> filename_include_set;
    size_t total_line_number;
};

LexerContext::LexerContext(std::vector<Token>* p_tokens, std::vector<std::string>* p_includedirs) :
    p_tokens(p_tokens), p_includedirs(p_includedirs), stdlibdirs({
#ifdef __GNUC__
                                                          "/usr/include/", "/usr/local/include/"
#endif
                                                      }),
    total_line_number(0) {
}

static std::unique_ptr<LexerContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

static void tokenize_include(std::string include_match, size_t tokenize_include);

#ifdef __WITH_CTRE__
#define RE_MATCH_TOKEN(X, Y)                                                                  \
    {                                                                                         \
        ctre::regex_results re_match = ctre::starts_with<X>(context->re_iterator_view_slice); \
        if (re_match.size()) {                                                                \
            context->re_match_token_kind = Y;                                                 \
            context->re_match_token = std::string(re_match.get<0>());                         \
            return;                                                                           \
        }                                                                                     \
    }
#else
#define RE_MATCH_TOKEN(X, Y)                               \
    {                                                      \
        size_t i = static_cast<size_t>(Y);                 \
        context->re_capture_groups[i] = std::to_string(i); \
        re_pattern += "(?<";                               \
        re_pattern += context->re_capture_groups[i];       \
        re_pattern += ">";                                 \
        re_pattern += X;                                   \
        re_pattern += ")|";                                \
    }
#endif

static void
#ifdef __WITH_CTRE__
    re_match_current_tok()
#else
    re_build_tok_pattern(std::string& re_pattern)
#endif
{
    RE_MATCH_TOKEN(R"([ \n\r\t\f\v])", TOK_skip)

    RE_MATCH_TOKEN(R"(<<=)", TOK_assignment_bitshiftleft)
    RE_MATCH_TOKEN(R"(>>=)", TOK_assignment_bitshiftright)

    RE_MATCH_TOKEN(R"(\+\+)", TOK_unop_increment)
    RE_MATCH_TOKEN(R"(--)", TOK_unop_decrement)
    RE_MATCH_TOKEN(R"(<<)", TOK_binop_bitshiftleft)
    RE_MATCH_TOKEN(R"(>>)", TOK_binop_bitshiftright)
    RE_MATCH_TOKEN(R"(&&)", TOK_binop_and)
    RE_MATCH_TOKEN(R"(\|\|)", TOK_binop_or)
    RE_MATCH_TOKEN(R"(==)", TOK_binop_equalto)
    RE_MATCH_TOKEN(R"(!=)", TOK_binop_notequal)
    RE_MATCH_TOKEN(R"(<=)", TOK_binop_lessthanorequal)
    RE_MATCH_TOKEN(R"(>=)", TOK_binop_greaterthanorequal)
    RE_MATCH_TOKEN(R"(\+=)", TOK_assignment_plus)
    RE_MATCH_TOKEN(R"(-=)", TOK_assignment_difference)
    RE_MATCH_TOKEN(R"(\*=)", TOK_assignment_product)
    RE_MATCH_TOKEN(R"(/=)", TOK_assignment_quotient)
    RE_MATCH_TOKEN(R"(%=)", TOK_assignment_remainder)
    RE_MATCH_TOKEN(R"(&=)", TOK_assignment_bitand)
    RE_MATCH_TOKEN(R"(\|=)", TOK_assignment_bitor)
    RE_MATCH_TOKEN(R"(\^=)", TOK_assignment_bitxor)
    RE_MATCH_TOKEN(R"(->)", TOK_structop_pointer)

    RE_MATCH_TOKEN(R"(//)", TOK_comment_singleline)
    RE_MATCH_TOKEN(R"(/\*)", TOK_comment_multilinestart)
    RE_MATCH_TOKEN(R"(\*/)", TOK_comment_multilineend)

    RE_MATCH_TOKEN(R"(\()", TOK_parenthesis_open)
    RE_MATCH_TOKEN(R"(\))", TOK_parenthesis_close)
    RE_MATCH_TOKEN(R"(\{)", TOK_brace_open)
    RE_MATCH_TOKEN(R"(\})", TOK_brace_close)
    RE_MATCH_TOKEN(R"(\[)", TOK_brackets_open)
    RE_MATCH_TOKEN(R"(\])", TOK_brackets_close)
    RE_MATCH_TOKEN(R"(;)", TOK_semicolon)
    RE_MATCH_TOKEN(R"(~)", TOK_unop_complement)
    RE_MATCH_TOKEN(R"(-)", TOK_unop_negation)
    RE_MATCH_TOKEN(R"(!)", TOK_unop_not)
    RE_MATCH_TOKEN(R"(\+)", TOK_binop_addition)
    RE_MATCH_TOKEN(R"(\*)", TOK_binop_multiplication)
    RE_MATCH_TOKEN(R"(/)", TOK_binop_division)
    RE_MATCH_TOKEN(R"(%)", TOK_binop_remainder)
    RE_MATCH_TOKEN(R"(&)", TOK_binop_bitand)
    RE_MATCH_TOKEN(R"(\|)", TOK_binop_bitor)
    RE_MATCH_TOKEN(R"(\^)", TOK_binop_bitxor)
    RE_MATCH_TOKEN(R"(<)", TOK_binop_lessthan)
    RE_MATCH_TOKEN(R"(>)", TOK_binop_greaterthan)
    RE_MATCH_TOKEN(R"(=)", TOK_assignment_simple)
    RE_MATCH_TOKEN(R"(\?)", TOK_ternary_if)
    RE_MATCH_TOKEN(R"(:)", TOK_ternary_else)
    RE_MATCH_TOKEN(R"(,)", TOK_separator_comma)
    RE_MATCH_TOKEN(R"(\.(?![0-9]+))", TOK_structop_member)

    RE_MATCH_TOKEN(R"(char\b)", TOK_key_char)
    RE_MATCH_TOKEN(R"(int\b)", TOK_key_int)
    RE_MATCH_TOKEN(R"(long\b)", TOK_key_long)
    RE_MATCH_TOKEN(R"(double\b)", TOK_key_double)
    RE_MATCH_TOKEN(R"(signed\b)", TOK_key_signed)
    RE_MATCH_TOKEN(R"(unsigned\b)", TOK_key_unsigned)
    RE_MATCH_TOKEN(R"(void\b)", TOK_key_void)
    RE_MATCH_TOKEN(R"(struct\b)", TOK_key_struct)
    RE_MATCH_TOKEN(R"(union\b)", TOK_key_union)
    RE_MATCH_TOKEN(R"(sizeof\b)", TOK_key_sizeof)
    RE_MATCH_TOKEN(R"(return\b)", TOK_key_return)
    RE_MATCH_TOKEN(R"(if\b)", TOK_key_if)
    RE_MATCH_TOKEN(R"(else\b)", TOK_key_else)
    RE_MATCH_TOKEN(R"(goto\b)", TOK_key_goto)
    RE_MATCH_TOKEN(R"(do\b)", TOK_key_do)
    RE_MATCH_TOKEN(R"(while\b)", TOK_key_while)
    RE_MATCH_TOKEN(R"(for\b)", TOK_key_for)
    RE_MATCH_TOKEN(R"(switch\b)", TOK_key_switch)
    RE_MATCH_TOKEN(R"(case\b)", TOK_key_case)
    RE_MATCH_TOKEN(R"(default\b)", TOK_key_default)
    RE_MATCH_TOKEN(R"(break\b)", TOK_key_break)
    RE_MATCH_TOKEN(R"(continue\b)", TOK_key_continue)
    RE_MATCH_TOKEN(R"(static\b)", TOK_key_static)
    RE_MATCH_TOKEN(R"(extern\b)", TOK_key_extern)

    RE_MATCH_TOKEN(R"([a-zA-Z_]\w*\b)", TOK_identifier)
    RE_MATCH_TOKEN(R"("([^"\\\n]|\\['"\\?abfnrtv])*")", TOK_string_literal)
    RE_MATCH_TOKEN(R"('([^'\\\n]|\\['"?\\abfnrtv])')", TOK_char_constant)
    RE_MATCH_TOKEN(R"([0-9]+(?![\w.]))", TOK_constant)
    RE_MATCH_TOKEN(R"([0-9]+[lL](?![\w.]))", TOK_long_constant)
    RE_MATCH_TOKEN(R"([0-9]+[uU](?![\w.]))", TOK_unsigned_constant)
    RE_MATCH_TOKEN(R"([0-9]+([lL][uU]|[uU][lL])(?![\w.]))", TOK_unsigned_long_constant)
    RE_MATCH_TOKEN(
        R"((([0-9]*\.[0-9]+|[0-9]+\.?)[Ee][+\-]?[0-9]+|[0-9]*\.[0-9]+|[0-9]+\.)(?![\w.]))", TOK_float_constant)

    RE_MATCH_TOKEN(R"(#\s*include\s*[<"][^>"]+\.h[>"])", TOK_include_directive)
    RE_MATCH_TOKEN(R"(#\s*[_acdefgilmnoprstuwx]+\b)", TOK_preprocessor_directive)

    RE_MATCH_TOKEN(R"(.)", TOK_error)
}

static void tokenize_file() {
    std::string line;
    bool is_comment = false;
    for (size_t line_number = 1; read_line(line); ++line_number) {
        context->total_line_number++;

#ifdef __WITH_CTRE__
        const std::string_view re_iterator_view(line);
#else
        boost::sregex_iterator re_iterator_end;
#endif
        for (
#ifdef __WITH_CTRE__
            size_t i = 0; i < line.size(); i += context->re_match_token.size()
#else
            boost::sregex_iterator re_iterator_begin =
                boost::sregex_iterator(line.begin(), line.end(), *context->re_compiled_pattern);
            re_iterator_begin != re_iterator_end; re_iterator_begin++
#endif
        ) {
#ifdef __WITH_CTRE__
            context->re_iterator_view_slice = re_iterator_view.substr(i);
            re_match_current_tok();
#else
            {
                boost::smatch re_match = *re_iterator_begin;
                for (size_t i = TOKEN_KIND_SIZE; i-- > 0;) {
                    if (re_match[context->re_capture_groups[i]].matched) {
                        context->re_match_token_kind = static_cast<TOKEN_KIND>(i);
                        context->re_match_token = re_match.get_last_closed_paren();
                        break;
                    }
                }
            }
#endif

            if (is_comment) {
                if (context->re_match_token_kind == TOK_comment_multilineend) {
                    is_comment = false;
                }
                continue;
            }
            else {
                switch (context->re_match_token_kind) {
                    case TOK_error:
                    case TOK_comment_multilineend:
                        RAISE_RUNTIME_ERROR_AT(
                            GET_LEXER_MSG(MSG_invalid_token, context->re_match_token.c_str()), line_number);
                    case TOK_skip:
                        goto Lcontinue;
                    case TOK_comment_multilinestart: {
                        is_comment = true;
                        goto Lcontinue;
                    }
                    case TOK_include_directive: {
#ifdef __WITH_CTRE__
                        i += context->re_match_token.size();
#endif
                        tokenize_include(context->re_match_token, line_number);
#ifdef __WITH_CTRE__
                        context->re_match_token.clear();
#endif
                        goto Lcontinue;
                    }
                    case TOK_comment_singleline:
                    case TOK_preprocessor_directive:
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

            Token token = {context->re_match_token_kind, context->re_match_token, context->total_line_number};
            context->p_tokens->emplace_back(std::move(token));
        }
    }
}

static bool find_include(std::vector<std::string>& dirnames, std::string& filename) {
    for (std::string dirname : dirnames) {
        dirname += filename;
        if (find_file(dirname)) {
            filename = std::move(dirname);
            return true;
        }
    }
    return false;
}

static void tokenize_include(std::string filename, size_t line_number) {
    if (filename.back() == '>') {
        filename = filename.substr(filename.find('<') + 1);
        filename.pop_back();
        hash_t filename_include = string_to_hash(filename);
        if (context->filename_include_set.find(filename_include) != context->filename_include_set.end()) {
            return;
        }
        context->filename_include_set.insert(filename_include);
        if (!find_include(context->stdlibdirs, filename)) {
            if (!find_include(*context->p_includedirs, filename)) {
                RAISE_RUNTIME_ERROR_AT(GET_LEXER_MSG(MSG_failed_to_include_header_file, filename.c_str()), line_number);
            }
        }
    }
    else {
        filename = filename.substr(filename.find('"') + 1);
        filename.pop_back();
        hash_t filename_include = string_to_hash(filename);
        if (context->filename_include_set.find(filename_include) != context->filename_include_set.end()) {
            return;
        }
        context->filename_include_set.insert(filename_include);
        if (!find_include(*context->p_includedirs, filename)) {
            RAISE_RUNTIME_ERROR_AT(GET_LEXER_MSG(MSG_failed_to_include_header_file, filename.c_str()), line_number);
        }
    }

    std::string include_filename = errors->file_open_lines.back().filename;
    open_fread(filename);
    {
        FileOpenLine file_open_line = {1, context->total_line_number + 1, std::move(filename)};
        errors->file_open_lines.emplace_back(std::move(file_open_line));
    }
    tokenize_file();
    close_fread(line_number);
    {
        FileOpenLine file_open_line = {line_number + 1, context->total_line_number + 1, std::move(include_filename)};
        errors->file_open_lines.emplace_back(std::move(file_open_line));
    }
}

static void tokenize_src() {
#ifndef __WITH_CTRE__
    std::string re_pattern("");
    re_build_tok_pattern(re_pattern);
    re_pattern.pop_back();
    context->re_compiled_pattern = std::make_unique<const boost::regex>(std::move(re_pattern));
#endif
    tokenize_file();
}

static void strip_filename_ext(std::string& filename) { filename = filename.substr(0, filename.size() - 2); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Token> lex_c_code(std::string& filename, std::vector<std::string>&& includedirs) {
    open_fread(filename);
    {
        FileOpenLine file_open_line = {1, 1, filename};
        errors->file_open_lines.emplace_back(std::move(file_open_line));
    }

    std::vector<Token> tokens;
    context = std::make_unique<LexerContext>(&tokens, &includedirs);
    tokenize_src();
    context.reset();

    close_fread(0);
    includedirs.clear();
    std::vector<std::string>().swap(includedirs);
    set_filename(filename);
    strip_filename_ext(filename);
    return tokens;
}
