#!/bin/bash

PACKAGE_TEST="$(dirname $(readlink -f ${0}))"
PACKAGE_DIR="$(dirname ${PACKAGE_TEST})/bin"
PACKAGE_PATH="$(dirname ${PACKAGE_TEST})"
if [ -f "${PACKAGE_DIR}/pkgpath.cfg" ]; then
    PACKAGE_PATH="$(cat ${PACKAGE_DIR}/pkgpath.cfg)"
fi

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

ERRS_IN=$(sed "s|@PACKAGE_PATH@|${PACKAGE_PATH}|g" ${PACKAGE_TEST}/tools/print_errors.in)
ERRS_OUT=$(cd ${PACKAGE_TEST}/tools/; ./print-errors.sh; cd ${PACKAGE_TEST})

diff -q <(echo "${ERRS_IN}" | sort) <(echo "${ERRS_OUT}" | sort)
RETURN=${?}
if [ ${RETURN} -eq 0 ]; then
    echo -e "${LIGHT_GREEN}PASS: errors in and out are identical${NC}"

else
    diff <(echo "${ERRS_IN}" | sort) <(echo "${ERRS_OUT}" | sort)
    echo -e "${LIGHT_RED}FAIL: errors in and out differ${NC}"
    exit 1
fi

exit ${RETURN}
