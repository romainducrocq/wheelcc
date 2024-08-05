#!/bin/bash

function apt_install () {
    i=0
    sudo apt-get update
    sudo apt-get -y upgrade
    for PKG in $(echo "\
        diffutils \
        valgrind  \
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
        diffutils.x86_64 \
        valgrind.x86_64  \
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
        diffutils \
        valgrind  \
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
        echo -e "\033[1;34mwarning:\033[0m install the following packages before testing: ${INSTALL_MSG}"
    fi
}

function get_install () {
    i=0
    for PKG in $(echo "\
        diffutils \
        valgrind  \
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

INSTALL_DIFFUTILS=0
INSTALL_VALGRIND=1

INSTALL_PKGS=(
    0 0
)

diff --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_DIFFUTILS}]=1
fi

valgrind --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[${INSTALL_VALGRIND}]=1
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

exit 0
