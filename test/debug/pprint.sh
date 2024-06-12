#!/bin/bash

OUT=$(./ast.sh --assert)
if [ ! -z "${OUT}" ]; then
    exit 1
fi

./ast.sh
python3 pprint.py > ../../src/util/pprint.cpp
cd ../
./format.sh ../src/util/pprint.cpp

exit 0
