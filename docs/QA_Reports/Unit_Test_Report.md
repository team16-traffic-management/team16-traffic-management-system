# Unit Test Report

**Status: PASS.** 42 unit test functions and 1,198 checks passed, 0 failed (local Windows run on the Sprint 3 code, 16-11-2026). The same suite runs on Ubuntu and Windows, and under ASan/UBSan, in GitHub Actions on every push.

## Test environment

| Field | Value |
|---|---|
| Code under test | Sprint 3 baseline (`docs/SPRINT_3.md`) |
| Test source | `tests/test_traffic_management.cpp` (consolidated harness, real production classes, no mocks) |
| Local platform | Windows 11, MSYS2 UCRT64 `g++ 16.2.0`, `-std=c++17 -Wall -Wextra -Wpedantic -pthread` |
| CI platforms | GitHub Actions `ubuntu-latest` and `windows-latest`; Linux also runs the suite under AddressSanitizer + UBSan (`make sanitize`) |
| Executed | 16-11-2026 |
| Raw log | `docs/QA_Reports/evidence/unit_tests_windows.txt` |

## How to run

```bash
make test                      # build and run the unit suite
make sanitize                  # Linux: unit suite + acceptance runner under ASan/UBSan
bash scripts/run_acceptance.sh # also records the unit results in docs/QA_Reports/evidence
```

Each test function prints `[PASS] <name> (<n> checks)`, and the run ends with `All 1198 checks passed.` The first failing check stops the run with `FAIL: <message>` and a non-zero exit code.

## Results by unit

| Unit | Test functions | Checks | Failed | Requirements exercised |
|---|---:|---:|---:|---|
| Vehicle / VehicleGenerator | 7 | 711 | 0 | TMS-F-001, F-002, F-030, PRJ-SR-002 |
| LaneQueue | 4 | 31 | 0 | TMS-F-003, F-010, F-011, F-031, PRJ-SR-004 |
| SignalController | 7 | 306 | 0 | TMS-F-020 to F-024, F-032 |
| StatisticsCollector | 3 | 14 | 0 | TMS-F-013, F-042, F-050 to F-052 |
| ConsoleView | 3 | 35 | 0 | TMS-NF-004 |
| Intersection | 1 | 8 | 0 | TMS-F-010, PRJ-SR-002 |
| SimulationEngine and configuration (integration level) | 17 | 93 | 0 | TMS-F-012, F-024, F-032, F-040 to F-042, F-052, F-060, F-061, NF-001 to NF-003, PRJ-SR-002, -004, -005 |
| **Total** | **42** | **1,198** | **0** | |

## Results by test function

| # | Test function | Unit | Checks | Result |
|---:|---|---|---:|---|
| 1 | testProbabilityBoundaries | VehicleGenerator | 300 | PASS |
| 2 | testVehicleIdsAreUniqueAcrossLanesAndIntersections | VehicleGenerator | 201 | PASS |
| 3 | testArrivalRates | VehicleGenerator | 3 | PASS |
| 4 | testConfigurationUpdates | VehicleGenerator | 3 | PASS |
| 5 | testInvalidConfiguration | VehicleGenerator | 4 | PASS |
| 6 | testEmergencyQueuePriorityAndStableOrdering | LaneQueue | 8 | PASS |
| 7 | testQueueCapacityAndEmptyDequeue | LaneQueue | 5 | PASS |
| 8 | testMalformedConfiguration | Engine/config | 1 | PASS |
| 9 | testMissingConfigurationFileRejected | Engine/config | 1 | PASS |
| 10 | testConfigurationValidationBoundaries | Engine/config | 15 | PASS |
| 11 | testEngineDequeuesVehiclesOnlyOnGreen | Engine | 3 | PASS |
| 12 | testConfigDrivenIntersections | Engine/config | 4 | PASS |
| 13 | testConnectedVehicleTransferTakesOneTick | Engine | 4 | PASS |
| 14 | testPendingTransferWaitsForDestinationCapacity | Engine | 2 | PASS |
| 15 | testConsoleLongLaneAndHighQueueDisplay | ConsoleView | 28 | PASS |
| 16 | testConsoleStatusFields | ConsoleView | 4 | PASS |
| 17 | testRepeatedSimulationLifecycle | Engine | 21 | PASS |
| 18 | testTenThousandTickStability | Engine | 2 | PASS |
| 19 | testZeroTickConfigurationMeansOpenEndedRun | Engine/config | 1 | PASS |
| 20 | testManualStopProducesPartialSummary | Engine | 4 | PASS |
| 21 | testSigintStopsOpenEndedRun | Engine | 3 | PASS |
| 22 | testSignalPhaseTransitions | SignalController | 8 | PASS |
| 23 | testNoSimultaneousGreenSignals | SignalController | 80 | PASS |
| 24 | testAdaptiveGreenExtensionBounds *(rewritten in Sprint 3)* | SignalController | 17 | PASS |
| 25 | testAdaptiveModeCanBeChangedDuringSignalOperation | SignalController | 5 | PASS |
| 26 | testAdaptiveModeLoadedFromConfiguration | Engine/config | 4 | PASS |
| 27 | testEmergencyPreemption | SignalController | 5 | PASS |
| 28 | testStatisticsCalculations | StatisticsCollector | 9 | PASS |
| 29 | testMaxQueueLengthOneBoundary | StatisticsCollector | 2 | PASS |
| 30 | testZeroThroughputIntersectionAppearsInAggregateReport | StatisticsCollector | 3 | PASS |
| 31 | testSimultaneousOverflowAcrossEveryLane | LaneQueue | 12 | PASS |
| 32 | testIntersectionValidationAndSignalRegistration | Intersection | 8 | PASS |
| 33 | testSiblingLaneQueueIsolation | LaneQueue | 6 | PASS |
| 34 | testZeroEmergencyProbabilityProducesNormalVehicles | VehicleGenerator | 100 | PASS |
| 35 | testEmergencyProbabilityOneAlwaysGeneratesEmergencyVehicles | VehicleGenerator | 100 | PASS |
| 36 | testBoundedEmergencyPreemptionDelay *(new)* | SignalController | 184 | PASS |
| 37 | testPreemptionDelayConfigurationBounds *(new)* | SignalController | 7 | PASS |
| 38 | testEnginePreemptsWithinConfiguredDelay *(new)* | Engine | 5 | PASS |
| 39 | testSignalTimingsLoadedFromConfiguration *(new)* | Engine/config | 14 | PASS |
| 40 | testPerIntersectionTotalsSumToAggregate *(new)* | Engine/StatisticsCollector | 6 | PASS |
| 41 | testConsoleColumnsStayAlignedWithLongLaneNames *(new)* | ConsoleView | 3 | PASS |
| 42 | testPendingTransfersBoundedByDownstreamCapacity *(new)* | Engine | 3 | PASS |

