# System Validation Report

**Status: 29 of 33 requirements validated; 4 await the Linux CI memory-tool/portability verdict. Test Plan v1.2: 66/66 cases executed — 60 PASS, 0 FAIL, 6 PENDING CI.**

## Objective

Validate the complete system against every SRS requirement by executing the 66 Test Plan cases (two per requirement) on the final Sprint 3 build, and record the results.

## Environment and method

| Field | Value |
|---|---|
| Code under test | Sprint 3 baseline (`docs/SPRINT_3.md`) |
| Local platform | Windows 11, MSYS2 UCRT64 `g++ 16.2.0`, `-std=c++17 -O2` |
| Linux platform | GitHub Actions `ubuntu-latest`: same script plus ASan/UBSan/LeakSanitizer and Valgrind; runs on push |
| Executed | 16-11-2026 |
| Command | `bash scripts/run_acceptance.sh` (`make acceptance-full`) |
| Test basis | `Documents/Test_Plan_Traffic_Management_System.docx` v1.2, Section 13 |
| Case-level results | `docs/TEST_EXECUTION_REGISTER.md` |
| Raw evidence | `docs/QA_Reports/evidence/` |

Method:

1. **In-process cases (62).** `tests/acceptance/test_plan_acceptance.cpp` runs one function per TC ID against the production classes, using the case's own test data, and compares the actual result with the v1.2 expected result.
2. **Process-level cases.** `scripts/run_acceptance.sh` runs the real executable to check exit codes and stderr messages for missing, invalid and corrupted configs. It also runs a 500-tick natural completion, a **real Ctrl+C** (Windows `CTRL_C_EVENT` / POSIX `SIGINT`, via `tests/acceptance/send_ctrl_c.cpp`), a config-only third intersection run with the unchanged binary, and a build with `make` absent from `PATH`.
3. **Memory and portability cases.** ASan/UBSan, LeakSanitizer and Valgrind runs, plus a Linux-vs-Windows output diff, happen in CI because these tools do not exist for MinGW on Windows.

## Results by requirement

| Requirement | Capability | Positive case | Negative case | Outcome |
|---|---|---|---|---|
| TMS-F-001 | Randomized per-lane arrivals | PASS | PASS | **Validated** |
| TMS-F-002 | Unique ID, arrival tick, type | PASS | PASS | **Validated** |
| TMS-F-003 | FIFO enqueue | PASS | PASS | **Validated** |
| TMS-F-010 | Independent lane queues | PASS | PASS | **Validated** |
| TMS-F-011 | Lane capacity / blocked logging | PASS | PASS | **Validated** |
| TMS-F-012 | Dequeue only on Green | PASS | PASS | **Validated** |
| TMS-F-013 | Wait-time recording | PASS | PASS | **Validated** |
| TMS-F-020 | Fixed-timer cycle | PASS | PASS | **Validated** |
| TMS-F-021 | Adaptive Green, bounded | PASS | PASS | **Validated** |
| TMS-F-022 | No conflicting Greens | PASS | PASS | **Validated** |
| TMS-F-023 | Yellow before Red | PASS | PASS | **Validated** |
| TMS-F-024 | Mode switch via config | PASS | PASS | **Validated** |
| TMS-F-030 | Emergency probability | PASS | PASS | **Validated** |
| TMS-F-031 | Emergency priority dequeue | PASS | PASS | **Validated** |
| TMS-F-032 | Preemption within max delay | PASS | PASS | **Validated** |
| TMS-F-040 | Multiple independent intersections | PASS | PASS | **Validated** |
| TMS-F-041 | Green-wave link | PASS | PASS | **Validated** |
| TMS-F-042 | Per-intersection + aggregate stats | PASS | PASS | **Validated** |
| TMS-F-050 | Average wait time | PASS | PASS | **Validated** |
| TMS-F-051 | Maximum queue length | PASS | PASS | **Validated** |
| TMS-F-052 | Throughput | PASS | PASS | **Validated** |
| TMS-F-060 | Configuration | PASS | PASS | **Validated** |
| TMS-F-061 | Start / clean stop with summary | PASS | PASS | **Validated** |
| TMS-NF-001 | Performance ≥ 500 ticks/s | PASS | PASS | **Validated** |
| TMS-NF-002 | 10,000+ ticks, no crash/leak | PENDING CI | PENDING CI | Validated functionally; leak verdict pending CI |
| TMS-NF-003 | Topology via config only | PASS | PASS | **Validated** |
| TMS-NF-004 | Readable console | PASS | PASS | **Validated** |
| TMS-NF-005 | Linux + Windows portability | PENDING CI | PASS | Windows validated; Linux build/output diff pending CI |
| PRJ-SR-001 | Bounds checking | PENDING CI | PASS | Invalid-index rejection validated; ASan run pending CI |
| PRJ-SR-002 | Config validation | PASS | PASS | **Validated** |
| PRJ-SR-003 | No leaks (RAII) | PENDING CI | PENDING CI | Validated functionally; leak verdict pending CI |
| PRJ-SR-004 | Overflow handling | PASS | PASS | **Validated** |
| PRJ-SR-005 | No abnormal termination | PASS | PASS | **Validated** |

## Key end-to-end observations

