# Branch Coverage Report

Status: Initial local GCOV evidence — limited source scope.

## Environment and scope
- Compiler/tool: GCC / GCOV 16.2.0 (MSYS2, local Windows environment).
- Covered translation units: tests/test_traffic_management.cpp and vehicle/VehicleGenerator.cpp.
- Execution date and tested commit: [Record from final rerun].

## Results
| Source file | Branches | Branches executed | Executed | Taken at least once |
|---|---:|---:|---:|---:|
| tests/test_traffic_management.cpp | 86 | 86 | 100.00% | 53.49% |
| vehicle/VehicleGenerator.cpp | 46 | 42 | 91.30% | 67.39% |

## Interpretation
GCOV reports whether branch paths were executed and whether an outcome was taken at least once. These are different measurements; do not describe 100% branches executed as 100% branch outcomes taken.

This report covers only the two listed translation units. It does not establish branch coverage for the entire traffic-management application. Re-run on the final commit and retain raw GCOV output.