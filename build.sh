#!/bin/bash
set -euo pipefail

trap exit INT

cd "$(dirname "$0")"

build() {
    cmake -B build "-DCMAKE_BUILD_TYPE=$1" -G Ninja
    cmake --build build || exit 1

    # Copy compile_commands.json
    [[ -f build/compile_commands.json ]] && cp build/compile_commands.json compile_commands.json
}

if [[ ! -v 1 ]]; then
    build Release
    exit
fi

case $1 in
    run)
        build Debug && ./build/out/bin/Birdy3d_sandbox
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
