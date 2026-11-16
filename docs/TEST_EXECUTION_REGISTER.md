# Test Execution Register — Traffic Management System

**Source:** Section 13 of `Documents/Test_Plan_Traffic_Management_System.docx` (v1.2, 16-11-2026).
**Purpose:** Record the execution result of each of the 66 Test Plan cases individually.

## Execution record

| Field | Value |
|---|---|
| Execution date | 16-11-2026 |
| Local environment | Windows 11, MSYS2 UCRT64 `g++ 16.2.0`, `-std=c++17 -O2` |
| Linux environment | GitHub Actions `ubuntu-latest`, g++ 15.2.0 (ASan/UBSan/LeakSanitizer, Valgrind) — [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798), 16-11-2026 19:22 UTC |
| Code under test | Sprint 3 baseline (see `docs/SPRINT_3.md`) |
| Command | `bash scripts/run_acceptance.sh` (or `make acceptance-full`) |
| In-process runner | `tests/acceptance/test_plan_acceptance.cpp` — one function per TC ID, 62 cases |
| Process-level cases | `scripts/run_acceptance.sh` — CLI exit codes, real Ctrl+C, missing build tool, config-only topology, memory tools |
| Raw evidence | `docs/QA_Reports/evidence/acceptance_results_windows.txt` and the other files in that folder |

## Status rules

- `PASS` — executed against the recorded build; actual result matched the v1.2 expected result.
- `FAIL` — executed; actual result did not match. Logged as a defect (see `docs/SPRINT_3.md`).
- Six cases (TC-Rob-001-P, TC-Rel-01-P/N, TC-Rob-003-P/N, TC-Port-01-P) need Linux-only tools or the Linux build; their verdicts come from [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798).
- `BLOCKED` / `NOT RUN` — not executed.
- Expected results are those of Test Plan v1.2. Cases whose expectations changed during the sprints are listed in the "Change in v1.2" rows of the Test Plan and in `docs/SPRINT_3.md`.

## Defects found during execution

Execution of the first full run found two failures, both fixed in Sprint 3 and re-executed:

| Case | First result | Defect | Fix |
|---|---|---|---|
| TC-Perf-01-N | FAIL — 8 chained intersections ran at 4% of baseline throughput | Transfers left a lane even when the downstream lane was full, so an unbounded in-flight queue grew (2,051 vehicles after 10,000 ticks) and was rescanned every tick | Spillback: a linked vehicle leaves only if the downstream lane has room (DEF-05) |
| TC-UX-01-N | FAIL (new alignment check) — a 29-character lane name shifted the Signal/Queue columns | Fixed 14-character lane column | Lane column sized to the longest lane name (DEF-04) |

Requirement gaps closed before execution (TMS-F-032 preemption delay, configurable signal timings, adaptive baseline, per-intersection totals) are listed in `docs/SPRINT_3.md`.

## Case register

