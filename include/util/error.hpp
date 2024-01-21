#ifndef _UTIL_ERROR_HPP
#define _UTIL_ERROR_HPP

#include <string>

namespace error
{
    void raise_runtime_error(const std::string message);
}

#endif