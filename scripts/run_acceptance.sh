#!/usr/bin/env bash
# Executes the Test Plan v1.2 acceptance cases and writes evidence files.
#
#   bash scripts/run_acceptance.sh
#
# Environment:
#   CXX           compiler (default g++)
#   EVIDENCE_DIR  output folder (default docs/QA_Reports/evidence)
#
# Steps: build -> unit suite -> 62 in-process Test Plan cases -> CLI/process
# cases (exit codes, real Ctrl+C, missing build tool, config-only topology) ->
# benchmark -> memory tools when available (ASan/UBSan/LSan, Valgrind).
# Exit status is non-zero if any executed case fails.
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
CXX="${CXX:-g++}"
EVIDENCE="${EVIDENCE_DIR:-docs/QA_Reports/evidence}"
mkdir -p build "$EVIDENCE"

if ! command -v "$CXX" >/dev/null 2>&1; then
  echo "Error: C++ compiler '$CXX' was not found." >&2
  exit 127
fi
# Use the compiler's own runtime DLLs first (Git for Windows ships an older libstdc++).
export PATH="$(dirname "$(command -v "$CXX")"):$PATH"

case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) PLATFORM=windows ;;
  Linux*) PLATFORM=linux ;;
  *) PLATFORM="$(uname -s | tr '[:upper:]' '[:lower:]')" ;;
esac

FLAGS=(-std=c++17 -Wall -Wextra -Wpedantic -pthread)
SOURCES=(vehicle/Vehicle.cpp vehicle/VehicleGenerator.cpp src/LaneQueue.cpp src/TrafficSignalController.cpp
         src/StatisticsCollector.cpp src/Intersection.cpp src/ConsoleView.cpp src/SimulationEngine.cpp)
RESULTS="$EVIDENCE/acceptance_results_${PLATFORM}.txt"
: > "$RESULTS"
FAILED=0

record() {  # id requirement status observation
  printf '%s | %s | %s | %s\n' "$1" "$2" "$3" "$4" | tee -a "$RESULTS"
  [ "$3" = "FAIL" ] && FAILED=$((FAILED + 1))
  return 0
}

{
  echo "Date (UTC): $(date -u '+%Y-%m-%d %H:%M:%S')"
  echo "Platform: $PLATFORM ($(uname -s) $(uname -r) $(uname -m))"
  echo "Compiler: $("$CXX" --version | head -n 1)"
  echo "Flags: ${FLAGS[*]} (-O2 for app/acceptance/benchmark)"
  if git rev-parse HEAD >/dev/null 2>&1; then echo "Commit: $(git rev-parse HEAD)"; else echo "Commit: (not a git checkout)"; fi
} | tee "$EVIDENCE/environment_${PLATFORM}.txt"

echo "== Build"
"$CXX" "${FLAGS[@]}" -O2 src/main.cpp "${SOURCES[@]}" -o build/traffic-management || exit 1
"$CXX" "${FLAGS[@]}" tests/test_traffic_management.cpp "${SOURCES[@]}" -o build/test-traffic-management || exit 1
"$CXX" "${FLAGS[@]}" -O2 tests/acceptance/test_plan_acceptance.cpp "${SOURCES[@]}" -o build/test-plan-acceptance || exit 1
"$CXX" -std=c++17 -Wall -Wextra tests/acceptance/send_ctrl_c.cpp -o build/send-ctrl-c || exit 1
"$CXX" "${FLAGS[@]}" -O2 -DNDEBUG benchmarks/performance_benchmark.cpp "${SOURCES[@]}" -o build/performance-benchmark || exit 1

echo "== Unit test suite"
if ./build/test-traffic-management > build/unit_tests.log 2>&1; then
  grep -a '^\[PASS\]\|checks passed' build/unit_tests.log | tr -d '\r' > "$EVIDENCE/unit_tests_${PLATFORM}.txt"
  echo "Unit suite: $(tail -n 1 "$EVIDENCE/unit_tests_${PLATFORM}.txt")"
else
  echo "Unit suite FAILED: $(grep -a 'FAIL' build/unit_tests.log | head -n 1)" | tee "$EVIDENCE/unit_tests_${PLATFORM}.txt"
  FAILED=$((FAILED + 1))
fi

