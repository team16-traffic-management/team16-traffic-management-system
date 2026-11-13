#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-g++}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

COVERAGE_DIR="build/coverage"
rm -rf "$COVERAGE_DIR"
mkdir -p "$COVERAGE_DIR/app" "$COVERAGE_DIR/tests"

CXXFLAGS=(-std=c++17 -Wall -Wextra -Wpedantic -pthread -O0 -g --coverage)

APP_SOURCES=(src/main.cpp vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp src/ConsoleView.cpp src/SimulationEngine.cpp)
TEST_SOURCES=(tests/test_traffic_management.cpp vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp src/ConsoleView.cpp src/SimulationEngine.cpp)

compile_objects() {
  local out_dir="$1"
  shift
  local -a objects=()
  for source in "$@"; do
    local object="$out_dir/${source//\//_}.o"
    "$CXX" "${CXXFLAGS[@]}" -c "$source" -o "$object"
    objects+=("$object")
  done
  printf '%s\n' "${objects[@]}"
}

mapfile -t app_objects < <(compile_objects "$COVERAGE_DIR/app" "${APP_SOURCES[@]}")
mapfile -t test_objects < <(compile_objects "$COVERAGE_DIR/tests" "${TEST_SOURCES[@]}")

"$CXX" --coverage "${app_objects[@]}" -o "$COVERAGE_DIR/traffic-management"
"$CXX" --coverage "${test_objects[@]}" -o "$COVERAGE_DIR/test-traffic-management"

"$COVERAGE_DIR/test-traffic-management"
"$COVERAGE_DIR/traffic-management" tests/data/baseline.cfg
"$COVERAGE_DIR/traffic-management" tests/data/zero_values.cfg
"$COVERAGE_DIR/traffic-management" tests/data/high_load.cfg

if "$COVERAGE_DIR/traffic-management" tests/data/invalid_values.cfg; then
  echo "Expected invalid_values.cfg to be rejected" >&2
  exit 1
fi
if "$COVERAGE_DIR/traffic-management" tests/data/corrupted.cfg; then
  echo "Expected corrupted.cfg to be rejected" >&2
  exit 1
fi

# Product-code scope: all .cpp files under src/ and vehicle/, including main.cpp.
# The test harness source is deliberately excluded from product-code coverage.
gcovr --root . --object-directory "$COVERAGE_DIR" --filter '^(src|vehicle)/' --txt-summary
gcovr --root . --object-directory "$COVERAGE_DIR" --filter '^(src|vehicle)/' --html-details "$COVERAGE_DIR/index.html"
gcovr --root . --object-directory "$COVERAGE_DIR" --filter '^(src|vehicle)/' --xml-pretty --output "$COVERAGE_DIR/coverage.xml"
