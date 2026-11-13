# Test Plan to Automated Test Crosswalk

**Repository baseline:** `main` after PR #113 merge, commit `1f8932fbf2ce8c8bc4d332daf446f4774cbfb6db` plus the test source revision inspected for this crosswalk.  
**Test source reviewed:** `tests/test_traffic_management.cpp`.  
**Purpose:** Identify which of the 66 planned Test Plan cases have related automated assertions in the consolidated test executable or CI. This is a coverage map, not a test-execution report.

## Summary

| Classification | Cases | Meaning |
|---|---:|---|
| Direct automated check | 40 | A named automated test checks the core behavior described by the case. |
| Partial / related evidence | 18 | Related behavior is checked, but one or more explicit preconditions, outputs or acceptance criteria are not asserted. |
| No direct automated check identified | 8 | No corresponding check was found in the reviewed test source/CI workflow. |
| **Total** | **66** | |

**Important:** All 66 Test Plan cases remain `NOT RUN` in `docs/TEST_EXECUTION_REGISTER.md`. Even a direct unit-test mapping does not automatically count as a complete Test Plan case execution. Each case still needs its specified test data, steps, expected-vs-actual result, environment and evidence recorded. The 436 historical CI assertions are not 66 separately executed acceptance cases.

## Case-by-case crosswalk

