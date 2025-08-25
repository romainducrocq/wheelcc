#ifndef _UTIL_FILEIO_H
#define _UTIL_FILEIO_H

#include <stdio.h>

#include "util/c_std.h"
#include "util/throw.h"

typedef struct ErrorsContext ErrorsContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

typedef struct FileRead {
    size_t len;
    char* buf;
    FILE* fd;
    string_t filename;
} FileRead;

typedef struct FileIoContext {
    ErrorsContext* errors;
    // File io
    FILE* fd_write;
    string_t write_buf;
    string_t filename;
    vector_t(FileRead) file_reads;
} FileIoContext;

#ifdef __cplusplus
extern "C" {
#endif
bool find_file(const char* filename);
const char* get_filename(FileIoContext* ctx);
void set_filename(FileIoContext* ctx, const string_t filename);
error_t open_fread(FileIoContext* ctx, const string_t filename);
error_t open_fwrite(FileIoContext* ctx, const string_t filename);
bool read_line(FileIoContext* ctx, char** line, size_t* line_size);
void write_buffer(FileIoContext* ctx, const char* buf);
error_t close_fread(FileIoContext* ctx, size_t linenum);
void close_fwrite(FileIoContext* ctx);
void free_fileio(FileIoContext* ctx);
#ifdef __cplusplus
}
#endif

#endif
