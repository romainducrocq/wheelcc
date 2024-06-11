#!/bin/bash

FILE="$(readlink -f ${1})"

vi ${FILE} +'e ++ff=dos | set ff=unix | wq!'
clang-format ${FILE} -style=file > format.out.1
