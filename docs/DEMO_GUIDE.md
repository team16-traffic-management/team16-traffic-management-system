# Traffic Management System — Demo Guide

Purpose: provide a repeatable walkthrough. Confirm commands against a clean checkout before recording.

## 1. Before the demo
- Use the latest reviewed project commit.
- Confirm a C++14-compatible GCC compiler and GNU Make are installed.
- Open a terminal at the repository root.
- Run build and tests before presenting.
- Keep the terminal visible so actual output can be seen.

## 2. Build and run
```bash
make clean
make all
./build/traffic-management
```

If GNU Make is unavailable, use the repository's Windows build script if compatible with the current source, or compile using the documented compiler command. Do not claim a platform build passed unless it was executed.

## 3. Test demonstration
```bash
make test
make sanitize
```

Explain which modules the current automated tests cover. The current unit-test suite is focused on vehicle generation/configuration; do not present it as exhaustive testing of every traffic-management requirement.

## 4. Suggested presentation sequence
1. State the problem and objectives from the approved SRS.
2. Explain the high-level architecture using the component diagram.
3. Walk through the simulation entry point and vehicle-generation module.
4. Run the application and explain the visible simulation output.
5. Run automated tests and explain their scope.
6. Show GitHub Actions and a real successful run, if available.
7. Close with known limitations and future work.

## 5. Evidence to capture
- Application running with visible output.
- Test command and actual test result.
- CI run showing commit and completed jobs.
- Coverage report with included source files clearly named.
- Each team member's contribution / speaking section, as agreed by the team.

## 6. Demo integrity
Do not fabricate test results, screenshots, CI runs, coverage percentages or teammate approvals. If a step fails, record the failure and explain it accurately.