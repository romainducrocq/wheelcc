#!/bin/bash

PACKAGE_TEST="$(dirname $(readlink -f ${0}))"
PACKAGE_DIR="$(dirname ${PACKAGE_TEST})/bin"
PACKAGE_NAME="$(cat ${PACKAGE_DIR}/package_name.txt)"
CC="gcc"
DIST="linux"
if [[ "$(uname -s)" = "Darwin"* ]]; then
    CC="clang -arch x86_64"
    DIST="osx"

    CLANG_MAJOR_VERSION=$(clang -dumpversion | cut -d"." -f1)
    if [ ${CLANG_MAJOR_VERSION} -lt 5 ]; then
        echo -e "${PACKAGE_NAME}: \033[0;31merror:\033[0m requires \033[clang\033[0m >= 5.0.0" 1>&2
        exit 1
    fi
else
    GCC_MAJOR_VERSION=$(gcc -dumpversion | cut -d"." -f1)
    if [ ${GCC_MAJOR_VERSION} -lt 8 ]; then
        echo -e "${PACKAGE_NAME}: \033[0;31merror:\033[0m requires \033[1m‘gcc’\033[0m >= 8.1.0" 1>&2
        exit 1
    elif [ ${GCC_MAJOR_VERSION} -eq 8 ]; then
        GCC_MINOR_VERSION=$(gcc -dumpfullversion | cut -d"." -f2)
        if [ ${GCC_MINOR_VERSION} -eq 0 ]; then
            echo -e "${PACKAGE_NAME}: \033[0;31merror:\033[0m requires \033[1m‘gcc’\033[0m >= 8.1.0" 1>&2
            exit 1
        fi
    fi
fi
CC="${CC} -pedantic-errors -std=c17"

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

EXT_IN="c"
TEST_DIR_GCC="${PACKAGE_TEST}/tests/compiler"
TEST_DIR="${TEST_DIR_GCC}"
TEST_SRCS=()
for i in $(seq 1 20); do
    TEST_SRCS+=("$(basename $(find ${TEST_DIR_GCC} -maxdepth 1 -name "${i}_*" -type d))")
done
if [ -f "${PACKAGE_DIR}/filename_ext.txt" ]; then
    EXT_IN="$(cat ${PACKAGE_DIR}/filename_ext.txt)"
fi
if [ -f "${PACKAGE_DIR}/package_path.txt" ]; then
    TEST_DIR="$(cat ${PACKAGE_DIR}/package_path.txt)/test/tests/compiler"
fi

function file () {
    FILE=${1%.*}
    if [ -f "${FILE}" ]; then rm ${FILE}; fi
    echo "${FILE}"
}

function total () {
    echo "----------------------------------------------------------------------"
    RESULT="${PASS} / ${TOTAL}"
    if [ ${PASS} -eq ${TOTAL} ]; then
        RESULT="${LIGHT_GREEN}PASS: ${RESULT}${NC}"
        RETURN=0
    else
        RESULT="${LIGHT_RED}FAIL: ${RESULT}${NC}"
        RETURN=1
    fi
    echo -e "${RESULT}"
}

function indent () {
    echo -n "$(echo "${TOTAL} [ ] ${FILE}.${EXT_IN}" | sed -r 's/./ /g')"
}

function print_check () {
    echo " ${OPTIM} check ${1} -> ${2}"
}

function print_fail () {
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.${EXT_IN}${NC}"
    PRINT="${PACKAGE_NAME}: ${RETURN_THIS}"
    print_check "fail" "[${PRINT}]"
}

function print_single () {
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.${EXT_IN}${NC}"
    if [ ${RETURN_PASS} -ne 0 ]; then
        PRINT="${COMP_2}: ${RETURN_THIS}"
        print_check "return" "[${PRINT}]"
    else
        PRINT="${COMP_1}: ${RETURN_GCC}, ${COMP_2}: ${RETURN_THIS}"
        print_check "return" "[${PRINT}]"
        if [ ! -z "${STDOUT_THIS}" ]; then
            indent
            PRINT="${COMP_1}: \"${STDOUT_GCC}\", ${COMP_2}: \"${STDOUT_THIS}\""
            print_check "stdout" "[${PRINT}]"
        fi
    fi
}

