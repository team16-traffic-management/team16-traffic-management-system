# System Validation Report

Status: Partial system validation. Baseline main CI run #99 passed; manual-stop PR #94 was subsequently merged after cross-platform CI run #132 passed. Full SRS/Test Plan acceptance is not signed off.

## Objective
Record automated CI evidence, local performance evidence and outstanding system-level acceptance checks. Automated CI evidence and manual Test Plan execution are separate.

## Test environment and evidence
| Field | Value |
|---|---|
| Language standard | C++17 |
| Baseline main workflow | [Run #99](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604) |
| Baseline tested commit | `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540` |
| Baseline result | 436 automated checks passed on Ubuntu and Windows; default-app and multi-intersection startup smoke tests passed; Ubuntu ASan/UBSan passed |
| Manual-stop PR workflow | [Run #132](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865) |
| Manual-stop PR head | `9b8730c4cb0fdb5f8e0a172e40b16912fff49c53` |
| Manual-stop merge commit | `339dfed320036ed1c42a375f70bc1189fb5b4244` |
| PR #94 CI result | Ubuntu and Windows build, automated tests, default-app and multi-intersection smoke tests passed; Ubuntu ASan/UBSan passed |
| Benchmark | Local Windows; `-std=c++17 -O2 -DNDEBUG`; exact OS build/compiler/hardware metadata not recorded |

## Validation register
| ID | Scenario | Evidence | Status |
|---|---|---|---|
| SYS-01 | Default application startup | Default-app smoke tests passed in baseline and PR #94 CI on Ubuntu/Windows | PASS — CI smoke test |
| SYS-02 | Vehicle arrivals and statistics | Automated test executable and 10,000-tick fixture completed | PASS — tested execution only |
| SYS-03 | Lane queue behavior | Capacity, overflow, FIFO and empty-dequeue assertions | PASS — covered assertions |
| SYS-04 | Signal phase behavior | Phase-transition tests and PR #90 80-tick at-most-one-GREEN-lane invariant | PASS — covered assertions |
| SYS-05 | Conflicting green signals | At-most-one-GREEN-lane invariant; no geometric movement matrix | PARTIAL — does not prove turning-movement conflict safety |
| SYS-06 | Emergency behavior | Preemption request acceptance/rejection and deferred activation tests | PASS — tested cases only |
| SYS-07 | Multi-intersection transfer | Two-intersection transfer assertion and config-driven startup smoke tests | PASS — tested topology only |
| SYS-08 | Invalid configuration | PR #91 missing/invalid values, numeric boundaries, malformed probabilities, duplicate/unknown keys and invalid connections | PASS — covered inputs only |
| SYS-09 | Long simulation / memory safety | 10,000-tick test and Linux ASan/UBSan CI | PARTIAL — no repeated-run leak/memory-growth report |
| SYS-10 | Performance | Five local 10,000-tick runs; average 84,371.19 and median 88,344.30 ticks/sec vs 500 target | PASS — local fixture only |
| SYS-11 | Graceful manual stop | PR #94 merged; automated direct-stop and SIGINT tests passed on Ubuntu/Windows CI; Linux sanitizer passed | PASS — automated verification; manual interactive acceptance still NOT RUN/documented |

## Local performance benchmark
One warm-up followed by five measured runs of 10,000 ticks, four independent intersections and four lanes per intersection; console rendering disabled.

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

The median is approximately 176.7 times the target. This is evidence for the local fixture only: intersections are independent, rendering is disabled, and exact CPU, RAM, Windows build and compiler version were not recorded. It is not evaluator-hardware or connected-network performance certification.

## Remaining validation
- Execute and record all 66 Test Plan cases individually; 436 automated assertions are not equivalent to the 66 planned cases.
- Perform a manual interactive Ctrl+C run and verify the printed completed-tick count and partial summary.
- Test geometric movement conflicts between turning movements; current invariant checks only at most one GREEN lane.
- Test exact one-tick transfer latency, full destination queue behavior and invalid-intersection isolation.
- Test adaptive timing bounds and emergency maximum-wait/preemption delay.
- Run repeated create/run/destroy cycles and retain memory-growth/leak evidence; sanitizer pass is not a leak report.
- Inspect console formatting with long lane names and high queue lengths.
- Benchmark connected intersections and evaluator hardware if required.
- Resolve SRS PRJ-SR-002 rejection behavior versus Test Plan safe-default expectations; correct `TC-Gen-003-N` mapping.
- Regenerate final coverage and record exact file scope.

## Conclusion
The repository has successful cross-platform CI evidence, Linux sanitizer evidence, and a local benchmark exceeding the stated throughput target for its fixture. Graceful stop is implemented and automated-CI verified. Overall system acceptance remains PARTIAL until outstanding requirement-level cases are executed, documented and signed off.
