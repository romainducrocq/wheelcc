#!/bin/bash

PACKAGE_NAME="wheelcc"

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then
    echo -e "\033[0;31merror:\033[0m configuration failed" 1>&2
    exit 1
fi

function failed_install () {
    echo -e "\033[1;34mwarning:\033[0m failed to install \033[1m‘${1}’\033[0m"
}

function apt_install_gcc () {
    sudo apt-get update
    if [ ${?} -ne 0 ]; then failed_install "gcc"; return 1; fi
    sudo apt-get -y install gcc g++
    if [ ${?} -ne 0 ]; then failed_install "gcc"; return 1; fi
    return 0
}

function dnf_install_gcc () {
    sudo dnf check-update
    if [ ${?} -ne 0 ]; then failed_install "gcc"; return 1; fi
    sudo dnf -y install gcc.x86_64 gcc-c++.x86_64
    if [ ${?} -ne 0 ]; then failed_install "gcc"; return 1; fi
    return 0
}

function pacman_install_gcc () {
    sudo pacman -Syy
    if [ ${?} -ne 0 ]; then failed_install "gcc"; return 1; fi
    yes | sudo pacman -S gcc
    if [ ${?} -ne 0 ]; then failed_install "gcc"; return 1; fi
    return 0
}

INSTALL_GCC=0
gcc --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_GCC=1
else
    GCC_MAJOR_VERSION=$(gcc -dumpversion | cut -d"." -f1)
    if [ ${GCC_MAJOR_VERSION} -lt 8 ]; then
        INSTALL_GCC=1
    elif [ ${GCC_MAJOR_VERSION} -eq 8 ]; then
        GCC_MINOR_VERSION=$(gcc -dumpfullversion | cut -d"." -f2)
        if [ ${GCC_MINOR_VERSION} -eq 0 ]; then
            INSTALL_GCC=1
        fi
    fi
fi

INSTALL_Y="n"
if [ ${INSTALL_GCC} -ne 0 ]; then
    echo -e -n "install missing dependency \033[1m‘gcc’\033[0m >= 8.1.0? [y/n]: "
    read -p "" INSTALL_Y
fi

if [ "${INSTALL_Y}" = "y" ]; then
    DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
    case "${DISTRO}" in
        "Debian GNU/Linux") ;&
        "Linux Mint") ;&
        "Ubuntu")
            apt_install_gcc
            INSTALL_GCC=${?}
            ;;
        "openSUSE Leap") ;&
        "Rocky Linux")
            dnf_install_gcc
            INSTALL_GCC=${?}
            ;;
        "Arch Linux") ;&
        "EndeavourOS")
            pacman_install_gcc
            INSTALL_GCC=${?}
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
    esac
fi

if [ ${INSTALL_GCC} -ne 0 ]; then
    echo -e "\033[1;34mwarning:\033[0m install \033[1m‘gcc’\033[0m >= 8.1.0 before building"
fi

echo -e "configuration was successful, build with \033[1m‘./make.sh’\033[0m"
exit 0
