# Branch Coverage Report

<badge color="success">PR CI branch coverage generated</badge>

## Measurement scope
- Workflow: [PR #121 CI run #230](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43380638801).
- Tested PR head: `7e8b6c58666abfea2fdcc4161217346deed76068` (before merge to main).
- Script: `scripts/run_coverage.sh`; GCC `--coverage` at `-O0 -g`.
- Included production translation units: all `.cpp` files under `src/` and `vehicle/`, including `src/main.cpp`, plus instrumented inline code in `vehicle/VehicleGenerator.h`.
- Excluded: test harness source and `include/` header-only code.
- The workflow uploads the gcovr HTML and Cobertura XML report.

## Results

| Metric | Result |
|---|---:|
| Branches executed | 683 / 1,252 |
| Branch coverage | **54.6%** |
| Line coverage (context) | 667 / 758 (88.0%) |

## Artifact
[Download HTML and Cobertura XML coverage artifact](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43380638801/artifacts/14016517314).

## Interpretation and limitations
Branch coverage is not the same as line coverage. The current result is from the PR branch before merge; rerun on final main and update the commit/run reference. Coverage does not prove geometric movement-conflict safety, runtime fault isolation, or full requirement acceptance. All 66 planned acceptance cases remain NOT RUN until executed and recorded individually.
