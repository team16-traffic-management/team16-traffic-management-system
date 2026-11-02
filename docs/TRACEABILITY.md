# Requirements Traceability Matrix — Traffic Management System

Purpose: connect requirements to implementation locations and test evidence. A requirement is not verified merely because it appears in a design document.

## Source and test inventory
| Area | Repository location | Evidence scope |
|---|---|---|
| Simulation entry point | src/main.cpp | Application entry point and simulation loop, as described in README. |
| Vehicle generation | vehicle/VehicleGenerator.h and .cpp | Seeded per-lane randomized generation and configuration validation. |
| Automated tests | tests/test_traffic_management.cpp | Tests focused on vehicle generation/configuration; inspect the current file before claiming broader coverage. |
| Build | Makefile | Build, test, sanitizer and coverage-related commands present on the checked-out branch. |
| CI | .github/workflows/ci-cd.yml | Linux build, tests, smoke test, sanitizer and artifact workflow; confirm latest run. |

## Requirement-to-evidence mapping
The identifiers below follow Documents/RTM_Implementation_Status.md. This table does not claim that every requirement has passed acceptance testing.

| Requirement group | Intended behavior | Evidence location | Verification status / gap |
|---|---|---|---|
| TMS-F-001–003 | Vehicle arrivals, metadata and FIFO handling | vehicle/VehicleGenerator.*, src/main.cpp, tests/test_traffic_management.cpp | Vehicle generation has automated tests. Verify queue/FIFO behavior against current source and dedicated tests. |
| TMS-F-010–013 | Lane queues, capacity, green-only dequeue and waiting time | Simulation/queue source, if present | Confirm implementation files and dedicated positive/negative tests. Generator tests alone do not establish coverage. |
| TMS-F-020–024 | Fixed/adaptive signals and safe phases | Signal-controller source, if present | Adaptive timing bounds and phase/conflict scenarios need dedicated evidence. |
| TMS-F-030–032 | Emergency classification, priority and preemption | Emergency-handling source, if present | Independently test maximum-delay/preemption boundary. |
| TMS-F-040–042 | Multi-intersection behavior and reporting | Intersection/configuration source, if present | Exact one-tick transfer latency and stress scenarios need dedicated verification. |
| TMS-F-050–052 | Traffic statistics and reporting | Statistics/reporting source, if present | Record expected-versus-observed values in system tests. |
| TMS-F-060–061 | Configuration, run length and graceful completion | Configuration and simulation source | Malformed/duplicate configuration and graceful-stop behavior need evidence. |
| TMS-NF-001 | Performance | Benchmark output and environment | Not certified until benchmarked on evaluator hardware. |
| TMS-NF-002 | Resource safety / long run | Sanitizer output and long-run test | Retain actual CI run link, date and tested commit. |
| TMS-NF-003–005 | Modularity, usability and portability | Source review, manual review, Linux/Windows build logs | Record each platform and review actually performed. |
| PRJ-SR-001–005 | Bounds safety, validation, RAII and robustness | Source, negative tests and sanitizer output | Expand malformed-input and boundary tests before claiming complete verification. |

## Evidence rules
1. Link each requirement to a real source file and named test case.
2. Record PASS, FAIL, BLOCKED or NOT RUN; do not turn missing evidence into PASS.
3. Record commit SHA, date, compiler, operating system and command for each run.
4. Update this matrix after implementation changes and independent retesting.
5. Coverage for a subset of source files is not whole-system coverage.

## Known verification gaps
The implementation-status document calls out adaptive signal timing bounds, emergency preemption maximum-delay boundary, exact one-tick transfer latency, malformed configuration cases, graceful user stop, evaluator-hardware performance and platform build evidence. Track these until actual evidence exists.