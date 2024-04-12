#include "util/util.hpp"

#include <memory>

MainContext::MainContext()
    : VERBOSE(false) {}

std::unique_ptr<UtilContext> util;
