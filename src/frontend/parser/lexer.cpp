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
    size_t match_at;
    size_t match_size;
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

static char get_next(Ctx ctx) {
    size_t i = ctx->match_at + ctx->match_size;
    if (i < ctx->line.size()) {
        return ctx->line[i];
    }
    else {
        return 0;
    }
}

#define LEX_SPACE ' ' : case '\n' : case '\r' : case '\t'

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
        ctx->match_size++;
        return true;
    }
    else {
        return false;
    }
}

static bool match_nexts(Ctx ctx, const char* nexts, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (!match_next(ctx, nexts[i])) {
            return false;
        }
    }
    return true;
}

static bool match_invert(Ctx ctx, char next) {
    char invert = get_next(ctx);
    if (invert != 0 && next != invert) {
        ctx->match_size++;
        return true;
    }
    else {
        return false;
    }
}

static bool match_space(Ctx ctx) {
    switch (get_next(ctx)) {
        case LEX_SPACE: {
            ctx->match_size++;
            return true;
        }
        default:
            return false;
    }
}

static bool match_digit(Ctx ctx) {
    switch (get_next(ctx)) {
        case LEX_DIGIT: {
            ctx->match_size++;
            return true;
        }
        default:
            return false;
    }
}

static bool match_word(Ctx ctx) {
    switch (get_next(ctx)) {
        case LEX_WORD: {
            ctx->match_size++;
            return true;
        }
        default:
            return false;
    }
}

static TOKEN_KIND match_preproc(Ctx ctx) {
    while (match_space(ctx)) {
    }

    switch (get_next(ctx)) {
        case LEX_LETTER:
            break;
        default:
            return TOK_error;
    }

    if (match_nexts(ctx, "include", 7)) {
        while (match_space(ctx)) {
        }

        if (match_next(ctx, '"')) {
            while (match_invert(ctx, '"')) {
            }
            if (get_next(ctx) == '"') {
                ctx->match_size++;
                return TOK_include_preproc;
            }
        }
        else if (match_next(ctx, '<')) {
            while (match_invert(ctx, '>')) {
            }
            if (get_next(ctx) == '>') {
                ctx->match_size++;
                return TOK_include_preproc;
            }
        }
        return TOK_error;
    }
    else {
        while (match_word(ctx)) {
        }

        return TOK_strip_preproc;
    }
}

static TOKEN_KIND match_char_const(Ctx ctx, bool is_str) {
    // TODO when switch on get_next, also return error when 0 ?
    switch (get_next(ctx)) {
        case '\'': {
            if (!is_str) {
                return TOK_error;
            }
            break;
        }
        case '"': {
            if (is_str) {
                ctx->match_size++;
                return TOK_string_literal;
            }
            break;
        }
        case '\n':
            return TOK_error;
        case '\\': {
            ctx->match_size++;
            switch (get_next(ctx)) {
                case '\'':
                case '"':
                case '\\':
                case '?':
                case 'a':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case 'v':
                    break;
                default:
                    return TOK_error;
            }
        }
        default:
            break;
    }
    ctx->match_size++;

    if (is_str || match_next(ctx, '\'')) {
        return TOK_char_const;
    }
    else {
        return TOK_error;
    }
}

static TOKEN_KIND match_string_literal(Ctx ctx) {
    TOKEN_KIND tok_kind;
    do {
        tok_kind = match_char_const(ctx, true);
    }
    while (tok_kind == TOK_char_const);
    return tok_kind;
}

static TOKEN_KIND match_const_end(Ctx ctx, TOKEN_KIND tok_kind) {
    switch (get_next(ctx)) {
        case LEX_WORD:
        case '.':
            // TODO when TOK_error, need to incr match_size for error msg ?
            return TOK_error;
        default:
            return tok_kind;
    }
}

static TOKEN_KIND match_dbl_exponent(Ctx ctx) {
    switch (get_next(ctx)) {
        case '+':
        case '-': {
            ctx->match_size++;
            break;
        }
        default:
            break;
    }

    if (!match_digit(ctx)) {
        return TOK_error;
    }
    while (match_digit(ctx)) {
    }
    return match_const_end(ctx, TOK_dbl_const);
}

static TOKEN_KIND match_dbl_fraction(Ctx ctx) {
    while (match_digit(ctx)) {
    }

    switch (get_next(ctx)) {
        case 'e':
        case 'E': {
            ctx->match_size++;
            return match_dbl_exponent(ctx);
        }
        default:
            return match_const_end(ctx, TOK_dbl_const);
    }
}

static TOKEN_KIND match_const(Ctx ctx) {
    while (match_digit(ctx)) {
    }

    switch (get_next(ctx)) {
        case 'l':
        case 'L': {
            ctx->match_size++;
            if (match_next(ctx, 'u') || match_next(ctx, 'U')) {
                return match_const_end(ctx, TOK_ulong_const);
            }
            else {
                return match_const_end(ctx, TOK_long_const);
            }
        }
        case 'u':
        case 'U': {
            ctx->match_size++;
            if (match_next(ctx, 'l') || match_next(ctx, 'L')) {
                return match_const_end(ctx, TOK_ulong_const);
            }
            else {
                return match_const_end(ctx, TOK_uint_const);
            }
        }
        case 'e':
        case 'E': {
            ctx->match_size++;
            return match_dbl_exponent(ctx);
        }
        case '.': {
            ctx->match_size++;
            return match_dbl_fraction(ctx);
        }
        default:
            return match_const_end(ctx, TOK_int_const);
    }
}

