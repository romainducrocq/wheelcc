#include "util/fopen.hpp"

#include <stdio.h>
#include <string>
#include <stdexcept>

namespace Fopen {

static FILE* file_in = nullptr;

}

void Fopen::file_open_read(const std::string& filename) {
    file_in = nullptr;

    file_in = fopen(filename.c_str(), "rb");
    if(file_in == nullptr) {
        throw std::runtime_error("File " + filename + " does not exist\n");
    }
}

bool Fopen::read_line(std::string& line) {
    size_t l = 0;
    char* buffer = nullptr;

    if(getline(&buffer, &l, file_in) == -1) {
        line = "";
        return false;
    }

    line = buffer;
    return true;
}

void Fopen::file_close_read() {
    fclose(file_in);
}
