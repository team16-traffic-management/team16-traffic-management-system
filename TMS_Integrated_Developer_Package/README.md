# Traffic Management System (TMS)

A modular, console-based Traffic Management System implemented in **C++17**. The system simulates vehicle arrivals, lane queues, traffic signals, emergency vehicle priority, connected intersections, and traffic statistics.

## 1. Project Overview

The Traffic Management System simulates traffic flow through multiple intersections. It manages vehicles in independent lanes, controls signal phases, prioritizes emergency vehicles, and records traffic statistics during simulation.

The current demonstration uses:
- 2 connected intersections (`INT_01` and `INT_02`)
- 4 lanes per intersection
- Configurable vehicle arrival probabilities
- Bounded lane queues
- Adaptive signal mode
- 10,000 simulation ticks

## 2. Features

- **Vehicle Generation:** Generates vehicles with unique IDs, vehicle types, and arrival timestamps.
- **Lane Management:** Maintains independent FIFO lane queues with configurable capacity.
- **Emergency Priority:** Gives emergency vehicles priority in queues and supports signal preemption.
- **Traffic Signals:** Simulates GREEN, YELLOW, and RED signal phases.
- **Adaptive Timing:** Supports configurable adaptive signal mode.
- **Multiple Intersections:** Supports multiple independent intersections.
- **Intersection Transfers:** Transfers vehicles from connected source lanes to matching destination lanes.
- **Traffic Statistics:** Records throughput, average waiting time, maximum queue length, blocked vehicles, and emergency preemption events.
- **Configuration:** Loads simulation parameters from `config.txt`.
- **Input Validation:** Rejects invalid configuration values and malformed numeric input.
- **Console Monitoring:** Displays lane queue lengths and signal states during live simulation.

## 3. Project Structure

```text
TMS_Integrated_Developer_Package/
│
├── include/
│   ├── ConsoleView.h
│   ├── Intersection.h
│   ├── LaneQueue.h
│   ├── SimulationEngine.h
│   ├── StatisticsCollector.h
│   ├── TrafficSignalController.h
│   ├── Vehicle.h
│   └── VehicleGenerator.h
│
├── src/
│   ├── ConsoleView.cpp
│   ├── Intersection.cpp
│   ├── LaneQueue.cpp
│   ├── SimulationEngine.cpp
│   ├── StatisticsCollector.cpp
│   ├── TrafficSignalController.cpp
│   ├── Vehicle.cpp
│   └── VehicleGenerator.cpp
│
├── main.cpp
├── config.txt
├── Makefile
├── README.md
├── IMPLEMENTATION_GAP_REPORT.md
└── VERIFICATION_LOG.md
```

## 4. Technologies

- Language: C++17
- Compiler: GNU G++ (MinGW on Windows)
- Standard library: STL containers and utilities
- Build environment: Windows PowerShell / VS Code
- External dependencies: None required

## 5. Configuration

Simulation parameters are stored in `config.txt`.

Example:

```ini
total_ticks=10000
emergency_probability=0.08
adaptive_mode=true

North_Bound=0.30
South_Bound=0.40
East_Bound=0.50
West_Bound=0.35

queue_capacity=15
```

| Parameter | Description |
|---|---|
| `total_ticks` | Number of simulation ticks |
| `emergency_probability` | Probability of generating an emergency vehicle |
| `adaptive_mode` | Enables or disables adaptive signal mode |
| Lane probabilities | Arrival probabilities for individual lanes |
| `queue_capacity` | Maximum capacity of each lane queue |

## 6. Build and Run

### Windows (MinGW)

Compile the project:

```powershell
g++ -std=c++17 -O2 -Iinclude src/*.cpp main.cpp -o tms.exe
```

Run:

```powershell
.\tms.exe
```

### Linux

Compile:

```bash
g++ -std=c++17 -O2 -Iinclude src/*.cpp main.cpp -o tms
```

Run:

```bash
./tms
```

## 7. Simulation Setup

The demonstration configures two intersections:

- Intersection 1: `INT_01`
- Intersection 2: `INT_02`

Each intersection contains four lanes:
- North_Bound
- South_Bound
- East_Bound
- West_Bound

The corresponding lanes of `INT_01` are connected to `INT_02` for vehicle transfers.

## 8. Verification Results

The project was compiled successfully using the C++17 compiler command above.

A 10,000-tick simulation was executed with two connected intersections.

### Final observed results

| Metric | Result |
|---|---:|
| Simulation ticks | 10,000 |
| Intersections | 2 |
| Total processed throughput | 6,435 |
| Overall average waiting time | 521.47 ticks |
| Blocked vehicles | 24,063 |
| Emergency preemption events | 248 |
| Successful vehicle transfers | 5,347 |
| Vehicles waiting for transfer at simulation end | 409 |
| Maximum queue length | 15 |
| Execution time (silent mode) | 820.88 ms |
| Processing throughput (silent mode) | 12,182.12 ticks/sec |

### Performance Verification

The observed silent-mode processing throughput was approximately 12,182 ticks/sec, exceeding the project's target of 500 ticks/sec.

Live console output is slower because it prints lane and signal information during simulation. The silent-mode benchmark is therefore used for the performance measurement.

### Configuration Validation

The following invalid configuration tests were performed:

| Test | Input | Observed result |
|---|---|---|
| Queue capacity validation | `queue_capacity=0` | Rejected with a validation error |
| Malformed tick count | `total_ticks=abc` | Rejected with an invalid-value error |

Both tests passed. The original configuration was restored after testing.

## 9. Observations and Limitations

The final simulation reported 24,063 blocked vehicles and 409 vehicles still waiting for transfer when the simulation ended.

These results indicate that the configured vehicle arrival rates create significant queue pressure. The queue capacity was reached in all eight lanes during the simulation.

Vehicles still waiting for transfer at the end of the simulation are reported separately from successful transfers.

## 10. Conclusion

The current implementation successfully builds and runs as a C++17 console application. The final verification demonstrates a 10,000-tick simulation with two connected intersections, vehicle generation, lane management, traffic signal operation, emergency handling, transfer processing, traffic statistics, and configuration validation.

The measured silent-mode performance exceeded the target of 500 ticks per second.