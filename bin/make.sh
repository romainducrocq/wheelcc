#!/bin/bash

cd ../build/
./build.sh
if [ ${?} -ne 0 ]; then
    echo -e "\033[0;31merror:\033[0m build failed" 1>&2
    exit 1
fi

echo -e "build was successful, install with \033[1m‘./install.sh’\033[0m"
exit 0
