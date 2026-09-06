CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -Iinclude

SRC = src/ConsoleView.cpp src/StatisticsCollector.cpp src/demo_main.cpp

demo.exe: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o demo.exe

run: demo.exe
	demo.exe

clean:
	del /Q demo.exe 2>nul || true
