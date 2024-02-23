#ifndef _UTIL_CTYPES_HPP
#define _UTIL_CTYPES_HPP

#include <inttypes.h>
#include <string>

intmax_t string_to_intmax(const std::string& s_int, size_t line);
int32_t intmax_to_int32(intmax_t intmax);
int64_t intmax_to_int64(intmax_t intmax);
double string_to_double(const std::string& s_double, size_t line);
uint64_t double_to_binary(double decimal);
uintmax_t string_to_uintmax(const std::string& s_uint, size_t line);
uint32_t uintmax_to_uint32(uintmax_t uintmax);
uint64_t uintmax_to_uint64(uintmax_t uintmax);

#endif
