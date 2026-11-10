# Requirements Traceability Matrix — Traffic Management System

Purpose: connect each requirement group to implementation and actual verification evidence. Presence in source code alone does not establish acceptance.

## Source and test inventory
| Area | Repository location | Evidence scope |
|---|---|---|
| Simulation entry point | `src/main.cpp`, `src/SimulationEngine.cpp` | Application entry, configuration-driven simulation and graceful stop |
| Vehicle generation | `vehicle/VehicleGenerator.h/.cpp` | Seeded per-lane generation and configuration validation |
| Vehicle and queues | `vehicle/Vehicle.h/.cpp`, `include/LaneQueue.h`, `src/LaneQueue.cpp` | Vehicle metadata, bounded FIFO queue and overflow behavior |
| Signals and intersections | `include/TrafficSignalController.h`, `src/TrafficSignalController.cpp`, `include/Intersection.h`, `src/Intersection.cpp` | Signal phases, lane registration and intersection behavior |
| Statistics / console | `include/StatisticsCollector.h`, `src/StatisticsCollector.cpp`, `src/ConsoleView.cpp` | Metrics and console output |
| Automated tests | `tests/test_traffic_management.cpp` | Unit/integration assertions; see Unit Test Report and RTM for limitations |
| Performance | `benchmarks/performance_benchmark.cpp` | Local four-independent-intersection benchmark fixture |
| Build / CI | `Makefile`, `.github/workflows/ci-cd.yml` | C++17 build, Ubuntu/Windows tests, Linux sanitizer |

## Requirement-to-evidence mapping
The detailed status and limitations are maintained in `Documents/RTM_Implementation_Status.md`. The following is a concise index, not an assertion that every requirement has passed acceptance.

| Requirement group | Intended behavior | Evidence | Remaining verification |
|---|---|---|---|
| TMS-F-001–003 | Vehicle arrivals, metadata and FIFO handling | Generator and queue tests in `tests/test_traffic_management.cpp` | Explicit unique-ID and all detailed positive/negative cases |
| TMS-F-010–013 | Lane queues, capacity, green-only dequeue and waiting time | Queue capacity/FIFO/overflow, signal and statistics tests | Direct engine-level green-only dequeue proof if required by case mapping |
| TMS-F-020–024 | Fixed/adaptive signals and safe phases | Signal transition tests; PR #90 80-tick at-most-one-GREEN-lane invariant; PR #98 adaptive extension cap/reset/invalid-boundary tests (CI #141) | Geometric movement-conflict matrix; full adaptive policy validation beyond tested boundaries |
| TMS-F-030–032 | Emergency priority and preemption | Emergency request acceptance/rejection/deferred activation tests | Maximum emergency wait/preemption delay boundary |
| TMS-F-040–042 | Multi-intersection transfer and reporting | Two-intersection transfer test; PR #99 destination-full backpressure test (CI #142); config startup smoke tests | Exact one-tick latency and invalid-intersection isolation |
| TMS-F-050–052 | Statistics and reporting | Statistics calculation and report-format tests | End-to-end report workflow |
| TMS-F-060 | Configuration and run length | PR #91 configuration boundary tests; application smoke tests | Full Test Plan case-by-case execution and SRS/Test Plan conflict resolution |
| TMS-F-061 | Graceful start/stop and summary | PR #94 merged; workflow #132 passed Ubuntu/Windows; Linux ASan/UBSan passed | Manual interactive Ctrl+C acceptance record |
| TMS-NF-001 | Performance | Local benchmark average 84,371.19 / median 88,344.30 ticks/sec | Connected topology and evaluator hardware |
| TMS-NF-002 / PRJ-SR-003 | Memory safety and long-run stability | 10,000-tick run and Linux ASan/UBSan | Repeated-run memory-growth/leak report |
| TMS-NF-003–005 | Modularity, usability and portability | Config-driven topology; PR #102 long lane/high queue console assertions (CI #144); Ubuntu/Windows CI | Manual visual usability and clean-checkout verification |
| PRJ-SR-001–005 | Bounds, validation, RAII and robustness | Invalid-input tests, queue overflow tests, sanitizers | Full boundary/failure-injection cases |

## Latest regression additions
- PR #98 adaptive timing boundaries: CI run #141, merged.
- PR #99 transfer backpressure: CI run #142, merged.
- PR #101 emergency queue priority/order: CI run #143, merged.
- PR #102 console long-lane/high-queue output: CI run #144, merged.
- Post-merge main CI run #149 passed Ubuntu/Windows builds, tests and smoke tests; Ubuntu ASan/UBSan passed.

## Evidence rules
1. Link each requirement to an actual source file and named test case.
2. Record PASS, FAIL, BLOCKED or NOT RUN; missing evidence is not PASS.
3. Record commit SHA, date, compiler, operating system and command for each execution.
4. Do not equate 436 automated assertions with the 66 Test Plan cases.
5. Coverage of a subset of source files is not whole-system coverage.
6. Update this matrix after implementation changes and independent retesting.

## Known acceptance gaps
See `Documents/RTM_Implementation_Status.md` and `docs/SUBMISSION_CHECKLIST.md` for the current requirement-level list. Overall system acceptance remains PARTIAL.
