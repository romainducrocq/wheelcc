#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "tinydir/tinydir.h"

#include "util/throw.hpp"
#include "util/util.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

static void free_resources() {
    if (util->read_buf != nullptr) {
        free(util->read_buf);
        util->read_buf = nullptr;
    }
    if (util->file_read != nullptr) {
        fclose(util->file_read);
        util->file_read = nullptr;
    }
    if (util->file_write != nullptr) {
        fclose(util->file_write);
        util->file_write = nullptr;
    }
    if(util->is_dir_open) {
        tinydir_close(&util->tiny_dir);
        util->is_dir_open = false;
    }
}

std::string em(const std::string& message) { return "\033[1m‘" + message + "’\033[0m"; }

[[noreturn]] void raise_argument_error(const std::string& message) {
    throw std::runtime_error("\n\033[0;31merror:\033[0m " + message + "\n");
}

[[noreturn]] void raise_runtime_error(const std::string& message) {
    free_resources();
    throw std::runtime_error("\n\033[1m" + util->filename + ":\033[0m\n\033[0;31merror:\033[0m " + message + "\n");
}

[[noreturn]] void raise_runtime_error_at_line(const std::string& message, size_t line_number) {
    if (line_number == 0) {
        raise_runtime_error(message);
    }
    free_resources();
    std::string line;
    {
        size_t l = 0;
        char* buffer = nullptr;
        FILE* file_in = fopen(util->filename.c_str(), "rb");
        if (file_in == nullptr) {
            raise_runtime_error(message);
        }
        for (size_t i = 0; i < line_number; ++i) {
            if (getline(&buffer, &l, file_in) == -1) {
                free(buffer);
                fclose(file_in);
                buffer = nullptr;
                file_in = nullptr;
                raise_runtime_error(message);
            }
        }
        line = buffer;
        free(buffer);
        fclose(file_in);
        buffer = nullptr;
        file_in = nullptr;
        if (line.back() == '\n') {
            line.pop_back();
        }
    }
    throw std::runtime_error("\n\033[1m" + util->filename + ":" + std::to_string(line_number)
                             + ":\033[0m\n\033[0;31merror:\033[0m " + message + "\nat line "
                             + std::to_string(line_number) + ": \033[1m" + line + "\033[0m");
}

[[noreturn]] void raise_internal_error(const char* func, const char* file, int line) {
    free_resources();
    throw std::runtime_error("\n\033[1m" + std::string(file) + ":" + std::to_string(line)
                             + ":\033[0m\n\033[0;31minternal error:\033[0m " + std::string(func));
}
