# S3 demo walkthrough

> ## ⚠ DRAFT — under review, not for release
>
> Every number, table and figure in this folder is real simulator output, produced by the
> commands the scripts describe. But **this material has not been reviewed**, and the wording,
> segment structure and choice of experiments are all still changing. Treat it as work in
> progress rather than as documentation.

A two-part walkthrough of S3, aimed at someone who has never run the simulator.

| Part | What it covers | Length |
|---|---|---|
| **Part 1** — [`PART1_script_running_experiments.md`](PART1_script_running_experiments.md) | Install and build S3; read `config.txt`; run FCFS / PS / SRPT at fixed speed; check the result against M/M/1 and M/G/1 theory; sweep the load; turn on speed scaling and sweep past ρ = 1; measure per-job-size behaviour with probes | 14 segments, ~37 min |
| **Part 2** — [`PART2_script_adding_a_scheduler.md`](PART2_script_adding_a_scheduler.md) | Implement a new scheduling policy end to end — PSJF, preemptive shortest job first — and evaluate it against the shipped policies | 12 segments, ~30 min |

## What's here

```
demo/
├── PART1_script_running_experiments.md   recording script, Part 1
├── PART2_script_adding_a_scheduler.md    recording script, Part 2
├── S3-simulator-walkthrough.html         self-contained slide player (generated)
├── figures/                              the graphs, plus CSVs of the numbers behind them
├── build/                                generators for the page and the figures
└── source/                               PSJF.h / PSJF.cpp — the finished Part 2 policy
```

`S3-simulator-walkthrough.html` is **generated** — open it in a browser to view, but edit
`build/build_demo_player.py` and re-run it rather than editing the HTML. See
[`build/README.md`](build/README.md) for the regeneration loop and for how the figures are
produced. Figures are inlined as data URIs, so the page needs no network access and no server.

## The results in it

All runs are Poisson arrivals, exponential job sizes with E[X] = 1, 100,000 jobs.

**Fixed speed, mean response time** — measured E[T] tracks E[X]/(1−ρ) across 19 loads:

| ρ | FCFS | PS | SRPT | E[X]/(1−ρ) |
|---|---|---|---|---|
| 0.50 | 2.030 | 2.031 | 1.440 | 2.000 |
| 0.80 | 5.264 | 5.251 | 2.432 | 5.000 |
| 0.95 | 22.502 | 22.463 | 6.035 | 20.000 |

**Speed scaling** (`CoupledSpeed INV`, `ALPHA 2`) at ρ = 1.0, where a fixed-speed server is
unstable:

| Policy | E[T] | E[e] | z = E[T]+E[e] |
|---|---|---|---|
| FCFS | 1.538 | 1.538 | 3.076 |
| PS | 1.537 | 1.537 | 3.074 |
| SRPT | 1.341 | 1.341 | 2.681 |

**Mean slowdown by job size**, load 0.8 — the fine probe sizes are what make the
non-preemptive penalty on small jobs visible at all:

| x | FCFS | PS | SRPT | | SJF | PSJF |
|---|---|---|---|---|---|---|
| 0.05 | 105.9 | 6.19 | 1.01 | | 21.3 | 1.005 |
| 1 | 6.25 | 6.13 | 1.47 | | 2.49 | 1.36 |
| 25 | 1.21 | 5.49 | 5.91 | | 2.09 | 6.69 |

## Two things a reader should know

**PSJF is not registered in `src/`.** The finished policy lives in `demo/source/` as reference
material — Part 2 is an exercise in writing it, and shipping it pre-registered would both spoil
that and add a policy to the simulator that nobody asked for. To actually run it, follow the
three registration edits in Part 2, Segment 9.

**Probe instants are shared, deliberately.** `ProbeSetting::computeRandomProbeInsertionTime`
is deterministic, so every probe size and every policy is measured at the same 100 moments.
That makes a comparison between two policies at a given x exact. It also means the whole
slowdown curve inherits one common sampling offset — which is why measured PS sits at 5.4–6.2
where theory says 1/(1−ρ) = 5, smoothly rather than noisily. Part 1 has a segment on this.
