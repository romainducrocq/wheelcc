#ifndef _UTIL_FILEIO_HPP
#define _UTIL_FILEIO_HPP

#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

void set_filename(const std::string& filename);
void file_open_read(const std::string& filename);
void file_open_write(const std::string& filename);
bool find_file(const std::string& filename);
bool read_line(std::string& line);
void write_line(std::string&& line);
void file_close_read();
void file_close_write();

#endif
