#ifndef _UTIL_UTIL_HPP
#define _UTIL_UTIL_HPP

#include <stdio.h>
#include <string>
#include <memory>

struct UtilContext {
    std::string filename_in;
    // fileio
    size_t line_number;
    std::string stream_buf;
    size_t l;
    char* buffer;
    FILE* file_in;
    FILE* file_out;
};

extern std::unique_ptr<UtilContext> util;

#endif
