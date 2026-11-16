# Traffic Management System

A C++17 console simulation of vehicle flow through configurable road intersections.

## Build and run

Install a C++17 compiler and GNU Make, then run the dependency check and build:

```bash
make check-deps
make
./build/traffic-management
```

To load a topology from a configuration file:

```bash
./build/traffic-management examples/multi_intersection.cfg
```

## Multi-intersection configuration

The configuration supports a variable number of intersections. Add an `intersection.<ID>.lanes` entry for each intersection. Lane names are comma-separated. A `connection.<SOURCE>=<DESTINATION>` entry connects matching lane names between two intersections. Shared simulation settings include:

- `total_ticks`: number of simulation ticks (1–10,000,000); set to `0` for an open-ended run that ends when the user requests a stop.
- `arrival_rate`: vehicle arrival probability per lane (0–1).
- `emergency_probability`: emergency-vehicle probability (0–1).
- `adaptive_mode`: `true` or `false`.
- `queue_capacity`: positive queue capacity (1–10,000).

Optional signal-timing keys (added in Sprint 3):

- `green_duration`, `yellow_duration`, `red_duration`: phase lengths in ticks (1–10,000; defaults 10, 3, 10). `red_duration` is the all-red clearance before the next lane turns Green.
- `max_preemption_delay`: maximum ticks before a lane holding an emergency vehicle turns Green (default and minimum: `yellow_duration + 1`). Yellow is never skipped.
- `max_green_extension`: maximum adaptive-mode extension added to `green_duration` (0–10,000; default 15).

Invalid or missing values are rejected with a clear error and a non-zero exit code.

Example: `examples/multi_intersection.cfg`.

The engine builds each configured intersection and its lanes from the file; adding another intersection does not require editing the engine or entry-point source.

## Live console status

Each simulation tick prints a clearly labelled intersection and tick, followed by each lane's active signal phase and queue length. Output is flushed after each intersection block so status is visible as the simulation progresses.

## Graceful stop

Set `total_ticks=0` in the configuration for an open-ended run. Press Ctrl+C to request a graceful stop; the simulation completes the current tick and prints a summary using the actual number of completed ticks.

## Tests

```bash
make test             # unit suite: 42 test functions, 1,198 checks
make acceptance       # the 62 in-process Test Plan v1.2 cases, one result line per TC ID
make acceptance-full  # all 66 cases incl. CLI exit codes, real Ctrl+C, memory tools; writes docs/QA_Reports/evidence
make coverage         # line/branch coverage (requires gcovr: pip install gcovr)
make sanitize         # Linux: unit + acceptance suites under ASan/UBSan
```

CI builds and runs the unit and acceptance suites on Ubuntu and Windows (MinGW-w64). On Linux it also runs AddressSanitizer, UndefinedBehaviorSanitizer, LeakSanitizer and Valgrind, and it compares the simulation output from both platforms.

On Windows Git Bash, put your compiler's `bin` folder (for example `/c/msys64/ucrt64/bin`) first on `PATH` when running binaries by hand. Git for Windows ships an older `libstdc++-6.dll` that can crash optimised builds. The scripts handle this automatically.

## Project documentation

- [Demo guide](docs/DEMO_GUIDE.md)
- [Team roles](docs/TEAM_ROLES.md)
- [Requirements traceability](docs/TRACEABILITY.md)
- [Full 33-requirement RTM](Documents/RTM_Implementation_Status.md)
- [Release and verification status](docs/RELEASE_STATUS.md)
- [QA reports — Part 3 (unit, system validation, code and branch coverage)](docs/QA_Reports/README.md)
- [Sprint 3 record](docs/SPRINT_3.md)
- [Submission checklist](docs/SUBMISSION_CHECKLIST.md)
- [Document compliance audit](docs/DOCUMENT_COMPLIANCE_AUDIT.md)
- [SAD implementation addendum](docs/SAD_IMPLEMENTATION_ADDENDUM.md)
- [SAD UML diagram sources](Documents/diagrams/)
- [Reproducible config fixtures](tests/data/README.md)
- [66-case test execution register](docs/TEST_EXECUTION_REGISTER.md)
- [Test Plan to automated test crosswalk](docs/TEST_CASE_AUTOMATION_CROSSWALK.md)
- [Final project report](docs/FINAL_PROJECT_REPORT.md)

Test Plan v1.2 status (16-11-2026): 66/66 cases executed — 66 PASS, 0 FAIL (Linux verdicts from [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798)). See the [execution register](docs/TEST_EXECUTION_REGISTER.md).
