#include "util/error.hpp"

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string>
#include <vector>

static intmax_t string_to_intmax(const std::string& s_int) {
    std::vector<char> buffer(s_int.begin(), s_int.end());
    char* end_ptr = nullptr;
    errno = 0;
    intmax_t intmax = strtoimax(&buffer[0], &end_ptr, 10);

    if(end_ptr == &buffer[0]) {
        raise_runtime_error("String \"" + s_int + "\" is not an integer");
    }
    if(errno == ERANGE || (errno != 0 && intmax == 0)) {
        raise_runtime_error("String \"" + s_int + "\" is out of range");
    }

    return intmax;
}

static uintmax_t string_to_uintmax(const std::string& s_uint) {
    std::vector<char> buffer(s_uint.begin(), s_uint.end());
    char* end_ptr = nullptr;
    errno = 0;
    uintmax_t uintmax = strtoumax(&buffer[0], &end_ptr, 10);

    if(end_ptr == &buffer[0]) {
        raise_runtime_error("String \"" + s_uint + "\" is not an unsigned integer");
    }
    if(errno == ERANGE || (errno != 0 && uintmax == 0)) {
        raise_runtime_error("String \"" + s_uint + "\" is out of range");
    }

    return uintmax;
}

int32_t string_to_int32(const std::string& s_int32) {
    return static_cast<int32_t>(string_to_intmax(s_int32));
}

int64_t string_to_int64(const std::string& s_int64) {
    return static_cast<int64_t>(string_to_intmax(s_int64));
}

uint32_t string_to_uint32(const std::string& s_uint32) {
    return static_cast<uint32_t>(string_to_uintmax(s_uint32));
}

uint64_t string_to_uint64(const std::string& s_uint64) {
    return static_cast<uint64_t>(string_to_uintmax(s_uint64));
}

/** TODO
cdef double str_to_double(str str_double):
    cdef bytes b_str_double = str_double.encode("UTF-8")
    cdef char *c_str_double = b_str_double
    cdef char *end_ptr = NULL
    errno = 0
    cdef double val_double = strtod(c_str_double, &end_ptr)
    if end_ptr == c_str_double:

        raise RuntimeError(
            f"String \"{str_double}\" is not a floating point number")

    if (errno == ERANGE) \
       or (errno != 0 and val_double == 0):

        raise RuntimeError(
            f"String \"{str_double}\" is out of range")

    return val_double
*/

/** TODO
cdef union DoubleRepr:
    double decimal
    uint64_t binary
*/

/** TODO
cdef uint64_t double_to_binary(double decimal):
    return DoubleRepr(decimal=decimal).binary
*/