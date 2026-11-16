# Traffic Management System — Demo Guide

Use this guide to build, run and demonstrate the current C++17 application from the repository root.

## 1. Prerequisites
- A C++17-compatible GCC compiler.
- GNU Make.
- Git, to clone the repository.

On Windows, use the MinGW-w64 UCRT64 toolchain used by the GitHub Actions workflow.

## 2. Clean build
```bash
make clean
make all
```

The executable is created at `build/traffic-management`.

## 3. Run the application
Run the default simulation:

```bash
./build/traffic-management
```

Run the supplied multi-intersection example:

```bash
./build/traffic-management examples/multi_intersection.cfg
```

The example configuration defines two intersections and a lane-matched connection. See `README.md` for the supported configuration keys and limits.

## 4. Run automated checks
Build and execute the test suite:

```bash
make test
```

Run the Linux AddressSanitizer and UndefinedBehaviorSanitizer target where the compiler/runtime supports them:

```bash
make sanitize
```

The consolidated test executable in `tests/test_traffic_management.cpp` (42 test functions, 1,198 checks) covers vehicle generation, queue behaviour, configuration validation, signal transitions, adaptive timing, bounded emergency preemption, statistics, console output, multi-intersection transfers with spillback, graceful stop and repeated simulation lifecycles.

To show the Test Plan being executed case by case:

```bash
make acceptance        # prints "TC-xxx | requirement | PASS | observed result" for 62 cases
bash scripts/run_acceptance.sh   # all 66 cases, including a real Ctrl+C and CLI exit codes
```

Good demo moments: TC-Emg-003-P (emergency lane Green within 2 ticks), TC-Multi-002-N (spillback when the downstream lane is full), and TC-Sim-002-N (real Ctrl+C prints a summary for exactly the completed ticks).

## 5. CI demonstration
Open the repository's GitHub Actions tab and select a completed run. Show the tested commit and the Ubuntu and Windows job results. Linux ASan/UBSan runs as a separate Linux-only workflow step; do not imply that the Windows job runs sanitizers.

## 6. Suggested presentation sequence
1. Explain the problem and objectives using the approved SRS.
2. Present the component diagram and module responsibilities.
3. Show the configuration-driven intersection setup.
4. Run the default simulation and the multi-intersection example.
5. Demonstrate the automated test command and explain its actual scope.
6. Show a successful GitHub Actions run.
7. State known limitations accurately and distinguish them from verified behavior.

## 7. Demo integrity
- Run the commands on the machine used for the demonstration before presenting.
- Capture actual application output and test results.
- Do not invent test counts, coverage percentages, CI outcomes or teammate approvals.
- Do not describe unexecuted Test Plan cases as PASS.
