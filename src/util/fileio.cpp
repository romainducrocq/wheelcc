#include "tinydir/tinydir.h"
#include <limits.h>
#include <stdio.h>

#include "util/c_std.h"
#include "util/fileio.h"
#include "util/throw.h"

#include "frontend/parser/errors.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

typedef FileIoContext* Ctx;

#define WRITE_BUF_SIZE 4096

bool find_file(const char* filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename) != -1 && !file.is_dir;
}

const char* get_filename(Ctx ctx) {
    if (!vec_empty(ctx->file_reads)) {
        return vec_back(ctx->file_reads).filename;
    }
    else {
        return ctx->filename;
    }
}

void set_filename(Ctx ctx, const string_t filename) { str_copy(filename, ctx->filename); }

error_t open_fread(Ctx ctx, const string_t filename) {
    CATCH_ENTER;
    for (size_t i = 0; i < vec_size(ctx->file_reads); ++i) {
        if (ctx->file_reads[i].fd) {
            size_t n_fopens = vec_size(ctx->file_reads) - i;
            THROW_ABORT_IF(n_fopens > FOPEN_MAX);
            if (n_fopens == FOPEN_MAX) {
                ctx->file_reads[i].len = 0;
                free(ctx->file_reads[i].buf);
                ctx->file_reads[i].buf = NULL;
                fclose(ctx->file_reads[i].fd);
                ctx->file_reads[i].fd = NULL;
            }
            break;
        }
    }

    FileRead file_read = {0, NULL, NULL, str_new(NULL)};
    file_read.fd = fopen(filename, "rb");
    if (!file_read.fd || str_size(filename) >= PATH_MAX) {
        THROW_AT(0, GET_UTIL_MSG(MSG_failed_fread, filename));
    }
    str_copy(filename, file_read.filename);
    vec_push_back(ctx->file_reads, file_read);
    FINALLY;
    CATCH_EXIT;
}

error_t open_fwrite(Ctx ctx, const string_t filename) {
    CATCH_ENTER;
    THROW_ABORT_IF(!vec_empty(ctx->file_reads));

    ctx->fd_write = NULL;
    ctx->fd_write = fopen(filename, "wb");
    if (!ctx->fd_write || str_size(filename) >= PATH_MAX) {
        THROW_AT(0, GET_UTIL_MSG(MSG_failed_fwrite, filename));
    }

    ctx->write_buf = str_new("");
    str_reserve(ctx->write_buf, WRITE_BUF_SIZE);
    FINALLY;
    CATCH_EXIT;
}

bool read_line(Ctx ctx, char** line, size_t* line_size) {
    ssize_t line_ssize =
        getline(&vec_back(ctx->file_reads).buf, &vec_back(ctx->file_reads).len, vec_back(ctx->file_reads).fd);
    if (line_ssize == -1) {
        line = NULL;
        *line_size = 0;
        vec_back(ctx->file_reads).len = 0;
        free(vec_back(ctx->file_reads).buf);
        vec_back(ctx->file_reads).buf = NULL;
        return false;
    }
    else {
        *line = vec_back(ctx->file_reads).buf;
        *line_size = (size_t)line_ssize;
        return true;
    }
}

static void write_chunk(Ctx ctx, const char* buf, size_t buf_size) {
    fwrite(buf, sizeof(char), buf_size, ctx->fd_write);
}

void write_buffer(Ctx ctx, const char* buf) {
    str_append(ctx->write_buf, buf);
    while (str_size(ctx->write_buf) >= WRITE_BUF_SIZE) {
        write_chunk(ctx, ctx->write_buf, WRITE_BUF_SIZE);
        str_substr(ctx->write_buf, WRITE_BUF_SIZE, -1);
    }
}

error_t close_fread(Ctx ctx, size_t linenum) {
    CATCH_ENTER;
    fclose(vec_back(ctx->file_reads).fd);
    vec_back(ctx->file_reads).fd = NULL;
    str_delete(vec_back(ctx->file_reads).filename);
    vec_pop_back(ctx->file_reads);

    if (!vec_empty(ctx->file_reads) && !vec_back(ctx->file_reads).fd) {
        THROW_ABORT_IF(vec_back(ctx->file_reads).buf || vec_back(ctx->file_reads).len != 0);
        vec_back(ctx->file_reads).fd = fopen(vec_back(ctx->file_reads).filename, "rb");
        if (!vec_back(ctx->file_reads).fd) {
            THROW_AT(0, GET_UTIL_MSG(MSG_failed_fread, vec_back(ctx->file_reads).filename));
        }
        for (size_t i = 0; i < linenum; ++i) {
            if (getline(&vec_back(ctx->file_reads).buf, &vec_back(ctx->file_reads).len, vec_back(ctx->file_reads).fd)
                == -1) {
                THROW_AT(0, GET_UTIL_MSG(MSG_failed_fread, vec_back(ctx->file_reads).filename));
            }
        }
    }
    FINALLY;
    CATCH_EXIT;
}

void close_fwrite(Ctx ctx) {
    write_chunk(ctx, ctx->write_buf, str_size(ctx->write_buf));
    str_clear(ctx->write_buf);

    fclose(ctx->fd_write);
    ctx->fd_write = NULL;
}

void free_fileio(Ctx ctx) {
    for (size_t i = 0; i < vec_size(ctx->file_reads); ++i) {
        FileRead* file_read = &ctx->file_reads[i];
        if (file_read->buf != NULL) {
            free(file_read->buf);
            file_read->buf = NULL;
        }
        if (file_read->fd != NULL) {
            fclose(file_read->fd);
            file_read->fd = NULL;
        }
    }
    if (ctx->fd_write != NULL) {
        fclose(ctx->fd_write);
        ctx->fd_write = NULL;
    }
}
