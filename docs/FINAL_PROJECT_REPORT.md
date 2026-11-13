# Traffic Management System — Final Project Report

Course: Software Engineering mini-project  
Team: Team 16  
Repository: https://github.com/team16-traffic-management/team16-traffic-management-system  
Document status: Draft based on the current repository; team review and the items marked for confirmation are still required.

## 1. Problem statement and objectives

This project implements a configurable C++17 simulation of vehicle flow through road intersections. It models vehicle arrivals, lane queues, traffic-signal phases, emergency-vehicle priority, and transfers between connected intersections. The simulation provides a console view of lane and signal status and collects traffic statistics.

The project objectives reflected in the implemented system are to:
- Simulate vehicle arrivals using configurable per-lane arrival probabilities.
- Model bounded FIFO lane queues and prevent vehicles from being processed while their lane is not green.
- Support fixed and adaptive signal timing and emergency-vehicle priority/preemption.
- Configure multiple intersections and lane-matched connections without changing the simulation engine for each topology.
- Report simulation statistics and support a graceful user-requested stop.
- Build and run automated checks on Ubuntu and Windows, with Linux AddressSanitizer and UndefinedBehaviorSanitizer validation.

**Team confirmation required:** Check this description against the approved SRS problem statement, stakeholder assumptions, and measurable objectives. Do not replace the approved SRS wording with this summary if it differs.

## 2. Scope and implemented functionality

### Implemented in the repository
- Vehicle generation with normal/emergency classification and configurable arrival probabilities.
- Vehicle identifiers and per-lane / per-intersection generation state.
- Bounded lane queues with FIFO behavior and overflow rejection.
- Traffic-signal phase control and adaptive green-time behavior.
- Emergency vehicle queue priority and signal-preemption request handling.
- Configurable multi-intersection simulation and lane-matched vehicle transfers.
- Traffic statistics and console status output.
- Graceful stop handling, including open-ended runs when `total_ticks=0`.
- C++17 build, automated C++ test executable, GitHub Actions CI and Linux sanitizer step.

### Partially verified / not fully accepted
Implementation and automated tests exist for the features above, but full system acceptance remains partial. The 66 planned Test Plan cases have not all been individually executed and recorded. Geometric turning-movement conflict safety, invalid-intersection isolation, emergency maximum-wait boundaries, manual Ctrl+C acceptance, visual console usability, and quantified memory-growth/leak evidence remain open.

### Configuration and execution
Build and run from the repository root:

```bash
make clean
make all
./build/traffic-management
```

Run the supplied multi-intersection example:

```bash
./build/traffic-management examples/multi_intersection.cfg
```

Run automated checks:

```bash
make test
make sanitize
```

The configuration supports intersection lane definitions, lane-matched connections, total ticks, arrival rate, emergency probability, adaptive mode and queue capacity. See `README.md` and `docs/DEMO_GUIDE.md` for the current documented configuration and demonstration steps.

## 3. Requirements and design

The project documents under review are stored in `Documents/` (the SRS and Test Plan remain marked Draft; the SAD is marked Submitted):
- Software Requirements Specification: `Documents/SRS_Traffic_Management_System.docx`
- Software Architecture and Design: `Documents/SAD_Traffic_Management_System_v3.docx`
- Test Plan: `Documents/Test_Plan_Traffic_Management_System.docx`
- Component diagram: `Documents/diagrams/TMS_Component_Diagram.png`

The repository's implementation and verification mapping is maintained in:
- `Documents/RTM_Implementation_Status.md`
- `docs/TRACEABILITY.md`

The source tree separates the application entry point, simulation engine, intersection and lane behavior, signal control, statistics, console output, and vehicle generation. The Makefile compiles these modules into the application and the consolidated test executable.

**Document-compliance findings:** A detailed audit is recorded in `docs/DOCUMENT_COMPLIANCE_AUDIT.md`. The SRS contains 23 functional and 5 non-functional requirements, but its RTM has only five sample rows; Section 7 describes use cases in prose, with no clearly evidenced UML use-case diagram in the extracted content. Its 3 objectives and 5 requirements are explicitly robustness/data-integrity rather than a clearly labeled security section. The Test Plan contains 66 detailed positive/negative cases, but explicitly marks all 66 Not Run; it also has a PRJ-SR-002 safe-default/rejection conflict, a mis-mapped `TC-Gen-003-N`, and references five `tests/data/` fixtures that are absent from the current tree. The SAD has component descriptions, an architecture pattern, two textual workflows and partial class interfaces, but no clearly evidenced UML sequence diagrams, full requirement traceability, dedicated security architecture or dedicated error-handling section. The documents also need reconciliation with the final implementation and team/instructor approval. Do not treat Part 1 as fully complete until these items are resolved.

## 4. Implementation

The application entry point is `src/main.cpp`. It installs the simulation signal handlers, constructs the `SimulationEngine`, loads either the default intersection setup or a configuration file, prints the run mode, and starts the simulation.

The implementation is organized into the following modules:

