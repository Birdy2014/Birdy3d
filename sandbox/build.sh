#!/bin/bash
set -euo pipefail

trap exit INT

cd "$(dirname "$0")"

build() {
    # Build engine
    cd ../engine
    mkdir -p build
    cd build
    cmake .. -G Ninja
    ninja

    # Build sandbox
    cd ../../sandbox
    mkdir -p build
    cd build
    cmake .. -G Ninja
    ninja

    # Return to starting directory
    cd ..
}

if [[ ! -v 1 ]]; then
    build
    exit
fi

case $1 in
    run)
        build && ./build/sandbox
        ;;
    build)
        build
        ;;
    clean)
        rm -rf ../engine/build
        rm -rf build
        ;;
    *)
        echo "Invalid argument $1. Valid arguments are run build and clean"
        ;;
esac