function print_client () {
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.${EXT_IN}${NC}"
    if [ ${RETURN_PASS} -ne 0 ]; then
        PRINT="${COMP_4}: ${RETURN_THIS}"
        print_check "return" "[${PRINT}]"
    else
        PRINT="${COMP_1}: ${RETURN_GCC}, ${COMP_2}: ${RETURN_GCC_THIS}, "
        PRINT="${PRINT}${COMP_3}: ${RETURN_THIS_GCC}, ${COMP_4}: ${RETURN_THIS}"
        print_check "return" "[${PRINT}]"
        if [ ! -z "${STDOUT_THIS}" ]; then
            indent
            PRINT="${COMP_1}: \"${STDOUT_GCC}\", ${COMP_2}: \"${STDOUT_GCC_THIS}\", "
            PRINT="${PRINT}${COMP_3}: \"${STDOUT_THIS_GCC}\", ${COMP_4}: \"${STDOUT_THIS}\""
            print_check "stdout" "[${PRINT}]"
        fi
    fi
}

function check_fail () {
    ${PACKAGE_NAME} ${OPTIM} ${LIBS} ${TEST_DIR}/${FILE}.${EXT_IN} > /dev/null 2>&1
    RETURN_THIS=${?}

    if [ ${RETURN_THIS} -ne 0 ]; then
        RESULT="${LIGHT_GREEN}[y]"
        if [ ${1} -eq 0 ]; then
            let PASS+=1
        fi
    else
        RESULT="${LIGHT_RED}[n]"
    fi

    print_fail
}

function check_pass () {
    if [ ${RETURN_THIS} -ne 0 ]; then
        RESULT="${LIGHT_RED}[n]"
        return 1
    fi

    STDOUT_THIS=$(${TEST_DIR}/${FILE})
    RETURN_THIS=${?}
    rm ${TEST_DIR}/${FILE}

    if [ ${RETURN_GCC} -eq ${RETURN_THIS} ]; then
        if [[ "${STDOUT_GCC}" == "${STDOUT_THIS}" ]]; then
            RESULT="${LIGHT_GREEN}[y]"
            if [ ${1} -eq 0 ]; then
                let PASS+=1
            fi
        fi
    else
        RESULT="${LIGHT_RED}[n]"
    fi

    return 0
}

function check_single () {
    ${CC} ${FILE}.c ${LIBS} -o ${FILE} > /dev/null 2>&1
    RETURN_GCC=${?}

    if [ ${RETURN_GCC} -ne 0 ]; then
        check_fail 0
        return
    fi

    STDOUT_GCC=$(${FILE})
    RETURN_GCC=${?}
    rm ${FILE}

    if [ -f "${TEST_DIR}/${FILE}" ]; then rm ${TEST_DIR}/${FILE}; fi

    ${PACKAGE_NAME} ${OPTIM} ${LIBS} ${TEST_DIR}/${FILE}.${EXT_IN} > /dev/null 2>&1
    RETURN_THIS=${?}

    check_pass 0
    RETURN_PASS=${?}

    COMP_1="gcc"
    COMP_2="${PACKAGE_NAME}"
    print_single
}

function check_data () {
    ${CC} -c ${FILE}.c ${LIBS} -o ${FILE}.o > /dev/null 2>&1
    ${CC} ${FILE}.o ${FILE}_${DIST}.s ${LIBS} -o ${FILE} > /dev/null 2>&1
    STDOUT_GCC=$(${FILE})
    RETURN_GCC=${?}
    rm ${FILE}.o
    rm ${FILE}

    if [ -f "${TEST_DIR}/${FILE}.o" ]; then rm ${TEST_DIR}/${FILE}.o; fi
    if [ -f "${TEST_DIR}/${FILE}" ]; then rm ${TEST_DIR}/${FILE}; fi

    ${PACKAGE_NAME} ${OPTIM} -c ${LIBS} ${TEST_DIR}/${FILE}.${EXT_IN} > /dev/null 2>&1
    RETURN_THIS=${?}

    if [ ${RETURN_THIS} -eq 0 ]; then
        ${CC} ${TEST_DIR}/${FILE}.o ${FILE}_${DIST}.s ${LIBS} -o ${TEST_DIR}/${FILE} > /dev/null 2>&1
        RETURN_THIS=${?}
    fi

    if [ -f "${TEST_DIR}/${FILE}.o" ]; then rm ${TEST_DIR}/${FILE}.o; fi

    check_pass 0
    RETURN_PASS=${?}

    COMP_1="gcc"
    COMP_2="${PACKAGE_NAME}"
    print_single
}

