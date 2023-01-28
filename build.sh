#!/bin/bash
set -euo pipefail

trap exit INT

cd "$(dirname "$0")"

build() {
    local build_target="$1"
    local build_type="$2"
    if [ -z "$build_target" ] || [ -z "$build_type" ]; then
        echo "Invalid target or build type"
        exit 1
    fi

    mkdir -p build
    local build_directory="build/$build_type"

    cmake -B "$build_directory" "-DCMAKE_BUILD_TYPE=$build_type" -G Ninja
    cmake --build "$build_directory" --target "$build_target" || exit 1

    # Copy compile_commands.json
    if [[ "$build_type" == "Debug" ]]; then
        [[ -f "$build_directory/compile_commands.json" ]] && cp "$build_directory/compile_commands.json" compile_commands.json
    fi
}

if [[ ! -v 1 ]]; then
    build Release
    exit
fi

case $1 in
    run)
        build Birdy3d_sandbox Debug && ./build/Debug/out/bin/Birdy3d_sandbox
        ;;
    build)
        build Birdy3d_sandbox Debug
        ;;
    release)
        build Birdy3d_sandbox Release
        ;;
    test)
        build test Debug && ./build/Debug/out/bin/test
        ;;
    clean)
        rm -rf build
        ;;
    *)
        echo "Invalid argument $1. Valid arguments are run build and clean"
        ;;
esac
