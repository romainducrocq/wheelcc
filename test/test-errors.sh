#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"
PACKAGE_PATH="$(readlink -f ..)"

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

TOOLS_DIR="${PWD}/tools/"

ERRS_IN=$(sed "s|@PACKAGE_PATH@|${PACKAGE_PATH}|g" ${TOOLS_DIR}/print_errors.in)
ERRS_OUT=$(cd ${TOOLS_DIR}; ./print-errors.sh; cd ../)

diff -q <(echo "${ERRS_IN}") <(echo "${ERRS_OUT}")
RETURN=${?}
if [ ${RETURN} -eq 0 ]; then
    echo -e "${LIGHT_GREEN}PASS: errors in and out are identical${NC}"

else
    diff <(echo "${ERRS_IN}") <(echo "${ERRS_OUT}")
    echo -e "${LIGHT_RED}FAIL: errors in and out differ${NC}"
    exit 1
fi

exit ${RETURN}
