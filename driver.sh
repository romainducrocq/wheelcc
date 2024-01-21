#!/bin/bash

#PACKAGE_NAME="$(cat $(dirname $(readlink -f ${0}))/package_name.txt)"
#PYTHON_VERSION="$(cat $(dirname $(readlink -f ${0}))/python_version.txt)"

ARGC=${#}
ARGV=(${@})

function verbose () {
    if [ ${OPT_CODE} -gt 0 ]; then
        echo "${1}"
    fi
}
#
#function usage () {
#    if [ -f "$HOME/.${PACKAGE_NAME}/${PACKAGE_NAME}/${PACKAGE_NAME}" ]; then
#        echo "Usage: ${PACKAGE_NAME} [Help] [Link] [Lib] FILE"
#        echo ""
#        echo "[Help]:"
#        echo "    --help       print help and exit"
#        echo "    -v           enable verbose mode"
#        echo ""
#        echo "[Link]:"
#        echo "    -c           compile, but do not link"
#        echo ""
#        echo "[Lib]:"
#        echo "    -l<libname>  links with a library file"
#        echo ""
#        echo "FILE:            .c file to compile"
#    else
#        echo "Usage: ${PACKAGE_NAME} [Help] [Debug] [Link] [Lib] FILE"
#        echo ""
#        echo "[Help]:"
#        echo "    --help       print help and exit"
#        echo ""
#        echo "[Debug]:"
#        echo "    --lex        print lexing and exit"
#        echo "    --parse      print parsing and exit"
#        echo "    --validate   print semantic analysis and exit"
#        echo "    --tacky      print tac representation and exit"
#        echo "    --codegen    print assembly generation and exit"
#        echo "    --codeemit   print code emission and exit"
#        echo ""
#        echo "[Link]:"
#        echo "    -c           compile, but do not link"
#        echo ""
#        echo "[Lib]:"
#        echo "    -l<libname>  links with a library file"
#        echo ""
#        echo "FILE:            .c file to compile"
#    fi
#    exit 0
#}

function clean () {
    if [ -f ${FILE}.i ]; then rm ${FILE}.i; fi
    if [ -f ${FILE}.s ]; then rm ${FILE}.s; fi
}

function shift_arg () {
    if [ ${i} -lt ${ARGC} ]; then
        ARG="${ARGV[${i}]}"
        i=$((i+1))
        return 0
    else
        ARG=""
        return 1
    fi
}

function help_arg () {
    if [ "${ARG}" = "--help" ]; then
        usage
    fi
}

function opt_arg () {
    if [ "${ARG}" = "-v" ]; then
        OPT_CODE=1
    elif [ "${ARG}" = "--lex" ]; then
        OPT_CODE=255
    elif [ "${ARG}" = "--parse" ]; then
        OPT_CODE=254
    elif [ "${ARG}" = "--validate" ]; then
        OPT_CODE=253
    elif [ "${ARG}" = "--tacky" ]; then
        OPT_CODE=252
    elif [ "${ARG}" = "--codegen" ]; then
        OPT_CODE=251
    elif [ "${ARG}" = "--codeemit" ]; then
        OPT_CODE=250
    else
        return 1
    fi
    return 0
}

function link_arg () {
    if [ "${ARG}" = "-c" ]; then
        LINK_CODE=1
    else
        return 1
    fi
    return 0
}

function file_arg () {
    FILE=$(readlink -f ${ARG})
    FILE=${FILE%.*}
}

function lib_arg () {
    if [[ "${ARG}" == "-l"* ]]; then
      LINK_LIBS="${LINK_LIBS} ${ARG}"
      return 0
    fi
    return 1
}

function parse_args () {
    i=0

    shift_arg
    if [ ${?} -ne 0 ]; then exit 1; fi
    help_arg

    if [ ${?} -ne 0 ]; then exit 1; fi
    opt_arg

    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then exit 1; fi
    fi
    link_arg

    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then exit 1; fi
    fi
    lib_arg

    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then exit 1; fi
    fi
    file_arg

    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -eq 0 ]; then exit 1; fi
    else
        exit 1
    fi
}

function preprocess () {
    verbose "Preprocess -> ${FILE}.c"
    gcc -E -P ${FILE}.c -o ${FILE}.i
    if [ ${?} -ne 0 ]; then clean; exit 1; fi
}

function compile () {
#    if [[ ! "${PYTHONPATH}" == *":$HOME/.${PACKAGE_NAME}:"* ]] ; then
#        export PYTHONPATH="$PYTHONPATH:$HOME/.${PACKAGE_NAME}"
#    fi

    verbose "Compile    -> ${FILE}.i"

    DIR=$(pwd)
    cd /home/romain/proj/writing-a-c-compiler/cpp/bin/
    ./run.sh ${OPT_CODE} ${FILE}.i
    if [ ${?} -ne 0 ]; then echo "ERROR"; clean; exit 1; fi
    cd ${DIR}

#    if [ -f "$HOME/.${PACKAGE_NAME}/${PACKAGE_NAME}/${PACKAGE_NAME}" ]; then
#        $HOME/.${PACKAGE_NAME}/${PACKAGE_NAME}/${PACKAGE_NAME} ${OPT_CODE} ${FILE}.i
#        if [ ${?} -ne 0 ]; then echo "ERROR"; clean; exit 1; fi
#    else
#        python${PYTHON_VERSION} -c "from ${PACKAGE_NAME}.main_compiler import main_py; main_py()" ${OPT_CODE} ${FILE}.i
#        if [ ${?} -ne 0 ]; then echo "ERROR"; clean; exit 1; fi
#    fi
}

function link () {
    if [ ${OPT_CODE} -lt 200 ]; then
        verbose "Link       -> ${FILE}.s"
        if [ ${LINK_CODE} -eq 0 ]; then
            gcc ${FILE}.s${LINK_LIBS} -o ${FILE}
            if [ ${?} -ne 0 ]; then clean; exit 1; fi
            verbose "Executable -> ${FILE}"
        elif [ ${LINK_CODE} -eq 1 ]; then
            gcc -c ${FILE}.s${LINK_LIBS} -o ${FILE}.o
            if [ ${?} -ne 0 ]; then clean; exit 1; fi
            verbose "Object     -> ${FILE}.o"
        else
            if [ ${?} -ne 0 ]; then clean; exit 1; fi
        fi
    fi
}


OPT_CODE=0
LINK_CODE=0
FILE=""
LINK_LIBS=""
parse_args

preprocess
compile
#link

clean
exit 0
