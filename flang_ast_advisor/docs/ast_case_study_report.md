# In-Memory Flang AST Modernization Report

Root: `examples/case_study`

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
| 94 | common | ./examples/case_study/solver.f:19 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 94 | common | ./examples/case_study/physics.f:4 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 94 | common | ./examples/case_study/solver.f:5 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 94 | common | ./examples/case_study/physics.f:16 | complex | risky | Flang semantic scope contains a COMMON block; compare layouts before module conversion. |
| 90 | common | ./examples/case_study/physics.f:4 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 90 | common | ./examples/case_study/physics.f:16 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 90 | common | ./examples/case_study/solver.f:19 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 90 | common | ./examples/case_study/solver.f:5 | complex | risky | Replace COMMON storage with a module after checking layout compatibility. |
| 81 | equivalence | ./examples/case_study/physics.f:5 | complex | risky | Review EQUIVALENCE manually before replacing storage overlays. |
| 81 | equivalence | examples/case_study/physics.f:1 | complex | risky | Flang semantic scope resolved an EQUIVALENCE set; keep as risky until aliasing is reviewed. |
| 52 | arithmetic-if | ./examples/case_study/solver.f:7 | moderate | review-needed | Replace arithmetic IF with structured IF/ELSE blocks. |
| 49 | assumed-size-array | ./examples/case_study/physics.f:3 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 49 | assumed-size-array | ./examples/case_study/physics.f:3 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 49 | assumed-size-array | ./examples/case_study/physics.f:15 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 49 | assumed-size-array | ./examples/case_study/physics.f:15 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 49 | assumed-size-array | ./examples/case_study/solver.f:18 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 49 | assumed-size-array | ./examples/case_study/solver.f:18 | moderate | review-needed | Consider assumed-shape arrays only after introducing explicit interfaces. |
| 46 | implicit-typing | examples/case_study/solver.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 46 | implicit-typing | examples/case_study/physics.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 46 | implicit-typing | examples/case_study/physics.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 46 | implicit-typing | examples/case_study/physics.f:1 | moderate | review-needed | Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE. |
| 27 | statement-function | ./examples/case_study/physics.f:6 | moderate | safe | Convert statement function to an internal function. |
| 13 | fixed-form | examples/case_study/solver.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |
| 13 | fixed-form | examples/case_study/physics.f:1 | trivial | safe | Convert fixed-form source to free-form Fortran. |

## Flang Evidence And Impact Details

### common at `./examples/case_study/solver.f:19`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `init`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f` `examples/case_study/solver.f`
- Dependent construct: semantic COMMON members: ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.

### common at `./examples/case_study/physics.f:4`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `step`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f` `examples/case_study/physics.f`
- Dependent construct: semantic COMMON members: ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.

### common at `./examples/case_study/solver.f:5`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `SOLVER`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f` `examples/case_study/solver.f`
- Dependent construct: semantic COMMON members: n@offset=0,size=4, x@offset=4,size=80, y@offset=84,size=80
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.

### common at `./examples/case_study/physics.f:16`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `finalize`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f` `examples/case_study/physics.f`
- Dependent construct: semantic COMMON members: ng@offset=0,size=4, gx@offset=4,size=4, gy@offset=8,size=4
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Semantic storage offsets show layout-sensitive modernization.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang semantic evidence: Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.

### common at `./examples/case_study/physics.f:4`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `step`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f`
- Dependent construct: COMMON objects: ng, gx, gy
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang AST evidence: Visited parser::CommonStmt in memory.

### common at `./examples/case_study/physics.f:16`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `finalize`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f`
- Dependent construct: COMMON objects: ng, gx, gy
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang AST evidence: Visited parser::CommonStmt in memory.

### common at `./examples/case_study/solver.f:19`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `init`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f`
- Dependent construct: COMMON objects: ng, gx, gy
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang AST evidence: Visited parser::CommonStmt in memory.

### common at `./examples/case_study/solver.f:5`

- Effort: complex
- Safety: risky
- Construct: `COMMON /grid/`
- Routine/scope: `solver`
- Affected files: `./examples/case_study/physics.f` `./examples/case_study/solver.f`
- Dependent construct: COMMON objects: n, x, y
- Dependent construct: cross-file COMMON users: 2
- Behavior/aliasing risk: Changing storage association can alter aliasing, initialization, and binary I/O layout.
- Behavior/aliasing risk: Module replacement requires updating every semantic COMMON declaration consistently.
- Flang AST evidence: Visited parser::CommonStmt in memory.

