# Sprint 3 — Spill-over Development and Parallel Testing

**Deliverable:** Mini-Project Part 3, item 3 (optional Sprint 3, run in parallel with testing).
**Work recorded:** 16-11-2026.
**Outcome:** Six spill-over items closed; the 66 Test Plan cases executed: 66 PASS, 0 FAIL (Linux verdicts from [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798)).

## Why a Sprint 3 was needed

Updating the test cases against the Sprint 2 code (Part 3, item 1) showed that several SRS requirements were only partly implemented, so their Test Plan cases could not pass as written. The first full acceptance run then found two more defects. These were treated as Sprint 2 spill-overs.

## Spill-over items

| ID | Requirement | Problem found | Evidence before the fix | Fix | Verified by |
|---|---|---|---|---|---|
| DEF-01 | TMS-F-032 | Emergency preemption waited for the whole current signal cycle; there was no configurable maximum delay. | Worst-case delay of 22 ticks with Green/Yellow/Red = 10/3/10 (measured with the original `SignalController`). | An accepted request ends the current Green at once, keeps the full Yellow phase, holds 1 all-red tick, then grants Green. Worst case = Yellow + 1 = 4 ticks by default. New config key `max_preemption_delay`, rejected if shorter than Yellow + 1. | `testBoundedEmergencyPreemptionDelay` (all 23 request points in a cycle), `testPreemptionDelayConfigurationBounds`, `testEnginePreemptsWithinConfiguredDelay`; TC-Emg-003-P/N |
| DEF-02 | TMS-F-020, TMS-F-060 | Signal durations were only constructor arguments and could not be set from the config file. | No config key existed for Green/Yellow/Red. | New optional keys `green_duration`, `yellow_duration` and `red_duration` (integers 1–10000; defaults 10/3/10). | `testSignalTimingsLoadedFromConfiguration`; TC-Sig-001-P/N, TC-Sim-001-P |
| DEF-03 | TMS-F-021 | Adaptive mode used a hard-coded base Green of 10 and cap of 10 + 15, ignoring the configured Green; the maximum extension could not be configured. | A 2-tick configured Green became 10 ticks after one light-queue adaptive update. | The adaptive baseline follows the configured Green; cap = base + `max_green_extension` (new key, 0–10000, default 15); disabling adaptive mode restores the base. | `testAdaptiveGreenExtensionBounds` (rewritten for the corrected rule); TC-Sig-002-P/N |
| DEF-04 | TMS-NF-004 | Long lane names pushed the Signal/Queue columns out of alignment in the live view and the end-of-run table. | The new alignment test fails against the original `ConsoleView.cpp`. | Lane and intersection columns are sized to the longest name. | `testConsoleColumnsStayAlignedWithLongLaneNames`; TC-UX-01-P/N |
| DEF-05 | TMS-NF-001, PRJ-SR-004 | With chained intersections under heavy load, vehicles left a linked lane even when the downstream lane was full. They piled up in an unbounded in-flight list that was rescanned every tick. | First acceptance run: TC-Perf-01-N **FAILED**. 8 chained intersections at 0.95 arrival: 2,051 vehicles in flight after 10,000 ticks (lane capacity 15); 3.13 s for 10,000 ticks. | Spillback: a linked vehicle leaves only when the downstream lane has room after counting transfers already in flight. | `testPendingTransfersBoundedByDownstreamCapacity`; TC-Perf-01-N, TC-Multi-002-N. After the fix: at most 3 in flight, 0.18 s for the same run. |
| DEF-06 | TMS-F-042, TMS-F-052 | The report had per-lane rows only, with no per-intersection totals. | The aggregate could not be checked against per-intersection totals. | `StatisticsCollector::intersectionTotals()`, `intersectionThroughput()` and `blockedFor()`; the report adds a "Per-Intersection Totals" table with a `SYSTEM TOTAL` row. | `testPerIntersectionTotalsSumToAggregate`; TC-Multi-003-P/N |

## Behaviour changes that affect existing expectations

- **Preemption timing.** A pending emergency request now interrupts Green straight away instead of waiting for the cycle to end. Yellow is never skipped, and at most one lane is ever Green (TC-Emg-003-N).
- **Linked-lane transfers.** A full downstream lane now holds traffic upstream (spillback) instead of collecting it in a hidden buffer. "Vehicles still waiting for transfer" in the summary is now bounded by downstream capacity.
- **Adaptive mode.** The cap is base Green + maximum extension. Previously it was 10 + maximum extension whatever the configured Green.
- **Config validation stays fail-fast.** A file containing an invalid intersection is rejected as a whole (PRJ-SR-002). Runtime isolation between intersections is verified separately (TC-Multi-001-N).

## New configuration keys (all optional)

| Key | Range | Default | Purpose |
|---|---|---|---|
| `green_duration` | 1–10000 | 10 | Base Green ticks |
| `yellow_duration` | 1–10000 | 3 | Yellow ticks |
| `red_duration` | 1–10000 | 10 | All-red clearance ticks before the next lane |
| `max_preemption_delay` | Yellow + 1 to 10000 | Yellow + 1 | Maximum emergency preemption delay (ticks) |
| `max_green_extension` | 0–10000 | 15 | Adaptive-mode maximum Green extension |

## Testing work done in parallel

| Item | Location |
|---|---|
| Test Plan v1.2 (updated cases, actual results, status, RTM) | `Documents/Test_Plan_Traffic_Management_System.docx` |
| Acceptance runner: one function per TC ID (62 in-process cases) | `tests/acceptance/test_plan_acceptance.cpp` |
| Real Ctrl+C harness (Windows `CTRL_C_EVENT` / POSIX `SIGINT`) | `tests/acceptance/send_ctrl_c.cpp` |
| Orchestration: CLI exit codes, Ctrl+C, missing tools, memory tools, benchmark | `scripts/run_acceptance.sh` (`make acceptance-full`) |
| New fixtures | `tests/data/open_ended.cfg`, `three_intersections.cfg`, `long_run.cfg` |
| 7 new unit tests (42 in total, 1,198 checks) | `tests/test_traffic_management.cpp` |
| Coverage now includes the acceptance runner and the default app run | `scripts/run_coverage.sh` |
| CI: acceptance on Ubuntu and Windows, Valgrind, sanitizers on the acceptance runner, cross-platform output comparison | `.github/workflows/ci-cd.yml` |
| Execution register and the four QA reports | `docs/TEST_EXECUTION_REGISTER.md`, `docs/QA_Reports/` |

## Files changed

- `include/TrafficSignalController.h`, `src/TrafficSignalController.cpp` — DEF-01, DEF-02, DEF-03
- `include/SimulationEngine.h`, `src/SimulationEngine.cpp` — config keys, spillback (DEF-05), read-only accessors for tests
- `include/StatisticsCollector.h`, `src/StatisticsCollector.cpp` — DEF-06; DEF-04 for the report table
- `src/ConsoleView.cpp` — DEF-04
- `tests/test_traffic_management.cpp` — 7 new tests, 1 rewritten (adaptive bounds), per-test result lines
- `Makefile`, `scripts/run_coverage.sh`, `.github/workflows/ci-cd.yml` — build, coverage and CI wiring

## Open items

- Record the Sprint 3 items in the team's backlog tool and assign owners, as for Sprints 1 and 2.
- Team review and sign-off.
