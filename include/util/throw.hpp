#ifndef _UTIL_THROW_HPP
#define _UTIL_THROW_HPP

#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

std::string em(const std::string& text);
[[noreturn]] void raise_argument_error(const std::string& error_message);
[[noreturn]] void raise_runtime_error(const std::string& error_message);
[[noreturn]] void raise_runtime_error_at_line(const std::string& error_message, size_t line_number);
[[noreturn]] void raise_internal_error(const char* func, const char* file, int line);
#define RAISE_INTERNAL_ERROR raise_internal_error(__func__, __FILE__, __LINE__)

#endif
