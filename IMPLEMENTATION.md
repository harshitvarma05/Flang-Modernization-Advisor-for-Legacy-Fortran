# Implementation

## LLVM/Flang Integration

The analyzer links against LLVM Flang libraries:

- `FortranParser`
- `FortranSemantics`
- `FortranEvaluate`
- `FortranSupport`
- `FortranDecimal`
- LLVM support library

CMake accepts explicit `FLANG_ROOT` and `LLVM_ROOT` values and also searches common macOS/Linux install locations. The helper script `scripts/detect_llvm.sh` detects Homebrew LLVM/Flang on macOS, `llvm-config` installations, and common Linux paths such as `/usr/lib/llvm-*`.

## Parse-Tree Detection

`FlangAstAdvisor.cpp` uses `Fortran::parser::Walk` with typed visitors for required constructs:

- `parser::ArithmeticIfStmt`
- `parser::ComputedGotoStmt`
- `parser::CommonStmt`
- `parser::EquivalenceStmt`
- `parser::StmtFunctionStmt`
- `parser::EntryStmt`
- `parser::AssumedImpliedSpec`

Fixed-form detection comes from the configured Flang parser options/source form.

## Semantic Impact Analysis

After parsing, the tool runs `semantics::Semantics` and extracts semantic facts from Flang scopes and symbols:

- `Scope::commonBlocks()` and `CommonBlockDetails::objects()` for COMMON layout, offsets, and sizes
- `Scope::equivalenceSets()` for true storage association / alias classes
- `Symbol::Flag::Implicit` for symbols created by implicit typing

A project-level enrichment pass then computes:

- affected files
- COMMON declaration/layout map
- cross-file migration scope
- alias class size and array-element overlay risk
- direct call-site impact for assumed-size arrays and ENTRY points
- declaration worklists for implicit typing modernization

## Priority Model

Findings are scored by pattern weight, affected-file count, dependency count, behavior-risk count, effort, and safety. The UI sorts by priority and displays the numeric score beside each recommendation.

## Outputs

- CLI Markdown report
- JSON report
- Local web dashboard
- Flang syntax validation
- Flang parse-tree/symbol dumps
- transformed outputs for safe transformations
