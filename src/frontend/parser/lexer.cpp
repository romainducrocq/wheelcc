#include "ctre/ctre.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "util/fileio.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

struct LexerContext {
    LexerContext(std::vector<std::string>* p_includedirs, std::vector<Token>* p_toks);

    TOKEN_KIND re_match_tok_kind;
    std::string re_match_tok;
    std::string_view re_iter_sv_slice;
    std::vector<std::string> stdlibdirs;
    std::unordered_set<hash_t> includename_set;
    std::vector<std::string>* p_includedirs;
    std::vector<Token>* p_toks;
    size_t total_linenum;
};

LexerContext::LexerContext(std::vector<std::string>* p_includedirs, std::vector<Token>* p_toks) :
    stdlibdirs({
#ifdef __linux__
        "/usr/include/", "/usr/local/include/"
#endif
    }),
    p_includedirs(p_includedirs), p_toks(p_toks), total_linenum(0) {
}

static std::unique_ptr<LexerContext> ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

static void tokenize_include(std::string include_match, size_t linenum);

#define RE_MATCH_TOKEN(X, Y)                                                                 \
    {                                                                                        \
        CTLL_FIXED_STRING re_pattern = {X};                                                  \
        ctre::regex_results re_match = ctre::starts_with<re_pattern>(ctx->re_iter_sv_slice); \
        if (re_match.size()) {                                                               \
            ctx->re_match_tok_kind = Y;                                                      \
            ctx->re_match_tok = std::string(re_match.get<0>());                              \
            return;                                                                          \
        }                                                                                    \
    }

static void re_match_current() {
    RE_MATCH_TOKEN(R"([ \n\r\t\f\v])", TOK_skip)

    RE_MATCH_TOKEN(R"(<<=)", TOK_assign_shiftleft)
    RE_MATCH_TOKEN(R"(>>=)", TOK_assign_shiftright)

    RE_MATCH_TOKEN(R"(\+\+)", TOK_unop_incr)
    RE_MATCH_TOKEN(R"(--)", TOK_unop_decr)
    RE_MATCH_TOKEN(R"(<<)", TOK_binop_shiftleft)
    RE_MATCH_TOKEN(R"(>>)", TOK_binop_shiftright)
    RE_MATCH_TOKEN(R"(&&)", TOK_binop_and)
    RE_MATCH_TOKEN(R"(\|\|)", TOK_binop_or)
    RE_MATCH_TOKEN(R"(==)", TOK_binop_eq)
    RE_MATCH_TOKEN(R"(!=)", TOK_binop_ne)
    RE_MATCH_TOKEN(R"(<=)", TOK_binop_le)
    RE_MATCH_TOKEN(R"(>=)", TOK_binop_ge)
    RE_MATCH_TOKEN(R"(\+=)", TOK_assign_add)
    RE_MATCH_TOKEN(R"(-=)", TOK_assign_subtract)
    RE_MATCH_TOKEN(R"(\*=)", TOK_assign_multiply)
    RE_MATCH_TOKEN(R"(/=)", TOK_assign_divide)
    RE_MATCH_TOKEN(R"(%=)", TOK_assign_remainder)
    RE_MATCH_TOKEN(R"(&=)", TOK_assign_bitand)
    RE_MATCH_TOKEN(R"(\|=)", TOK_assign_bitor)
    RE_MATCH_TOKEN(R"(\^=)", TOK_assign_xor)
    RE_MATCH_TOKEN(R"(->)", TOK_structop_ptr)

    RE_MATCH_TOKEN(R"(//)", TOK_comment_line)
    RE_MATCH_TOKEN(R"(/\*)", TOK_comment_start)
    RE_MATCH_TOKEN(R"(\*/)", TOK_comment_end)

    RE_MATCH_TOKEN(R"(\()", TOK_open_paren)
    RE_MATCH_TOKEN(R"(\))", TOK_close_paren)
    RE_MATCH_TOKEN(R"(\{)", TOK_open_brace)
    RE_MATCH_TOKEN(R"(\})", TOK_close_brace)
    RE_MATCH_TOKEN(R"(\[)", TOK_open_bracket)
    RE_MATCH_TOKEN(R"(\])", TOK_close_bracket)
    RE_MATCH_TOKEN(R"(;)", TOK_semicolon)
    RE_MATCH_TOKEN(R"(~)", TOK_unop_complement)
    RE_MATCH_TOKEN(R"(-)", TOK_unop_neg)
    RE_MATCH_TOKEN(R"(!)", TOK_unop_not)
    RE_MATCH_TOKEN(R"(\+)", TOK_binop_add)
    RE_MATCH_TOKEN(R"(\*)", TOK_binop_multiply)
    RE_MATCH_TOKEN(R"(/)", TOK_binop_divide)
    RE_MATCH_TOKEN(R"(%)", TOK_binop_remainder)
    RE_MATCH_TOKEN(R"(&)", TOK_binop_bitand)
    RE_MATCH_TOKEN(R"(\|)", TOK_binop_bitor)
    RE_MATCH_TOKEN(R"(\^)", TOK_binop_xor)
    RE_MATCH_TOKEN(R"(<)", TOK_binop_lt)
    RE_MATCH_TOKEN(R"(>)", TOK_binop_gt)
    RE_MATCH_TOKEN(R"(=)", TOK_assign)
    RE_MATCH_TOKEN(R"(\?)", TOK_ternary_if)
    RE_MATCH_TOKEN(R"(:)", TOK_ternary_else)
    RE_MATCH_TOKEN(R"(,)", TOK_comma_separator)
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
    RE_MATCH_TOKEN(R"('([^'\\\n]|\\['"?\\abfnrtv])')", TOK_char_const)
    RE_MATCH_TOKEN(R"([0-9]+(?![\w.]))", TOK_int_const)
    RE_MATCH_TOKEN(R"([0-9]+[lL](?![\w.]))", TOK_long_const)
    RE_MATCH_TOKEN(R"([0-9]+[uU](?![\w.]))", TOK_uint_const)
    RE_MATCH_TOKEN(R"([0-9]+([lL][uU]|[uU][lL])(?![\w.]))", TOK_ulong_const)
    RE_MATCH_TOKEN(R"((([0-9]*\.[0-9]+|[0-9]+\.?)[Ee][+\-]?[0-9]+|[0-9]*\.[0-9]+|[0-9]+\.)(?![\w.]))", TOK_dbl_const)

    RE_MATCH_TOKEN(R"(#\s*include\s*[<"][^>"]+\.h[>"])", TOK_include_preproc)
    RE_MATCH_TOKEN(R"(#\s*[_acdefgilmnoprstuwx]+\b)", TOK_strip_preproc)

    RE_MATCH_TOKEN(R"(.)", TOK_error)
}

