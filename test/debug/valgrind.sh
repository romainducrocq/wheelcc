#!/bin/bash

PACKAGE_NAME="$(cat ../../bin/package_name.txt)"

LOG_DIR="${PWD}/../tmp/valgrind"
mkdir -p ${LOG_DIR}

LOG_FILE="out_$(date +%Y%m%d_%H%M%S)"

cd ../bin/
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=${LOG_DIR}/${LOG_FILE} \
         ./${PACKAGE_NAME} ${@}

cat ${LOG_DIR}/${LOG_FILE}
