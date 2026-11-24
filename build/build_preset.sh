#!/usr/bin/env bash

PACKAGE_NAME="$(cat ../bin/pkgname.cfg)"

if [ -f "../bin/${PACKAGE_NAME}" ]; then
    rm ../bin/${PACKAGE_NAME}
fi

function em () {
    echo "\033[1m‘${1}’\033[0m"
}

function raise_error () {
    ERROR_MESSAGE="${1}"
    echo -e "build_preset: \033[0;31merror:\033[0m ${ERROR_MESSAGE}" 1>&2
    exit 1
}

case "${1}" in
    "clang")
        CLANG_MAJOR_VERSION=$(clang -dumpversion | cut -d"." -f1)
        if [ ${CLANG_MAJOR_VERSION} -lt 5 ]; then
            raise_error "requires $(em "clang") >= 5.0.0"
        fi
        ;;
    "gcc")
        GCC_MAJOR_VERSION=$(gcc -dumpversion | cut -d"." -f1)
        if [ ${GCC_MAJOR_VERSION} -lt 8 ]; then
            raise_error "requires $(em "gcc") >= 8.1.0"
        elif [ ${GCC_MAJOR_VERSION} -eq 8 ]; then
            GCC_MINOR_VERSION=$(gcc -dumpfullversion | cut -d"." -f2)
            if [ ${GCC_MINOR_VERSION} -eq 0 ]; then
                raise_error "requires $(em "gcc") >= 8.1.0"
            fi
        fi
        ;;
    *)
        exit 1
esac

exit 0
