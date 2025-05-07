#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"

std::unique_ptr<ErrorsContext> errors;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

static void free_fileio() {
    for (auto& file_read : fileio->file_reads) {
        if (file_read.buf != nullptr) {
            free(file_read.buf);
            file_read.buf = nullptr;
        }
        if (file_read.fd != nullptr) {
            fclose(file_read.fd);
            file_read.fd = nullptr;
        }
    }
    if (fileio->fd_write != nullptr) {
        fclose(fileio->fd_write);
        fileio->fd_write = nullptr;
    }
}

static const std::string& get_filename() {
    if (!fileio->file_reads.empty()) {
        return fileio->file_reads.back().filename;
    }
    else {
        return fileio->filename;
    }
}

size_t handle_error_at_line(size_t total_linenum) {
    for (size_t i = 0; i < errors->fopen_lines.size() - 1; ++i) {
        if (total_linenum < errors->fopen_lines[i + 1].total_linenum) {
            set_filename(errors->fopen_lines[i].filename);
            return total_linenum - errors->fopen_lines[i].total_linenum + errors->fopen_lines[i].linenum;
        }
    }
    set_filename(errors->fopen_lines.back().filename);
    return total_linenum - errors->fopen_lines.back().total_linenum + errors->fopen_lines.back().linenum;
}

[[noreturn]] void raise_internal_error(const char* func, const char* file, int line) {
    free_fileio();
    std::string msg = "\033[1m";
    msg += std::string(file);
    msg += ":";
    msg += std::to_string(line);
    msg += ":\033[0m\n\033[0;31minternal error:\033[0m ";
    msg += std::string(func);
    throw std::runtime_error(msg);
}

[[noreturn]] void raise_base_error(const char* error_msg) {
    std::string msg = "\033[0;31merror:\033[0m ";
    msg += std::string(error_msg);
    throw std::runtime_error(msg);
}

[[noreturn]] void raise_runtime_error(const char* error_msg) {
    free_fileio();
    const std::string& filename = get_filename();
    std::string msg = "\033[1m";
    msg += filename;
    msg += ":\033[0m\n\033[0;31merror:\033[0m ";
    msg += std::string(error_msg);
    throw std::runtime_error(msg);
}

[[noreturn]] void raise_runtime_error_at_line(const char* error_msg, size_t linenum) {
    if (linenum == 0) {
        raise_runtime_error(error_msg);
    }
    free_fileio();
    const std::string& filename = get_filename();
    std::string line;
    {
        size_t len = 0;
        char* buf = nullptr;
        FILE* fd = fopen(filename.c_str(), "rb");
        if (!fd) {
            raise_runtime_error(error_msg);
        }
        for (size_t i = 0; i < linenum; ++i) {
            if (getline(&buf, &len, fd) == -1) {
                free(buf);
                fclose(fd);
                buf = nullptr;
                fd = nullptr;
                raise_runtime_error(error_msg);
            }
        }
        line = buf;
        free(buf);
        fclose(fd);
        buf = nullptr;
        fd = nullptr;
        if (line.back() == '\n') {
            line.pop_back();
        }
    }
    std::string msg = "\033[1m";
    msg += filename;
    msg += ":";
    msg += std::to_string(linenum);
    msg += ":\033[0m\n\033[0;31merror:\033[0m ";
    msg += std::string(error_msg);
    msg += "\nat line ";
    msg += std::to_string(linenum);
    msg += ": \033[1m";
    msg += line;
    msg += "\033[0m";
    throw std::runtime_error(msg);
}
