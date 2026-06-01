# Design

## Goal

The project is a Flang-based modernization advisor for legacy Fortran. It detects deprecated or risky constructs, uses compiler semantic facts to estimate modernization impact, and produces a prioritized plan rather than blindly rewriting source.

## Architecture

```text
Fortran source
  -> Flang prescan
  -> Flang parse tree
  -> typed AST visitor
  -> Flang semantic analysis
  -> project-level impact enrichment
  -> priority scoring
  -> Markdown/JSON/web reports
  -> conservative safe transformations
```

## Main Components

| Component | Responsibility |
|---|---|
| `flang_ast_advisor/src/FlangAstAdvisor.cpp` | In-memory Flang parse/semantic pipeline, AST pattern detection, semantic impact analysis |
| `flang_ast_advisor/src/Findings.cpp` | Effort/safety labels, priority scoring, Markdown/JSON reports |
| `src/Transform.cpp` | Conservative transformations used to validate safe recommendations |
| `src/WebServer.cpp` | Local dashboard for analysis, Flang validation, dumps, and transformations |
| `scripts/detect_llvm.sh` | macOS/Linux LLVM and Flang installation discovery |

## Alternatives Considered

1. Text/regex-only scanner: simpler, but does not meet the assignment requirement because it cannot use Flang semantic information.
2. External Flang dump parser: easier than linking Flang, but still weaker because parse/symbol dumps are unstable textual output.
3. In-process Flang library integration: chosen approach. It directly consumes Flang parse-tree and semantic structures.

## Safety Strategy

The tool separates recommendations from transformations. Safe changes such as fixed-form conversion and conservative statement-function conversion can be executed automatically. Risky semantic changes such as `COMMON` to modules and `EQUIVALENCE` removal are reported with impact details instead of being rewritten blindly.
