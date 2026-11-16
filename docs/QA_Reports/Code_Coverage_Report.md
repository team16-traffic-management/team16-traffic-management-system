# Code Coverage Report

**Line coverage 92.3% (849 / 920); function coverage 90.4% (103 / 114)** for all production sources, measured on the Sprint 3 code on 16-11-2026.

## Measurement scope

| Field | Value |
|---|---|
| Code under test | Sprint 3 baseline (`docs/SPRINT_3.md`) |
| Script | `bash scripts/run_coverage.sh` (`make coverage`) |
| Toolchain | MSYS2 UCRT64 `g++ 16.2.0` / `gcov 16.2.0`, gcovr 8.8; `--coverage -O0 -g` |
| Platform | Windows 11 (local). CI repeats the same script on `ubuntu-latest` on every push and uploads the HTML/XML artifact. |
| Production scope | Every `.cpp` under `src/` and `vehicle/` (including `src/main.cpp`), plus instrumented inline code in `vehicle/VehicleGenerator.h` |
| Excluded | Test code (`tests/`), benchmark, and header-only declarations in `include/` |
| Executions that contribute | Unit suite (42 tests, 1,198 checks); Test Plan acceptance runner (60 in-process cases, `--no-perf`); application runs on `baseline.cfg`, `zero_values.cfg`, `high_load.cfg` and the built-in default topology; rejected runs on `invalid_values.cfg` and `corrupted.cfg` |

## Results

| Metric | Covered | Total | Coverage |
|---|---:|---:|---:|
| Lines | 849 | 920 | **92.3%** |
| Functions | 103 | 114 | **90.4%** |
| Branches (see Branch Coverage Report) | 942 | 1,600 | 58.9% |

### Per-file line coverage

| Source file | Lines | Executed | Coverage | Uncovered lines |
|---|---:|---:|---:|---|
| src/ConsoleView.cpp | 22 | 22 | 100% | — |
| src/Intersection.cpp | 25 | 25 | 100% | — |
| src/main.cpp | 30 | 30 | 100% | — |
| src/TrafficSignalController.cpp | 146 | 142 | 97% | 77, 150, 166, 274 |
| src/SimulationEngine.cpp | 320 | 305 | 95% | 48, 58, 73, 90, 93, 124, 164, 206, 222, 226, 229, 313, 318, 370, 372 |
| src/StatisticsCollector.cpp | 162 | 149 | 92% | 124–151 (legacy single-intersection overloads); 106 (closing brace: compiler-generated cleanup path) |
| src/LaneQueue.cpp | 50 | 44 | 88% | 7–8, 19, 21, 94–95 |
| vehicle/Vehicle.cpp | 34 | 28 | 82% | 16, 19, 30, 33, 36, 46 |
| vehicle/VehicleGenerator.cpp | 126 | 99 | 78% | 45–51, 68, 80, 90, 99–114, 143, 160, 167, 187, 195, 200 |
| vehicle/VehicleGenerator.h | 5 | 5 | 100% | — |
| **Total** | **920** | **849** | **92.3%** | |

## Analysis of uncovered code

| Category | Examples | Assessment |
|---|---|---|
| Defensive error paths not yet triggered | Duplicate intersection ID (`SimulationEngine.cpp:48`), destination missing a lane (`:90`), duplicate outgoing connection (`:93`), empty key/value (`:124`), duplicate intersection definition (`:222`), negative `run()` ticks (`:370`), invalid vehicle ID/type (`Vehicle.cpp:16,19,30`) | Low risk: simple guard-and-throw lines. Good candidates for further negative unit tests. |
| Unused compatibility API | SAD-style `LaneQueue(size_t)` constructor and `getQueueLength()`; lane-name-only `VehicleGenerator` overloads (`:99–114`); single-intersection `StatisticsCollector` overloads (`:129–151`); `setEmergencyProbability` | Kept for interface compatibility with the SAD. The integrated engine does not call them. |
| Unreachable by construction | Preemption fallback when the pending lane disappears (`TrafficSignalController.cpp:150`); missing destination lane in `hasDownstreamCapacity` (`SimulationEngine.cpp:313`); `INT_MAX` tick and vehicle-ID overflow guards (`:318`, `:372`, `VehicleGenerator.cpp:143`) | Earlier validation prevents these states, or reaching them would need about 2³¹ ticks. Kept as safety nets. |

## Comparison with the previous measurement

| Measurement | Lines | Functions | Branches |
|---|---|---|---|
| Previous main-branch CI (PRs #121–#123, before Sprint 3) | 88.0% (667 / 758) | 86.2% (81 / 94) | 54.8% (686 / 1,252) |
| **Sprint 3, this report** | **92.3% (849 / 920)** | **90.4% (103 / 114)** | **58.9% (942 / 1,600)** |

The production code grew by 162 lines with the Sprint 3 fixes, while uncovered lines fell from 91 to 71. The gain comes from the new unit tests, the Test Plan acceptance runner (which now contributes to coverage), and running the app's default topology.

## Report files

- HTML report (per-file, line and branch annotations): [`coverage/index.html`](coverage/index.html)
- Cobertura XML: [`coverage/coverage.xml`](coverage/coverage.xml) (local path replaced by `.`)
- Text summaries: [`coverage/line_summary.txt`](coverage/line_summary.txt), [`coverage/branch_raw.txt`](coverage/branch_raw.txt), [`coverage/branch_nothrow.txt`](coverage/branch_nothrow.txt)

## Limitations

Coverage shows which code ran, not that it behaved correctly. Correctness against the requirements is reported in `System_Validation_Report.md`. The Ubuntu CI coverage job produces the same metrics for the pushed commit; small differences between compilers are possible.
