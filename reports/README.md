# Test and Quality Reports

This folder contains the four testing deliverables. Reports distinguish committed-main evidence, pull-request CI evidence, local coverage measurements and scenarios that have not been run.

| Report | Purpose | Current status |
|---|---|---|
| Unit_Test_Report.md | Unit cases executed against individual units | 355 checks verified on Ubuntu and Windows in PR #87 CI; PR #87 merged as `0c2f9fb`; main-branch workflow confirmation still required |
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

PR #87 updated workflow run #92: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42073258550

PR #87 merge commit: `0c2f9fb5451200fd84abaa5b941a15d1ded81b87`

- Ubuntu and Windows builds and automated tests passed.
- Test executable printed `All 355 checks passed.` on both platforms.
- Default application and multi-intersection startup smoke tests passed on both platforms.
- Linux AddressSanitizer and UndefinedBehaviorSanitizer passed.
- This is PR integration-ref evidence for the final PR #87 head (`863c8c3`). PR #87 is now merged; a main-branch Actions run for merge commit `0c2f9fb` is still required before treating this as final-main evidence.

Never describe a local subset's coverage as whole-project coverage. Preserve raw GCOV output and CI artifacts where available. Do not claim unexecuted system scenarios as PASS.
