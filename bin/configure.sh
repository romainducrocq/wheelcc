#!/bin/bash

PACKAGE_NAME="wheelcc"

function apt_install () {
    i=0
    sudo apt-get update
    sudo apt-get -y upgrade
    for PKG in $(echo "\
        gcc   \
        g++   \
        make  \
        cmake \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            sudo apt-get -y install ${PKG}
        fi
        i=$((i+1))
    done
}

function dnf_install () {
    i=0
    sudo dnf -y upgrade
    for PKG in $(echo "\
        gcc.x86_64     \
        gcc-c++.x86_64 \
        make.x86_64    \
        cmake.x86_64   \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            sudo dnf -y install ${PKG}
        fi
        i=$((i+1))
    done
}

function pacman_install () {
    i=0
    sudo pacman -Syu
    for PKG in $(echo "\
        gcc   \
        gcc   \
        make  \
        cmake \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            sudo pacman -Syu ${PKG}
        fi
        i=$((i+1))
    done
}

function no_install () {
    if [ ! -z "${INSTALL_MSG}" ]; then
        echo -e "\033[1;34mwarning:\033[0m install the following packages before building: ${INSTALL_MSG}"
    fi
}

function get_install () {
    i=0
    for PKG in $(echo "\
        gcc   \
        g++   \
        make  \
        cmake \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            if [ ! "${INSTALL_Y}" = "y" ]; then
                INSTALL_Y="y"
            else
                INSTALL_MSG="${INSTALL_MSG}, "
            fi
            INSTALL_MSG="${INSTALL_MSG}\033[1m‘${PKG}’\033[0m"
        fi
        i=$((i+1))
    done

    if [ "${INSTALL_Y}" = "y" ]; then
        echo -e -n "install missing dependencies ${INSTALL_MSG}? [y/n]: "
        read -p "" INSTALL_Y
    fi
}

INSTALL_Y=""
INSTALL_MSG=""

INSTALL_GCC=0
INSTALL_GPP=1
INSTALL_MAKE=2
INSTALL_CMAKE=3

INSTALL_PKGS=(
    0 0 0 0
)

gcc --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_GCC}]=1
else
    GCC_MAJOR_VERSION=$(gcc -dumpversion | cut -d"." -f1)
    if [ ${GCC_MAJOR_VERSION} -lt 8 ]; then
        INSTALL_PKGS[${INSTALL_GCC}]=1
    elif [ ${GCC_MAJOR_VERSION} -eq 8 ]; then
        GCC_MINOR_VERSION=$(gcc -dumpfullversion | cut -d"." -f2)
        if [ ${GCC_MINOR_VERSION} -eq 0 ]; then
            INSTALL_PKGS[${INSTALL_GCC}]=1
        fi
    fi
fi

g++ --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_GPP}]=1
fi

make --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_MAKE}]=1
fi

cmake --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_CMAKE}]=1
fi

get_install

if [ "${INSTALL_Y}" = "y" ]; then
    DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
    case "${DISTRO}" in
        # Debian-based
        "Debian GNU/Linux") ;&
        "elementary OS") ;&
        "Kali GNU/Linux") ;&
        "Linux Mint") ;&
        "Pop!_OS") ;&
        "Ubuntu")
            apt_install
            ;;
        # RPM-based
        "AlmaLinux") ;&
        "CentOS Linux") ;&
        "CentOS Stream") ;&
        "Clear Linux OS") ;&
        "ClearOS") ;&
        "Fedora") ;&
        "Fedora Linux") ;&
        "Mageia") ;&
        "openSUSE Leap") ;&
        "Red Hat Enterprise Linux") ;&
        "Rocky Linux")
            dnf_install
            ;;
        # Pacman-based
        "Arch Linux") ;&
        "EndeavourOS") ;&
        "Manjaro")
            pacman_install
            ;;
        # Other
        *)
            no_install
    esac
else
    no_install
fi

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then 
    echo -e "\033[0;31merror:\033[0m configuration failed" 1>&2
    exit 1
fi

echo -e "configuration was successful, build with \033[1m‘./make.sh’\033[0m"
exit 0