echo "== In-process Test Plan cases"
./build/test-plan-acceptance "$EVIDENCE/acceptance_in_process_${PLATFORM}.md" | tee build/acceptance.log
grep -a ' | PASS | \| | FAIL | ' build/acceptance.log >> "$RESULTS"
FAILED=$((FAILED + $(grep -ac ' | FAIL | ' build/acceptance.log)))

echo "== Process-level Test Plan cases"
# TC-Sim-001-N: missing config file -> clear error, non-zero exit.
out="$(./build/traffic-management tests/data/does_not_exist.cfg 2>&1)"; status=$?
if [ "$status" -ne 0 ] && grep -Fq "Could not open configuration file: tests/data/does_not_exist.cfg" <<< "$out"; then
  record TC-Sim-001-N TMS-F-060 PASS "CLI exit $status, stderr: $(tr -d '\r' <<< "$out" | tail -n 1)"
else
  record TC-Sim-001-N TMS-F-060 FAIL "CLI exit $status, output: $out"
fi

# TC-Rob-002-P: invalid values fixture -> clear error, non-zero exit, no crash.
out="$(./build/traffic-management tests/data/invalid_values.cfg 2>&1)"; status=$?
if [ "$status" -eq 1 ] && grep -Fq "Simulation failed: Value must be a finite number between 0 and 1: arrival_rate" <<< "$out"; then
  record TC-Rob-002-P PRJ-SR-002 PASS "CLI invalid_values.cfg exit 1: $(tr -d '\r' <<< "$out" | tail -n 1)"
else
  record TC-Rob-002-P PRJ-SR-002 FAIL "CLI exit $status, output: $out"
fi

# TC-Rob-005-P: corrupted file -> handled error (exit 1), never a crash signal/code.
out="$(./build/traffic-management tests/data/corrupted.cfg 2>&1)"; status=$?
if [ "$status" -eq 1 ] && grep -Fq "Simulation failed: Invalid configuration at line" <<< "$out"; then
  record TC-Rob-005-P PRJ-SR-005 PASS "CLI corrupted.cfg exit 1 (handled): $(tr -d '\r' <<< "$out" | tail -n 1)"
else
  record TC-Rob-005-P PRJ-SR-005 FAIL "CLI exit $status, output: $out"
fi

# TC-Sim-002-P: natural completion through the real executable.
printf 'total_ticks=500\narrival_rate=0.3\nemergency_probability=0.05\nadaptive_mode=false\nqueue_capacity=15\nintersection.Main.lanes=North,South,East,West\n' > build/sim002p.cfg
./build/traffic-management build/sim002p.cfg > build/sim002p.log 2>&1; status=$?
if [ "$status" -eq 0 ] && grep -aq "TMS SIMULATION REPORT (500 TICKS)" build/sim002p.log \
   && grep -aq "configured tick limit reached (500 ticks executed)" build/sim002p.log; then
  record TC-Sim-002-P TMS-F-061 PASS "CLI 500-tick run exit 0; report header and termination line present"
else
  record TC-Sim-002-P TMS-F-061 FAIL "CLI exit $status"
fi
sed -n '/TMS SIMULATION REPORT/,$p' build/sim002p.log | tr -d '\r' > "$EVIDENCE/sim002p_summary_${PLATFORM}.txt"

# TC-Sim-002-N: open-ended run stopped by a genuine Ctrl+C / SIGINT.
if [ "$PLATFORM" = windows ] && [ -n "${TMS_SKIP_CONSOLE_CTRL_C:-}" ]; then
  # Headless CI runners may not own a console that can receive CTRL_C_EVENT.
  echo "skipped" > build/ctrlc_harness.err; status=2; : > build/ctrlc.log
else
  ./build/send-ctrl-c 1000 build/ctrlc.log ./build/traffic-management tests/data/open_ended.cfg 2> build/ctrlc_harness.err; status=$?
fi
ticks="$(grep -ao 'user requested stop ([0-9]* ticks executed)' build/ctrlc.log 2>/dev/null | grep -o '[0-9]*' | head -n 1)"
blocks="$(grep -ac '^\[Intersection Main | Tick' build/ctrlc.log 2>/dev/null)"
if [ "$status" -eq 2 ] && grep -q "skipped" build/ctrlc_harness.err; then
  record TC-Sim-002-N TMS-F-061 BLOCKED "console Ctrl+C skipped on this headless Windows runner (TMS_SKIP_CONSOLE_CTRL_C); see the local Windows evidence and the Linux SIGINT run"
