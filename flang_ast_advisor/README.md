# In-Memory Flang AST Advisor

This folder is the assignment-strict implementation path.

Unlike the dashboard-oriented advisor in the repository root, this executable embeds
Flang directly as a library. It prescans, parses, and semantically analyzes Fortran
source in the same process, then visits Flang's in-memory parse tree with
`Fortran::parser::Walk`.

## What It Uses From Flang

- `Fortran::parser::Parsing` for prescan and parse
- `Fortran::parser::Program` parse tree held in memory
- `Fortran::parser::Walk` with typed `Pre(...)` handlers for legacy constructs
- `Fortran::semantics::Semantics` for semantic analysis
- `Fortran::semantics::Scope` and `Symbol` tables for COMMON layout,
  EQUIVALENCE sets, and implicit symbols

## Patterns Covered

- arithmetic IF: `parser::ArithmeticIfStmt`
- computed GOTO: `parser::ComputedGotoStmt`
- EQUIVALENCE: `parser::EquivalenceStmt` plus semantic equivalence sets
- COMMON: `parser::CommonStmt` plus semantic COMMON block objects and offsets
- implicit typing: `parser::ImplicitStmt` plus semantic implicit symbol flags
- fixed-form source: Flang parser options used for prescan
- statement functions: `parser::StmtFunctionStmt`
- assumed-size arrays: `parser::AssumedSizeSpec`
- ENTRY statements: `parser::EntryStmt`

## Build

```bash
make ast-advisor
make ast-test
```

Or with CMake:

```bash
cmake -S . -B cmake-build
cmake --build cmake-build --target flang-ast-advisor
ctest --test-dir cmake-build --output-on-failure
```

## Run

```bash
./build/flang-ast-advisor examples/case_study
./build/flang-ast-advisor examples/real_case_study/minpack \
  --output flang_ast_advisor/docs/ast_real_case_study_report.md \
  --json flang_ast_advisor/docs/ast_real_case_study_report.json
```

## How This Aligns With The Assignment

This implementation is Flang-based in the literal sense: the tool does not consume
text dumps as its frontend. It holds Flang's parse tree in memory, visits typed
parse-tree nodes, runs Flang semantic analysis, and reads semantic scopes/symbols
to estimate impact.

The repository root still contains the nicer local web dashboard and conservative
transformation demo. For viva, present this folder as the compiler-frontend core
and the root dashboard as the user-facing interface around the same project idea.
