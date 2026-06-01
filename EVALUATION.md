# Evaluation

## Metrics

| Metric | Current Coverage |
|---|---|
| Required pattern classes | 9/9 |
| Safety labels | safe, review-needed, risky |
| Effort labels | trivial, moderate, complex |
| Legacy fixture coverage | arithmetic IF, computed GOTO, EQUIVALENCE, COMMON, implicit typing, fixed-form, statement functions, assumed-size arrays, ENTRY |
| Real-code case study | Netlib MINPACK subset |
| Safe transformation validation | fixed-form conversion and conservative statement-function conversion |
| Executable demo | MINPACK `hybrd1` driver |

## Test Cases

1. `examples/legacy/all_patterns.f`: positive fixture containing every required construct.
2. `examples/legacy/false_positives.f90`: negative fixture checking that normal constructs are not misreported.
3. `examples/case_study/solver.f`: multi-routine demo with COMMON, assumed-size arrays, calls, and fixed-form source.
4. `examples/case_study/physics.f`: demo with COMMON, EQUIVALENCE, statement function, assumed-size arrays, and fixed-form source.
5. `examples/real_case_study/minpack/`: real legacy MINPACK subset with fixed-form source, EQUIVALENCE, and implicit typing.
6. `examples/real_case_study/minpack_driver/main.f90`: executable driver that links transformed MINPACK routines.

## Commands

```bash
./build.sh
./run.sh examples/case_study
./run.sh examples/real_case_study/minpack
./demo.sh
```

## Baseline Comparison

A formatting-only source rewriter can convert fixed-form source, but it cannot answer semantic questions such as which COMMON declarations share storage, which EQUIVALENCE objects are aliases, or which callers are affected by interface modernization. This advisor uses Flang AST and semantic facts to report those dependencies and risks.

## Expected Demonstration Results

- Initial case-study analysis reports all risky/review-needed/safe findings.
- Safe transformation removes safe fixed-form/statement-function findings while leaving risky COMMON/EQUIVALENCE findings for manual review.
- MINPACK transformed sources validate with Flang.
- `run-minpack-driver` links transformed MINPACK routines into an executable and solves `x^2 - 1 = 0`.
