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

#define ERROR_MSG_SIZE 1024

typedef struct FileOpenLine {
    size_t linenum;
    size_t total_linenum;
    string_t filename;
} FileOpenLine;

typedef struct TokenInfo {
    size_t linenum;
    size_t tok_pos;
    size_t tok_len;
} TokenInfo;

typedef struct ErrorsContext {
    struct ErrorsContext* errors;
    FileIoContext* fileio;
    // Throw
    char msg[ERROR_MSG_SIZE];
    bool is_stdout;
    size_t linebuf;
    hashmap_t(hash_t, size_t) linebuf_map;
    vector_t(FileOpenLine) fopen_lines;
    vector_t(TokenInfo) token_infos;
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
extern "C" {
[[noreturn]]
#else
_Noreturn
#endif
void raise_sigabrt(const char* func, const char* file, int line, const char* msg);
#ifdef __cplusplus
}
#endif
#define THROW_PANIC(X) raise_sigabrt(__func__, __FILE__, __LINE__, X)
#define THROW_ABORT THROW_PANIC("abort")
#define THROW_ALLOC(T) THROW_PANIC("alloc " #T)
#ifdef __NDEBUG__
#define THROW_ABORT_IF(X)
#else
#define THROW_ABORT_IF(X) \
    if (X)                \
    THROW_ABORT
#endif

#ifdef __cplusplus
extern "C" {
#endif
void raise_init_error(ErrorsContext* ctx);
void raise_error_at_line(ErrorsContext* ctx, size_t linenum);
size_t handle_error_at_line(ErrorsContext* ctx, size_t total_linenum);
#ifdef __cplusplus
}
#endif
#define GET_ERROR_MSG(X, ...) snprintf(ctx->errors->msg, sizeof(char) * ERROR_MSG_SIZE, X, __VA_ARGS__)
#define THROW_ERROR(X, Y, ...)                            \
    do {                                                  \
        GET_ERROR_MSG(__VA_ARGS__) > 0 ? Y : THROW_ABORT; \
        _errval = X;                                      \
        EARLY_EXIT;                                       \
    }                                                     \
    while (0)
#define THROW_INIT(...) THROW_ERROR(1, raise_init_error(ctx->errors), __VA_ARGS__)
#define THROW_AT(X, ...) THROW_ERROR(1, raise_error_at_line(ctx->errors, X), __VA_ARGS__)
#define THROW_AT_LINE(X, ...) \
    THROW_ERROR(1, raise_error_at_line(ctx->errors, handle_error_at_line(ctx->errors, X)), __VA_ARGS__)

#endif