| Test case ID | Requirement | Test case | Owner per Test Plan | Status | Executed by | Actual result / evidence |
|---|---|---|---|---|---|---|
| TC-Gen-001-P | TMS-F-001 | Vehicles spawn at configured arrival rate | Tarun Gowda P | **PASS** | `genSpawnRate` | North=293 South=307 East=297 West=298 (expected 300 +/-10%) |
| TC-Gen-001-N | TMS-F-001 | Zero arrival rate produces zero vehicles | Tarun Gowda P | **PASS** | `genZeroRate` | 500 ticks: throughput=0 blocked=0 queued=0 |
| TC-Gen-002-P | TMS-F-002 | Unique vehicle ID, arrival tick and valid type | Tarun Gowda P | **PASS** | `genIdTickType` | 69 vehicles in 50 ticks, 69 unique IDs |
| TC-Gen-002-N | TMS-F-002 | Same-tick spawns across lanes retain unique IDs | Tarun Gowda P | **PASS** | `genSameTickUnique` | tick 1: 4 same-tick spawns, IDs 1001 1002 1003 1004 |
| TC-Gen-003-P | TMS-F-003 | Vehicles enqueue in FIFO order | Tarun Gowda P | **PASS** | `genFifo` | dequeue order: 101 102 103 104 105 |
| TC-Gen-003-N | TMS-F-003 | Emergency priority does not reverse FIFO order among normal vehicles | Tarun Gowda P | **PASS** | `genFifoWithEmergency` | N1,N2,E9,N3 enqueued -> dequeued 9 1 2 3 |
| TC-Queue-001-P | TMS-F-010 | Each lane maintains independent queue state | Tejas Shetty | **PASS** | `queueIndependent` | North=3 South=0 East=0 West=0 |
| TC-Queue-001-N | TMS-F-010 | Operating on one lane does not corrupt sibling lane | Tejas Shetty | **PASS** | `queueSiblingIntact` | after draining North, South holds 2 vehicles: 3,4 |
| TC-Queue-002-P | TMS-F-011 | Capacity limit is enforced | Tejas Shetty | **PASS** | `queueCapacityEnforced` | accept pattern 11100; engine: Target size=3 blocked=2 |
| TC-Queue-002-N | TMS-F-011 | Zero-capacity lane rejects every vehicle | Tejas Shetty | **PASS** | `queueZeroCapacity` | LaneQueue: "Lane capacity must be greater than zero"; config: "queue_capacity must be between 1 and 10000" |
| TC-Queue-003-P | TMS-F-012 | Dequeue only happens while Green | Tejas Shetty | **PASS** | `queueDequeueOnlyGreen` | 22 Red/Yellow ticks with 2 queued: 0 dequeues; first Green at tick 23 |
| TC-Queue-003-N | TMS-F-012 | Empty Green lane dequeue attempt does not error | Tejas Shetty | **PASS** | `queueEmptyGreen` | 5 ticks with empty Green lane North: throughput=0 exception=no |
| TC-Queue-004-P | TMS-F-013 | Wait time is calculated correctly | Tejas Shetty | **PASS** | `queueWaitTime` | arrival 5, dequeue 12 -> recorded wait 7 |
| TC-Queue-004-N | TMS-F-013 | Immediate dequeue wait time is zero | Tejas Shetty | **PASS** | `queueImmediateWait` | tick 1: throughput=1 average wait=0 |
| TC-Sig-001-P | TMS-F-020 | Fixed-timer cycle follows configured durations | Tejas Shetty | **PASS** | `sigFixedCycle` | North phase changes at ticks 10 13; South Green at tick 23: yes |
| TC-Sig-001-N | TMS-F-020 | Zero-duration phase is rejected (cycle cannot freeze) | Tejas Shetty | **PASS** | `sigZeroDuration` | yellow_duration=0 -> "yellow_duration must be an integer between 1 and 10000" |
| TC-Sig-002-P | TMS-F-021 | Adaptive mode extends Green for longer queues | Tejas Shetty | **PASS** | `sigAdaptiveExtends` | queue=8: Green lasted 20 ticks (base 10, max 20); config max_green_extension=10 |
| TC-Sig-002-N | TMS-F-021 | Adaptive extension is capped for extreme queue | Tejas Shetty | **PASS** | `sigAdaptiveCapped` | queue=50: Green lasted 20 ticks (max 20) |
| TC-Sig-003-P | TMS-F-022 | Conflicting directions are never simultaneously Green | Tejas Shetty | **PASS** | `sigNoConflictingGreen` | 100 ticks logged: N/S+E/W conflicts=0, ticks with >1 Green=0, ticks with a Green=53, preemptions=5 |
| TC-Sig-003-N | TMS-F-022 | Rapid config changes cannot force conflicting state | Tejas Shetty | **PASS** | `sigForcedConflict` | 200 ticks x 2 simultaneous override requests: accepted=40 rejected=360 multi-Green ticks=0 |
| TC-Sig-004-P | TMS-F-023 | Yellow phase occurs before Red | Tejas Shetty | **PASS** | `sigYellowBeforeRed` | Yellow ticks between Green and Red: 3 |
| TC-Sig-004-N | TMS-F-023 | One-tick Yellow duration renders correctly | Tejas Shetty | **PASS** | `sigOneTickYellow` | Yellow=1: Yellow ticks=1, Green->Yellow->Red sequence observed, console shows YELLOW: yes |
| TC-Sig-005-P | TMS-F-024 | Signal mode switches via config alone | Tejas Shetty | **PASS** | `sigModeViaConfig` | adaptive_mode=false -> adaptive 0; adaptive_mode=true -> adaptive 1 (same binary) |
| TC-Sig-005-N | TMS-F-024 | Invalid adaptive_mode is rejected with a clear error | Tejas Shetty | **PASS** | `sigInvalidMode` | adaptive_mode=addaptive -> "adaptive_mode must be true or false" |
| TC-Emg-001-P | TMS-F-030 | Emergency vehicles spawn at configured probability | Tejas Shetty | **PASS** | `emgSpawnProbability` | 188 of 4000 vehicles EMERGENCY (4.70%, expected 5% +/-1.5%) |
| TC-Emg-001-N | TMS-F-030 | Zero emergency probability produces no emergency vehicles | Tejas Shetty | **PASS** | `emgZeroProbability` | 500 ticks, throughput=217, preemptions=0, generator: 0/500 emergency |
| TC-Emg-002-P | TMS-F-031 | Emergency vehicle dequeued ahead of normal vehicles | Tejas Shetty | **PASS** | `emgPriority` | 3 normal then emergency 99 -> first dequeued 99 |
| TC-Emg-002-N | TMS-F-031 | Emergency vehicles preserve FIFO among themselves | Tejas Shetty | **PASS** | `emgFifoAmongEmergency` | A(1) then B(2) -> 1, 2 |
| TC-Emg-003-P | TMS-F-032 | Signal preempts within the configured max delay | Tejas Shetty | **PASS** | `emgPreemptWithinDelay` | emergency in Red East_Bound, preemption logged at tick 1, Green at tick 3 (delay 2, configured max 2) |
| TC-Emg-003-N | TMS-F-032 | Preemption does not create conflicting Green state | Tejas Shetty | **PASS** | `emgPreemptNoConflict` | East/North per tick: YELLOW/RED YELLOW/RED YELLOW/RED RED/RED RED/GREEN RED/GREEN |
| TC-Multi-001-P | TMS-F-040 | Multiple intersections run independently | Tarun Gowda P | **PASS** | `multiIndependent` | 300 ticks: A(rate 0.2): arrived=107 queued=13 processed=94 B(rate 0.5): arrived=305 queued=180 processed=125 C(rate 0.8): arrived=481 queued=351 processed=130 |
| TC-Multi-001-N | TMS-F-040 | Invalid/overloaded intersection does not affect others | Tarun Gowda P | **PASS** | `multiFaultIsolation` | invalid Two config -> "Empty lane name in intersection: Two"; runtime overload of Two: blocked One/Two/Three=0/309/0, throughput One/Three=81/80 |
| TC-Multi-002-P | TMS-F-041 | Green-wave link passes a vehicle downstream | Tarun Gowda P | **PASS** | `multiGreenWave` | vehicle 4242 exits A at tick 1; B.North size after tick 1=0, after tick 2=1 (front id 4242) |
| TC-Multi-002-N | TMS-F-041 | Full downstream lane: handoff deferred upstream (spillback) | Tarun Gowda P | **PASS** | `multiFullDownstream` | B.North full (size 1/1): vehicle held in A=1, in-flight=0, vehicles accounted=2; after B drains: transfers=1, accounted=2 |
| TC-Multi-003-P | TMS-F-042 | Per-intersection and aggregate stats are correct | Tarun Gowda P | **PASS** | `multiStatsSum` | Busy=125 + Quiet=59 = 184; reported aggregate=184 |
| TC-Multi-003-N | TMS-F-042 | Zero-throughput intersection does not break aggregate | Tarun Gowda P | **PASS** | `multiZeroThroughput` | Two: throughput=0 avg wait=0; aggregate=84 == One=84 |
| TC-Stats-001-P | TMS-F-050 | Average wait time is computed correctly | Khushi Gupta | **PASS** | `statsAverage` | waits 4,6,8 -> reported average 6 |
| TC-Stats-001-N | TMS-F-050 | Zero dequeue events do not error | Khushi Gupta | **PASS** | `statsAverageNoData` | 0 events -> average 0, report prints "0.00 ticks": yes |
| TC-Stats-002-P | TMS-F-051 | Maximum queue length is tracked correctly | Khushi Gupta | **PASS** | `statsMaxQueue` | Target peaked at 6, drained to 0; reported max 6 |
| TC-Stats-002-N | TMS-F-051 | Maximum queue length remains correct when queue ≤ 1 | Khushi Gupta | **PASS** | `statsMaxQueueOne` | light traffic, Green=10000: arrivals processed=24, reported max queue=1 |
| TC-Stats-003-P | TMS-F-052 | Throughput count is accurate | Khushi Gupta | **PASS** | `statsThroughputCount` | 200 ticks: replayed arrivals=307, still queued=222, counted dequeues=85, reported throughput=85 |
| TC-Stats-003-N | TMS-F-052 | Blocked/dropped vehicles are not counted as throughput | Khushi Gupta | **PASS** | `statsBlockedNotThroughput` | 5 arrivals at capacity-3 lane: blocked=2, throughput=3 |
| TC-Sim-001-P | TMS-F-060 | Config values are applied correctly | Khushi Gupta | **PASS** | `simConfigApplied` | lanes=4, Green ended at tick 15 (green_duration=15), observed spawn rate 0.509 (0.5) |
| TC-Sim-001-N | TMS-F-060 | Missing config file is rejected with a clear error and non-zero exit | Khushi Gupta | **PASS** | `simMissingConfig` + `run_acceptance.sh` | "Could not open configuration file: tests/data/this_file_does_not_exist.cfg" (CLI exit code checked by run_acceptance.sh); CLI exit 1, stderr: Simulation failed: Could not open configuration file: tests/data/does_not_exist.cfg |
| TC-Sim-002-P | TMS-F-061 | Natural run completion produces clean summary | Khushi Gupta | **PASS** | `simNaturalCompletion` + `run_acceptance.sh` | ended at tick 500; summary header present; termination line present; CLI 500-tick run exit 0; report header and termination line present |
| TC-Sim-002-N | TMS-F-061 | Manually stopped run produces clean summary | Khushi Gupta | **PASS** | `run_acceptance.sh` | real Ctrl+C after 1 s: exit 0, summary for 20420 ticks = 20420 rendered tick blocks |
| TC-Perf-01-P | TMS-NF-001 | Throughput performance target is met | Tarun Gowda P | **PASS** | `perfTarget` | 4 intersections x 4 lanes, 10,000 ticks: 15437 ticks/s (target >= 500) |
| TC-Perf-01-N | TMS-NF-001 | Performance does not collapse under maximum load | Tarun Gowda P | **PASS** | `perfMaxLoad` | 4-intersection baseline 28379 ticks/s; 8 chained intersections @0.95 arrival 10222 ticks/s (ratio 0.36, linear expectation 0.50) |
| TC-Rel-01-P | TMS-NF-002 | No crashes or leaks over a long run | Tarun Gowda P | **PASS** | `relLongRun` + Linux CI | Linux CI run #246: 10,000-tick connected/adaptive run (long_run.cfg) under Valgrind — clean exit, 0 bytes in use at exit, 182,701 allocs / 182,701 frees, ERROR SUMMARY 0 errors (evidence/valgrind_linux.txt). |
| TC-Rel-01-N | TMS-NF-002 | Repeated start/stop cycles do not accumulate leaks | Tarun Gowda P | **PASS** | `relRepeatedRuns` + Linux CI | Linux CI run #246: 10 repeated create/run/destroy cycles — LeakSanitizer reports no leaks; Valgrind over the acceptance runner: all heap blocks freed, 0 errors. |
| TC-Maint-01-P | TMS-NF-003 | New topology can be added via config only | Tarun Gowda P | **PASS** | `maintNewTopology` + `run_acceptance.sh` | 3rd intersection added by config only: intersections=3, transfers=17, Harbor throughput=23; unchanged binary ran three_intersections.cfg: 3 intersection(s), Harbor reported |
| TC-Maint-01-N | TMS-NF-003 | Invalid topology is rejected, not silently accepted | Tarun Gowda P | **PASS** | `maintInvalidTopology` | connection.One=One -> "Invalid configured connection: An intersection cannot connect to itself" |
| TC-UX-01-P | TMS-NF-004 | Console output is readable during a run | Khushi Gupta | **PASS** | `uxReadable` | 30 tick blocks, 150 table rows checked, columns aligned=yes |
| TC-UX-01-N | TMS-NF-004 | Output remains readable with long lane/high queue | Khushi Gupta | **PASS** | `uxLongName` | 29-char lane name, queue 15: 5 rows, columns aligned=yes, 15-char bar=yes |
| TC-Port-01-P | TMS-NF-005 | Build succeeds on Linux and Windows | Tarun Gowda P | **PASS** | `run_acceptance.sh` + Linux CI | Built and run on Windows (local g++ 16.2.0; CI MinGW g++ 15.2.0) and Linux (CI g++ 15.2.0). CI job 'portability-compare' in run #246: baseline.cfg output identical on Linux and Windows (196 lines). |
| TC-Port-01-N | TMS-NF-005 | Missing build dependency fails with clear message | Tarun Gowda P | **PASS** | `run_acceptance.sh` | 'make' absent from PATH -> exit 127: scripts/run_acceptance.sh: line 138: make: command not found |
| TC-Rob-001-P | PRJ-SR-001 | Container access is bounds-checked | Tejas Shetty | **PASS** | — + Linux CI | Linux CI run #246 (Ubuntu, g++ 15.2.0): unit suite (1,198 checks), 60 acceptance cases and a 10,000-tick app run under AddressSanitizer + UBSan — no out-of-bounds or undefined-behaviour reports (evidence/sanitizers_linux.txt). |
| TC-Rob-001-N | PRJ-SR-001 | Invalid index access is caught | Tejas Shetty | **PASS** | `robInvalidIndex` | lane index 10 of 4 -> out_of_range; generator lane 10 -> "Lane index is out of range"; engine -> "Unknown intersection ID: Intersection-10" |
| TC-Rob-002-P | PRJ-SR-002 | Invalid config values are rejected safely | Tejas Shetty | **PASS** | `robInvalidValues` + `run_acceptance.sh` | arrival_rate=-0.5 -> "Value must be a finite number between 0 and 1: arrival_rate"; queue_capacity=0 -> "queue_capacity must be between 1 and 10000"; invalid_values.cfg -> "Value must be a finite number between 0 and 1: arrival_rate"; CLI invalid_values.cfg exit 1: Simulation failed: Value must be a finite number between 0 and 1: arrival_rate |
| TC-Rob-002-N | PRJ-SR-002 | Wrong-type config value does not crash parser | Tejas Shetty | **PASS** | `robWrongType` | arrival_rate=abc -> "Invalid number for: arrival_rate"; total_ticks=ten -> "Invalid total_ticks value" |
| TC-Rob-003-P | PRJ-SR-003 | Zero memory leaks over sustained run | Tejas Shetty | **PASS** | `robSustainedRun` + Linux CI | Linux CI run #246: Valgrind on the 10,000-tick connected run — 'All heap blocks were freed -- no leaks are possible', 0 bytes definitely/indirectly lost, 0 errors. |
| TC-Rob-003-N | PRJ-SR-003 | Mid-run exception still releases memory | Tejas Shetty | **PASS** | `robMidRunException` + Linux CI | Linux CI run #246: exception thrown from a live engine at tick 500 and caught — LeakSanitizer and Valgrind both report no leaks (acceptance runner: 311,327 allocs / 311,327 frees, 0 errors). |
| TC-Rob-004-P | PRJ-SR-004 | Queue overflow is handled safely | Tejas Shetty | **PASS** | `robOverflow` | capacity 3 @0.95 for 200 ticks: Hot blocked=140, Cold blocked=0, Hot size=3 |
| TC-Rob-004-N | PRJ-SR-004 | Simultaneous overflow on every lane is handled | Tejas Shetty | **PASS** | `robOverflowEveryLane` | all 4 lanes capacity 1 @0.95, 50 ticks: blocked North=39 South=35 East=43 West=47 |
| TC-Rob-005-P | PRJ-SR-005 | Malformed input does not cause abnormal termination | Tejas Shetty | **PASS** | `robMalformedInput` + `run_acceptance.sh` | corrupted.cfg -> "Invalid configuration at line 8" (CLI exit code checked by run_acceptance.sh); CLI corrupted.cfg exit 1 (handled): Simulation failed: Invalid configuration at line 8 |
| TC-Rob-005-N | PRJ-SR-005 | Fault in one module does not terminate entire simulation | Tejas Shetty | **PASS** | `robModuleFault` | getCurrentPhase("Ghost-Lane")=RED, preempt("Ghost-Lane")=rejected, stats empty-lane -> "Intersection ID and lane name cannot be empty"; simulation continued to tick 500 |

