# System Validation Report

Status: Partial system validation. Main-branch CI run #99 passed; local Windows performance benchmark met the SRS throughput target for the measured fixture. Full SRS/Test Plan acceptance is not signed off.

## Objective

Record checks executed by the repository's GitHub Actions workflow on the final tested main commit `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540`, together with the separately executed local Windows performance benchmark.

## Test environment

| Field | Value |
|---|---|
| CI operating systems | Ubuntu 26.04 and Windows GitHub-hosted runner |
| CI compiler | GCC 15.2.0 (Ubuntu); MinGW GCC 15.2.0 (Windows) |
| CI build system | GNU Make 4.4.1 |
| Language standard | C++17 |
| CI tested main commit | `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540` |
| CI execution date | 8 November 2026 |
| CI workflow | [Run #99](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604) |
| Local benchmark OS | Windows (PowerShell; exact OS build not recorded) |
| Local benchmark compiler | MinGW-w64 GCC; exact version not recorded |
| Local benchmark flags | `-std=c++17 -O2 -DNDEBUG` |
| Local benchmark source | `benchmarks/performance_benchmark.cpp` |

## Validation register

| ID | Scenario | Actual evidence | Status |
|---|---|---|---|
| SYS-01 | Application starts with default configuration | Default application smoke test exited successfully on Ubuntu and Windows | PASS |
| SYS-02 | Vehicle arrivals and statistics are exercised | Test executable completed; 10,000-tick run reported throughput 4,341, average wait 59.57 ticks, and 3,745 blocked vehicles in this deterministic CI run | PASS — execution only |
| SYS-03 | Lane queue behavior | Automated queue capacity, overflow rejection, FIFO and empty-dequeue checks passed | PASS |
| SYS-04 | Signal phase behavior | Signal transition, lane rotation and maximum-queue-statistics checks passed; PR #90's 80-tick regression check confirmed no more than one lane reports GREEN at a time | PASS |
| SYS-05 | Conflicting green signals | PR #90 checks serialized GREEN phases for 80 ticks; geometric conflicts between turning movements are not modeled by this test | PASS — serialized-green invariant only |
| SYS-06 | Emergency vehicle behavior | Preemption request acceptance/rejection and deferred activation checks passed | PASS — tested cases only |
| SYS-07 | Multi-intersection transfer | Multi-intersection config startup passed on both platforms; automated transfer test passed and CI run reported 11 successful transfers in its 25-tick fixture | PASS — tested configuration only |
| SYS-08 | Invalid configuration | PR #91 adds missing required fields, out-of-range tick/capacity/rate values, invalid booleans, non-finite/malformed probabilities, duplicate/unknown keys, empty lane entries and invalid connections | PASS — tested inputs only |
| SYS-09 | Long simulation run | 10,000-tick test completed; Linux ASan/UBSan run completed successfully | PASS — execution/stability evidence |
| SYS-10 | Performance throughput | Local Windows benchmark: five measured 10,000-tick runs; average 84,371.19 ticks/sec and median 88,344.30 ticks/sec against the 500 ticks/sec SRS target | PASS — local benchmark fixture only |

## CI execution details

- Ubuntu and Windows: application and test executable built with C++17.
- Ubuntu and Windows: automated test executable reported 436 checks passed, 0 failed.
- Ubuntu and Windows: default application smoke test passed.
- Ubuntu and Windows: config-driven multi-intersection startup smoke test passed.
- Ubuntu: AddressSanitizer and UndefinedBehaviorSanitizer target passed.
- Ubuntu: Linux application executable uploaded as a workflow artifact.
- Windows: sanitizer and Linux artifact steps were skipped by workflow design; no Windows sanitizer result is claimed.

## Local performance benchmark

The standalone benchmark was compiled and run locally on Windows using `-std=c++17 -O2 -DNDEBUG`. It performed one warm-up followed by five measured runs. Each measured run executed 10,000 ticks with four intersections and four lanes per intersection; console rendering was disabled.

| Run | Throughput |
|---|---:|
| 1 | 59,850.24 ticks/second |
| 2 | 85,234.42 ticks/second |
| 3 | 88,344.30 ticks/second |
| 4 | 97,249.59 ticks/second |
| 5 | 91,177.40 ticks/second |
| Average | 84,371.19 ticks/second |
| Median | 88,344.30 ticks/second |
| SRS target | 500.00 ticks/second |

The median is approximately 176.7 times the target. This result is evidence for this local benchmark fixture only. The four intersections are independent (not connected by transfer links), console rendering is disabled, and the exact CPU, RAM, Windows build and compiler version were not recorded. It is not evaluator-hardware evidence or a full end-to-end application performance measurement.

## Results and limitations

The main-branch workflow run #99 passed on Ubuntu and Windows; both test logs reported 436 checks passed. The 10,000-tick run completed and produced the observed metrics above. This does not by itself prove bounded memory growth or every simulation invariant. The current test's non-negative checks on unsigned totals are not meaningful invariant assertions.

The workflow's multi-intersection smoke test proves successful startup with the supplied example configuration; the automated integration test adds evidence of transfers, but does not validate every possible topology or transfer timing boundary.

The following remain outside the evidence recorded here:
- Full SRS/Test Plan walkthrough and requirement-by-requirement acceptance.
- A geometric movement-conflict matrix and proof that conflicting turning movements are never green simultaneously; the current regression only checks at most one GREEN lane.
- Graceful user-stop behavior.
- Performance benchmark on evaluator hardware and a connected multi-intersection benchmark fixture.
- Full leak/memory-growth analysis across repeated create/remove cycles.
- Independent manual end-to-end validation with retained screenshots/logs.

## Conclusion

CI provides successful cross-platform build, automated test, default startup and multi-intersection startup evidence, plus Linux sanitizer evidence. The local Windows performance benchmark exceeded the 500 ticks/sec target for its measured fixture. These results support the tested scenarios only. Overall system validation remains partial; do not mark the entire system accepted until the remaining scenarios are executed and documented.
