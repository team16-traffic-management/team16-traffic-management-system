# SRS, SAD and Test Plan Compliance Audit

**Audit basis:** The three DOCX files currently stored under `Documents/`, the Part 1 assignment brief supplied by the team, and the current `main` repository tree. This is a document-compliance review, not a claim that the system has passed acceptance testing.

## Executive finding

The documents are present and contain substantial requirements, design and test-plan material. They are **not yet fully compliant with all explicitly listed Part 1 items**. The largest gaps are the absence of clearly evidenced UML use-case and sequence diagrams, a missing security architecture section, incomplete requirement traceability in the SAD, and a Test Plan that explicitly labels all 66 cases Not Run.

## 1. SRS — `Documents/SRS_Traffic_Management_System.docx`

**Document identity:** Version 0.4, dated 04-09-2026, status **Draft for review**. Course Coordinator approval fields are blank.

### Present
- Descriptive sections 1–3: purpose, scope, audience, definitions, product perspective, functions, user roles, operating environment, constraints and external interfaces.
- Section 4 contains **23 functional requirements** across vehicle generation, queues, signal control, emergency priority, multi-intersection coordination, statistics and simulation control.
- Section 5 contains **5 non-functional requirements** for performance, reliability, maintainability, usability and portability.
- Requirements generally use shall-statements and include acceptance criteria; several criteria are quantitative (for example 500 ticks/second, 10,000 ticks, and configured timing boundaries).
- Section 5.1 contains **3 robustness/data-integrity objectives** and **5 robustness requirements** (PRJ-SR-001 to PRJ-SR-005).

### Gaps / decisions required
1. **UML use-case diagram:** Section 7.1 and 7.2 describe two use cases in prose, but the extracted DOCX content does not evidence an embedded UML diagram. A PlantUML source has now been added at `Documents/diagrams/TMS_Use_Case_Diagram.puml`; render it and embed it in the SRS DOCX, then verify the figure caption and placement.
2. **Security section:** Section 5.1 is explicitly titled “Robustness & Data-Integrity Requirements” and says there is no user-account/network attack surface. It contains enough objectives and requirements numerically (3 and 5), but they are robustness/data-integrity content, not clearly presented as security objectives/requirements. Ask the instructor whether this scoped treatment satisfies Part 1; otherwise add a short security objectives/requirements subsection appropriate to a standalone local simulation, without inventing a network attack surface.
3. **Approval:** The SRS is still marked Draft and the course coordinator approval fields are blank. Obtain review/sign-off or update the status accurately.
4. **RTM:** The SRS RTM shown in Section 8 contains only five sample rows and says it will be completed later. It does not provide a full mapping for all 33 requirements.

## 2. Test Plan — `Documents/Test_Plan_Traffic_Management_System.docx`

**Document identity:** Version 1.0, dated 02-10-2026, status **Draft**. Course Coordinator approval fields are blank.

### Present
- Sections 1–13 are present, including Sections 3 (Features to be Tested), 4 (Features Not to be Tested), 5 (Test Approach/Strategy) and 5.1 (Robustness Validation).
- The plan covers all **33 SRS requirements** and defines **66 detailed cases**: a positive and a negative/boundary case per requirement.
- Section 5.1 defines config validation, queue bounds/overflow, memory safety and fault-isolation validation.
- The document contains test environment, test data management, schedule, deliverables, defect management, roles, risks, assumptions, suspension/resumption criteria and a requirement/test-case traceability section.

### Gaps / decisions required
1. **Execution state:** The document explicitly states that all 66 cases are currently **Not Run**. They must be updated and executed individually; CI assertion totals must not be substituted for case-by-case execution.
2. **Section 5.1 scope:** It is titled “Robustness Validation” and explicitly says there is no authentication or network attack surface. This is useful robustness testing but is not clearly conventional security validation. Confirm with the instructor that this interpretation is accepted or add appropriate local-application security validation.
3. **Conflicting expected behavior:** SRS PRJ-SR-002 says invalid configuration values are rejected with a clear error. Some Test Plan cases expect safe defaults/fallback. These expected results must be reconciled before execution.
4. **Mis-mapped case:** `TC-Gen-003-N` is about queue capacity/overflow, while its requirement ID is TMS-F-003 (FIFO enqueue). Map it to the capacity/robustness requirement (likely TMS-F-011 / PRJ-SR-004) and update the traceability references.
5. **Test data absent:** Section 6.1 says five config sets (baseline, zero values, invalid values, high load and corrupted) are version-controlled in `tests/data/`. The current repository tree contains only `tests/test_traffic_management.cpp`; no `tests/data/` directory or those five config files is present. Add the data files or revise the test plan to reference the actual reproducible fixtures.
6. **Test method drift:** The Test Plan describes module tests with mock collaborators and fixed interfaces such as `IQueueOps`, `ISignalControl`, `IStatsRecorder`, `IVehicleSource` and `IView`. The current repository uses a consolidated C++ test executable; update the strategy and test-data/execution instructions to describe the actual test harness and commands.
7. **RTM completeness:** The plan claims a complete RTM, but the SRS RTM itself is a five-row sample. Reconcile a full 33-requirement RTM with all 66 test-case IDs and the implementation-status matrix.
8. **Approval:** The Test Plan is marked Draft and the course coordinator approval fields are blank. Obtain review/sign-off or update status accurately.

