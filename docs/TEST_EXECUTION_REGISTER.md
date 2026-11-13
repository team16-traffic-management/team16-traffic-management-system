# Test Execution Register — Traffic Management System

**Source:** Section 13 of `Documents/Test_Plan_Traffic_Management_System.docx` (implementation-aligned revision 1.1; revised DOCX supplied separately).  
**Purpose:** Track the 66 planned test cases individually. This register does not replace the approved Test Plan or its expected-result steps.

Related coverage analysis: [`docs/TEST_CASE_AUTOMATION_CROSSWALK.md`](TEST_CASE_AUTOMATION_CROSSWALK.md). This identifies related automated checks but does not change any case status.

## Execution rules

- Initial status for every case is **NOT RUN**, matching the Test Plan's explicit status.
- Change a status only after the case is executed against a recorded build.
- Allowed statuses: `PASS`, `FAIL`, `BLOCKED`, `NOT RUN`.
- For every executed case, add commit SHA, date/time, OS/compiler, exact command/config, actual result, expected-result comparison, and an evidence link or log path.
- A related unit-test assertion or CI run is supporting evidence only; it does not automatically mean the corresponding Test Plan case passed.
- `TC-Gen-003-N` is a FIFO boundary case: interleaved emergency insertion must not reorder normal vehicles. Queue capacity cases remain mapped to TMS-F-011 / PRJ-SR-004.
- Invalid/missing configuration is rejected with a descriptive error and non-zero exit; the parser does not substitute safe defaults.
- Zero queue capacity is rejected at construction/config validation; exercise the zero-capacity constructor boundary separately from valid config fixtures.
- TC-Sim-001-N has a direct automated missing-file error assertion, and TC-Port-01-N has a Makefile/CI missing-compiler diagnostic check; both acceptance cases remain NOT RUN until individually executed and recorded.

## Case register

