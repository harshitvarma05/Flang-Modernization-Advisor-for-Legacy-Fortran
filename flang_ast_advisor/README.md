# In-Memory Flang AST Advisor

This folder contains the main analysis engine used by both the CLI and the web dashboard. It embeds Flang directly as a library. It prescans, parses, and semantically analyzes Fortran
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
- assumed-size arrays: Flang assumed-implied `*` array-spec nodes
- ENTRY statements: `parser::EntryStmt`

## Build

```bash
cmake -S . -B cmake-build
cmake --build cmake-build --target flang-ast-advisor
ctest --test-dir cmake-build --output-on-failure
```

## Run

```bash
./cmake-build/flang-ast-advisor examples/case_study
./cmake-build/flang-ast-advisor examples/real_case_study/minpack \
  --output flang_ast_advisor/docs/ast_real_case_study_report.md \
  --json flang_ast_advisor/docs/ast_real_case_study_report.json
```

## How This Aligns With The Assignment

This implementation is Flang-based in the literal sense: the tool does not consume
text dumps as its frontend. It holds Flang's parse tree in memory, visits typed
parse-tree nodes, runs Flang semantic analysis, and reads semantic scopes/symbols
to estimate impact.

The repository root provides the CLI, web dashboard, build files, and conservative transformation demo. They all use this Flang AST analyzer as the analysis core.
