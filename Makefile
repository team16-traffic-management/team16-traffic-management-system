CXX      = g++
CXXFLAGS = -std=c++14 -Wall -Wextra
INCLUDES = -I. -Icommon -Ivehicle -Isimulation -Istatistics -Iconsole
SOURCES  = main.cpp $(wildcard vehicle/*.cpp) $(wildcard simulation/*.cpp) \
           $(wildcard statistics/*.cpp) $(wildcard console/*.cpp)
GENERATOR_TEST = tests/test_vehicle_generator.cpp vehicle/VehicleGenerator.cpp

.PHONY: all test sanitize clean

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o build/traffic-management

test: all
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(GENERATOR_TEST) -o build/test-vehicle-generator
	./build/test-vehicle-generator
	./build/traffic-management

sanitize:
	mkdir -p build
	$(CXX) $(CXXFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined $(INCLUDES) $(GENERATOR_TEST) -o build/test-vehicle-generator-sanitized
	./build/test-vehicle-generator-sanitized
	$(CXX) $(CXXFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined $(INCLUDES) $(SOURCES) -o build/traffic-management-sanitized
	./build/traffic-management-sanitized

clean:
	rm -rf build