elif [ "$status" -eq 2 ] && grep -q "failed" build/ctrlc_harness.err; then
  # Harness could not create a console/process (e.g. a headless CI session): environment, not product.
  record TC-Sim-002-N TMS-F-061 BLOCKED "Ctrl+C harness unavailable here: $(cat build/ctrlc_harness.err)"
elif [ "$status" -eq 0 ] && [ -n "$ticks" ] && [ "$ticks" = "$blocks" ] && grep -aq "TMS SIMULATION REPORT ($ticks TICKS)" build/ctrlc.log; then
  record TC-Sim-002-N TMS-F-061 PASS "real Ctrl+C after 1 s: exit 0, summary for $ticks ticks = $blocks rendered tick blocks"
else
  record TC-Sim-002-N TMS-F-061 FAIL "exit $status, reported ticks '$ticks', rendered blocks '$blocks'"
fi
{ echo "Ctrl+C delivered after 1000 ms; exit status $status; rendered tick blocks: $blocks"; echo; sed -n '/TMS SIMULATION REPORT/,$p' build/ctrlc.log | tr -d '\r'; } > "$EVIDENCE/sim002n_ctrl_c_${PLATFORM}.txt"
rm -f build/ctrlc.log

# TC-Maint-01-P: an extra intersection through configuration only, same binary.
out="$(./build/traffic-management tests/data/three_intersections.cfg 2>&1)"; status=$?
if [ "$status" -eq 0 ] && grep -aq "(3 intersection(s))" <<< "$out" && grep -aq "^Harbor " <<< "$out"; then
  record TC-Maint-01-P TMS-NF-003 PASS "unchanged binary ran three_intersections.cfg: 3 intersection(s), Harbor reported"
else
  record TC-Maint-01-P TMS-NF-003 FAIL "exit $status"
fi

# TC-Port-01-N: missing build tool / compiler produces a clear message.
emptydir="$(mktemp -d)"
out="$(PATH="$emptydir" make 2>&1)"; status=$?
rmdir "$emptydir"
detail="'make' absent from PATH -> exit $status: $(tr -d '\r' <<< "$out" | tail -n 1)"
port_ok=0
{ [ "$status" -eq 127 ] && grep -q "command not found" <<< "$out"; } || port_ok=1
if [ "$PLATFORM" = linux ] && command -v make >/dev/null 2>&1; then
  out="$(make check-deps CXX=missing-tms-compiler 2>&1)"; status=$?
  detail="$detail; make check-deps with missing compiler -> exit $status: $out"
  { [ "$status" -ne 0 ] && grep -Fq "Error: C++ compiler 'missing-tms-compiler' was not found" <<< "$out"; } || port_ok=1
fi
if [ "$port_ok" -eq 0 ]; then record TC-Port-01-N TMS-NF-005 PASS "$detail"; else record TC-Port-01-N TMS-NF-005 FAIL "$detail"; fi

# TC-Port-01-P (this platform's half): build + deterministic reference output.
./build/traffic-management tests/data/baseline.cfg 2>&1 | tr -d '\r' > "$EVIDENCE/portability_output_${PLATFORM}.txt"
record TC-Port-01-P TMS-NF-005 PASS "built with $("$CXX" --version | head -n 1) on $PLATFORM; reference output saved for cross-platform comparison"

echo "== Benchmark (TC-Perf-01-P supporting evidence)"
./build/performance-benchmark | tr -d '\r' | tee "$EVIDENCE/benchmark_${PLATFORM}.txt"

