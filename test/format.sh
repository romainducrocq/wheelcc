#!/bin/bash

FILE="$(readlink -f ${1})"
cp ${FILE} .

FILE="$(basename ${FILE})"
vi ${FILE} +'e ++ff=dos | set ff=unix | wq!'
clang-format ${FILE} -style=file > format.out.1
rm ${FILE}
