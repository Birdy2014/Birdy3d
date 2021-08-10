#!/bin/bash
set -euo pipefail

trap exit INT

cd "$(dirname "$0")"

build() {
    if [ ! -d build ]; then
        mkdir build
        cd build
        cmake .. "-DCMAKE_BUILD_TYPE=$1" -G Ninja
    else
        cd build
        [[ -f sandbox ]] && rm sandbox
    fi
    ninja || exit 1

    # Return to starting directory
    cd ..
}

if [[ ! -v 1 ]]; then
    build Release
    exit
fi

case $1 in
    run)
        build Debug && ./build/out/bin/sandbox
        ;;
    build)
        build Debug
        ;;
    release)
        build Release
        ;;
    clean)
        rm -rf build
        ;;
    *)
        echo "Invalid argument $1. Valid arguments are run build and clean"
        ;;
esac
