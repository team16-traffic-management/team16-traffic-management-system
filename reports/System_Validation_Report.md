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
| SYS-12 | Adaptive green timing boundaries | PR #98 verifies capped extension, low-queue base-duration reset and negative extension rejection; CI run #141 passed Ubuntu/Windows | PASS — tested boundaries only; not full timing policy validation |
| SYS-13 | Destination queue transfer backpressure | PR #99 verifies pending transfer waits while destination lane is full and is delivered after capacity frees; CI run #142 passed Ubuntu/Windows | PASS — tested scenario; exact one-tick latency and invalid-intersection isolation remain unverified |
| SYS-14 | Emergency queue ordering | PR #101 verifies emergency-first ordering and FIFO stability within emergency and normal vehicles; CI run #143 passed Ubuntu/Windows | PASS — queue ordering cases only; maximum emergency wait remains unverified |
| SYS-15 | Console long-lane/high-queue output | PR #102 verifies full long lane name, queue count and visual bar; PR run #144 and main run #149 passed Ubuntu/Windows | PASS — output assertions; manual visual/usability inspection remains outstanding |
| SYS-16 | Vehicle ID uniqueness | PR #104 checks 200 generated vehicles across four lanes in two intersections for duplicate IDs; CI run #153 passed Ubuntu/Windows | PASS — tested generation fixture |
| SYS-17 | Engine-level green-only dequeue | PR #105 verifies a red-lane vehicle remains queued until its lane turns green; CI run #154 passed Ubuntu/Windows | PASS — tested signal cycle and engine behavior |
| SYS-18 | Exact one-tick connected transfer latency | PR #107 verifies a vehicle scheduled from the source in tick one is delivered and processed by the destination in tick two; CI run #159 passed Ubuntu/Windows | PASS — tested topology and timing path |
| SYS-19 | Repeated simulation lifecycle | PR #108 creates, runs and destroys 20 independent SimulationEngine instances, each for 100 ticks; CI run #162 passed Ubuntu/Windows | PASS — repeated lifecycle execution; not a quantified leak/memory-growth measurement |

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
- Test exact one-tick transfer latency and invalid-intersection isolation. Destination full-queue backpressure is covered by PR #99.
- Test emergency maximum-wait/preemption delay. Adaptive timing boundary cases are covered by PR #98.
- PR #108 now runs 20 create/run/destroy cycles under the sanitizer-enabled suite; retain a quantified memory-growth/leak report if required, since repeated lifecycle assertions are not a leak report.
- Perform manual visual/usability inspection of console formatting; long lane names and high queue lengths are now covered by PR #102 assertions.
- Benchmark connected intersections and evaluator hardware if required.
- Resolve SRS PRJ-SR-002 rejection behavior versus Test Plan safe-default expectations; correct `TC-Gen-003-N` mapping.
- Regenerate final coverage and record exact file scope.

## Latest merged QA regression evidence
- PR #98 adaptive timing boundaries: [CI run #141](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42791702831), merged.
- PR #99 transfer backpressure: [CI run #142](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42792945910), merged.
- PR #101 emergency queue ordering: [CI run #143](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42795154445), merged.
- PR #102 long-lane/high-queue console output: [CI run #144](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42795769676), merged.
- PR #104 vehicle ID uniqueness: [CI run #153](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42796601039), merged.
- PR #105 green-only engine dequeue: [CI run #154](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42796698371), merged.
- PR #107 exact one-tick transfer latency: [CI run #159](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797325343), merged.
- PR #108 repeated simulation lifecycle: [CI run #162](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797660386), merged.
- Post-merge main CI: [run #149](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42796030003) passed Ubuntu and Windows; Ubuntu ASan/UBSan passed.

## Conclusion
The repository has successful cross-platform CI evidence, Linux sanitizer evidence, and a local benchmark exceeding the stated throughput target for its fixture. Graceful stop is implemented and automated-CI verified. Overall system acceptance remains PARTIAL until outstanding requirement-level cases are executed, documented and signed off.
