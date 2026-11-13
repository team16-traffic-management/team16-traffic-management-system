# Historical QA Test Report — TMS-23

> **Historical record:** This report documents a scoped test run from 2 November 2026. Its C++14 environment and 310-check count are not the current repository baseline and must not be used as current project-wide QA evidence.

**Issue:** TMS-23 — Generate Vehicles at Randomized Intervals  
**Date:** 2 November 2026  
**Branch:** `feature/TMS-23-randomized-vehicle-arrivals`  
**Scope:** Vehicle generation and randomized arrival behavior

## Historical test environment
- Language: C++
- Standard recorded at the time: C++14
- Compiler: g++
- Build system: Makefile

## Historical results
The original scoped report recorded a clean build, 310 automated checks, a 20-tick application smoke test, and AddressSanitizer/UndefinedBehaviorSanitizer passing for that test run.

The recorded functional scope included randomized arrivals, independent lane probabilities, vehicle IDs and tick tracking, probability validation, invalid lane handling, arrival-rate updates and seeded reproducibility.

## Current source of truth
For current test-suite scope, CI evidence, tested commits and limitations, use:
- `reports/Unit_Test_Report.md`
- `reports/System_Validation_Report.md`
- `docs/TRACEABILITY.md`
- `Documents/RTM_Implementation_Status.md`

Do not add the historical 310 checks to current CI totals or describe this scoped report as final system acceptance.
