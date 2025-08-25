cmake_minimum_required(VERSION 3.5)

file(READ "../bin/package_name.txt" PACKAGE_NAME)
project("${PACKAGE_NAME}")

set(PROJECT_DIR "${PROJECT_SOURCE_DIR}/..")

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wpedantic")
if(__NDEBUG__)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -pedantic-errors -D__NDEBUG__")
endif(__NDEBUG__)
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(OUTPUT_DIR "${PROJECT_DIR}/bin/")

list(APPEND INCLUDE_DIRS "${PROJECT_DIR}/include/")
list(APPEND INCLUDE_DIRS "${PROJECT_DIR}/src/ast/")
list(APPEND INCLUDE_DIRS "${PROJECT_DIR}/src/optimization/")
list(APPEND INCLUDE_DIRS "${PROJECT_DIR}/src/frontend/")
list(APPEND INCLUDE_DIRS "${PROJECT_DIR}/src/backend/")
list(APPEND INCLUDE_DIRS "${PROJECT_DIR}/src/frontend/")
list(APPEND INCLUDE_DIRS "${PROJECT_DIR}/lib/")

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_DIR}/src/*.c" "${PROJECT_DIR}/src/*.cpp"
    "${PROJECT_DIR}/lib/*.c" "${PROJECT_DIR}/lib/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
