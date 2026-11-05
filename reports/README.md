# Test and Quality Reports

This folder contains the four testing deliverables. Reports distinguish committed-main evidence, pull-request CI evidence, local coverage measurements and scenarios that have not been run.

| Report | Purpose | Current status |
|---|---|---|
| Unit_Test_Report.md | Unit cases executed against individual units | 355 checks verified on Ubuntu and Windows in PR #87 CI; pre-merge evidence, rerun/confirm after merge |
| System_Validation_Report.md | End-to-end / system-level validation | Partial CI smoke/stress evidence recorded; full SRS-based validation remains incomplete |
| Code_Coverage_Report.md | Line coverage and included source files | Initial local GCOV evidence only; limited to two translation units and requires a final-commit rerun |
| Branch_Coverage_Report.md | Branch execution and outcomes | Initial local GCOV evidence only; limited to two translation units and requires a final-commit rerun |

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

PR #87 workflow run: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42069243817

- Ubuntu and Windows builds and automated tests passed.
- Test executable printed `All 355 checks passed.` on both platforms.
- Default application and multi-intersection startup smoke tests passed on both platforms.
- Linux AddressSanitizer and UndefinedBehaviorSanitizer passed.
- This is PR integration-ref evidence; PR #87 must be merged and final-main evidence reconfirmed before treating it as final submission evidence.

Never describe a local subset's coverage as whole-project coverage. Preserve raw GCOV output and CI artifacts where available. Do not claim unexecuted system scenarios as PASS.
