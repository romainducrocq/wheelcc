#include "lexer/lexer.hpp"
#include "util/error.hpp"

#include <string>
#include <vector>

#include <iostream>

std::vector<std::string> args;

static void shift_args(std::string& arg) {
    if(!args.empty()) {
        arg = std::move(args.back());
        args.pop_back();
        return;
    }
    arg = "";
}

static void arg_parse(std::string& filename, int& opt_code, int& opt_s_code) {
    std::string arg;
    shift_args(arg);

    shift_args(arg);
    if(arg.empty()) {
        raise_runtime_error("No option code passed in args(0)");
    }
    opt_code = std::stoi(arg);

    shift_args(arg);
    if(arg.empty()) {
        raise_runtime_error("No file name passed in args(1)");
    }
    filename = std::move(arg);

    opt_s_code = 0;
}

/**
TODO

cdef void entry(list[str] args):
    global VERBOSE

    cdef str filename
    cdef int32 opt_code
    cdef int32 opt_s_code
    filename, opt_code, opt_s_code = arg_parse(args)
    if opt_code > 0:
        VERBOSE = True

    do_compile(filename, opt_code, opt_s_code)


cdef public int main_c(int argc, char **argv):
    cdef Py_ssize_t i
    cdef list[str] args = []
    for i in range(argc):
        args.append(str(argv[i].decode("UTF-8")))

    entry(args)
 */

int main(int argc, char **argv) {

    for(size_t i = static_cast<size_t>(argc); i-- > 0 ;){
        args.push_back(argv[i]);
    }

    std::string filename;
    int opt_code;
    int opt_s_code;
    arg_parse(filename, opt_code, opt_s_code);

    std::vector<Token> tokens;
    lexing(filename, tokens);

    for(const auto& token: tokens) {
        std::cout << token.token_kind << ", " << token.token << std::endl;
    }

    return 0;
}