#!/usr/bin/env bash

KERNEL_NAME="$(uname -s)"

function em () {
    echo "\033[1m‘${1}’\033[0m"
}

function log_warning () {
    WARN_MESSAGE="${1}"
    echo -e "\033[1;34mwarning:\033[0m ${WARN_MESSAGE}"
}

# Check for MacOS first, as it supports only bash <= 3.2
if [[ "${KERNEL_NAME}" == "Darwin"* ]]; then
    cmake --help > /dev/null 2>&1
    if [ ${?} -ne 0 ]; then
        echo -e -n "install missing dependencies $(em "cmake")? [y/n]: "
        read -p "" INSTALL_CMAKE
        if [ "${INSTALL_CMAKE}" = "y" ]; then
            brew install cmake
            if [ ${?} -ne 0 ]; then
                log_warning "failed to install $(em "cmake")"
                INSTALL_CMAKE="n"
            fi
        fi
        if [ ! "${INSTALL_CMAKE}" = "y" ]; then
            log_warning "install the missing dependencies before testing"
        fi
    fi

    exit 0
fi

PKG_MAKE="make"
if [[ "${KERNEL_NAME}" == "FreeBSD"* ]]; then
    PKG_MAKE="clang"
fi

INSTALL_PKGS=(0 0 0 0)
INSTALL_DEPS="${PKG_MAKE} cmake diffutils valgrind"

${PKG_MAKE} --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[0]=1
fi

cmake --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[1]=1
fi

diff --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[2]=1
fi

valgrind --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    INSTALL_PKGS[3]=1
fi

INSTALL_Y="n"
for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
        DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
        if [ "${INSTALL_Y}" = "y" ]; then
            echo -e -n ", $(em "${DEP}")"
        else
            echo -e -n "install missing dependencies $(em "${DEP}")"
            INSTALL_Y="y"
        fi
    fi
done
if [ "${INSTALL_Y}" = "y" ]; then
    echo -e -n "? [y/n]: "
    read -p "" INSTALL_Y
fi

if [ "${INSTALL_Y}" = "y" ]; then
    DISTRO="FreeBSD"
    if [[ "${KERNEL_NAME}" != "FreeBSD"* ]]; then
        DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
    fi
    case "${DISTRO}" in
        "FreeBSD")
            sudo pkg update && {
                for i in $(seq 2 ${#INSTALL_PKGS[@]}); do
                    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
                        DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
                        sudo pkg install -y ${DEP}
                        INSTALL_PKGS[$((i-1))]=${?}
                    fi
                done
            }
            ;;
        "Debian GNU/Linux") ;&
        "Linux Mint") ;&
        "Ubuntu")
            sudo apt-get update && {
                for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
                    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
                        DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
                        sudo apt-get -y install ${DEP}
                        INSTALL_PKGS[$((i-1))]=${?}
                    fi
                done
            }
            ;;
        "openSUSE Leap") ;&
        "Rocky Linux")
            sudo dnf check-update
            for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
                if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
                    DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
                    sudo dnf -y install ${DEP}.x86_64
                    INSTALL_PKGS[$((i-1))]=${?}
                fi
            done
            ;;
        "Arch Linux") ;&
        "EndeavourOS")
            sudo pacman -Syy && {
                for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
                    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
                        DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
                        yes | sudo pacman -S ${DEP}
                        INSTALL_PKGS[$((i-1))]=${?}
                    fi
                done
            }
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

if [ "${INSTALL_Y}" = "y" ]; then
    for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
        if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
            DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
            log_warning "failed to install $(em "${DEP}")"
        fi
    done
fi
for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
        log_warning "install the missing dependencies before testing"
        break
    fi
done

exit 0
