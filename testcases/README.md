# Testcases

The executable tests are driven by `flang_ast_advisor/tests/test_flang_ast_advisor.cpp` and run with:

```bash
./build.sh
```

The source testcases used by the tests and demo are:

| Testcase | Purpose |
|---|---|
| `examples/legacy/all_patterns.f` | Positive fixture covering all required legacy patterns |
| `examples/legacy/false_positives.f90` | Negative fixture for common false positives |
| `examples/case_study/solver.f` | Demo program unit with COMMON, calls, assumed-size arrays, fixed-form source |
| `examples/case_study/physics.f` | Demo program unit with EQUIVALENCE, statement function, COMMON, fixed-form source |
| `examples/real_case_study/minpack/` | Real legacy Fortran case study from MINPACK |
| `examples/real_case_study/minpack_driver/main.f90` | Main program used to link and run transformed MINPACK code |