| Test case ID | Requirement ID | Test case | Owner per Test Plan | Status | Execution evidence / notes |
|---|---|---|---|---|---|
| TC-Gen-001-P | TMS-F-001 | Vehicles spawn at configured arrival rate | Tarun Gowda P | NOT RUN | |
| TC-Gen-001-N | TMS-F-001 | Zero arrival rate produces zero vehicles | Tarun Gowda P | NOT RUN | |
| TC-Gen-002-P | TMS-F-002 | Unique vehicle ID, arrival tick and valid type | Tarun Gowda P | NOT RUN | |
| TC-Gen-002-N | TMS-F-002 | Same-tick spawns across lanes retain unique IDs | Tarun Gowda P | NOT RUN | |
| TC-Gen-003-P | TMS-F-003 | Vehicles enqueue in FIFO order | Tarun Gowda P | NOT RUN | |
| TC-Gen-003-N | TMS-F-003 | Emergency priority does not reverse FIFO order among normal vehicles | Tarun Gowda P | NOT RUN | |
| TC-Queue-001-P | TMS-F-010 | Each lane maintains independent queue state | Tejas Shetty | NOT RUN | |
| TC-Queue-001-N | TMS-F-010 | Operating on one lane does not corrupt sibling lane | Tejas Shetty | NOT RUN | |
| TC-Queue-002-P | TMS-F-011 | Capacity limit is enforced | Tejas Shetty | NOT RUN | |
| TC-Queue-002-N | TMS-F-011 | Zero-capacity lane rejects every vehicle | Tejas Shetty | NOT RUN | |
| TC-Queue-003-P | TMS-F-012 | Dequeue only happens while Green | Tejas Shetty | NOT RUN | |
| TC-Queue-003-N | TMS-F-012 | Empty Green lane dequeue attempt does not error | Tejas Shetty | NOT RUN | |
| TC-Queue-004-P | TMS-F-013 | Wait time is calculated correctly | Tejas Shetty | NOT RUN | |
| TC-Queue-004-N | TMS-F-013 | Immediate dequeue wait time is zero | Tejas Shetty | NOT RUN | |
| TC-Sig-001-P | TMS-F-020 | Fixed-timer cycle follows configured durations | Tejas Shetty | NOT RUN | |
| TC-Sig-001-N | TMS-F-020 | Zero-duration phase does not freeze cycle | Tejas Shetty | NOT RUN | |
| TC-Sig-002-P | TMS-F-021 | Adaptive mode extends Green for longer queues | Tejas Shetty | NOT RUN | |
| TC-Sig-002-N | TMS-F-021 | Adaptive extension is capped for extreme queue | Tejas Shetty | NOT RUN | |
| TC-Sig-003-P | TMS-F-022 | Conflicting directions are never simultaneously Green | Tejas Shetty | NOT RUN | |
| TC-Sig-003-N | TMS-F-022 | Rapid config changes cannot force conflicting state | Tejas Shetty | NOT RUN | |
| TC-Sig-004-P | TMS-F-023 | Yellow phase occurs before Red | Tejas Shetty | NOT RUN | |
| TC-Sig-004-N | TMS-F-023 | One-tick Yellow duration renders correctly | Tejas Shetty | NOT RUN | |
| TC-Sig-005-P | TMS-F-024 | Signal mode switches via config alone | Tejas Shetty | NOT RUN | |
| TC-Sig-005-N | TMS-F-024 | Invalid adaptive_mode is rejected with a clear error | Tejas Shetty | NOT RUN | |
| TC-Emg-001-P | TMS-F-030 | Emergency vehicles spawn at configured probability | Tejas Shetty | NOT RUN | |
| TC-Emg-001-N | TMS-F-030 | Zero emergency probability produces no emergency vehicles | Tejas Shetty | NOT RUN | |
| TC-Emg-002-P | TMS-F-031 | Emergency vehicle dequeued ahead of normal vehicles | Tejas Shetty | NOT RUN | |
| TC-Emg-002-N | TMS-F-031 | Emergency vehicles preserve FIFO among themselves | Tejas Shetty | NOT RUN | |
| TC-Emg-003-P | TMS-F-032 | Signal preempts for a waiting emergency vehicle | Tejas Shetty | NOT RUN | |
| TC-Emg-003-N | TMS-F-032 | Preemption does not create conflicting Green state | Tejas Shetty | NOT RUN | |
| TC-Multi-001-P | TMS-F-040 | Multiple intersections run independently | Tarun Gowda P | NOT RUN | |
| TC-Multi-001-N | TMS-F-040 | Fault in one intersection does not corrupt/halt others | Tarun Gowda P | NOT RUN | |
| TC-Multi-002-P | TMS-F-041 | Green-wave link passes a vehicle downstream | Tarun Gowda P | NOT RUN | |
| TC-Multi-002-N | TMS-F-041 | Full downstream lane handles handoff gracefully | Tarun Gowda P | NOT RUN | |
| TC-Multi-003-P | TMS-F-042 | Per-intersection and aggregate stats are correct | Tarun Gowda P | NOT RUN | |
| TC-Multi-003-N | TMS-F-042 | Zero-throughput intersection does not break aggregate | Tarun Gowda P | NOT RUN | |
| TC-Stats-001-P | TMS-F-050 | Average wait time is computed correctly | Khushi Gupta | NOT RUN | |
| TC-Stats-001-N | TMS-F-050 | Zero dequeue events do not error | Khushi Gupta | NOT RUN | |
| TC-Stats-002-P | TMS-F-051 | Maximum queue length is tracked correctly | Khushi Gupta | NOT RUN | |
| TC-Stats-002-N | TMS-F-051 | Maximum queue length remains correct when queue ≤ 1 | Khushi Gupta | NOT RUN | |
| TC-Stats-003-P | TMS-F-052 | Throughput count is accurate | Khushi Gupta | NOT RUN | |
| TC-Stats-003-N | TMS-F-052 | Blocked/dropped vehicles are not counted as throughput | Khushi Gupta | NOT RUN | |
| TC-Sim-001-P | TMS-F-060 | Config values are applied correctly | Khushi Gupta | NOT RUN | |
| TC-Sim-001-N | TMS-F-060 | Missing config file is rejected with a clear error and non-zero exit | Khushi Gupta | NOT RUN | |
| TC-Sim-002-P | TMS-F-061 | Natural run completion produces clean summary | Khushi Gupta | NOT RUN | |
| TC-Sim-002-N | TMS-F-061 | Manually stopped run produces clean summary | Khushi Gupta | NOT RUN | |
| TC-Perf-01-P | TMS-NF-001 | Throughput performance target is met | Tarun Gowda P | NOT RUN | |
| TC-Perf-01-N | TMS-NF-001 | Performance does not collapse under maximum load | Tarun Gowda P | NOT RUN | |
| TC-Rel-01-P | TMS-NF-002 | No crashes or leaks over a long run | Tarun Gowda P | NOT RUN | |
| TC-Rel-01-N | TMS-NF-002 | Repeated start/stop cycles do not accumulate leaks | Tarun Gowda P | NOT RUN | |
| TC-Maint-01-P | TMS-NF-003 | New topology can be added via config only | Tarun Gowda P | NOT RUN | |
| TC-Maint-01-N | TMS-NF-003 | Invalid topology is rejected, not silently accepted | Tarun Gowda P | NOT RUN | |
| TC-UX-01-P | TMS-NF-004 | Console output is readable during a run | Khushi Gupta | NOT RUN | |
| TC-UX-01-N | TMS-NF-004 | Output remains readable with long lane/high queue | Khushi Gupta | NOT RUN | |
| TC-Port-01-P | TMS-NF-005 | Build succeeds on Linux and Windows | Tarun Gowda P | NOT RUN | |
| TC-Port-01-N | TMS-NF-005 | Missing build dependency fails with clear message | Tarun Gowda P | NOT RUN | |
| TC-Rob-001-P | PRJ-SR-001 | Container access is bounds-checked | Tejas Shetty | NOT RUN | |
| TC-Rob-001-N | PRJ-SR-001 | Invalid index access is caught | Tejas Shetty | NOT RUN | |
| TC-Rob-002-P | PRJ-SR-002 | Invalid config values are rejected safely | Tejas Shetty | NOT RUN | |
| TC-Rob-002-N | PRJ-SR-002 | Wrong-type config value does not crash parser | Tejas Shetty | NOT RUN | |
| TC-Rob-003-P | PRJ-SR-003 | Zero memory leaks over sustained run | Tejas Shetty | NOT RUN | |
| TC-Rob-003-N | PRJ-SR-003 | Mid-run exception still releases memory | Tejas Shetty | NOT RUN | |
| TC-Rob-004-P | PRJ-SR-004 | Queue overflow is handled safely | Tejas Shetty | NOT RUN | |
| TC-Rob-004-N | PRJ-SR-004 | Simultaneous overflow on every lane is handled | Tejas Shetty | NOT RUN | |
| TC-Rob-005-P | PRJ-SR-005 | Malformed input does not cause abnormal termination | Tejas Shetty | NOT RUN | |
| TC-Rob-005-N | PRJ-SR-005 | Fault in one module does not terminate entire simulation | Tejas Shetty | NOT RUN | |

## Ownership from Test Plan Section 9

- **Tejas Shetty:** LaneQueue, SignalController, Emergency Priority and Robustness cases (Sections 13.2, 13.3, 13.4 and 13.9).
- **Tarun Gowda P:** VehicleGenerator, Multi-Intersection and Performance/Reliability cases (Sections 13.1, 13.5 and relevant cases in 13.8).
- **Khushi Gupta:** Statistics, Console and Simulation-Control cases.
- **Portability cases:** assigned to Tarun here as part of Section 13.8; confirm this allocation with the team because the Test Plan does not explicitly allocate them in Section 9.

## Summary

| Status | Count |
|---|---:|
| PASS | 0 |
| FAIL | 0 |
| BLOCKED | 0 |
| NOT RUN | 66 |
| **Total** | **66** |

The 436 automated assertions reported in the baseline CI run are not counted as executions of these 66 test cases.
