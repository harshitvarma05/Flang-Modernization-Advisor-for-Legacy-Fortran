# Presentation Guide

## Explanation

This project is a Flang-based modernization advisor for legacy Fortran. It takes a Fortran file or folder, parses it with Flang in process, walks Flang's typed parse tree, runs semantic analysis, and generates a prioritized modernization plan. The plan includes effort, safety, affected files, dependencies, and behavior risks.

## Demo Flow

```bash
cd "Flang-Modernization-Advisor-for-Legacy-Fortran"
cmake -S . -B cmake-build
cmake --build cmake-build
ctest --test-dir cmake-build --output-on-failure
cmake --build cmake-build --target real-report
./cmake-build/flang-modernizer --serve
```

Open `http://localhost:8080`, select `examples/real_case_study/minpack`, and click **Analyze**.

## Main Code

Open `flang_ast_advisor/src/FlangAstAdvisor.cpp` and point to:

- `parser::Parsing` for Flang prescan/parse
- `parser::Walk(*parsing.parseTree(), visitor)` for typed AST traversal
- visitor methods such as `Pre(const parser::CommonStmt&)`
- `semantics::Semantics` and `semantics.Perform()`
- `Scope::commonBlocks()` and `Scope::equivalenceSets()`

## Tool Importance

Regex can find words, but Flang semantic analysis resolves symbols, scopes, COMMON layout, and EQUIVALENCE alias sets. That is why this tool can attach risk and safety assessments instead of only flagging old syntax.
