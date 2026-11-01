CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Iinclude

TARGET = build/traffic-management
SOURCE = src/main.cpp

.PHONY: all test sanitize clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

test: $(TARGET)
	./$(TARGET)

sanitize:
	mkdir -p build
	$(CXX) $(CXXFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined $(SOURCE) -o build/traffic-management-sanitized
	ASAN_OPTIONS=detect_leaks=1 ./build/traffic-management-sanitized

clean:
	rm -rf build
