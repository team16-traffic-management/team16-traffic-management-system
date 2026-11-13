# Traffic Management System

A C++17 console simulation of vehicle flow through configurable road intersections.

## Build and run

Install a C++17 compiler and GNU Make, then run:

```bash
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

Example: `examples/multi_intersection.cfg`.

The engine builds each configured intersection and its lanes from the file; adding another intersection does not require editing the engine or entry-point source.

## Live console status

Each simulation tick prints a clearly labelled intersection and tick, followed by each lane's active signal phase and queue length. Output is flushed after each intersection block so status is visible as the simulation progresses.

## Graceful stop

Set `total_ticks=0` in the configuration for an open-ended run. Press Ctrl+C to request a graceful stop; the simulation completes the current tick and prints a summary using the actual number of completed ticks.

## Tests

```bash
make test
make sanitize
```

CI builds and runs the automated suite on Ubuntu and Windows (MinGW-w64); AddressSanitizer and UndefinedBehaviorSanitizer run on Linux.

## Project documentation

- [Demo guide](docs/DEMO_GUIDE.md)
- [Team roles](docs/TEAM_ROLES.md)
- [Requirements traceability](docs/TRACEABILITY.md)
- [Release and verification status](docs/RELEASE_STATUS.md)
- [Submission checklist](docs/SUBMISSION_CHECKLIST.md)
- [QA reports](docs/QA_Reports/README.md)
- [Submission checklist](docs/SUBMISSION_CHECKLIST.md)
- [Document compliance audit](docs/DOCUMENT_COMPLIANCE_AUDIT.md)
- [66-case test execution register](docs/TEST_EXECUTION_REGISTER.md)
- [Final project report](docs/FINAL_PROJECT_REPORT.md)

The QA reports distinguish automated CI checks from the 66-case Test Plan. A passing build does not imply that every requirement has been manually accepted.
