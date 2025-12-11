#!/bin/bash

PACKAGE_DIR="$(dirname $(readlink -f ${0}))"
PACKAGE_NAME="$(cat ${PACKAGE_DIR}/pkgname.cfg)"
CC="gcc"
AS_FLAGS="--64"
LD_LIB_64="/lib64/ld-linux-x86-64.so.2"
if [[ "$(uname -s)" = "Darwin"* ]]; then
    CC="clang -arch x86_64"
    AS_FLAGS="-arch x86_64"
    LD_LIB_64=""
fi
LIBC_DIR="${PACKAGE_DIR}/libc/"
PP="${CC}"

ARGC=${#}
ARGV=(${@})

function verbose () {
    if [ ${IS_VERBOSE} -eq 1 ]; then
        echo "${1}"
    fi
}

function usage () {
    echo "Usage: ${PACKAGE_NAME} [Help] [Debug] [Optimize...] [Preprocess] [Link] [Include...] [Linkdir...] [Linklib...] [Output] FILES"
    echo ""
    echo -e "\033[1;34mwarning:\033[0m 1. optional arguments must be passed in this order only"
    echo -e "\033[1;34mwarning:\033[0m 2. whitespaces are not supported in paths and file names"
    echo ""
    echo "[Help]:"
    echo "    --help  print help and exit"
    echo ""
    echo "[Debug]:"
    echo "    -v          enable verbose mode"
    echo "    (Test|Debug build only):"
    echo "    --lex       print  lexing    stage and exit"
    echo "    --parse     print  parsing   stage and exit"
    echo "    --validate  print  semantic  stage and exit"
    echo "    --tacky     print  interm    stage and exit"
    echo "    --codegen   print  assembly  stage and exit"
    echo ""
    echo "[Optimize...]:"
    echo "    (Level 0):"
    echo "    -O0                           disable  all optimizations"
    echo "    (Level 1):"
    echo "    --fold-constants              enable   constant folding"
    echo "    --propagate-copies            enable   copy propagation"
    echo "    --eliminate-unreachable-code  enable   unreachable code elimination"
    echo "    --eliminate-dead-stores       enable   dead store elimination"
    echo "    --optimize                    enable   all level 1 optimizations"
    echo "    -O1                           alias    for --optimize"
    echo "    (Level 2):"
    echo "    --no-allocation               disable  register allocation"
    echo "    --no-coalescing               disable  register coalescing"
    echo "    --allocate-register           enable   register allocation (default)"
    echo "    -O2                           alias    for --allocate-register"
    echo "    (Level 3):"
    echo "    -O3                           alias    for -O1 -O2"
    echo ""
    echo "[Preprocess]:"
    echo "    -E  enable macro expansion with ${PP}"
    echo ""
    echo "[Link]:"
    echo "    -s  compile, but do not assemble and link"
    echo "    -c  compile and assemble, but do not link"
    echo ""
    echo "[Include...]:"
    echo "    -I<includedir>  add a list of paths to include path"
    echo ""
    echo "[Linkdir...]:"
    echo "    -L<linkdir>  add a list of paths to link path"
    echo ""
    echo "[Linklib...]:"
    echo "    -l<libname>  link with a list of library files"
    echo ""
    echo "[Output]:"
    echo "    -o <file>  write the output into <file>"
    echo ""
    echo "FILES:  list of .${EXT_IN} files to compile"
    exit 0
}

function clean_exit () {
    EXIT_CODE=${1}
    if [ ${EXIT_CODE} -ne 0 ]; then
        LINK_ENUM=255
    fi
    for FILE in ${FILES}; do
        if [ -f "${FILE}.i" ]; then
            rm ${FILE}.i
        fi
        if [ ${LINK_ENUM} -ne 0 ]; then
            if [ -f "${FILE}" ]; then
                rm ${FILE}
            fi
        fi
        if [ ${LINK_ENUM} -ne 1 ]; then
            if [ -f "${FILE}.${EXT_OUT}" ]; then
                rm ${FILE}.${EXT_OUT}
            fi
        fi
        if [ ${LINK_ENUM} -ne 2 ]; then
            if [ -f "${FILE}.o" ]; then
                rm ${FILE}.o
            fi
        fi
    done
    if [ -f "${PACKAGE_DIR}/crt.o" ]; then
        rm ${PACKAGE_DIR}/crt.o
    fi
    exit ${EXIT_CODE}
}

function em() {
    echo "\033[1m‘${1}’\033[0m"
}

function raise_error () {
    ERROR_MESSAGE="${1}"
    echo -e "${PACKAGE_NAME}: \033[0;31merror:\033[0m ${ERROR_MESSAGE}, see $(em "--help")" 1>&2
    clean_exit 1
}

function shift_arg () {
    if [ ${i} -ge ${ARGC} ]; then
        ARG=""
        return 1
    fi
    ARG="${ARGV[${i}]}"
    i=$((i+1))
    return 0
}

function parse_help_arg () {
    if [ ! "${ARG}" = "--help" ]; then
        return 1
    fi
    usage
}

function parse_verbose_arg () {
    if [ ! "${ARG}" = "-v" ]; then
        return 1
    fi
    IS_VERBOSE=1
    DEBUG_ENUM=1
    return 0
}

function parse_debug_arg () {
    case "${ARG}" in
        "--lex")
            DEBUG_ENUM=255
            ;;
        "--parse")
            DEBUG_ENUM=254
            ;;
        "--validate")
            DEBUG_ENUM=253
            ;;
        "--tacky")
            DEBUG_ENUM=252
            ;;
        "--codegen")
            DEBUG_ENUM=251
            ;;
        *)
            return 1
    esac
    return 0
}

