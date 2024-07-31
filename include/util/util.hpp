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

    bool is_verbose;
    int debug_code;
    int optim_mask;
    std::string filename;
    std::vector<std::string> includedirs;
    std::vector<std::string> args;
};

struct FileRead {
    size_t len;
    char* buffer;
    FILE* file_descriptor;
    std::string filename;
};

struct UtilContext {
    // File io
    FILE* file_descriptor_write;
    std::string write_buffer;
    std::string filename;
    std::vector<FileRead> file_reads;
};

extern std::unique_ptr<UtilContext> util;
#define INIT_UTIL_CONTEXT util = std::make_unique<UtilContext>()
#define FREE_UTIL_CONTEXT util.reset()

#endif
