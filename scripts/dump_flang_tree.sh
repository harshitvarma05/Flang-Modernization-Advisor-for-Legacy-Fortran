#!/bin/sh
set -eu
FLANG_BIN="${1:-flang}"
mkdir -p docs/flang_dumps
"$FLANG_BIN" -fc1 -fdebug-dump-parse-tree examples/legacy/all_patterns.f > docs/flang_dumps/legacy_parse_tree.txt
"$FLANG_BIN" -fc1 -fdebug-dump-symbols examples/legacy/all_patterns.f > docs/flang_dumps/legacy_symbols.txt
"$FLANG_BIN" -fc1 -fdebug-dump-parse-tree examples/case_study/solver.f > docs/flang_dumps/solver_parse_tree.txt
"$FLANG_BIN" -fc1 -fdebug-dump-parse-tree examples/case_study/physics.f > docs/flang_dumps/physics_parse_tree.txt
echo "Wrote Flang dumps to docs/flang_dumps"
