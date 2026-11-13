# Software Engineering Mini-Project Submission Checklist

Use this as the final evidence checklist. Mark an item complete only when the deliverable exists and has been reviewed.

## Part 1 — Requirements and design
- [x] SRS, SAD and Test Plan are present in `Documents/`.
- [x] Component diagram is present in `Documents/diagrams/`.
- [x] Review SRS v0.4 content: sections 1–8, 23 functional requirements, 5 non-functional requirements, acceptance criteria, 3 robustness objectives and 5 robustness requirements are present. See `docs/DOCUMENT_COMPLIANCE_AUDIT.md`.
- [x] Add PlantUML use-case diagram source at `Documents/diagrams/TMS_Use_Case_Diagram.puml`.
- [ ] Render and embed the use-case diagram in the SRS DOCX and verify its figure caption/placement; Section 7 currently provides textual use-case descriptions.
- [ ] Confirm whether the SRS robustness/data-integrity objectives and requirements satisfy the assignment's security objectives/requirements requirement; Section 5.1 is explicitly scoped as robustness, not a conventional security section.
- [ ] Complete the SRS RTM: Section 8 currently contains only five sample rows for 33 requirements.
- [ ] Update SRS draft status and obtain approval/sign-off.
- [x] Review Test Plan v1.0 content: Sections 1–13, required Sections 3/4/5, Section 5.1, and 66 detailed cases (two per SRS requirement) are present.
- [ ] Confirm/add security validation to Test Plan Section 5.1; current content is robustness/data-integrity validation.
- [ ] Reconcile the SRS PRJ-SR-002 rejection requirement with Test Plan cases that expect safe defaults/fallback.
- [ ] Correct `TC-Gen-003-N`: it tests capacity/overflow but maps to FIFO requirement TMS-F-003 instead of TMS-F-011 / PRJ-SR-004.
- [x] Add five current-parser-compatible config fixtures under `tests/data/` (baseline, zero-values, invalid-values, high-load, corrupted) with reproducibility notes.
- [x] Extend CI to run the three valid fixtures and verify the two invalid fixtures are rejected on Ubuntu and Windows.
- [ ] Update Test Plan Section 6.1 and Appendix B to match the current parser syntax; zero-capacity behavior must be tested through LaneQueue, not config.
- [ ] Update Test Plan method/interfaces to match the current consolidated C++ test harness; its mock interfaces and execution assumptions differ from the final repo.
- [ ] Update Test Plan from Draft and obtain approval/sign-off.
- [x] Review SAD v1.0 content: component responsibilities/descriptions, synchronous tick-driven modular architecture pattern, two textual workflows and partial C++ class interface outlines are present.
- [x] Add two implementation-aligned PlantUML sequence diagram sources under `Documents/diagrams/` (`TMS_Vehicle_Admission_Sequence.puml` and `TMS_Signal_And_Dispatch_Sequence.puml`).
- [ ] Render and embed both sequence diagrams in the SAD DOCX and verify figure captions/placement.
- [x] Draft implementation-aligned SAD addendum with requirement-to-design traceability, local-process security boundary, actual public API summary, ownership/return contracts and error-handling design (`docs/SAD_IMPLEMENTATION_ADDENDUM.md`).
- [ ] Review and incorporate the addendum into the SAD DOCX; confirm the security section meets the instructor's interpretation.
- [x] Document current implementation architecture and module/interface differences in `docs/SAD_IMPLEMENTATION_ADDENDUM.md`.
- [ ] Incorporate the documented implementation changes into the SAD DOCX and obtain team approval.
- [ ] Reconcile all three DOCX documents against final implementation and obtain team review. See `docs/DOCUMENT_COMPLIANCE_AUDIT.md`.
- [ ] Resolve the PRJ-SR-002 conflict: the SRS requires invalid configuration rejection with a clear error, while some Test Plan cases expect safe defaults.
- [ ] Correct the Test Plan mapping for `TC-Gen-003-N` (queue capacity belongs to queue-capacity/robustness requirements, not FIFO enqueue).
- [ ] Reconcile requirement IDs and all 66 Test Plan cases with the SRS RTM.
- [ ] Record design deviations and obtain team review.

