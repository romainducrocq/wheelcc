#ifndef _UTIL_UTIL_HPP
#define _UTIL_UTIL_HPP

#include <inttypes.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Util

struct MainContext {
    MainContext();

    bool is_verbose;
    uint8_t debug_code;
    uint8_t optim_1_mask;
    uint8_t optim_2_code;
    std::string filename;
    std::vector<std::string> includedirs;
    std::vector<std::string> args;
};

#endif
