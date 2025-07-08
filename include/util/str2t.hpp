#ifndef _UTIL_STR2T_H
#define _UTIL_STR2T_H

#include <inttypes.h>
#include <vector>

#include "util/c_std.hpp"
#include "util/throw.hpp"

struct ErrorsContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String to type

int32_t intmax_to_int32(intmax_t intmax);
int64_t intmax_to_int64(intmax_t intmax);
uint32_t uintmax_to_uint32(uintmax_t uintmax);
uint64_t uintmax_to_uint64(uintmax_t uintmax);
void string_to_literal(string_t str_string, std::vector<int8_t>& string_literal);
int32_t string_to_char_ascii(string_t str_char);
int8_t string_bytes_to_int8(const std::vector<int8_t>& string_literal, size_t byte_at);
int32_t string_bytes_to_int32(const std::vector<int8_t>& string_literal, size_t byte_at);
int64_t string_bytes_to_int64(const std::vector<int8_t>& string_literal, size_t byte_at);
string_t string_literal_to_const(const std::vector<int8_t>& string_literal);
uint64_t dbl_to_binary(double decimal);
error_t string_to_intmax(ErrorsContext* ctx, const char* str_int, size_t line, intmax_t* value);
error_t string_to_uintmax(ErrorsContext* ctx, const char* str_uint, size_t line, uintmax_t* value);
error_t string_to_dbl(ErrorsContext* ctx, const char* str_dbl, size_t line, double* value);

#endif