| Test Plan case | Existing automated test / evidence | Classification | Coverage note |
|---|---|---|---|
| TC-Gen-001-P | `testArrivalRates` | Direct | Checks observed per-lane generation rates against configured probabilities. |
| TC-Gen-001-N | `testProbabilityBoundaries` | Direct | Checks that a zero-probability lane generates no vehicles while a probability-1 lane does. |
| TC-Gen-002-P | `testVehicleIdsAreUniqueAcrossLanesAndIntersections; testProbabilityBoundaries` | Direct | Checks generated vehicle IDs, lane identity and arrival ticks. |
| TC-Gen-002-N | `testVehicleIdsAreUniqueAcrossLanesAndIntersections` | Direct | Checks unique IDs for same-tick generation across lanes and intersections. |
| TC-Gen-003-P | `testQueueCapacityAndEmptyDequeue; testEmergencyQueuePriorityAndStableOrdering` | Direct | Checks FIFO order for normal vehicles and stable FIFO within emergency/normal groups. |
| TC-Gen-003-N | `testQueueCapacityAndEmptyDequeue` | Direct, but mis-mapped | Capacity overflow is rejected; Test Plan currently maps this case to TMS-F-003 instead of the capacity/robustness requirement. |
| TC-Queue-001-P | `testSiblingLaneQueueIsolation` | Direct | Checks that sibling lane queues retain independent contents during enqueue/dequeue operations. |
| TC-Queue-001-N | `testSiblingLaneQueueIsolation` | Direct | Checks that operations on North do not change or corrupt South queue state. |
| TC-Queue-002-P | `testQueueCapacityAndEmptyDequeue` | Direct | Checks accepted enqueue at capacity and rejection after the queue is full. |
| TC-Queue-002-N | `testIntersectionValidationAndSignalRegistration; testConfigurationValidationBoundaries` | Direct | Checks zero lane/configured queue capacity is rejected; note the configuration parser rejects zero capacity before simulation. |
| TC-Queue-003-P | `testEngineDequeuesVehiclesOnlyOnGreen` | Direct | Checks throughput increases only when the lane is green. |
| TC-Queue-003-N | `testQueueCapacityAndEmptyDequeue` | Partial | Checks LaneQueue throws on empty dequeue, but not that the engine safely skips an empty green lane. |
| TC-Queue-004-P | `testStatisticsCalculations` | Direct | Checks average wait-time calculation from recorded dequeue events. |
| TC-Queue-004-N | `testStatisticsCalculations` | Partial | Checks empty statistics return zero average; does not explicitly test a single immediate-dequeue wait. |
| TC-Sig-001-P | `testSignalPhaseTransitions` | Direct | Checks Green → Yellow → Red and rotation to the next lane. |
| TC-Sig-001-N | `testSignalPhaseTransitions` | Partial / expected-result conflict | Checks non-positive signal duration is rejected, not that a zero-duration config falls back and continues. |
| TC-Sig-002-P | `testAdaptiveGreenExtensionBounds` | Direct | Checks adaptive green extension for a long queue. |
| TC-Sig-002-N | `testAdaptiveGreenExtensionBounds` | Direct | Checks the extension cap and restoration to the base duration. |
| TC-Sig-003-P | `testNoSimultaneousGreenSignals` | Direct | Checks at most one lane is green over repeated phase cycles. |
| TC-Sig-003-N | `—` | Not identified | No direct rapid/mid-run configuration-change test was found. |
| TC-Sig-004-P | `testSignalPhaseTransitions` | Direct | Checks the Yellow phase occurs between Green and Red. |
| TC-Sig-004-N | `testSignalPhaseTransitions; testConsoleStatusFields` | Partial | A one-tick Yellow duration is exercised, but the exact one-tick Yellow console rendering is not asserted. |
| TC-Sig-005-P | `—` | Not identified | No direct valid config-mode-switch test was found. |
| TC-Sig-005-N | `testConfigurationValidationBoundaries` | Partial / expected-result conflict | Invalid adaptive_mode is rejected; Test Plan currently expects safe fallback. |
| TC-Emg-001-P | `testEmergencyProbabilityOneAlwaysGeneratesEmergencyVehicles` | Direct | With emergency probability 1.0, 100 generated vehicles are asserted to be emergency vehicles. |
| TC-Emg-001-N | `testZeroEmergencyProbabilityProducesNormalVehicles` | Direct | With arrival probability 1 and emergency probability 0, 100 generated vehicles are asserted to be normal. |
| TC-Emg-002-P | `testEmergencyQueuePriorityAndStableOrdering` | Direct | Checks emergency vehicles are dequeued before normal vehicles. |
| TC-Emg-002-N | `testEmergencyQueuePriorityAndStableOrdering` | Direct | Checks FIFO ordering among emergency vehicles. |
| TC-Emg-003-P | `testEmergencyPreemption` | Direct | Checks a valid emergency lane receives green after the current signal cycle. |
| TC-Emg-003-N | `testEmergencyPreemption; testNoSimultaneousGreenSignals` | Partial | Tests preemption rejection and the general one-green invariant separately, not together under preemption. |
| TC-Multi-001-P | `testConfigDrivenIntersections` | Partial | Checks two intersections are loaded from configuration; does not fully assert independent state/statistics. |
| TC-Multi-001-N | `—` | Not identified | No direct fault-isolation test across intersections was found. |
| TC-Multi-002-P | `testConnectedVehicleTransferTakesOneTick` | Direct | Checks a connected vehicle is delivered at the start of the following tick. |
| TC-Multi-002-N | `testPendingTransferWaitsForDestinationCapacity` | Direct | Checks pending transfer remains queued until destination capacity becomes available. |
| TC-Multi-003-P | `testStatisticsCalculations` | Partial | Checks events across two intersection IDs and aggregate totals, but not the full per-intersection report contract. |
| TC-Multi-003-N | `testZeroThroughputIntersectionAppearsInAggregateReport` | Direct | Checks zero aggregate throughput and confirms a zero-throughput intersection/lane remains in the report breakdown. |
| TC-Stats-001-P | `testStatisticsCalculations` | Direct | Checks average wait-time arithmetic. |
| TC-Stats-001-N | `testStatisticsCalculations` | Partial | Checks empty statistics average is zero; report distinction between zero and no data is not fully asserted. |
| TC-Stats-002-P | `testStatisticsCalculations` | Direct | Checks maximum queue length retains the highest observed value. |
| TC-Stats-002-N | `testMaxQueueLengthOneBoundary` | Direct | Checks the maximum queue remains one after a later zero-length observation. |
| TC-Stats-003-P | `testStatisticsCalculations` | Direct | Checks throughput total from dequeued vehicles. |
| TC-Stats-003-N | `testStatisticsCalculations` | Direct | Checks blocked count and throughput remain separate. |
| TC-Sim-001-P | `testConfigDrivenIntersections; testConfigurationValidationBoundaries` | Partial | Checks configuration is parsed and applied to intersections/capacity; not every setting is asserted. |
| TC-Sim-001-N | `—` | Not identified / expected-result conflict | No missing-config fallback test was found; current loader throws when a config file cannot be opened. |
| TC-Sim-002-P | `testRepeatedSimulationLifecycle; testTenThousandTickStability` | Partial | Checks normal runs complete and summaries are produced; natural-exit status is not fully asserted. |
| TC-Sim-002-N | `testManualStopProducesPartialSummary; testSigintStopsOpenEndedRun` | Direct | Checks user-requested stop and summary tick count. |
| TC-Perf-01-P | `Separate benchmark evidence` | Partial | The repository has benchmark evidence, but the consolidated unit test does not assert the target on evaluator hardware. |
| TC-Perf-01-N | `testTenThousandTickStability` | Partial | Runs 10,000 ticks but does not measure throughput under the Test Plan's maximum-load fixture. |
| TC-Rel-01-P | `testTenThousandTickStability; Linux ASan/UBSan CI` | Partial | Long run and sanitizer evidence exist; no quantified zero-leak report is asserted by this test. |
| TC-Rel-01-N | `testRepeatedSimulationLifecycle` | Partial | Repeats create/run/destroy cycles but does not measure leak/memory growth. |
| TC-Maint-01-P | `testConfigDrivenIntersections` | Direct | Checks topology and connections are created from config. |
| TC-Maint-01-N | `testConfigurationValidationBoundaries` | Partial | Checks self-connection and missing destination rejection; not all malformed topology shapes. |
| TC-UX-01-P | `testConsoleStatusFields` | Direct | Checks intersection, tick, signal and queue labels are present. |
| TC-UX-01-N | `testConsoleLongLaneAndHighQueueDisplay` | Direct | Checks long lane name, high queue value and full visual bar are present. |
| TC-Port-01-P | `GitHub Actions build-test matrix` | Direct | Builds and runs tests on Ubuntu and Windows. |
| TC-Port-01-N | `—` | Not identified | No test that removes a compiler/Make dependency and asserts a clear setup error was found. |
| TC-Rob-001-P | `—` | Not identified | No direct container bounds-check acceptance case was found. |
| TC-Rob-001-N | `testInvalidConfiguration` | Partial | Checks invalid lane index rejection; this is not the same as deliberately invalid container access. |
| TC-Rob-002-P | `testConfigurationValidationBoundaries; config fixture CI` | Direct | Checks invalid values are rejected by parser; CI also runs invalid_values.cfg and checks non-zero termination. |
| TC-Rob-002-N | `testConfigurationValidationBoundaries; testMalformedConfiguration` | Direct | Checks malformed, invalid-type/trailing-character and duplicate/unknown config inputs are rejected. |
| TC-Rob-003-P | `testTenThousandTickStability; Linux ASan/UBSan CI` | Partial | No dedicated zero-leak measurement or Valgrind evidence is present. |
| TC-Rob-003-N | `—` | Not identified | No injected mid-run exception/resource-release test was found. |
| TC-Rob-004-P | `testQueueCapacityAndEmptyDequeue; testPendingTransferWaitsForDestinationCapacity` | Direct | Checks queue overflow rejection and downstream capacity backpressure. |
| TC-Rob-004-N | `testSimultaneousOverflowAcrossEveryLane` | Direct | Fills four independent lanes to capacity, attempts overflow on all lanes and checks every queue remains within capacity. |
| TC-Rob-005-P | `testMalformedConfiguration; config fixture CI` | Direct | Checks malformed config is rejected; CI verifies corrupted fixture exits unsuccessfully. |
| TC-Rob-005-N | `—` | Not identified | No direct test injecting a fault into one module and asserting the rest of the simulation continues was found. |

