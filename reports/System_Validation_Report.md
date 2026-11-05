# System Validation Report

Status: Partial CI validation evidence recorded. Full SRS-based system validation is not signed off.

## Objective

Record system-level checks actually executed by the repository's GitHub Actions workflow for PR #87. PR #87 has since been merged to `main`; the detailed test results below are from its PR integration ref, not a post-merge main-branch workflow run.

## Test environment

| Field | Value |
|---|---|
| Operating system | Ubuntu 26.04 and Windows GitHub-hosted runner |
| Compiler | GCC 15.2.0 (Ubuntu); MinGW GCC 15.2.0 (Windows) |
| Build system | GNU Make 4.4.1 |
| Language standard | C++17 |
| PR head commit | `863c8c3abbe374e133168164cb7bed59bab9ceff` |
| CI integration ref | `74f8252dd534fa259a1c669fc0b92cc96e7d5132` |
| Execution date | 5 November 2026 |
| Workflow | [Run #92](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42073258550) |

## Validation register

| ID | Scenario | Actual evidence | Status |
|---|---|---|---|
| SYS-01 | Application starts with default configuration | Default application smoke test exited successfully on Ubuntu and Windows | PASS |
| SYS-02 | Vehicle arrivals and statistics are exercised | Test executable completed; 10,000-tick run reported throughput 4,341, average wait 59.57 ticks, and 3,745 blocked vehicles in this deterministic CI run | PASS — execution only |
| SYS-03 | Lane queue behavior | Automated queue capacity, overflow rejection, FIFO and empty-dequeue checks passed | PASS |
| SYS-04 | Signal phase behavior | PR #87 signal transition, lane rotation and maximum-queue-statistics checks passed | PASS |
| SYS-05 | Conflicting green signals | No dedicated cross-lane conflict invariant/system scenario is recorded by this workflow | NOT RUN |
| SYS-06 | Emergency vehicle behavior | Preemption request acceptance/rejection and deferred activation checks passed | PASS — tested cases only |
| SYS-07 | Multi-intersection transfer | Multi-intersection config startup passed on both platforms; automated transfer test passed and CI run reported 11 successful transfers in its 25-tick fixture | PASS — tested configuration only |
| SYS-08 | Invalid configuration | Malformed configuration rejection and invalid-input unit checks passed | PASS — tested inputs only |
| SYS-09 | Long simulation run | 10,000-tick test completed; Linux ASan/UBSan run completed successfully | PASS — execution/stability evidence |

## CI execution details

- Ubuntu and Windows: application and test executable built with C++17.
- Ubuntu and Windows: automated test executable reported 355 checks passed, 0 failed.
- Ubuntu and Windows: default application smoke test passed.
- Ubuntu and Windows: config-driven multi-intersection startup smoke test passed.
- Ubuntu: AddressSanitizer and UndefinedBehaviorSanitizer target passed.
- Ubuntu: Linux application executable uploaded as a workflow artifact.
- Windows: sanitizer and Linux artifact steps were skipped by workflow design; no Windows sanitizer result is claimed.

## Results and limitations

PR #87 merge commit: `0c2f9fb5451200fd84abaa5b941a15d1ded81b87`. Updated PR workflow run #92 passed before merge. The 10,000-tick run completed and produced the observed metrics above. This does not by itself prove bounded memory growth or every simulation invariant. The current test's non-negative checks on unsigned totals are not meaningful invariant assertions.

The workflow's multi-intersection smoke test proves successful startup with the supplied example configuration; the automated integration test adds evidence of transfers, but does not validate every possible topology or transfer timing boundary.

The following remain outside the evidence recorded here:
- Full SRS/Test Plan walkthrough and requirement-by-requirement acceptance.
- A dedicated system invariant check that conflicting directions are never green simultaneously.
- Graceful user-stop behavior.
- Evaluator-hardware performance benchmark.
- Full leak/memory-growth analysis across repeated create/remove cycles.
- Independent manual end-to-end validation with retained screenshots/logs.

## Conclusion

CI provides successful cross-platform build, automated test, default startup and multi-intersection startup evidence, plus Linux sanitizer evidence. These results support the tested scenarios only. Overall system validation remains partial; do not mark the entire system accepted until the remaining scenarios are executed and documented.
