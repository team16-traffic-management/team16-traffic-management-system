# Requirements Traceability Matrix — Traffic Management System

Connects each requirement group to its implementation, its Test Plan cases, and the execution result. The full 33-row matrix is in `Documents/RTM_Implementation_Status.md`; case-level results are in `docs/TEST_EXECUTION_REGISTER.md`.

## Source and test inventory

| Area | Repository location | Evidence scope |
|---|---|---|
| Simulation entry point | `src/main.cpp`, `src/SimulationEngine.cpp` | Configuration, tick loop, transfers with spillback, graceful stop |
| Vehicle generation | `vehicle/VehicleGenerator.h/.cpp` | Seeded per-lane arrivals, emergency probability |
| Vehicles and queues | `vehicle/Vehicle.h/.cpp`, `include/LaneQueue.h`, `src/LaneQueue.cpp` | Metadata, bounded FIFO with emergency priority |
| Signals and intersections | `include/TrafficSignalController.h`, `src/TrafficSignalController.cpp`, `include/Intersection.h`, `src/Intersection.cpp` | Configurable phases, adaptive mode, bounded preemption |
| Statistics and console | `include/StatisticsCollector.h`, `src/StatisticsCollector.cpp`, `src/ConsoleView.cpp` | Per-lane, per-intersection and system totals; aligned live view |
| Unit tests | `tests/test_traffic_management.cpp` | 42 functions, 1,198 checks |
| Test Plan acceptance | `tests/acceptance/test_plan_acceptance.cpp`, `tests/acceptance/send_ctrl_c.cpp`, `scripts/run_acceptance.sh` | One executable check per Test Plan case (66) |
| Coverage | `scripts/run_coverage.sh` | Line, function and branch coverage of all production sources |
| Performance | `benchmarks/performance_benchmark.cpp` | 4-intersection benchmark |
| Build / CI | `Makefile`, `.github/workflows/ci-cd.yml` | Ubuntu and Windows builds, unit and acceptance suites, sanitizers, Valgrind, coverage, cross-platform output diff |

## Requirement-to-evidence mapping

| Requirement group | Intended behaviour | Test Plan cases | Result (16-11-2026) |
|---|---|---|---|
| TMS-F-001–003 | Vehicle arrivals, metadata, FIFO | TC-Gen-001…003-P/N | 6/6 PASS |
| TMS-F-010–013 | Lane queues, capacity, green-only dequeue, wait time | TC-Queue-001…004-P/N | 8/8 PASS |
| TMS-F-020–024 | Fixed/adaptive signals, no conflicts, Yellow, mode via config | TC-Sig-001…005-P/N | 10/10 PASS |
| TMS-F-030–032 | Emergency probability, priority, bounded preemption | TC-Emg-001…003-P/N | 6/6 PASS |
| TMS-F-040–042 | Multiple intersections, green-wave link, per-intersection stats | TC-Multi-001…003-P/N | 6/6 PASS |
| TMS-F-050–052 | Average wait, max queue, throughput | TC-Stats-001…003-P/N | 6/6 PASS |
| TMS-F-060–061 | Configuration; start/stop with summary (real Ctrl+C) | TC-Sim-001…002-P/N | 4/4 PASS |
| TMS-NF-001 | Performance ≥ 500 ticks/s, graceful under load | TC-Perf-01-P/N | 2/2 PASS |
| TMS-NF-002 | 10,000+ ticks without crash or leak | TC-Rel-01-P/N | 2/2 PASS (Valgrind/LSan in CI) |
| TMS-NF-003 | Topology via config only | TC-Maint-01-P/N | 2/2 PASS |
| TMS-NF-004 | Readable console | TC-UX-01-P/N | 2/2 PASS |
| TMS-NF-005 | Linux and Windows | TC-Port-01-P/N | 2/2 PASS (identical Linux/Windows output in CI) |
| PRJ-SR-001 | Bounds checking | TC-Rob-001-P/N | 2/2 PASS (ASan/UBSan in CI) |
| PRJ-SR-002 | Config validation | TC-Rob-002-P/N | 2/2 PASS |
| PRJ-SR-003 | RAII, zero leaks | TC-Rob-003-P/N | 2/2 PASS (Valgrind/LSan in CI) |
| PRJ-SR-004 | Overflow handling | TC-Rob-004-P/N | 2/2 PASS |
| PRJ-SR-005 | No abnormal termination | TC-Rob-005-P/N | 2/2 PASS |

**Totals:** 66/66 executed; 66 PASS, 0 FAIL. Linux-only verdicts: [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798).

## Evidence rules

1. Link each requirement to a source file and to the named check that executes each of its Test Plan cases.
2. Record PASS, FAIL, BLOCKED or NOT RUN; missing evidence is never PASS.
3. Record the date, compiler, operating system and command for each execution (`docs/QA_Reports/evidence/environment_*.txt`).
4. Unit-check counts support the Test Plan cases but are not the same thing; each case is reported on its own.
5. Re-run `scripts/run_acceptance.sh` and update this matrix after any implementation change.
