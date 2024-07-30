#include "tinydir/tinydir.h"
#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"
#include "util/util.hpp"

#include "frontend/parser/errors.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

void file_open_read(const std::string& filename) {
    util->file_read = nullptr;

    util->file_read = fopen(filename.c_str(), "rb");
    if (util->file_read == nullptr) {
        raise_runtime_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_read_output_file, filename));
    }

    util->read_len = 0;
    util->read_buf = nullptr;
    util->filename = filename;
}

void file_open_write(const std::string& filename) {
    util->file_write = nullptr;

    util->file_write = fopen(filename.c_str(), "wb");
    if (util->file_write == nullptr) {
        raise_runtime_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_write_to_output_file, filename));
    }
    util->write_buf = "";
}

bool find_file(const std::string& filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename.c_str()) != -1 && !file.is_dir;
}

bool read_line(std::string& line) {
    if (getline(&util->read_buf, &util->read_len, util->file_read) == -1) {
        util->read_len = 0;
        line = "";
        free(util->read_buf);
        util->read_buf = nullptr;
        return false;
    }

    line = util->read_buf;
    return true;
}

static void write_chunk(const std::string& chunk_fp, size_t chunk_l) {
    fwrite(chunk_fp.c_str(), sizeof(char), chunk_l, util->file_write);
}

static void write_file(std::string&& string_stream, size_t chunk_size) {
    util->write_buf += string_stream;
    while (util->write_buf.size() >= chunk_size) {
        write_chunk(util->write_buf.substr(0, chunk_size), chunk_size);
        util->write_buf = util->write_buf.substr(chunk_size, util->write_buf.size() - chunk_size);
    }
}

void write_line(std::string&& line) { write_file(line + "\n", 4096); }

void file_close_read() {
    fclose(util->file_read);
    util->file_read = nullptr;
}

void file_close_write() {
    write_chunk(util->write_buf, util->write_buf.size());
    util->write_buf = "";

    fclose(util->file_write);
    util->file_write = nullptr;
}
