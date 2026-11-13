# Release and Verification Status

Last repository review: 13 November 2026

## Repository baseline
The repository is a C++17 console-based traffic simulation with a Makefile, automated C++ test suite, GitHub Actions workflow, SRS, SAD, Test Plan, component diagram, benchmark source, reports and submission checklist.

The implementation includes vehicle generation, bounded lane queues, signal phases, emergency preemption, configurable multi-intersection simulation, statistics and graceful user-requested stop. Refer to `Documents/RTM_Implementation_Status.md` for requirement-level status and evidence limitations.

## Verified evidence
- QA PRs #107 and #108 are merged. PR #107 verifies exact one-tick transfer latency (CI #159); PR #108 verifies 20 repeated simulation lifecycles (CI #162); both passed Ubuntu/Windows.
- QA regression PRs #98, #99, #101 and #102 are merged. Their respective Ubuntu/Windows CI runs #141, #142, #143 and #144 passed. Coverage added: adaptive timing boundaries, destination queue backpressure, emergency queue ordering, long lane/high queue console output, vehicle ID uniqueness and engine-level green-only dequeue.
- Post-merge main CI run #163 passed Ubuntu and Windows build, automated tests and smoke tests; Ubuntu ASan/UBSan passed: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797884604.
- Baseline main workflow #99: 436 automated checks passed on Ubuntu and Windows; default-app and multi-intersection startup smoke tests passed; Ubuntu ASan/UBSan passed.
- Manual-stop PR #94: merged to main as `339dfed320036ed1c42a375f70bc1189fb5b4244`. PR workflow run #132 passed Ubuntu and Windows builds, automated tests and smoke tests; Ubuntu ASan/UBSan passed.
- Local Windows performance benchmark: five measured runs, average 84,371.19 and median 88,344.30 ticks/sec against a 500 ticks/sec target for the benchmark fixture only.

## Outstanding acceptance work
- Execute and record all 66 Test Plan cases individually.
- Manually verify Ctrl+C behavior and partial-run summary (TMS-F-061).
- Validate geometric movement conflicts; the existing one-GREEN-lane invariant is narrower than geometric conflict safety.
- Verify invalid-intersection isolation; destination-capacity backpressure and exact one-tick transfer latency are covered by PRs #99 and #107.
- Verify emergency preemption delay limits; adaptive timing boundary cases are covered by PR #98.
- Obtain quantified repeated-run memory-growth/leak evidence if required; PR #108 covers repeated lifecycle execution but is not a leak report.
- Perform manual visual/usability inspection of console formatting; long lane names/high queue lengths are covered by PR #102 automated assertions.
- Benchmark connected intersections and evaluator hardware if required.
- Regenerate final line/branch coverage and record its exact source scope.
- Resolve the SRS/Test Plan invalid-configuration contradiction and correct the queue-capacity test mapping.
- Complete team review, demo/presentation and required sign-offs.

## Release decision
Status: **Implementation and CI verification substantially complete; system acceptance remains PARTIAL.**

Do not call the project fully accepted or submission-ready until the remaining Test Plan cases, document reconciliation and sign-offs are complete.


## Latest QA additions — 13 November 2026

- PR #115 merged: sibling-lane isolation and zero emergency probability; CI #43377370669 passed Ubuntu/Windows.
- PR #116 merged: guaranteed emergency generation boundary; CI #43377755772 passed Ubuntu/Windows and Linux ASan/UBSan.
- PR #117 merged: queue/statistics boundaries and simultaneous all-lane overflow; CI #43378089050 passed Ubuntu/Windows and Linux ASan/UBSan.
- PR #118 merged: runtime adaptive-mode switch; CI #43378320849 passed Ubuntu/Windows and Linux ASan/UBSan.
- PR #119 merged: crosswalk and submission evidence refresh.
- PR #120 merged: adaptive mode loaded from configuration; CI #43379351361 passed Ubuntu/Windows and Linux ASan/UBSan.
- PR #121 is the current open documentation/coverage PR. Its Ubuntu coverage job and Ubuntu build/test/smoke/sanitizer jobs passed in run #43380638801; the Windows job was still in progress at this review point.
- PR #121 coverage artifact reports 88.0% line coverage (667/758), 86.2% function coverage (81/94), and 54.6% branch coverage (683/1,252). This is pre-merge PR-branch evidence; final main coverage is pending.
- Automation crosswalk: 42 direct, 18 partial, 6 without direct automated checks. The 66 Test Plan acceptance cases remain NOT RUN.
