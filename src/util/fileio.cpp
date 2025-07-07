#include "tinydir/tinydir.h"
#include <limits.h>
#include <stdio.h>
#include <string>

#include "util/c_std.hpp"
#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

typedef FileIoContext* Ctx;

#define WRITE_BUF_SIZE 4096

bool find_file(const char* filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename) != -1 && !file.is_dir;
}

const char* get_filename(Ctx ctx) {
    if (!ctx->file_reads.empty()) {
        return ctx->file_reads.back().filename.c_str();
    }
    else {
        return ctx->filename;
    }
}

void set_filename(Ctx ctx, string_t filename) { str_copy(filename, ctx->filename); }

error_t open_fread(Ctx ctx, string_t filename) {
    CATCH_ENTER;
    for (size_t i = 0; i < ctx->file_reads.size(); ++i) {
        if (ctx->file_reads[i].fd) {
            size_t n_fopens = ctx->file_reads.size() - i;
            THROW_ABORT_IF(n_fopens > FOPEN_MAX);
            if (n_fopens == FOPEN_MAX) {
                ctx->file_reads[i].len = 0;
                free(ctx->file_reads[i].buf);
                ctx->file_reads[i].buf = nullptr;
                fclose(ctx->file_reads[i].fd);
                ctx->file_reads[i].fd = nullptr;
            }
            break;
        }
    }

    // {
    //     FileRead file_read = {0, NULL, NULL, NULL};
    //     file_read.filename = std::string(filename);
    //     ctx->file_reads.emplace_back(std::move(file_read));
    // }

    ctx->file_reads.emplace_back();
    ctx->file_reads.back().fd = nullptr;
    ctx->file_reads.back().fd = fopen(filename, "rb");
    if (!ctx->file_reads.back().fd || str_size(filename) >= PATH_MAX) {
        THROW_AT(GET_UTIL_MSG(MSG_failed_fread, filename), 0);
    }

    ctx->file_reads.back().len = 0;
    ctx->file_reads.back().buf = nullptr;
    ctx->file_reads.back().filename = std::string(filename);
    FINALLY;
    CATCH_EXIT;
}

error_t open_fwrite(Ctx ctx, string_t filename) {
    CATCH_ENTER;
    THROW_ABORT_IF(!ctx->file_reads.empty());

    ctx->fd_write = nullptr;
    ctx->fd_write = fopen(filename, "wb");
    if (!ctx->fd_write || str_size(filename) >= PATH_MAX) {
        THROW_AT(GET_UTIL_MSG(MSG_failed_fwrite, filename), 0);
    }

    ctx->write_buf.reserve(WRITE_BUF_SIZE);
    ctx->write_buf = "";
    FINALLY;
    CATCH_EXIT;
}

bool read_line(Ctx ctx, char*& line, size_t& line_size) {
    ssize_t line_ssize = getline(&ctx->file_reads.back().buf, &ctx->file_reads.back().len, ctx->file_reads.back().fd);
    if (line_ssize == -1) {
        line = nullptr;
        line_size = 0;
        ctx->file_reads.back().len = 0;
        free(ctx->file_reads.back().buf);
        ctx->file_reads.back().buf = nullptr;
        return false;
    }
    else {
        line = ctx->file_reads.back().buf;
        line_size = (size_t)line_ssize;
        return true;
    }
}

static void write_chunk(Ctx ctx, const std::string& buf) {
    fwrite(buf.c_str(), sizeof(char), buf.size(), ctx->fd_write);
}

void write_buffer(Ctx ctx, const char* buf) {
    ctx->write_buf += std::string(buf);
    while (ctx->write_buf.size() >= WRITE_BUF_SIZE) {
        write_chunk(ctx, ctx->write_buf.substr(0, WRITE_BUF_SIZE));
        ctx->write_buf = ctx->write_buf.substr(WRITE_BUF_SIZE, ctx->write_buf.size() - WRITE_BUF_SIZE);
    }
}

error_t close_fread(Ctx ctx, size_t linenum) {
    CATCH_ENTER;
    fclose(ctx->file_reads.back().fd);
    ctx->file_reads.back().fd = nullptr;
    ctx->file_reads.pop_back();

    if (!ctx->file_reads.empty() && !ctx->file_reads.back().fd) {
        THROW_ABORT_IF(ctx->file_reads.back().buf || ctx->file_reads.back().len != 0);
        ctx->file_reads.back().fd = fopen(ctx->file_reads.back().filename.c_str(), "rb");
        if (!ctx->file_reads.back().fd) {
            THROW_AT(GET_UTIL_MSG(MSG_failed_fread, ctx->file_reads.back().filename.c_str()), 0);
        }
        for (size_t i = 0; i < linenum; ++i) {
            if (getline(&ctx->file_reads.back().buf, &ctx->file_reads.back().len, ctx->file_reads.back().fd) == -1) {
                THROW_AT(GET_UTIL_MSG(MSG_failed_fread, ctx->file_reads.back().filename.c_str()), 0);
            }
        }
    }
    FINALLY;
    CATCH_EXIT;
}

void close_fwrite(Ctx ctx) {
    write_chunk(ctx, ctx->write_buf);
    ctx->write_buf = "";

    fclose(ctx->fd_write);
    ctx->fd_write = nullptr;
}

void free_fileio(Ctx ctx) {
    for (auto& file_read : ctx->file_reads) {
        if (file_read.buf != nullptr) {
            free(file_read.buf);
            file_read.buf = nullptr;
        }
        if (file_read.fd != nullptr) {
            fclose(file_read.fd);
            file_read.fd = nullptr;
        }
    }
    if (ctx->fd_write != nullptr) {
        fclose(ctx->fd_write);
        ctx->fd_write = nullptr;
    }
}
