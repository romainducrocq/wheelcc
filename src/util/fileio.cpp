#include "tinydir/tinydir.h"
#include <limits.h>
#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"
#include "util/util.hpp"

#include "frontend/parser/errors.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

void set_filename(const std::string& filename) { util->filename = filename; }

void file_open_read(const std::string& filename) {
    for (size_t i = 0; i < util->file_reads.size(); ++i) {
        if (util->file_reads[i].file_descriptor) {
            size_t n_open_files = util->file_reads.size() - i;
            if (n_open_files > FOPEN_MAX) {
                RAISE_INTERNAL_ERROR;
            }
            else if (n_open_files == FOPEN_MAX) {
                util->file_reads[i].len = 0;
                free(util->file_reads[i].buffer);
                util->file_reads[i].buffer = nullptr;
                fclose(util->file_reads[i].file_descriptor);
                util->file_reads[i].file_descriptor = nullptr;
            }
            break;
        }
    }

    util->file_reads.emplace_back();
    util->file_reads.back().file_descriptor = nullptr;
    util->file_reads.back().file_descriptor = fopen(filename.c_str(), "rb");
    if (!util->file_reads.back().file_descriptor || filename.size() >= PATH_MAX) {
        raise_runtime_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_read_input_file, filename));
    }

    util->file_reads.back().len = 0;
    util->file_reads.back().buffer = nullptr;
    util->file_reads.back().filename = filename;
}

void file_open_write(const std::string& filename) {
    if (!util->file_reads.empty()) {
        RAISE_INTERNAL_ERROR;
    }

    util->file_descriptor_write = nullptr;
    util->file_descriptor_write = fopen(filename.c_str(), "wb");
    if (!util->file_descriptor_write || filename.size() >= PATH_MAX) {
        raise_runtime_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_write_to_output_file, filename));
    }

    util->write_buffer.reserve(4096);
    util->write_buffer = "";
}

bool find_file(const std::string& filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename.c_str()) != -1 && !file.is_dir;
}

bool read_line(std::string& line) {
    if (getline(&util->file_reads.back().buffer, &util->file_reads.back().len, util->file_reads.back().file_descriptor)
        == -1) {
        line = "";
        util->file_reads.back().len = 0;
        free(util->file_reads.back().buffer);
        util->file_reads.back().buffer = nullptr;
        return false;
    }

    line = util->file_reads.back().buffer;
    return true;
}

static void write_chunk(const std::string& chunk_buffer) {
    fwrite(chunk_buffer.c_str(), sizeof(char), chunk_buffer.size(), util->file_descriptor_write);
}

static void write_file(std::string&& string_stream, size_t chunk_size) {
    util->write_buffer += string_stream;
    while (util->write_buffer.size() >= chunk_size) {
        write_chunk(util->write_buffer.substr(0, chunk_size));
        util->write_buffer = util->write_buffer.substr(chunk_size, util->write_buffer.size() - chunk_size);
    }
}

void write_line(std::string&& line) {
    line += "\n";
    write_file(std::move(line), 4096);
}

void file_close_read(size_t line_number) {
    fclose(util->file_reads.back().file_descriptor);
    util->file_reads.back().file_descriptor = nullptr;
    util->file_reads.pop_back();

    if (!util->file_reads.empty() && !util->file_reads.back().file_descriptor) {
        if (util->file_reads.back().buffer || util->file_reads.back().len != 0) {
            RAISE_INTERNAL_ERROR;
        }
        util->file_reads.back().file_descriptor = fopen(util->file_reads.back().filename.c_str(), "rb");
        if (!util->file_reads.back().file_descriptor) {
            raise_runtime_error(
                GET_ERROR_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_read_input_file, util->file_reads.back().filename));
        }
        for (size_t i = 0; i < line_number; ++i) {
            if (getline(&util->file_reads.back().buffer, &util->file_reads.back().len,
                    util->file_reads.back().file_descriptor)
                == -1) {
                RAISE_INTERNAL_ERROR;
            }
        }
    }
}

void file_close_write() {
    write_chunk(util->write_buffer);
    util->write_buffer = "";

    fclose(util->file_descriptor_write);
    util->file_descriptor_write = nullptr;
}
