# Reproducible configuration fixtures

These fixtures use the configuration syntax parsed by `SimulationEngine::loadConfigFromFile`. They are the test data of Test Plan v1.2, Section 6.1, and are used by `scripts/run_acceptance.sh`, `scripts/run_coverage.sh` and the CI smoke tests.

| Fixture | Content | Expected outcome | Used by |
|---|---|---|---|
| `baseline.cfg` | One intersection, 20 ticks | Runs and prints the summary; also the reference output compared between Linux and Windows | CI smoke test, TC-Port-01-P |
| `zero_values.cfg` | Arrival and emergency probability 0 | No vehicles for the whole run | TC-Gen-001-N |
| `high_load.cfg` | Two connected intersections, arrival 0.99, capacity 2 | Runs 200 ticks with blocking and spillback, no crash | CI smoke test |
| `invalid_values.cfg` | `arrival_rate=-0.1` | Rejected: "Value must be a finite number between 0 and 1: arrival_rate", exit 1 | TC-Rob-002-P |
| `corrupted.cfg` | Line 8 has no `=` | Rejected: "Invalid configuration at line 8", exit 1 | TC-Rob-005-P |
| `open_ended.cfg` | `total_ticks=0` | Runs until Ctrl+C, then prints the summary for the completed ticks | TC-Sim-002-N |
| `three_intersections.cfg` | Third intersection added only through config | Unchanged binary runs 3 connected intersections | TC-Maint-01-P |
| `long_run.cfg` | 10,000 connected, adaptive ticks with emergencies and every timing key | Clean exit; used for Valgrind/LeakSanitizer | TC-Rel-01-P, TC-Rob-003-P |

Run from the repository root:

```bash
make
./build/traffic-management tests/data/baseline.cfg
./build/traffic-management tests/data/invalid_values.cfg   # expected: non-zero exit with a clear error
```

Zero queue capacity and zero signal durations are rejected by the parser. Their boundary cases (TC-Queue-002-N, TC-Sig-001-N) are therefore exercised by the acceptance runner with temporary config files and the object APIs, not by a committed fixture.