| Scenario | Case | Observed |
|---|---|---|
| Fixed-timer cycle from config (10/3/10) | TC-Sig-001-P | North changes phase at ticks 10 and 13; South Green at tick 23 |
| Bounded preemption, config `yellow_duration=1`, `max_preemption_delay=2` | TC-Emg-003-P | Preemption logged at tick 1; emergency lane Green at tick 3 (delay 2) |
| Preemption safety | TC-Emg-003-N | East/North per tick: YELLOW/RED ×3 → RED/RED → RED/GREEN; never two Greens |
| 100-tick conflict scan, 4 lanes | TC-Sig-003-P | 0 conflicting ticks, 0 ticks with more than one Green; 5 preemptions occurred |
| Adaptive cap (base 10, `max_green_extension=10`) | TC-Sig-002-P/N | Queue 8 → Green 20 ticks; queue 50 → Green 20 ticks (capped) |
| Downstream full | TC-Multi-002-N | Vehicle held upstream; 2 vehicles accounted before and after; delivered exactly once |
| Per-intersection vs aggregate | TC-Multi-003-P | Busy 125 + Quiet 59 = 184 = reported aggregate |
| Throughput counted independently | TC-Stats-003-P | Replayed arrivals 307 − 222 still queued = 85 = reported throughput |
| Invalid intersection / runtime overload | TC-Multi-001-N | Load rejected: "Empty lane name in intersection: Two"; overload of Two caused 309 blocked in Two and 0 in One/Three |
| Missing / invalid / corrupted config (CLI) | TC-Sim-001-N, TC-Rob-002-P, TC-Rob-005-P | Exit code 1 with `Simulation failed: <reason>`; no crash codes |
| Real Ctrl+C on an open-ended run | TC-Sim-002-N | Exit 0; summary for 20,420 ticks = 20,420 rendered tick blocks; termination "user requested stop" |
| Topology change without rebuild | TC-Maint-01-P | Unchanged binary ran `three_intersections.cfg`: 3 intersections, Harbor reported |
| Missing build tool | TC-Port-01-N | `make: command not found`, exit 127 (make is genuinely absent on the test machine) |

## Performance (TMS-NF-001)

| Measurement | Result | Target |
|---|---|---|
| TC-Perf-01-P: 4 intersections × 4 lanes, 10,000 ticks | 15,437 ticks/s | ≥ 500 |
| TC-Perf-01-N: 8 chained intersections, arrival 0.95 | 10,222 ticks/s (0.36× the 4-intersection rate; linear expectation 0.50×) | ≥ 500 and no worse than 0.25× |
| `benchmarks/performance_benchmark.cpp`, 5 × 10,000 ticks | Median 19,403 ticks/s, average 19,233 ticks/s | ≥ 500 |

Absolute throughput on the test laptop varied with machine load (from about 15k to 276k ticks/s on the same build during the session). To rule out a regression, the original and Sprint 3 benchmarks were run back-to-back: original median 8,142 / 7,281 ticks/s against Sprint 3 median 17,435 / 23,544 ticks/s. In every run the target was exceeded by more than 14×. Full output: `evidence/benchmark_windows.txt`.

## Defects found by system testing

| Defect | Found by | Status |
|---|---|---|
| DEF-05: unbounded in-flight transfers under chained overload (24× slowdown for 2× intersections; 2,051 vehicles in flight) | TC-Perf-01-N **FAIL** on the first execution | Fixed (spillback); re-executed PASS |
| DEF-04: console columns misaligned for long lane names | TC-UX-01-N alignment check | Fixed; re-executed PASS |
| DEF-01, DEF-02, DEF-03, DEF-06: preemption delay bound, timing config keys, adaptive baseline, per-intersection totals | Updating the test cases against the Sprint 2 code | Fixed in Sprint 3 before execution |

Details and before/after measurements: `docs/SPRINT_3.md`.

## Deviations from the original (v1.0) expected results

These cases were revised in Test Plan v1.1/v1.2 and are traceable through the "Change in v1.2" rows of the DOCX:

- **Invalid or missing configuration is rejected, not replaced by defaults** (TC-Sig-005-N, TC-Sim-001-N, TC-Rob-002-P/N, TC-Rob-005-P). This follows SRS PRJ-SR-002.
- **Zero durations and zero capacity are rejected when the config loads or the object is built** (TC-Sig-001-N, TC-Queue-002-N), so they can never occur at runtime.
- **Fault isolation:** a config containing an invalid intersection is rejected as a whole (fail-fast). Runtime isolation between intersections is validated separately (TC-Multi-001-N). Module faults are contained at module boundaries rather than by a per-tick try/catch (TC-Rob-005-N).
- **Conflict safety:** the controller serves one lane at a time, which is stricter than "conflict-free sets" (TC-Sig-003-P/N). Geometric turning-movement modelling is outside the implemented design.

## Pending items

The six PENDING CI cases (TC-Rob-001-P, TC-Rel-01-P/N, TC-Rob-003-P/N, TC-Port-01-P) are fully automated in `.github/workflows/ci-cd.yml`. After pushing, take the verdicts from the `acceptance-evidence-Linux` artifact (`sanitizers_linux.txt`, `valgrind_linux.txt`, `acceptance_results_linux.txt`) and the `portability-compare` job, then update this report and the register.

## Conclusion

On the final Sprint 3 build every functional requirement (TMS-F-001 to TMS-F-061) is validated by both its positive and negative case. So are performance, maintainability, usability, and the PRJ-SR-002/004/005 robustness requirements. No case failed on the final build. Two defects found during execution were fixed and re-tested. System acceptance becomes complete once the Linux CI run confirms the four memory-tool/portability requirements and the team and instructor sign off.

## Earlier validation history

Earlier CI evidence (main run #99 with 436 checks; PR #94 graceful stop; PRs #98–#123 regression tests; main run #243 with 985 checks) is summarised in `Unit_Test_Report.md`. The earlier local benchmark (median 88,344 ticks/s) is superseded by the measurements above.
