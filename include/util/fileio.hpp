#ifndef _UTIL_FILEIO_H
#define _UTIL_FILEIO_H

#include <stdio.h>
#include <string>
#include <vector>

struct ErrorsContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

struct FileRead {
    size_t len;
    char* buf;
    FILE* fd;
    std::string filename;
};

struct FileIoContext {
    ErrorsContext* errors;
    // File io
    FILE* fd_write;
    std::string write_buf;
    std::string filename;
    std::vector<FileRead> file_reads;
};

bool find_file(const std::string& filename);
const std::string& get_filename(FileIoContext* ctx);
void set_filename(FileIoContext* ctx, const std::string& filename);
void open_fread(FileIoContext* ctx, const std::string& filename);
void open_fwrite(FileIoContext* ctx, const std::string& filename);
bool read_line(FileIoContext* ctx, std::string& line);
void write_line(FileIoContext* ctx, std::string&& line);
void close_fread(FileIoContext* ctx, size_t linenum);
void close_fwrite(FileIoContext* ctx);
void free_fileio(FileIoContext* ctx);

#endif
