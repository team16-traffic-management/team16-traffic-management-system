# Code Coverage Report

Status: Initial local GCOV evidence — limited source scope.

## Environment and scope
- Compiler: GCC 16.2.0 (MSYS2, as shown in local terminal output).
- Tool: GCOV 16.2.0.
- Covered translation units: tests/test_traffic_management.cpp and vehicle/VehicleGenerator.cpp.
- Combined executable line count reported by GCOV: 114.
- Execution date and tested commit: [Record from final rerun].

## Results
| Source file | Lines executed | Executable lines | Line coverage |
|---|---:|---:|---:|
| tests/test_traffic_management.cpp | 72 | 74 | 97.30% |
| vehicle/VehicleGenerator.cpp | 37 | 40 | 92.50% |
| Combined | 109 | 114 | 95.61% |

## Interpretation and limitations
The 95.61% figure applies only to the two translation units listed above. It is not whole-project or whole-system coverage. The application entry point and any additional modules not included in this coverage build are outside this measurement.

Coverage indicates which instrumented lines executed; it does not prove assertions were sufficient or all requirements passed. Re-run on the final commit and retain raw .gcov files or CI artifact.