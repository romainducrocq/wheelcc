#!/bin/bash

FILE="$(readlink -f ${1})"

while read LINE; do
    EXPR=$(echo "${LINE}" | cut -d" " -f1)
    # REPLACE=$(echo "${LINE}" | cut -d" " -f2)
    sed -i "s/${EXPR}/${EXPR}@@/g" ${FILE}
done < <(cat "${2}")
