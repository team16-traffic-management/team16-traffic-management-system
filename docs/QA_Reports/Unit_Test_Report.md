# Unit Test Report

Status: Main-branch CI run #243 passed Ubuntu and Windows build/test/smoke jobs; Ubuntu printed `All 985 checks passed.` Linux ASan/UBSan and the production-source coverage job passed. Full 66-case acceptance execution remains separate.

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
- System-level gaps are listed in `docs/QA_Reports/System_Validation_Report.md` and `Documents/RTM_Implementation_Status.md`.
- GCOV line/branch coverage is documented separately and must not be inferred from these results.

## Conclusion
Baseline main CI reported 436 checks passed on Ubuntu and Windows. PR #94's manual-stop implementation also passed its Ubuntu and Windows CI jobs, with Ubuntu ASan/UBSan passing, and was merged to main. These are verified automated results for the tested commits, not full system acceptance.


## Latest QA regression additions — PRs #115–#120

| PR | Added regression coverage | CI run | Result |
|---|---|---|---|
| #115 | Sibling-lane queue isolation; zero emergency probability | [#43377370669](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43377370669) | Ubuntu/Windows passed |
| #116 | Emergency probability 1.0 always generates emergency vehicles | [#43377755772](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43377755772) | Ubuntu/Windows and Linux ASan/UBSan passed |
| #117 | Queue-length-one boundary; zero-throughput intersection report; simultaneous overflow across lanes | [#43378089050](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43378089050) | Ubuntu/Windows and Linux ASan/UBSan passed |
| #118 | Runtime adaptive signal mode switch | [#43378320849](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43378320849) | Ubuntu/Windows and Linux ASan/UBSan passed |
| #120 | Adaptive mode loaded from configuration | [#43379351361](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43379351361) | Ubuntu/Windows and Linux ASan/UBSan passed |

PR #121 merged the production-source gcovr coverage job; PR #123 added missing-config and missing-compiler diagnostic checks. Final main run #235 reports 88.0% line coverage (667/758), 86.2% function coverage (81/94) and 54.8% branch coverage (683/1,252). See the coverage reports and main-branch artifact link. These metrics do not convert the 66 Test Plan cases from NOT RUN to PASS.


## Final main-branch verification — run #243

- Workflow: https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43382805876
- Tested commit: `43681916f3f18da3b29d19c4bc79ae10c46ae13e`
- Ubuntu: build, all 985 automated checks, default/config-fixture smoke tests and Linux ASan/UBSan passed.
- Windows: build, all 985 automated checks and default/config-fixture smoke tests passed. Sanitizer and Linux executable upload steps are Linux-only and correctly skipped on Windows.
- The workflow's coverage job generated 88.0% line coverage, 86.2% function coverage and 54.8% branch coverage. See the Code Coverage and Branch Coverage reports for scope and artifact.

PRs #115–#123 are merged and their regression additions are described above. Automated checks remain distinct from the 66 acceptance cases, all of which remain NOT RUN in the execution register.


PR #123 added the missing-config CLI rejection smoke test and a Makefile check-deps diagnostic test. Main run #241 passed the new Ubuntu checks; Windows build/tests/smoke tests also passed (the missing-compiler negative check is intentionally Linux-only).


PR #123 added the missing-config CLI rejection smoke test and a Makefile check-deps diagnostic test. Main run #241 passed the new Ubuntu checks; Windows build/tests/smoke tests also passed (the missing-compiler negative check is intentionally Linux-only).
