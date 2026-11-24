#!/usr/bin/env bash

KERNEL_NAME="$(uname -s)"
PACKAGE_DIR="$(dirname $(readlink -f ${0}))"
PACKAGE_NAME="wheelcc"
if [ ! -z "${1}" ]; then
    PACKAGE_NAME="${1}"
fi
PACKAGE_LIBC="${PACKAGE_DIR}/libc"

function em () {
    echo "\033[1m‘${1}’\033[0m"
}

function log_warning () {
    WARN_MESSAGE="${1}"
    echo -e "\033[1;34mwarning:\033[0m ${WARN_MESSAGE}"
}

function raise_error () {
    ERROR_MESSAGE="${1}"
    echo -e "configure: \033[0;31merror:\033[0m ${ERROR_MESSAGE}" 1>&2
    exit 1
}

echo -n "${PACKAGE_NAME}" > ${PACKAGE_DIR}/pkgname.cfg
if [ ${?} -ne 0 ]; then
    raise_error "failed to set package name"
fi
echo "-- Package name ${PACKAGE_NAME}"

if [ ! -d "${PACKAGE_LIBC}/" ]; then
    mkdir -p ${PACKAGE_LIBC}/
fi

CC="gcc"
CXX="g++"
if [[ "${KERNEL_NAME}" == "Darwin"* ]]; then
    CC="clang"
    CXX="clang++"
elif [[ "${KERNEL_NAME}" == "FreeBSD"* ]]; then
    CC="clang"
    CXX="clang++"
fi

INSTALL_CC=0
${CC} --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_CC=1
elif [ "${CC}" = "clang" ]; then
    CLANG_MAJOR_VERSION=$(clang -dumpversion | cut -d"." -f1)
    if [ ${CLANG_MAJOR_VERSION} -lt 5 ]; then
        INSTALL_CC=1
    fi
else
    GCC_MAJOR_VERSION=$(gcc -dumpversion | cut -d"." -f1)
    if [ ${GCC_MAJOR_VERSION} -lt 8 ]; then
        INSTALL_CC=1
    elif [ ${GCC_MAJOR_VERSION} -eq 8 ]; then
        GCC_MINOR_VERSION=$(gcc -dumpfullversion | cut -d"." -f2)
        if [ ${GCC_MINOR_VERSION} -eq 0 ]; then
            INSTALL_CC=1
        fi
    fi
fi

${CXX} --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_CC=1
fi

CC_VER="8.1.0"
INSTALL_BASE=0
if [ "${CC}" = "clang" ]; then
    CC_VER="5.0.0"
    if [ ${INSTALL_CC} -ne 0 ]; then
        INSTALL_BASE=1
        INSTALL_CC=0
    fi
fi

as --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_CC=1
fi

PKG_M4=""
MSG_M4=""
if [ -f "${PACKAGE_DIR}/fileext.cfg" ]; then
    EXT_IN="$(cat ${PACKAGE_DIR}/fileext.cfg)"
    if [[ "${EXT_IN}" != "c"* ]]; then
        m4 <(echo "") > /dev/null 2>&1
        if [ ${?} -ne 0 ]; then
            INSTALL_CC=1
            PKG_M4="m4"
            MSG_M4="$(em "m4"), "
        fi
    fi
fi

# Check for MacOS first, as it supports only bash <= 3.2
if [[ "${KERNEL_NAME}" == "Darwin"* ]]; then
    if [ ${INSTALL_BASE} -ne 0 ]; then
        INSTALL_CC=1
    fi
    if [ ${INSTALL_CC} -ne 0 ]; then
        log_warning "install ${MSG_M4}$(em "clang") >= 5.0.0 before building"
    fi

    echo -e "configuration was successful, build with $(em "./make.sh")"
    exit 0
fi

ld --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_CC=1
fi

INSTALL_Y="n"
if [ ${INSTALL_CC} -ne 0 ]; then
    echo -e -n "install missing dependencies $(em "binutils"), ${MSG_M4}$(em "${CC}") >= ${CC_VER}? [y/n]: "
    read -p "" INSTALL_Y
fi

if [ "${INSTALL_Y}" = "y" ]; then
    DISTRO="FreeBSD"
    if [[ "${KERNEL_NAME}" != "FreeBSD"* ]]; then
        DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
    fi
    case "${DISTRO}" in
        "FreeBSD")
            sudo pkg update && sudo pkg install -y binutils ${PKG_M4}
            INSTALL_CC=${?}
            ;;
        "Debian GNU/Linux") ;&
        "Linux Mint") ;&
        "Ubuntu")
            sudo apt-get update && sudo apt-get -y install binutils gcc g++ ${PKG_M4}
            INSTALL_CC=${?}
            ;;
        "openSUSE Leap") ;&
        "Rocky Linux")
            sudo dnf check-update
            sudo dnf -y install binutils.x86_64 gcc.x86_64 gcc-c++.x86_64 ${PKG_M4//m4/m4.x86_64}
            INSTALL_CC=${?}
            ;;
        "Arch Linux") ;&
        "EndeavourOS")
            sudo pacman -Syy && yes | sudo pacman -S binutils gcc ${PKG_M4}
            INSTALL_CC=${?}
            ;;
        # Not tested yet
        # "elementary OS") ;&
        # "Kali GNU/Linux") ;&
        # "Pop!_OS") ;&
        # "AlmaLinux") ;&
        # "CentOS Linux") ;&
        # "CentOS Stream") ;&
        # "Clear Linux OS") ;&
        # "ClearOS") ;&
        # "Fedora") ;&
        # "Fedora Linux") ;&
        # "Mageia") ;&
        # "Red Hat Enterprise Linux") ;&
        # "Manjaro") ;&
        *)
            # this distribution was not tested yet, use at your own risk!
            INSTALL_Y="n"
    esac
fi

if [ ${INSTALL_BASE} -ne 0 ]; then
    INSTALL_CC=1
fi
if [ ${INSTALL_CC} -ne 0 ]; then
    if [ "${INSTALL_Y}" = "y" ]; then
        log_warning "failed to install $(em "binutils"), ${MSG_M4}$(em "${CC}")"
    fi
    log_warning "install $(em "binutils"), ${MSG_M4}$(em "${CC}") >= ${CC_VER} before building"
fi

echo -e "configuration was successful, build with $(em "./make.sh")"
exit 0
