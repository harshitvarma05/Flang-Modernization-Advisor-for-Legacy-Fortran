#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-cmake-build}"
DETECTED_ARGS="$(scripts/detect_llvm.sh --cmake)"

# shellcheck disable=SC2086
cmake -S . -B "$BUILD_DIR" $DETECTED_ARGS
cmake --build "$BUILD_DIR"
ctest --test-dir "$BUILD_DIR" --output-on-failure
