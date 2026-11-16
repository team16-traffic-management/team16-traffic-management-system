| Test case | Requirement | Title | Result | Observed |
|---|---|---|---|---|
| TC-Gen-001-P | TMS-F-001 | Vehicles spawn at configured arrival rate | PASS | North=293 South=307 East=297 West=298 (expected 300 +/-10%) |
| TC-Gen-001-N | TMS-F-001 | Zero arrival rate produces zero vehicles | PASS | 500 ticks: throughput=0 blocked=0 queued=0 |
| TC-Gen-002-P | TMS-F-002 | Unique ID, arrival tick and valid type | PASS | 69 vehicles in 50 ticks, 69 unique IDs |
| TC-Gen-002-N | TMS-F-002 | Same-tick spawns across lanes keep unique IDs | PASS | tick 1: 4 same-tick spawns, IDs 1001 1002 1003 1004 |
| TC-Gen-003-P | TMS-F-003 | Vehicles enqueue in FIFO order | PASS | dequeue order: 101 102 103 104 105 |
| TC-Gen-003-N | TMS-F-003 | Emergency insertion does not reorder normal FIFO | PASS | N1,N2,E9,N3 enqueued -> dequeued 9 1 2 3 |
| TC-Queue-001-P | TMS-F-010 | Each lane maintains independent queue state | PASS | North=3 South=0 East=0 West=0 |
| TC-Queue-001-N | TMS-F-010 | Operating on one lane does not corrupt sibling | PASS | after draining North, South holds 2 vehicles: 3,4 |
| TC-Queue-002-P | TMS-F-011 | Capacity limit is enforced and logged | PASS | accept pattern 11100; engine: Target size=3 blocked=2 |
| TC-Queue-002-N | TMS-F-011 | Zero capacity is rejected without crashing | PASS | LaneQueue: "Lane capacity must be greater than zero"; config: "queue_capacity must be between 1 and 10000" |
| TC-Queue-003-P | TMS-F-012 | Dequeue only happens while Green | PASS | 22 Red/Yellow ticks with 2 queued: 0 dequeues; first Green at tick 23 |
| TC-Queue-003-N | TMS-F-012 | Empty Green lane dequeue attempt does not error | PASS | 5 ticks with empty Green lane North: throughput=0 exception=no |
| TC-Queue-004-P | TMS-F-013 | Wait time is calculated correctly | PASS | arrival 5, dequeue 12 -> recorded wait 7 |
| TC-Queue-004-N | TMS-F-013 | Immediate dequeue wait time is zero | PASS | tick 1: throughput=1 average wait=0 |
| TC-Sig-001-P | TMS-F-020 | Fixed-timer cycle follows configured durations | PASS | North phase changes at ticks 10 13; South Green at tick 23: yes |
| TC-Sig-001-N | TMS-F-020 | Zero-duration phase is rejected (cannot freeze) | PASS | yellow_duration=0 -> "yellow_duration must be an integer between 1 and 10000" |
| TC-Sig-002-P | TMS-F-021 | Adaptive mode extends Green for longer queues | PASS | queue=8: Green lasted 20 ticks (base 10, max 20); config max_green_extension=10 |
| TC-Sig-002-N | TMS-F-021 | Adaptive extension is capped for extreme queue | PASS | queue=50: Green lasted 20 ticks (max 20) |
| TC-Sig-003-P | TMS-F-022 | Conflicting directions never simultaneously Green | PASS | 100 ticks logged: N/S+E/W conflicts=0, ticks with >1 Green=0, ticks with a Green=53, preemptions=5 |
| TC-Sig-003-N | TMS-F-022 | Forced same-tick overrides cannot create conflict | PASS | 200 ticks x 2 simultaneous override requests: accepted=40 rejected=360 multi-Green ticks=0 |
| TC-Sig-004-P | TMS-F-023 | Yellow phase occurs before Red | PASS | Yellow ticks between Green and Red: 3 |
| TC-Sig-004-N | TMS-F-023 | One-tick Yellow duration renders correctly | PASS | Yellow=1: Yellow ticks=1, Green->Yellow->Red sequence observed, console shows YELLOW: yes |
| TC-Sig-005-P | TMS-F-024 | Signal mode switches via config alone | PASS | adaptive_mode=false -> adaptive 0; adaptive_mode=true -> adaptive 1 (same binary) |
| TC-Sig-005-N | TMS-F-024 | Invalid adaptive_mode is rejected with clear error | PASS | adaptive_mode=addaptive -> "adaptive_mode must be true or false" |
| TC-Emg-001-P | TMS-F-030 | Emergency vehicles spawn at configured probability | PASS | 188 of 4000 vehicles EMERGENCY (4.70%, expected 5% +/-1.5%) |
| TC-Emg-001-N | TMS-F-030 | Zero emergency probability produces none | PASS | 500 ticks, throughput=217, preemptions=0, generator: 0/500 emergency |
| TC-Emg-002-P | TMS-F-031 | Emergency vehicle dequeued ahead of normal | PASS | 3 normal then emergency 99 -> first dequeued 99 |
| TC-Emg-002-N | TMS-F-031 | Emergency vehicles preserve FIFO among themselves | PASS | A(1) then B(2) -> 1, 2 |
| TC-Emg-003-P | TMS-F-032 | Signal preempts within configured max delay | PASS | emergency in Red East_Bound, preemption logged at tick 1, Green at tick 3 (delay 2, configured max 2) |
| TC-Emg-003-N | TMS-F-032 | Preemption does not create conflicting Green | PASS | East/North per tick: YELLOW/RED YELLOW/RED YELLOW/RED RED/RED RED/GREEN RED/GREEN |
| TC-Multi-001-P | TMS-F-040 | Multiple intersections run independently | PASS | 300 ticks: A(rate 0.2): arrived=107 queued=13 processed=94 B(rate 0.5): arrived=305 queued=180 processed=125 C(rate 0.8): arrived=481 queued=351 processed=130 |
| TC-Multi-001-N | TMS-F-040 | Invalid/overloaded intersection does not affect others | PASS | invalid Two config -> "Empty lane name in intersection: Two"; runtime overload of Two: blocked One/Two/Three=0/309/0, throughput One/Three=81/80 |
| TC-Multi-002-P | TMS-F-041 | Green-wave link passes a vehicle downstream | PASS | vehicle 4242 exits A at tick 1; B.North size after tick 1=0, after tick 2=1 (front id 4242) |
| TC-Multi-002-N | TMS-F-041 | Full downstream lane handles handoff gracefully | PASS | B.North full (size 1/1): vehicle held in A=1, in-flight=0, vehicles accounted=2; after B drains: transfers=1, accounted=2 |
| TC-Multi-003-P | TMS-F-042 | Per-intersection and aggregate stats are correct | PASS | Busy=125 + Quiet=59 = 184; reported aggregate=184 |
| TC-Multi-003-N | TMS-F-042 | Zero-throughput intersection does not break aggregate | PASS | Two: throughput=0 avg wait=0; aggregate=84 == One=84 |
| TC-Stats-001-P | TMS-F-050 | Average wait time is computed correctly | PASS | waits 4,6,8 -> reported average 6 |
| TC-Stats-001-N | TMS-F-050 | Zero dequeue events do not error | PASS | 0 events -> average 0, report prints "0.00 ticks": yes |
| TC-Stats-002-P | TMS-F-051 | Maximum queue length is tracked correctly | PASS | Target peaked at 6, drained to 0; reported max 6 |
| TC-Stats-002-N | TMS-F-051 | Maximum queue length correct when queue <= 1 | PASS | light traffic, Green=10000: arrivals processed=24, reported max queue=1 |
| TC-Stats-003-P | TMS-F-052 | Throughput count is accurate | PASS | 200 ticks: replayed arrivals=307, still queued=222, counted dequeues=85, reported throughput=85 |
| TC-Stats-003-N | TMS-F-052 | Blocked vehicles are not counted as throughput | PASS | 5 arrivals at capacity-3 lane: blocked=2, throughput=3 |
| TC-Sim-001-P | TMS-F-060 | Config values are applied correctly | PASS | lanes=4, Green ended at tick 15 (green_duration=15), observed spawn rate 0.509 (0.5) |
| TC-Sim-001-N | TMS-F-060 | Missing config file is rejected with clear error | PASS | "Could not open configuration file: tests/data/this_file_does_not_exist.cfg" (CLI exit code checked by run_acceptance.sh) |
| TC-Sim-002-P | TMS-F-061 | Natural run completion produces clean summary | PASS | ended at tick 500; summary header present; termination line present |
| TC-Perf-01-P | TMS-NF-001 | Throughput performance target is met | PASS | 4 intersections x 4 lanes, 10,000 ticks: 265913 ticks/s (target >= 500) |
| TC-Perf-01-N | TMS-NF-001 | Performance does not collapse under maximum load | PASS | 4-intersection baseline 286387 ticks/s; 8 chained intersections @0.95 arrival 103935 ticks/s (ratio 0.36, linear expectation 0.50) |
| TC-Rel-01-P | TMS-NF-002 | 10,000-tick run completes (functional part) | PASS | 10,000 ticks completed=yes, throughput=6217 (leak verdict: memory tool run) |
| TC-Rel-01-N | TMS-NF-002 | Repeated runs complete (functional part) | PASS | 10/10 sequential create/run/destroy cycles completed (leak verdict: memory tool run) |
| TC-Maint-01-P | TMS-NF-003 | New topology can be added via config only | PASS | 3rd intersection added by config only: intersections=3, transfers=17, Harbor throughput=23 |
| TC-Maint-01-N | TMS-NF-003 | Invalid topology is rejected | PASS | connection.One=One -> "Invalid configured connection: An intersection cannot connect to itself" |
| TC-UX-01-P | TMS-NF-004 | Console output is readable during a run | PASS | 30 tick blocks, 150 table rows checked, columns aligned=yes |
| TC-UX-01-N | TMS-NF-004 | Output readable with long lane name/high queue | PASS | 29-char lane name, queue 15: 5 rows, columns aligned=yes, 15-char bar=yes |
| TC-Rob-001-N | PRJ-SR-001 | Invalid index access is caught | PASS | lane index 10 of 4 -> out_of_range; generator lane 10 -> "Lane index is out of range"; engine -> "Unknown intersection ID: Intersection-10" |
| TC-Rob-002-P | PRJ-SR-002 | Invalid config values are rejected safely | PASS | arrival_rate=-0.5 -> "Value must be a finite number between 0 and 1: arrival_rate"; queue_capacity=0 -> "queue_capacity must be between 1 and 10000"; invalid_values.cfg -> "Value must be a finite number between 0 and 1: arrival_rate" |
| TC-Rob-002-N | PRJ-SR-002 | Wrong-type config value does not crash parser | PASS | arrival_rate=abc -> "Invalid number for: arrival_rate"; total_ticks=ten -> "Invalid total_ticks value" |
| TC-Rob-003-P | PRJ-SR-003 | Sustained run completes (functional part) | PASS | 10,000-tick connected run completed at tick 10000 (0-byte leak verdict: memory tool run) |
| TC-Rob-003-N | PRJ-SR-003 | Mid-run exception unwinds cleanly (functional part) | PASS | exception "injected fault at tick 500" propagated out of a live engine at tick 500 and was caught; engine destroyed by stack unwinding (0-byte leak verdict: memory tool run) |
| TC-Rob-004-P | PRJ-SR-004 | Queue overflow is handled safely | PASS | capacity 3 @0.95 for 200 ticks: Hot blocked=140, Cold blocked=0, Hot size=3 |
| TC-Rob-004-N | PRJ-SR-004 | Simultaneous overflow on every lane is handled | PASS | all 4 lanes capacity 1 @0.95, 50 ticks: blocked North=39 South=35 East=43 West=47 |
| TC-Rob-005-P | PRJ-SR-005 | Malformed input does not cause abnormal termination | PASS | corrupted.cfg -> "Invalid configuration at line 8" (CLI exit code checked by run_acceptance.sh) |
| TC-Rob-005-N | PRJ-SR-005 | Fault in one module does not terminate simulation | PASS | getCurrentPhase("Ghost-Lane")=RED, preempt("Ghost-Lane")=rejected, stats empty-lane -> "Intersection ID and lane name cannot be empty"; simulation continued to tick 500 |
