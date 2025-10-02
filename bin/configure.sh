#!/bin/bash

PACKAGE_NAME="wheelcc"
if [ ! -z "${1}" ]; then
    PACKAGE_NAME="${1}"
fi

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then
    echo -e "\033[0;31merror:\033[0m configuration failed" 1>&2
    exit 1
fi
echo "-- Package name ${PACKAGE_NAME}"

INSTALL_CC=0

# Check for MacOS first, as it supports only bash <= 3.2
if [[ "$(uname -s)" = "Darwin"* ]]; then
    clang --help > /dev/null 2>&1
    if [ ${?} -ne 0 ]; then
        INSTALL_CC=1
    else
        CLANG_MAJOR_VERSION=$(clang -dumpversion | cut -d"." -f1)
        if [ ${CLANG_MAJOR_VERSION} -lt 5 ]; then
            INSTALL_CC=1
        fi
    fi

    clang++ --help > /dev/null 2>&1
    if [ ${?} -ne 0 ]; then
        INSTALL_CC=1
    fi

    if [ ${INSTALL_CC} -ne 0 ]; then
        echo -e "\033[1;34mwarning:\033[0m install \033[1m‘clang’\033[0m >= 5.0.0 before building"
    fi

    echo -e "configuration was successful, build with \033[1m‘./make.sh’\033[0m"
    exit 0
fi

as --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_CC=1
fi

ld --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_CC=1
fi

gcc --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_CC=1
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

INSTALL_Y="n"
if [ ${INSTALL_CC} -ne 0 ]; then
    echo -e -n "install missing dependencies \033[1m‘binutils’\033[0m, \033[1m‘gcc’\033[0m >= 8.1.0? [y/n]: "
    read -p "" INSTALL_Y
fi

if [ "${INSTALL_Y}" = "y" ]; then
    DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
    case "${DISTRO}" in
        "Debian GNU/Linux") ;&
        "Linux Mint") ;&
        "Ubuntu")
            sudo apt-get update && sudo apt-get -y install binutils gcc g++
            INSTALL_CC=${?}
            ;;
        "openSUSE Leap") ;&
        "Rocky Linux")
            sudo dnf check-update && sudo dnf -y install binutils.x86_64 gcc.x86_64 gcc-c++.x86_64
            INSTALL_CC=${?}
            ;;
        "Arch Linux") ;&
        "EndeavourOS")
            sudo pacman -Syy && yes | sudo pacman -S binutils gcc
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
            echo -e "\033[1;34mwarning:\033[0m this distribution was not tested yet, use at your own risk!"
            INSTALL_Y="n"
    esac
fi

if [ ${INSTALL_CC} -ne 0 ]; then
    if [ "${INSTALL_Y}" = "y" ]; then
        echo -e "\033[1;34mwarning:\033[0m failed to install \033[1m‘binutils’\033[0m, \033[1m‘gcc’\033[0m"
    fi
    echo -e "\033[1;34mwarning:\033[0m install \033[1m‘binutils’\033[0m, \033[1m‘gcc’\033[0m >= 8.1.0 before building"
fi

echo -e "configuration was successful, build with \033[1m‘./make.sh’\033[0m"
exit 0