### equivalence at `./examples/case_study/physics.f:5`

- Effort: complex
- Safety: risky
- Construct: `EQUIVALENCE set 1`
- Routine/scope: `step`
- Affected files: `./examples/case_study/physics.f`
- Dependent construct: overlaid objects: tmp(1), local
- Behavior/aliasing risk: Removing this overlay can change aliasing, alignment, and old binary layout assumptions.
- Flang AST evidence: Visited parser::EquivalenceStmt in memory.

### equivalence at `examples/case_study/physics.f:1`

- Effort: complex
- Safety: risky
- Construct: `semantic EQUIVALENCE set 1`
- Routine/scope: `step`
- Affected files: `examples/case_study/physics.f`
- Dependent construct: semantic aliases: tmp(1), local
- Behavior/aliasing risk: Flang resolved actual storage association, so this is a real aliasing dependency.
- Flang semantic evidence: Read semantics::Scope::equivalenceSets() in memory.

### arithmetic-if at `./examples/case_study/solver.f:7`

- Effort: moderate
- Safety: review-needed
- Construct: `if(mode)10,20,30`
- Routine/scope: `solver`
- Affected files: `./examples/case_study/solver.f`
- Dependent construct: target labels: 10, 20, 30
- Behavior/aliasing risk: Branch ordering depends on negative, zero, and positive expression values.
- Flang AST evidence: Visited parser::ArithmeticIfStmt in memory.

### assumed-size-array at `./examples/case_study/physics.f:3`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*),tmp(2)`
- Routine/scope: `step`
- Affected files: `./examples/case_study/physics.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `./examples/case_study/physics.f:3`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*),tmp(2)`
- Routine/scope: `step`
- Affected files: `./examples/case_study/physics.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `./examples/case_study/physics.f:15`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `finalize`
- Affected files: `./examples/case_study/physics.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `./examples/case_study/physics.f:15`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `finalize`
- Affected files: `./examples/case_study/physics.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `./examples/case_study/solver.f:18`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `init`
- Affected files: `./examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### assumed-size-array at `./examples/case_study/solver.f:18`

- Effort: moderate
- Safety: review-needed
- Construct: `realx(*),y(*)`
- Routine/scope: `init`
- Affected files: `./examples/case_study/solver.f`
- Dependent construct: dummy array bound uses assumed-size '*'
- Behavior/aliasing risk: Changing to assumed-shape changes calling conventions and requires explicit interfaces.
- Flang AST evidence: Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.

### implicit-typing at `examples/case_study/solver.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `init`
- Affected files: `examples/case_study/solver.f`
- Dependent construct: implicit symbols: gx, gy, i, ng
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### implicit-typing at `examples/case_study/physics.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `finalize`
- Affected files: `examples/case_study/physics.f`
- Dependent construct: implicit symbols: gx, gy, ng
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### implicit-typing at `examples/case_study/physics.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `scale`
- Affected files: `examples/case_study/physics.f`
- Dependent construct: implicit symbols: scale, z
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### implicit-typing at `examples/case_study/physics.f:1`

- Effort: moderate
- Safety: review-needed
- Construct: `implicit symbols`
- Routine/scope: `step`
- Affected files: `examples/case_study/physics.f`
- Dependent construct: implicit symbols: gx, gy, local, ng
- Behavior/aliasing risk: Modernization can fail compilation until every implicit symbol has an explicit declaration.
- Flang semantic evidence: Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.

### statement-function at `./examples/case_study/physics.f:6`

- Effort: moderate
- Safety: safe
- Construct: `scale(z)`
- Routine/scope: `step`
- Affected files: `./examples/case_study/physics.f`
- Dependent construct: dummy arguments: z
- Flang AST evidence: Visited parser::StmtFunctionStmt in memory.

### fixed-form at `examples/case_study/solver.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `examples/case_study/solver.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

### fixed-form at `examples/case_study/physics.f:1`

- Effort: trivial
- Safety: safe
- Construct: `source form`
- Affected files: `examples/case_study/physics.f`
- Dependent construct: Flang prescan configured as fixed-form
- Flang AST evidence: File was parsed in memory with parser::Options::isFixedForm=true.

