#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

./build_preset.sh
if [ ${?} -ne 0 ]; then exit 1; fi

BUILD_RELEASE=0
PROJECT_DIR="$(dirname $(dirname $(readlink -f ${0})))"

CXX_FLAGS="-std=c++17 -Wall -Wextra -Wpedantic"
CXX_FLAGS_RELEASE="-O3 -DNDEBUG -Werror -pedantic-errors -D__NDEBUG__"
CXX_FLAGS_DEBUG="-ggdb3 -O0"
if [ ${BUILD_RELEASE} -eq 0 ]; then
    CXX_FLAGS="${CXX_FLAGS} ${CXX_FLAGS_RELEASE}"
else
    CXX_FLAGS="${CXX_FLAGS} ${CXX_FLAGS_DEBUG}"
fi

PROJECT_NAME="-o ${PROJECT_DIR}/bin/${PACKAGE_NAME}"

INCLUDE_DIRS="-I${PROJECT_DIR}/include/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/ast/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/optimization/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/frontend/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/backend/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/lib/"

SOURCE_FILES="${PROJECT_DIR}/src/main.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/ast.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/back_ast.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/back_symt.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/front_ast.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/front_symt.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/interm_ast.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/optimization/optim_tac.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/optimization/reg_alloc.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/parser/errors.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/parser/lexer.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/parser/parser.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/intermediate/idents.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/intermediate/semantic.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/intermediate/tac_repr.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/asm_gen.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/registers.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/stack_fix.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/symt_cvt.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/emitter/gas_code.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/fileio.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/pprint.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/str2t.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/throw.cpp"

g++ ${SOURCE_FILES} ${CXX_FLAGS} ${INCLUDE_DIRS} ${PROJECT_NAME}
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
