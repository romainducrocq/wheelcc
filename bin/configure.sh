#!/bin/bash

PACKAGE_NAME="wheelcc"

echo -e "GET http://google.com HTTP/1.0\n\n" | nc google.com 80 > /dev/null 2>&1
if [ $? -eq 0 ]; then
    sudo apt-get update
    apt-get install gcc \
        g++ \
        cmake
fi

echo -n "${PACKAGE_NAME}" > ./package_name.txt
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
