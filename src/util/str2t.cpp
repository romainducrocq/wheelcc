#include <cstring>
#include <inttypes.h>
#include <stdio.h>
#include <vector>

#include "util/c_std.hpp"
#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String to type

int32_t intmax_to_int32(intmax_t intmax) { return (int32_t)intmax; }

int64_t intmax_to_int64(intmax_t intmax) { return (int64_t)intmax; }

uint32_t uintmax_to_uint32(uintmax_t uintmax) { return (uint32_t)uintmax; }

uint64_t uintmax_to_uint64(uintmax_t uintmax) { return (uint64_t)uintmax; }

void string_to_literal(string_t str_string, std::vector<int8_t>& string_literal) {
    THROW_ABORT_IF(str_size(str_string) < 2);
    for (size_t byte = 1; byte < str_size(str_string) - 1; ++byte) {
        char c_char = (char)str_string[byte];
        if (c_char == '\\') {
            c_char = (char)str_string[++byte];
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
                    THROW_ABORT;
            }
        }
        else {
            string_literal.push_back((int8_t)c_char);
        }
    }
}

int32_t string_to_char_ascii(string_t str_char) {
    THROW_ABORT_IF(str_size(str_char) < 2 || str_size(str_char) > 4);
    char c_char = (char)str_char[1];
    if (c_char == '\\') {
        c_char = (char)str_char[2];
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
                THROW_ABORT;
        }
    }
    else {
        return (int32_t)c_char;
    }
}

static intmax_t hex_string_to_intmax(const char* str_hex) {
    char* end_ptr = nullptr;
    intmax_t value = strtoimax(str_hex, &end_ptr, 16);
    THROW_ABORT_IF(end_ptr == str_hex);
    return value;
}

static int8_t hex_string_to_int8(const char* str_hex) { return (int8_t)hex_string_to_intmax(str_hex); }

static int32_t hex_string_to_int32(const char* str_hex) { return (int32_t)hex_string_to_intmax(str_hex); }

static int64_t hex_string_to_int64(const char* str_hex) { return (int64_t)hex_string_to_intmax(str_hex); }

static void string_literal_byte_to_hex(int8_t value, string_t* str_hex) {
    char byte_hex[3];
    snprintf(byte_hex, sizeof(char) * 3, "%.2x", (uint8_t)value);
    str_append(*str_hex, byte_hex);
}

int8_t string_bytes_to_int8(const std::vector<int8_t>& string_literal, size_t byte_at) {
    string_t str_hex = str_new("");
    for (size_t byte = byte_at + 1; byte-- > byte_at;) {
        if (byte < string_literal.size()) {
            string_literal_byte_to_hex(string_literal[byte], &str_hex);
        }
    }
    int8_t hex_value = hex_string_to_int8(str_hex);
    str_delete(str_hex);
    return hex_value;
}

int32_t string_bytes_to_int32(const std::vector<int8_t>& string_literal, size_t byte_at) {
    string_t str_hex = str_new("");
    for (size_t byte = byte_at + 4; byte-- > byte_at;) {
        if (byte < string_literal.size()) {
            string_literal_byte_to_hex(string_literal[byte], &str_hex);
        }
    }
    int32_t hex_value = hex_string_to_int32(str_hex);
    str_delete(str_hex);
    return hex_value;
}

int64_t string_bytes_to_int64(const std::vector<int8_t>& string_literal, size_t byte_at) {
    string_t str_hex = str_new("");
    for (size_t byte = byte_at + 8; byte-- > byte_at;) {
        if (byte < string_literal.size()) {
            string_literal_byte_to_hex(string_literal[byte], &str_hex);
        }
    }
    int64_t hex_value = hex_string_to_int64(str_hex);
    str_delete(str_hex);
    return hex_value;
}

string_t string_literal_to_const(const std::vector<int8_t>& string_literal) {
    string_t string_const = str_new("");
    for (int8_t byte : string_literal) {
        switch (byte) {
            case 39:
                str_append(string_const, "\\047");
                break;
            case 34:
                str_append(string_const, "\\042");
                break;
            case 63:
                str_append(string_const, "\\077");
                break;
            case 92:
                str_append(string_const, "\\134");
                break;
            case 7:
                str_append(string_const, "\\007");
                break;
            case 8:
                str_append(string_const, "\\010");
                break;
            case 12:
                str_append(string_const, "\\014");
                break;
            case 10:
                str_append(string_const, "\\012");
                break;
            case 13:
                str_append(string_const, "\\013");
                break;
            case 9:
                str_append(string_const, "\\011");
                break;
            case 11:
                str_append(string_const, "\\013");
                break;
            default:
                str_push_back(string_const, (char)byte);
                break;
        }
    }
    return string_const;
}

uint64_t dbl_to_binary(double decimal) {
    uint64_t binary;
    std::memcpy(&binary, &decimal, sizeof(uint64_t));
    return binary;
}

error_t string_to_intmax(ErrorsContext* ctx, const char* str_int, size_t line, return_t(intmax_t) value) {
    CATCH_ENTER;
    char* end_ptr = nullptr;
    *value = strtoimax(str_int, &end_ptr, 10);
    if (end_ptr == str_int) {
        THROW_AT_LINE(line, GET_UTIL_MSG(MSG_failed_strtoi, str_int));
    }
    FINALLY;
    CATCH_EXIT;
}

error_t string_to_uintmax(ErrorsContext* ctx, const char* str_uint, size_t line, return_t(uintmax_t) value) {
    CATCH_ENTER;
    char* end_ptr = nullptr;
    *value = strtoumax(str_uint, &end_ptr, 10);
    if (end_ptr == str_uint) {
        THROW_AT_LINE(line, GET_UTIL_MSG(MSG_failed_strtou, str_uint));
    }
    FINALLY;
    CATCH_EXIT;
}

error_t string_to_dbl(ErrorsContext* ctx, const char* str_dbl, size_t line, return_t(double) value) {
    CATCH_ENTER;
    char* end_ptr = nullptr;
    *value = strtod(str_dbl, &end_ptr);
    if (end_ptr == str_dbl) {
        THROW_AT_LINE(line, GET_UTIL_MSG(MSG_failed_strtod, str_dbl));
    }
    FINALLY;
    CATCH_EXIT;
}
