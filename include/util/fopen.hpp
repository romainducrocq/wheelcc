#ifndef _UTIL_FOPEN_HPP
#define _UTIL_FOPEN_HPP

#include <string>

void file_open_read(const std::string& filename);
bool read_line(std::string& line);
size_t get_line_number();
void file_close_read();

#endif
