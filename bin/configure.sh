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
    # Other
    *)
        echo "install the following dependencies with your preferred package manager:"
        echo "  - gcc/g++ (>=8)"
        echo "  - cmake (>=3.5)"
esac

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
