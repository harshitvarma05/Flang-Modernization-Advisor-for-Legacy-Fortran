# Flang Modernization Advisor for Legacy Fortran

This project is a compiler-design lab prototype for a Flang-style static analysis tool that detects legacy Fortran anti-patterns, estimates modernization impact, and produces a prioritized modernization plan.

The main implementation is now C++17, matching LLVM/Flang's native ecosystem. It uses a lightweight Fortran front-end in this lab build so it can run without a local LLVM/Flang installation, while keeping the detector, semantic facts, impact analysis, and reporting separated so a Flang parse-tree adapter can replace the scanner.

## Features

- Detects arithmetic `IF`, computed `GOTO`, `EQUIVALENCE`, `COMMON`, implicit typing, fixed-form source, statement functions, assumed-size arrays, and `ENTRY`.
- Computes cross-file impact for shared `COMMON` blocks.
- Records dependent constructs such as labels, storage association, dummy argument interfaces, and call graph risks.
- Assigns effort ratings: `trivial`, `moderate`, `complex`.
- Assigns safety ratings: `safe`, `review-needed`, `risky`.
- Produces Markdown or JSON reports.
- Includes a conservative fixed-form cleanup transformer for validating safe recommendations.

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

## Flang Integration Design

The lab prototype currently builds `SourceUnit`, `Routine`, and `Finding` facts from source text. In a full Flang integration:

1. Use Flang parsing to obtain source form, labels, parse-tree nodes, and program units.
2. Use semantic analysis to resolve symbols, dummy arguments, common block objects, procedure calls, and storage association.
3. Populate the existing `SourceUnit` and `Routine` model from Flang semantic facts.
4. Reuse the impact analyzer and report generator unchanged.

This separation matters because pattern matching alone can find a `COMMON` statement, but semantic facts are needed to say which program units and files depend on that storage layout.

## Deliverables Mapping

| Required deliverable | Project location |
|---|---|
| Pattern detector | `src/Analyzer.cpp`, `include/Analyzer.hpp` |
| Impact analyzer | `ModernizationAnalyzer::computeImpact` in `src/Analyzer.cpp` |
| Prioritized plan | `src/Reporter.cpp` |
| Legacy tests | `examples/legacy`, `tests/test_analyzer.cpp` |
| Case study | `examples/case_study`, `docs/case_study_report.md` after running the command |