function parse_optimize_arg () {
    case "${ARG}" in
        "-O0")
            OPTIM_L1_MASK=0
            OPTIM_L2_ENUM=0
            ;;
        "--fold-constants")
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 0))
            ;;
        "--propagate-copies")
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 1))
            ;;
        "--eliminate-unreachable-code")
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 2))
            ;;
        "--eliminate-dead-stores")
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 3))
            ;;
        "--optimize")
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 0))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 1))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 2))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 3))
            ;;
        "-O1")
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 0))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 1))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 2))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 3))
            ;;
        "--no-allocation")
            OPTIM_L2_ENUM=0
            ;;
        "--no-coalescing")
            OPTIM_L2_ENUM=1
            ;;
        "--allocate-register")
            OPTIM_L2_ENUM=2
            ;;
        "-O2")
            OPTIM_L2_ENUM=2
            ;;
        "-O3")
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 0))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 1))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 2))
            OPTIM_L1_MASK=$((OPTIM_L1_MASK | 1 << 3))
            OPTIM_L2_ENUM=2
            ;;
        *)
            return 1
    esac
    return 0
}

function parse_preproc_arg () {
    if [ ! "${ARG}" = "-E" ]; then
        return 1
    fi
    IS_PREPROC=1
    return 0
}

function parse_link_arg () {
    case "${ARG}" in
        "-s")
            LINK_ENUM=1
            ;;
        "-c")
            LINK_ENUM=2
            ;;
        *)
            return 1
    esac
    return 0
}

function parse_include_arg () {
    if [[ "${ARG}" != "-I"* ]]; then
        return 1
    fi
    ARG="${ARG:2}"
    if [[ "${ARG}" == "-"* ]]; then
        raise_error "unknown or malformed option: $(em "${ARG}")"
    fi
    INCLUDE_DIR="$(readlink -f ${ARG})"
    if [ ! -d "${INCLUDE_DIR}" ]; then
        raise_error "cannot find $(em "${INCLUDE_DIR}"): no such directory"
    fi
    INCLUDE_DIRS="${INCLUDE_DIRS} ${INCLUDE_DIR}/"
    return 0
}

function parse_linkdir_arg () {
    if [[ "${ARG}" != "-L"* ]]; then
        return 1
    fi
    ARG="${ARG:2}"
    if [[ "${ARG}" == "-"* ]]; then
        raise_error "unknown or malformed option: $(em "${ARG}")"
    fi
    LINK_DIR="$(readlink -f ${ARG})"
    if [ ! -d "${LINK_DIR}" ]; then
        raise_error "cannot find $(em "${LINK_DIR}"): no such directory"
    fi
    LINK_DIRS="${LINK_DIRS} -L${LINK_DIR}/"
    return 0
}

function parse_lib_arg () {
    if [[ "${ARG}" != "-l"* ]]; then
        return 1
    fi
    LINK_LIBS="${LINK_LIBS} ${ARG}"
    return 0
}

