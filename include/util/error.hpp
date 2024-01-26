#ifndef _UTIL_ERROR_HPP
#define _UTIL_ERROR_HPP

#include <string>

void set_filename(const std::string& _filename);
void raise_runtime_error(const std::string& message);

#endif
