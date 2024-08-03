#!/bin/bash

function apt_install () {
    i=0
    for PKG in $(echo "\
        diffutils \
        valgrind  \
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
        diffutils \ # TODO
        valgrind.x86_64  \
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
        diffutils \
        valgrind  \
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
        diffutils \
        valgrind  \
    ")
    do
        if [ ${INSTALL_PKGS[${i}]} -eq 1 ]; then
            if [ ${GET_UPDATE} -eq 0 ]; then
                echo -e -n "\033[1;34mwarning:\033[0m install the following packages before testing: "
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

DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
case ${DISTRO} in
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
        other_install
esac

exit 0
