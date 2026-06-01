#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-cmake-build}"
INPUT="${1:-examples/case_study}"

if [[ ! -x "$BUILD_DIR/flang-modernizer" ]]; then
  ./build.sh
fi

"$BUILD_DIR/flang-modernizer" "$INPUT"
