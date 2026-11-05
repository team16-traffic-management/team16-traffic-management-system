# Unit Test Report

Status: CI-verified for PR #87; pre-merge evidence. This report records the test branch run, not a claim that PR #87 is already part of `main`.

## Test environment

| Field | Value |
|---|---|
| Language standard | C++17 |
| Operating systems | GitHub Actions Ubuntu 26.04 and Windows latest |
| Compiler | GCC 15.2.0 on Ubuntu; MinGW GCC 15.2.0 on Windows |
| Build tool | GNU Make 4.4.1 |
| Test source | `tests/test_traffic_management.cpp` |
| Implementation sources | Vehicle, VehicleGenerator, LaneQueue, TrafficSignalController, StatisticsCollector, Intersection, ConsoleView and SimulationEngine |
| PR branch | `test/signal-statistics-intersection-coverage` |
| PR head commit | `2addefbccb1fd8c65b2ad6d0a62dccc8f0c1a107` |
| CI merge ref tested | `3e3b85ee35fc7b73d77b4b34f738bc6ef1dc19af` |
| Execution date | 5 November 2026 |
| Workflow run | [GitHub Actions run #90](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42069243817) |

## Test scope

The automated test executable covers:

- Vehicle generation: probability boundaries, lane/tick metadata, seeded arrival-rate behavior, configuration updates and invalid inputs.
- Lane queues: capacity, overflow rejection, FIFO dequeue and empty-dequeue handling.
- Configuration and simulation: malformed configuration, multi-intersection setup, transfers, console status and a 10,000-tick run.
- Signal controller: phase transitions, lane rotation, invalid durations and lane registration.
- Emergency handling: valid/invalid preemption requests and deferred activation.
- Statistics: throughput, average wait, blocked events, preemption counts and invalid timestamps.
- Intersection validation: ID, lane name, capacity, duplicate lane and signal registration checks.

## Results

| Result | Ubuntu 26.04 | Windows |
|---|---:|---:|
| Automated checks passed | 355 | 355 |
| Automated checks failed | 0 | 0 |
| Test executable exit status | Success | Success |
| Application build | Passed | Passed |
| Default application smoke test | Passed | Passed |
| Multi-intersection startup smoke test | Passed | Passed |
| AddressSanitizer / UndefinedBehaviorSanitizer | Passed | Not run (Linux-only workflow step) |

The CI log prints `All 355 checks passed.` on both platforms. The Linux sanitizer target also completed successfully. Windows sanitizer execution is not claimed.

## Reproduction commands

```bash
make clean all build/test-traffic-management
./build/test-traffic-management
make sanitize
```

The sanitizer command is run on Ubuntu in the workflow. The application smoke tests use:

```bash
./build/traffic-management
./build/traffic-management examples/multi_intersection.cfg
```

## Limitations

- This is pre-merge PR evidence. Re-run or confirm the workflow after PR #87 is merged into the final submission commit.
- Passing automated checks do not establish that every SRS acceptance criterion is satisfied.
- The 10,000-tick test proves completion in this run; its current non-negative statistics assertions are not meaningful bounds checks for unsigned counters.
- System-level scenarios not explicitly executed are listed as NOT RUN in the System Validation Report.
- GCOV line/branch coverage is documented separately and must not be inferred from these test results.

## Conclusion

The test executable completed successfully with 355 checks on Ubuntu and Windows. Linux AddressSanitizer and UndefinedBehaviorSanitizer also completed successfully. This is verified CI evidence for PR #87's pre-merge integration ref; it is not a full system-validation sign-off.
