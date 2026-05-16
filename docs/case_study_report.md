# In-Memory Flang AST Modernization Report

Root: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study`

Files analyzed: 2

Total findings: 24

## Pattern Summary

| Pattern | Count |
|---|---:|
| arithmetic-if | 1 |
| assumed-size-array | 6 |
| common | 8 |
| equivalence | 2 |
| fixed-form | 2 |
| implicit-typing | 4 |
| statement-function | 1 |

## Prioritized Modernization Plan

| Priority | Pattern | Location | Effort | Safety | Recommendation |
|---:|---|---|---|---|---|
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 114 | common | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 94 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:5 | complex | risky | Review EQUIVALENCE manually before replacing storage overlays. |
| 94 | equivalence | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1 | complex | risky | Flang semantic scope resolved an EQUIVALENCE set; keep as risky until aliasing is reviewed. |
| 64 | assumed-size-array | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:3 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 64 | assumed-size-array | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:3 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 64 | assumed-size-array | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:15 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 64 | assumed-size-array | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:15 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 60 | assumed-size-array | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:18 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 60 | assumed-size-array | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:18 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 54 | implicit-typing | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 54 | implicit-typing | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 54 | implicit-typing | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 54 | implicit-typing | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 52 | arithmetic-if | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:7 | moderate | review-needed | Replace arithmetic IF with structured IF/ELSE blocks. |
| 27 | statement-function | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:6 | moderate | safe | Convert statement function to an internal function. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |

## Flang Evidence And Impact Details

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `solver`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: COMMON objects: n, x, y
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang AST evidence: Visited parser::CommonStmt in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: COMMON objects: ng, gx, gy
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang AST evidence: Visited parser::CommonStmt in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `init`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: semantic COMMON members: ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `finalize`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: COMMON objects: ng, gx, gy
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang AST evidence: Visited parser::CommonStmt in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: semantic COMMON members: ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `SOLVER`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: semantic COMMON members: n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `init`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: COMMON objects: ng, gx, gy
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang AST evidence: Visited parser::CommonStmt in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### common at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `finalize`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: semantic COMMON members: ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4
- Dependent construct: COMMON migration scope: 8 declarations across 2 files
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:4 scope step layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:16 scope finalize layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope solver layout [n, x, y]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng, gx, gy]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:5 scope SOLVER layout [n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80]
- Dependent construct: COMMON declaration: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:19 scope init layout [ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4]
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: COMMON layout is not textually identical across scopes; module conversion must preserve storage order, offsets, and any intentional name differences.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.
- Flang semantic evidence: Whole-project COMMON impact grouped Flang AST/semantic COMMON declarations by block name.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:5`

- Effort: complex
- Safety: risky
- Construct: `EQUIVALENCE set 1`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f`
- Dependent construct: overlaid objects: tmp(1), local
- Dependent construct: alias class size: 2 storage-associated objects
- Behavior/aliasing risk: Removing this overlay can change aliasing, alignment, and old binary layout assumptions.
- Behavior/aliasing risk: Alias class includes array elements or substrings; replacing it can change element-level storage interpretation.
- Behavior/aliasing risk: A safe rewrite must preserve every use that relies on shared storage, not just replace names locally.
- Flang AST evidence: Visited parser::EquivalenceStmt in memory.

### equivalence at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1`

- Effort: complex
- Safety: risky
- Construct: `semantic EQUIVALENCE set 1`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f`
- Dependent construct: semantic aliases: tmp(1), local
- Dependent construct: alias class size: 2 storage-associated objects
- Behavior/aliasing risk: Flang resolved actual storage association, so this is a real aliasing dependency.
- Behavior/aliasing risk: Alias class includes array elements or substrings; replacing it can change element-level storage interpretation.
- Behavior/aliasing risk: A safe rewrite must preserve every use that relies on shared storage, not just replace names locally.
- Flang semantic evidence: Read semantics::Scope::equivalenceSets() in memory.

### assumed-size-array at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:3`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*),tmp(2)`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Dependent construct: call-site impact: 1 direct calls across 1 files
- Dependent construct: call site: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:10 in solver calls step
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Behavior/aliasing risk: Every listed caller may need an explicit interface before assumed-shape modernization.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:3`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*),tmp(2)`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Dependent construct: call-site impact: 1 direct calls across 1 files
- Dependent construct: call site: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:10 in solver calls step
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Behavior/aliasing risk: Every listed caller may need an explicit interface before assumed-shape modernization.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:15`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `finalize`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Dependent construct: call-site impact: 1 direct calls across 1 files
- Dependent construct: call site: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:12 in solver calls finalize
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Behavior/aliasing risk: Every listed caller may need an explicit interface before assumed-shape modernization.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:15`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `finalize`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f` `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Dependent construct: call-site impact: 1 direct calls across 1 files
- Dependent construct: call site: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:12 in solver calls finalize
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Behavior/aliasing risk: Every listed caller may need an explicit interface before assumed-shape modernization.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:18`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `init`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Dependent construct: call-site impact: 1 direct calls across 1 files
- Dependent construct: call site: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:8 in solver calls init
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Behavior/aliasing risk: Every listed caller may need an explicit interface before assumed-shape modernization.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:18`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `init`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Dependent construct: call-site impact: 1 direct calls across 1 files
- Dependent construct: call site: /Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:8 in solver calls init
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Behavior/aliasing risk: Every listed caller may need an explicit interface before assumed-shape modernization.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### implicit-typing at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `init`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: implicit symbols: gx, gy, i, ng
- Dependent construct: declaration worklist: add explicit declarations for gx, gy, i, ng before inserting IMPLICIT NONE
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Behavior/aliasing risk: Changing implicit typing can reveal misspelled variables that previously compiled as new implicit symbols.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### implicit-typing at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `finalize`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f`
- Dependent construct: implicit symbols: gx, gy, ng
- Dependent construct: declaration worklist: add explicit declarations for gx, gy, ng before inserting IMPLICIT NONE
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Behavior/aliasing risk: Changing implicit typing can reveal misspelled variables that previously compiled as new implicit symbols.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### implicit-typing at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `scale`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f`
- Dependent construct: implicit symbols: scale, z
- Dependent construct: declaration worklist: add explicit declarations for scale, z before inserting IMPLICIT NONE
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Behavior/aliasing risk: Changing implicit typing can reveal misspelled variables that previously compiled as new implicit symbols.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### implicit-typing at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f`
- Dependent construct: implicit symbols: gx, gy, local, ng
- Dependent construct: declaration worklist: add explicit declarations for gx, gy, local, ng before inserting IMPLICIT NONE
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Behavior/aliasing risk: Changing implicit typing can reveal misspelled variables that previously compiled as new implicit symbols.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### arithmetic-if at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:7`

- Effort: moderate
- Safety: review-needed
- Construct: `if(mode)10,20,30`
- Routine/scope: `solver`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: target labels: 10, 20, 30
- Behavior/aliasing risk: Branch ordering depends on negative, zero, and positive expression values.
- Flang AST evidence: Visited parser::ArithmeticIfStmt in memory.

### statement-function at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:6`

- Effort: moderate
- Safety: safe
- Construct: `scale(z)`
- Routine/scope: `step`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f`
- Dependent construct: dummy arguments: z
- Flang AST evidence: Visited parser::StmtFunctionStmt in memory.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/solver.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `/Users/harshit/Documents/Flang Modernization Advisor for Legacy Fortran/examples/case_study/physics.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