static TOKEN_KIND match_identifier(Ctx ctx) {
    // switch (ctx->line[ctx->match_at]) {
    //     case 'b': {
    //         if (match_next(ctx, 'r') && match_next('e') && match_next('a') && match_next('k') && !match_word()) {
    //             return TOK_key_break;
    //         }
    //         break;
    //     }
    //     case 'c': {
    //         break;
    //     }
    //     case 'd': {
    //         break;
    //     }
    //     case 'e': {
    //         break;
    //     }
    //     case 'f': {
    //         break;
    //     }
    //     case 'g': {
    //         break;
    //     }
    //     case 'i': {
    //         break;
    //     }
    //     case 'l': {
    //         break;
    //     }
    //     case 'r': {
    //         break;
    //     }
    //     case 's': {
    //         break;
    //     }
    //     case 'u': {
    //         break;
    //     }
    //     case 'v': {
    //         break;
    //     }
    //     case 'w': {
    //         break;
    //     }
    //     default:
    //         break;
    // }

    while (match_word(ctx)) {
    }

    return TOK_identifier;
}

static TOKEN_KIND match_token(Ctx ctx) {
    ctx->match_size = 1;
    switch (ctx->line[ctx->match_at]) {
        case '(':
            return TOK_open_paren;
        case ')':
            return TOK_close_paren;
        case '{':
            return TOK_open_brace;
        case '}':
            return TOK_close_brace;
        case '[':
            return TOK_open_bracket;
        case ']':
            return TOK_close_bracket;
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
            if (match_next(ctx, '>')) {
                return TOK_structop_ptr;
            }
            else if (match_next(ctx, '-')) {
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
            if (match_next(ctx, '/')) {
                return TOK_comment_end;
            }
            else if (match_next(ctx, '=')) {
                return TOK_assign_multiply;
            }
            else {
                return TOK_binop_multiply;
            }
        }
        case '/': {
            if (match_next(ctx, '/')) {
                return TOK_comment_line;
            }
            else if (match_next(ctx, '*')) {
                return TOK_comment_start;
            }
            else if (match_next(ctx, '=')) {
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
        case '.': {
            switch (get_next(ctx)) {
                case LEX_DIGIT:
                    return match_dbl_fraction(ctx);
                default:
                    return TOK_structop_member;
            }
        }
        case '#':
            return match_preproc(ctx);
        case '\'':
            return match_char_const(ctx, false);
        case '"':
            return match_string_literal(ctx);
        case LEX_DIGIT:
            return match_const(ctx);
        case LEX_LETTER:
            return match_identifier(ctx);
        case LEX_SPACE:
        case '\f':
        case '\v':
            return TOK_skip;
        default:
            return TOK_error;
    }
}

static void tokenize_include(Ctx ctx, std::string include_match, size_t linenum);

static void tokenize_file(Ctx ctx) {
    bool is_comment = false;
    char* line = nullptr;
    for (size_t linenum = 1; read_line(ctx->fileio, line); ++linenum) {
        ctx->line = std::string(line);
        ctx->total_linenum++;

        for (ctx->match_at = 0; ctx->match_at < ctx->line.size(); ctx->match_at += ctx->match_size) {
            TOKEN_KIND match_kind = match_token(ctx);
            std::string match_tok = ctx->match_size == 1 ? std::string({ctx->line[ctx->match_at]}) :
                                                           ctx->line.substr(ctx->match_at, ctx->match_size);
            if (is_comment) {
                if (match_kind == TOK_comment_end) {
                    is_comment = false;
                }
                continue;
            }
            else {
                switch (match_kind) {
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
                        size_t match_at = ctx->match_at;
                        size_t match_size = ctx->match_size;
                        tokenize_include(ctx, match_tok, linenum);
                        ctx->match_at = match_at;
                        ctx->match_size = match_size;
                        ctx->line = std::string(line);
                        goto Lcontinue;
                    }
                    case TOK_comment_line:
                    case TOK_strip_preproc:
                        goto Lbreak;
                    case TOK_identifier: {
                        hash_t identifier = string_to_hash(match_tok);
                        if (ctx->keyword_map.find(identifier) != ctx->keyword_map.end()) {
                            match_kind = ctx->keyword_map[identifier];
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

            Token token = {match_kind, match_tok, ctx->total_linenum};
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

static void tokenize_include(Ctx ctx, std::string filename, size_t linenum) {
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

    std::string fopen_name = ctx->errors->fopen_lines.back().filename;
    open_fread(ctx->fileio, filename);
    {
        FileOpenLine fopen_line = {1, ctx->total_linenum + 1, std::move(filename)};
        ctx->errors->fopen_lines.emplace_back(std::move(fopen_line));
    }
    tokenize_file(ctx);
    close_fread(ctx->fileio, linenum);
    {
        FileOpenLine fopen_line = {linenum + 1, ctx->total_linenum + 1, std::move(fopen_name)};
        ctx->errors->fopen_lines.emplace_back(std::move(fopen_line));
    }
}

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
