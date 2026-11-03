CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Wpedantic
APP_SOURCES = src/main.cpp vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp
TEST_SOURCES = tests/test_traffic_management.cpp vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp src/StatisticsCollector.cpp src/Intersection.cpp

.PHONY: all test sanitize coverage clean

all: build/traffic-management

build/traffic-management: $(APP_SOURCES) vehicle/Vehicle.h vehicle/VehicleGenerator.h include/LaneQueue.h include/TrafficSignalController.h include/StatisticsCollector.h include/Intersection.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $@

build/test-traffic-management: $(TEST_SOURCES) vehicle/Vehicle.h vehicle/VehicleGenerator.h include/LaneQueue.h include/TrafficSignalController.h include/StatisticsCollector.h include/Intersection.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $@

test: all build/test-traffic-management
	./build/test-traffic-management
	./build/traffic-management

sanitize:
	mkdir -p build
	$(CXX) $(CXXFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined $(TEST_SOURCES) -o build/test-traffic-management-sanitized
	./build/test-traffic-management-sanitized

coverage:
	mkdir -p build/coverage
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c tests/test_traffic_management.cpp -o build/coverage/test_traffic_management.o
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c vehicle/Vehicle.cpp -o build/coverage/Vehicle.o
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c vehicle/VehicleGenerator.cpp -o build/coverage/VehicleGenerator.o
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c src/LaneQueue.cpp -o build/coverage/LaneQueue.o
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c src/TrafficSignalController.cpp -o build/coverage/TrafficSignalController.o
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c src/StatisticsCollector.cpp -o build/coverage/StatisticsCollector.o
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c src/Intersection.cpp -o build/coverage/Intersection.o
	$(CXX) --coverage build/coverage/test_traffic_management.o build/coverage/Vehicle.o build/coverage/VehicleGenerator.o build/coverage/LaneQueue.o build/coverage/TrafficSignalController.o build/coverage/StatisticsCollector.o build/coverage/Intersection.o -o build/coverage/test-traffic-management
	cd build/coverage && ./test-traffic-management
	cd build/coverage && gcov -b -c test_traffic_management.gcno Vehicle.gcno VehicleGenerator.gcno LaneQueue.gcno TrafficSignalController.gcno StatisticsCollector.gcno Intersection.gcno

clean:
	rm -rf build
