#ifndef __cplusplus
#if __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 10)
#define _POSIX_C_SOURCE 200809L
#else
#define _GNU_SOURCE
#endif
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef __cplusplus
#include <stdnoreturn.h>
#endif

#include "util/c_std.h"
#include "util/fileio.h"
#include "util/throw.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

typedef ErrorsContext* Ctx;

#ifdef __cplusplus
[[noreturn]]
#else
_Noreturn
#endif
void panic_sigabrt(const char* func, const char* file, int line, const char* msg) {
    fflush(stdout);
    fprintf(stderr,
        "\033[1m%s:%i:\033[0m\n"
        "\033[0;31minternal error:\033[0m %s (%s)\n",
        file, line, func, msg);
    abort();
}

void raise_init_error(Ctx ctx) {
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    fprintf(stderr, "\033[0;31merror:\033[0m %s\n", ctx->msg);
}

void raise_base_error(Ctx ctx) {
    free_fileio(ctx->fileio);
    const char* filename = get_filename(ctx->fileio);
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    fprintf(stderr,
        "\033[1m%s:\033[0m\n"
        "\033[0;31merror:\033[0m %s\n",
        filename, ctx->msg);
}

static size_t get_token_linenum(Ctx ctx, size_t total_linenum) {
    for (size_t i = 0; i < vec_size(ctx->fopen_lines) - 1; ++i) {
        if (total_linenum < ctx->fopen_lines[i + 1].total_linenum) {
            set_filename(ctx->fileio, ctx->fopen_lines[i].filename);
            return total_linenum - ctx->fopen_lines[i].total_linenum + ctx->fopen_lines[i].linenum;
        }
    }
    set_filename(ctx->fileio, vec_back(ctx->fopen_lines).filename);
    return total_linenum - vec_back(ctx->fopen_lines).total_linenum + vec_back(ctx->fopen_lines).linenum;
}

void raise_error_at_token(Ctx ctx, size_t info_at) {
    THROW_ABORT_IF(info_at >= vec_size(ctx->errors->token_infos));
    const TokenInfo* token_info = &ctx->errors->token_infos[info_at];
    size_t tok_linenum = get_token_linenum(ctx, token_info->total_linenum);

    free_fileio(ctx->fileio);
    const char* filename = get_filename(ctx->fileio);
    string_t line = str_new(NULL);
    {
        size_t len = 0;
        char* buf = NULL;
        FILE* fd = fopen(filename, "rb");
        if (!fd) {
            raise_base_error(ctx);
            return;
        }
        for (size_t i = 0; i < tok_linenum; ++i) {
            if (getline(&buf, &len, fd) == -1) {
                free(buf);
                fclose(fd);
                buf = NULL;
                fd = NULL;
                raise_base_error(ctx);
                return;
            }
        }
        line = str_new(buf);
        free(buf);
        fclose(fd);
        buf = NULL;
        fd = NULL;
        if (str_back(line) == '\n') {
            str_pop_back(line);
        }
    }
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    {
        string_t tok_overline = str_new("");
        int tok_pos = 0;
        if (token_info->tok_pos >= 0) {
            tok_pos = token_info->tok_pos;
            if (token_info->tok_len > 1) {
                str_resize(tok_overline, token_info->tok_len - 1);
                for (size_t i = 0; i < str_size(tok_overline); ++i) {
                    tok_overline[i] = '~';
                }
            }
        }

        string_t strto_linenum = str_to_string(tok_linenum);
        int pad_linenum = (int)str_size(strto_linenum);
        if (pad_linenum < 0) {
            pad_linenum = 0;
        }

        fprintf(stderr,
            "\033[1m%s:%zu:%i:\033[0m\n"
            "\033[0;31merror:\033[0m %s\n"
            "at line %s: \033[0;31m%*sv%s\033[0m\n"
            "        %*s| \033[1m%s\033[0m\n",
            filename, tok_linenum, tok_pos, ctx->msg, strto_linenum, tok_pos, "", tok_overline, pad_linenum, "", line);

        // TODO rm underline
        // fprintf(stderr,
        //     "\033[1m%s:%zu:%i:\033[0m\n"
        //     "\033[0;31merror:\033[0m %s\n"
        //     "at line %s: \033[1m%s\033[0m\n"
        //     "        %*s| %*s^%s\n",
        //     filename, tok_linenum, tok_pos, ctx->msg, strto_linenum, line, pad_linenum, "", tok_pos, "",
        //     tok_overline);

        // TODO rm before
        // fprintf(stderr,
        //     "\033[1m%s:%zu:\033[0m\n"
        //     "\033[0;31merror:\033[0m %s\n"
        //     "at line %s: \033[1m%s\033[0m\n",
        //     filename, tok_linenum, ctx->msg, strto_linenum, line);


        str_delete(tok_overline);
        str_delete(strto_linenum);
    }
    str_delete(line);
}