## Part 2 — Implementation, planning and CI
- [x] C++17 source, Makefile, build scripts and README are present.
- [x] GitHub Actions workflow builds/tests Ubuntu and Windows.
- [x] PR #94 graceful-stop implementation merged to main as `339dfed320036ed1c42a375f70bc1189fb5b4244`.
- [x] PR #94 CI run #132 passed Ubuntu and Windows build, automated tests and smoke tests; Ubuntu ASan/UBSan passed: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865.
- [x] Capture fresh push-to-main CI after QA regression merges: run #163 passed Ubuntu/Windows build, tests and smoke tests; Ubuntu ASan/UBSan passed.
- [x] PR #110 repository documentation/maintenance cleanup merged to main as `03b8c7f9ca4f168ec86c122a9cbc3c887d309cb7`.
- [ ] Verify the required GitHub Project is linked to the repository; a separate Jira TMS board (board ID 67) exists, but that does not by itself prove the GitHub Project requirement.
- [x] Jira TMS board contains 33 SRS-related story issues; all 33 have story-point estimates (108 points total).
- [ ] Review Jira story assignment: all 33 story issues are currently assigned to Khushi Gupta; confirm whether this matches the team's agreed allocation and the requirement to assign stories to team members.
- [ ] Reconcile Jira Sprint 1 dates: the board's closed `Sprint 1 — Core Simulation` (ID 101) ran 1–3 Nov 2026, not the required 19–23 Oct 2026; confirm whether the date discrepancy is a Jira setup error or requires an explanation.
- [ ] Upload the required one-minute working-product Sprint 1 video to the repository; no video file is present in the current repository tree.
- [ ] Reconcile Jira Sprint 2 dates: the closed `Sprint 2 — Advanced Features` (ID 134) ran 2–4 Nov 2026, not the required 26–30 Oct 2026; it overlaps Sprint 1. An additional empty future sprint `Sprint 2 — Advanced Features 2` (ID 135) also exists; confirm and clean up the planning record.
- [ ] Verify Sprint 2 completion and development freeze.
- [ ] Upload the required two-minute working-product Sprint 2 video to the repository; no video file is present in the current repository tree.
- [ ] Confirm team roles/contributions with all members and obtain final approvals.
- [ ] Record the final presentation walkthrough if required.
- [ ] Verify README build/run instructions from a clean checkout.

