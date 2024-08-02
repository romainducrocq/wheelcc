#!/bin/bash

PACKAGE_NAME="wheelcc"

function apt_install () {
    i=0
    for PKG in $(echo "\
        gcc   \
        g++   \
        cmake \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            if [ ${GET_UPDATE} -eq 0 ]; then
                sudo apt-get update
                sudo apt-get -y upgrade
                GET_UPDATE=1
            fi
            sudo apt-get -y install ${PKG}
        fi
        i=$((i+1))
    done
}

function dnf_install () {
    i=0
    for PKG in $(echo "\
        gcc     \
        gcc-c++ \
        cmake   \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            if [ ${GET_UPDATE} -eq 0 ]; then
                sudo dnf -y upgrade
                GET_UPDATE=1
            fi
            sudo dnf -y install ${PKG}
        fi
        i=$((i+1))
    done
}

function pacman_install () {
    i=0
    for PKG in $(echo "\
        gcc   \
        gcc   \
        cmake \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            if [ ${GET_UPDATE} -eq 0 ]; then
                sudo pacman -Syu
                GET_UPDATE=1
            fi
            sudo pacman -Syu ${PKG}
        fi
        i=$((i+1))
    done
}

function other_install () {
    i=0
    for PKG in $(echo "\
        gcc   \
        g++   \
        cmake \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            if [ ${GET_UPDATE} -eq 0 ]; then
                echo -e -n "\033[1;34mwarning:\033[0m install the following packages before building: "
                GET_UPDATE=1
            else
                echo -n ", "
            fi
            echo -e -n "\033[1m‘${PKG}’\033[0m"
        fi
        i=$((i+1))
    done
    if [ ${GET_UPDATE} -eq 1 ]; then
        echo ""
    fi
}

GET_UPDATE=0

INSTALL_GCC=0
INSTALL_GPP=1
INSTALL_CMAKE=2

INSTALL_PKGS=(
    0 0 0
)

gcc --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_GCC}]=1
fi

g++ --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_GPP}]=1
fi

cmake --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_CMAKE}]=1
fi

DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
case ${DISTRO} in
    # Debian-based
    "Debian GNU/Linux") ;&
    "elementary OS") ;&
    "Kali GNU/Linux") ;&
    "Linux Mint") ;&
    "Pop!_OS") ;&
    "Raspbian GNU/Linux") ;&
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
    "Rocky Linux") ;&
    "Scientific Linux")
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
        other_install
esac

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
