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
- [ ] Verify latest main-branch Actions run is green and save its URL. PR #87 was merged as `0c2f9fb5451200fd84abaa5b941a15d1ded81b87`. Confirm and record a main-branch Actions run for this commit before sign-off.
- [ ] Confirm backlog, sprint dates, story points and issue statuses in the team's tracker.
- [ ] Confirm each team member's actual role and contribution.
- [ ] Capture required pull-request/review evidence.
- [ ] Record a short working demo and presentation walkthrough if required.
- [ ] Confirm README instructions work on a clean checkout.

## Part 3 — Testing and final documentation
- [ ] Unit Test Report records PR #87 CI evidence (355 checks passed on Ubuntu and Windows); post-merge main-branch run still needs confirmation.
- [ ] System Validation Report completed from executed system-level cases. Current report records partial CI smoke/stress evidence and remaining NOT RUN cases.
- [ ] Code Coverage Report regenerated from final main-branch coverage output.
- [ ] Branch Coverage Report regenerated from final main-branch coverage output.
- [x] Failed, blocked and not-run scope is disclosed in the current draft reports; review again after final validation.
- [x] Coverage scope names the exact source files included in the initial local GCOV report.
- [ ] Performance test executed on evaluator environment, or limitation recorded.
- [x] Linux build evidence recorded from PR #87 CI; confirm against final main after merge.
- [x] Windows build evidence recorded from PR #87 CI; confirm against final main after merge.
- [x] Linux sanitizer result recorded from PR #87 CI; retain workflow link and confirm against final main after merge.
- [ ] Final Academic Project Report reviewed by the team.
- [ ] Demo guide and presentation reviewed.
- [ ] Final repository and submission links checked.

## Important evidence notes
- PR #87 CI run: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42069243817
- The PR integration ref built successfully on Ubuntu and Windows; both test runs printed 355 checks passed. Linux AddressSanitizer and UndefinedBehaviorSanitizer passed.
- PR #87 was merged into main by squash commit `0c2f9fb5451200fd84abaa5b941a15d1ded81b87`. Its CI evidence is still from the PR integration ref; confirm a main-branch run before final sign-off.
- Existing local GCOV output reports 95.61% line execution across 114 lines in tests/test_traffic_management.cpp and vehicle/VehicleGenerator.cpp. This is not whole-system coverage and must be rerun on the final commit.
- The old testresult.md claim of 310 checks is superseded by the directly observed 355-check PR CI output for the PR integration ref. Keep PR-run evidence distinct from post-merge main-run evidence.

## Final sign-off
| Review | Name | Date | Status / comments |
|---|---|---|---|
| Developer handoff | [Name] | [Date] | Pending |
| Independent testing | [Name] | [Date] | Pending |
| QA / documentation review | [Name] | [Date] | Pending |
| Faculty / evaluator acceptance, if required | [Name] | [Date] | Pending |
