#!/bin/bash

FILE="$(readlink -f ${1})"

while read LINE; do
    REPLACE=$(echo "${LINE}" | cut -d"+" -f1)
    EXPR=$(echo "${REPLACE}" | tr -d '@')
    sed -i "s/${EXPR}/${REPLACE}/g" ${FILE}
done < <(cat "${2}")
