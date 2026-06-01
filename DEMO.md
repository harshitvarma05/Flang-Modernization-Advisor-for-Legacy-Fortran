# Demo Guide

Use this file as the script for the required demo video or screenshots.

## 1. Build Through Provided Script

```bash
./build.sh
```

Show that LLVM/Flang is detected, the project builds, and CTest passes.

## 2. Analyze Legacy Fixture

```bash
./run.sh examples/legacy/all_patterns.f
```

Show that all required legacy patterns are detected.

## 3. Analyze Case Study In Web UI

```bash
./cmake-build/flang-modernizer --serve
```

Open `http://localhost:8080`, click **Case Study**, then **Analyze**.

Screenshots to capture:

- output report summary
- Modernization Plan modal
- Semantic Analysis modal

## 4. Safe Transform Validation

In the web UI, click **Safe Transform**, then analyze the transformed folder. Show that safe findings disappear while risky semantic findings remain for review.

## 5. Real MINPACK Case Study

```bash
./run.sh examples/real_case_study/minpack
./demo.sh
```

Show:

- MINPACK report generation
- transformed files generation
- executable MINPACK driver output

Expected driver result includes `info = 1`, solution near `1.0`, and near-zero residual.

## 6. Failure/Review Case

Use the remaining `EQUIVALENCE` or `COMMON` findings as the failure/review case: the tool intentionally refuses to auto-transform risky storage-aliasing changes and instead reports affected files, layouts, alias classes, and behavior risks.
