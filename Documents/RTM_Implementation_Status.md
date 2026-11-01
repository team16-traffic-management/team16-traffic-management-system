# SRS / SAD / Test Plan implementation status

This is a project-wide implementation package, not merely the Sprint 1 statistics/configuration work. The source includes implementations for the functional areas below. The status column describes source coverage; it is not a claim that every acceptance test in the Test Plan has been executed.

| Requirement IDs | Implementation coverage | Verification in included tests |
|---|---|---|
| TMS-F-001–003 | Per-lane Poisson arrivals, IDs/ticks/types, FIFO normal queue | Generator exercised by integration run; FIFO checked |
| TMS-F-010–013 | Independent bounded lane queues, green-only dequeue, wait calculation | Queue capacity/priority and statistics checks; green-cycle check |
| TMS-F-020–024 | Fixed/adaptive modes, conflict groups, green-yellow-red phases | Phase/conflict checks; adaptive mode is not separately exhaustively tested |
| TMS-F-030–032 | Emergency classification, priority queue behavior, signal override and event count | Emergency queue priority and override checked; maximum-delay boundary not independently tested |
| TMS-F-040–042 | Multiple intersections, optional configured lane links, per-intersection and aggregate reporting | Two-intersection 10,000-tick run; downstream link exercised, exact one-tick latency needs dedicated verification |
| TMS-F-050–052 | Average wait, max queue, throughput, blocked and preemption counters | Statistics calculations and report formatting checked |
| TMS-F-060–061 | Configuration parser/validation, configured run length, summary on normal completion | Valid config and invalid range checked; interactive pause/resume and OS-signal graceful stop are not implemented |
| TMS-NF-001 | Performance target | Not certified; benchmark must be run on evaluator hardware |
| TMS-NF-002 | RAII/STL ownership; 10,000-tick run | 10,000-tick test passed; sanitizer/Valgrind not run here |
| TMS-NF-003 | Modular headers/source and config-driven topology | Multiple intersections configured without engine edits |
| TMS-NF-004 | Tick/lane/signal console output | Console view included; visual/manual usability review remains |
| TMS-NF-005 | Standard C++17 source | GCC build checked here; Windows and Linux target-machine builds remain to be recorded |
| PRJ-SR-001–005 | STL bounds-safe containers, config checks, RAII, bounded queues, top-level exception handling | Selected robustness checks included; malformed-config matrix and sanitizer verification remain |

## Test-plan caution

The included `tests/test_tms.cpp` is a compact smoke/integration suite. The repeated invariant loop is not a substitute for 66 independent positive/negative test cases. The full Test Plan must be executed case-by-case and its actual results recorded before claiming full acceptance. In particular, add dedicated tests for adaptive timing bounds, emergency preemption delay, one-tick transfer latency, invalid/duplicate config entries, graceful user stop, and the full multi-intersection stress matrix.
