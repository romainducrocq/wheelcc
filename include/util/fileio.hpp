#ifndef _UTIL_FILEIO_H
#define _UTIL_FILEIO_H

#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

struct FileRead {
    size_t len;
    char* buf;
    FILE* fd;
    std::string filename;
};

struct FileIoContext {
    FILE* fd_write;
    std::string write_buf;
    std::string filename;
    std::vector<FileRead> freads;
};

extern std::unique_ptr<FileIoContext> fileio;
#define INIT_FILEIO_CTX fileio = std::make_unique<FileIoContext>()
#define FREE_FILEIO_CTX fileio.reset()

void set_filename(const std::string& filename);
void open_fread(const std::string& filename);
void open_fwrite(const std::string& filename);
bool find_file(const std::string& filename);
bool read_line(std::string& line);
void write_line(std::string&& line);
void close_fread(size_t linenum);
void close_fwrite();

#endif
