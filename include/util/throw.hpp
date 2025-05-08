#ifndef _UTIL_THROW_H
#define _UTIL_THROW_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/str2t.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

struct FileOpenLine {
    size_t linenum;
    size_t total_linenum;
    std::string filename;
};

struct ErrorsContext {
    char msg[1024];
    size_t linebuf;
    std::unordered_map<hash_t, size_t> linebuf_map;
    std::vector<FileOpenLine> fopen_lines;
};

extern std::unique_ptr<ErrorsContext> errors;
#define INIT_ERRORS_CTX errors = std::make_unique<ErrorsContext>()
#define FREE_ERRORS_CTX errors.reset()

size_t handle_error_at_line(size_t total_linenum);
[[noreturn]] void raise_internal_error(const char* func, const char* file, int line);
[[noreturn]] void raise_base_error(const char* error_msg);
[[noreturn]] void raise_runtime_error(const char* error_msg);
[[noreturn]] void raise_runtime_error_at_line(const char* error_msg, size_t linenum);
#define GET_ERROR_MSG(X, ...) snprintf(errors->msg, sizeof(char) * 1024, X, __VA_ARGS__)
#define RAISE_INTERNAL_ERROR raise_internal_error(__func__, __FILE__, __LINE__)
#define RAISE_FATAL_ERROR(X) X > 0 ? raise_base_error(errors->msg) : RAISE_INTERNAL_ERROR
#define RAISE_ARGUMENT_ERROR(X) X > 0 ? raise_base_error(errors->msg) : RAISE_INTERNAL_ERROR
#define RAISE_RUNTIME_ERROR(X) X > 0 ? raise_runtime_error(errors->msg) : RAISE_INTERNAL_ERROR
#define RAISE_RUNTIME_ERROR_AT(X, Y) X > 0 ? raise_runtime_error_at_line(errors->msg, Y) : RAISE_INTERNAL_ERROR
#define RAISE_RUNTIME_ERROR_AT_LINE(X, Y) RAISE_RUNTIME_ERROR_AT(X, handle_error_at_line(Y))
#ifdef __NDEBUG__
#define ABORT_IF(X)
#else
#define ABORT_IF(X)           \
    if (X) {                  \
        RAISE_INTERNAL_ERROR; \
    }
#endif

#endif
