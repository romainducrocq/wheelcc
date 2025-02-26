#ifndef _UTIL_STR2T_HPP
#define _UTIL_STR2T_HPP

#include <inttypes.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String to type

using hash_t = size_t;

hash_t string_to_hash(const std::string& string);
intmax_t string_to_intmax(const std::string& s_int, size_t line);
int32_t intmax_to_int32(intmax_t intmax);
int64_t intmax_to_int64(intmax_t intmax);
uintmax_t string_to_uintmax(const std::string& s_uint, size_t line);
uint32_t uintmax_to_uint32(uintmax_t uintmax);
uint64_t uintmax_to_uint64(uintmax_t uintmax);
void string_to_string_literal(const std::string& s_string, std::vector<int8_t>& string_literal);
int32_t string_to_char_ascii(const std::string& s_char);
int8_t string_literal_bytes_to_int8(const std::vector<int8_t>& string_literal, size_t byte_at);
int32_t string_literal_bytes_to_int32(const std::vector<int8_t>& string_literal, size_t byte_at);
int64_t string_literal_bytes_to_int64(const std::vector<int8_t>& string_literal, size_t byte_at);
std::string string_literal_to_string_constant(const std::vector<int8_t>& string_literal);
double string_to_double(const std::string& s_double, size_t line);
uint64_t double_to_binary(double decimal);

#endif
