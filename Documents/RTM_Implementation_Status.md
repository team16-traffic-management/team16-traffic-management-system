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
| TMS-F-061 | Clean start/termination and final summary for configured or user-requested stop | Implementation is in open PR #94; latest branch adds a stop request, SIGINT handler, zero-tick open-ended mode, partial-run summary and related automated tests | PENDING — PR #94 CI run #128 was still in progress at this update; review and merge only after final CI and review. Latest diff's zero-tick config test writes literal `\\n` sequences rather than newline characters and should be corrected before approval |
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

### Main-branch CI

- Workflow run #99: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604
- Tested main commit: `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540`
- Automated test executable reported 436 checks passed on Ubuntu and Windows.
- Default application and config-driven multi-intersection startup smoke tests passed on both platforms.
- Ubuntu AddressSanitizer and UndefinedBehaviorSanitizer passed.
- Windows sanitizer was not run by the workflow.

### Performance benchmark

- Benchmark source merged through PR #93: `benchmarks/performance_benchmark.cpp`.
- Evidence documented through PR #95.
- Five measured runs of 10,000 ticks after one warm-up.
- Average: 84,371.19 ticks/second; median: 88,344.30 ticks/second; target: 500 ticks/second.
- Four intersections were independent, rendering was disabled, and exact hardware/compiler metadata was not recorded. Do not claim evaluator-hardware or connected-intersection performance certification.

### Manual-stop implementation

PR #94: https://github.com/team16-traffic-management/team16-traffic-management-system/pull/94

The latest PR diff adds a process-level SIGINT handler, atomic stop request, open-ended `total_ticks=0` mode, actual completed-tick reporting and automated tests for direct stop request and SIGINT. At the time this matrix was refreshed, workflow run #128 was still in progress. Do not mark TMS-F-061 as accepted until the test issue noted above is corrected, CI passes, the PR is reviewed and the change is merged.

## Test Plan execution and acceptance

The automated suite's 436 checks are not the same as the Test Plan's 66 positive/negative cases. Record each planned case's actual execution result separately; do not infer a PASS solely from related unit assertions.

Priority remaining evidence:
1. TMS-F-061: manual stop, SIGINT handling and accurate partial-run summary after PR #94 is corrected, passes CI and is merged.
2. TMS-F-022: geometric movement-conflict validation, not merely a maximum-one-GREEN-lane invariant.
3. TMS-F-040–042: one-tick transfer latency, full destination queue behavior and invalid-intersection handling.
4. TMS-F-020 / TMS-F-030: adaptive timing bounds and emergency preemption delay.
5. TMS-NF-002 / PRJ-SR-003: repeated-run memory-growth/leak validation.
6. TMS-NF-004: manual console formatting/usability checks.
7. TMS-NF-001: connected-intersection benchmark and evaluator-hardware run, if available.

The Test Plan currently contains a conflict with the SRS for invalid configuration behavior: SRS PRJ-SR-002 calls for rejection with a clear error, while some Test Plan cases expect safe-default substitution. Resolve the document conflict before marking those cases passed. The Test Plan also describes automated scripts as out of scope even though the repository now has an automated C++ test suite; update its execution-method description.

The SRS RTM and Test Plan RTM must be reconciled case-by-case. Do not mark all 66 cases complete from the 436 automated assertion count. Overall system acceptance remains PARTIAL pending the outstanding cases and sign-off.
