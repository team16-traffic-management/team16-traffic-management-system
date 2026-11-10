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
- [x] Verify latest main-branch Actions run is green and save its URL. Main commit `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540`; run #99 passed: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604.
- [ ] Confirm backlog, sprint dates, story points and issue statuses in the team's tracker.
- [ ] Confirm each team member's actual role and contribution.
- [ ] Capture required pull-request/review evidence.
- [ ] Record a short working demo and presentation walkthrough if required.
- [ ] Confirm README instructions work on a clean checkout.

## Part 3 — Testing and final documentation
- [x] Unit Test Report records main-branch CI evidence (436 checks passed on Ubuntu and Windows in run #99).
- [ ] System Validation Report reviewed against the complete SRS/Test Plan; remaining NOT RUN cases still require review.
- [ ] Code Coverage Report regenerated from final main-branch coverage output.
- [ ] Branch Coverage Report regenerated from final main-branch coverage output.
- [x] Failed, blocked and not-run scope is disclosed in the current draft reports; review again after final validation.
- [x] Coverage scope names the exact source files included in the initial local GCOV report.
- [x] Local Windows performance benchmark executed; 5 measured runs, median 88,344.30 ticks/sec against 500 ticks/sec target. Evaluator-hardware and connected-intersection performance evidence remain outstanding.
- [x] Linux build evidence confirmed against tested main in run #99.
- [x] Windows build evidence confirmed against tested main in run #99.
- [x] Linux sanitizer result confirmed in main workflow #99; retain workflow link.
- [ ] Final Academic Project Report reviewed by the team.
- [ ] Demo guide and presentation reviewed.
- [ ] Final repository and submission links checked.

## Important evidence notes
- Main-branch CI run #99: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604
- Tested main commit `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540` built successfully on Ubuntu and Windows; both test logs printed 436 checks passed. The Ubuntu sanitizer step passed.
- PR #90 adds an 80-tick serialized-green regression check; PR #91 adds configuration validation boundary cases. Neither alone constitutes full SRS acceptance.
- Local Windows performance benchmark: average 84,371.19 ticks/sec; median 88,344.30 ticks/sec; 500 ticks/sec target met for the measured fixture. Four intersections were independent, console rendering was disabled, and hardware/compiler details were not recorded. Do not present this as evaluator-hardware or connected-topology evidence.
- Existing local GCOV output reports 95.61% line execution across 114 lines in tests/test_traffic_management.cpp and vehicle/VehicleGenerator.cpp. This is not whole-system coverage and must be rerun on the final commit.
- The old testresult.md claim of 310 checks is superseded by directly observed main-branch run #99 output: 436 checks passed on Ubuntu and Windows. Keep PR-run evidence distinct from main-branch evidence.

## Final sign-off
| Review | Name | Date | Status / comments |
|---|---|---|---|
| Developer handoff | [Name] | [Date] | Pending |
| Independent testing | [Name] | [Date] | Pending |
| QA / documentation review | [Name] | [Date] | Pending |
| Faculty / evaluator acceptance, if required | [Name] | [Date] | Pending |
