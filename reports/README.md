# Test and Quality Reports

This folder contains the four testing deliverables. Update each report from the final source commit and distinguish actual execution from planned tests.

| Report | Purpose | Current status |
|---|---|---|
| Unit_Test_Report.md | Unit cases executed against individual units | Draft; reconcile exact test count and commit. |
| System_Validation_Report.md | End-to-end / system-level validation | Draft; additional system evidence required. |
| Code_Coverage_Report.md | Line coverage and included source files | Initial local GCOV evidence recorded. |
| Branch_Coverage_Report.md | Branch execution and outcomes | Initial local GCOV evidence recorded. |

## Evidence metadata required
- Source commit SHA / branch
- Date and time of execution
- Operating system and compiler version
- Exact command used
- Test executable and source files included
- Actual pass/fail counts and output
- Known limitations and cases not run

Never describe a local subset's coverage as whole-project coverage. Preserve raw GCOV output and CI artifacts where available.