function parse_name_arg () {
    if [ ! "${ARG}" = "-o" ]; then
        return 1
    fi
    shift_arg
    if [ ${?} -ne 0 ]; then
        raise_error "no input files"
    elif [[ "${ARG}" == *".${EXT_IN}" ]]; then
        raise_error "missing filename after $(em "-o")"
    fi
    NAME_OUT=${ARG}
    return 0
}

function parse_file_arg () {
    if [[ "${ARG}" == "-"* ]]; then
        raise_error "unknown or malformed option: $(em "${ARG}")"
    fi
    FILE="$(readlink -f ${ARG})"
    if [ ! -f "${FILE}" ]; then
        raise_error "cannot find $(em "${FILE}"): no such file"
    elif [[ "${FILE}" != *".${EXT_IN}" ]]; then
        raise_error "$(em "${FILE}"): file format not recognized"
    else
        FILES="${FILE%.*}"
        if [ -z "${NAME_OUT}" ]; then
            NAME_OUT=${FILES}
        fi
    fi
    return 0
}

function parse_file_2_arg () {
    if [[ "${ARG}" == "-"* ]]; then
        raise_error "unknown or malformed option: $(em "${ARG}")"
    fi
    FILE="$(readlink -f ${ARG})"
    if [ ! -f "${FILE}" ]; then
        raise_error "cannot find $(em "${FILE}"): no such file"
    elif [[ "${FILE}" != *".${EXT_IN}" ]]; then
        raise_error "$(em "${FILE}"): file format not recognized"
    else
       FILES="${FILES} ${FILE%.*}"
    fi
    return 0
}

function parse_args () {
    i=0

    shift_arg
    if [ ${?} -ne 0 ]; then
        raise_error "no input files"
    fi

    parse_help_arg

    parse_verbose_arg
    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then
            raise_error "no input files"
        fi
    fi

    parse_debug_arg
    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then
            raise_error "no input files"
        fi
    fi

    while :; do
        parse_optimize_arg
        if [ ${?} -eq 0 ]; then
            shift_arg
            if [ ${?} -ne 0 ]; then
                raise_error "no input files"
            fi
        else
            break
        fi
    done

    parse_preproc_arg
    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then
            raise_error "no input files"
        fi
    fi

    parse_link_arg
    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then
            raise_error "no input files"
        fi
    fi

    while :; do
        parse_include_arg
        if [ ${?} -eq 0 ]; then
            shift_arg
            if [ ${?} -ne 0 ]; then
                raise_error "no input files"
            fi
        else
            break
        fi
    done

    while :; do
        parse_linkdir_arg
        if [ ${?} -eq 0 ]; then
            shift_arg
            if [ ${?} -ne 0 ]; then
                raise_error "no input files"
            fi
        else
            break
        fi
    done

    while :; do
        parse_lib_arg
        if [ ${?} -eq 0 ]; then
            shift_arg
            if [ ${?} -ne 0 ]; then
                raise_error "no input files"
            fi
        else
            break
        fi
    done

    parse_name_arg
    if [ ${?} -eq 0 ]; then
        shift_arg
        if [ ${?} -ne 0 ]; then
            raise_error "no input files"
        fi
    fi

    parse_file_arg
    while :; do
        shift_arg
        if [ ${?} -ne 0 ]; then
            break
        fi
        parse_file_2_arg
    done
    return 0
}

function add_includedirs () {
    if [ ! -z "${INCLUDE_DIRS}" ]; then
        INCLUDE_DIRS=$(echo "${INCLUDE_DIRS}" | tr ' ' '\n' | sort --uniq | tr '\n' ' ')
    fi
    return 0
}

function add_linkdirs () {
    if [ ! -z "${LINK_DIRS}" ]; then
        LINK_DIRS=$(echo "${LINK_DIRS}" | tr ' ' '\n' | sort --uniq | tr '\n' ' ')
    fi
    if [ -d "${LIBC_DIR}" ]; then
        LINK_DIRS="${LINK_DIRS} -L${LIBC_DIR}"
    fi
    return 0
}

function add_linklibs () {
    if [ ! -z "${LINK_LIBS}" ]; then
        LINK_LIBS=$(echo "${LINK_LIBS}" | tr ' ' '\n' | sort --uniq | tr '\n' ' ')
    fi
    if [ -d "${LIBC_DIR}" ]; then
        if [ -f "${LIBC_DIR}/lib${PACKAGE_NAME}.so" ]; then
            LINK_LIBS="${LINK_LIBS} -l${PACKAGE_NAME}"
        fi
    fi
    return 0
}

