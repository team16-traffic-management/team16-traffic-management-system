CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Wpedantic
APP_SOURCES = src/main.cpp vehicle/VehicleGenerator.cpp
TEST_SOURCES = tests/test_traffic_management.cpp vehicle/VehicleGenerator.cpp

.PHONY: all test sanitize coverage clean

all: build/traffic-management

build/traffic-management: $(APP_SOURCES) vehicle/VehicleGenerator.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $@

build/test-traffic-management: $(TEST_SOURCES) vehicle/VehicleGenerator.h
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
	$(CXX) $(CXXFLAGS) --coverage -O0 -g -c vehicle/VehicleGenerator.cpp -o build/coverage/VehicleGenerator.o
	$(CXX) --coverage build/coverage/test_traffic_management.o build/coverage/VehicleGenerator.o -o build/coverage/test-traffic-management
	cd build/coverage && ./test-traffic-management
	cd build/coverage && gcov -b -c test_traffic_management.gcno VehicleGenerator.gcno

clean:
	rm -rf build
