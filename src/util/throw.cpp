#include "util/throw.hpp"

#include <array>
#include <cstdio>
#include <string>
#include <memory>
#include <stdexcept>

static std::string filename;

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
    std::string line;
    {
        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if(!pipe) {
            raise_runtime_error(message);
        }
        while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            line += buffer.data();
        }
    }
    throw std::runtime_error("\n\033[1m" + filename + ":" + std::to_string(line_number) +
                             ":\033[0m\n\033[0;31merror:\033[0m " + message + "\nat line " +
                             std::to_string(line_number) + ": \033[1m" + line + "\033[0m");
}

[[ noreturn ]] void raise_internal_error(const char* func, const char* file, int line) {
    throw std::runtime_error("\n\033[1m" + std::string(file) + ":" + std::to_string(line) +
                             ":\033[0m\n\033[0;31minternal error:\033[0m " + std::string(func));
}
