#include "util/error.hpp"

#include <inttypes.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cstring>

intmax_t string_to_intmax(const std::string& s_int, size_t line) {
    std::vector<char> buffer(s_int.begin(), s_int.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    intmax_t intmax = strtoimax(&buffer[0], &end_ptr, 10);

    if(end_ptr == &buffer[0]) {
        raise_runtime_error_at_line("String " + em(s_int) + " is not an integer",
                                    line);
    }

    return intmax;
}

uintmax_t string_to_uintmax(const std::string& s_uint, size_t line) {
    std::vector<char> buffer(s_uint.begin(), s_uint.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    uintmax_t uintmax = strtoumax(&buffer[0], &end_ptr, 10);

    if(end_ptr == &buffer[0]) {
        raise_runtime_error_at_line("String " + em(s_uint) + " is not an unsigned integer",
                                    line);
    }

    return uintmax;
}

int32_t intmax_to_int32(intmax_t intmax) {
    return static_cast<int32_t>(intmax);
}

int64_t intmax_to_int64(intmax_t intmax) {
    return static_cast<int64_t>(intmax);
}

uint32_t uintmax_to_uint32(uintmax_t uintmax) {
    return static_cast<uint32_t>(uintmax);
}

uint64_t uintmax_to_uint64(uintmax_t uintmax) {
    return static_cast<uint64_t>(uintmax);
}

double string_to_double(const std::string& s_double, size_t line) {
    std::vector<char> buffer(s_double.begin(), s_double.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    double float64 = strtod(&buffer[0], &end_ptr);

    if(end_ptr == &buffer[0]) {
        raise_runtime_error_at_line("String " + em(s_double) + " is not a floating point number",
                                    line);
    }

    return float64;
}

uint64_t double_to_binary(double decimal) {
    uint64_t binary;
    std::memcpy(&binary, &decimal, sizeof(uint64_t));
    return binary;
}
