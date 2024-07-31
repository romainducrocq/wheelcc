#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "util/throw.hpp"
#include "util/util.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Throw

static void free_resources() {
    for (auto& file_read : util->file_reads) {
        if (file_read.buffer != nullptr) {
            free(file_read.buffer);
            file_read.buffer = nullptr;
        }
        if (file_read.file_descriptor != nullptr) {
            fclose(file_read.file_descriptor);
            file_read.file_descriptor = nullptr;
        }
    }
    if (util->file_descriptor_write != nullptr) {
        fclose(util->file_descriptor_write);
        util->file_descriptor_write = nullptr;
    }
}

static const std::string& get_filename() {
    if (!util->file_reads.empty()) {
        return util->file_reads.back().filename;
    }
    else {
        return util->filename;
    }
}

std::string em(const std::string& message) { return "\033[1m‘" + message + "’\033[0m"; }

[[noreturn]] void raise_argument_error(const std::string& message) {
    throw std::runtime_error("\n\033[0;31merror:\033[0m " + message + "\n");
}

[[noreturn]] void raise_runtime_error(const std::string& message) {
    free_resources();
    throw std::runtime_error("\n\033[1m" + get_filename() + ":\033[0m\n\033[0;31merror:\033[0m " + message + "\n");
}

[[noreturn]] void raise_runtime_error_at_line(const std::string& message, size_t line_number) {
    if (line_number == 0) {
        raise_runtime_error(message);
    }
    free_resources();
    const std::string& filename = get_filename();
    std::string line;
    {
        size_t len = 0;
        char* buffer = nullptr;
        FILE* file_descriptor = fopen(filename.c_str(), "rb");
        if (file_descriptor == nullptr) {
            raise_runtime_error(message);
        }
        for (size_t i = 0; i < line_number; ++i) {
            if (getline(&buffer, &len, file_descriptor) == -1) {
                free(buffer);
                fclose(file_descriptor);
                buffer = nullptr;
                file_descriptor = nullptr;
                raise_runtime_error(message);
            }
        }
        line = buffer;
        free(buffer);
        fclose(file_descriptor);
        buffer = nullptr;
        file_descriptor = nullptr;
        if (line.back() == '\n') {
            line.pop_back();
        }
    }
    throw std::runtime_error("\n\033[1m" + filename + ":" + std::to_string(line_number)
                             + ":\033[0m\n\033[0;31merror:\033[0m " + message + "\nat line "
                             + std::to_string(line_number) + ": \033[1m" + line + "\033[0m");
}

[[noreturn]] void raise_internal_error(const char* func, const char* file, int line) {
    free_resources();
    throw std::runtime_error("\n\033[1m" + std::string(file) + ":" + std::to_string(line)
                             + ":\033[0m\n\033[0;31minternal error:\033[0m " + std::string(func));
}
