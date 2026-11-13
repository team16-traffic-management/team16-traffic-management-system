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
1. **UML use-case diagram:** Section 7.1 and 7.2 describe two use cases in prose, but the extracted document content shows descriptions, not a UML diagram. The repository's `Documents/diagrams/` folder contains only `TMS_Component_Diagram.png`. Inspect the DOCX visually and add at least one actual UML use-case diagram if it is not embedded.
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
2. **UML sequence diagrams:** The two Section 4.1 workflows are numbered prose steps, not UML sequence diagrams. The current `Documents/diagrams/` folder has no sequence-diagram images. Add at least two UML sequence diagrams (for example, vehicle spawn/enqueue and signal update/dequeue/statistics).
3. **Requirement traceability:** No full requirement-to-component/design mapping is present in the extracted SAD content. Add a design traceability table mapping all 33 SRS requirement IDs to components/classes and relevant design sections.
4. **Security architecture:** No dedicated security architecture section is present. Add a proportionate section explaining the local, single-process threat boundary, input validation, bounds checks, memory ownership and error handling. Keep it consistent with the SRS scope and instructor expectations.
5. **API design:** Section 4.2 provides only partial class interface outlines. Expand this into a consistent API/interface description for the actual modules, including key methods, inputs/outputs, ownership and error/return behavior.
6. **Error handling:** Error/overflow behavior is mentioned within workflows, but there is no clear dedicated error-handling design section. Document config parsing failures, invalid values, queue overflow, invalid connections, stop requests and reporting behavior.
7. **Implementation drift:** The SAD describes a small initial module set and `std::queue`-based interface; the final code has a larger SimulationEngine/Intersection/TrafficSignalController architecture and other implementation details. Reconcile the SAD against final source and document deliberate deviations.

## 4. Repository evidence cross-check

Current `main` tree confirms:
- Three DOCX documents are present in `Documents/`.
- Only one separate diagram file is present: `Documents/diagrams/TMS_Component_Diagram.png`.
- Only one test source file is present: `tests/test_traffic_management.cpp`.
- No `tests/data/` config fixture files are present.
- The four QA reports and index are present in `docs/QA_Reports/`.

This tree check does not prove whether images are embedded inside DOCX files. The use-case/sequence diagram observations above are based on the extracted document content and the separate diagram directory; visually inspect the DOCX pages before treating embedded-image absence as conclusive.

## 5. Recommended order to close the Part 1 gaps

1. Confirm with the instructor whether robustness/data-integrity satisfies the requested security objectives, security requirements and security validation for this offline simulation.
2. Add/verify the SRS UML use-case diagram; add two actual UML sequence diagrams to the SAD.
3. Add SAD security architecture, full requirement traceability, complete API design and error-handling design.
4. Reconcile SRS PRJ-SR-002 and Test Plan fallback/rejection behavior.
5. Correct `TC-Gen-003-N`, complete the full RTM, and update Test Plan instructions to match the actual test harness.
6. Add the five referenced test config fixtures or revise the document to use existing reproducible test data.
7. Update the Test Plan with execution results for all 66 cases and obtain team/instructor approvals.

**Do not mark Part 1 fully complete until these gaps are resolved and the final DOCX versions are reviewed.**
