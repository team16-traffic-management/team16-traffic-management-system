# SAD Implementation Addendum — Final Integrated Architecture

**Purpose:** Implementation-aligned supplement to `Documents/SAD_Traffic_Management_System_v3.docx`. This file records the architecture found in the current C++ source and supplies UML diagram source files. It should be reviewed by the team and incorporated into the approved SAD DOCX before final submission.

## 1. Architecture pattern and runtime structure

The implementation is a synchronous, tick-driven, modular C++ application. `main.cpp` creates a `SimulationEngine`, configures or loads intersections, and invokes `run()`. The engine advances the simulation one tick at a time and coordinates the domain modules.

| Component | Actual responsibility | Main implementation |
|---|---|---|
| SimulationEngine | Owns the run loop, intersections, configuration, lane arrival setup, inter-intersection links, pending transfers and tick orchestration | `include/SimulationEngine.h`, `src/SimulationEngine.cpp` |
| Intersection | Owns an intersection ID, its lane queues and its signal controller | `include/Intersection.h`, `src/Intersection.cpp` |
| LaneQueue | Bounded FIFO storage for Vehicle objects; exposes emergency detection and queue state | `include/LaneQueue.h`, `src/LaneQueue.cpp` |
| SignalController | Maintains the active lane and signal phase; supports emergency preemption requests and adaptive timing | `include/TrafficSignalController.h`, `src/TrafficSignalController.cpp` |
| VehicleGenerator | Uses configured probabilities and per-lane random state to decide arrivals and create uniquely identified vehicles | `vehicle/VehicleGenerator.h`, `vehicle/VehicleGenerator.cpp` |
| Vehicle | Carries vehicle identity, arrival tick, lane information and NORMAL/EMERGENCY type | `vehicle/Vehicle.h`, `vehicle/Vehicle.cpp` |
| StatisticsCollector | Records dequeue, blocked-vehicle and preemption events, queue maxima and run summaries | `include/StatisticsCollector.h`, `src/StatisticsCollector.cpp` |
| ConsoleView | Renders a tick/intersection view; can be silenced for tests and benchmarks | `include/ConsoleView.h`, `src/ConsoleView.cpp` |

## 2. Runtime sequence

### 2.1 Vehicle generation and queue admission

The engine increments the tick, attempts delivery of pending transfers, then visits each intersection and lane. It asks the VehicleGenerator whether an arrival occurs. If so, it generates a Vehicle and calls LaneQueue.enqueue(). A rejected enqueue is recorded as a blocked event. Queue maxima are updated. The detailed UML sequence is in `Documents/diagrams/TMS_Vehicle_Admission_Sequence.puml`.

### 2.2 Signal update, emergency handling and dispatch

For each intersection, the engine checks lane queues for emergency vehicles and requests preemption from the SignalController. It advances the intersection signal state, updates adaptive timing if enabled, and dequeues only from a lane whose current phase is GREEN. If that lane has a configured outgoing connection, the Vehicle is placed in the pending-transfer queue; otherwise its dequeue is recorded in statistics. The detailed UML sequence is in `Documents/diagrams/TMS_Signal_And_Dispatch_Sequence.puml`.

### 2.3 Configuration and operator use cases

The executable accepts an optional configuration-file path. The configuration loader validates keys, numeric ranges, intersection lane definitions and connections; malformed, duplicate, unknown or missing required values raise an exception and are reported by main. Ctrl+C requests a stop that is checked between completed ticks; the run then prints the end-of-run summary. The use-case diagram source is in `Documents/diagrams/TMS_Use_Case_Diagram.puml`.

## 3. API and interface design

The following is a source-aligned summary of public interfaces. It is not a complete generated API reference; the headers remain authoritative.

| Class | Key public operations | Contract / observable behavior |
|---|---|---|
| SimulationEngine | `loadConfig(...)`, `addIntersection(...)`, `configureLaneArrivalRate(...)`, `connectIntersections(...)`, `loadConfigFromFile(...)`, `run(...)`, `tick(...)` | Invalid intersection IDs, duplicate IDs, invalid rates, invalid connection endpoints/lane matches and malformed config are rejected with exceptions. `loadConfigFromFile` returns configured tick count; zero means run until stopped. |
| Intersection | `addLane(...)`, `update(...)`, `getLanes()`, `getSignalController()` | Rejects empty/duplicate lane names and zero capacity. Provides mutable/const access to owned lane and signal state. |
| LaneQueue | `enqueue(...)`, `dequeue()`, `isFull()`, `isEmpty()`, `hasEmergency()`, `getQueueLength()` | Enqueue returns false when capacity prevents admission. Dequeue returns a Vehicle and throws `std::runtime_error` if called on an empty queue. |
| SignalController | `addLane(...)`, `updateSignalState(...)`, `getCurrentPhase(...)`, `preempt(...)`, `setAdaptiveMode(...)`, `updateAdaptiveTimers(...)` | Rejects invalid phase durations and negative extension. `preempt` returns false for unknown lanes, a pending request or the already active lane. |
| VehicleGenerator | `configureLane(...)`, `shouldGenerate(...)`, `generateVehicle(...)`, `setEmergencyProbability(...)` | Probability inputs are validated; generation uses lane-specific random state and allocates vehicle IDs. |
| StatisticsCollector | `recordDequeue(...)`, `recordBlocked(...)`, `recordPreemption(...)`, `updateMaxQueueLength(...)`, `averageWaitTime()`, `totalThroughput()`, `totalBlocked()`, `totalPreemptions()`, `printEndOfRunReport(...)` | Stores event and aggregate data, including intersection/lane identifiers. |
| ConsoleView | `renderTick(...)` | Displays per-tick/per-intersection state unless silent rendering is requested. |

