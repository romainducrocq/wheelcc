#include "util/fopen.hpp"
#include "util/error.hpp"

#include <stdio.h>
#include <string>

static FILE* file_in = nullptr;

void file_open_read(const std::string& filename) {
    file_in = nullptr;

    file_in = fopen(filename.c_str(), "rb");
    if(file_in == nullptr) {
        raise_runtime_error("File \"" + filename + "\" does not exist");
    }
}

bool read_line(std::string& line) {
    size_t l = 0;
    char* buffer = nullptr;

    if(getline(&buffer, &l, file_in) == -1) {
        line = "";
        return false;
    }

    line = buffer;
    return true;
}

void file_close_read() {
    fclose(file_in);
}
