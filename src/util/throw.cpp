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
void raise_sigabrt(const char* func, const char* file, int line) {
    fflush(stdout);
    fprintf(stderr, "\033[1m%s:%i:\033[0m\n\033[0;31minternal error:\033[0m %s\n", file, line, func);
    abort();
}

void raise_init_error(Ctx ctx) {
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    fprintf(stderr, "\033[0;31merror:\033[0m %s\n", ctx->msg);
}

static void raise_base_error(Ctx ctx) {
    free_fileio(ctx->fileio);
    const char* filename = get_filename(ctx->fileio);
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    fprintf(stderr, "\033[1m%s:\033[0m\n\033[0;31merror:\033[0m %s\n", filename, ctx->msg);
}

void raise_error_at_line(Ctx ctx, size_t linenum) {
    if (linenum == 0) {
        raise_base_error(ctx);
        return;
    }
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
        for (size_t i = 0; i < linenum; ++i) {
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
    fprintf(stderr, "\033[1m%s:%zu:\033[0m\n\033[0;31merror:\033[0m %s\nat line %zu: \033[1m%s\033[0m\n", filename,
        linenum, ctx->msg, linenum, line);
    str_delete(line);
}

size_t handle_error_at_line(Ctx ctx, size_t total_linenum) {
    for (size_t i = 0; i < vec_size(ctx->fopen_lines) - 1; ++i) {
        if (total_linenum < ctx->fopen_lines[i + 1].total_linenum) {
            set_filename(ctx->fileio, ctx->fopen_lines[i].filename);
            return total_linenum - ctx->fopen_lines[i].total_linenum + ctx->fopen_lines[i].linenum;
        }
    }
    set_filename(ctx->fileio, vec_back(ctx->fopen_lines).filename);
    return total_linenum - vec_back(ctx->fopen_lines).total_linenum + vec_back(ctx->fopen_lines).linenum;
}
