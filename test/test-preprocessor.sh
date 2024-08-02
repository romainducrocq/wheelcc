#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

TEST_SRC="${PWD}/tests/preprocessor"

function header_dir () {
    HEADER_DIR=""
    for i in $(seq 1 ${1})
    do
        HEADER_DIR="${HEADER_DIR}${i}/"
    done
    mkdir -p ${TEST_SRC}/${HEADER_DIR}
    echo "${HEADER_DIR}"
}

function create_test () {
    if [ -d "${TEST_SRC}" ]; then
        rm -r ${TEST_SRC}
    fi
    mkdir ${TEST_SRC}

    for i in $(seq 1 $((N-1)))
    do
        echo "int x${i} = 1;" > ${TEST_SRC}/$(header_dir ${i})test-header_${i}.h
        echo "// a single-line comment ${i}" >> ${TEST_SRC}/$(header_dir ${i})test-header_${i}.h
        echo "#include \"$(header_dir $((${N}-${i})))test-header_$((${N}-${i})).h\"" >> ${TEST_SRC}/$(header_dir ${i})test-header_${i}.h
        echo "/* a multi-line" >> ${TEST_SRC}/$(header_dir ${i})test-header_${i}.h
        echo "comment ${i}" >> ${TEST_SRC}/$(header_dir ${i})test-header_${i}.h
        echo "   */" >> ${TEST_SRC}/$(header_dir ${i})test-header_${i}.h
        echo "char* s${i} = \"Hello ${i}!\";" >> ${TEST_SRC}/$(header_dir ${i})test-header_${i}.h
    done

    echo "int x${N} = 1;" > ${TEST_SRC}/test-header_${N}.h
    echo "// a single-line comment ${N}" >> ${TEST_SRC}/test-header_${N}.h
    echo "#include \"test-header_0.h\"" >> ${TEST_SRC}/test-header_${N}.h
    echo "/* a multi-line" >> ${TEST_SRC}/test-header_${N}.h
    echo "comment ${N}" >> ${TEST_SRC}/test-header_${N}.h
    echo "   */" >> ${TEST_SRC}/test-header_${N}.h
    echo "char* s${N} = \"Hello ${N}!\";" >> ${TEST_SRC}/test-header_${N}.h

    echo -n "" > ${TEST_SRC}/test-header_0.h

    echo "int puts(char* s);" > ${FILE}.c
    echo "" >> ${FILE}.c
    echo "int x$((${N}+1)) = 1;" >> ${FILE}.c
    echo "// a single-line comment $((${N}+1))" >> ${FILE}.c
    echo "" >> ${FILE}.c
    for i in $(seq 1 $((N-1)))
    do
        echo "#include \"$(header_dir ${i})test-header_${i}.h\"" >> ${FILE}.c
    done
    echo "#include \"test-header_${N}.h\"" >> ${FILE}.c
    echo "" >> ${FILE}.c
    echo "/* a multi-line" >> ${FILE}.c
    echo "comment $((${N}+1))" >> ${FILE}.c
    echo "   */" >> ${FILE}.c
    echo "char* s$((${N}+1)) = \"Hello $((${N}+1))!\";" >> ${FILE}.c
    echo "" >> ${FILE}.c
    echo "int main(void) {" >> ${FILE}.c
    for i in $(seq 1 $((${N}+1)))
    do
        echo "    puts(s${i});" >> ${FILE}.c
    done
        echo "    return 0" >> ${FILE}.c;
    for i in $(seq 1 $((${N}+1)))
    do
        echo "    + x${i}" >> ${FILE}.c
    done
    echo "    ;" >> ${FILE}.c
    echo "}" >> ${FILE}.c
}

test_preprocess () {
    create_test

    ${PACKAGE_NAME} ${FILE}.c > /dev/null 2>&1
    RETURN=${?}
    STDOUT=""
    if [ ${RETURN} -ne 0 ]; then
        # TODO
        return 1
    fi

    STDOUT=$(${FILE})
    RETURN=${?}
    rm ${FILE}

    diff -sq <(echo "${STDOUT}") <(
        for i in $(seq 1 $((${N}+1)))
        do
            echo "Hello ${i}!"
        done
    ) | grep -q "identical"
    if [ ${?} -eq 0 ]; then
        if [ ${RETURN} -eq $((${N}+1)) ]; then
            echo "YESSS"
            # TODO
            return 0
        fi
    fi

    # TODO
    return 1
}

test_error () {
    if [ ! -f "${FILE}.c" ]; then
        create_test
    fi

    echo "int e1 = {0};" >> ${TEST_SRC}/$(header_dir ${ERR})test-header_${ERR}.h

    if [ -f "${FILE}" ]; then
        rm ${FILE}
    fi

    STDOUT=$(${PACKAGE_NAME} ${FILE}.c 2>&1)
    RETURN=${?}
    if [ ${RETURN} -eq 0 ]; then
        rm ${FILE}
        # TODO
        return 1
    fi

    diff -sq <(echo "${STDOUT}") <(
        echo -e -n "\033[1m${TEST_SRC}/"
        for i in $(seq 1 $((${ERR})))
        do
            echo -n "${i}/"
        done
        echo -e "test-header_${ERR}.h:8:\033[0m"
        echo -e "\033[0;31merror:\033[0m (no. 545) cannot initialize scalar type \033[1m‘int’\033[0m with compound initializer"
        echo -e "at line 8: \033[1mint e1 = {0};\033[0m"
        echo -e "wheelcc: \033[0;31merror:\033[0m compilation failed"
    ) | grep -q "identical"
    if [ ${?} -eq 0 ]; then
        echo "YESSS 2"
        # TODO
        return 0
    fi

    # TODO
    return 1
}

N=63
ERR=27

FILE=${TEST_SRC}/main

test_preprocess
test_error

exit 0
