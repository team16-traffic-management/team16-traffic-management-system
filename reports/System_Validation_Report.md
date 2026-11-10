# System Validation Report

Status: Partial system validation. Main-branch CI run #99 passed; full SRS/Test Plan acceptance is not signed off.

## Objective

Record checks executed by the repository's GitHub Actions workflow on the final tested main commit `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540`.

## Test environment

| Field | Value |
|---|---|
| Operating system | Ubuntu 26.04 and Windows GitHub-hosted runner |
| Compiler | GCC 15.2.0 (Ubuntu); MinGW GCC 15.2.0 (Windows) |
| Build system | GNU Make 4.4.1 |
| Language standard | C++17 |
| PR head commit | `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540` |
| CI integration ref | `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540` |
| Execution date | 8 November 2026 |
| Workflow | [Run #99](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604) |

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

## CI execution details

- Ubuntu and Windows: application and test executable built with C++17.
- Ubuntu and Windows: automated test executable reported 355 checks passed, 0 failed.
- Ubuntu and Windows: default application smoke test passed.
- Ubuntu and Windows: config-driven multi-intersection startup smoke test passed.
- Ubuntu: AddressSanitizer and UndefinedBehaviorSanitizer target passed.
- Ubuntu: Linux application executable uploaded as a workflow artifact.
- Windows: sanitizer and Linux artifact steps were skipped by workflow design; no Windows sanitizer result is claimed.

## Results and limitations

Final tested main commit: `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540`. Main-branch workflow run #99 passed on Ubuntu and Windows; both test logs reported 436 checks passed. The 10,000-tick run completed and produced the observed metrics above. This does not by itself prove bounded memory growth or every simulation invariant. The current test's non-negative checks on unsigned totals are not meaningful invariant assertions.

The workflow's multi-intersection smoke test proves successful startup with the supplied example configuration; the automated integration test adds evidence of transfers, but does not validate every possible topology or transfer timing boundary.

The following remain outside the evidence recorded here:
- Full SRS/Test Plan walkthrough and requirement-by-requirement acceptance.
- A geometric movement-conflict matrix and proof that conflicting turning movements are never green simultaneously; the current regression only checks at most one GREEN lane.
- Graceful user-stop behavior.
- Evaluator-hardware performance benchmark.
- Full leak/memory-growth analysis across repeated create/remove cycles.
- Independent manual end-to-end validation with retained screenshots/logs.

## Conclusion

CI provides successful cross-platform build, automated test, default startup and multi-intersection startup evidence, plus Linux sanitizer evidence. These results support the tested scenarios only. Overall system validation remains partial; do not mark the entire system accepted until the remaining scenarios are executed and documented.
