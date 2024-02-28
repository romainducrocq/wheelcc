#include "util/fileio.hpp"
#include "util/error.hpp"

#include <stdio.h>
#include <string>

static size_t line_number = 0;
static std::string stream_buf = "";

static FILE* file_in = nullptr;
static FILE* file_out = nullptr;

size_t get_line_number() {
    return line_number;
}

void file_open_read(const std::string& filename) {
    file_in = nullptr;

    file_in = fopen(filename.c_str(), "rb");
    if(file_in == nullptr) {
        raise_runtime_error("File " + em(filename) + " does not exist");
    }
    set_filename(filename);
}

void file_open_write(const std::string& filename) {
    file_out = nullptr;

    file_out = fopen(filename.c_str(), "wb");
    if(file_out == nullptr) {
        raise_runtime_error("File " + em(filename) + " was not created");
    }
    stream_buf = "";
}

// https://stackoverflow.com/questions/64390291/c-getline-memory-leak-different-behaviours

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

static void write_chunk(const std::string& chunk_fp, size_t chunk_l) {
    fwrite(chunk_fp.c_str(), sizeof(char), chunk_l, file_out);
}

static void write_file(std::string&& stream, size_t chunk_size) {
    stream_buf += stream;
    while(stream_buf.size() >= chunk_size) {
        write_chunk(stream_buf.substr(0, chunk_size), chunk_size);
        stream_buf = stream_buf.substr(chunk_size, stream_buf.size() - chunk_size);
    }
}

void write_line(std::string&& line) {
    write_file(line + "\n", 4096);
}

void file_close_read() {
    fclose(file_in);
}

void file_close_write() {
    write_chunk(stream_buf, stream_buf.size());
    fclose(file_out);
}
