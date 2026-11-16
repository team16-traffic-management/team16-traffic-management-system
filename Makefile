CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -pthread
CORE_SOURCES = vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp src/ConsoleView.cpp src/SimulationEngine.cpp
APP_SOURCES = src/main.cpp $(CORE_SOURCES)
TEST_SOURCES = tests/test_traffic_management.cpp $(CORE_SOURCES)
ACCEPTANCE_SOURCES = tests/acceptance/test_plan_acceptance.cpp $(CORE_SOURCES)
HEADERS = vehicle/Vehicle.h vehicle/VehicleGenerator.h include/LaneQueue.h include/TrafficSignalController.h include/StatisticsCollector.h include/Intersection.h include/ConsoleView.h include/SimulationEngine.h

.PHONY: all test acceptance acceptance-full coverage sanitize clean check-deps
check-deps:
	@if ! command -v "$(CXX)" >/dev/null 2>&1; then \
		echo "Error: C++ compiler '$(CXX)' was not found. Install a C++17 compiler or set CXX to its executable." >&2; \
		exit 127; \
	fi

all: build/traffic-management
build/traffic-management: | check-deps
build/traffic-management: $(APP_SOURCES) $(HEADERS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $@
build/test-traffic-management: | check-deps
build/test-traffic-management: $(TEST_SOURCES) $(HEADERS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $@
build/test-plan-acceptance: | check-deps
build/test-plan-acceptance: $(ACCEPTANCE_SOURCES) $(HEADERS)
	mkdir -p build
	$(CXX) $(CXXFLAGS) -O2 $(ACCEPTANCE_SOURCES) -o $@
test: build/test-traffic-management
	./build/test-traffic-management
# The 62 in-process Test Plan v1.2 cases (one result line per TC ID).
acceptance: build/test-plan-acceptance
	./build/test-plan-acceptance
# All Test Plan cases including process-level, Ctrl+C and memory-tool cases; writes docs/QA_Reports/evidence.
acceptance-full: check-deps
	bash scripts/run_acceptance.sh
coverage: check-deps
	bash scripts/run_coverage.sh
sanitize: check-deps
	mkdir -p build
	$(CXX) $(CXXFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined $(TEST_SOURCES) -o build/test-traffic-management-sanitized
	./build/test-traffic-management-sanitized
	$(CXX) $(CXXFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined $(ACCEPTANCE_SOURCES) -o build/test-plan-acceptance-sanitized
	./build/test-plan-acceptance-sanitized --no-perf
clean:
	rm -rf build
