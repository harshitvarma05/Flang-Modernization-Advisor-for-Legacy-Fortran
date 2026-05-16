# Flang Modernization Advisor for Legacy Fortran

This project is a C++17 Flang-oriented static analysis tool that detects legacy Fortran anti-patterns, estimates modernization impact, and produces a prioritized modernization plan.

The main advisor uses a C++ source-fact layer plus Flang-generated parse-tree and semantic-symbol metadata. It also includes an optional in-process Flang probe that links against the installed Flang parser and semantic-analysis libraries to demonstrate direct compiler API integration.

## Features

- Detects arithmetic `IF`, computed `GOTO`, `EQUIVALENCE`, `COMMON`, implicit typing, fixed-form source, statement functions, assumed-size arrays, and `ENTRY`.
- Computes cross-file impact for shared `COMMON` blocks.
- Records dependent constructs such as exact branch labels, storage association, dummy argument interfaces, and call graph risks.
- Assigns effort ratings: `trivial`, `moderate`, `complex`.
- Assigns safety ratings: `safe`, `review-needed`, `risky`.
- Produces Markdown or JSON reports.
- Includes a conservative fixed-form cleanup transformer for validating safe recommendations.
- Consumes Flang parse-tree and symbol dumps to attach compiler-produced evidence to findings.
- Provides an optional in-process Flang parser/semantic probe target.

## Run

```bash
make
./build/flang-modernizer examples/legacy
```

Generate the case-study report:

```bash
make report
```

Run safe demonstration transformations:

```bash
make transform
```

Run tests:

```bash
make test
```

Validate the Fortran fixtures with real Flang, write parse-tree dumps, and run the in-process Flang probe:

```bash
make flang-validate
make flang-dump-tree
make flang-inprocess
```


## Local Web Dashboard

Start the browser-based local UI:

```bash
make
./build/flang-modernizer --serve
```

Then open:

```text
http://localhost:8080
```

The dashboard lets you enter a Fortran file or folder path and run:

- Analyze
- Validate with Flang
- Generate Flang parse-tree dumps
- Run safe transformations

For CMake-based IDEs, build the `serve` target to start the same dashboard.

## Assignment-Strict In-Memory Flang AST Core

A separate implementation lives in `flang_ast_advisor/`. This is the version to emphasize when a professor asks whether the project truly uses Flang parse-tree and semantic analysis. It links against Flang libraries, parses Fortran in process, visits typed Flang AST nodes with `Fortran::parser::Walk`, and reads `semantics::Scope` / `Symbol` data for COMMON, EQUIVALENCE, and implicit typing impact.

Useful commands:

```bash
make ast-advisor
make ast-test
make ast-report
make ast-real-report
```

CMake IDEs also see the subproject through `add_subdirectory(flang_ast_advisor)`.

## Flang Integration Design

The advisor now uses Flang in three ways:

1. `flang -fsyntax-only` validates analyzed Fortran inputs.
2. `FlangMetadataProvider` consumes `-fdebug-dump-parse-tree` and `-fdebug-dump-symbols` output and attaches Flang parse-tree / semantic-symbol evidence to findings.
3. `flang-inprocess-probe` links against Homebrew Flang libraries (`FortranParser`, `FortranSemantics`, `FortranEvaluate`, and support libraries) and runs Flang parsing plus semantic analysis in process.

The main modernization pipeline remains separated into source facts, impact analysis, prioritization, reporting, and transformations. This keeps the project demonstrable while still showing real Flang parse-tree and semantic-analysis integration.

## Deliverables Mapping

| Required deliverable | Project location |
|---|---|
| Pattern detector | `src/Analyzer.cpp`, `include/Analyzer.hpp` |
| Impact analyzer | `ModernizationAnalyzer::computeImpact`, `ModernizationAnalyzer::enrichWithFlangMetadata` |
| Prioritized plan | `src/Reporter.cpp`, `docs/scoring_model.md` |
| Legacy tests | `examples/legacy`, `tests/test_analyzer.cpp` |
| Demo case study | `examples/case_study`, `docs/case_study_report.md` |
| Real legacy case study | `examples/real_case_study/minpack`, `docs/real_case_study_report.md` |