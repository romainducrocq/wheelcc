#ifndef _UTIL_UTIL_HPP
#define _UTIL_UTIL_HPP

#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Util

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
    std::string filename;
    // File io
    size_t l;
    char* buffer;
    FILE* file_in;
    FILE* file_out;
    std::string stream_buf;
};

extern std::unique_ptr<UtilContext> util;
#define INIT_UTIL_CONTEXT util = std::make_unique<UtilContext>()
#define FREE_UTIL_CONTEXT util.reset()

#endif
