#!/bin/bash

PACKAGE_NAME="ccc2"

sudo apt-get update
apt-get install gcc \
    cmake

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
