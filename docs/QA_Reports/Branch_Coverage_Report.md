# Branch Coverage Report

<badge color="success">Final main-branch branch coverage generated</badge>

## Measurement scope
- Main-branch workflow: [Run #241](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43382188203).
- Tested main commit: `10873b6686019ff4643463888be0050f491de032`.
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
[Download HTML and Cobertura XML coverage artifact](https://github.com/team16-traffic-management/team16-traffic-management-system/actions/runs/43381117586/artifacts/14016947229).

## Interpretation and limitations
Branch coverage is not the same as line coverage. This is the final main-branch result for the commit above. Coverage does not prove geometric movement-conflict safety, runtime fault isolation, or full requirement acceptance. All 66 planned acceptance cases remain NOT RUN until executed and recorded individually.
