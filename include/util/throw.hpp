#ifndef _UTIL_THROW_HPP
#define _UTIL_THROW_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "util/str2t.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

struct FileOpenLine {
    size_t line_number;
    size_t total_line_number;
    std::string filename;
};

struct ErrorsContext {
    size_t line_buffer;
    std::unordered_map<hash_t, size_t> line_buffer_map;
    std::vector<FileOpenLine> file_open_lines;
};

extern std::unique_ptr<ErrorsContext> errors;
#define INIT_ERRORS_CONTEXT errors = std::make_unique<ErrorsContext>()
#define FREE_ERRORS_CONTEXT errors.reset()

std::string em(const std::string& text);
size_t handle_error_at_line(size_t total_line_number);
[[noreturn]] void raise_argument_error(const std::string& error_message);
[[noreturn]] void raise_runtime_error(const std::string& error_message);
[[noreturn]] void raise_runtime_error_at_line(const std::string& error_message, size_t line_number);
[[noreturn]] void raise_internal_error(const char* func, const char* file, int line);
#define RAISE_INTERNAL_ERROR raise_internal_error(__func__, __FILE__, __LINE__)
#define RAISE_RUNTIME_ERROR_AT_LINE(X, Y) raise_runtime_error_at_line((X), handle_error_at_line(Y))

#endif
