#!/bin/bash

cmake -G "Unix Makefiles" -S . -B out/ -DCMAKE_BUILD_TYPE=Release
cd out/
make
