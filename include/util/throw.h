#ifndef _UTIL_THROW_H
#define _UTIL_THROW_H

#ifndef __cplusplus
#include <stdnoreturn.h>
#endif

#include "util/c_std.h"

typedef struct FileIoContext FileIoContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

typedef size_t hash_t;
PairKeyValue(hash_t, size_t);

typedef struct FileOpenLine {
    size_t linenum;
    size_t total_linenum;
    string_t filename;
} FileOpenLine;

typedef struct TokenInfo {
    int tok_pos;
    int tok_len;
    size_t total_linenum;
} TokenInfo;

typedef struct ErrorsContext {
    struct ErrorsContext* errors;
    FileIoContext* fileio;
    // Throw
    char msg[ERROR_MSG_SIZE];
    bool is_stdout;
    size_t info_at_buf;
    hashmap_t(hash_t, size_t) info_at_map;
    vector_t(FileOpenLine) fopen_lines;
    vector_t(TokenInfo) token_infos;
} ErrorsContext;

#ifdef __cplusplus
extern "C" {
[[noreturn]]
#else
_Noreturn
#endif
void panic_sigabrt(const char* func, const char* file, int line, const char* msg);
#ifdef __cplusplus
}
#endif
#define THROW_PANIC(X) panic_sigabrt(__func__, __FILE__, __LINE__, X)
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
void raise_base_error(ErrorsContext* ctx);
void raise_error_at_token(ErrorsContext* ctx, size_t info_at);
#ifdef __cplusplus
}
#endif
#define ERROR_MSG_BUF ctx->errors->msg
#define THROW_INIT(...) THROW_ERROR(1, raise_init_error(ctx->errors), __VA_ARGS__)
#define THROW_BASE(...) THROW_ERROR(1, raise_base_error(ctx->errors), __VA_ARGS__)
#define THROW_AT_TOKEN(X, ...) THROW_ERROR(1, raise_error_at_token(ctx->errors, X), __VA_ARGS__)

#endif
