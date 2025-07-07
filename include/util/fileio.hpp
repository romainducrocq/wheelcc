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
    char* filename; /* string_t */
};

struct FileIoContext {
    ErrorsContext* errors;
    // File io
    FILE* fd_write;
    std::string write_buf;
    char* filename; /* string_t */
    std::vector<FileRead> file_reads;
};

bool find_file(const char* filename);
const char* get_filename(FileIoContext* ctx);
void set_filename(FileIoContext* ctx, char* filename);
int open_fread(FileIoContext* ctx, char* filename);
int open_fwrite(FileIoContext* ctx, char* filename);
bool read_line(FileIoContext* ctx, char*& line, size_t& line_size);
void write_buffer(FileIoContext* ctx, const char* buf);
int close_fread(FileIoContext* ctx, size_t linenum);
void close_fwrite(FileIoContext* ctx);
void free_fileio(FileIoContext* ctx);

#endif