| Module | Repository files | Responsibility |
|---|---|---|
| Application entry | `src/main.cpp` | Startup, configuration selection, error handling and simulation invocation |
| Simulation engine | `include/SimulationEngine.h`, `src/SimulationEngine.cpp` | Coordinates simulation execution and configured intersections |
| Intersections | `include/Intersection.h`, `src/Intersection.cpp` | Represents intersection and lane behavior |
| Lane queues | `include/LaneQueue.h`, `src/LaneQueue.cpp` | Bounded queue operations and vehicle ordering |
| Signal control | `include/TrafficSignalController.h`, `src/TrafficSignalController.cpp` | Signal phases and timing behavior |
| Vehicle model and generation | `vehicle/Vehicle.h`, `vehicle/Vehicle.cpp`, `vehicle/VehicleGenerator.h`, `vehicle/VehicleGenerator.cpp` | Vehicle metadata, arrival generation and identifiers |
| Statistics | `include/StatisticsCollector.h`, `src/StatisticsCollector.cpp` | Simulation metrics |
| Console output | `include/ConsoleView.h`, `src/ConsoleView.cpp` | Tick, lane, signal and queue status |

The build uses GCC with the C++17 language standard and warning flags. The project uses standard-library containers and RAII-oriented ownership; the robustness review documents the safeguards and their current verification limits.

## 5. Testing and results

The repository contains four QA reports, a 66-case execution register (`docs/TEST_EXECUTION_REGISTER.md`), and an automated-test crosswalk (`docs/TEST_CASE_AUTOMATION_CROSSWALK.md`). The crosswalk identifies 33 cases with direct automated checks, 18 with partial evidence and 15 with no direct automated check identified. This does not change the Test Plan's execution status: all 66 cases remain NOT RUN until executed and recorded individually. All 66 cases are initialized as NOT RUN, matching the current Test Plan; the register is ready for execution records but is not evidence of completed testing.
- `docs/QA_Reports/Unit_Test_Report.md`
- `docs/QA_Reports/System_Validation_Report.md`
- `docs/QA_Reports/Code_Coverage_Report.md`
- `docs/QA_Reports/Branch_Coverage_Report.md`

