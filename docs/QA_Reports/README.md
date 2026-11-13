# Test and Quality Reports

This folder contains the four testing deliverables. Reports distinguish committed-main evidence, local benchmark evidence, local coverage measurements and scenarios that have not been run.

| Report | Purpose | Current status |
|---|---|---|
| Unit_Test_Report.md | Unit cases executed against individual units | 984 checks passed on Ubuntu and Windows in final main-branch workflow #235; Linux ASan/UBSan passed |
| System_Validation_Report.md | End-to-end / system-level validation | Main CI evidence and local Windows benchmark recorded; full SRS-based validation remains incomplete |
| Code_Coverage_Report.md | Line coverage and included source files | Main CI run #235 generated 88.0% line coverage (667/758) on commit 1b4804a |
| Branch_Coverage_Report.md | Branch execution and outcomes | Main CI run #235 generated 54.6% branch coverage (683/1,252) on commit 1b4804a |

## Evidence metadata

Each report should record:
- Source commit SHA and branch/ref.
- Date and time of execution.
- Operating system and compiler version.
- Exact command used.
- Test executable and source files included.
- Actual pass/fail counts and output.
- Known limitations and cases not run.

## Current CI evidence

Main-branch workflow run #235: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43381117586

Tested commit: `1b4804a331247a3ae2f6c2704dd1a31957334e7a`

- Ubuntu and Windows builds, automated tests and smoke tests passed.
- Both platforms printed `All 984 checks passed.`
- Linux ASan/UBSan passed.
- Coverage job generated 88.0% line coverage (667/758), 86.2% function coverage (81/94), and 54.6% branch coverage (683/1,252); HTML/XML artifact: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43381117586/artifacts/14016975299.

Main-branch workflow run #99: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604

Tested main commit: `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540`

- Ubuntu and Windows builds and automated tests passed.
- Test executable printed `All 436 checks passed.` on both platforms.
- Default application and multi-intersection startup smoke tests passed on both platforms.
- Linux AddressSanitizer and UndefinedBehaviorSanitizer passed.
- PR #90 adds an 80-tick serialized-green regression test; PR #91 adds configuration validation boundary cases. The serialized-green test does not validate geometric turning-movement conflicts.

## Local performance evidence

A standalone benchmark was run on a local Windows machine using `-std=c++17 -O2 -DNDEBUG`. One warm-up and five measured runs each executed 10,000 ticks with four independent intersections and four lanes per intersection; console rendering was disabled.

- Average: 84,371.19 ticks/second.
- Median: 88,344.30 ticks/second.
- SRS target: 500 ticks/second.
- Result: target met for this local benchmark fixture.

The exact CPU, RAM, Windows build and compiler version were not recorded. The intersections were not connected, and this is not evaluator-hardware or full end-to-end performance evidence. See System_Validation_Report.md for the complete run table and limitations.

Never describe the historical two-file GCOV subset as whole-project coverage. The CI job instruments production sources under src/ and vehicle/ (including main.cpp and instrumented VehicleGenerator.h inline code) and uploads HTML/XML reports. PR #121 run #230: 88.0% lines, 54.6% branches; rerun after merge to main. Preserve the artifact and record the exact tested commit. Do not claim unexecuted system scenarios as PASS.
