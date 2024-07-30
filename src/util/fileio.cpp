#include "tinydir/tinydir.h"
#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"
#include "util/util.hpp"

#include "frontend/parser/errors.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

void directory_open(const std::string& dirname) {
    util->tiny_dir = {};
    if (tinydir_open(&util->tiny_dir, dirname.c_str()) == -1) {
        raise_runtime_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_open_directory, dirname));
    }
    util->is_dir_open = true;
}

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

bool list_file(std::string& filename) {
    if (!util->tiny_dir.has_next) {
        return false;
    }
    tinydir_file file = {};
    if (tinydir_readfile(&util->tiny_dir, &file) == -1) {
        return false;
    }
    filename = file.is_dir ? "" : file.name;
    tinydir_next(&util->tiny_dir);
    return true;
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

void directory_close() {
    tinydir_close(&util->tiny_dir);
    util->is_dir_open = false;
}

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