## 4. Error handling and validation

- **Configuration input:** `loadConfigFromFile` rejects unreadable files, malformed lines, empty/duplicate keys, missing required keys, unknown keys, invalid types, non-finite/out-of-range probabilities, invalid tick limits, invalid queue capacity and invalid intersection/connection definitions. It throws descriptive exceptions; `main` catches standard exceptions, prints `Simulation failed: ...` and exits with status 1.
- **Object construction:** `Intersection`, `SignalController` and related constructors/mutators reject invalid identifiers, lane names, duplicate lanes, zero capacities, invalid signal durations and negative adaptive extensions with `std::invalid_argument`.
- **Queue capacity:** `LaneQueue.enqueue` reports admission failure through its boolean return; the engine records a blocked event rather than treating a full queue as a successful arrival.
- **Inter-intersection transfer:** pending transfers are retried on later ticks if the destination lane cannot accept the vehicle. A transfer is counted only after successful enqueue at the destination.
- **Simulation lifecycle:** `run` rejects negative tick counts and durations that would overflow the tick counter. SIGINT requests a stop between ticks, after which the run summary is printed.
- **Statistics and rendering:** the engine records dequeue, blocked and preemption events and calls the view once per intersection per tick. Silent mode suppresses interactive rendering for tests/benchmarks.

## 5. Security and trust boundary

The inspected implementation is a local, single-process C++ simulation. The repository does not expose a network listener, user-account system, remote authentication service or external database. Therefore the relevant security posture is input and process robustness rather than a fabricated network threat model.

- Treat configuration files as untrusted input; validate keys, syntax, numeric ranges, IDs, lane definitions and connection endpoints before use.
- Use bounded queue capacity and validate indices/identifiers at module boundaries.
- Use standard C++ containers and value ownership for vehicles, intersections, queues and pending transfers; retain RAII-based resource management.
- Report malformed input through controlled exceptions and a non-zero process exit instead of silently accepting malformed settings.
- Keep the process-level stop handler limited to setting the atomic stop request; the simulation loop performs shutdown and summary work outside the signal handler.

This section is a design description, not a claim of a formal penetration test or a substitute for instructor approval of the assignment's security requirement.

## 6. Requirement-to-design traceability

| Requirement IDs | Primary design elements | Implementation locations |
|---|---|---|
| TMS-F-001, TMS-F-002 | Arrival decision, Vehicle construction and ID allocation | VehicleGenerator, Vehicle |
| TMS-F-003, TMS-F-010, TMS-F-011 | FIFO and bounded lane admission; lane isolation | LaneQueue, Intersection |
| TMS-F-012, TMS-F-020–TMS-F-024 | Green-only dispatch, phase cycle, adaptive timing and phase invariants | SignalController, Intersection, SimulationEngine |
| TMS-F-013, TMS-F-050–TMS-F-052 | Wait-time, throughput, blocked/preemption events and queue maxima | StatisticsCollector, SimulationEngine |
| TMS-F-030–TMS-F-032 | Emergency vehicle type, queue detection, preemption and priority dispatch | VehicleGenerator, Vehicle, LaneQueue, SignalController, SimulationEngine |
| TMS-F-040–TMS-F-042 | Independent intersections, configured connections, delayed transfers and aggregate reporting | SimulationEngine, Intersection, StatisticsCollector |
| TMS-F-060–TMS-F-061 | Configuration parsing, tick control, graceful stop and end-of-run report | SimulationEngine, main, StatisticsCollector |
| TMS-NF-001 | Tick processing and optional silent rendering for throughput | SimulationEngine, ConsoleView |
| TMS-NF-002 | Value ownership, standard containers and controlled lifecycle | SimulationEngine, Intersection, LaneQueue, Vehicle |
| TMS-NF-003 | Modular classes and configurable intersection/lane topology | SimulationEngine, Intersection and domain modules |
| TMS-NF-004 | Per-tick console presentation | ConsoleView |
| TMS-NF-005 | Portable C++ build and cross-platform CI | Makefile, `.github/workflows/ci-cd.yml` |
| PRJ-SR-001 | Bounds/validity checks at queue, lane and configuration boundaries | LaneQueue, Intersection, SimulationEngine |
| PRJ-SR-002 | Strict configuration syntax/type/range validation | SimulationEngine::loadConfigFromFile |
| PRJ-SR-003 | RAII and standard-library ownership containers | SimulationEngine, LaneQueue, VehicleGenerator |
| PRJ-SR-004 | Capacity-limited queue and blocked-event recording | LaneQueue, SimulationEngine, StatisticsCollector |
| PRJ-SR-005 | Exceptions and controlled top-level error reporting | domain constructors, SimulationEngine, main |

**Traceability limitation:** These mappings are architectural responsibility mappings. They do not assert that each requirement has passed its acceptance test. Link each requirement to its exact SRS text, Test Plan case IDs and executed evidence in the RTM/Test Execution Register.

## 7. Items requiring team review before DOCX sign-off

1. Verify each diagram against the final DOCX layout and embed the rendered UML diagrams in the SAD/SRS DOCX files.
2. Confirm the use-case actor and system boundary with the team.
3. Review the security/trust-boundary text with the instructor; the project scope has no remote/network interface.
4. Confirm the requirement groupings against the final approved SRS and complete any one-to-one RTM mapping needed.
5. Reconcile all behavior described here with the final source revision before signing the SAD.
