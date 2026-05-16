# In-Memory Flang AST Modernization Report

Root: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack`

Files analyzed: 10

Total findings: 18

## Pattern Summary

| Pattern | Count |
|---|---:|
| equivalence | 7 |
| fixed-form | 10 |
| implicit-typing | 1 |

## Prioritized Modernization Plan

| Priority | Pattern | Location | Effort | Safety | Recommendation |
|---:|---|---|---|---|---|
| 81 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:38 | complex | risky | Review EQUIVALENCE manually before replacing storage overlays. |
| 81 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:39 | complex | risky | Review EQUIVALENCE manually before replacing storage overlays. |
| 81 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:40 | complex | risky | Review EQUIVALENCE manually before replacing storage overlays. |
| 81 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1 | complex | risky | Flang semantic scope resolved an EQUIVALENCE set; keep as risky until aliasing is reviewed. |
| 81 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1 | complex | risky | Flang semantic scope resolved an EQUIVALENCE set; keep as risky until aliasing is reviewed. |
| 81 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1 | complex | risky | Flang semantic scope resolved an EQUIVALENCE set; keep as risky until aliasing is reviewed. |
| 81 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1 | complex | risky | Flang semantic scope resolved an EQUIVALENCE set; keep as risky until aliasing is reviewed. |
| 46 | implicit-typing | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/fdjac1.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dogleg.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/enorm.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/fdjac1.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/hybrd.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/hybrd1.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/qform.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/qrfac.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/r1mpyq.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/r1updt.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |

## Flang Evidence And Impact Details

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:38`

- Effort: complex
- Safety: risky
- Construct: `EQUIVALENCE set 1`
- Routine/scope: `dpmpar`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: overlaid objects: dmach(1), mcheps(1)
- Behavior/aliasing risk: Removing this overlay can change aliasing, alignment, and old binary layout assumptions.
- Flang AST evidence: Visited parser::EquivalenceStmt in memory.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:39`

- Effort: complex
- Safety: risky
- Construct: `EQUIVALENCE set 1`
- Routine/scope: `dpmpar`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: overlaid objects: dmach(2), minmag(1)
- Behavior/aliasing risk: Removing this overlay can change aliasing, alignment, and old binary layout assumptions.
- Flang AST evidence: Visited parser::EquivalenceStmt in memory.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:40`

- Effort: complex
- Safety: risky
- Construct: `EQUIVALENCE set 1`
- Routine/scope: `dpmpar`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: overlaid objects: dmach(3), maxmag(1)
- Behavior/aliasing risk: Removing this overlay can change aliasing, alignment, and old binary layout assumptions.
- Flang AST evidence: Visited parser::EquivalenceStmt in memory.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1`

- Effort: complex
- Safety: risky
- Construct: `semantic EQUIVALENCE set 1`
- Routine/scope: `dpmpar`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: semantic aliases: dmach(1), mcheps(1)
- Behavior/aliasing risk: Flang resolved actual storage association, so this is a real aliasing dependency.
- Flang semantic evidence: Read semantics::Scope::equivalenceSets() in memory.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1`

- Effort: complex
- Safety: risky
- Construct: `semantic EQUIVALENCE set 2`
- Routine/scope: `dpmpar`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: semantic aliases: dmach(2), minmag(1)
- Behavior/aliasing risk: Flang resolved actual storage association, so this is a real aliasing dependency.
- Flang semantic evidence: Read semantics::Scope::equivalenceSets() in memory.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1`

- Effort: complex
- Safety: risky
- Construct: `semantic EQUIVALENCE set 3`
- Routine/scope: `dpmpar`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: semantic aliases: dmach(3), maxmag(1)
- Behavior/aliasing risk: Flang resolved actual storage association, so this is a real aliasing dependency.
- Flang semantic evidence: Read semantics::Scope::equivalenceSets() in memory.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1`

- Effort: complex
- Safety: risky
- Construct: `semantic EQUIVALENCE set 4`
- Routine/scope: `dpmpar`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: semantic aliases: .F18.0, dmach
- Behavior/aliasing risk: Flang resolved actual storage association, so this is a real aliasing dependency.
- Flang semantic evidence: Read semantics::Scope::equivalenceSets() in memory.

### implicit-typing at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/fdjac1.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `fdjac1`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/fdjac1.f`
- Dependent construct: implicit symbols: fcn
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dogleg.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dogleg.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/dpmpar.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/enorm.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/enorm.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/fdjac1.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/fdjac1.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/hybrd.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/hybrd.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/hybrd1.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/hybrd1.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/qform.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/qform.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/qrfac.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/qrfac.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/r1mpyq.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/r1mpyq.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/r1updt.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/real_case_study/minpack/r1updt.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

