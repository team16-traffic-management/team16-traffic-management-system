# Unit Test Report

Status: Baseline main CI run #99 passed; subsequent manual-stop PR #94 also passed cross-platform CI before merge.

## Test environment
| Field | Value |
|---|---|
| Language standard | C++17 |
| Operating systems | GitHub Actions Ubuntu and Windows |
| Test source | `tests/test_traffic_management.cpp` |
| Baseline tested main commit | `f8c6cfda5355f3e2cea2ff2bbd71dae23180c540` |
| Baseline workflow | [Run #99](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42450330604) |
| Manual-stop PR head | `9b8730c4cb0fdb5f8e0a172e40b16912fff49c53` |
| Manual-stop workflow | [PR #94 run #132](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42785408865) |
| Manual-stop merge commit | `339dfed320036ed1c42a375f70bc1189fb5b4244` |

## Automated test scope
- Vehicle generation, metadata, probability boundaries and seeded arrival behavior.
- Bounded lane queues, overflow rejection, FIFO dequeue and empty-dequeue handling.
- Configuration validation, multi-intersection setup and vehicle transfers.
- Signal phase transitions, lane rotation and invalid durations.
- Emergency preemption request acceptance/rejection and deferred activation.
- Statistics, console status, intersection validation and long-run execution.
- PR #98: adaptive green-extension cap, low-queue reset and negative extension rejection.
- PR #99: pending transfer retained under destination queue backpressure and delivered after capacity becomes available.
- PR #101: emergency vehicles are inserted ahead of normal vehicles while preserving FIFO ordering within each class.
- PR #102: console output retains long lane names and renders high queue counts/bars.
- PR #104: vehicle IDs remain unique across 200 generated vehicles from multiple lanes and intersections.
- PR #105: engine dequeues a vehicle only when its lane has a GREEN signal; red-lane vehicle remains queued through its preceding phases.
- PR #107: connected vehicle is delivered and processed exactly one tick after leaving the source lane.
- PR #108: 20 independent SimulationEngine create/configure/run/destroy cycles, 100 ticks each.
- PR #94 adds direct stop-request, open-ended-run, partial-summary and SIGINT regression coverage.

## Baseline main CI results — run #99
| Result | Ubuntu | Windows |
|---|---:|---:|
| Automated checks passed | 436 | 436 |
| Automated checks failed | 0 | 0 |
| Application build | Passed | Passed |
| Default application smoke test | Passed | Passed |
| Multi-intersection startup smoke test | Passed | Passed |
| ASan / UBSan | Passed | Not run (Linux-only workflow step) |

## Manual-stop PR CI results — run #132
| Result | Ubuntu | Windows |
|---|---|---|
| Application and test build | Passed | Passed |
| Automated tests | Passed | Passed |
| Default application smoke test | Passed | Passed |
| Multi-intersection startup smoke test | Passed | Passed |
| ASan / UBSan | Passed | Not run (Linux-only workflow step) |

Run #132 is reported as pass/fail by job outcome; this report does not invent a new assertion count for that run.

## Reproduction commands
```bash
make clean all build/test-traffic-management
./build/test-traffic-management
make sanitize
```

Application smoke tests:
```bash
./build/traffic-management
./build/traffic-management examples/multi_intersection.cfg
```

## Latest merged regression CI
| PR | Coverage | Workflow | Ubuntu | Windows |
|---|---|---|---|---|
| #98 | Adaptive timing boundaries | [Run #141](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42791702831) | Passed, including ASan/UBSan | Passed |
| #99 | Transfer backpressure | [Run #142](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42792945910) | Passed, including ASan/UBSan | Passed |
| #101 | Emergency queue priority/order | [Run #143](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42795154445) | Passed, including ASan/UBSan | Passed |
| #102 | Long lane names/high queue console output | [Run #144](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42795769676) | Passed, including ASan/UBSan | Passed |
| #104 | Vehicle ID uniqueness across lanes/intersections | [Run #153](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42796601039) | Passed, including ASan/UBSan | Passed |
| #105 | Engine-level green-only dequeue | [Run #154](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42796698371) | Passed, including ASan/UBSan | Passed |
| #107 | Exact one-tick transfer latency | [Run #159](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797325343) | Passed, including ASan/UBSan | Passed |
| #108 | Repeated simulation lifecycle | [Run #162](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797660386) | Passed, including ASan/UBSan | Passed |

Post-merge main CI [run #163](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/42797884604) passed Ubuntu and Windows build/tests/smoke tests; Ubuntu ASan/UBSan passed.

## Limitations
- Automated checks do not establish that every SRS acceptance criterion is satisfied.
- At-most-one-GREEN-lane does not prove geometric conflict safety for turning movements.
- New regression tests cover adaptive timing boundaries, transfer backpressure, emergency queue ordering and console long-lane/high-queue output, but do not close the remaining system-level gaps listed in the System Validation Report.
- The 10,000-tick run and PR #108 repeated lifecycle test do not quantify memory growth or independently certify leak-free operation.
- Manual interactive Ctrl+C acceptance and all 66 Test Plan case executions are not yet recorded.
- System-level gaps are listed in `reports/System_Validation_Report.md` and `Documents/RTM_Implementation_Status.md`.
- GCOV line/branch coverage is documented separately and must not be inferred from these results.

## Conclusion
Baseline main CI reported 436 checks passed on Ubuntu and Windows. PR #94's manual-stop implementation also passed its Ubuntu and Windows CI jobs, with Ubuntu ASan/UBSan passing, and was merged to main. These are verified automated results for the tested commits, not full system acceptance.
