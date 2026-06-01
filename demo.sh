#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-cmake-build}"
if [[ ! -x "$BUILD_DIR/flang-modernizer" ]]; then
  ./build.sh
fi

cmake --build "$BUILD_DIR" --target report real-report ast-report ast-real-report real-transform run-minpack-driver
