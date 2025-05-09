#ifndef _UTIL_THROW_H
#define _UTIL_THROW_H

#include <memory>
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
    ErrorsContext(FileIoContext* p_fileio);

    char msg[1024];
    size_t linebuf;
    FileIoContext* p_fileio;
    std::unordered_map<hash_t, size_t> linebuf_map;
    std::vector<FileOpenLine> fopen_lines;
};

extern std::unique_ptr<ErrorsContext> errors;
#define INIT_ERRORS_CTX errors = std::make_unique<ErrorsContext>(fileio.get())
#define FREE_ERRORS_CTX errors.reset()

[[noreturn]] void raise_sigabrt(const char* func, const char* file, int line);
[[noreturn]] void raise_init_error(ErrorsContext* ctx);
[[noreturn]] void raise_rtime_error(ErrorsContext* ctx);
[[noreturn]] void raise_rtime_error_at_line(ErrorsContext* ctx, size_t linenum);
size_t handle_error_at_line(ErrorsContext* ctx, size_t total_linenum);
#define GET_ERROR_MSG(X, ...) snprintf(errors->msg, sizeof(char) * 1024, X, __VA_ARGS__)
#define THROW_ABORT raise_sigabrt(__func__, __FILE__, __LINE__)
#define THROW_INIT(X) X > 0 ? raise_init_error(errors.get()) : THROW_ABORT
#define THROW_BASE(X) X > 0 ? raise_rtime_error(errors.get()) : THROW_ABORT
#define THROW_AT(X, Y) X > 0 ? raise_rtime_error_at_line(errors.get(), Y) : THROW_ABORT
#define THROW_AT_LINE(X, Y) THROW_AT(X, handle_error_at_line(errors.get(), Y))
#ifdef __NDEBUG__
#define THROW_ABORT_IF(X)
#else
#define THROW_ABORT_IF(X) \
    if (X) {              \
        THROW_ABORT;      \
    }
#endif

#endif
