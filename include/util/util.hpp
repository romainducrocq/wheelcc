#ifndef _UTIL_UTIL_HPP
#define _UTIL_UTIL_HPP

#include "tinydir/tinydir.h"
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Util

struct MainContext {
    MainContext();

    bool is_verbose;
    int debug_code;
    int optim_mask;
    std::string filename;
    std::vector<std::string> args;
};

struct UtilContext {
    std::string filename;
    // File io
    bool is_dir_open;
    tinydir_dir tiny_dir;
    size_t read_len;
    char* read_buf;
    FILE* file_read;
    FILE* file_write;
    std::string write_buf;
};

extern std::unique_ptr<UtilContext> util;
#define INIT_UTIL_CONTEXT util = std::make_unique<UtilContext>()
#define FREE_UTIL_CONTEXT util.reset()

#endif
