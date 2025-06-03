#!/bin/bash

if [[ "$(uname -s)" = "Darwin"* ]]; then
    exit 0
fi

INSTALL_PKGS=(0 0 0 0)
INSTALL_DEPS="make cmake diffutils valgrind"

make --help > /dev/null 2>&1
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
            echo -e -n ", \033[1m‘${DEP}’\033[0m"
        else
            echo -e -n "install missing dependencies \033[1m‘${DEP}’\033[0m"
            INSTALL_Y="y"
        fi
    fi
done
if [ "${INSTALL_Y}" = "y" ]; then
    echo -e -n "? [y/n]: "
    read -p "" INSTALL_Y
fi

if [ "${INSTALL_Y}" = "y" ]; then
    DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
    case "${DISTRO}" in
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
            sudo dnf check-update && {
                for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
                    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
                        DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
                        sudo dnf -y install ${DEP}.x86_64
                        INSTALL_PKGS[$((i-1))]=${?}
                    fi
                done
            }
            ;;
        "Arch Linux") ;&
        "EndeavourOS")
            sudo pacman -Syy && {
                for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
                    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
                        DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
                        sudo pacman -Syu ${DEP}
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
            echo -e "\033[1;34mwarning:\033[0m this distribution was not tested yet, use at your own risk!"
            INSTALL_Y="n"
    esac
fi

if [ "${INSTALL_Y}" = "y" ]; then
    for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
        if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
            DEP="$(echo "${INSTALL_DEPS}" | cut -d" " -f${i})"
            echo -e "\033[1;34mwarning:\033[0m failed to install \033[1m‘${DEP}’\033[0m"
        fi
    done
fi
for i in $(seq 1 ${#INSTALL_PKGS[@]}); do
    if [ ${INSTALL_PKGS[$((i-1))]} -ne 0 ]; then
        echo -e "\033[1;34mwarning:\033[0m install the missing dependencies before testing"
        break
    fi
done

exit 0
