#!/usr/bin/env bash

KERNEL_NAME="$(uname -s)"
PACKAGE_DIR="$(dirname $(readlink -f ${0}))"
PACKAGE_NAME="$(cat ${PACKAGE_DIR}/pkgname.cfg)"
PACKAGE_LIBC="${PACKAGE_DIR}/libc"

LIGHT_BLUE='\033[1;36m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

INSTALL_Y=""
INSTALL_DIR="/usr/local/bin"
INSTALL_LINK="${INSTALL_DIR}/${PACKAGE_NAME}"

function em () {
    echo "\033[1m‘${1}’\033[0m"
}

function log_warning () {
    WARN_MESSAGE="${1}"
    echo -e "\033[1;34mwarning:\033[0m ${WARN_MESSAGE}"
}

function raise_error () {
    ERROR_MESSAGE="${1}"
    echo -e "install: \033[0;31merror:\033[0m ${ERROR_MESSAGE}" 1>&2
    exit 1
}

RC_FILE=""
if [ -d "${PACKAGE_LIBC}/" ]; then
    if [ ! -z "$(ls ${PACKAGE_LIBC}/)" ]; then
        RC_FILE="bashrc"
        if [[ "${KERNEL_NAME}" == "Darwin"* ]]; then
            RC_FILE="zshrc"
        elif [[ "${KERNEL_NAME}" == "FreeBSD"* ]]; then
            RC_FILE="shrc"
        fi
        if [ ! -f "${HOME}/.${RC_FILE}" ]; then
            if [ -f "${HOME}/.bashrc" ]; then
                RC_FILE="bashrc"
            elif [ -f "${HOME}/.zshrc" ]; then
                RC_FILE="zshrc"
            elif [ -f "${HOME}/.shrc" ]; then
                RC_FILE="shrc"
            elif [ -f "${HOME}/.tcshrc" ]; then
                RC_FILE="tcshrc"
            elif [ -f "${HOME}/.cshrc" ]; then
                RC_FILE="cshrc"
            elif [ -f "${HOME}/.kshrc" ]; then
                RC_FILE="kshrc"
            fi
        fi
    else
        rm -r ${PACKAGE_LIBC}/
    fi
fi

MSG_RC=""
if [ ! -z "${RC_FILE}" ]; then
    MSG_RC="~/.${RC_FILE} and "
fi
echo -e -n "install $(em "${PACKAGE_NAME}") to ${MSG_RC}$(em "${INSTALL_DIR}/")? [y/n]: "
read -p "" INSTALL_Y
if [ ! "${INSTALL_Y}" = "y" ]; then
    MSG_RC=""
    if [ -z "${RC_FILE}" ]; then
        MSG_RC=", use with $(em "${PACKAGE_DIR}/driver.sh")"
    fi
    log_warning "command $(em "${PACKAGE_NAME}") was not installed${MSG_RC}"
    exit 0
fi

if [ ! -d "${INSTALL_DIR}/" ]; then
    sudo mkdir -p ${INSTALL_DIR}/
fi
sudo find ${INSTALL_DIR}/ -maxdepth 1 -name "${PACKAGE_NAME}" -type l -delete
if [ ${?} -ne 0 ]; then
    raise_error "failed to remove existing installation from $(em "${INSTALL_DIR}/")"
fi

sudo ln -s ${PACKAGE_DIR}/driver.sh ${INSTALL_LINK}
if [ ${?} -ne 0 ]; then
    raise_error "failed to create symlink in $(em "${INSTALL_DIR}/")"
fi
echo -e "created symlink ${LIGHT_BLUE}${INSTALL_LINK}${NC} -> ${LIGHT_GREEN}${PACKAGE_DIR}/driver.sh${NC}"

if [ ! -z "${RC_FILE}" ]; then
    if [[ "${RC_FILE}" == *"cshrc" ]]; then
        sudo echo 'if ( ! ${?LD_LIBRARY_PATH} ) then; setenv LD_LIBRARY_PATH; endif' >> ~/.${RC_FILE}
        sudo echo 'setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:'"${PACKAGE_LIBC}/\"" >> ~/.${RC_FILE}
    else
        sudo echo 'export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:'"${PACKAGE_LIBC}/\"" >> ~/.${RC_FILE}
    fi
    if [ ${?} -ne 0 ]; then
        raise_error "failed to install runtime to ~/.${RC_FILE}"
    fi
    echo -e "exported $(em "${PACKAGE_LIBC}/") library path to ~/.${RC_FILE}"
fi

echo -e "installation was successful, see usage with command $(em "${PACKAGE_NAME} --help")"
exit 0
