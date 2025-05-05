#include <cstring>
#include <inttypes.h>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String to type

hash_t string_to_hash(const std::string& string) { return std::hash<std::string> {}(string); }

intmax_t string_to_intmax(const std::string& s_int, size_t line) {
    std::vector<char> buffer(s_int.begin(), s_int.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    intmax_t intmax = strtoimax(&buffer[0], &end_ptr, 10);

    if (end_ptr == &buffer[0]) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_UTIL_MSG(MSG_failed_to_interpret_string_to_integer, s_int.c_str()), line);
    }

    return intmax;
}

int32_t intmax_to_int32(intmax_t intmax) { return static_cast<int32_t>(intmax); }

int64_t intmax_to_int64(intmax_t intmax) { return static_cast<int64_t>(intmax); }

uintmax_t string_to_uintmax(const std::string& s_uint, size_t line) {
    std::vector<char> buffer(s_uint.begin(), s_uint.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    uintmax_t uintmax = strtoumax(&buffer[0], &end_ptr, 10);

    if (end_ptr == &buffer[0]) {
        RAISE_RUNTIME_ERROR_AT_LINE(
            GET_UTIL_MSG(MSG_failed_to_interpret_string_to_unsigned_integer, s_uint.c_str()), line);
    }

    return uintmax;
}

uint32_t uintmax_to_uint32(uintmax_t uintmax) { return static_cast<uint32_t>(uintmax); }

uint64_t uintmax_to_uint64(uintmax_t uintmax) { return static_cast<uint64_t>(uintmax); }

void string_to_literal(const std::string& s_string, std::vector<int8_t>& string_literal) {
    for (size_t byte = 1; byte < s_string.size() - 1; ++byte) {
        char c_char = static_cast<char>(s_string[byte]);
        if (c_char == '\\') {
            c_char = static_cast<char>(s_string[++byte]);
            switch (c_char) {
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
                    RAISE_INTERNAL_ERROR;
            }
        }
        else {
            string_literal.push_back(static_cast<int8_t>(c_char));
        }
    }
}

int32_t string_to_char_ascii(const std::string& s_char) {
    char c_char = static_cast<char>(s_char[1]);
    if (c_char == '\\') {
        c_char = static_cast<char>(s_char[2]);
        switch (c_char) {
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
                RAISE_INTERNAL_ERROR;
        }
    }
    else {
        return static_cast<int32_t>(c_char);
    }
}

static intmax_t hex_string_to_intmax(const std::string& s_hex) {
    std::vector<char> buffer(s_hex.begin(), s_hex.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    intmax_t intmax = strtoimax(&buffer[0], &end_ptr, 16);

    if (end_ptr == &buffer[0]) {
        RAISE_INTERNAL_ERROR;
    }

    return intmax;
}

static int8_t hex_string_to_int8(const std::string& s_hex) { return static_cast<int8_t>(hex_string_to_intmax(s_hex)); }

static int32_t hex_string_to_int32(const std::string& s_hex) {
    return static_cast<int32_t>(hex_string_to_intmax(s_hex));
}

static int64_t hex_string_to_int64(const std::string& s_hex) {
    return static_cast<int64_t>(hex_string_to_intmax(s_hex));
}

static std::string string_literal_byte_to_hex(int8_t val) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(sizeof(int8_t) * 2) << std::hex << (val | 0);
    return ss.str();
}

int8_t string_bytes_to_int8(const std::vector<int8_t>& string_literal, size_t byte_at) {
    std::string s_hex = "";
    for (size_t byte = byte_at + 1; byte-- > byte_at;) {
        if (byte < string_literal.size()) {
            s_hex += string_literal_byte_to_hex(string_literal[byte]);
        }
    }
    return hex_string_to_int8(s_hex);
}

int32_t string_bytes_to_int32(const std::vector<int8_t>& string_literal, size_t byte_at) {
    std::string s_hex = "";
    for (size_t byte = byte_at + 4; byte-- > byte_at;) {
        if (byte < string_literal.size()) {
            s_hex += string_literal_byte_to_hex(string_literal[byte]);
        }
    }
    return hex_string_to_int32(s_hex);
}

int64_t string_bytes_to_int64(const std::vector<int8_t>& string_literal, size_t byte_at) {
    std::string s_hex = "";
    for (size_t byte = byte_at + 8; byte-- > byte_at;) {
        if (byte < string_literal.size()) {
            s_hex += string_literal_byte_to_hex(string_literal[byte]);
        }
    }
    return hex_string_to_int64(s_hex);
}

std::string string_literal_to_const(const std::vector<int8_t>& string_literal) {
    std::string string_constant = "";
    for (int8_t byte : string_literal) {
        switch (byte) {
            case 39:
                string_constant += "\\047";
                break;
            case 34:
                string_constant += "\\042";
                break;
            case 63:
                string_constant += "\\077";
                break;
            case 92:
                string_constant += "\\134";
                break;
            case 7:
                string_constant += "\\007";
                break;
            case 8:
                string_constant += "\\010";
                break;
            case 12:
                string_constant += "\\014";
                break;
            case 10:
                string_constant += "\\012";
                break;
            case 13:
                string_constant += "\\013";
                break;
            case 9:
                string_constant += "\\011";
                break;
            case 11:
                string_constant += "\\013";
                break;
            default:
                string_constant += static_cast<char>(byte);
                break;
        }
    }
    return string_constant;
}

double string_to_dbl(const std::string& s_double, size_t line) {
    std::vector<char> buffer(s_double.begin(), s_double.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    double float64 = strtod(&buffer[0], &end_ptr);

    if (end_ptr == &buffer[0]) {
        RAISE_RUNTIME_ERROR_AT_LINE(GET_UTIL_MSG(MSG_failed_to_interpret_string_to_float, s_double.c_str()), line);
    }

    return float64;
}

uint64_t dbl_to_binary(double decimal) {
    uint64_t binary;
    std::memcpy(&binary, &decimal, sizeof(uint64_t));
    return binary;
}
