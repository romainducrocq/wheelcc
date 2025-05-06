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
    for (size_t i = 0; i < fileio->freads.size(); ++i) {
        if (fileio->freads[i].fd) {
            size_t n_open_files = fileio->freads.size() - i;
            if (n_open_files > FOPEN_MAX) {
                RAISE_INTERNAL_ERROR;
            }
            else if (n_open_files == FOPEN_MAX) {
                fileio->freads[i].len = 0;
                free(fileio->freads[i].buf);
                fileio->freads[i].buf = nullptr;
                fclose(fileio->freads[i].fd);
                fileio->freads[i].fd = nullptr;
            }
            break;
        }
    }

    fileio->freads.emplace_back();
    fileio->freads.back().fd = nullptr;
    fileio->freads.back().fd = fopen(filename.c_str(), "rb");
    if (!fileio->freads.back().fd || filename.size() >= PATH_MAX) {
        RAISE_RUNTIME_ERROR(GET_UTIL_MSG(MSG_failed_fread, filename.c_str()));
    }

    fileio->freads.back().len = 0;
    fileio->freads.back().buf = nullptr;
    fileio->freads.back().filename = filename;
}

void open_fwrite(const std::string& filename) {
    if (!fileio->freads.empty()) {
        RAISE_INTERNAL_ERROR;
    }

    fileio->fd_write = nullptr;
    fileio->fd_write = fopen(filename.c_str(), "wb");
    if (!fileio->fd_write || filename.size() >= PATH_MAX) {
        RAISE_RUNTIME_ERROR(GET_UTIL_MSG(MSG_failed_fwrite, filename.c_str()));
    }

    fileio->write_buf.reserve(4096);
    fileio->write_buf = "";
}

bool find_file(const std::string& filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename.c_str()) != -1 && !file.is_dir;
}

bool read_line(std::string& line) {
    if (getline(&fileio->freads.back().buf, &fileio->freads.back().len, fileio->freads.back().fd) == -1) {
        line = "";
        fileio->freads.back().len = 0;
        free(fileio->freads.back().buf);
        fileio->freads.back().buf = nullptr;
        return false;
    }

    line = fileio->freads.back().buf;
    return true;
}

static void write_chunk(const std::string& chunk_buffer) {
    fwrite(chunk_buffer.c_str(), sizeof(char), chunk_buffer.size(), fileio->fd_write);
}

static void write_file(std::string&& string_stream, size_t chunk_size) {
    fileio->write_buf += string_stream;
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
    fclose(fileio->freads.back().fd);
    fileio->freads.back().fd = nullptr;
    fileio->freads.pop_back();

    if (!fileio->freads.empty() && !fileio->freads.back().fd) {
        if (fileio->freads.back().buf || fileio->freads.back().len != 0) {
            RAISE_INTERNAL_ERROR;
        }
        fileio->freads.back().fd = fopen(fileio->freads.back().filename.c_str(), "rb");
        if (!fileio->freads.back().fd) {
            RAISE_RUNTIME_ERROR(GET_UTIL_MSG(MSG_failed_fread, fileio->freads.back().filename.c_str()));
        }
        for (size_t i = 0; i < linenum; ++i) {
            if (getline(&fileio->freads.back().buf, &fileio->freads.back().len, fileio->freads.back().fd) == -1) {
                RAISE_INTERNAL_ERROR;
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