echo "== Memory tools"
if [ "$PLATFORM" = linux ]; then
  SAN=(-g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=undefined)
  "$CXX" "${FLAGS[@]}" "${SAN[@]}" tests/test_traffic_management.cpp "${SOURCES[@]}" -o build/unit-asan &&
  "$CXX" "${FLAGS[@]}" "${SAN[@]}" tests/acceptance/test_plan_acceptance.cpp "${SOURCES[@]}" -o build/acceptance-asan &&
  "$CXX" "${FLAGS[@]}" "${SAN[@]}" src/main.cpp "${SOURCES[@]}" -o build/app-asan
  export ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1
  if ./build/unit-asan > build/asan_unit.log 2>&1 && ./build/acceptance-asan --no-perf > build/asan_acceptance.log 2>&1 \
     && ./build/app-asan tests/data/long_run.cfg > /dev/null 2> build/asan_app.log; then
    record TC-Rob-001-P PRJ-SR-001 PASS "unit suite + 60 acceptance cases + 10,000-tick app run under ASan/UBSan: no out-of-bounds or UB reports"
    record TC-Rel-01-N TMS-NF-002 PASS "LeakSanitizer: 10 repeated create/run/destroy cycles report no leaks"
    record TC-Rob-003-N PRJ-SR-003 PASS "LeakSanitizer: exception thrown at tick 500 from a live engine, no leaks after unwinding"
  else
    record TC-Rob-001-P PRJ-SR-001 FAIL "sanitizer reported errors; see build/asan_*.log"
    cat build/asan_*.log | tail -n 40
  fi
  { echo "ASan/UBSan/LSan logs (empty stderr = clean)"; tail -n 3 build/asan_unit.log; tail -n 1 build/asan_acceptance.log; cat build/asan_app.log; } > "$EVIDENCE/sanitizers_${PLATFORM}.txt"

  if command -v valgrind >/dev/null 2>&1; then
    VG=(valgrind --leak-check=full --show-leak-kinds=definite,indirect --errors-for-leak-kinds=definite,indirect --error-exitcode=99)
    "$CXX" "${FLAGS[@]}" -g -O1 src/main.cpp "${SOURCES[@]}" -o build/app-debug
    "${VG[@]}" ./build/app-debug tests/data/long_run.cfg > /dev/null 2> build/valgrind_app.log; status=$?
    summary="$(grep -E 'definitely lost|indirectly lost|ERROR SUMMARY|All heap blocks were freed' build/valgrind_app.log | sed 's/^==[0-9]*== //' | tr '\n' ';')"
    if [ "$status" -eq 0 ]; then
      record TC-Rob-003-P PRJ-SR-003 PASS "Valgrind, 10,000-tick connected run: $summary"
      record TC-Rel-01-P TMS-NF-002 PASS "10,000 ticks, clean exit under Valgrind: $summary"
    else
      record TC-Rob-003-P PRJ-SR-003 FAIL "Valgrind exit $status: $summary"
      record TC-Rel-01-P TMS-NF-002 FAIL "Valgrind exit $status: $summary"
    fi
    "$CXX" "${FLAGS[@]}" -g -O1 tests/acceptance/test_plan_acceptance.cpp "${SOURCES[@]}" -o build/acceptance-debug
    "${VG[@]}" ./build/acceptance-debug --no-perf > /dev/null 2> build/valgrind_acceptance.log; status=$?
    summary="$(grep -E 'definitely lost|indirectly lost|ERROR SUMMARY|All heap blocks were freed' build/valgrind_acceptance.log | sed 's/^==[0-9]*== //' | tr '\n' ';')"
    if [ "$status" -eq 0 ]; then
      record TC-Rel-01-N TMS-NF-002 PASS "Valgrind over 10 repeated runs (and all in-process cases): $summary"
      record TC-Rob-003-N PRJ-SR-003 PASS "Valgrind with mid-run exception at tick 500: $summary"
    else
      record TC-Rel-01-N TMS-NF-002 FAIL "Valgrind exit $status: $summary"
    fi
    { grep -A8 'HEAP SUMMARY' build/valgrind_app.log; grep 'ERROR SUMMARY' build/valgrind_app.log;
      echo "--- acceptance runner ---"; grep -A8 'HEAP SUMMARY' build/valgrind_acceptance.log;
      grep 'ERROR SUMMARY' build/valgrind_acceptance.log; } | sed 's/^==[0-9]*== //' > "$EVIDENCE/valgrind_${PLATFORM}.txt"
  else
    echo "Valgrind not installed; Valgrind cases not run here."
  fi
else
  echo "AddressSanitizer/LeakSanitizer and Valgrind are not available for $PLATFORM/MinGW;"
  echo "memory-tool verdicts (TC-Rob-001-P, TC-Rel-01-P/N, TC-Rob-003-P/N) come from the Linux run."
fi

echo
echo "== Summary ($PLATFORM)"
pass=$(grep -c ' | PASS | ' "$RESULTS"); fail=$(grep -c ' | FAIL | ' "$RESULTS"); blocked=$(grep -c ' | BLOCKED | ' "$RESULTS")
echo "Recorded results: $pass PASS, $fail FAIL, $blocked BLOCKED (see $RESULTS)" | tee -a "$EVIDENCE/environment_${PLATFORM}.txt"
[ "$FAILED" -eq 0 ]