## Changes to the unit suite in Sprint 3

| Change | Reason |
|---|---|
| 7 new test functions (#36–#42) | Cover the Sprint 3 fixes DEF-01 to DEF-06 (`docs/SPRINT_3.md`). |
| `testAdaptiveGreenExtensionBounds` rewritten | The old test encoded the defect (adaptive cap = 10 + extension whatever the configured Green). It now asserts cap = configured base + extension, restoration of the configured base, and the cap for an extreme queue of 50. |
| `testPendingTransferWaitsForDestinationCapacity` comment updated | Same assertions; the vehicle is now held upstream (spillback) instead of in a hidden pending list. |
| Per-test result lines | `RUN_TEST` prints each function's name and check count for this report; the final `All N checks passed.` line is unchanged. |

**Regression check for the defect fixes:** `testConsoleColumnsStayAlignedWithLongLaneNames` was also run against the original `ConsoleView.cpp` and failed (`FAIL: signal and queue columns stay aligned when one lane name is very long`), which confirms that it detects DEF-04. The other new tests call APIs added in Sprint 3, so they cannot be compiled against the old code. Their before-fix evidence is the measurements in `docs/SPRINT_3.md`: 22-tick worst-case preemption and 2,051 in-flight transfers.

## Earlier CI history (pre-Sprint 3)

| Run | Scope | Result |
|---|---|---|
| [Main run #99](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604) | Baseline suite, commit `f8c6cfd` | 436 checks passed on Ubuntu and Windows; ASan/UBSan passed |
| [PR #94 run #132](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865) | Graceful stop / SIGINT | Ubuntu and Windows passed; ASan/UBSan passed |
| PRs #98–#108 (runs #141–#162) | Adaptive bounds, transfer backpressure, emergency ordering, console output, ID uniqueness, green-only dequeue, one-tick transfer, repeated lifecycle | All passed on Ubuntu and Windows with ASan/UBSan |
| PRs #115–#123 | Sibling isolation, emergency probability boundaries, overflow and statistics boundaries, adaptive config, missing-config and missing-compiler diagnostics | All passed; [main run #243](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43382805876) reported 985 checks |

The jump from 985 to 1,198 checks comes from the 7 new Sprint 3 tests (+222 checks) minus the shorter rewritten adaptive test (−9 checks).

## Limitations

- Unit checks confirm the behaviour of individual classes and small integrations. Requirement-level acceptance is reported separately in `System_Validation_Report.md` and `docs/TEST_EXECUTION_REGISTER.md`.
- The Ubuntu run and the sanitizer run of this exact code happen in GitHub Actions on push. Record that run's link here once it finishes.
