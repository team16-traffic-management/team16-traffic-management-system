# Branch Coverage Report

<badge color="success">Final main-branch branch coverage generated</badge>

## Measurement scope
- Main-branch workflow: [Run #243](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43382805876).
- Tested main commit: `43681916f3f18da3b29d19c4bc79ae10c46ae13e`.
- Script: `scripts/run_coverage.sh`; GCC `--coverage` at `-O0 -g`.
- Included production translation units: all `.cpp` files under `src/` and `vehicle/`, including `src/main.cpp`, plus instrumented inline code in `vehicle/VehicleGenerator.h`.
- Excluded: test harness source and `include/` header-only code.
- The workflow uploads the gcovr HTML and Cobertura XML report.

## Results

| Metric | Result |
|---|---:|
| Branches executed | 686 / 1,252 |
| Branch coverage | **54.8%** |
| Line coverage (context) | 667 / 758 (88.0%) |

## Artifact
[Download HTML and Cobertura XML coverage artifact](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43382188203/artifacts/14016733343).

## Interpretation and limitations
Branch coverage is not the same as line coverage. This is the final main-branch result for the commit above. Coverage does not prove geometric movement-conflict safety, runtime fault isolation, or full requirement acceptance. All 66 planned acceptance cases remain NOT RUN until executed and recorded individually.
