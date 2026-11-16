# Test and Quality Reports (Mini-Project Part 3)

These are the four Part 3 testing deliverables plus their raw evidence. All results are for the Sprint 3 code, executed on 16-11-2026.

| Report | Result |
|---|---|
| [Unit_Test_Report.md](Unit_Test_Report.md) | 42 test functions, 1,198 checks, 0 failures |
| [System_Validation_Report.md](System_Validation_Report.md) | 66/66 Test Plan cases executed: 60 PASS, 0 FAIL, 6 PENDING CI (Linux memory tools/portability); 29/33 requirements fully validated |
| [Code_Coverage_Report.md](Code_Coverage_Report.md) | 92.3% lines (849/920), 90.4% functions (103/114) |
| [Branch_Coverage_Report.md](Branch_Coverage_Report.md) | 58.9% raw GCC branches (942/1,600); 85.7% excluding compiler-generated exception branches (926/1,081) |

## Supporting material

| Item | Location |
|---|---|
| Case-by-case results for all 66 cases | [`../TEST_EXECUTION_REGISTER.md`](../TEST_EXECUTION_REGISTER.md) |
| Updated test case document (Test Plan v1.2) | `../../Documents/Test_Plan_Traffic_Management_System.docx` |
| Sprint 3 changes and defects | [`../SPRINT_3.md`](../SPRINT_3.md) |
| HTML coverage report | [`coverage/index.html`](coverage/index.html) |
| Raw evidence (logs, results, environment) | [`evidence/`](evidence/) |

### Evidence files (`evidence/`)

| File | Content |
|---|---|
| `environment_windows.txt` | Date, platform, compiler, flags, and the result count |
| `unit_tests_windows.txt` | Per-test unit results |
| `acceptance_results_windows.txt` | One line per executed Test Plan case: `ID / requirement / result / observed` |
| `acceptance_in_process_windows.md` | Markdown table of the 62 in-process cases |
| `sim002n_ctrl_c_windows.txt` | Summary printed after a real Ctrl+C (TC-Sim-002-N) |
| `sim002p_summary_windows.txt` | Summary of a 500-tick natural completion (TC-Sim-002-P) |
| `portability_output_windows.txt` | Reference output compared with Linux by CI (TC-Port-01-P) |
| `benchmark_windows.txt` | Performance benchmark (TMS-NF-001) |

CI adds the matching `*_linux.txt` files, including `sanitizers_linux.txt` and `valgrind_linux.txt`, to the `acceptance-evidence-Linux` artifact of every run.

## Reproducing

```bash
make test                       # unit suite
make acceptance                 # 62 in-process Test Plan cases
bash scripts/run_acceptance.sh  # all cases + evidence files (make acceptance-full)
bash scripts/run_coverage.sh    # line/branch coverage (needs gcovr: pip install gcovr)
```

On Windows Git Bash, the scripts put the compiler's own `bin` folder first on `PATH`. Without that, Git's older `libstdc++-6.dll` can be loaded instead of MSYS2's and crash `-O2` binaries.
