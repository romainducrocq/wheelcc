#include "util/error.hpp"

#include <array>
#include <cstdio>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>

static std::string filename;

static std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void set_filename(const std::string& _filename) {
    filename = _filename;
}

void raise_runtime_error(const std::string& message) {
    throw std::runtime_error("\n\e[1m" + filename + ":\n\033[0;31merror:\033[0m " + message + "\n");
}

void raise_runtime_error_at_line(const std::string& message, int l) {
    std::string cmd = "sed -n " + std::to_string(l) + "p " + filename;
    std::string line = exec(cmd.c_str());
    // TODO
}
