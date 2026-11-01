@echo off
g++ -std=c++17 -O2 -Wall -Wextra -I include src\TMS.cpp src\main.cpp -o tms.exe
if errorlevel 1 exit /b 1
g++ -std=c++17 -O2 -Wall -Wextra -I include src\TMS.cpp tests\test_tms.cpp -o tms_tests.exe
if errorlevel 1 exit /b 1
echo Build complete. Run tms.exe simulation.conf and tms_tests.exe
