# Traffic Management System — Complete Project Implementation

A native C++17 console simulation covering the project-wide feature areas in the supplied SRS: vehicle arrivals, lane queues, fixed/adaptive signals, emergency priority, multiple intersections, optional intersection links, statistics, configuration and robustness.

## Project structure

- `include/TMS.h` — public interfaces and data models
- `src/TMS.cpp` — implementation of configuration, queues, signals, generator, intersections, statistics, console view and engine
- `src/main.cpp` — command-line entry point
- `simulation.conf` — editable multi-intersection sample configuration
- `tests/test_tms.cpp` — unit/integration/reliability test executable
- `docs/` — the supplied SRS, SAD and Test Plan
- `RTM_IMPLEMENTATION_STATUS.md` — implementation coverage and verification limits

## Requirements and features represented

- Configurable stochastic vehicle arrivals and unique IDs
- Independent bounded lane queues; emergency-first priority and FIFO within each class
- Fixed and adaptive signal timing, yellow transition, conflict groups
- Emergency signal preemption
- Multiple intersections and configurable source-to-destination lane links
- Per-lane, per-intersection and system-wide wait/throughput/queue reporting
- Configuration validation, deterministic seed, standard-library-only implementation
- 10,000-tick integration test

## Build and run (Linux / macOS)

```sh
make
./tms simulation.conf
make test
```

Or use CMake:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Build and run (Windows MinGW)

Install a C++17 compiler and GNU Make, then in PowerShell:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -I include src/TMS.cpp src/main.cpp -o tms.exe
.\tms.exe simulation.conf

g++ -std=c++17 -O2 -Wall -Wextra -I include src/TMS.cpp tests/test_tms.cpp -o tms_tests.exe
.\tms_tests.exe
```

The project uses only the C++ standard library. No database, network service, GUI toolkit or third-party dependency is required.

## Configuration format

One `key=value` entry per line. Blank lines and lines beginning with `#` are ignored.

- `ticks`: number of simulation ticks (1–10,000,000)
- `seed`: deterministic random seed
- `queue_capacity`: maximum vehicles per lane
- `green_duration`, `yellow_duration`, `red_duration`: phase durations in ticks
- `signal_mode`: `fixed` or `adaptive`
- `adaptive_max_green`, `adaptive_factor`: adaptive extension controls
- `emergency_probability`: value from 0 to 1
- `max_preemption_delay`: configured maximum delay (preemption is requested immediately in this implementation)
- `render_every`: console refresh interval in ticks
- `live_output`: `true` or `false`
- `intersection.NAME.lane=LANE,GROUP,ARRIVAL_RATE`: repeat for every lane
- `link=SOURCE_INTERSECTION.SOURCE_LANE -> DEST_INTERSECTION.DESTINATION_LANE`: optional downstream link

Conflict group IDs identify mutually exclusive direction sets. The controller activates one group at a time; lanes in other groups remain red.

## Important verification note

The package has been compiled with GCC in the build environment and its included integration test (including a deterministic 10,000-tick, two-intersection run) has passed there. This does **not** establish that every test case in the supplied Test Plan has passed, nor does it replace a Windows build, sanitizer/Valgrind run, or evaluator acceptance run. See `RTM_IMPLEMENTATION_STATUS.md` for the exact distinction. Do not report unexecuted test cases as passed.
