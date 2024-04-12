#ifndef _UTIL_UTIL_HPP
#define _UTIL_UTIL_HPP

#include <stdio.h>
#include <string>
#include <memory>
#include <vector>

struct MainContext {
    MainContext();

    bool VERBOSE;
    // TODO change to uint32_t bitmask
    int opt_code;
    int opt_s_code;
    std::string filename;
    std::vector<std::string> args;
};

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
