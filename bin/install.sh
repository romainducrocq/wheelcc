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

function help () {
    echo "Usage: ./install.sh [--remove]"
    exit 0
}

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

REMOVE=0
if [ "${1}" = "--remove" ]; then
    REMOVE=1
elif [ ! -z "${1}" ]; then
   help
fi

RC_FILE=""
if [ -d "${PACKAGE_LIBC}/" ]; then
    if [ ! -z "$(ls ${PACKAGE_LIBC}/)" ]; then
        RC_FILE="${HOME}/.bashrc"
        if [[ "${KERNEL_NAME}" == "Darwin"* ]]; then
            RC_FILE="${HOME}/.zshrc"
        elif [[ "${KERNEL_NAME}" == "FreeBSD"* ]]; then
            RC_FILE="${HOME}/.shrc"
        fi
        if [ ! -f "${RC_FILE}" ]; then
            if [ -f "${HOME}/.bashrc" ]; then
                RC_FILE="${HOME}/.bashrc"
            elif [ -f "${HOME}/.zshrc" ]; then
                RC_FILE="${HOME}/.zshrc"
            elif [ -f "${HOME}/.shrc" ]; then
                RC_FILE="${HOME}/.shrc"
            elif [ -f "${HOME}/.tcshrc" ]; then
                RC_FILE="${HOME}/.tcshrc"
            elif [ -f "${HOME}/.cshrc" ]; then
                RC_FILE="${HOME}/.cshrc"
            elif [ -f "${HOME}/.kshrc" ]; then
                RC_FILE="${HOME}/.kshrc"
            fi
        fi
    else
        rm -r ${PACKAGE_LIBC}/
    fi
fi

MSG_RC=""
if [ ! -z "${RC_FILE}" ]; then
    MSG_RC="~/$(basename ${RC_FILE}) and "
fi
if [ ${REMOVE} -eq 1 ]; then
    echo -e -n "remove $(em "${PACKAGE_NAME}") from"
else
    echo -e -n "install $(em "${PACKAGE_NAME}") to"
fi
echo -e -n " ${MSG_RC}$(em "${INSTALL_DIR}/")? [y/n]: "
read -p "" INSTALL_Y
if [ ! "${INSTALL_Y}" = "y" ]; then
    if [ ${REMOVE} -eq 1 ]; then
        log_warning "system-wide $(em "${PACKAGE_NAME}") installation was not removed"
    else
        MSG_RC=""
        if [ -z "${RC_FILE}" ]; then
            MSG_RC=", use with $(em "./driver.sh")"
        fi
        log_warning "$(em "${PACKAGE_NAME}") was not installed system-wide${MSG_RC}"
    fi
    exit 0
fi

if [ ! -d "${INSTALL_DIR}/" ]; then
    sudo mkdir -p ${INSTALL_DIR}/
fi
sudo find ${INSTALL_DIR}/ -maxdepth 1 -name "${PACKAGE_NAME}" -type l -delete
if [ ${?} -ne 0 ]; then
    raise_error "failed to remove symlink from $(em "${INSTALL_DIR}/")"
fi

if [ ${REMOVE} -eq 1 ]; then
    if [ ! -z "${RC_FILE}" ]; then
        EXPR="/$(basename $(dirname ${PACKAGE_DIR}))"
        EXPR="${EXPR}/$(basename ${PACKAGE_DIR})"
        EXPR="${EXPR}/$(basename ${PACKAGE_LIBC})/"
        EXPR="LD_LIBRARY_PATH.*${EXPR//\/\//\/}"
        if [[ "${KERNEL_NAME}" == "Darwin"* ]]; then
            sed -i '' "\|${EXPR}|d" ${RC_FILE}
        elif [[ "${KERNEL_NAME}" == "FreeBSD"* ]]; then
            sed -i '' "\|${EXPR}|d" ${RC_FILE}
        else
            sed -i "\|${EXPR}|d" ${RC_FILE}
        fi
        if [ ${?} -ne 0 ]; then
            raise_error "failed to remove runtime library from ~/$(basename ${RC_FILE})"
        fi
    fi

    echo -e "system-wide $(em "${PACKAGE_NAME}") installation was successfully removed"
    exit 0
fi

sudo ln -s ${PACKAGE_DIR}/driver.sh ${INSTALL_LINK}
if [ ${?} -ne 0 ]; then
    raise_error "failed to create symlink in $(em "${INSTALL_DIR}/")"
fi
echo -e "created symlink ${LIGHT_BLUE}${INSTALL_LINK}${NC} -> ${LIGHT_GREEN}${PACKAGE_DIR}/driver.sh${NC}"

if [ ! -z "${RC_FILE}" ]; then
    if [[ "${RC_FILE}" == *"cshrc" ]]; then
        cat ${RC_FILE} | grep -q "setenv LD_LIBRARY_PATH"
        if [ ${?} -ne 0 ]; then
            echo 'if ( ! ${?LD_LIBRARY_PATH} ) then' >> ${RC_FILE}
            echo '    setenv LD_LIBRARY_PATH' >> ${RC_FILE}
            echo 'endif' >> ${RC_FILE}
        fi
        echo 'setenv LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:'"${PACKAGE_LIBC}/\"" >> ${RC_FILE}
    else
        echo 'export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:'"${PACKAGE_LIBC}/\"" >> ${RC_FILE}
    fi
    if [ ${?} -ne 0 ]; then
        raise_error "failed to install runtime library to ~/$(basename ${RC_FILE})"
    fi
    echo -e "exported $(em "${PACKAGE_LIBC}/") library path to ~/$(basename ${RC_FILE})"
fi

echo -e "installation was successful, see usage with command $(em "${PACKAGE_NAME} --help")"
exit 0
