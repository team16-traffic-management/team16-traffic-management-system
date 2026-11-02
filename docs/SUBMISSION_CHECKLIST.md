# Software Engineering Mini-Project Submission Checklist

Use this as a live checklist. Mark an item complete only after the deliverable exists and has been reviewed.

## Part 1 — Requirements and design
- [x] SRS is present in Documents/.
- [x] SAD is present in Documents/.
- [x] Test Plan is present in Documents/.
- [x] At least one architecture/component diagram is present.
- [ ] Review SRS, SAD and Test Plan against the final implementation.
- [ ] Ensure requirement IDs are consistent across SRS, RTM and test cases.
- [ ] Record design deviations and explain why they were made.

## Part 2 — Planning, implementation and CI/CD
- [x] Source code and build instructions are present.
- [x] GitHub Actions workflow is present.
- [x] Automated test source is present.
- [ ] Verify latest main-branch Actions run is green and save its URL.
- [ ] Confirm backlog, sprint dates, story points and issue statuses in the team's tracker.
- [ ] Confirm each team member's actual role and contribution.
- [ ] Capture required pull-request/review evidence.
- [ ] Record a short working demo and presentation walkthrough if required.
- [ ] Confirm README instructions work on a clean checkout.

## Part 3 — Testing and final documentation
- [ ] Unit Test Report generated from current test run.
- [ ] System Validation Report generated from executed system-level cases.
- [ ] Code Coverage Report generated from current coverage output.
- [ ] Branch Coverage Report generated from current coverage output.
- [ ] Failed, blocked and not-run cases are disclosed.
- [ ] Coverage scope names the exact source files included.
- [ ] Performance test executed on evaluator environment, or limitation recorded.
- [ ] Linux build evidence recorded.
- [ ] Windows build evidence recorded.
- [ ] Sanitizer output retained with date and commit.
- [ ] Final Academic Project Report reviewed by the team.
- [ ] Demo guide and presentation reviewed.
- [ ] Final repository and submission links checked.

## Important evidence note
The local GCOV output shared during project work reports 95.61% line execution across 114 lines in tests/test_traffic_management.cpp and vehicle/VehicleGenerator.cpp. This is not whole-system coverage. The existing test-result document claims 310 checks; reconcile that claim against the exact test executable and commit before repeating it.

## Final sign-off
| Review | Name | Date | Status / comments |
|---|---|---|---|
| Developer handoff | [Name] | [Date] | Pending |
| Independent testing | [Name] | [Date] | Pending |
| QA / documentation review | [Name] | [Date] | Pending |
| Faculty / evaluator acceptance, if required | [Name] | [Date] | Pending |