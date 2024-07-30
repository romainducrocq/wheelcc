// TODO rm

// #ifndef _UTIL_PREPROC_HPP
// #define _UTIL_PREPROC_HPP

// #include <memory>
// #include <stdio.h>
// #include <string>
// #include <unordered_set>
// #include <vector>

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// // Preprocessor

// struct FileInclude {
//     size_t l;
//     char* buffer;
//     FILE* file_in;
//     std::string filename;
// };

// struct PreprocessorContext {
//     std::vector<FileInclude> include_stack;
//     std::vector<size_t> line_chunks;
//     std::unordered_set<std::string> is_read_set;
// };

// extern std::unique_ptr<PreprocessorContext> preprocessor;
// #define INIT_PREPROCESSOR_CONTEXT util = std::make_unique<PreprocessorContext>()
// #define FREE_PREPROCESSOR_CONTEXT util.reset()

// #endif
