#ifndef _UTIL_UTIL_HPP
#define _UTIL_UTIL_HPP

#include <stdio.h>
#include <string>
#include <memory>

struct UtilContext {
    size_t l;
    char* buffer;
    FILE* file_in;
    FILE* file_out;
    std::string stream_buf;
    std::string filename_in;
};

extern std::unique_ptr<UtilContext> util;

#endif
