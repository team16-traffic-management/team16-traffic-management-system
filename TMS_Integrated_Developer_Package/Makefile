CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -O3
SRC = $(wildcard src/*.cpp) main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = tms_10k_visible

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
