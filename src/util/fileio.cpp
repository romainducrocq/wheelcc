#include "tinydir/tinydir.h"
#include <limits.h>
#include <memory>
#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"

std::unique_ptr<FileIoContext> fileio;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

typedef FileIoContext* Ctx;

bool find_file(const std::string& filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename.c_str()) != -1 && !file.is_dir;
}

void set_filename(Ctx ctx, const std::string& filename) { ctx->filename = filename; }

void open_fread(Ctx ctx, const std::string& filename) {
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

    ctx->file_reads.emplace_back();
    ctx->file_reads.back().fd = nullptr;
    ctx->file_reads.back().fd = fopen(filename.c_str(), "rb");
    if (!ctx->file_reads.back().fd || filename.size() >= PATH_MAX) {
        THROW_AT(GET_UTIL_MSG(MSG_failed_fread, filename.c_str()), 0);
    }

    ctx->file_reads.back().len = 0;
    ctx->file_reads.back().buf = nullptr;
    ctx->file_reads.back().filename = filename;
}

void open_fwrite(Ctx ctx, const std::string& filename) {
    THROW_ABORT_IF(!ctx->file_reads.empty());

    ctx->fd_write = nullptr;
    ctx->fd_write = fopen(filename.c_str(), "wb");
    if (!ctx->fd_write || filename.size() >= PATH_MAX) {
        THROW_AT(GET_UTIL_MSG(MSG_failed_fwrite, filename.c_str()), 0);
    }

    ctx->write_buf.reserve(4096);
    ctx->write_buf = "";
}

bool read_line(Ctx ctx, std::string& line) {
    if (getline(&ctx->file_reads.back().buf, &ctx->file_reads.back().len, ctx->file_reads.back().fd) == -1) {
        line = "";
        ctx->file_reads.back().len = 0;
        free(ctx->file_reads.back().buf);
        ctx->file_reads.back().buf = nullptr;
        return false;
    }

    line = ctx->file_reads.back().buf;
    return true;
}

static void write_chunk(Ctx ctx, const std::string& chunk_buf) {
    fwrite(chunk_buf.c_str(), sizeof(char), chunk_buf.size(), ctx->fd_write);
}

static void write_file(Ctx ctx, std::string&& stream_buf, size_t chunk_size) {
    ctx->write_buf += stream_buf;
    while (ctx->write_buf.size() >= chunk_size) {
        write_chunk(ctx, ctx->write_buf.substr(0, chunk_size));
        ctx->write_buf = ctx->write_buf.substr(chunk_size, ctx->write_buf.size() - chunk_size);
    }
}

void write_line(Ctx ctx, std::string&& line) {
    line += "\n";
    write_file(ctx, std::move(line), 4096);
}

void close_fread(Ctx ctx, size_t linenum) {
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
}

void close_fwrite(Ctx ctx) {
    write_chunk(ctx, ctx->write_buf);
    ctx->write_buf = "";

    fclose(ctx->fd_write);
    ctx->fd_write = nullptr;
}
