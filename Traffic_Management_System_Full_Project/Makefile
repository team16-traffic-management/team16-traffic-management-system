CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude
SRC = src/TMS.cpp
all: tms

tms: $(SRC) src/main.cpp include/TMS.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SRC) src/main.cpp -o $@

test: $(SRC) tests/test_tms.cpp include/TMS.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(SRC) tests/test_tms.cpp -o tms_tests
	./tms_tests

clean:
	$(RM) tms tms_tests
.PHONY: all test clean
