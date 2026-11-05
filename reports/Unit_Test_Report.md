# Unit Test Report

Status: PR #87 merged; this report records successful PR CI evidence. Post-merge main-branch workflow confirmation remains outstanding.

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
| PR head commit | `863c8c3abbe374e133168164cb7bed59bab9ceff` |
| CI merge ref tested | `74f8252dd534fa259a1c669fc0b92cc96e7d5132` |
| Execution date | 5 November 2026 |
| Workflow run | [GitHub Actions run #92](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42073258550) |

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

- PR #87 was merged as `0c2f9fb5451200fd84abaa5b941a15d1ded81b87`. The cited CI run is still PR integration-ref evidence; confirm a main-branch workflow run for the merge commit before final submission.
- Passing automated checks do not establish that every SRS acceptance criterion is satisfied.
- The 10,000-tick test proves completion in this run; its current non-negative statistics assertions are not meaningful bounds checks for unsigned counters.
- System-level scenarios not explicitly executed are listed as NOT RUN in the System Validation Report.
- GCOV line/branch coverage is documented separately and must not be inferred from these test results.

## Conclusion

The test executable completed successfully with 355 checks on Ubuntu and Windows. Linux AddressSanitizer and UndefinedBehaviorSanitizer also completed successfully. This is verified CI evidence for PR #87's pre-merge integration ref; it is not a full system-validation sign-off.