## Ownership from Test Plan Section 9

- **Tejas Shetty:** LaneQueue, SignalController, Emergency Priority and Robustness cases (Sections 13.2, 13.3, 13.4 and 13.9).
- **Tarun Gowda P:** VehicleGenerator, Multi-Intersection and Performance/Reliability cases (Sections 13.1, 13.5 and relevant cases in 13.8).
- **Khushi Gupta:** Statistics, Console and Simulation-Control cases.
- **Portability cases:** assigned to Tarun as part of Section 13.8; confirm with the team, because Section 9 does not allocate them explicitly.

## Summary

| Status | Count |
|---|---:|
| PASS | 66 |
| FAIL | 0 |
| BLOCKED | 0 |
| NOT RUN | 0 |
| **Total** | **66** |

## CI verification record ([CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798))

| Job | Result |
|---|---|
| Build and test (ubuntu-latest) | PASS — 1,198 unit checks; ASan/UBSan; Valgrind; 77 acceptance results PASS, 0 FAIL |
| Build and test (windows-latest) | PASS — 1,198 unit checks; 69 acceptance results PASS, 0 FAIL, 1 BLOCKED (console Ctrl+C cannot be sent on a headless runner; TC-Sim-002-N passed locally on Windows and via SIGINT on Linux) |
| Cross-platform output comparison | PASS — identical output on Linux and Windows |
| Source line and branch coverage | PASS |

The CI runs tested commit `1f39d1e`, the merge preview GitHub builds for the pull request of branch `part3-testing-sprint3` (head `a92de57`). The artifacts are copied into `docs/QA_Reports/evidence/` (`*_linux.txt`) and `docs/QA_Reports/evidence/ci_windows/`.