## 3. SAD — `Documents/SAD_Traffic_Management_System_v3.docx`

**Document identity:** “Software Architecture and Design Specification”, Version 1.0, dated 30-09-2026, status **Submitted**.

### Present
- Introduction, purpose, scope, audience and revision history.
- High-level component responsibilities for SimulationEngine, VehicleGenerator, LaneQueue, SignalController and StatisticsCollector.
- Component descriptions and a **Synchronous Tick-Driven Modular Architecture** pattern.
- Section 4.1 includes two numbered textual workflows: vehicle spawning/queueing and signal changes/traffic clearance.
- Section 4.2 includes C++ class interface outlines for LaneQueue and SignalController.
- Project tooling and workflow.

### Gaps / decisions required
1. **Component diagram:** A separate `Documents/diagrams/TMS_Component_Diagram.png` exists. Ensure it is embedded or clearly referenced in the SAD with a figure number and description.
2. **UML sequence diagrams:** The two Section 4.1 workflows are numbered prose steps, not embedded UML sequence diagrams. Two implementation-aligned PlantUML sources now exist: `TMS_Vehicle_Admission_Sequence.puml` and `TMS_Signal_And_Dispatch_Sequence.puml`. Render and embed them in the SAD DOCX, then verify figure captions and placement.
3. **Requirement traceability:** No full requirement-to-component/design mapping is present in the extracted SAD DOCX content. A source-aligned requirement-group-to-component mapping is drafted in `docs/SAD_IMPLEMENTATION_ADDENDUM.md`; review it against the approved SRS and incorporate a complete mapping in the SAD DOCX.
4. **Security architecture:** No dedicated security architecture section is present in the SAD DOCX. A proportionate local-process trust-boundary section is drafted in `docs/SAD_IMPLEMENTATION_ADDENDUM.md`; obtain instructor confirmation that this satisfies the assignment's security requirement and incorporate it into the SAD DOCX.
5. **API design:** Section 4.2 provides only partial class interface outlines. A source-aligned public API summary is drafted in `docs/SAD_IMPLEMENTATION_ADDENDUM.md`; incorporate and review it in the SAD DOCX.
6. **Error handling:** Error/overflow behavior is mentioned within workflows, but there is no clear dedicated error-handling design section in the SAD DOCX. The implementation-aligned addendum now documents config parsing failures, invalid values, queue overflow, invalid connections, stop requests and reporting behavior; incorporate it in the SAD DOCX.
7. **Implementation drift:** The SAD describes a small initial module set and `std::queue`-based interface; the final code has a larger SimulationEngine/Intersection/TrafficSignalController architecture and other implementation details. Reconcile the SAD against final source and document deliberate deviations.

## 4. Repository evidence cross-check

The current `main` tree contains the three DOCX documents, one separate component diagram image, one consolidated C++ test source file, and no `tests/data/` configuration fixtures.

The PR #113 documentation branch additionally contains:
- `Documents/diagrams/TMS_Use_Case_Diagram.puml`
- `Documents/diagrams/TMS_Vehicle_Admission_Sequence.puml`
- `Documents/diagrams/TMS_Signal_And_Dispatch_Sequence.puml`
- `docs/SAD_IMPLEMENTATION_ADDENDUM.md`

These PlantUML files are editable UML source, not rendered images or embedded DOCX figures. The addendum is an implementation-aligned Markdown supplement, not yet an approved replacement for the SAD DOCX. The SRS/SAD/Test Plan DOCX files still need their corresponding diagrams and revised sections embedded and reviewed.

The repository tree alone cannot prove whether the original DOCX files contain embedded images. The diagram sources were added to close the source-artifact gap, but final DOCX page layout and figure placement still require visual review.

## 5. Recommended order to close the Part 1 gaps

1. Confirm with the instructor whether robustness/data-integrity satisfies the requested security objectives, security requirements and security validation for this offline simulation.
2. Render and embed the new UML use-case diagram in the SRS and the two UML sequence diagrams in the SAD.
3. Review and incorporate the SAD implementation addendum (security boundary, requirement mapping, API summary and error handling) into the DOCX, then obtain instructor/team review.
4. Reconcile SRS PRJ-SR-002 and Test Plan fallback/rejection behavior.
5. Correct `TC-Gen-003-N`, complete the full RTM, and update Test Plan instructions to match the actual test harness.
6. Add the five referenced test config fixtures or revise the document to use existing reproducible test data.
7. Update the Test Plan with execution results for all 66 cases and obtain team/instructor approvals.

**Do not mark Part 1 fully complete until these gaps are resolved and the final DOCX versions are reviewed.**
