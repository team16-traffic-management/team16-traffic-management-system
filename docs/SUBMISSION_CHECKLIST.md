# Software Engineering Mini-Project Submission Checklist

Use this as the final evidence checklist. Mark an item complete only when the deliverable exists and has been reviewed.

## Part 1 — Requirements and design
- [x] SRS, SAD and Test Plan are present in `Documents/`.
- [x] Component diagram is present in `Documents/diagrams/`.
- [x] Review SRS v0.4 content: sections 1–8, 23 functional requirements, 5 non-functional requirements, acceptance criteria, 3 robustness objectives and 5 robustness requirements are present. See `docs/DOCUMENT_COMPLIANCE_AUDIT.md`.
- [x] Add PlantUML use-case diagram source at `Documents/diagrams/TMS_Use_Case_Diagram.puml`.
- [x] Supplied SRS v0.5 revision includes the system-level use-case diagram and full RTM; PDF preview reviewed. Team sign-off remains pending.
- [ ] Confirm whether the SRS robustness/data-integrity objectives and requirements satisfy the assignment's security objectives/requirements requirement; Section 5.1 is explicitly scoped as robustness, not a conventional security section.
- [ ] Replace the SRS DOCX with the supplied v0.5 revision, which includes the complete 33-requirement RTM; statuses remain Not Run until acceptance execution.
- [ ] Update SRS draft status and obtain approval/sign-off.
- [x] Review Test Plan v1.0 content: Sections 1–13, required Sections 3/4/5, Section 5.1, and 66 detailed cases (two per SRS requirement) are present.
- [x] Supplied Test Plan v1.1 clarifies the local configuration-file trust boundary and limits of security validation; instructor confirmation is still required.
- [x] Supplied Test Plan v1.1 changes invalid/missing configuration expectations to clear rejection, matching SRS PRJ-SR-002 and the parser.
- [x] Supplied Test Plan v1.1 maps `TC-Gen-003-N` to FIFO stability when emergency traffic is interleaved; queue-capacity tests remain under TMS-F-011 / PRJ-SR-004.
- [x] Add five current-parser-compatible config fixtures under `tests/data/` (baseline, zero-values, invalid-values, high-load, corrupted) with reproducibility notes.
- [x] Extend CI to run the three valid fixtures and verify the two invalid fixtures are rejected on Ubuntu and Windows.
- [x] Repository Test Plan DOCX updated to v1.2 (includes the v1.1 alignment): Section 6.1 and Appendix B use current parser syntax; zero-capacity validation is separated from valid config fixtures.
- [x] Supplied Test Plan v1.1 documents the consolidated real-class C++ test harness and separates automated CI checks from the 66 acceptance cases.
- [x] Test Plan moved from Draft to v1.2 (executed). Approval/sign-off is still pending (see Sign-off below).
- [x] Review SAD v1.0 content: component responsibilities/descriptions, synchronous tick-driven modular architecture pattern, two textual workflows and partial C++ class interface outlines are present.
- [x] Add two implementation-aligned PlantUML sequence diagram sources under `Documents/diagrams/` (`TMS_Vehicle_Admission_Sequence.puml` and `TMS_Signal_And_Dispatch_Sequence.puml`).
- [x] Supplied SAD v3.1 includes both implementation-aligned sequence diagrams; PDF preview reviewed.
- [x] Draft implementation-aligned SAD addendum with requirement-to-design traceability, local-process security boundary, actual public API summary, ownership/return contracts and error-handling design (`docs/SAD_IMPLEMENTATION_ADDENDUM.md`).
- [x] Supplied SAD v3.1 incorporates the implementation-aligned architecture/API addendum; instructor confirmation of security scope remains pending.
- [x] Document current implementation architecture and module/interface differences in `docs/SAD_IMPLEMENTATION_ADDENDUM.md`.
- [x] Supplied SAD v3.1 contains the implementation-aligned architecture supplement; team approval remains pending.
- [ ] Reconcile all three DOCX documents against final implementation and obtain team review. See `docs/DOCUMENT_COMPLIANCE_AUDIT.md`.
- [x] Resolved in the supplied Test Plan v1.1: invalid and missing configuration inputs are rejected with clear errors, matching parser behavior.
- [x] Corrected `TC-Gen-003-N` in the supplied Test Plan v1.1: it checks FIFO stability when an emergency vehicle is interleaved; queue-capacity tests remain under TMS-F-011 / PRJ-SR-004.
- [x] Supplied SRS v0.5 RTM and Test Plan v1.1 case matrix align all 33 requirement IDs to the 66 planned cases; execution remains Not Run.
- [ ] Record design deviations and obtain team review.

