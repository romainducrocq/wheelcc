#include "tinydir/tinydir.h"
#include <limits.h>
#include <memory>
#include <stdio.h>
#include <string>

#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "frontend/parser/errors.hpp"

std::unique_ptr<FileIoContext> fileio;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File io

void set_filename(const std::string& filename) { fileio->filename = filename; }

void file_open_read(const std::string& filename) {
    for (size_t i = 0; i < fileio->file_reads.size(); ++i) {
        if (fileio->file_reads[i].file_descriptor) {
            size_t n_open_files = fileio->file_reads.size() - i;
            if (n_open_files > FOPEN_MAX) {
                RAISE_INTERNAL_ERROR;
            }
            else if (n_open_files == FOPEN_MAX) {
                fileio->file_reads[i].len = 0;
                free(fileio->file_reads[i].buffer);
                fileio->file_reads[i].buffer = nullptr;
                fclose(fileio->file_reads[i].file_descriptor);
                fileio->file_reads[i].file_descriptor = nullptr;
            }
            break;
        }
    }

    fileio->file_reads.emplace_back();
    fileio->file_reads.back().file_descriptor = nullptr;
    fileio->file_reads.back().file_descriptor = fopen(filename.c_str(), "rb");
    if (!fileio->file_reads.back().file_descriptor || filename.size() >= PATH_MAX) {
        RAISE_RUNTIME_ERROR(GET_UTIL_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_read_input_file, filename.c_str()));
    }

    fileio->file_reads.back().len = 0;
    fileio->file_reads.back().buffer = nullptr;
    fileio->file_reads.back().filename = filename;
}

void file_open_write(const std::string& filename) {
    if (!fileio->file_reads.empty()) {
        RAISE_INTERNAL_ERROR;
    }

    fileio->file_descriptor_write = nullptr;
    fileio->file_descriptor_write = fopen(filename.c_str(), "wb");
    if (!fileio->file_descriptor_write || filename.size() >= PATH_MAX) {
        RAISE_RUNTIME_ERROR(GET_UTIL_MESSAGE(ERROR_MESSAGE_UTIL::failed_to_write_to_output_file, filename.c_str()));
    }

    fileio->write_buffer.reserve(4096);
    fileio->write_buffer = "";
}

bool find_file(const std::string& filename) {
    tinydir_file file = {};
    return tinydir_file_open(&file, filename.c_str()) != -1 && !file.is_dir;
}

bool read_line(std::string& line) {
    if (getline(&fileio->file_reads.back().buffer, &fileio->file_reads.back().len,
            fileio->file_reads.back().file_descriptor)
        == -1) {
        line = "";
        fileio->file_reads.back().len = 0;
        free(fileio->file_reads.back().buffer);
        fileio->file_reads.back().buffer = nullptr;
        return false;
    }

    line = fileio->file_reads.back().buffer;
    return true;
}

static void write_chunk(const std::string& chunk_buffer) {
    fwrite(chunk_buffer.c_str(), sizeof(char), chunk_buffer.size(), fileio->file_descriptor_write);
}

static void write_file(std::string&& string_stream, size_t chunk_size) {
    fileio->write_buffer += string_stream;
    while (fileio->write_buffer.size() >= chunk_size) {
        write_chunk(fileio->write_buffer.substr(0, chunk_size));
        fileio->write_buffer = fileio->write_buffer.substr(chunk_size, fileio->write_buffer.size() - chunk_size);
    }
}

void write_line(std::string&& line) {
    line += "\n";
    write_file(std::move(line), 4096);
}

void file_close_read(size_t line_number) {
    fclose(fileio->file_reads.back().file_descriptor);
    fileio->file_reads.back().file_descriptor = nullptr;
    fileio->file_reads.pop_back();

    if (!fileio->file_reads.empty() && !fileio->file_reads.back().file_descriptor) {
        if (fileio->file_reads.back().buffer || fileio->file_reads.back().len != 0) {
            RAISE_INTERNAL_ERROR;
        }
        fileio->file_reads.back().file_descriptor = fopen(fileio->file_reads.back().filename.c_str(), "rb");
        if (!fileio->file_reads.back().file_descriptor) {
            RAISE_RUNTIME_ERROR(GET_UTIL_MESSAGE(
                ERROR_MESSAGE_UTIL::failed_to_read_input_file, fileio->file_reads.back().filename.c_str()));
        }
        for (size_t i = 0; i < line_number; ++i) {
            if (getline(&fileio->file_reads.back().buffer, &fileio->file_reads.back().len,
                    fileio->file_reads.back().file_descriptor)
                == -1) {
                RAISE_INTERNAL_ERROR;
            }
        }
    }
}

void file_close_write() {
    write_chunk(fileio->write_buffer);
    fileio->write_buffer = "";

    fclose(fileio->file_descriptor_write);
    fileio->file_descriptor_write = nullptr;
}
