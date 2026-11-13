# Code Coverage Report

<badge color="success">PR CI coverage generated</badge>

## Measurement scope
- Workflow: [PR #121 CI run #230](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43380638801).
- Tested PR head: `7e8b6c58666abfea2fdcc4161217346deed76068` (before merge to main).
- Runner/compiler: Ubuntu GitHub Actions runner with GCC (version recorded in the workflow log).
- Instrumentation: GCC `--coverage`, `-O0 -g`.
- Executed: consolidated automated test executable; baseline, zero-value and high-load application fixtures; invalid and corrupted fixtures (expected non-zero exits).
- Product scope: all `.cpp` translation units under `src/` and `vehicle/`, including `src/main.cpp`; gcovr also counts the instrumented inline code in `vehicle/VehicleGenerator.h`.
- Excluded: `tests/test_traffic_management.cpp` as test harness; `include/` header-only code is not included in the reported scope.

## Results

| Metric | Result |
|---|---:|
| Executable lines | 667 / 758 |
| Line coverage | **88.0%** |
| Functions executed | 81 / 94 |
| Function coverage | 86.2% |
| Branches executed | 683 / 1,252 |
| Branch coverage | 54.6% |

### Per-file line coverage

| Source file | Lines | Executed | Coverage |
|---|---:|---:|---:|
| src/ConsoleView.cpp | 19 | 19 | 100% |
| src/Intersection.cpp | 25 | 25 | 100% |
| src/LaneQueue.cpp | 48 | 40 | 83% |
| src/main.cpp | 30 | 18 | 60% |
| src/SimulationEngine.cpp | 278 | 261 | 93% |
| src/StatisticsCollector.cpp | 99 | 86 | 86% |
| src/TrafficSignalController.cpp | 91 | 85 | 93% |
| vehicle/Vehicle.cpp | 35 | 27 | 77% |
| vehicle/VehicleGenerator.cpp | 128 | 101 | 78% |
| vehicle/VehicleGenerator.h | 5 | 5 | 100% |
| **TOTAL** | **758** | **667** | **88.0%** |

## Artifact
[Download HTML and Cobertura XML coverage artifact](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43380638801/artifacts/14016517314).

## Limitations
This is PR-branch coverage, not yet a post-merge main-branch measurement. Repeat on the final main commit and update the recorded SHA/run before submission. Coverage is execution evidence, not requirements acceptance. It does not mean all 66 Test Plan cases passed; the acceptance register remains 66/66 NOT RUN.