## Important specification and test gaps

1. **TC-Gen-003-N mapping:** The Test Plan case tests queue capacity, but its requirement field says TMS-F-003 (FIFO). Confirm and correct to the appropriate queue-capacity/robustness requirement before sign-off.
2. **Configuration behavior conflict:** Test Plan cases expect safe fallback for invalid signal mode and missing config in some places, while the current parser rejects invalid values and throws if the config file cannot be opened. Reconcile SRS, Test Plan and implementation rather than silently changing expected results.
3. **TC-Multi-001-N and TC-Multi-003-N:** Add explicit fault-isolation and zero-throughput aggregate-statistics tests if these requirements remain in scope.
4. **TC-Multi-001-N and TC-Multi-003-N:** Add explicit fault-isolation and zero-throughput aggregate-statistics tests if these requirements remain in scope.
5. **TC-Rob-003-P/N:** Add quantified memory/leak evidence; repeated lifecycle and ASan/UBSan runs alone are not a documented Valgrind leak report or injected-exception cleanup test.
6. **TC-Rob-004-N:** Add a simultaneous-overflow test across every lane.
7. **TC-Rob-005-N:** Add a controlled fault-isolation test demonstrating that a failure in one module does not terminate unrelated intersections/modules, if that is the intended requirement.
8. **TC-Port-01-N:** Add a documented/manual check for missing compiler or Make dependency messaging if this remains a required case.

## Recommended next execution order

1. Resolve specification conflicts and the TC-Gen-003-N mapping.
2. Execute the existing direct/partial cases using the Test Plan's own steps and record each outcome in `docs/TEST_EXECUTION_REGISTER.md`.
3. Add targeted automated tests for the cases classified as having no direct automated check, prioritizing lane isolation, invalid-intersection isolation, zero-throughput aggregation, simultaneous overflow and module fault isolation.
4. Keep memory/leak, usability, performance and portability evidence distinct from ordinary unit-test assertions.
