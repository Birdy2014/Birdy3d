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
        [[ -f Birdy3d_sandbox ]] && rm Birdy3d_sandbox
    fi
    ninja Birdy3d_sandbox || exit 1

    # Return to starting directory
    cd ..

    # Copy compile_commands.json
    [[ -f build/compile_commands.json ]] && cp build/compile_commands.json compile_commands.json
}

if [[ ! -v 1 ]]; then
    build Release
    exit
fi

case $1 in
    run)
        build DEBUG && ./build/out/bin/Birdy3d_sandbox
        ;;
    build)
        build DEBUG
        ;;
    release)
        build RELEASE
        ;;
    clean)
        rm -rf build
        ;;
    *)
        echo "Invalid argument $1. Valid arguments are run build and clean"
        ;;
esac
