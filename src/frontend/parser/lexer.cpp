#include <string>
#include <unordered_set>
#include <vector>

#include "util/fileio.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

struct LexerContext {
    ErrorsContext* errors;
    FileIoContext* fileio;
    // Lexer
    size_t match_tok_at;
    size_t match_tok_size;
    std::string line;
    std::vector<std::string> stdlibdirs;
    std::unordered_map<hash_t, TOKEN_KIND> keyword_map;
    std::unordered_set<hash_t> includename_set;
    std::vector<std::string>* p_includedirs;
    std::vector<Token>* p_toks;
    size_t total_linenum;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

typedef LexerContext* Ctx;

// static void tokenize_include(Ctx ctx, std::string include_match, size_t linenum);

static char get_next(Ctx ctx) {
    size_t i = ctx->match_tok_at + ctx->match_tok_size;
    if (i < ctx->line.size()) {
        return ctx->line[i];
    }
    else {
        return 0;
    }
}

#define LEX_DIGIT '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' : case '9'

#define LEX_LETTER                                                                                                    \
    '_' : case 'a' : case 'b' : case 'c' : case 'd' : case 'e' : case 'f' : case 'g' : case 'h' : case 'i' : case 'j' \
        : case 'k' : case 'l' : case 'm' : case 'n' : case 'o' : case 'p' : case 'q' : case 'r' : case 's' : case 't' \
        : case 'u' : case 'v' : case 'w' : case 'x' : case 'y' : case 'z' : case 'A' : case 'B' : case 'C' : case 'D' \
        : case 'E' : case 'F' : case 'G' : case 'H' : case 'I' : case 'J' : case 'K' : case 'L' : case 'M' : case 'N' \
        : case 'O' : case 'P' : case 'Q' : case 'R' : case 'S' : case 'T' : case 'U' : case 'V' : case 'W' : case 'X' \
        : case 'Y' : case 'Z'

#define LEX_WORD \
    LEX_DIGIT:   \
    case LEX_LETTER

static bool match_next(Ctx ctx, char next) {
    if (next == get_next(ctx)) {
        ctx->match_tok_size++;
        return true;
    }
    else {
        return false;
    }
}

static bool match_digit(Ctx ctx) {
    switch (get_next(ctx)) {
        case LEX_DIGIT: {
            ctx->match_tok_size++;
            return true;
        }
        default:
            return false;
    }
}

static bool match_word(Ctx ctx) {
    switch (get_next(ctx)) {
        case LEX_WORD: {
            ctx->match_tok_size++;
            return true;
        }
        default:
            return false;
    }
}

static TOKEN_KIND match_const(Ctx ctx) {
    while (match_digit(ctx)) {
    }

    TOKEN_KIND match_tok_kind;
    if (match_next(ctx, 'l') || match_next(ctx, 'L')) {
        if (match_next(ctx, 'u') || match_next(ctx, 'U')) {
            match_tok_kind = TOK_ulong_const;
        }
        else {
            match_tok_kind = TOK_long_const;
        }
    }
    else if (match_next(ctx, 'u') || match_next(ctx, 'U')) {
        if (match_next(ctx, 'l') || match_next(ctx, 'L')) {
            match_tok_kind = TOK_ulong_const;
        }
        else {
            match_tok_kind = TOK_uint_const;
        }
    }
    else {
        match_tok_kind = TOK_int_const;
    }

    switch (get_next(ctx)) {
        case LEX_WORD:
            return TOK_error;
        default:
            return match_tok_kind;
    }
}

static TOKEN_KIND match_identifier(Ctx ctx) {
    while (match_word(ctx)) {
    }

    return TOK_identifier;
}

static TOKEN_KIND match_token(Ctx ctx) {
    ctx->match_tok_size = 1;
    switch (ctx->line[ctx->match_tok_at]) {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
        case '\f':
        case '\v':
            return TOK_skip;
        case '(':
            return TOK_open_paren;
        case ')':
            return TOK_close_paren;
        case '{':
            return TOK_open_brace;
        case '}':
            return TOK_close_brace;
        case ';':
            return TOK_semicolon;
        case '~':
            return TOK_unop_complement;
        case '?':
            return TOK_ternary_if;
        case ':':
            return TOK_ternary_else;
        case ',':
            return TOK_comma_separator;
        case '=': {
            if (match_next(ctx, '=')) {
                return TOK_binop_eq;
            }
            else {
                return TOK_assign;
            }
        }
        case '!': {
            if (match_next(ctx, '=')) {
                return TOK_binop_ne;
            }
            else {
                return TOK_unop_not;
            }
        }
        case '-': {
            if (match_next(ctx, '-')) {
                return TOK_unop_decr;
            }
            else if (match_next(ctx, '=')) {
                return TOK_assign_subtract;
            }
            else {
                return TOK_unop_neg;
            }
        }
        case '+': {
            if (match_next(ctx, '+')) {
                return TOK_unop_incr;
            }
            else if (match_next(ctx, '=')) {
                return TOK_assign_add;
            }
            else {
                return TOK_binop_add;
            }
        }
        case '*': {
            if (match_next(ctx, '=')) {
                return TOK_assign_multiply;
            }
            else {
                return TOK_binop_multiply;
            }
        }
        case '/': {
            if (match_next(ctx, '=')) {
                return TOK_assign_divide;
            }
            else {
                return TOK_binop_divide;
            }
        }
        case '%': {
            if (match_next(ctx, '=')) {
                return TOK_assign_remainder;
            }
            else {
                return TOK_binop_remainder;
            }
        }
        case '&': {
            if (match_next(ctx, '&')) {
                return TOK_binop_and;
            }
            else if (match_next(ctx, '=')) {
                return TOK_assign_bitand;
            }
            else {
                return TOK_binop_bitand;
            }
        }
        case '|': {
            if (match_next(ctx, '|')) {
                return TOK_binop_or;
            }
            else if (match_next(ctx, '=')) {
                return TOK_assign_bitor;
            }
            else {
                return TOK_binop_bitor;
            }
        }
        case '<': {
            if (match_next(ctx, '<')) {
                if (match_next(ctx, '=')) {
                    return TOK_assign_shiftleft;
                }
                else {
                    return TOK_binop_shiftleft;
                }
            }
            else if (match_next(ctx, '=')) {
                return TOK_binop_le;
            }
            else {
                return TOK_binop_lt;
            }
        }
        case '>': {
            if (match_next(ctx, '>')) {
                if (match_next(ctx, '=')) {
                    return TOK_assign_shiftright;
                }
                else {
                    return TOK_binop_shiftright;
                }
            }
            else if (match_next(ctx, '=')) {
                return TOK_binop_ge;
            }
            else {
                return TOK_binop_gt;
            }
        }
        case '^': {
            if (match_next(ctx, '=')) {
                return TOK_assign_xor;
            }
            else {
                return TOK_binop_xor;
            }
        }
        case LEX_DIGIT:
            return match_const(ctx);
        case LEX_LETTER:
            return match_identifier(ctx);
        default:
            return TOK_error;
    }
}

// #define RE_MATCH_TOKEN(X, Y)                                                                 \
//     {                                                                                        \
//         CTLL_FIXED_STRING re_pattern = {X};                                                  \
//         ctre::regex_results re_match = ctre::starts_with<re_pattern>(ctx->re_iter_sv_slice); \
//         if (re_match.size()) {                                                               \
//             ctx->re_match_tok_kind = Y;                                                      \
//             ctx->re_match_tok = std::string(re_match.get<0>());                              \
//             return;                                                                          \
//         }                                                                                    \
//     }

// static void re_match_current(Ctx ctx) {
//     RE_MATCH_TOKEN(R"([ \n\r\t\f\v])", TOK_skip)

//     RE_MATCH_TOKEN(R"(<<=)", TOK_assign_shiftleft)
//     RE_MATCH_TOKEN(R"(>>=)", TOK_assign_shiftright)

//     RE_MATCH_TOKEN(R"(\+\+)", TOK_unop_incr)
//     RE_MATCH_TOKEN(R"(--)", TOK_unop_decr)
//     RE_MATCH_TOKEN(R"(<<)", TOK_binop_shiftleft)
//     RE_MATCH_TOKEN(R"(>>)", TOK_binop_shiftright)
//     RE_MATCH_TOKEN(R"(&&)", TOK_binop_and)
//     RE_MATCH_TOKEN(R"(\|\|)", TOK_binop_or)
//     RE_MATCH_TOKEN(R"(==)", TOK_binop_eq)
//     RE_MATCH_TOKEN(R"(!=)", TOK_binop_ne)
//     RE_MATCH_TOKEN(R"(<=)", TOK_binop_le)
//     RE_MATCH_TOKEN(R"(>=)", TOK_binop_ge)
//     RE_MATCH_TOKEN(R"(\+=)", TOK_assign_add)
//     RE_MATCH_TOKEN(R"(-=)", TOK_assign_subtract)
//     RE_MATCH_TOKEN(R"(\*=)", TOK_assign_multiply)
//     RE_MATCH_TOKEN(R"(/=)", TOK_assign_divide)
//     RE_MATCH_TOKEN(R"(%=)", TOK_assign_remainder)
//     RE_MATCH_TOKEN(R"(&=)", TOK_assign_bitand)
//     RE_MATCH_TOKEN(R"(\|=)", TOK_assign_bitor)
//     RE_MATCH_TOKEN(R"(\^=)", TOK_assign_xor)
//     RE_MATCH_TOKEN(R"(->)", TOK_structop_ptr)

//     RE_MATCH_TOKEN(R"(//)", TOK_comment_line)
//     RE_MATCH_TOKEN(R"(/\*)", TOK_comment_start)
//     RE_MATCH_TOKEN(R"(\*/)", TOK_comment_end)

//     RE_MATCH_TOKEN(R"(\()", TOK_open_paren)
//     RE_MATCH_TOKEN(R"(\))", TOK_close_paren)
//     RE_MATCH_TOKEN(R"(\{)", TOK_open_brace)
//     RE_MATCH_TOKEN(R"(\})", TOK_close_brace)
//     RE_MATCH_TOKEN(R"(\[)", TOK_open_bracket)
//     RE_MATCH_TOKEN(R"(\])", TOK_close_bracket)
//     RE_MATCH_TOKEN(R"(;)", TOK_semicolon)
//     RE_MATCH_TOKEN(R"(~)", TOK_unop_complement)
//     RE_MATCH_TOKEN(R"(-)", TOK_unop_neg)
//     RE_MATCH_TOKEN(R"(!)", TOK_unop_not)
//     RE_MATCH_TOKEN(R"(\+)", TOK_binop_add)
//     RE_MATCH_TOKEN(R"(\*)", TOK_binop_multiply)
//     RE_MATCH_TOKEN(R"(/)", TOK_binop_divide)
//     RE_MATCH_TOKEN(R"(%)", TOK_binop_remainder)
//     RE_MATCH_TOKEN(R"(&)", TOK_binop_bitand)
//     RE_MATCH_TOKEN(R"(\|)", TOK_binop_bitor)
//     RE_MATCH_TOKEN(R"(\^)", TOK_binop_xor)
//     RE_MATCH_TOKEN(R"(<)", TOK_binop_lt)
//     RE_MATCH_TOKEN(R"(>)", TOK_binop_gt)
//     RE_MATCH_TOKEN(R"(=)", TOK_assign)
//     RE_MATCH_TOKEN(R"(\?)", TOK_ternary_if)
//     RE_MATCH_TOKEN(R"(:)", TOK_ternary_else)
//     RE_MATCH_TOKEN(R"(,)", TOK_comma_separator)
//     RE_MATCH_TOKEN(R"(\.(?![0-9]+))", TOK_structop_member)

//     RE_MATCH_TOKEN(R"(char\b)", TOK_key_char)
//     RE_MATCH_TOKEN(R"(int\b)", TOK_key_int)
//     RE_MATCH_TOKEN(R"(long\b)", TOK_key_long)
//     RE_MATCH_TOKEN(R"(double\b)", TOK_key_double)
//     RE_MATCH_TOKEN(R"(signed\b)", TOK_key_signed)
//     RE_MATCH_TOKEN(R"(unsigned\b)", TOK_key_unsigned)
//     RE_MATCH_TOKEN(R"(void\b)", TOK_key_void)
//     RE_MATCH_TOKEN(R"(struct\b)", TOK_key_struct)
//     RE_MATCH_TOKEN(R"(union\b)", TOK_key_union)
//     RE_MATCH_TOKEN(R"(sizeof\b)", TOK_key_sizeof)
//     RE_MATCH_TOKEN(R"(return\b)", TOK_key_return)
//     RE_MATCH_TOKEN(R"(if\b)", TOK_key_if)
//     RE_MATCH_TOKEN(R"(else\b)", TOK_key_else)
//     RE_MATCH_TOKEN(R"(goto\b)", TOK_key_goto)
//     RE_MATCH_TOKEN(R"(do\b)", TOK_key_do)
//     RE_MATCH_TOKEN(R"(while\b)", TOK_key_while)
//     RE_MATCH_TOKEN(R"(for\b)", TOK_key_for)
//     RE_MATCH_TOKEN(R"(switch\b)", TOK_key_switch)
//     RE_MATCH_TOKEN(R"(case\b)", TOK_key_case)
//     RE_MATCH_TOKEN(R"(default\b)", TOK_key_default)
//     RE_MATCH_TOKEN(R"(break\b)", TOK_key_break)
//     RE_MATCH_TOKEN(R"(continue\b)", TOK_key_continue)
//     RE_MATCH_TOKEN(R"(static\b)", TOK_key_static)
//     RE_MATCH_TOKEN(R"(extern\b)", TOK_key_extern)

//     RE_MATCH_TOKEN(R"([a-zA-Z_]\w*\b)", TOK_identifier)
//     RE_MATCH_TOKEN(R"("([^"\\\n]|\\['"\\?abfnrtv])*")", TOK_string_literal)
//     RE_MATCH_TOKEN(R"('([^'\\\n]|\\['"?\\abfnrtv])')", TOK_char_const)
//     RE_MATCH_TOKEN(R"([0-9]+(?![\w.]))", TOK_int_const)
//     RE_MATCH_TOKEN(R"([0-9]+[lL](?![\w.]))", TOK_long_const)
//     RE_MATCH_TOKEN(R"([0-9]+[uU](?![\w.]))", TOK_uint_const)
//     RE_MATCH_TOKEN(R"([0-9]+([lL][uU]|[uU][lL])(?![\w.]))", TOK_ulong_const)
//     RE_MATCH_TOKEN(R"((([0-9]*\.[0-9]+|[0-9]+\.?)[Ee][+\-]?[0-9]+|[0-9]*\.[0-9]+|[0-9]+\.)(?![\w.]))",
//     TOK_dbl_const)

//     RE_MATCH_TOKEN(R"(#\s*include\s*[<"][^>"]+\.h[>"])", TOK_include_preproc)
//     RE_MATCH_TOKEN(R"(#\s*[_acdefgilmnoprstuwx]+\b)", TOK_strip_preproc)

//     RE_MATCH_TOKEN(R"(.)", TOK_error)
// }

static void tokenize_file(Ctx ctx) {
    bool is_comment = false;
    for (size_t linenum = 1; read_line(ctx->fileio, ctx->line); ++linenum) {
        ctx->total_linenum++;

        for (ctx->match_tok_at = 0; ctx->match_tok_at < ctx->line.size(); ctx->match_tok_at += ctx->match_tok_size) {
            TOKEN_KIND match_tok_kind = match_token(ctx);
            std::string match_tok = ctx->match_tok_size == 1 ? std::string({ctx->line[ctx->match_tok_at]}) :
                                                               ctx->line.substr(ctx->match_tok_at, ctx->match_tok_size);
            if (is_comment) {
                if (match_tok_kind == TOK_comment_end) {
                    is_comment = false;
                }
                continue;
            }
            else {
                switch (match_tok_kind) {
                    case TOK_error:
                    case TOK_comment_end:
                        THROW_AT(GET_LEXER_MSG(MSG_invalid_tok, match_tok.c_str()), linenum);
                    case TOK_skip:
                        goto Lcontinue;
                    case TOK_comment_start: {
                        is_comment = true;
                        goto Lcontinue;
                    }
                    case TOK_include_preproc: {
                        // TODO
                        //  i += ctx->re_match_tok.size();
                        //  tokenize_include(ctx, ctx->re_match_tok, linenum);
                        //  ctx->re_match_tok.clear();
                        goto Lcontinue;
                    }
                    case TOK_comment_line:
                    case TOK_strip_preproc:
                        goto Lbreak;
                    case TOK_identifier: {
                        hash_t identifier = string_to_hash(match_tok);
                        if (ctx->keyword_map.find(identifier) != ctx->keyword_map.end()) {
                            match_tok_kind = ctx->keyword_map[identifier];
                        }
                        goto Lpass;
                    }
                    default:
                        goto Lpass;
                }
            Lbreak:
                break;
            Lcontinue:
                continue;
            Lpass:;
            }

            Token token = {match_tok_kind, match_tok, ctx->total_linenum};
            ctx->p_toks->emplace_back(std::move(token));
        }
    }
}

// static bool find_include(std::vector<std::string>& dirnames, std::string& filename) {
//     for (std::string dirname : dirnames) {
//         dirname += filename;
//         if (find_file(dirname)) {
//             filename = std::move(dirname);
//             return true;
//         }
//     }
//     return false;
// }

// static void tokenize_include(Ctx ctx, std::string filename, size_t linenum) {
//     if (filename.back() == '>') {
//         filename = filename.substr(filename.find('<') + 1);
//         filename.pop_back();
//         hash_t includename = string_to_hash(filename);
//         if (ctx->includename_set.find(includename) != ctx->includename_set.end()) {
//             return;
//         }
//         ctx->includename_set.insert(includename);
//         if (!find_include(ctx->stdlibdirs, filename)) {
//             if (!find_include(*ctx->p_includedirs, filename)) {
//                 THROW_AT(GET_LEXER_MSG(MSG_failed_include, filename.c_str()), linenum);
//             }
//         }
//     }
//     else {
//         filename = filename.substr(filename.find('"') + 1);
//         filename.pop_back();
//         hash_t includename = string_to_hash(filename);
//         if (ctx->includename_set.find(includename) != ctx->includename_set.end()) {
//             return;
//         }
//         ctx->includename_set.insert(includename);
//         if (!find_include(*ctx->p_includedirs, filename)) {
//             THROW_AT(GET_LEXER_MSG(MSG_failed_include, filename.c_str()), linenum);
//         }
//     }

//     std::string fopen_name = ctx->errors->fopen_lines.back().filename;
//     open_fread(ctx->fileio, filename);
//     {
//         FileOpenLine fopen_line = {1, ctx->total_linenum + 1, std::move(filename)};
//         ctx->errors->fopen_lines.emplace_back(std::move(fopen_line));
//     }
//     tokenize_file(ctx);
//     close_fread(ctx->fileio, linenum);
//     {
//         FileOpenLine fopen_line = {linenum + 1, ctx->total_linenum + 1, std::move(fopen_name)};
//         ctx->errors->fopen_lines.emplace_back(std::move(fopen_line));
//     }
// }

static void strip_filename_ext(std::string& filename) { filename = filename.substr(0, filename.size() - 2); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Token> lex_c_code(
    std::string& filename, std::vector<std::string>&& includedirs, ErrorsContext* errors, FileIoContext* fileio) {
    std::vector<Token> tokens;
    LexerContext ctx;
    {
        ctx.errors = errors;
        ctx.fileio = fileio;
#ifndef __APPLE__
        ctx.stdlibdirs.push_back("/usr/include/");
        ctx.stdlibdirs.push_back("/usr/local/include/");
#endif
        ctx.keyword_map[string_to_hash("char")] = TOK_key_char;
        ctx.keyword_map[string_to_hash("int")] = TOK_key_int;
        ctx.keyword_map[string_to_hash("long")] = TOK_key_long;
        ctx.keyword_map[string_to_hash("double")] = TOK_key_double;
        ctx.keyword_map[string_to_hash("signed")] = TOK_key_signed;
        ctx.keyword_map[string_to_hash("unsigned")] = TOK_key_unsigned;
        ctx.keyword_map[string_to_hash("void")] = TOK_key_void;
        ctx.keyword_map[string_to_hash("struct")] = TOK_key_struct;
        ctx.keyword_map[string_to_hash("union")] = TOK_key_union;
        ctx.keyword_map[string_to_hash("sizeof")] = TOK_key_sizeof;
        ctx.keyword_map[string_to_hash("return")] = TOK_key_return;
        ctx.keyword_map[string_to_hash("if")] = TOK_key_if;
        ctx.keyword_map[string_to_hash("else")] = TOK_key_else;
        ctx.keyword_map[string_to_hash("goto")] = TOK_key_goto;
        ctx.keyword_map[string_to_hash("do")] = TOK_key_do;
        ctx.keyword_map[string_to_hash("while")] = TOK_key_while;
        ctx.keyword_map[string_to_hash("for")] = TOK_key_for;
        ctx.keyword_map[string_to_hash("switch")] = TOK_key_switch;
        ctx.keyword_map[string_to_hash("case")] = TOK_key_case;
        ctx.keyword_map[string_to_hash("default")] = TOK_key_default;
        ctx.keyword_map[string_to_hash("break")] = TOK_key_break;
        ctx.keyword_map[string_to_hash("continue")] = TOK_key_continue;
        ctx.keyword_map[string_to_hash("static")] = TOK_key_static;
        ctx.keyword_map[string_to_hash("extern")] = TOK_key_extern;

        ctx.p_includedirs = &includedirs;
        ctx.p_toks = &tokens;
        ctx.total_linenum = 0;
    }
    open_fread(ctx.fileio, filename);
    {
        FileOpenLine fopen_line = {1, 1, filename};
        ctx.errors->fopen_lines.emplace_back(std::move(fopen_line));
    }
    tokenize_file(&ctx);

    close_fread(ctx.fileio, 0);
    includedirs.clear();
    std::vector<std::string>().swap(includedirs);
    set_filename(ctx.fileio, filename);
    strip_filename_ext(filename);
    return tokens;
}
