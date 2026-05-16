# Real Legacy Case Study: Netlib MINPACK Subset

Source: https://netlib.org/minpack/

The real case study uses 10 public legacy Fortran files from Netlib MINPACK:

- dogleg.f
- dpmpar.f
- enorm.f
- fdjac1.f
- hybrd.f
- hybrd1.f
- qform.f
- qrfac.f
- r1mpyq.f
- r1updt.f

Validation:

```bash
flang -fsyntax-only examples/real_case_study/minpack/*.f
```

Modernization report:

```bash
./build/flang-modernizer examples/real_case_study/minpack --output docs/real_case_study_report.md
```

Safe transformation execution:

```bash
./build/flang-modernizer examples/real_case_study/minpack --safe-transform-out transformed/real_case_study
```

Summary from the generated report:

- Files analyzed: 10
- Findings: 23
- Main risks: 3 `EQUIVALENCE` storage overlays in `dpmpar.f`
- Repeated modernization opportunities: fixed-form conversion and explicit typing review
- Flang evidence: `EQUIVALENCE` findings are confirmed by Flang parse-tree nodes and semantic symbol-table equivalence sets
