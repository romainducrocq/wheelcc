#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

typedef ErrorsContext* Ctx;

static void raise_base_error(Ctx ctx) {
    free_fileio(ctx->fileio);
    const std::string& filename = get_filename(ctx->fileio);
    std::string err_what = "\033[1m";
    err_what += filename;
    err_what += ":\033[0m\n\033[0;31merror:\033[0m ";
    err_what += std::string(ctx->msg);
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    fprintf(stderr, "%s\n", err_what.c_str());
}

[[noreturn]] void raise_sigabrt(const char* func, const char* file, int line) {
    fflush(stdout);
    fprintf(stderr, "\033[1m%s:%i:\033[0m\n\033[0;31minternal error:\033[0m %s\n", file, line, func);
    abort();
}

void raise_init_error(Ctx ctx) {
    std::string err_what = "\033[0;31merror:\033[0m ";
    err_what += std::string(ctx->msg);
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    fprintf(stderr, "%s\n", err_what.c_str());
}

void raise_error_at_line(Ctx ctx, size_t linenum) {
    if (linenum == 0) {
        raise_base_error(ctx);
        return;
    }
    free_fileio(ctx->fileio);
    const std::string& filename = get_filename(ctx->fileio);
    std::string line;
    {
        size_t len = 0;
        char* buf = nullptr;
        FILE* fd = fopen(filename.c_str(), "rb");
        if (!fd) {
            raise_base_error(ctx);
            return;
        }
        for (size_t i = 0; i < linenum; ++i) {
            if (getline(&buf, &len, fd) == -1) {
                free(buf);
                fclose(fd);
                buf = nullptr;
                fd = nullptr;
                raise_base_error(ctx);
                return;
            }
        }
        line = buf;
        free(buf);
        fclose(fd);
        buf = nullptr;
        fd = nullptr;
        if (line.back() == '\n') {
            line.pop_back();
        }
    }
    std::string err_what = "\033[1m";
    err_what += filename;
    err_what += ":";
    err_what += std::to_string(linenum);
    err_what += ":\033[0m\n\033[0;31merror:\033[0m ";
    err_what += std::string(ctx->msg);
    err_what += "\nat line ";
    err_what += std::to_string(linenum);
    err_what += ": \033[1m";
    err_what += line;
    err_what += "\033[0m";
    if (ctx->is_stdout) {
        printf("\n");
        fflush(stdout);
    }
    fprintf(stderr, "%s\n", err_what.c_str());
}

size_t handle_error_at_line(Ctx ctx, size_t total_linenum) {
    for (size_t i = 0; i < ctx->fopen_lines.size() - 1; ++i) {
        if (total_linenum < ctx->fopen_lines[i + 1].total_linenum) {
            set_filename(ctx->fileio, ctx->fopen_lines[i].filename);
            return total_linenum - ctx->fopen_lines[i].total_linenum + ctx->fopen_lines[i].linenum;
        }
    }
    set_filename(ctx->fileio, ctx->fopen_lines.back().filename);
    return total_linenum - ctx->fopen_lines.back().total_linenum + ctx->fopen_lines.back().linenum;
}
