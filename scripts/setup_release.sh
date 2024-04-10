#! /bin/bash

cd ..
mkdir --parents build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
