# QA Test Report – TMS-23

**Issue:** TMS-23 – Generate Vehicles at Randomized Intervals  
**Date:** 2 November 2026  
**Branch:** feature/TMS-23-randomized-vehicle-arrivals  
**Tested by:** QA

## 1. Objective

Verify randomized vehicle arrivals, lane-specific probabilities, and vehicle tracking.

## 2. Test Environment

- Language: C++
- Standard: C++14
- Compiler: g++
- Build system: Makefile

## 3. Tests Executed

| Test | Result |
|---|---|
| Clean build | PASS |
| Automated unit tests | PASS – 310 checks |
| Application smoke test | PASS – 20 ticks |
| AddressSanitizer | PASS – no errors reported |
| UndefinedBehaviorSanitizer | PASS – no errors reported |

## 4. Functional Verification

- Randomized vehicle arrivals.
- Independent lane arrival probabilities.
- Unique vehicle IDs.
- Lane and tick tracking.
- Probability validation.
- Invalid lane handling.
- Arrival-rate updates.
- Reproducible generation using a seed.

## 5. Observations

All 310 automated checks passed. The application compiled and completed its 20-tick simulation successfully. No sanitizer errors were reported.

## 6. Defects Observed

No defects observed during the tests executed.

## 7. Final QA Result

**PASS – TESTED SCOPE**

The current implementation passed the automated tests, smoke test, and sanitizer run. This report does not claim that every possible scenario has been tested.