## Part 2 — Implementation, planning and CI
- [x] C++17 source, Makefile, build scripts and README are present.
- [x] GitHub Actions workflow builds/tests Ubuntu and Windows.
- [x] PR #94 graceful-stop implementation merged to main as `339dfed320036ed1c42a375f70bc1189fb5b4244`.
- [x] PR #94 CI run #132 passed Ubuntu and Windows build, automated tests and smoke tests; Ubuntu ASan/UBSan passed: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865.
- [x] Capture fresh push-to-main CI after QA regression merges: run #243 passed Ubuntu/Windows build, tests and smoke tests; Ubuntu passed 985 checks, ASan/UBSan and production-source coverage passed.
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
- [x] Verify README build/run instructions from a clean checkout: main CI run #243 used a fresh checkout and passed compiler preflight, build, tests and smoke tests on Ubuntu/Windows.

## Part 3 — Testing and final documentation

### 1. Test case updation
- [x] Update the earlier test cases to reflect the sprint changes: Test Plan v1.2 revises 38 cases, each with a "Change in v1.2" row (`Documents/Test_Plan_Traffic_Management_System.docx`).
- [x] Update the Test Case Document: every one of the 66 cases now records the automated check, the actual result and the status; RTM status, Section 5.1, Section 6.1 test data and Appendix B updated; revision history rows 1.1 and 1.2 added.

### 2. Testing
- [x] Complete the testing according to the Test Plan: all 66 cases executed individually on 16-11-2026 — 66 PASS, 0 FAIL; Linux-only verdicts from [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798) (`docs/TEST_EXECUTION_REGISTER.md`).
- [x] Unit Test Report: 42 test functions, 1,198 checks, 0 failures (`docs/QA_Reports/Unit_Test_Report.md`).
- [x] System Validation Report: all 33 requirements validated (`docs/QA_Reports/System_Validation_Report.md`).
- [x] Code Coverage Report: 92.3% lines (849/920), 90.4% functions (`docs/QA_Reports/Code_Coverage_Report.md`, HTML in `docs/QA_Reports/coverage/`).
- [x] Branch Coverage Report: 58.9% raw (942/1,600); 85.7% excluding compiler-generated exception branches (`docs/QA_Reports/Branch_Coverage_Report.md`).
- [x] Upload all the reports to the `docs` folder: `docs/QA_Reports/` with `coverage/` and `evidence/`.
- [x] Manual Ctrl+C stop verified with a genuine console Ctrl+C (TC-Sim-002-N).
- [x] Emergency preemption delay verified (TC-Emg-003-P: delay 2 with a configured maximum of 2).
- [x] Invalid-intersection isolation verified (TC-Multi-001-N: fail-fast rejection plus runtime isolation).
- [x] Conflicting-Green safety verified, including forced same-tick overrides (TC-Sig-003-P/N). The design serves one lane at a time; turning-movement modelling is a documented deviation.
- [x] Console usability verified by automated alignment checks (TC-UX-01-P/N) after fixing DEF-04.
- [x] Connected-topology performance measured (TC-Perf-01-N: 8 chained intersections).
- [x] Linux CI verdicts recorded for TC-Rob-001-P, TC-Rel-01-P/N, TC-Rob-003-P/N and TC-Port-01-P (all PASS: Valgrind 0 bytes lost, no ASan/UBSan/LSan reports, identical Linux/Windows output).
- [x] CI run link recorded in the Unit Test, System Validation and Coverage reports: [CI run #246](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43938226798).

### 3. Optional Sprint 3
- [x] Spill-over development completed as Sprint 3: six items DEF-01 to DEF-06 (`docs/SPRINT_3.md`).
- [x] Testing and reports completed in parallel; the two failures found during execution (TC-Perf-01-N, TC-UX-01-N) were fixed and re-executed.
- [ ] Record the Sprint 3 items in the team's backlog tool (GitHub Project / Jira) with owners and story points.

### Sign-off
- [ ] Review and approve `docs/FINAL_PROJECT_REPORT.md` with the full team.
- [ ] Review the final academic report, demo guide and presentation.
- [ ] Verify final repository/submission links and obtain the required sign-offs.

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
- Historical GCOV result (95.61% across 114 lines) covered only two files and is superseded by final main run #235 (667/758 lines; 683/1,252 branches).
- 436 CI assertions are not equivalent to the Test Plan's 66 independently tracked cases.

## Final sign-off
| Review | Name | Date | Status / comments |
|---|---|---|---|
| Developer handoff | Khushi Gupta | Pending | |
| Independent testing | Tejas Shetty | Pending | |
| Jira / CI coordination | Tarun Gowda | Pending | |
| Faculty / evaluator acceptance, if required | To confirm | Pending | Pending |
