# Flang Modernization Advisor for Legacy Fortran

This project is a compiler-design lab prototype for a Flang-style static analysis tool that detects legacy Fortran anti-patterns, estimates modernization impact, and produces a prioritized modernization plan.

The runnable prototype uses a lightweight Fortran front-end so it works without a local LLVM/Flang build. The architecture keeps the detector, semantic facts, impact analysis, and reporting separate so the front-end can be replaced with Flang parse-tree and semantic-symbol extraction.

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
python3 -m flang_modernizer.cli examples/legacy
```

Generate the case-study report:

```bash
python3 -m flang_modernizer.cli examples/case_study --output docs/case_study_report.md
```

Run safe demonstration transformations:

```bash
python3 -m flang_modernizer.cli examples/case_study --safe-transform-out transformed/case_study
```

Run tests:

```bash
python3 -m unittest discover -s tests
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
| Pattern detector | `flang_modernizer/analyzer.py` |
| Impact analyzer | `ModernizationAnalyzer._compute_impact` |
| Prioritized plan | `flang_modernizer/reporter.py` |
| Legacy tests | `examples/legacy`, `tests/test_analyzer.py` |
| Case study | `examples/case_study`, `docs/case_study_report.md` after running the command |