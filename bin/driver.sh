#!/bin/bash

PACKAGE_DIR="$(echo $(dirname $(readlink -f ${0})))"
PACKAGE_NAME="$(cat $(echo ${PACKAGE_DIR})/package_name.txt)"
CC="gcc -pedantic-errors -std=c17"

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
    echo "    --codeemit  print  emission  stage and exit"
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
    echo "    -E  enable macro expansion with gcc"
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
    echo "FILES:  list of .c files to compile"
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
    exit ${EXIT_CODE}
}

function em() {
    echo "\033[1m‘${1}’\033[0m"
}

function raise_error () {
    ERROR_MESSAGE="${1}"
    echo -e "${PACKAGE_NAME}: \033[0;31merror:\033[0m ${ERROR_MESSAGE}" 1>&2
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
        "--codeemit")
            DEBUG_ENUM=250
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
        "--optimize") ;&
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
        "--allocate-register") ;&
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
       IS_FILE_2=1
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
    return 0
}

function add_linklibs () {
    if [ ! -z "${LINK_LIBS}" ]; then
        LINK_LIBS=$(echo "${LINK_LIBS}" | tr ' ' '\n' | sort --uniq | tr '\n' ' ')
    fi
    return 0
}

function preprocess () {
    if [ ${IS_PREPROC} -eq 1 ]; then
        for FILE in ${FILES}; do
            verbose "Preprocess -> ${FILE}.i"
            ${CC} -E -P ${FILE}.c -o ${FILE}.i
            if [ ${?} -ne 0 ]; then
                raise_error "preprocessing failed"
            fi
        done
        EXT_IN="i"
    fi
    return 0
}

function compile () {
    for FILE in ${FILES}; do
        verbose "Compile    -> ${FILE}.${EXT_OUT}"
        SOURCE_DIR="$(dirname ${FILE})/"
        echo "${INCLUDE_DIRS}" | grep -q ${SOURCE_DIR}
        if [ ${?} -eq 0 ]; then
            SOURCE_DIR=""
        fi
        STDOUT=$(${PACKAGE_DIR}/${PACKAGE_NAME} ${DEBUG_ENUM} ${OPTIM_L1_MASK} ${OPTIM_L2_ENUM} ${FILE}.${EXT_IN} ${SOURCE_DIR} ${INCLUDE_DIRS} 2>&1)
        if [ ${?} -ne 0 ]; then
            echo "${STDOUT}" | tail -n +3 1>&2
            raise_error "compilation failed"
        fi
        if [ ! -z "${STDOUT}" ]; then
            echo "${STDOUT}"
        fi
        if [ ${DEBUG_ENUM} -eq 250 ]; then
            cat ${FILE}.${EXT_OUT}
        fi
    done
    return 0
}

function link () {
    if [ ${DEBUG_ENUM} -le 127 ]; then
        case ${LINK_ENUM} in
            0)
                FILES_OUT="${FILES}.${EXT_OUT}"
                if [ ${IS_FILE_2} -eq 1 ]; then
                    FILES_OUT="$(echo "${FILES_OUT}" |\
                        sed "s/ /.${EXT_OUT} /g")"
                fi
                ${CC} ${FILES_OUT} ${LINK_DIRS} ${LINK_LIBS} -o ${NAME_OUT}
                if [ ${?} -ne 0 ]; then
                    raise_error "linking failed"
                fi
                verbose "Link       -> ${NAME_OUT}"
                ;;
            1)
                ;;
            2)
                for FILE in ${FILES}; do
                    ${CC} -c ${FILE}.${EXT_OUT} ${LINK_DIRS} ${LINK_LIBS} -o ${FILE}.o
                    if [ ${?} -ne 0 ]; then
                        raise_error "assembling failed"
                    fi
                    verbose "Assemble   -> ${FILE}.o"
                done
                ;;
            *)
                raise_error "linking failed"
        esac
    fi
    return 0
}

IS_VERBOSE=0
IS_PREPROC=0
IS_FILE_2=0

DEBUG_ENUM=0
LINK_ENUM=0

OPTIM_L1_MASK=0
OPTIM_L2_ENUM=2

INCLUDE_DIRS=""
LINK_DIRS=""
LINK_LIBS=""
NAME_OUT=""
FILES=""

EXT_IN="c"
EXT_OUT="s"

parse_args
add_includedirs
add_linkdirs
add_linklibs

preprocess
compile
link

clean_exit 0
