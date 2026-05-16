# Modernization Scoring Model

Each finding receives a numeric priority so the report can sort modernization work by expected modernization risk and impact. The implemented formula lives in `flang_ast_advisor/src/Findings.cpp`.

Priority is computed as:

```text
pattern weight
+ 4 * affected file count
+ 3 * dependent construct count
+ 5 * behavior risk count
+ effort penalty
+ safety weight
```

## Pattern Weights

| Pattern | Weight | Rationale |
|---|---:|---|
| `equivalence` | 34 | Storage overlays create aliasing and binary-layout risk |
| `common` | 31 | Shared storage layout and cross-file migration risk |
| `entry` | 29 | Alternate procedure entry points and shared local state |
| `computed-goto` | 24 | Indexed label-based control flow |
| `arithmetic-if` | 23 | Three-way label-based branch |
| `assumed-size-array` | 20 | Interface and calling-convention risk |
| `implicit-typing` | 17 | Hidden symbol/type assumptions |
| `statement-function` | 14 | Usually local, but can capture host-scope assumptions |
| `fixed-form` | 8 | Mostly mechanical source-form modernization |
| unknown pattern | 10 | Conservative default |

## Effort Penalties

| Effort | Penalty |
|---|---:|
| `trivial` | -2 |
| `moderate` | 6 |
| `complex` | 13 |

`trivial` receives a small negative penalty so safe mechanical cleanup does not outrank riskier semantic work.

## Safety Weights

| Safety | Weight |
|---|---:|
| `safe` | 0 |
| `review-needed` | 11 |
| `risky` | 22 |

Riskier findings are intentionally pushed higher in the plan because they need earlier human attention, even when they are not automatically transformed.

## Interpretation

Higher priority does not mean “rewrite this first automatically.” It means “review this first in the modernization plan.” For example, `COMMON` and `EQUIVALENCE` often rank high because they may change storage layout or aliasing behavior, while fixed-form conversion usually ranks lower because it is safer and more mechanical.
