# Branch Coverage Report

**Branch coverage 58.9% (942 / 1,600) as reported by GCC; 85.7% (926 / 1,081) once compiler-generated exception-handling branches are excluded.** Measured on the Sprint 3 code on 16-11-2026.

## Measurement scope

Same build and executions as the Code Coverage Report: `bash scripts/run_coverage.sh`, MSYS2 UCRT64 `g++ 16.2.0` / `gcov`, gcovr 8.8, `--coverage -O0 -g`. All production sources under `src/` and `vehicle/` are included. The run covers the unit suite, the Test Plan acceptance runner (60 in-process cases), and six application runs, including the default topology and the rejected configurations.

## Why two branch figures are reported

GCC counts a branch for every possible exception-propagation edge, for example each `std::string` or `std::map` call that could throw `bad_alloc`. In this code base 519 of the 1,600 counted branches are such edges. They cannot be exercised without memory-exhaustion fault injection and are not decisions in our code. gcovr's `--exclude-throw-branches` removes them, which leaves the decisions the code actually makes (`if`, `while`, `switch`, `&&` / `||`, `?:`).

| View | Branches taken | Total | Coverage |
|---|---:|---:|---:|
| Raw GCC branches (primary, comparable with earlier reports) | 942 | 1,600 | **58.9%** |
| Excluding compiler-generated exception branches (decision coverage view) | 926 | 1,081 | **85.7%** |

## Per-file branch coverage

| Source file | Raw: taken / total | Raw % | Excl. throw: taken / total | Excl. throw % |
|---|---:|---:|---:|---:|
| src/ConsoleView.cpp | 47 / 86 | 54% | 47 / 48 | 97% |
| src/Intersection.cpp | 18 / 28 | 64% | 18 / 19 | 94% |
| src/LaneQueue.cpp | 25 / 38 | 65% | 25 / 29 | 86% |
| src/main.cpp | 39 / 76 | 51% | 38 / 41 | 92% |
| src/SimulationEngine.cpp | 492 / 810 | 60% | 481 / 562 | 85% |
| src/StatisticsCollector.cpp | 162 / 294 | 55% | 161 / 176 | 91% |
| src/TrafficSignalController.cpp | 75 / 100 | 75% | 74 / 85 | 87% |
| vehicle/Vehicle.cpp | 10 / 28 | 35% | 10 / 22 | 45% |
| vehicle/VehicleGenerator.cpp | 74 / 140 | 52% | 72 / 99 | 72% |
| **Total** | **942 / 1,600** | **58.9%** | **926 / 1,081** | **85.7%** |

## Decision-level observations

- **Every phase transition is covered in both directions.** `TrafficSignalController.cpp` covers all three phases, the normal rotation, and all three preemption states (from Green, Yellow and Red), including the "request rejected" outcomes (unknown lane, already pending, already Green).
- **Configuration parser** (`SimulationEngine.cpp`): the missing, invalid-type, out-of-range, unknown-key, self-loop, invalid-lane-list, and timing-key validation branches are all taken. The remaining untaken decisions are the duplicate-ID, duplicate-connection, empty key/value and overflow guards listed in the Code Coverage Report.
- **Spillback decision** (`hasDownstreamCapacity`): both outcomes are taken — room available, and lane full or in-flight transfers already filling it.
- **Lowest file, `vehicle/Vehicle.cpp` (45%)**: its untaken branches are the constructor validation throws (non-positive ID, invalid type, tick overflow). They are good candidates for additional negative unit tests.

## Comparison with the previous measurement

| Measurement | Raw branch coverage |
|---|---|
| Previous main-branch CI (before Sprint 3) | 54.8% (686 / 1,252) |
| **Sprint 3, this report** | **58.9% (942 / 1,600)** |

## Report files

- Annotated HTML (taken and not-taken markers on every line): [`coverage/index.html`](coverage/index.html)
- Text tables: [`coverage/branch_raw.txt`](coverage/branch_raw.txt), [`coverage/branch_nothrow.txt`](coverage/branch_nothrow.txt), [`coverage/summary_excluding_throw_branches.txt`](coverage/summary_excluding_throw_branches.txt)
- CI artifact: the `traffic-management-coverage` artifact of each GitHub Actions run (Ubuntu)

## Limitations

Branch coverage does not show that every combination of conditions was tried, and it is not requirement acceptance. Results for the requirements are in `System_Validation_Report.md`.
