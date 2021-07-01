#!/bin/bash
set -euo pipefail

trap exit INT

cd "$(dirname "$0")"

build() {
    # Build engine
    cd ../engine
    mkdir -p build
    cd build
    cmake .. "-DCMAKE_BUILD_TYPE=$1" -G Ninja
    ninja || exit 1

    # Build sandbox
    cd ../../sandbox
    mkdir -p build
    cd build
    [[ -f sandbox ]] && rm sandbox
    cmake .. "-DCMAKE_BUILD_TYPE=$1" -G Ninja
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
        rm -rf ../engine/build
        rm -rf build
        ;;
    *)
        echo "Invalid argument $1. Valid arguments are run build and clean"
        ;;
esac
