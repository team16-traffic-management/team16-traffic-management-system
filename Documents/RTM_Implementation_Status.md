# SRS / SAD / Test Plan Implementation Status (RTM)

**Updated 16-11-2026 after Part 3 testing and Sprint 3.** Every requirement is implemented and both of its Test Plan v1.2 cases have been executed. All 33 requirements are accepted. The Linux-only verdicts come from [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798).

Case-level results: `docs/TEST_EXECUTION_REGISTER.md`. Reports: `docs/QA_Reports/`. Sprint 3 changes: `docs/SPRINT_3.md`.

## Full requirement-level RTM

**Status convention:** ACCEPTED = both Test Plan cases executed and passed on the final build.

| Requirement | Implementation component(s) | Test Plan cases | Executable checks (acceptance runner / script) | Acceptance status |
|---|---|---|---|---|
| TMS-F-001 | VehicleGenerator | TC-Gen-001-P/N | `genSpawnRate`; `genZeroRate` | **ACCEPTED (both cases PASS)** |
| TMS-F-002 | Vehicle / VehicleGenerator | TC-Gen-002-P/N | `genIdTickType`; `genSameTickUnique` | **ACCEPTED (both cases PASS)** |
| TMS-F-003 | LaneQueue | TC-Gen-003-P/N | `genFifo`; `genFifoWithEmergency` | **ACCEPTED (both cases PASS)** |
| TMS-F-010 | Intersection / LaneQueue | TC-Queue-001-P/N | `queueIndependent`; `queueSiblingIntact` | **ACCEPTED (both cases PASS)** |
| TMS-F-011 | LaneQueue / StatisticsCollector | TC-Queue-002-P/N | `queueCapacityEnforced`; `queueZeroCapacity` | **ACCEPTED (both cases PASS)** |
| TMS-F-012 | SimulationEngine / SignalController | TC-Queue-003-P/N | `queueDequeueOnlyGreen`; `queueEmptyGreen` | **ACCEPTED (both cases PASS)** |
| TMS-F-013 | StatisticsCollector | TC-Queue-004-P/N | `queueWaitTime`; `queueImmediateWait` | **ACCEPTED (both cases PASS)** |
| TMS-F-020 | SignalController | TC-Sig-001-P/N | `sigFixedCycle`; `sigZeroDuration` | **ACCEPTED (both cases PASS)** |
| TMS-F-021 | SignalController | TC-Sig-002-P/N | `sigAdaptiveExtends`; `sigAdaptiveCapped` | **ACCEPTED (both cases PASS)** |
| TMS-F-022 | SignalController / Intersection | TC-Sig-003-P/N | `sigNoConflictingGreen`; `sigForcedConflict` | **ACCEPTED (both cases PASS)** |
| TMS-F-023 | SignalController / ConsoleView | TC-Sig-004-P/N | `sigYellowBeforeRed`; `sigOneTickYellow` | **ACCEPTED (both cases PASS)** |
| TMS-F-024 | SimulationEngine / SignalController | TC-Sig-005-P/N | `sigModeViaConfig`; `sigInvalidMode` | **ACCEPTED (both cases PASS)** |
| TMS-F-030 | VehicleGenerator / Vehicle | TC-Emg-001-P/N | `emgSpawnProbability`; `emgZeroProbability` | **ACCEPTED (both cases PASS)** |
| TMS-F-031 | LaneQueue | TC-Emg-002-P/N | `emgPriority`; `emgFifoAmongEmergency` | **ACCEPTED (both cases PASS)** |
| TMS-F-032 | SignalController / SimulationEngine | TC-Emg-003-P/N | `emgPreemptWithinDelay`; `emgPreemptNoConflict` | **ACCEPTED (both cases PASS)** |
| TMS-F-040 | SimulationEngine / Intersection | TC-Multi-001-P/N | `multiIndependent`; `multiFaultIsolation` | **ACCEPTED (both cases PASS)** |
| TMS-F-041 | SimulationEngine / PendingTransfer | TC-Multi-002-P/N | `multiGreenWave`; `multiFullDownstream` | **ACCEPTED (both cases PASS)** |
| TMS-F-042 | StatisticsCollector / SimulationEngine | TC-Multi-003-P/N | `multiStatsSum`; `multiZeroThroughput` | **ACCEPTED (both cases PASS)** |
| TMS-F-050 | StatisticsCollector | TC-Stats-001-P/N | `statsAverage`; `statsAverageNoData` | **ACCEPTED (both cases PASS)** |
| TMS-F-051 | StatisticsCollector | TC-Stats-002-P/N | `statsMaxQueue`; `statsMaxQueueOne` | **ACCEPTED (both cases PASS)** |
| TMS-F-052 | StatisticsCollector | TC-Stats-003-P/N | `statsThroughputCount`; `statsBlockedNotThroughput` | **ACCEPTED (both cases PASS)** |
| TMS-F-060 | SimulationEngine::loadConfigFromFile | TC-Sim-001-P/N | `simConfigApplied`; `simMissingConfig` | **ACCEPTED (both cases PASS)** |
| TMS-F-061 | main / SimulationEngine | TC-Sim-002-P/N | `simNaturalCompletion`; run_acceptance.sh (real Ctrl+C) | **ACCEPTED (both cases PASS)** |
| TMS-NF-001 | SimulationEngine / ConsoleView | TC-Perf-01-P/N | `perfTarget`; `perfMaxLoad` | **ACCEPTED (both cases PASS)** |
| TMS-NF-002 | SimulationEngine / RAII containers | TC-Rel-01-P/N | `relLongRun`; `relRepeatedRuns` | **ACCEPTED (both cases PASS)** |
| TMS-NF-003 | SimulationEngine and domain modules | TC-Maint-01-P/N | `maintNewTopology`; `maintInvalidTopology` | **ACCEPTED (both cases PASS)** |
| TMS-NF-004 | ConsoleView | TC-UX-01-P/N | `uxReadable`; `uxLongName` | **ACCEPTED (both cases PASS)** |
| TMS-NF-005 | Makefile / GitHub Actions | TC-Port-01-P/N | run_acceptance.sh + CI portability-compare; run_acceptance.sh (missing make) | **ACCEPTED (both cases PASS)** |
| PRJ-SR-001 | LaneQueue / Intersection / SimulationEngine | TC-Rob-001-P/N | ASan/UBSan (Linux CI); `robInvalidIndex` | **ACCEPTED (both cases PASS)** |
| PRJ-SR-002 | SimulationEngine::loadConfigFromFile | TC-Rob-002-P/N | `robInvalidValues`; `robWrongType` | **ACCEPTED (both cases PASS)** |
| PRJ-SR-003 | RAII / standard containers | TC-Rob-003-P/N | `robSustainedRun`; `robMidRunException` | **ACCEPTED (both cases PASS)** |
| PRJ-SR-004 | LaneQueue / StatisticsCollector | TC-Rob-004-P/N | `robOverflow`; `robOverflowEveryLane` | **ACCEPTED (both cases PASS)** |
| PRJ-SR-005 | SimulationEngine / main | TC-Rob-005-P/N | `robMalformedInput`; `robModuleFault` | **ACCEPTED (both cases PASS)** |

