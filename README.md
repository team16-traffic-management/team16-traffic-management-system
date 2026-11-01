# Traffic Management System

[![CI](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/workflows/ci-cd.yml/badge.svg?branch=main)](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/workflows/ci-cd.yml)

A C++14 traffic-intersection simulation for the Team 16 project. The current implementation demonstrates randomized vehicle arrivals across four lanes, a rotating green signal, lane queues, and a per-lane traffic summary.

## Project structure

- `src/main.cpp` — simulation entry point and traffic-flow loop.
- `vehicle/VehicleGenerator.h/.cpp` — seeded, per-lane randomized vehicle generation.
- `tests/test_traffic_management.cpp` — consolidated tests for vehicle generation and configuration validation.
- `requirements.txt` — notes on build requirements; this C++ project has no Python packages to install.
- `Makefile` — build, test, sanitizer, and clean commands.

## Requirements

Install a C++14-compatible compiler and GNU Make (or compatible make).

On macOS, install Xcode Command Line Tools if the compiler or make command is unavailable:

```bash
xcode-select --install
```

## Build and run

```bash
make
./build/traffic-management
```

## Run tests

```bash
make test
make sanitize
```

The simulation is an initial working foundation, not yet the complete SRS implementation. Emergency-vehicle priority, adaptive signal timing, multi-intersection coordination, and full reporting remain future integration work.
