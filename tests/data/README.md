# Reproducible configuration fixtures

These fixtures implement the configuration syntax currently parsed by `SimulationEngine::loadConfigFromFile`. They are manual/integration-test inputs; the existing consolidated C++ test executable does not currently load every fixture automatically.

Run from the repository root after building:

```bash
make
./build/traffic-management tests/data/baseline.cfg
./build/traffic-management tests/data/zero_values.cfg
./build/traffic-management tests/data/high_load.cfg
```

Expected outcomes:
- `baseline.cfg`: loads one intersection and runs 20 ticks.
- `zero_values.cfg`: loads one intersection, creates no arrivals, and completes 10 ticks.
- `high_load.cfg`: loads two connected intersections and runs 200 ticks with high arrival probability and queue capacity 2.
- `invalid_values.cfg`: expected to be rejected with a clear configuration error and non-zero exit status.
- `corrupted.cfg`: expected to be rejected because a non-comment line has no `=` separator.

For the two negative fixtures, run:

```bash
./build/traffic-management tests/data/invalid_values.cfg
./build/traffic-management tests/data/corrupted.cfg
```

A non-zero exit is the expected result for invalid/corrupted inputs. These are fixtures, not evidence that the corresponding Test Plan cases have been executed. Record actual run output and environment in `docs/TEST_EXECUTION_REGISTER.md` before changing any case status.

## Compatibility note

The Test Plan's Appendix B sample uses legacy-style keys such as `arrivalRate`, `emergencyProbability`, `laneCapacity`, `greenDuration` and `yellowDuration`. Those names do not match the current parser's required keys. These committed fixtures use the current parser contract. Update Appendix B and Section 6.1 of the DOCX Test Plan to match this syntax, and separately test zero-capacity queue behavior through the `LaneQueue` API because the configuration parser correctly rejects `queue_capacity=0`.
