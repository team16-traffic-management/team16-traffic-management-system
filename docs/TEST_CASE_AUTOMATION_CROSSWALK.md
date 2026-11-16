# Test Plan to Automated Test Crosswalk

**Baseline:** Sprint 3 code, Test Plan v1.2 (16-11-2026).
**Purpose:** Show which executable check runs each of the 66 Test Plan cases, and which unit tests give supporting evidence.

## Summary

| Classification | Cases |
|---|---:|
| Executed by a dedicated acceptance function (`tests/acceptance/test_plan_acceptance.cpp`) | 62 |
| Executed only at process / tool level (`scripts/run_acceptance.sh`) | 4 |
| No executable check | 0 |
| **Total** | **66** |

Before Part 3 this crosswalk listed 44 direct, 18 partial and 4 unmapped cases. Every case now has a dedicated check that implements its own test data and expected result. The 18 partial and 4 unmapped cases were closed by the acceptance runner, by the process-level steps (real Ctrl+C, CLI exit codes, missing build tool, Valgrind/ASan), and by the Sprint 3 fixes. Results are in `docs/TEST_EXECUTION_REGISTER.md`.

## Case-by-case crosswalk

| Test Plan case | Acceptance function | Process-level step (`run_acceptance.sh`) | Related unit tests (`tests/test_traffic_management.cpp`) |
|---|---|---|---|
| TC-Gen-001-P | `genSpawnRate` | — | testArrivalRates |
| TC-Gen-001-N | `genZeroRate` | — | testProbabilityBoundaries |
| TC-Gen-002-P | `genIdTickType` | — | testVehicleIdsAreUniqueAcrossLanesAndIntersections; testProbabilityBoundaries |
| TC-Gen-002-N | `genSameTickUnique` | — | testVehicleIdsAreUniqueAcrossLanesAndIntersections |
| TC-Gen-003-P | `genFifo` | — | testQueueCapacityAndEmptyDequeue; testEmergencyQueuePriorityAndStableOrdering |
| TC-Gen-003-N | `genFifoWithEmergency` | — | testEmergencyQueuePriorityAndStableOrdering |
| TC-Queue-001-P | `queueIndependent` | — | testSiblingLaneQueueIsolation |
| TC-Queue-001-N | `queueSiblingIntact` | — | testSiblingLaneQueueIsolation |
| TC-Queue-002-P | `queueCapacityEnforced` | — | testQueueCapacityAndEmptyDequeue |
| TC-Queue-002-N | `queueZeroCapacity` | — | testIntersectionValidationAndSignalRegistration; testConfigurationValidationBoundaries |
| TC-Queue-003-P | `queueDequeueOnlyGreen` | — | testEngineDequeuesVehiclesOnlyOnGreen |
| TC-Queue-003-N | `queueEmptyGreen` | — | testQueueCapacityAndEmptyDequeue |
| TC-Queue-004-P | `queueWaitTime` | — | testStatisticsCalculations |
| TC-Queue-004-N | `queueImmediateWait` | — | testStatisticsCalculations |
| TC-Sig-001-P | `sigFixedCycle` | — | testSignalPhaseTransitions; testSignalTimingsLoadedFromConfiguration |
| TC-Sig-001-N | `sigZeroDuration` | — | testSignalPhaseTransitions; testSignalTimingsLoadedFromConfiguration |
| TC-Sig-002-P | `sigAdaptiveExtends` | — | testAdaptiveGreenExtensionBounds (rewritten Sprint 3) |
| TC-Sig-002-N | `sigAdaptiveCapped` | — | testAdaptiveGreenExtensionBounds (rewritten Sprint 3) |
| TC-Sig-003-P | `sigNoConflictingGreen` | — | testNoSimultaneousGreenSignals |
| TC-Sig-003-N | `sigForcedConflict` | — | testAdaptiveModeCanBeChangedDuringSignalOperation |
| TC-Sig-004-P | `sigYellowBeforeRed` | — | testSignalPhaseTransitions |
| TC-Sig-004-N | `sigOneTickYellow` | — | testSignalPhaseTransitions; testConsoleStatusFields |
| TC-Sig-005-P | `sigModeViaConfig` | — | testAdaptiveModeLoadedFromConfiguration |
| TC-Sig-005-N | `sigInvalidMode` | — | testConfigurationValidationBoundaries |
| TC-Emg-001-P | `emgSpawnProbability` | — | testEmergencyProbabilityOneAlwaysGeneratesEmergencyVehicles |
| TC-Emg-001-N | `emgZeroProbability` | — | testZeroEmergencyProbabilityProducesNormalVehicles |
| TC-Emg-002-P | `emgPriority` | — | testEmergencyQueuePriorityAndStableOrdering |
| TC-Emg-002-N | `emgFifoAmongEmergency` | — | testEmergencyQueuePriorityAndStableOrdering |
| TC-Emg-003-P | `emgPreemptWithinDelay` | — | testBoundedEmergencyPreemptionDelay; testEnginePreemptsWithinConfiguredDelay; testPreemptionDelayConfigurationBounds |
| TC-Emg-003-N | `emgPreemptNoConflict` | — | testBoundedEmergencyPreemptionDelay; testNoSimultaneousGreenSignals |
| TC-Multi-001-P | `multiIndependent` | — | testConfigDrivenIntersections |
| TC-Multi-001-N | `multiFaultIsolation` | — | — |
| TC-Multi-002-P | `multiGreenWave` | — | testConnectedVehicleTransferTakesOneTick |
| TC-Multi-002-N | `multiFullDownstream` | — | testPendingTransferWaitsForDestinationCapacity; testPendingTransfersBoundedByDownstreamCapacity |
| TC-Multi-003-P | `multiStatsSum` | — | testPerIntersectionTotalsSumToAggregate; testStatisticsCalculations |
| TC-Multi-003-N | `multiZeroThroughput` | — | testZeroThroughputIntersectionAppearsInAggregateReport |
| TC-Stats-001-P | `statsAverage` | — | testStatisticsCalculations |
| TC-Stats-001-N | `statsAverageNoData` | — | testStatisticsCalculations |
| TC-Stats-002-P | `statsMaxQueue` | — | testStatisticsCalculations |
| TC-Stats-002-N | `statsMaxQueueOne` | — | testMaxQueueLengthOneBoundary |
| TC-Stats-003-P | `statsThroughputCount` | — | testStatisticsCalculations |
| TC-Stats-003-N | `statsBlockedNotThroughput` | — | testStatisticsCalculations |
| TC-Sim-001-P | `simConfigApplied` | — | testSignalTimingsLoadedFromConfiguration; testConfigDrivenIntersections |
| TC-Sim-001-N | `simMissingConfig` | CLI exit code + message | — |
| TC-Sim-002-P | `simNaturalCompletion` | CLI 500-tick run | testRepeatedSimulationLifecycle; testTenThousandTickStability |
| TC-Sim-002-N | — | real Ctrl+C via send_ctrl_c | testManualStopProducesPartialSummary; testSigintStopsOpenEndedRun |
| TC-Perf-01-P | `perfTarget` | — | Separate benchmark evidence |
| TC-Perf-01-N | `perfMaxLoad` | — | testPendingTransfersBoundedByDownstreamCapacity |
| TC-Rel-01-P | `relLongRun` | Valgrind stage (Linux CI) | testTenThousandTickStability; Linux ASan/UBSan CI |
| TC-Rel-01-N | `relRepeatedRuns` | Valgrind / LSan stage (Linux CI) | testRepeatedSimulationLifecycle |
| TC-Maint-01-P | `maintNewTopology` | unchanged binary + three_intersections.cfg | testConfigDrivenIntersections |
| TC-Maint-01-N | `maintInvalidTopology` | — | testConfigurationValidationBoundaries |
| TC-UX-01-P | `uxReadable` | — | testConsoleStatusFields |
| TC-UX-01-N | `uxLongName` | — | testConsoleLongLaneAndHighQueueDisplay; testConsoleColumnsStayAlignedWithLongLaneNames |
| TC-Port-01-P | — | per-OS build + CI `portability-compare` diff | GitHub Actions build-test matrix |
| TC-Port-01-N | — | `make` absent from PATH; `make check-deps` (CI) | — |
| TC-Rob-001-P | — | ASan/UBSan stage (Linux CI) | — |
| TC-Rob-001-N | `robInvalidIndex` | — | testInvalidConfiguration |
| TC-Rob-002-P | `robInvalidValues` | CLI exit code on invalid_values.cfg | testConfigurationValidationBoundaries; config fixture CI |
| TC-Rob-002-N | `robWrongType` | — | testConfigurationValidationBoundaries; testMalformedConfiguration |
| TC-Rob-003-P | `robSustainedRun` | Valgrind stage (Linux CI) | testTenThousandTickStability; Linux ASan/UBSan CI |
| TC-Rob-003-N | `robMidRunException` | Valgrind / LSan stage (Linux CI) | — |
| TC-Rob-004-P | `robOverflow` | — | testQueueCapacityAndEmptyDequeue; testPendingTransferWaitsForDestinationCapacity |
| TC-Rob-004-N | `robOverflowEveryLane` | — | testSimultaneousOverflowAcrossEveryLane |
| TC-Rob-005-P | `robMalformedInput` | CLI exit code on corrupted.cfg | testMalformedConfiguration; config fixture CI |
| TC-Rob-005-N | `robModuleFault` | — | — |
