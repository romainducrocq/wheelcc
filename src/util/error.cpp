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

const std::string em(const std::string& message) {
    return "\033[1m‘" + message + "’\033[0m";
}

[[ noreturn ]] void raise_runtime_error(const std::string& message) {
    throw std::runtime_error("\n\033[1m" + filename + ":\033[0m\n\033[0;31merror:\033[0m " + message + "\n");
}

[[ noreturn ]] void raise_runtime_error_at_line(const std::string& message, size_t line_number) {
    std::string cmd = "sed -n " + std::to_string(line_number) + "p " + filename;
    std::string line = exec(cmd.c_str());
    throw std::runtime_error("\n\033[1m" + filename + ":" + std::to_string(line_number) +
                             ":\033[0m\n\033[0;31merror:\033[0m " + message + "\nat line " +
                             std::to_string(line_number) + ": \033[1m" + line + "\033[0m");
}

[[ noreturn ]] void raise_internal_error(const std::string& message) {
    raise_runtime_error(message);
}
