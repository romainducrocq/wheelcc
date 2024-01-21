#include "util/error.hpp"

#include <string>
#include <stdexcept>

void error::raise_runtime_error(const std::string message) {
    throw std::runtime_error("\n[ERROR] \033[0;31m" + message + "\033[0m\n");
}
