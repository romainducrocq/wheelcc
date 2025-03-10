#ifndef _UTIL_FILEIO_HPP
#define _UTIL_FILEIO_HPP

#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

struct FileRead {
    size_t len;
    char* buffer;
    FILE* file_descriptor;
    std::string filename;
};

struct FileIoContext {
    FILE* file_descriptor_write;
    std::string write_buffer;
    std::string filename;
    std::vector<FileRead> file_reads;
};

extern std::unique_ptr<FileIoContext> fileio;
#define INIT_FILEIO_CONTEXT fileio = std::make_unique<FileIoContext>()
#define FREE_FILEIO_CONTEXT fileio.reset()

void set_filename(const std::string& filename);
void file_open_read(const std::string& filename);
void file_open_write(const std::string& filename);
bool find_file(const std::string& filename);
bool read_line(std::string& line);
void write_line(std::string&& line);
void file_close_read(size_t line_number);
void file_close_write();

#endif
