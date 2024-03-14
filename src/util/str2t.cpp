#include "util/str2t.hpp"
#include "util/error.hpp"

#include <inttypes.h>
#include <cstring>
#include <stdlib.h>
#include <string>
#include <vector>

intmax_t string_to_intmax(const std::string& s_int, size_t line) {
    std::vector<char> buffer(s_int.begin(), s_int.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    intmax_t intmax = strtoimax(&buffer[0], &end_ptr, 10);

    if(end_ptr == &buffer[0]) {
        raise_runtime_error_at_line("String " + em(s_int) + " is not an integer", line);
    }

    return intmax;
}

int32_t intmax_to_int32(intmax_t intmax) {
    return static_cast<int32_t>(intmax);
}

int64_t intmax_to_int64(intmax_t intmax) {
    return static_cast<int64_t>(intmax);
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

uint32_t uintmax_to_uint32(uintmax_t uintmax) {
    return static_cast<uint32_t>(uintmax);
}

uint64_t uintmax_to_uint64(uintmax_t uintmax) {
    return static_cast<uint64_t>(uintmax);
}

void string_to_string_literal(const std::string& s_string, std::vector<int32_t>& string_literal) {
    for(size_t byte = 1; byte < s_string.size() - 1; byte++) {
        char c_char = static_cast<char>(s_string[byte]);
        if(c_char == '\\') {
            c_char = static_cast<char>(s_string[++byte]);
            switch(c_char) {
                case '\'':
                    string_literal.push_back(39);
                    break;
                case '"':
                    string_literal.push_back(34);
                    break;
                case '?':
                    string_literal.push_back(63);
                    break;
                case '\\':
                    string_literal.push_back(92);
                    break;
                case 'a':
                    string_literal.push_back(7);
                    break;
                case 'b':
                    string_literal.push_back(8);
                    break;
                case 'f':
                    string_literal.push_back(12);
                    break;
                case 'n':
                    string_literal.push_back(10);
                    break;
                case 'r':
                    string_literal.push_back(13);
                    break;
                case 't':
                    string_literal.push_back(9);
                    break;
                case 'v':
                    string_literal.push_back(11);
                    break;
                default:
                    break;
            }
        }
        else {
            string_literal.push_back(static_cast<int32_t>(c_char));
        }
    }
}

int32_t string_to_char_ascii(const std::string& s_char) {
    char c_char = static_cast<char>(s_char[1]);
    if(c_char == '\\') {
        c_char = static_cast<char>(s_char[2]);
        switch(c_char) {
            case '\'':
                return 39;
            case '"':
                return 34;
            case '?':
                return 63;
            case '\\':
                return 92;
            case 'a':
                return 7;
            case 'b':
                return 8;
            case 'f':
                return 12;
            case 'n':
                return 10;
            case 'r':
                return 13;
            case 't':
                return 9;
            case 'v':
                return 11;
            default:
                break;
        }
    }
    return static_cast<int32_t>(c_char);
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