function preprocess () {
    if [ ${IS_PREPROC} -eq 1 ]; then
        for FILE in ${FILES}; do
            verbose "Preprocess (${PP}) -> ${FILE}.i"
            if [ "${PP}" = m4 ]; then
                m4 -P ${FILE}.${EXT_IN} > ${FILE}.i
                if [ ${?} -ne 0 ]; then
                    raise_error "preprocessing failed"
                fi
            else
                ${CC} -E -P ${FILE}.${EXT_IN} -o ${FILE}.i
                if [ ${?} -ne 0 ]; then
                    raise_error "preprocessing failed"
                fi
            fi
        done
        EXT_IN="i"
    fi
    return 0
}

function compile () {
    for FILE in ${FILES}; do
        SOURCE_DIR="$(dirname ${FILE})/"
        echo "${INCLUDE_DIRS}" | grep -q "${SOURCE_DIR} "
        if [ ${?} -eq 0 ]; then
            SOURCE_DIR=""
        fi
        verbose "Compile (${PACKAGE_NAME}) -> ${FILE}.${EXT_OUT}"
        ${PACKAGE_DIR}/${PACKAGE_NAME} ${DEBUG_ENUM} ${OPTIM_L1_MASK} ${OPTIM_L2_ENUM} ${FILE}.${EXT_IN} ${LIBC_DIR} ${SOURCE_DIR} ${INCLUDE_DIRS}
        if [ ${?} -ne 0 ]; then
            raise_error "compilation failed"
        fi
    done
    return 0
}

function assemble () {
    for FILE in ${FILES}; do
        verbose "Assemble (as) -> ${FILE}.o"
        as ${AS_FLAGS} ${FILE}.${EXT_OUT} -o ${FILE}.o
        if [ ${?} -ne 0 ]; then
            raise_error "assembling failed"
        fi
    done
}

function link () {
    if [ ${DEBUG_ENUM} -le 127 ]; then
        case ${LINK_ENUM} in
            0)
                assemble
                if [ ! -f "${LD_LIB_64}" ]; then
                    LD_LIB_64=""
                fi
                if [ ! -z "${LD_LIB_64}" ]; then
                    verbose "Assemble (as) -> ${PACKAGE_DIR}/crt.o"
                    as ${AS_FLAGS} ${PACKAGE_DIR}/crt.${EXT_OUT} -o ${PACKAGE_DIR}/crt.o
                    if [ ${?} -ne 0 ]; then
                        raise_error "assembling failed"
                    fi
                    verbose "Link (ld) -> ${NAME_OUT}"
                    ld --build-id -m elf_x86_64 --hash-style=gnu -dynamic-linker ${LD_LIB_64} -pie -lc ${PACKAGE_DIR}/crt.o \
                        ${FILES// /.o }.o ${LINK_DIRS} ${LINK_LIBS} -o ${NAME_OUT}
                    if [ ${?} -ne 0 ]; then
                        LD_LIB_64=""
                    fi
                fi
                if [ -z "${LD_LIB_64}" ]; then
                    verbose "Link (${CC}) -> ${NAME_OUT}"
                    ${CC} ${FILES// /.o }.o ${LINK_DIRS} ${LINK_LIBS} -o ${NAME_OUT}
                    if [ ${?} -ne 0 ]; then
                        raise_error "linking failed"
                    fi
                fi
                ;;
            1)
                ;;
            2)
                assemble
                ;;
            *)
                raise_error "linking failed"
        esac
    fi
    return 0
}

EXT_IN="c"
EXT_OUT="s"
if [ -f "${PACKAGE_DIR}/fileext.cfg" ]; then
    EXT_IN="$(cat ${PACKAGE_DIR}/fileext.cfg)"
    if [[ "${EXT_IN}" != "c"* ]]; then
        PP="m4"
    fi
fi

IS_VERBOSE=0
IS_PREPROC=0

DEBUG_ENUM=0
LINK_ENUM=0

OPTIM_L1_MASK=0
OPTIM_L2_ENUM=2

INCLUDE_DIRS=""
LINK_DIRS=""
LINK_LIBS=""
NAME_OUT=""
FILES=""

parse_args
add_includedirs
add_linkdirs
add_linklibs

preprocess
compile
link

clean_exit 0
