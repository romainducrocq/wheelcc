#include "util/fopen.hpp"
#include "util/error.hpp"

#include <stdio.h>
#include <string>

static FILE* file_in = nullptr;
static size_t line_number = 0;

void file_open_read(const std::string& filename) {
    file_in = nullptr;

    file_in = fopen(filename.c_str(), "rb");
    if(file_in == nullptr) {
        raise_runtime_error("File \"" + filename + "\" does not exist");
    }
    set_filename(filename);
}

bool read_line(std::string& line) {
    size_t l = 0;
    char* buffer = nullptr;

    if(getline(&buffer, &l, file_in) == -1) {
        line = "";
        return false;
    }

    line = buffer;
    line_number++;
    return true;
}

size_t get_line_number() {
    return line_number;
}

void file_close_read() {
    fclose(file_in);
}
