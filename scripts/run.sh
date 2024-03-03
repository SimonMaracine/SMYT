#! /bin/bash

./build.sh

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ../build/smyt
./smyt ${@:1}
