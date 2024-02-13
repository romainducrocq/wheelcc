#include "util/fopen.hpp"
#include "util/error.hpp"

#include <stdio.h>
#include <string>

static size_t line_number = 0;
static FILE* file_in = nullptr;
static FILE* file_out = nullptr;

void file_open_read(const std::string& filename) {
    file_in = nullptr;

    file_in = fopen(filename.c_str(), "rb");
    if(file_in == nullptr) {
        raise_runtime_error("File " + em(filename) + " does not exist");
    }
    set_filename(filename);
}

/** TODO
cdef void file_open_write(str filename):
    global c_file_out
    global stream_buf
    c_file_out = NULL
    stream_buf = ""

    cdef bytes b_filename = filename.encode("UTF-8")
    cdef char *c_filename = b_filename

    c_file_out = fopen(c_filename, "wb")
    if c_file_out == NULL:

        raise RuntimeError(
            f"File {filename} does not exist")
*/

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

/** TODO
cdef void write_chunk(bytes chunk_fp, size_t chunk_l):

    cdef char *c_chunk_fp = chunk_fp
    fwrite(c_chunk_fp, sizeof(char), chunk_l, c_file_out)
*/

/** TODO
cdef void write_file(str stream, Py_ssize_t chunk_size = 4096):
    global stream_buf

    stream_buf += stream
    while len(stream_buf) >= chunk_size:
        write_chunk(stream_buf[:chunk_size].encode("UTF-8"),
                    chunk_size)

        stream_buf = stream_buf[chunk_size:]
*/

/** TODO
cdef void write_line(str line):
    write_file(line + "\n")
*/

void file_close_read() {
    fclose(file_in);
}

/** TODO
cdef void file_close_write():

    write_chunk(stream_buf.encode("UTF-8"), len(stream_buf))
    fclose(c_file_out)
*/
