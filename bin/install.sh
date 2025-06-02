#!/bin/bash

PACKAGE_NAME="$(cat ./package_name.txt)"

INSTALL_Y=""
INSTALL_DIR="/usr/local/bin"

echo -e -n "install \033[1m‘${PACKAGE_NAME}’\033[0m to \033[1m‘${INSTALL_DIR}/’\033[0m? [y/n]: "
read -p "" INSTALL_Y
if [ ! ${INSTALL_Y} = "y" ]; then
    echo -e "\033[1;34mwarning:\033[0m command \033[1m‘${PACKAGE_NAME}’\033[0m was not installed, use with \033[1m‘${PWD}/driver.sh’\033[0m"
    exit 0
fi

if [ ! -d "${INSTALL_DIR}" ]; then
    sudo mkdir -p ${INSTALL_DIR}/
fi
sudo find ${INSTALL_DIR}/ -maxdepth 1 -name "${PACKAGE_NAME}" -type l -delete
if [ ${?} -ne 0 ]; then
    echo -e "\033[0;31merror:\033[0m installation failed" 1>&2
    exit 1
fi

sudo ln -s ${PWD}/driver.sh ${INSTALL_DIR}/${PACKAGE_NAME}
if [ ${?} -ne 0 ]; then
    echo -e "\033[0;31merror:\033[0m installation failed" 1>&2
    exit 1
fi
echo -e "created symlink \033[1;36m${INSTALL_DIR}/${PACKAGE_NAME}\033[0m -> \033[1;32m${PWD}/driver.sh\033[0m"

echo -e "installation was successful, use with command \033[1m‘${PACKAGE_NAME}’\033[0m"
exit 0
