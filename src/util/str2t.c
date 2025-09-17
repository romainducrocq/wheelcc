#include <stdio.h>
#include <string.h>

#include "util/c_std.h"
#include "util/str2t.h"
#include "util/throw.h"

#include "frontend/parser/errors.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// String to type

int32_t intmax_to_int32(intmax_t intmax) { return (int32_t)intmax; }

int64_t intmax_to_int64(intmax_t intmax) { return (int64_t)intmax; }

uint32_t uintmax_to_uint32(uintmax_t uintmax) { return (uint32_t)uintmax; }

uint64_t uintmax_to_uint64(uintmax_t uintmax) { return (uint64_t)uintmax; }

void string_to_literal(const string_t str_string, vector_t(int8_t) * string_literal) {
    THROW_ABORT_IF(str_size(str_string) < 2);
    for (size_t byte = 1; byte < str_size(str_string) - 1; ++byte) {
        char str_char = (char)str_string[byte];
        if (str_char == '\\') {
            str_char = (char)str_string[++byte];
            switch (str_char) {
                case '\'':
                    vec_push_back(*string_literal, 39);
                    break;
                case '"':
                    vec_push_back(*string_literal, 34);
                    break;
                case '?':
                    vec_push_back(*string_literal, 63);
                    break;
                case '\\':
                    vec_push_back(*string_literal, 92);
                    break;
                case 'a':
                    vec_push_back(*string_literal, 7);
                    break;
                case 'b':
                    vec_push_back(*string_literal, 8);
                    break;
                case 'f':
                    vec_push_back(*string_literal, 12);
                    break;
                case 'n':
                    vec_push_back(*string_literal, 10);
                    break;
                case 'r':
                    vec_push_back(*string_literal, 13);
                    break;
                case 't':
                    vec_push_back(*string_literal, 9);
                    break;
                case 'v':
                    vec_push_back(*string_literal, 11);
                    break;
                default:
                    THROW_ABORT;
            }
        }
        else {
            vec_push_back(*string_literal, (int8_t)str_char);
        }
    }
}

int32_t string_to_char_ascii(const string_t str_char) {
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
    char* end_ptr = NULL;
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

int8_t string_bytes_to_int8(const vector_t(int8_t) string_literal, size_t byte_at) {
    string_t str_hex = str_new("");
    for (size_t byte = byte_at + 1; byte-- > byte_at;) {
        if (byte < vec_size(string_literal)) {
            string_literal_byte_to_hex(string_literal[byte], &str_hex);
        }
    }
    int8_t hex_value = hex_string_to_int8(str_hex);
    str_delete(str_hex);
    return hex_value;
}

int32_t string_bytes_to_int32(const vector_t(int8_t) string_literal, size_t byte_at) {
    string_t str_hex = str_new("");
    for (size_t byte = byte_at + 4; byte-- > byte_at;) {
        if (byte < vec_size(string_literal)) {
            string_literal_byte_to_hex(string_literal[byte], &str_hex);
        }
    }
    int32_t hex_value = hex_string_to_int32(str_hex);
    str_delete(str_hex);
    return hex_value;
}

int64_t string_bytes_to_int64(const vector_t(int8_t) string_literal, size_t byte_at) {
    string_t str_hex = str_new("");
    for (size_t byte = byte_at + 8; byte-- > byte_at;) {
        if (byte < vec_size(string_literal)) {
            string_literal_byte_to_hex(string_literal[byte], &str_hex);
        }
    }
    int64_t hex_value = hex_string_to_int64(str_hex);
    str_delete(str_hex);
    return hex_value;
}

string_t string_literal_to_const(const vector_t(int8_t) string_literal) {
    string_t string_const = str_new("");
    for (size_t i = 0; i < vec_size(string_literal); ++i) {
        int8_t byte = string_literal[i];
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
    memcpy(&binary, &decimal, sizeof(uint64_t));
    return binary;
}

error_t string_to_intmax(ErrorsContext* ctx, const char* str_int, size_t line, intmax_t* value) {
    CATCH_ENTER;
    char* end_ptr = NULL;
    *value = strtoimax(str_int, &end_ptr, 10);
    if (end_ptr == str_int) {
        THROW_AT_TOKEN(line, GET_UTIL_MSG(MSG_failed_strtoi, str_int));
    }
    FINALLY;
    CATCH_EXIT;
}

error_t string_to_uintmax(ErrorsContext* ctx, const char* str_uint, size_t line, uintmax_t* value) {
    CATCH_ENTER;
    char* end_ptr = NULL;
    *value = strtoumax(str_uint, &end_ptr, 10);
    if (end_ptr == str_uint) {
        THROW_AT_TOKEN(line, GET_UTIL_MSG(MSG_failed_strtou, str_uint));
    }
    FINALLY;
    CATCH_EXIT;
}

error_t string_to_dbl(ErrorsContext* ctx, const char* str_dbl, size_t line, double* value) {
    CATCH_ENTER;
    char* end_ptr = NULL;
    *value = strtod(str_dbl, &end_ptr);
    if (end_ptr == str_dbl) {
        THROW_AT_TOKEN(line, GET_UTIL_MSG(MSG_failed_strtod, str_dbl));
    }
    FINALLY;
    CATCH_EXIT;
}
