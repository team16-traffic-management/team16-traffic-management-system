# Code Coverage Report

<badge color="success">Final main-branch coverage generated</badge>

## Measurement scope
- Main-branch workflow: [Run #241](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43382188203).
- Tested main commit: `10873b6686019ff4643463888be0050f491de032`.
- Runner/compiler: Ubuntu GitHub Actions runner with GCC (version recorded in workflow log).
- Instrumentation: GCC `--coverage`, `-O0 -g`.
- Executed: consolidated automated test executable; baseline, zero-value and high-load application fixtures; invalid and corrupted fixtures (expected non-zero exits).
- Product scope: all `.cpp` translation units under `src/` and `vehicle/`, including `src/main.cpp`; gcovr also counts instrumented inline code in `vehicle/VehicleGenerator.h`.
- Excluded: `tests/test_traffic_management.cpp` as test harness; `include/` header-only code is not included in the reported scope.

## Results

| Metric | Result |
|---|---:|
| Executable lines | 667 / 758 |
| Line coverage | **88.0%** |
| Functions executed | 81 / 94 |
| Function coverage | 86.2% |
| Branches executed | 686 / 1,252 |
| Branch coverage | 54.8% |

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
[Download HTML and Cobertura XML coverage artifact](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43381117586/artifacts/14016947229).

## Limitations
This is final main-branch coverage for the tested commit above. Coverage is execution evidence, not requirements acceptance. It does not mean all 66 Test Plan cases passed; the acceptance register remains 66/66 NOT RUN. Review uncovered lines/branches and add tests only where they serve requirements, not just to inflate the percentage.
