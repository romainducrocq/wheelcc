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
    char* filename;
};

struct ErrorsContext {
    ErrorsContext* errors;
    FileIoContext* fileio;
    // Throw
    char msg[1024];
    bool is_stdout;
    size_t linebuf;
    std::unordered_map<hash_t, size_t> linebuf_map;
    std::vector<FileOpenLine> fopen_lines;
};

typedef int error_t;

// TODO rm
#ifdef __cplusplus
[[noreturn]]
#else
#include
#include <stdnoreturn.h>
_Noreturn
#endif
void raise_sigabrt(const char* func, const char* file, int line);
void raise_init_error(ErrorsContext* ctx);
void raise_error_at_line(ErrorsContext* ctx, size_t linenum);
size_t handle_error_at_line(ErrorsContext* ctx, size_t total_linenum);

#define GET_ERROR_MSG(X, ...) snprintf(ctx->errors->msg, sizeof(char) * 1024, X, __VA_ARGS__)

// TODO maybe remove return_t, as POD*, unique_ptr*, shared_ptr*, etc...
// already assumes mutable in/out parameter
#define return_t(X) X*
#define CATCH_ENTER error_t _errval = 0
#define CATCH_EXIT return _errval
#define EARLY_EXIT goto _Lfinally
#define FINALLY \
    _Lfinally:
#define TRY(X)              \
    do {                    \
        _errval = X;        \
        if (_errval != 0) { \
            EARLY_EXIT;     \
        }                   \
    }                       \
    while (0)

#define THROW_ABORT raise_sigabrt(__func__, __FILE__, __LINE__)
#ifdef __NDEBUG__
#define THROW_ABORT_IF(X)
#else
#define THROW_ABORT_IF(X) \
    if (X)                \
    THROW_ABORT
#endif

#define THROW_INIT(X)                                        \
    do {                                                     \
        X > 0 ? raise_init_error(ctx->errors) : THROW_ABORT; \
        _errval = 1;                                         \
        EARLY_EXIT;                                          \
    }                                                        \
    while (0)
#define THROW_AT(X, Y)                                             \
    do {                                                           \
        X > 0 ? raise_error_at_line(ctx->errors, Y) : THROW_ABORT; \
        _errval = 1;                                               \
        EARLY_EXIT;                                                \
    }                                                              \
    while (0)
#define THROW_AT_LINE(X, Y) THROW_AT(X, handle_error_at_line(ctx->errors, Y))

#endif
