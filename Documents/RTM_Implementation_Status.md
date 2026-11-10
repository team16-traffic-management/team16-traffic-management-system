# SRS / SAD / Test Plan implementation status

This matrix summarizes implementation and verification evidence currently available in the repository. Implementation presence, automated test evidence, manual Test Plan execution and final acceptance are separate statuses. The 436 CI assertions are not equivalent to the 66 planned Test Plan cases.

## Requirement traceability

| Requirement IDs | Implementation coverage | Verification evidence | Current QA status / remaining work |
|---|---|---|---|
| TMS-F-001–003 | Per-lane vehicle generation, IDs/ticks/types and FIFO queueing | Generator probability/rate tests, integration execution and FIFO dequeue check | PARTIAL — explicit unique-ID behavior should be tested if required by the detailed Test Plan |
| TMS-F-010–013 | Bounded lane queues, queue operations, green-phase dequeue and wait calculations | Capacity/overflow/FIFO/empty-dequeue checks; signal-cycle and statistics tests | PARTIAL — add direct engine-level proof that dequeue occurs only on GREEN if the Test Plan expects it |
| TMS-F-020–024 | Fixed/adaptive signal modes and green-yellow-red phase control | Phase-transition tests and PR #90's 80-tick invariant asserting no more than one lane is GREEN | PARTIAL — no geometric movement-conflict matrix test; adaptive timing bounds need dedicated validation |
| TMS-F-030–032 | Emergency vehicle priority and signal preemption | Emergency queue/preemption acceptance, rejection and deferred activation tests | PARTIAL — maximum emergency wait/delay boundary needs a dedicated test |
| TMS-F-040–042 | Multiple intersections, configured lane connections, transfers and statistics | Two-intersection configuration test and successful transfer assertion; CI multi-intersection startup smoke test | PARTIAL — exact one-tick transfer latency, full destination lane and invalid-intersection isolation need dedicated tests |
| TMS-F-050–052 | Throughput, average wait, maximum queue, blocked and preemption statistics | Statistics calculation and report-format checks | PASS for covered calculations; broader end-to-end report workflow remains manual |
| TMS-F-060 | Configuration parsing, validation and configured run length | PR #91 configuration boundary matrix; CI application/config startup smoke tests | PARTIAL — manual Test Plan cases and clear error behavior should be reconciled case-by-case |
| TMS-F-061 | Clean start/termination and final summary for configured or user-requested stop | Merged through PR #94; atomic stop request, SIGINT handling, `total_ticks=0` open-ended mode, actual completed-tick summary, direct-stop and SIGINT regression tests | IMPLEMENTED + AUTOMATED CI VERIFIED — PR #94 workflow run #132 passed Ubuntu and Windows build/tests and smoke tests; Ubuntu ASan/UBSan passed. Manual interactive Ctrl+C acceptance run and full Test Plan case execution remain unrecorded |
| TMS-NF-001 | Performance target of 500 ticks/second | Local Windows benchmark: five 10,000-tick runs; average 84,371.19 and median 88,344.30 ticks/second | PASS for this fixture only — four independent intersections, console rendering disabled; evaluator hardware and connected topology not tested |
| TMS-NF-002 | RAII/STL ownership and long-run stability | 10,000-tick run completed; Linux ASan/UBSan passed in CI run #99 | PARTIAL — no repeated-run leak/memory-growth or Valgrind evidence |
| TMS-NF-003 | Modular source/header structure and config-driven topology | Config-driven multi-intersection setup and transfer test | PASS for covered topology; topology extension and full configuration matrix not exhaustively tested |
| TMS-NF-004 | Console tick/lane/signal status | Console output field assertions | PARTIAL — visual usability, long lane names and high queue formatting require manual checks |
| TMS-NF-005 | C++17 and supported-platform builds | Main workflow #99 built/tested Ubuntu and Windows | PASS for CI environments; clean-checkout evaluator environment remains to be confirmed |
| PRJ-SR-001 | Bounds checking and invalid-index handling | Selected invalid-index and invalid-configuration tests | PARTIAL — full invalid access matrix not demonstrated |
| PRJ-SR-002 | User-supplied configuration values validated and invalid values rejected clearly | PR #91 covers missing/invalid values, numeric boundaries, malformed probabilities, duplicate/unknown keys and invalid connections | PASS for covered inputs; align Test Plan's contradictory safe-default expectations with the SRS rejection behavior |
| PRJ-SR-003 | RAII and memory safety | Linux ASan/UBSan workflow passed | PARTIAL — no repeated-run leak report or full resource-failure injection |
| PRJ-SR-004 | Bounded queues and overflow handling | Queue capacity and overflow rejection checks | PASS for direct queue test; simultaneous all-lane overflow scenario remains untested |
| PRJ-SR-005 | Malformed input does not cause abnormal termination | Malformed and invalid configuration rejection tests; CI application smoke tests | PARTIAL — full user-facing recovery/termination behavior should be checked manually |

