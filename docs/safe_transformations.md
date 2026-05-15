# Safe Transformation Validation

The case study executes three conservative transformations into `transformed_cpp/case_study`:

1. `solver.f` fixed-form source is emitted as `solver.f90` with fixed-form comments and labels normalized for free-form editing.
2. `physics.f` fixed-form source is emitted as `physics.f90` with fixed-form layout normalized.
3. The simple statement function `SCALE(Z) = Z * 0.5` is converted into an internal `real function SCALE(Z)` in `physics.f90`.

The tool intentionally does not rewrite `COMMON`, `EQUIVALENCE`, arithmetic `IF`, or assumed-size arrays automatically because the report rates those as review-needed or risky.
