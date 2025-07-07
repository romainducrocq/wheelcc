#include <string>
#include <unordered_set>
#include <vector>

#include "util/c_std.hpp"
#include "util/fileio.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

struct LexerContext {
    ErrorsContext* errors;
    FileIoContext* fileio;
    IdentifierContext* identifiers;
    // Lexer
    char* line;
    size_t line_size;
    size_t match_at;
    size_t match_size;
    std::vector<const char*> stdlibdirs;
    std::unordered_set<hash_t> includename_set;
    std::vector<const char*>* p_includedirs;
    std::vector<Token>* p_toks;
    size_t total_linenum;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Lexer

typedef LexerContext* Ctx;

static char get_char(Ctx ctx) {
    size_t i = ctx->match_at + ctx->match_size;
    if (i < ctx->line_size) {
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

static bool match_char(Ctx ctx, char c) {
    if (c == get_char(ctx)) {
        ctx->match_size++;
        return true;
    }
    else {
        return false;
    }
}

static bool match_chars(Ctx ctx, const char* cs, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (!match_char(ctx, cs[i])) {
            return false;
        }
    }
    return true;
}

static bool match_invert(Ctx ctx, char c) {
    char inv = get_char(ctx);
    if (inv != 0 && c != inv) {
        ctx->match_size++;
        return true;
    }
    else {
        return false;
    }
}

static bool match_space(Ctx ctx) {
    switch (get_char(ctx)) {
        case LEX_SPACE: {
            ctx->match_size++;
            return true;
        }
        default:
            return false;
    }
}

static bool match_digit(Ctx ctx) {
    switch (get_char(ctx)) {
        case LEX_DIGIT: {
            ctx->match_size++;
            return true;
        }
        default:
            return false;
    }
}

static bool match_word(Ctx ctx) {
    switch (get_char(ctx)) {
        case LEX_WORD: {
            ctx->match_size++;
            return true;
        }
        default:
            return false;
    }
}

static TOKEN_KIND match_error(Ctx ctx) {
    ctx->match_size++;
    return TOK_error;
}

static TOKEN_KIND match_preproc(Ctx ctx) {
    while (match_space(ctx)) {
    }

    switch (get_char(ctx)) {
        case LEX_LETTER:
            break;
        default:
            return match_error(ctx);
    }

    if (match_chars(ctx, "include", 7)) {
        while (match_space(ctx)) {
        }

        if (match_char(ctx, '"')) {
            ctx->match_at += ctx->match_size - 1;
            ctx->match_size = 1;

            while (match_invert(ctx, '"')) {
            }
            if (get_char(ctx) == '"') {
                ctx->match_size++;
                return TOK_include_preproc;
            }
        }
        else if (match_char(ctx, '<')) {
            ctx->match_at += ctx->match_size - 1;
            ctx->match_size = 1;

            while (match_invert(ctx, '>')) {
            }
            if (get_char(ctx) == '>') {
                ctx->match_size++;
                return TOK_include_preproc;
            }
        }
        return match_error(ctx);
    }
    else {
        while (match_word(ctx)) {
        }

        return TOK_strip_preproc;
    }
}

static TOKEN_KIND match_char_const(Ctx ctx, bool is_str) {
    switch (get_char(ctx)) {
        case '\'': {
            if (!is_str) {
                return match_error(ctx);
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
            return match_error(ctx);
        case '\\': {
            ctx->match_size++;
            switch (get_char(ctx)) {
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
                    return match_error(ctx);
            }
        }
        default:
            break;
    }
    ctx->match_size++;

    if (is_str || match_char(ctx, '\'')) {
        return TOK_char_const;
    }
    else {
        return match_error(ctx);
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
    switch (get_char(ctx)) {
        case LEX_WORD:
        case '.':
            return match_error(ctx);
        default:
            return tok_kind;
    }
}

static TOKEN_KIND match_dbl_exponent(Ctx ctx) {
    switch (get_char(ctx)) {
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

    switch (get_char(ctx)) {
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

    switch (get_char(ctx)) {
        case 'l':
        case 'L': {
            ctx->match_size++;
            switch (get_char(ctx)) {
                case 'u':
                case 'U': {
                    ctx->match_size++;
                    return match_const_end(ctx, TOK_ulong_const);
                }
                default:
                    return match_const_end(ctx, TOK_long_const);
            }
        }
        case 'u':
        case 'U': {
            ctx->match_size++;
            switch (get_char(ctx)) {
                case 'l':
                case 'L': {
                    ctx->match_size++;
                    return match_const_end(ctx, TOK_ulong_const);
                }
                default:
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
    switch (ctx->line[ctx->match_at]) {
        case 'b': {
            if (match_chars(ctx, "reak", 4) && !match_word(ctx)) {
                return TOK_key_break;
            }
            break;
        }
        case 'c': {
            if (match_char(ctx, 'a')) {
                if (match_chars(ctx, "se", 2) && !match_word(ctx)) {
                    return TOK_key_case;
                }
            }
            else if (match_char(ctx, 'h')) {
                if (match_chars(ctx, "ar", 2) && !match_word(ctx)) {
                    return TOK_key_char;
                }
            }
            else if (match_chars(ctx, "ontinue", 7) && !match_word(ctx)) {
                return TOK_key_continue;
            }
            break;
        }
        case 'd': {
            if (match_char(ctx, 'o')) {
                if (match_char(ctx, 'u')) {
                    if (match_chars(ctx, "ble", 3) && !match_word(ctx)) {
                        return TOK_key_double;
                    }
                }
                else if (!match_word(ctx)) {
                    return TOK_key_do;
                }
            }
            else if (match_chars(ctx, "efault", 6) && !match_word(ctx)) {
                return TOK_key_default;
            }
            break;
        }
        case 'e': {
            if (match_char(ctx, 'l')) {
                if (match_chars(ctx, "se", 2) && !match_word(ctx)) {
                    return TOK_key_else;
                }
            }
            else if (match_chars(ctx, "xtern", 5) && !match_word(ctx)) {
                return TOK_key_extern;
            }
            break;
        }
        case 'f': {
            if (match_chars(ctx, "or", 2) && !match_word(ctx)) {
                return TOK_key_for;
            }
            break;
        }
        case 'g': {
            if (match_chars(ctx, "oto", 3) && !match_word(ctx)) {
                return TOK_key_goto;
            }
            break;
        }
        case 'i': {
            if (match_char(ctx, 'f')) {
                if (!match_word(ctx)) {
                    return TOK_key_if;
                }
            }
            else if (match_chars(ctx, "nt", 2) && !match_word(ctx)) {
                return TOK_key_int;
            }
            break;
        }
        case 'l': {
            if (match_chars(ctx, "ong", 3) && !match_word(ctx)) {
                return TOK_key_long;
            }
            break;
        }
        case 'r': {
            if (match_chars(ctx, "eturn", 5) && !match_word(ctx)) {
                return TOK_key_return;
            }
            break;
        }
        case 's': {
            if (match_char(ctx, 'i')) {
                if (match_char(ctx, 'g')) {
                    if (match_chars(ctx, "ned", 3) && !match_word(ctx)) {
                        return TOK_key_signed;
                    }
                }
                else if (match_char(ctx, 'z')) {
                    if (match_chars(ctx, "eof", 3) && !match_word(ctx)) {
                        return TOK_key_sizeof;
                    }
                }
            }
            else if (match_char(ctx, 't')) {
                if (match_char(ctx, 'a')) {
                    if (match_chars(ctx, "tic", 3) && !match_word(ctx)) {
                        return TOK_key_static;
                    }
                }
                else if (match_char(ctx, 'r')) {
                    if (match_chars(ctx, "uct", 3) && !match_word(ctx)) {
                        return TOK_key_struct;
                    }
                }
            }
            else if (match_chars(ctx, "witch", 5) && !match_word(ctx)) {
                return TOK_key_switch;
            }
            break;
        }
        case 'u': {
            if (match_char(ctx, 'n')) {
                if (match_char(ctx, 'i')) {
                    if (match_chars(ctx, "on", 2) && !match_word(ctx)) {
                        return TOK_key_union;
                    }
                }
                else if (match_chars(ctx, "signed", 6) && !match_word(ctx)) {
                    return TOK_key_unsigned;
                }
            }
            break;
        }
        case 'v': {
            if (match_chars(ctx, "oid", 3) && !match_word(ctx)) {
                return TOK_key_void;
            }
            break;
        }
        case 'w': {
            if (match_chars(ctx, "hile", 4) && !match_word(ctx)) {
                return TOK_key_while;
            }
            break;
        }
        default:
            break;
    }

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
            if (match_char(ctx, '=')) {
                return TOK_binop_eq;
            }
            else {
                return TOK_assign;
            }
        }
        case '!': {
            if (match_char(ctx, '=')) {
                return TOK_binop_ne;
            }
            else {
                return TOK_unop_not;
            }
        }
        case '-': {
            if (match_char(ctx, '>')) {
                return TOK_structop_ptr;
            }
            else if (match_char(ctx, '-')) {
                return TOK_unop_decr;
            }
            else if (match_char(ctx, '=')) {
                return TOK_assign_subtract;
            }
            else {
                return TOK_unop_neg;
            }
        }
        case '+': {
            if (match_char(ctx, '+')) {
                return TOK_unop_incr;
            }
            else if (match_char(ctx, '=')) {
                return TOK_assign_add;
            }
            else {
                return TOK_binop_add;
            }
        }
        case '*': {
            if (match_char(ctx, '/')) {
                return TOK_error;
            }
            else if (match_char(ctx, '=')) {
                return TOK_assign_multiply;
            }
            else {
                return TOK_binop_multiply;
            }
        }
        case '/': {
            if (match_char(ctx, '/')) {
                return TOK_comment_line;
            }
            else if (match_char(ctx, '*')) {
                return TOK_comment_start;
            }
            else if (match_char(ctx, '=')) {
                return TOK_assign_divide;
            }
            else {
                return TOK_binop_divide;
            }
        }
        case '%': {
            if (match_char(ctx, '=')) {
                return TOK_assign_remainder;
            }
            else {
                return TOK_binop_remainder;
            }
        }
        case '&': {
            if (match_char(ctx, '&')) {
                return TOK_binop_and;
            }
            else if (match_char(ctx, '=')) {
                return TOK_assign_bitand;
            }
            else {
                return TOK_binop_bitand;
            }
        }
        case '|': {
            if (match_char(ctx, '|')) {
                return TOK_binop_or;
            }
            else if (match_char(ctx, '=')) {
                return TOK_assign_bitor;
            }
            else {
                return TOK_binop_bitor;
            }
        }
        case '<': {
            if (match_char(ctx, '<')) {
                if (match_char(ctx, '=')) {
                    return TOK_assign_shiftleft;
                }
                else {
                    return TOK_binop_shiftleft;
                }
            }
            else if (match_char(ctx, '=')) {
                return TOK_binop_le;
            }
            else {
                return TOK_binop_lt;
            }
        }
        case '>': {
            if (match_char(ctx, '>')) {
                if (match_char(ctx, '=')) {
                    return TOK_assign_shiftright;
                }
                else {
                    return TOK_binop_shiftright;
                }
            }
            else if (match_char(ctx, '=')) {
                return TOK_binop_ge;
            }
            else {
                return TOK_binop_gt;
            }
        }
        case '^': {
            if (match_char(ctx, '=')) {
                return TOK_assign_xor;
            }
            else {
                return TOK_binop_xor;
            }
        }
        case '.': {
            switch (get_char(ctx)) {
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

static TOKEN_KIND match_comment_end(Ctx ctx) {
    ctx->match_size = 1;
    switch (ctx->line[ctx->match_at]) {
        case '*': {
            if (match_char(ctx, '/')) {
                return TOK_comment_end;
            }
            else {
                return TOK_skip;
            }
            default:
                return TOK_skip;
        }
    }
}

static string_t get_match(Ctx ctx, size_t match_at, size_t match_size) {
    string_t match = str_new("");
    str_resize(match, match_size);
    for (size_t i = 0; i < match_size; ++i) {
        match[i] = ctx->line[match_at + i];
    }
    return match;
}

static error_t tokenize_include(Ctx ctx, size_t linenum);

static error_t tokenize_file(Ctx ctx) {
    string_t match = str_new(NULL);
    CATCH_ENTER;
    bool is_comment = false;
    for (size_t linenum = 1; read_line(ctx->fileio, ctx->line, ctx->line_size); ++linenum) {
        ctx->total_linenum++;

        for (ctx->match_at = 0; ctx->match_at < ctx->line_size; ctx->match_at += ctx->match_size) {
            TOKEN_KIND match_kind = is_comment ? match_comment_end(ctx) : match_token(ctx);
            TIdentifier match_tok = 0;
            switch (match_kind) {
                case TOK_comment_line:
                case TOK_strip_preproc:
                    goto Lbreak;
                case TOK_skip:
                    goto Lcontinue;
                case TOK_comment_start: {
                    is_comment = true;
                    goto Lcontinue;
                }
                case TOK_comment_end: {
                    is_comment = false;
                    goto Lcontinue;
                }
                case TOK_include_preproc:
                    TRY(tokenize_include(ctx, linenum));
                    goto Lcontinue;
                case TOK_identifier:
                case TOK_string_literal:
                case TOK_char_const:
                case TOK_int_const:
                case TOK_long_const:
                case TOK_uint_const:
                case TOK_ulong_const:
                case TOK_dbl_const: {
                    match = get_match(ctx, ctx->match_at, ctx->match_size);
                    match_tok = make_string_identifier(ctx->identifiers, std::string(match));
                    str_delete(match); // TODO rm
                    goto Lpass;
                }
                case TOK_error: {
                    match = get_match(ctx, ctx->match_at, ctx->match_size);
                    THROW_AT(GET_LEXER_MSG(MSG_invalid_tok, match), linenum);
                }
                default:
                    goto Lpass;
            }
        Lbreak:
            break;
        Lcontinue:
            continue;
        Lpass:
            Token token = {match_kind, match_tok, ctx->total_linenum};
            ctx->p_toks->emplace_back(std::move(token));
        }
    }
    FINALLY;
    str_delete(match);
    CATCH_EXIT;
}

static bool find_include(std::vector<const char*>& dirnames, string_t* filename) {
    for (size_t i = 0; i < dirnames.size(); ++i) {
        string_t dirname = str_new(dirnames[i]);
        str_append(dirname, *filename);
        if (find_file(dirname)) {
            str_move(&dirname, filename);
            return true;
        }
        str_delete(dirname);
    }
    return false;
}

static error_t tokenize_include(Ctx ctx, size_t linenum) {
    string_t filename = str_new(NULL);
    string_t fopen_name = str_new(NULL);
    CATCH_ENTER;
    char* line;
    size_t line_size;
    size_t match_at;
    size_t match_size;

    filename = get_match(ctx, ctx->match_at + 1, ctx->match_size - 2);
    {
        hash_t includename = string_to_hash(filename);
        if (ctx->includename_set.find(includename) != ctx->includename_set.end()) {
            EARLY_EXIT;
        }
        ctx->includename_set.insert(includename);
    }
    switch (ctx->line[ctx->match_at]) {
        case '<': {
            if (!find_include(ctx->stdlibdirs, &filename) && !find_include(*ctx->p_includedirs, &filename)) {
                THROW_AT(GET_LEXER_MSG(MSG_failed_include, filename), linenum);
            }
            break;
        }
        case '"': {
            if (!find_include(*ctx->p_includedirs, &filename)) {
                THROW_AT(GET_LEXER_MSG(MSG_failed_include, filename), linenum);
            }
            break;
        }
        default:
            THROW_ABORT;
    }

    line = ctx->line;
    line_size = ctx->line_size;
    match_at = ctx->match_at;
    match_size = ctx->match_size;

    str_copy(ctx->errors->fopen_lines.back().filename, fopen_name);
    TRY(open_fread(ctx->fileio, filename, str_size(filename)));
    {
        FileOpenLine fopen_line = {1, ctx->total_linenum + 1, NULL};
        str_move(&filename, &fopen_line.filename);
        ctx->errors->fopen_lines.emplace_back(std::move(fopen_line));
    }
    TRY(tokenize_file(ctx));
    TRY(close_fread(ctx->fileio, linenum));
    {
        FileOpenLine fopen_line = {linenum + 1, ctx->total_linenum + 1, NULL};
        str_move(&fopen_name, &fopen_line.filename);
        ctx->errors->fopen_lines.emplace_back(std::move(fopen_line));
    }

    ctx->line = line;
    ctx->line_size = line_size;
    ctx->match_at = match_at;
    ctx->match_size = match_size;
    FINALLY;
    str_delete(filename);
    str_delete(fopen_name);
    CATCH_EXIT;
}

static void strip_filename_ext(std::string& filename) {
    for (size_t i = filename.size(); i-- > 0;) {
        if (filename.back() == '.') {
            filename.pop_back();
            break;
        }
        filename.pop_back();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

error_t lex_c_code(std::string& filename, std::vector<const char*>&& includedirs, ErrorsContext* errors,
    FileIoContext* fileio, IdentifierContext* identifiers, return_t(std::vector<Token>) tokens) {
    LexerContext ctx;
    {
        ctx.errors = errors;
        ctx.fileio = fileio;
        ctx.identifiers = identifiers;
#ifndef __APPLE__
        ctx.stdlibdirs.push_back("/usr/include/");
        ctx.stdlibdirs.push_back("/usr/local/include/");
#endif
        ctx.p_includedirs = &includedirs;
        ctx.p_toks = tokens;
        ctx.total_linenum = 0;
    }
    string_t sds_filename = str_new(filename.c_str()); // TODO
    CATCH_ENTER;
    TRY(open_fread(ctx.fileio, sds_filename, str_size(sds_filename)));
    {
        FileOpenLine fopen_line = {1, 1, NULL};
        str_copy(sds_filename, fopen_line.filename);
        ctx.errors->fopen_lines.emplace_back(std::move(fopen_line));
    }
    TRY(tokenize_file(&ctx));

    TRY(close_fread(ctx.fileio, 0));
    includedirs.clear();
    set_filename(ctx.fileio, sds_filename);
    strip_filename_ext(filename);
    EARLY_EXIT;
    FINALLY;
    std::vector<const char*>().swap(includedirs);
    str_delete(sds_filename);
    CATCH_EXIT;
}
