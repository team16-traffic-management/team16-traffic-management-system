#!/usr/bin/env sh
set -eu
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I include src/TMS.cpp src/main.cpp -o tms
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I include src/TMS.cpp tests/test_tms.cpp -o tms_tests
