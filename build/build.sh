#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"
CC="gcc"
CXX="g++"
if [[ "$(uname -s)" = "Darwin"* ]]; then
    CC="clang"
    CXX="clang++"
fi

./build_preset.sh
if [ ${?} -ne 0 ]; then exit 1; fi

BUILD_RELEASE=0
PROJECT_DIR="$(dirname $(dirname $(readlink -f ${0})))"

CC="${CC} -std=c17"
CXX="${CXX} -std=c++17"
CC_FLAGS="-Wall -Wextra -Wpedantic"
CC_FLAGS_RELEASE="-O3 -DNDEBUG -Werror -pedantic-errors -D__NDEBUG__"
CC_FLAGS_DEBUG="-ggdb3 -O0"
if [ ${BUILD_RELEASE} -eq 0 ]; then
    CC_FLAGS="${CC_FLAGS} ${CC_FLAGS_RELEASE}"
else
    CC_FLAGS="${CC_FLAGS} ${CC_FLAGS_DEBUG}"
fi

PROJECT_NAME="${PROJECT_DIR}/bin/${PACKAGE_NAME}"

INCLUDE_DIRS="-I${PROJECT_DIR}/include/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/ast/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/optimization/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/frontend/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/src/backend/"
INCLUDE_DIRS="${INCLUDE_DIRS} -I${PROJECT_DIR}/lib/"

SOURCE_FILES="${PROJECT_DIR}/src/main.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/ast.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/back_ast.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/back_symt.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/front_ast.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/front_symt.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/ast/interm_ast.c"
# SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/optimization/optim_tac.cpp"
# SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/optimization/reg_alloc.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/parser/errors.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/parser/lexer.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/parser/parser.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/intermediate/idents.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/intermediate/semantic.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/frontend/intermediate/tac_repr.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/asm_gen.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/registers.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/stack_fix.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/assembly/symt_cvt.c"
# SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/backend/emitter/gas_code.cpp"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/fileio.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/pprint.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/str2t.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/src/util/throw.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/lib/sds/sds.c"
SOURCE_FILES="${SOURCE_FILES} ${PROJECT_DIR}/lib/stb_ds/stb_ds.c"

BUILD_CACHE="${PROJECT_DIR}/build/build_cache"
if [ -d "${BUILD_CACHE}/" ]; then
    rm -r ${BUILD_CACHE}/
    if [ ${?} -ne 0 ]; then exit 1; fi
fi
mkdir ${BUILD_CACHE}/
if [ ${?} -ne 0 ]; then exit 1; fi

OBJECT_FILES=""
echo "-- Build objects ..."
for FILE in ${SOURCE_FILES}; do
    OBJECT="${BUILD_CACHE}/$(basename ${FILE%.*}).o"
    OBJECT_FILES="${OBJECT_FILES} ${OBJECT}"
    echo "${FILE} -> ${OBJECT}"
    case "${FILE##*.}" in
        "c")
            ${CC} -c ${FILE} ${CC_FLAGS} ${INCLUDE_DIRS} -o ${OBJECT}
            if [ ${?} -ne 0 ]; then exit 1; fi
            ;;
        "cpp")
            ${CXX} -c ${FILE} ${CC_FLAGS} ${INCLUDE_DIRS} -o ${OBJECT}
            if [ ${?} -ne 0 ]; then exit 1; fi
            ;;
        *)
            exit 1
    esac
done
echo "OK"

echo "-- Linking executable ..."
echo "${BUILD_CACHE}/*.o -> ${PROJECT_NAME}"
${CXX} ${OBJECT_FILES} ${CC_FLAGS} -o ${PROJECT_NAME}
if [ ${?} -ne 0 ]; then exit 1; fi
echo "OK"

echo "-- Created target ${PROJECT_NAME}"

exit 0
