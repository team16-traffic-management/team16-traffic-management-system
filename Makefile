CXX      = g++
CXXFLAGS = -std=c++14 -Wall -Wextra
INCLUDES = -I. -Icommon -Ivehicle -Isimulation -Istatistics -Iconsole
SOURCES  = main.cpp $(wildcard vehicle/*.cpp) $(wildcard simulation/*.cpp) \
           $(wildcard statistics/*.cpp) $(wildcard console/*.cpp)

all:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o build/traffic-management

clean:
	rm -rf build