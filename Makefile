CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Wpedantic
APP_SOURCES = src/main.cpp vehicle/VehicleGenerator.cpp
TEST_SOURCES = tests/test_traffic_management.cpp vehicle/VehicleGenerator.cpp

.PHONY: all test sanitize clean

all: build/traffic-management

build/traffic-management: $(APP_SOURCES) vehicle/VehicleGenerator.h
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o $@

build/test-traffic-management: $(TEST_SOURCES) vehicle/VehicleGenerator.h
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