function compile_client () {
    ${PACKAGE_NAME} ${OPTIM} -c ${LIBS} ${TEST_DIR}/${1}.${EXT_IN} > /dev/null 2>&1
    RETURN_THIS=${?}

    if [ ${RETURN_THIS} -eq 0 ]; then
        ${CC} ${TEST_DIR}/${FILE}.o ${TEST_DIR}/${FILE}_client.o ${LIBS} -o ${TEST_DIR}/${FILE} > /dev/null 2>&1
        RETURN_THIS=${?}
    fi

    if [ -f "${TEST_DIR}/${FILE}.o" ]; then rm ${TEST_DIR}/${FILE}.o; fi
    if [ -f "${TEST_DIR}/${FILE}_client.o" ]; then rm ${TEST_DIR}/${FILE}_client.o; fi
}

function check_client () {
    ${CC} ${FILE}.c ${FILE}_client.c ${LIBS} -o ${FILE} > /dev/null 2>&1
    STDOUT_GCC=$(${FILE})
    RETURN_GCC=${?}
    rm ${FILE}

    if [ -f "${TEST_DIR}/${FILE}.o" ]; then rm ${TEST_DIR}/${FILE}.o; fi
    if [ -f "${TEST_DIR}/${FILE}_client.o" ]; then rm ${TEST_DIR}/${FILE}_client.o; fi
    if [ -f "${TEST_DIR}/${FILE}" ]; then rm ${TEST_DIR}/${FILE}; fi

    ${CC} -c ${FILE}_client.c ${LIBS} -o ${TEST_DIR}/${FILE}_client.o > /dev/null 2>&1
    compile_client ${FILE}
    check_pass 1
    RETURN_PASS=${?}
    RETURN_GCC_THIS=${RETURN_THIS}
    STDOUT_GCC_THIS=${STDOUT_THIS}
    if [ ${RETURN_PASS} -ne 0 ]; then
        COMP_4="gcc+${PACKAGE_NAME}"
        print_client
        return
    fi

    ${CC} -c ${FILE}.c ${LIBS} -o ${TEST_DIR}/${FILE}.o > /dev/null 2>&1
    compile_client ${FILE}_client
    check_pass 1
    RETURN_PASS=${?}
    RETURN_THIS_GCC=${RETURN_THIS}
    STDOUT_THIS_GCC=${STDOUT_THIS}
    if [ ${RETURN_PASS} -ne 0 ]; then
        COMP_4="${PACKAGE_NAME}+gcc"
        print_client
        return
    fi

    ${PACKAGE_NAME} ${OPTIM} -c ${LIBS} ${TEST_DIR}/${FILE}.${EXT_IN} > /dev/null 2>&1
    compile_client ${FILE}_client
    check_pass 0
    RETURN_PASS=${?}

    COMP_1="gcc"
    COMP_2="gcc+${PACKAGE_NAME}"
    COMP_3="${PACKAGE_NAME}+gcc"
    COMP_4="${PACKAGE_NAME}"
    print_client
}

function check_test () {
    FILE=$(file ${1})
    if [ ! -f "${TEST_DIR}/${FILE}.${EXT_IN}" ]; then
        return
    fi
    if [[ "${FILE}" == *"_client" ]]; then
        return
    fi

    let TOTAL+=1

    LIBS=""
    if [[ "${FILE}" == *"__+l"* ]]; then
        LIBS=" -"$(echo "${FILE}" | cut -d "+" -f2- | tr "+" "-" | tr "_" " ")
    fi

    if [ -f "${FILE}_${DIST}.s" ]; then
        check_data
        return
    fi

    if [ -f "${FILE}_client.c" ]; then
        check_client
        return
    fi

    # gcc outputs only a warning here
    if [[ "${FILE}" == "${TEST_SRCS[16]}/invalid_types/void/void_fun_params" ]]; then
        check_fail 0
        return
    fi

    check_single
}

function test_src () {
    SRC=${1}
    for FILE in $(find ${SRC} -name "*.c" -type f | sort --uniq); do
        check_test ${FILE}
    done
}

function test_all () {
    for SRC in ${TEST_SRCS[@]}; do
        test_src ${SRC}
    done
}

LIBS=""
PASS=0
TOTAL=0
RETURN=0

ARG=${1}

OPTIM="-O0"
if [ "${1}" = "-O0" ]; then
    ARG=${2}
elif [ "${1}" = "-O1" ]; then
    OPTIM="-O0 -O1"
    ARG=${2}
elif [ "${1}" = "-O2" ]; then
    OPTIM="-O0 -O2"
    ARG=${2}
elif [ "${1}" = "-O3" ]; then
    OPTIM="-O3"
    ARG=${2}
fi

cd ${TEST_DIR_GCC}
if [ ! -z "${ARG}" ]; then
    test_src ${TEST_SRCS["$((${ARG} - 1))"]}
else
    test_all
fi
total

exit ${RETURN}
