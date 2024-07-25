#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"
#include "util/util.hpp"

#include "frontend/parser/errors.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

void file_open_read(const std::string& filename) {
    util->file_in = nullptr;

    util->file_in = fopen(filename.c_str(), "rb");
    if (util->file_in == nullptr) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE::failed_to_read_output_file, filename));
    }

    util->l = 0;
    util->buffer = nullptr;
    util->filename = filename;
}

void file_open_write(const std::string& filename) {
    util->file_out = nullptr;

    util->file_out = fopen(filename.c_str(), "wb");
    if (util->file_out == nullptr) {
        raise_runtime_error(GET_ERROR_MESSAGE(ERROR_MESSAGE::failed_to_write_to_output_file, filename));
    }
    util->stream_buf = "";
}

bool read_line(std::string& line) {
    if (getline(&util->buffer, &util->l, util->file_in) == -1) {
        util->l = 0;
        line = "";
        free(util->buffer);
        util->buffer = nullptr;
        return false;
    }

    line = util->buffer;
    return true;
}

static void write_chunk(const std::string& chunk_fp, size_t chunk_l) {
    fwrite(chunk_fp.c_str(), sizeof(char), chunk_l, util->file_out);
}

static void write_file(std::string&& stream, size_t chunk_size) {
    util->stream_buf += stream;
    while (util->stream_buf.size() >= chunk_size) {
        write_chunk(util->stream_buf.substr(0, chunk_size), chunk_size);
        util->stream_buf = util->stream_buf.substr(chunk_size, util->stream_buf.size() - chunk_size);
    }
}

void write_line(std::string&& line) { write_file(line + "\n", 4096); }

void file_close_read() {
    fclose(util->file_in);
    util->file_in = nullptr;
}

void file_close_write() {
    write_chunk(util->stream_buf, util->stream_buf.size());
    util->stream_buf = "";

    fclose(util->file_out);
    util->file_out = nullptr;
}
