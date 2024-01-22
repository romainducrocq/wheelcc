cmake_minimum_required(VERSION 3.5)

file(READ "../bin/package_name.txt" PACKAGE_NAME)
project("${PACKAGE_NAME}")

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/../src/")
set(HEADER_DIR "${PROJECT_SOURCE_DIR}/../include/")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/../lib/")
set(BIN_DIR "${PROJECT_SOURCE_DIR}/../bin/")

file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${HEADER_DIR}/*.hpp" "${LIB_DIR}/*.hpp" "${LIB_DIR}/*.h")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_DIR} ${LIB_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