static void tokenize_file() {
    std::string line;
    bool is_comment = false;
    for (size_t linenum = 1; read_line(line); ++linenum) {
        ctx->total_linenum++;

        const std::string_view re_iter_sv(line);
        for (size_t i = 0; i < line.size(); i += ctx->re_match_tok.size()) {
            ctx->re_iter_sv_slice = re_iter_sv.substr(i);
            re_match_current();
            if (is_comment) {
                if (ctx->re_match_tok_kind == TOK_comment_end) {
                    is_comment = false;
                }
                continue;
            }
            else {
                switch (ctx->re_match_tok_kind) {
                    case TOK_error:
                    case TOK_comment_end:
                        THROW_AT(GET_LEXER_MSG(MSG_invalid_tok, ctx->re_match_tok.c_str()), linenum);
                    case TOK_skip:
                        goto Lcontinue;
                    case TOK_comment_start: {
                        is_comment = true;
                        goto Lcontinue;
                    }
                    case TOK_include_preproc: {
                        i += ctx->re_match_tok.size();
                        tokenize_include(ctx->re_match_tok, linenum);
                        ctx->re_match_tok.clear();
                        goto Lcontinue;
                    }
                    case TOK_comment_line:
                    case TOK_strip_preproc:
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

            Token token = {ctx->re_match_tok_kind, ctx->re_match_tok, ctx->total_linenum};
            ctx->p_toks->emplace_back(std::move(token));
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

static void tokenize_include(std::string filename, size_t linenum) {
    if (filename.back() == '>') {
        filename = filename.substr(filename.find('<') + 1);
        filename.pop_back();
        hash_t includename = string_to_hash(filename);
        if (ctx->includename_set.find(includename) != ctx->includename_set.end()) {
            return;
        }
        ctx->includename_set.insert(includename);
        if (!find_include(ctx->stdlibdirs, filename)) {
            if (!find_include(*ctx->p_includedirs, filename)) {
                THROW_AT(GET_LEXER_MSG(MSG_failed_include, filename.c_str()), linenum);
            }
        }
    }
    else {
        filename = filename.substr(filename.find('"') + 1);
        filename.pop_back();
        hash_t includename = string_to_hash(filename);
        if (ctx->includename_set.find(includename) != ctx->includename_set.end()) {
            return;
        }
        ctx->includename_set.insert(includename);
        if (!find_include(*ctx->p_includedirs, filename)) {
            THROW_AT(GET_LEXER_MSG(MSG_failed_include, filename.c_str()), linenum);
        }
    }

    std::string fopen_name = errors->fopen_lines.back().filename;
    open_fread(filename);
    {
        FileOpenLine fopen_line = {1, ctx->total_linenum + 1, std::move(filename)};
        errors->fopen_lines.emplace_back(std::move(fopen_line));
    }
    tokenize_file();
    close_fread(linenum);
    {
        FileOpenLine fopen_line = {linenum + 1, ctx->total_linenum + 1, std::move(fopen_name)};
        errors->fopen_lines.emplace_back(std::move(fopen_line));
    }
}

static void strip_filename_ext(std::string& filename) { filename = filename.substr(0, filename.size() - 2); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Token> lex_c_code(std::string& filename, std::vector<std::string>&& includedirs) {
    open_fread(filename);
    {
        FileOpenLine fopen_line = {1, 1, filename};
        errors->fopen_lines.emplace_back(std::move(fopen_line));
    }

    std::vector<Token> tokens;
    ctx = std::make_unique<LexerContext>(&includedirs, &tokens);
    tokenize_file();
    ctx.reset();

    close_fread(0);
    includedirs.clear();
    std::vector<std::string>().swap(includedirs);
    set_filename(filename);
    strip_filename_ext(filename);
    return tokens;
}
