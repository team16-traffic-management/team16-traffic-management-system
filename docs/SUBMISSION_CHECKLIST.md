# Software Engineering Mini-Project Submission Checklist

Use this as the final evidence checklist. Mark an item complete only when the deliverable exists and has been reviewed.

## Part 1 — Requirements and design
- [x] SRS, SAD and Test Plan are present in `Documents/`.
- [x] Component diagram is present in `Documents/diagrams/`.
- [ ] Reconcile SRS, SAD and Test Plan against the final implementation.
- [ ] Resolve the PRJ-SR-002 conflict: the SRS requires invalid configuration rejection with a clear error, while some Test Plan cases expect safe defaults.
- [ ] Correct the Test Plan mapping for `TC-Gen-003-N` (queue capacity belongs to queue-capacity/robustness requirements, not FIFO enqueue).
- [ ] Reconcile requirement IDs and all 66 Test Plan cases with the SRS RTM.
- [ ] Record design deviations and obtain team review.

## Part 2 — Implementation, planning and CI
- [x] C++17 source, Makefile, build scripts and README are present.
- [x] GitHub Actions workflow builds/tests Ubuntu and Windows.
- [x] PR #94 graceful-stop implementation merged to main as `339dfed320036ed1c42a375f70bc1189fb5b4244`.
- [x] PR #94 CI run #132 passed Ubuntu and Windows build, automated tests and smoke tests; Ubuntu ASan/UBSan passed: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865.
- [ ] Capture a fresh push-to-main CI run after the final documentation merges.
- [ ] Confirm backlog, sprint dates, story points and issue statuses in the team's tracker (Tarun).
- [ ] Confirm team roles/contributions with all members.
- [ ] Capture required PR/review evidence and final team approvals.
- [ ] Record the final working demo and presentation walkthrough if required.
- [ ] Verify README build/run instructions from a clean checkout.

## Part 3 — Testing and final documentation
- [x] Unit Test Report records baseline main CI run #99: 436 checks passed on Ubuntu and Windows.
- [x] System Validation Report distinguishes automated CI evidence from manual Test Plan execution.
- [x] RTM updated after PR #94 merge; implementation, automated verification and manual acceptance are separate.
- [ ] Execute and record the 66 Test Plan cases individually; do not infer their results from 436 automated assertions.
- [ ] Perform manual Ctrl+C stop and verify the partial-run summary (TMS-F-061).
- [ ] Test geometric movement conflicts (TMS-F-022); current automated invariant only checks at most one GREEN lane.
- [ ] Test exact transfer latency, full destination queue behavior and invalid-intersection isolation (TMS-F-040–042).
- [ ] Test adaptive timing bounds and emergency maximum-wait/preemption delay (TMS-F-020 / TMS-F-030).
- [ ] Run repeated simulations and retain memory-growth/leak evidence (TMS-NF-002 / PRJ-SR-003).
- [ ] Manually inspect console formatting with long lane names and high queue lengths (TMS-NF-004).
- [x] Local Windows benchmark met 500 ticks/sec for its fixture (median 88,344.30 ticks/sec); connected-topology and evaluator-hardware evidence remain outstanding.
- [ ] Regenerate line/branch coverage on final main and state the exact source scope.
- [ ] Review the final academic report, demo guide and presentation.
- [ ] Verify final repository/submission links and obtain required sign-offs.

## Evidence notes
- Baseline main CI run #99: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604 (436 checks passed on Ubuntu and Windows; smoke tests passed; Ubuntu ASan/UBSan passed).
- Manual-stop PR #94 CI run #132: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865 (Ubuntu and Windows builds/tests/smoke tests passed; Ubuntu ASan/UBSan passed).
- PR #94 merged to main as `339dfed320036ed1c42a375f70bc1189fb5b4244`.
- Local benchmark: five measured 10,000-tick runs; average 84,371.19 and median 88,344.30 ticks/sec against 500 ticks/sec. Four independent intersections, console rendering disabled; CPU/RAM/compiler metadata was not recorded. Not evaluator-hardware or connected-network evidence.
- Existing GCOV result (95.61% line execution across 114 lines) covers only `tests/test_traffic_management.cpp` and `vehicle/VehicleGenerator.cpp`; regenerate against final main before reporting it as final coverage.
- 436 CI assertions are not equivalent to the Test Plan's 66 independently tracked cases.

## Final sign-off
| Review | Name | Date | Status / comments |
|---|---|---|---|
| Developer handoff | Khushi Gupta | Pending | |
| Independent testing | Tejas Shetty | Pending | |
| Jira / CI coordination | Tarun Gowda | Pending | |
| Faculty / evaluator acceptance, if required | Pending | Pending | |