### Automated tests and CI evidence
- Baseline main workflow [#99](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604) reported 436 automated checks passed on Ubuntu and Windows. Default-application and multi-intersection startup smoke tests passed; Ubuntu ASan/UBSan passed.
- Graceful-stop PR #94 workflow [#132](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865) passed Ubuntu and Windows build, automated tests and smoke tests; Ubuntu ASan/UBSan passed. The change was merged to main.
- Regression PRs #98, #99, #101, #102, #104, #105, #107 and #108 added targeted automated checks for adaptive timing boundaries, transfer backpressure, emergency queue ordering, long-lane/high-queue console output, vehicle ID uniqueness, green-only dequeue, exact one-tick transfer latency and repeated simulation lifecycles. Their recorded CI runs passed on Ubuntu and Windows.
- Post-merge main workflow [#163](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797884604) passed Ubuntu and Windows build, automated tests and smoke tests; Ubuntu ASan/UBSan passed.

The 436-check baseline count is not the same as the 66 Test Plan cases. All 66 planned cases still need individual execution records with environment, command, expected result, actual result and PASS/FAIL/BLOCKED/NOT RUN status.

### Performance
A local Windows benchmark ran five measured 10,000-tick runs after a warm-up, using four independent intersections with console rendering disabled:
- Average: 84,371.19 ticks/second.
- Median: 88,344.30 ticks/second.
- Stated target: 500 ticks/second.

This demonstrates the target for that local benchmark fixture only. CPU, RAM, Windows build and compiler metadata were not recorded; the intersections were independent rather than connected. It is not evaluator-hardware or connected-network performance certification.

### Coverage
The existing GCOV report is initial local evidence for only two translation units:
- `tests/test_traffic_management.cpp`: 72/74 executable lines (97.30%).
- `vehicle/VehicleGenerator.cpp`: 37/40 executable lines (92.50%).
- Combined: 109/114 executable lines (95.61%).

The branch report also covers only those two translation units. Neither report is whole-project coverage, and both require a final-main rerun with the exact commit, environment and raw output recorded before being presented as final coverage.

## 6. CI/CD

The workflow is defined in `.github/workflows/ci-cd.yml`. It runs on pushes to `main` and `feature/**`, pull requests targeting those branches, and manual dispatch.

The workflow:
1. Checks out the repository.
2. Sets up the Windows MinGW-w64 UCRT64 toolchain where required.
3. Verifies GCC and Make versions.
4. Builds the application and test executable.
5. Runs the automated test executable.
6. Smoke-tests the default application and the supplied multi-intersection configuration.
7. Runs AddressSanitizer and UndefinedBehaviorSanitizer on Linux.
8. Uploads the Linux application executable as a workflow artifact.

Use the linked workflow runs in Section 5 as evidence. Sanitizers are a Linux-only workflow step; do not claim that the Windows job runs them.

## 7. Team roles and contributions

Jira's TMS board (board ID 67) contains 33 requirement-linked story issues, all with estimates (108 story points in total). All 33 are currently assigned to Khushi Gupta; the team should confirm that this allocation reflects the agreed developer/tester/coordination responsibilities. The board has two closed sprints, but their recorded dates (1–3 Nov and 2–4 Nov 2026) do not match the assignment's 19–23 Oct and 26–30 Oct windows, and they overlap. An additional empty future sprint is present. The GitHub Project requirement must be verified separately; the existence of a Jira board is not proof of a GitHub Project linked to the repository.

The repository's current role allocation is documented in `docs/TEAM_ROLES.md`:

| Role | Assigned member | Responsibility recorded in the repository |
|---|---|---|
| Developer | Khushi Gupta | Feature implementation, source modularity, integration and defect fixes |
| Tester / QA execution | Tejas Shetty | Requirement-based testing, test records, fix verification and defect reporting |
| Jira / CI-CD coordination | Tarun Gowda | Jira workflow, CI/CD coordination and project tracking |

**Team confirmation required:** Each member must verify their role, actual contributions and evidence. The table records the current allocation; it is not independent proof that every listed activity was completed by that person.

## 8. Limitations and future work

The current repository records these open validation and submission items:
- Execute and record all 66 Test Plan cases individually.
- Perform a manual interactive Ctrl+C test and verify the partial-run summary.
- Validate geometric conflicts between turning movements; the existing at-most-one-GREEN-lane invariant is narrower.
- Verify invalid-intersection isolation.
- Verify emergency maximum-wait / preemption-delay boundaries.
- Perform manual visual/usability inspection of console output.
- Retain quantified memory-growth or leak evidence if required; repeated lifecycle execution and sanitizer passes are not a standalone leak measurement.
- Run connected-intersection and evaluator-hardware performance tests if required.
- Regenerate final line and branch coverage on the final main commit.
- Resolve the SRS/Test Plan invalid-configuration conflict and correct the queue-capacity test mapping.
- Confirm GitHub Project linkage, story assignments, sprint date discrepancy, extra empty future sprint, required demo recordings, presentation evidence and team sign-offs.

## 9. Conclusion

The project provides a working C++17 traffic simulation with configurable intersections, vehicle generation, bounded queues, signal control, emergency priority, statistics, console status and graceful stop behavior. The repository contains a cross-platform CI workflow and targeted automated regression evidence, including Linux sanitizer execution. A local benchmark exceeded the stated throughput target for its specific fixture.

The available evidence supports substantial implementation and automated verification progress, but it does not establish complete system acceptance. Final submission readiness depends on completing the individual Test Plan execution records, reconciling the approved requirements/design/test documents, regenerating final coverage, completing the remaining manual validation, and obtaining team review and sign-off.

## Appendix — submission evidence checklist

- Repository: https://github.com/team16-traffic-management/team16-traffic-management-system
- Latest application-changing tested main commit: `4ccfc24ee7ef477454041aa5f7c3954e4ce31020`
- Main CI evidence: [workflow run #163](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797884604)
- Baseline automated test evidence: [workflow run #99](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604)
- Manual-stop implementation CI evidence: [PR #94 workflow run #132](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865)
- Sprint 1 working-product video (one minute): **Team to add the repository path/link; no video file is present in the current repository tree.**
- Sprint 2 working-product video (two minutes): **Team to add the repository path/link; no video file is present in the current repository tree.**
- Final presentation walkthrough: **Team to add the final approved recording/link if required.**
- Team review/sign-off: **Pending confirmation from all members.**


## 8. Latest QA and document alignment

PRs #115–#120 extended the automated suite with sibling-lane isolation, emergency-probability boundaries, queue/statistics boundaries, runtime adaptive-mode changes and adaptive-mode configuration loading. The automation crosswalk now identifies 42 direct automated checks, 18 partial mappings and 6 cases without direct automated checks.

PR #121 merged a reproducible Ubuntu gcovr job and HTML/XML artifacts. After PR #121 merged, main workflow run #235 on commit `1b4804a331247a3ae2f6c2704dd1a31957334e7a` reported 88.0% line coverage (667/758), 86.2% function coverage (81/94) and 54.6% branch coverage (683/1,252). Ubuntu and Windows each passed all 984 automated checks; Linux ASan/UBSan passed. Artifact: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43381117586/artifacts/14016975299. The 66 acceptance cases remain NOT RUN.

Implementation-aligned document revisions have been prepared separately:
- SRS v0.5: full 33-requirement RTM with Not Run statuses and an embedded operator use-case diagram.
- Test Plan v1.1: current parser syntax, real C++ test harness, corrected FIFO boundary case and invalid-config rejection expectations; all 66 cases remain Not Run.
- SAD v3.1: implementation-aligned module/API supplement and embedded sequence diagrams.

The repository's existing DOCX copies must be replaced with those revised files and visually reviewed in Word before final submission. Security-scope interpretation, team sign-offs, demo videos and manual acceptance execution remain outstanding.
