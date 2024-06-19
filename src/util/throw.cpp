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

static void file_free() {
    if (util->buffer != nullptr) {
        free(util->buffer);
        util->buffer = nullptr;
    }
    if (util->file_in != nullptr) {
        fclose(util->file_in);
        util->file_in = nullptr;
    }
    if (util->file_out != nullptr) {
        fclose(util->file_out);
        util->file_out = nullptr;
    }
}

std::string em(const std::string& message) { return "\033[1m‘" + message + "’\033[0m"; }

[[noreturn]] void raise_runtime_error(const std::string& message) {
    file_free();
    throw std::runtime_error("\n\033[1m" + util->filename + ":\033[0m\n\033[0;31merror:\033[0m " + message + "\n");
}

[[noreturn]] void raise_runtime_error_at_line(const std::string& message, size_t line_number) {
    file_free();
    std::string line;
    {
        size_t l = 0;
        char* buffer = nullptr;
        FILE* file_in = fopen(util->filename.c_str(), "rb");
        if (file_in == nullptr) {
            raise_runtime_error(message);
        }
        for(size_t i = 0; i < line_number; i++) {
            if(getline(&buffer, &l, file_in) == -1) {
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
    }
    throw std::runtime_error("\n\033[1m" + util->filename + ":" + std::to_string(line_number)
                             + ":\033[0m\n\033[0;31merror:\033[0m " + message + "\nat line "
                             + std::to_string(line_number) + ": \033[1m" + line + "\033[0m");
}

[[noreturn]] void raise_internal_error(const char* func, const char* file, int line) {
    file_free();
    throw std::runtime_error("\n\033[1m" + std::string(file) + ":" + std::to_string(line)
                             + ":\033[0m\n\033[0;31minternal error:\033[0m " + std::string(func));
}
