# Modernization Scoring Model

Each finding receives a numeric priority so the report can sort modernization work by risk and expected impact.

Priority is computed as:

```text
pattern weight
+ 5 * affected file count
+ 3 * dependent construct count
+ effort penalty
+ safety weight
```

Pattern weights:

| Pattern | Weight | Rationale |
|---|---:|---|
| EQUIVALENCE | 35 | Storage overlay and aliasing risk |
| COMMON block | 32 | Global storage layout and cross-file migration risk |
| ENTRY | 30 | Alternate procedure entry points and shared local state |
| Computed GOTO | 25 | Indexed control-flow table |
| Arithmetic IF | 23 | Label-based three-way branch |
| Implicit typing | 20 | Hidden symbol/type assumptions |
| Assumed-size array | 18 | Interface and bounds metadata risk |
| Statement function | 12 | Usually local but may capture host symbols |
| Fixed-form source | 8 | Mechanical source-form modernization |

Effort penalties:

| Effort | Penalty |
|---|---:|
| trivial | 0 |
| moderate | 8 |
| complex | 16 |

Safety weights:

| Safety | Weight |
|---|---:|
| safe | 0 |
| review-needed | 8 |
| risky | 16 |