## Part 3 — Testing and final documentation
- [x] Unit Test Report records baseline main CI run #99: 436 checks passed on Ubuntu and Windows.
- [x] System Validation Report distinguishes automated CI evidence from manual Test Plan execution.
- [x] RTM updated after PR #94 merge; implementation, automated verification and manual acceptance are separate.
- [ ] Update the earlier test cases and Test Case Document to reflect changes made during the sprints.
- [x] Create `docs/TEST_EXECUTION_REGISTER.md` with all 66 Test Plan case IDs, owners and initial NOT RUN status.
- [x] Refresh `docs/TEST_CASE_AUTOMATION_CROSSWALK.md` after PRs #115–#118: 41 cases have direct automated checks, 18 have partial evidence, and 7 have no direct automated check identified. This remains a mapping, not execution of all 66 Test Plan cases.
- [ ] Execute and record the 66 Test Plan cases individually; do not infer their results from 436 automated assertions.
- [ ] Perform manual Ctrl+C stop and verify the partial-run summary (TMS-F-061).
- [ ] Test geometric movement conflicts (TMS-F-022); current automated invariant only checks at most one GREEN lane.
- [x] Test destination queue backpressure (PR #99; CI run #142 passed).
- [x] Verify exact one-tick transfer latency (PR #107; CI run #159 passed).
- [ ] Verify invalid-intersection isolation (TMS-F-040–042).
- [x] Test adaptive timing boundaries (PR #98; CI run #141 passed).
- [ ] Verify emergency maximum-wait/preemption delay (TMS-F-030).
- [x] Test emergency queue priority and FIFO ordering (PR #101; CI run #143 passed).
- [x] Execute repeated simulation create/run/destroy lifecycle (PR #108; CI run #162 passed).
- [ ] Retain quantified memory-growth/leak evidence if required (TMS-NF-002 / PRJ-SR-003); repeated lifecycle execution is not a leak report.
- [x] Automated console assertions for long lane names/high queue lengths (PR #102; CI run #144 passed).
- [ ] Manually inspect visual console usability (TMS-NF-004).
- [x] Verify vehicle ID uniqueness across multiple lanes/intersections (PR #104; CI run #153 passed).
- [x] Verify engine dequeues vehicles only on green (PR #105; CI run #154 passed).
- [x] Local Windows benchmark met 500 ticks/sec for its fixture (median 88,344.30 ticks/sec); connected-topology and evaluator-hardware evidence remain outstanding.
- [ ] Regenerate line/branch coverage on final main and state the exact source scope.
- [x] Place the four QA reports in `docs/QA_Reports/`, satisfying Part 3's requirement to upload the reports to the `docs` folder. Coverage values remain initial/limited and must still be regenerated on final main.
- [x] Draft `docs/FINAL_PROJECT_REPORT.md` from repository, CI, QA and Jira evidence.
- [ ] Review and approve `docs/FINAL_PROJECT_REPORT.md` with the full team.
- [ ] Review the final academic report, demo guide and presentation.
- [ ] Verify final repository/submission links and obtain required sign-offs.

## Latest QA regression evidence
- PR #115 sibling-lane isolation and zero-emergency-probability checks: https://github.com/team16-traffic-management/team16-traffic-management-system/pull/115 (CI run #43377370669 passed Ubuntu and Windows).
- PR #116 emergency-probability-one boundary check: https://github.com/team16-traffic-management/team16-traffic-management-system/pull/116 (CI run #43377755772 passed Ubuntu, Windows and Linux ASan/UBSan).
- PR #117 queue-boundary, zero-throughput reporting and simultaneous-overflow checks: https://github.com/team16-traffic-management/team16-traffic-management-system/pull/117 (CI run #43378089050 passed Ubuntu, Windows and Linux ASan/UBSan).
- PR #118 runtime adaptive-mode switch check: https://github.com/team16-traffic-management/team16-traffic-management-system/pull/118 (CI run #43378320849 passed Ubuntu and Windows; Linux ASan/UBSan passed).
- PR #98 adaptive timing boundaries: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42791702831
- PR #99 transfer backpressure: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42792945910
- PR #101 emergency queue priority/order: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42795154445
- PR #102 long lane/high queue console output: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42795769676
- PR #104 vehicle ID uniqueness: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42796601039
- PR #105 green-only dequeue: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42796698371
- PR #107 exact one-tick transfer latency: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797325343
- PR #108 repeated simulation lifecycle: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797660386
- Post-merge main CI: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797884604

## Evidence notes
- Baseline main CI run #99: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604 (436 checks passed on Ubuntu and Windows; smoke tests passed; Ubuntu ASan/UBSan passed).
- Manual-stop PR #94 CI run #132: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865 (Ubuntu and Windows builds/tests/smoke tests passed; Ubuntu ASan/UBSan passed).
- PR #94 merged to main as `339dfed320036ed1c42a375f70bc1189fb5b4244`.
- PR #110 repository documentation/maintenance cleanup merged to main as `03b8c7f9ca4f168ec86c122a9cbc3c887d309cb7`.
- Local benchmark: five measured 10,000-tick runs; average 84,371.19 and median 88,344.30 ticks/sec against 500 ticks/sec. Four independent intersections, console rendering disabled; CPU/RAM/compiler metadata was not recorded. Not evaluator-hardware or connected-network evidence.
- Existing GCOV result (95.61% line execution across 114 lines) covers only `tests/test_traffic_management.cpp` and `vehicle/VehicleGenerator.cpp`; regenerate against final main before reporting it as final coverage.
- 436 CI assertions are not equivalent to the Test Plan's 66 independently tracked cases.

## Final sign-off
| Review | Name | Date | Status / comments |
|---|---|---|---|
| Developer handoff | Khushi Gupta | Pending | |
| Independent testing | Tejas Shetty | Pending | |
| Jira / CI coordination | Tarun Gowda | Pending | |
| Faculty / evaluator acceptance, if required | To confirm | Pending | Pending |
