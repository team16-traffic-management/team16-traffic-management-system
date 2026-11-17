# Team16 Traffic Management System — Modern Web Frontend

A futuristic smart-city traffic control dashboard and real-time 3D intersection visualizer connected directly to the C++17 simulation engine.

## Features

### 1. Dashboard (`/`)
- **Real-Time KPIs**: Total vehicles generated, total processed (throughput), waiting vehicles, and emergency vehicles.
- **Simulation Telemetry**: Current tick counter, clearance rates, average wait time, and blocked vehicle counts.
- **Traffic Signal Status**: Live phase indicator (`GREEN`, `YELLOW`, `RED`) for all 4 intersection approaches (North, South, East, West).
- **Interactive Flow Dynamics**: Recharts Area Chart showing processed throughput vs queued vehicles over the last 60 ticks.
- **Lane Queue Depth Chart**: Bar breakdown of queued vehicles vs remaining capacity.
- **Detailed Lane Table**: Live status, queue progress bar, emergency preemption active alerts, and lead vehicle IDs.
- **Engine Controls**: Instant Start, Pause, Resume, and Reset buttons directly interfacing with the C++ engine.

### 2. Live 3D Simulation (`/simulation`)
- **Realistic 4-Way Road Intersection**: Asphalt road surfaces, dashed lane dividers, zebra crosswalks, stop lines, curbs, and surrounding cyber terrain.
- **Accurate Traffic Signals**: 4 overhead gantry poles with 3-lens signal heads casting real dynamic point lights onto the pavement, reflecting the exact C++ signal controller phases.
- **Dynamic Vehicle Animation**:
  - **Normal Vehicles**: Metallic cyber-blue commuters queuing at stop lines and accelerating smoothly through the junction on green.
  - **Emergency Vehicles**: Glowing red cruisers with active flashing dual strobe lights and emergency sirens.
- **Interactive 3D Camera**:
  - Full OrbitControls: Rotate, pan, and zoom.
  - Camera Angle Presets: **Isometric Overview**, **Aerial Satellite View**, and **Street Level**.
  - One-click camera reset.
- **Floating HUD**: Real-time signal telemetry overlay and on-canvas simulation controls.

---

## Technical Stack
- **Framework**: React 18 + TypeScript + Vite
- **Styling**: Tailwind CSS (dark cyber aesthetic with glassmorphism panels)
- **3D Engine**: Three.js + React Three Fiber (`@react-three/fiber`) + React Three Drei (`@react-three/drei`)
- **Charts**: Recharts
- **State Management**: Zustand
- **Networking**: Axios (with automatic proxy to local C++ HTTP bridge on port `8080`)

---

## Getting Started

### Prerequisites
- Node.js (v18+) and npm
- C++17 compiler (g++ / MinGW-w64 on Windows or clang/gcc on Linux)

### Running the System

#### 1. Compile and Start the C++ Bridge
From the repository root:
```bash
# On Linux / macOS
make bridge
./build/traffic-bridge

# On Windows
g++ -std=c++17 -Wall -Wextra -D_WIN32_WINNT=0x0A00 src/bridge_main.cpp vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp src/ConsoleView.cpp src/SimulationEngine.cpp -lws2_32 -o build/traffic-bridge.exe
.\build\traffic-bridge.exe
```
The C++ bridge listens on `http://localhost:8080`.

#### 2. Start the Frontend
```bash
cd frontend
npm install
npm run dev
```
Open **`http://localhost:5173`** in your browser.

#### Or Single-Click Launch (Windows)
```powershell
.\run_full_system.ps1
```
