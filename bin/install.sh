#!/bin/bash

PACKAGE_NAME="$(cat ./package_name.txt)"

sudo find /usr/local/bin/ -maxdepth 1 -name "${PACKAGE_NAME}" -type l -delete
sudo ln -s $(pwd)/driver.sh /usr/local/bin/${PACKAGE_NAME}

exit 0
