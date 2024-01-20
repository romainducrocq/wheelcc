#ifndef _UTIL_FOPEN_HPP
#define _UTIL_FOPEN_HPP

#include <string>

namespace Fopen
{
    void file_open_read(const std::string& filename);
    bool read_line(std::string& line);
    void file_close_read();
}

#endif