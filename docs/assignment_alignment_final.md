# Final Assignment Alignment

The project has one analysis engine: the in-memory Flang AST advisor in `flang_ast_advisor/`. The CLI and web UI both call this AST-based analyzer.

## Deliverable Match

| Assignment requirement | Current implementation |
|---|---|
| Detect arithmetic IF | `parser::ArithmeticIfStmt` visitor |
| Detect computed GOTO | `parser::ComputedGotoStmt` visitor |
| Detect EQUIVALENCE | `parser::EquivalenceStmt` visitor plus `Scope::equivalenceSets()` |
| Detect COMMON | `parser::CommonStmt` visitor plus `Scope::commonBlocks()` / `CommonBlockDetails` |
| Detect implicit typing | `parser::ImplicitStmt` and semantic `Symbol::Flag::Implicit` |
| Detect fixed-form | Flang prescan parser option `Options::isFixedForm` |
| Detect statement functions | `parser::StmtFunctionStmt` visitor |
| Detect assumed-size arrays | Flang assumed-size/implied `*` array-spec visitor |
| Detect ENTRY | `parser::EntryStmt` visitor |
| Impact analyzer | affected files, target labels, COMMON members/layout, EQUIVALENCE alias sets, semantic implicit symbols |
| Prioritized plan | effort/safety/priority scoring in `flang_ast_advisor/src/Findings.cpp` |
| Test suite | `examples/legacy/`, `flang_ast_advisor/tests/test_flang_ast_advisor.cpp` |
| Real case study | Netlib MINPACK subset under `examples/real_case_study/minpack` |
| 3 safe transformations | `src/Transform.cpp` and committed outputs under `transformed/` |

## Workflow

The tool feeds Fortran source into Flang, builds the parse tree in memory, walks typed AST nodes for legacy constructs, runs Flang semantic analysis, reads semantic scopes/symbols for impact, and emits a prioritized modernization plan with effort and safety ratings.
