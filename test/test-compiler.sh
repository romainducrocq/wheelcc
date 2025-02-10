#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

TEST_DIR="${PWD}/tests/compiler"
TEST_SRCS=()
for i in $(seq 1 19); do
    TEST_SRCS+=("$(basename $(find ${TEST_DIR} -maxdepth 1 -name "${i}_*" -type d))")
done

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
    else
        RESULT="${LIGHT_RED}FAIL: ${RESULT}${NC}"
    fi
    echo -e "${RESULT}"
}

function indent () {
    echo -n "$(echo "${TOTAL} [ ] ${FILE}.c" | sed -r 's/./ /g')"
}

function print_check () {
    echo " - check ${1} -> ${2}"
}

function print_fail () {
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.c${NC}"
    PRINT="${PACKAGE_NAME}: ${RETURN_THIS}"
    print_check "fail" "[${PRINT}]"
}

function print_single () {
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.c${NC}"
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
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.c${NC}"
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
    ${PACKAGE_NAME} ${LIBS} ${FILE}.c > /dev/null 2>&1
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

    STDOUT_THIS=$(${FILE})
    RETURN_THIS=${?}
    rm ${FILE}

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
    gcc -pedantic-errors ${FILE}.c ${LIBS} -o ${FILE} > /dev/null 2>&1
    RETURN_GCC=${?}

    if [ ${RETURN_GCC} -ne 0 ]; then
        check_fail 0
        return
    fi

    STDOUT_GCC=$(${FILE})
    RETURN_GCC=${?}
    rm ${FILE}

    ${PACKAGE_NAME} ${LIBS} ${FILE}.c > /dev/null 2>&1
    RETURN_THIS=${?}

    check_pass 0
    RETURN_PASS=${?}

    COMP_1="gcc"
    COMP_2="${PACKAGE_NAME}"
    print_single
}

function check_data () {
    gcc -pedantic-errors -c ${FILE}_data.s ${LIBS} -o ${FILE}_data.o > /dev/null 2>&1
    gcc -pedantic-errors -c ${FILE}.c ${LIBS} -o ${FILE}.o > /dev/null 2>&1
    gcc -pedantic-errors ${FILE}.o ${FILE}_data.o ${LIBS} -o ${FILE} > /dev/null 2>&1
    STDOUT_GCC=$(${FILE})
    RETURN_GCC=${?}
    rm ${FILE}

    if [ -f "${FILE}.o" ]; then rm ${FILE}.o; fi

    ${PACKAGE_NAME} -c ${LIBS} ${FILE}.c > /dev/null 2>&1
    RETURN_THIS=${?}

    if [ ${RETURN_THIS} -eq 0 ]; then
        gcc -pedantic-errors ${FILE}.o ${FILE}_data.s ${LIBS} -o ${FILE} > /dev/null 2>&1
        RETURN_THIS=${?}
    fi

    if [ -f "${FILE}.o" ]; then rm ${FILE}.o; fi
    if [ -f "${FILE}_data.o" ]; then rm ${FILE}_data.o; fi

    check_pass 0
    RETURN_PASS=${?}

    COMP_1="gcc"
    COMP_2="${PACKAGE_NAME}"
    print_single
}

function compile_client () {
    ${PACKAGE_NAME} -c ${LIBS} ${1} > /dev/null 2>&1
    RETURN_THIS=${?}

    if [ ${RETURN_THIS} -eq 0 ]; then
        gcc -pedantic-errors ${FILE}.o ${FILE}_client.o ${LIBS} -o ${FILE} > /dev/null 2>&1
        RETURN_THIS=${?}
    fi

    if [ -f "${FILE}.o" ]; then rm ${FILE}.o; fi
    if [ -f "${FILE}_client.o" ]; then rm ${FILE}_client.o; fi
}

function check_client () {
    gcc -pedantic-errors ${FILE}.c ${FILE}_client.c ${LIBS} -o ${FILE} > /dev/null 2>&1
    STDOUT_GCC=$(${FILE})
    RETURN_GCC=${?}
    rm ${FILE}

    if [ -f "${FILE}.o" ]; then rm ${FILE}.o; fi
    if [ -f "${FILE}_client.o" ]; then rm ${FILE}_client.o; fi

    gcc -pedantic-errors -c ${FILE}_client.c ${LIBS} -o ${FILE}_client.o > /dev/null 2>&1
    compile_client ${FILE}.c
    check_pass 1
    RETURN_PASS=${?}
    RETURN_GCC_THIS=${RETURN_THIS}
    STDOUT_GCC_THIS=${STDOUT_THIS}
    if [ ${RETURN_PASS} -ne 0 ]; then
        COMP_4="gcc+${PACKAGE_NAME}"
        print_client
        return
    fi

    gcc -pedantic-errors -c ${FILE}.c ${LIBS} -o ${FILE}.o > /dev/null 2>&1
    compile_client ${FILE}_client.c
    check_pass 1
    RETURN_PASS=${?}
    RETURN_THIS_GCC=${RETURN_THIS}
    STDOUT_THIS_GCC=${STDOUT_THIS}
    if [ ${RETURN_PASS} -ne 0 ]; then
        COMP_4="${PACKAGE_NAME}+gcc"
        print_client
        return
    fi

    ${PACKAGE_NAME} -c ${LIBS} ${FILE}.c > /dev/null 2>&1
    compile_client ${FILE}_client.c
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
    if [[ "${FILE}" == *"_client" ]]; then
        return
    fi

    let TOTAL+=1

    LIBS=""
    if [[ "${FILE}" == *"__+l"* ]]; then
        LIBS=" -"$(echo "${FILE}" | cut -d "+" -f2- | tr "+" "-" | tr "_" " ")
    fi

    if [ -f "${FILE}_data.s" ]; then
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
    for FILE in $(find ${SRC} -name "*.c" -type f | sort --uniq)
    do
        check_test ${FILE}
    done
}

function test_all () {
    for SRC in ${TEST_SRCS[@]}
    do
        test_src ${SRC}
    done
}

LIBS=""
PASS=0
TOTAL=0
cd ${TEST_DIR}
if [ ! -z "${1}" ]; then
    test_src ${TEST_SRCS["$((${1} - 1))"]}
else
    test_all
fi
total

exit 0
