#include "tinydir/tinydir.h"
#include <limits.h>
#include <memory>
#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"

std::unique_ptr<FileIoContext> fileio;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

void set_filename(const std::string& filename) { fileio->filename = filename; }

void open_fread(const std::string& filename) {
    for (size_t i = 0; i < fileio->file_reads.size(); ++i) {
        if (fileio->file_reads[i].fd) {
            size_t n_fopens = fileio->file_reads.size() - i;
            THROW_ABORT_IF(n_fopens > FOPEN_MAX);
            if (n_fopens == FOPEN_MAX) {
                fileio->file_reads[i].len = 0;
                free(fileio->file_reads[i].buf);
                fileio->file_reads[i].buf = nullptr;
                fclose(fileio->file_reads[i].fd);
                fileio->file_reads[i].fd = nullptr;
            }
            break;
        }
    }

    fileio->file_reads.emplace_back();
    fileio->file_reads.back().fd = nullptr;
    fileio->file_reads.back().fd = fopen(filename.c_str(), "rb");
    if (!fileio->file_reads.back().fd || filename.size() >= PATH_MAX) {
        THROW_IO(GET_UTIL_MSG(MSG_failed_fread, filename.c_str()));
    }

    fileio->file_reads.back().len = 0;
    fileio->file_reads.back().buf = nullptr;
    fileio->file_reads.back().filename = filename;
}

void open_fwrite(const std::string& filename) {
    THROW_ABORT_IF(!fileio->file_reads.empty());

    fileio->fd_write = nullptr;
    fileio->fd_write = fopen(filename.c_str(), "wb");
    if (!fileio->fd_write || filename.size() >= PATH_MAX) {
        THROW_IO(GET_UTIL_MSG(MSG_failed_fwrite, filename.c_str()));
    }

    fileio->write_buf.reserve(4096);
    fileio->write_buf = "";
}

bool find_file(const std::string& filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename.c_str()) != -1 && !file.is_dir;
}

bool read_line(std::string& line) {
    if (getline(&fileio->file_reads.back().buf, &fileio->file_reads.back().len, fileio->file_reads.back().fd) == -1) {
        line = "";
        fileio->file_reads.back().len = 0;
        free(fileio->file_reads.back().buf);
        fileio->file_reads.back().buf = nullptr;
        return false;
    }

    line = fileio->file_reads.back().buf;
    return true;
}

static void write_chunk(const std::string& chunk_buf) {
    fwrite(chunk_buf.c_str(), sizeof(char), chunk_buf.size(), fileio->fd_write);
}

static void write_file(std::string&& stream_buf, size_t chunk_size) {
    fileio->write_buf += stream_buf;
    while (fileio->write_buf.size() >= chunk_size) {
        write_chunk(fileio->write_buf.substr(0, chunk_size));
        fileio->write_buf = fileio->write_buf.substr(chunk_size, fileio->write_buf.size() - chunk_size);
    }
}

void write_line(std::string&& line) {
    line += "\n";
    write_file(std::move(line), 4096);
}

void close_fread(size_t linenum) {
    fclose(fileio->file_reads.back().fd);
    fileio->file_reads.back().fd = nullptr;
    fileio->file_reads.pop_back();

    if (!fileio->file_reads.empty() && !fileio->file_reads.back().fd) {
        THROW_ABORT_IF(fileio->file_reads.back().buf || fileio->file_reads.back().len != 0);
        fileio->file_reads.back().fd = fopen(fileio->file_reads.back().filename.c_str(), "rb");
        if (!fileio->file_reads.back().fd) {
            THROW_IO(GET_UTIL_MSG(MSG_failed_fread, fileio->file_reads.back().filename.c_str()));
        }
        for (size_t i = 0; i < linenum; ++i) {
            if (getline(&fileio->file_reads.back().buf, &fileio->file_reads.back().len, fileio->file_reads.back().fd)
                == -1) {
                THROW_IO(GET_UTIL_MSG(MSG_failed_fread, fileio->file_reads.back().filename.c_str()));
            }
        }
    }
}

void close_fwrite() {
    write_chunk(fileio->write_buf);
    fileio->write_buf = "";

    fclose(fileio->fd_write);
    fileio->fd_write = nullptr;
}
