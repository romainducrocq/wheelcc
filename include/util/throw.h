#ifndef _UTIL_THROW_H
#define _UTIL_THROW_H

#ifndef __cplusplus
#include <stdnoreturn.h>
#endif

#include "util/c_std.h"

typedef struct FileIoContext FileIoContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

typedef int error_t;
typedef size_t hash_t;
PairKeyValue(hash_t, size_t);

typedef struct FileOpenLine {
    size_t linenum;
    size_t total_linenum;
    string_t filename;
} FileOpenLine;

typedef struct ErrorsContext {
    ErrorsContext* errors;
    FileIoContext* fileio;
    // Throw
    char msg[1024];
    bool is_stdout;
    size_t linebuf;
    hashmap_t(hash_t, size_t) linebuf_map;
    vector_t(FileOpenLine) fopen_lines;
} ErrorsContext;

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

#ifdef __cplusplus
[[noreturn]]
#else
_Noreturn
#endif
void raise_sigabrt(const char* func, const char* file, int line);
#define THROW_ABORT raise_sigabrt(__func__, __FILE__, __LINE__)
#ifdef __NDEBUG__
#define THROW_ABORT_IF(X)
#else
#define THROW_ABORT_IF(X) \
    if (X)                \
    THROW_ABORT
#endif

void raise_init_error(ErrorsContext* ctx);
void raise_error_at_line(ErrorsContext* ctx, size_t linenum);
size_t handle_error_at_line(ErrorsContext* ctx, size_t total_linenum);
#define GET_ERROR_MSG(X, ...) snprintf(ctx->errors->msg, sizeof(char) * 1024, X, __VA_ARGS__)
#define THROW_INIT(...)                                                               \
    do {                                                                              \
        GET_ERROR_MSG(__VA_ARGS__) > 0 ? raise_init_error(ctx->errors) : THROW_ABORT; \
        _errval = 1;                                                                  \
        EARLY_EXIT;                                                                   \
    }                                                                                 \
    while (0)
#define THROW_AT(X, ...)                                                                    \
    do {                                                                                    \
        GET_ERROR_MSG(__VA_ARGS__) > 0 ? raise_error_at_line(ctx->errors, X) : THROW_ABORT; \
        _errval = 1;                                                                        \
        EARLY_EXIT;                                                                         \
    }                                                                                       \
    while (0)
#define THROW_AT_LINE(X, ...) THROW_AT(handle_error_at_line(ctx->errors, X), __VA_ARGS__)

#endif
