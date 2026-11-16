# Release and Verification Status

Last repository review: 16 November 2026 (after Part 3 testing and Sprint 3)

## Release decision

**Status: Release candidate. Testing complete except the Linux CI verdicts for 6 of 66 cases, then sign-off.**

| Area | Status |
|---|---|
| Implementation | All 33 SRS requirements implemented; Sprint 3 closed six spill-over items (`docs/SPRINT_3.md`) |
| Unit tests | 42 functions, 1,198 checks, 0 failures |
| Test Plan v1.2 | 66/66 cases executed: 60 PASS, 0 FAIL, 6 PENDING CI |
| Requirements | 29/33 accepted; 4 functional PASS awaiting Linux memory-tool/portability verdicts |
| Coverage | 92.3% lines, 90.4% functions, 58.9% raw branches (85.7% excluding exception edges) |
| Performance | ≥ 15k ticks/s in every run (target 500), including 8 chained intersections under overload |
| Documentation | Test Plan v1.2 DOCX, execution register, four QA reports, Sprint 3 record, RTM updated |

## What must happen before the project is called accepted

1. Push the changes and let the GitHub Actions workflow finish on Ubuntu and Windows.
2. From the `acceptance-evidence-Linux` artifact, confirm the ASan/UBSan, LeakSanitizer and Valgrind results. From the `portability-compare` job, confirm identical output on Linux and Windows. Then mark TC-Rob-001-P, TC-Rel-01-P/N, TC-Rob-003-P/N and TC-Port-01-P as PASS (or FAIL) in the register, the Test Plan DOCX and the System Validation Report.
3. Team review and sign-off (`docs/SUBMISSION_CHECKLIST.md`, Final sign-off table).

## Verified evidence

- Local execution on 16-11-2026: Windows 11, MSYS2 UCRT64 `g++ 16.2.0`. Evidence in `docs/QA_Reports/evidence/`.
- Real Ctrl+C: open-ended run stopped with exit 0; the summary tick count equals the rendered tick count.
- Defects found during execution and fixed: DEF-04 (console alignment) and DEF-05 (unbounded in-flight transfers, found by TC-Perf-01-N).
- Earlier CI history: main run #99 (436 checks), PR #94 graceful stop, PRs #98–#123 regression tests, main run #243 (985 checks, 88.0% line coverage). See `docs/QA_Reports/Unit_Test_Report.md`.

## Known deviations (documented in Test Plan v1.2 and the SAD addendum)

- Invalid or missing configuration is rejected with a clear error instead of being replaced by defaults (PRJ-SR-002).
- A config containing an invalid intersection is rejected as a whole (fail-fast); runtime isolation between intersections is verified.
- Signal safety uses one Green lane at a time rather than modelled turning-movement conflict sets.
- A full downstream lane holds linked traffic upstream (spillback).