## Requirement gaps closed in Sprint 3

| Requirement | Gap before Part 3 | Closed by |
|---|---|---|
| TMS-F-032 | No maximum preemption delay; worst case 22 ticks | Bounded preemption sequence and `max_preemption_delay` (DEF-01) |
| TMS-F-020 / TMS-F-060 | Signal durations not configurable from file | `green_duration`, `yellow_duration`, `red_duration` (DEF-02) |
| TMS-F-021 | Adaptive base hard-coded; maximum not configurable | Configured base and `max_green_extension` (DEF-03) |
| TMS-NF-004 | Long lane names misaligned columns | Dynamic column widths (DEF-04) |
| TMS-NF-001 / PRJ-SR-004 | Unbounded in-flight transfers under chained overload | Spillback (DEF-05) |
| TMS-F-042 / TMS-F-052 | No per-intersection totals | Per-intersection totals and `SYSTEM TOTAL` row (DEF-06) |

## Items that were open before Part 3 and how they were resolved

| Previously open item | Resolution |
|---|---|
| Execute the 66 Test Plan cases individually | Done: 66 PASS, 0 FAIL |
| Manual Ctrl+C run (TMS-F-061) | Executed with a genuine console Ctrl+C (TC-Sim-002-N): exit 0, summary ticks = rendered ticks |
| Geometric movement conflicts (TMS-F-022) | The controller grants Green to one lane at a time, which is stricter than conflict-free sets. TC-Sig-003-P/N verify no simultaneous Greens, including under forced same-tick overrides. Turning-movement modelling is outside the implemented design (documented deviation). |
| Invalid-intersection isolation (TMS-F-040) | Fail-fast rejection of invalid definitions plus verified runtime isolation (TC-Multi-001-N) |
| Emergency preemption delay (TMS-F-032) | Implemented and verified (DEF-01; TC-Emg-003-P/N) |
| Quantified leak evidence (TMS-NF-002 / PRJ-SR-003) | Valgrind: 0 bytes in use at exit, 0 errors; LeakSanitizer: no leaks (CI run #246) |
| Manual console usability (TMS-NF-004) | Automated alignment checks over 30 rendered ticks and a 29-character lane name, plus DEF-04 fix |
| Connected-topology benchmark (TMS-NF-001) | TC-Perf-01-N: 8 chained intersections at 0.95 arrival, 10,222 ticks/s |
| SRS/Test Plan conflict on invalid config | Resolved in Test Plan v1.1/v1.2: rejection with a clear error, matching PRJ-SR-002 |

## Earlier CI evidence (history)

- Baseline main run #99 (`f8c6cfd`): 436 checks on Ubuntu and Windows; ASan/UBSan passed.
- PR #94 graceful stop (run #132), merged as `339dfed`.
- PRs #98–#108 (runs #141–#162): adaptive bounds, transfer backpressure, emergency ordering, console output, ID uniqueness, green-only dequeue, one-tick transfer, repeated lifecycle.
- PRs #115–#123: boundary and diagnostic regressions. Main run #243 reported 985 checks; coverage 88.0% lines and 54.8% branches.