## Verified CI and benchmark evidence

### CI evidence

- Baseline main workflow run #99: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604
- Baseline tested main commit: `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540`; 436 automated checks passed on Ubuntu and Windows, both smoke tests passed, and Ubuntu ASan/UBSan passed. Windows sanitizer was skipped.
- Manual-stop PR #94 workflow run #132: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865
- PR #94 head `9b8730c4cb0fdb5f8e0a172e40b16912fff49c53`: Ubuntu and Windows builds, automated tests, default-app smoke tests and config-driven multi-intersection smoke tests passed; Ubuntu ASan/UBSan passed.
- PR #94 was merged to main as `339dfed320036ed1c42a375f70bc1189fb5b4244`.

### Performance benchmark

- Benchmark source merged through PR #93: `benchmarks/performance_benchmark.cpp`.
- Evidence documented through PR #95.
- Five measured runs of 10,000 ticks after one warm-up.
- Average: 84,371.19 ticks/second; median: 88,344.30 ticks/second; target: 500 ticks/second.
- Four intersections were independent, rendering was disabled, and exact hardware/compiler metadata was not recorded. Do not claim evaluator-hardware or connected-intersection performance certification.

### Manual-stop implementation

PR #94: https://github.com/team16-traffic-management/team16-traffic-management-system/pull/94 — merged to `main` as `339dfed320036ed1c42a375f70bc1189fb5b4244`.

The merged implementation adds a process-level SIGINT handler, atomic stop request, open-ended `total_ticks=0` mode, actual completed-tick reporting and automated tests for direct stop request and SIGINT. PR #94 workflow run #132 passed on Ubuntu and Windows; Ubuntu ASan/UBSan passed. Automated CI verification is complete for this change. A separate manual interactive Ctrl+C run is still not evidenced in the repository, so do not claim all manual Test Plan cases are complete.

## Test Plan execution and acceptance

The automated suite's 436 checks are not the same as the Test Plan's 66 positive/negative cases. Record each planned case's actual execution result separately; do not infer a PASS solely from related unit assertions.

Priority remaining evidence:
1. TMS-F-061: implementation and cross-platform CI are complete; perform and record one manual interactive Ctrl+C run and verify the displayed partial summary.
2. TMS-F-022: geometric movement-conflict validation, not merely a maximum-one-GREEN-lane invariant.
3. TMS-F-040–042: one-tick transfer latency, full destination queue behavior and invalid-intersection handling.
4. TMS-F-020 / TMS-F-030: adaptive timing bounds and emergency preemption delay.
5. TMS-NF-002 / PRJ-SR-003: repeated-run memory-growth/leak validation.
6. TMS-NF-004: manual console formatting/usability checks.
7. TMS-NF-001: connected-intersection benchmark and evaluator-hardware run, if available.

The Test Plan currently contains a conflict with the SRS for invalid configuration behavior: SRS PRJ-SR-002 calls for rejection with a clear error, while some Test Plan cases expect safe-default substitution. Resolve the document conflict before marking those cases passed. The Test Plan also describes automated scripts as out of scope even though the repository now has an automated C++ test suite; update its execution-method description.

The SRS RTM and Test Plan RTM must be reconciled case-by-case. Do not mark all 66 cases complete from the 436 automated assertion count. Overall system acceptance remains PARTIAL pending the outstanding cases and sign-off.
