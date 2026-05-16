# Flang Modernization Advisor for Legacy Fortran

A C++17 static analysis tool for legacy Fortran modernization. The analyzer embeds LLVM Flang, builds Flang's parse tree in memory, runs Flang semantic analysis, and produces a prioritized modernization plan with effort and safety ratings.

## What It Detects

- Arithmetic `IF`
- Computed `GOTO`
- `EQUIVALENCE`
- `COMMON` blocks
- Implicit typing
- Fixed-form source
- Statement functions
- Assumed-size arrays
- `ENTRY` statements

## Project Structure

| Path | Purpose |
|---|---|
| `flang_ast_advisor/` | Main assignment analyzer: in-memory Flang AST visitor and semantic impact analyzer |
| `src/main.cpp` | CLI entry point and web-dashboard launcher |
| `src/WebServer.cpp` | Minimal local web UI that calls the AST analyzer |
| `src/Transform.cpp` | Conservative safe transformations used for validation/demo output |
| `src/InProcessFlangProbe.cpp` | Small Flang parser/semantic probe for viva/debugging |
| `examples/legacy/` | Fixture covering required patterns and false positives |
| `examples/case_study/` | Small demonstration case study |
| `examples/real_case_study/minpack/` | Real public Netlib MINPACK subset |
| `docs/` | Reports, scoring notes, presentation notes, Flang dumps |
| `transformed/` | Safe transformation outputs |

There is one official build entry point: the root `Makefile` and root `CMakeLists.txt`.

## Build And Run

```bash
make
./build/flang-modernizer examples/legacy/all_patterns.f
```

Generate reports:

```bash
make report
make real-report
make ast-report
make ast-real-report
```

Run tests:

```bash
make test
```

CMake flow for IDEs:

```bash
cmake -S . -B cmake-build
cmake --build cmake-build
ctest --test-dir cmake-build --output-on-failure
```

## Local Web Dashboard

```bash
make
./build/flang-modernizer --serve
```

Open:

```text
http://localhost:8080
```

The dashboard has one analysis button, **Analyze**, and it uses the in-memory Flang AST analyzer. The other buttons provide Flang validation, parse-tree dumps, and conservative safe transformations.

## How The Flang Analyzer Works

The main analyzer is in `flang_ast_advisor/src/FlangAstAdvisor.cpp`.

Pipeline:

```text
Fortran source
  -> Flang prescan
  -> Flang parse tree in memory
  -> typed AST visitor with Fortran::parser::Walk
  -> Flang semantic analysis
  -> semantic Scope/Symbol impact extraction
  -> priority scoring
  -> Markdown/JSON modernization report
```

Examples of typed AST nodes used:

- `parser::ArithmeticIfStmt`
- `parser::ComputedGotoStmt`
- `parser::CommonStmt`
- `parser::EquivalenceStmt`
- `parser::StmtFunctionStmt`
- `parser::EntryStmt`

Examples of semantic data used:

- `Scope::commonBlocks()`
- `CommonBlockDetails::objects()`
- `Scope::equivalenceSets()`
- `Symbol::Flag::Implicit`

## Assignment Deliverables Mapping

| Required deliverable | Project location |
|---|---|
| Pattern detector | `flang_ast_advisor/src/FlangAstAdvisor.cpp` typed Flang AST visitor |
| Impact analyzer | `collectSemanticScopeEvidence(...)` and cross-file COMMON consolidation in `FlangAstAdvisor.cpp` |
| Prioritized plan | `flang_ast_advisor/src/Findings.cpp` and generated Markdown/JSON reports |
| Legacy test suite | `examples/legacy/`, `flang_ast_advisor/tests/test_flang_ast_advisor.cpp` |
| Real case study | `examples/real_case_study/minpack/`, `docs/real_case_study_report.md`, `flang_ast_advisor/docs/ast_real_case_study_report.md` |
| Three safe transformations | `src/Transform.cpp`, `transformed/case_study/`, `transformed/real_case_study/` |
