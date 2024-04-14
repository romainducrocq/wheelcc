#!/bin/bash

PACKAGE_NAME="wheelcc"

INSTALL=0
dpkg -s gcc | grep Status | grep -q "install ok installed"
if [ ${?} -ne 0 ]; then
    INSTALL=1
fi
dpkg -s g++ | grep Status | grep -q "install ok installed"
if [ ${?} -ne 0 ]; then
    INSTALL=1
fi
dpkg -s cmake | grep Status | grep -q "install ok installed"
if [ ${?} -ne 0 ]; then
    INSTALL=1
fi
if [ ${?} -ne 0 ]; then
    sudo apt-get update
    sudo apt-get -y upgrade
    sudo apt-get -y install \
        gcc \
        g++ \
        cmake
fi

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
