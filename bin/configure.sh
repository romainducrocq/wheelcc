#!/bin/bash

PACKAGE_NAME="wheelcc"

function get_install () {
    PKG="${1}"
    dpkg -s ${PKG} | grep Status | grep -q "install ok installed"
    if [ ${?} -ne 0 ]; then
        if [ ${GET_UPDATE} -eq 0 ]; then
            sudo apt-get update
            sudo apt-get -y upgrade
            GET_UPDATE=1
        fi
        sudo apt-get -y install ${PKG}
    fi
}

GET_UPDATE=0
get_install gcc
get_install g++
get_install cmake

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
