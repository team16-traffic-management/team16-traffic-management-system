CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -pthread
APP_SOURCES = src/main.cpp vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp src/ConsoleView.cpp src/SimulationEngine.cpp
TEST_SOURCES = tests/test_traffic_management.cpp vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp src/ConsoleView.cpp src/SimulationEngine.cpp

.PHONY: all test sanitize coverage clean
all: build/traffic-management
build/traffic-management: $(APP_SOURCES) vehicle/Vehicle.h vehicle/VehicleGenerator.h include/LaneQueue.h include/TrafficSignalController.h include/StatisticsCollector.h include/Intersection.h include/ConsoleView.h include/SimulationEngine.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $@
build/test-traffic-management: $(TEST_SOURCES) vehicle/Vehicle.h vehicle/VehicleGenerator.h include/LaneQueue.h include/TrafficSignalController.h include/StatisticsCollector.h include/Intersection.h include/ConsoleView.h include/SimulationEngine.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $@
test: build/test-traffic-management
	./build/test-traffic-management
sanitize:
	mkdir -p build
	$(CXX) $(CXXFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined $(TEST_SOURCES) -o build/test-traffic-management-sanitized
	./build/test-traffic-management-sanitized
clean:
	rm -rf build
