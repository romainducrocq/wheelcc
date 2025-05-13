#ifndef _UTIL_THROW_H
#define _UTIL_THROW_H

#include <string>
#include <unordered_map>
#include <vector>

#include "util/str2t.hpp"

struct FileIoContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

struct FileOpenLine {
    size_t linenum;
    size_t total_linenum;
    std::string filename;
};

struct ErrorsContext {
    ErrorsContext* errors;
    FileIoContext* fileio;
    // Throw
    char msg[1024];
    size_t linebuf;
    std::unordered_map<hash_t, size_t> linebuf_map;
    std::vector<FileOpenLine> fopen_lines;
};

[[noreturn]] void raise_sigabrt(const char* func, const char* file, int line);
[[noreturn]] void raise_init_error(ErrorsContext* ctx);
[[noreturn]] void raise_error_at_line(ErrorsContext* ctx, size_t linenum);
size_t handle_error_at_line(ErrorsContext* ctx, size_t total_linenum);
#define GET_ERROR_MSG(X, ...) snprintf(ctx->errors->msg, sizeof(char) * 1024, X, __VA_ARGS__)
#define THROW_ABORT raise_sigabrt(__func__, __FILE__, __LINE__)
#define THROW_INIT(X) X > 0 ? raise_init_error(ctx->errors) : THROW_ABORT
#define THROW_AT(X, Y) X > 0 ? raise_error_at_line(ctx->errors, Y) : THROW_ABORT
#define THROW_AT_LINE(X, Y) THROW_AT(X, handle_error_at_line(ctx->errors, Y))
#ifdef __NDEBUG__
#define THROW_ABORT_IF(X)
#else
#define THROW_ABORT_IF(X) \
    if (X) {              \
        THROW_ABORT;      \
    }
#endif

#endif
