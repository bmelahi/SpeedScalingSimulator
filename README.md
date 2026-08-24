# S3: Speed Scaling Simulator

**S3** is a discrete-event simulator for studying the interaction between **job scheduling policies** and **dynamic CPU speed scaling** in a single-server queueing system. It was built to support research into how a scheduler's fairness and performance properties hold up (or break down) once the server's processing speed is allowed to change over time in response to system state — and how scheduling and speed scaling should be designed *together* rather than independently.

This repository is a public, cleaned-up continuation of the original private research codebase, started in 2011 and developed over the course of several papers and student projects since.

## Background

The simulator underlies a line of research on **speed scaling systems** — see [CITATION.cff](CITATION.cff) for the full list of related publications. Two central findings that shaped the simulator's design:

- **Coupled speed scaling breaks fairness.** If a scheduler's speed is set based only on the *current* queue (e.g., "run faster when more jobs are waiting"), provably fair policies like the Fair Sojourn Protocol (FSP) can lose their fairness guarantees once speed scaling is coupled to that same queue.
- **Decoupled speed scaling restores it.** Running a *virtual*, simpler scheduler (e.g., Processor Sharing) purely to decide the execution speed, while a separate real scheduler (e.g., FSP) decides execution order, restores fairness while keeping most of the performance and energy benefits. This is implemented directly in the simulator as the `ShadowSpeed` speed scaler, which wraps an internal "shadow" `Scheduler` instance used only to compute speed.

The simulator was built to let these ideas be evaluated computationally, at scale and across workload distributions, in a way that complements the smaller-scale hardware experiments described in the wider speed-scaling literature.

## What it simulates

At each run, S3 wires together four interchangeable components inside a discrete-event simulator (`DES`):

- **Scheduler** — decides which job runs and for how long. Implementations include FCFS, SJF, SRPT, LRPT, Processor Sharing (PS), the Fair Sojourn Protocol (FSP and FSPEI), Practical Size-Based Scheduling (PSBS), and FEST.
- **SpeedScaler** — decides the execution speed at any point in time. Implementations include `SingleSpeed` (fixed speed), `CoupledSpeed` / `CoupledSpeedWith1Addition` (speed derived from the real scheduler's own queue), `ShadowSpeed` / `ShadowSpeedEstimateSize` (decoupled speed scaling via a virtual scheduler), `LRPTSpeed`, and `RandomSpeed`.
- **PowerFunction** — maps a speed to a power/energy cost. Currently implemented as the standard `AlphaPowerFunction`, P(speed) = speed^alpha.
- **WorkloadGenerator** — produces the arrival stream. Implementations include exponential and Pareto arrival/size distributions, a batch-arrival exponential generator, a "custom" generator, and a file-based generator that replays a workload from a text file.

A `Configuration` object reads a plain-text config file and uses factory methods to construct the chosen Scheduler, SpeedScaler, PowerFunction, WorkloadGenerator, and Logger from it, so new experiments are set up by editing the config file rather than recompiling.

### Probes

Rather than only reporting the average behavior of the whole workload, S3 supports injecting **probe jobs** of specific sizes into an otherwise-unperturbed simulation (`ProbeSetting`), and tracking that probe's own response time and energy. This is what produces the response-time-vs-job-size curves used to compare scheduling/speed-scaling combinations in the associated papers.

## Repository layout

```
S3Sim/               Main Visual Studio project (currently a minimal/scaffold entry point)
src/                 Core simulator: DES engine, schedulers, speed scalers, power
                     functions, workload generators, job/event model, loggers
resources/           Config file template, code templates for adding new
                     schedulers/speed-scalers, and a standalone
                     MeanVariancesComputation helper tool
testprojects/
  DEStest/           The actual runnable driver: reads a config file, builds
                     the simulation objects, runs it, and can post-process
                     the resulting logs into summary reports
  PRandTest/         Small tester for the pseudo-random generator and
                     workload generators
data/
  inputs/            Sample file-based workload input
  outputs/           Simulation output lands here (one timestamped folder
                     per run)
```

## Building

S3 is a native C++ project targeting **Windows**, built with **Visual Studio 2022** (`PlatformToolset v143`). It relies on `<Windows.h>` (for output-folder creation and timestamped folder naming), so it does not build as-is on Linux/macOS without adapting those calls.

1. Open `S3Sim22.sln` in Visual Studio 2022.
2. Build the **DEStest** project under `testprojects/DEStest` — this is the runnable entry point. (The `S3Sim` project's `main()` is currently a minimal scaffold left over from an in-progress refactor and does not run a full simulation; use `DEStest`.)
3. Run the built executable from a working directory that contains a `Config.txt` file (see below) and a `data/` folder alongside it.

## Configuring a run

`DEStest` reads a config file (`Config.txt` next to the executable) in the format documented in [`resources/Configuration_Template.txt`](resources/Configuration_Template.txt). A minimal example:

```
PATH		..\\..\\data\\
SCHEDULER	PS
SPEEDSCALER	ShadowSpeed	SRPT		1
POWERFUNCTION	ALPHA		3
WORKLOAD	EXPONENTIAL	100000		1		0.8
LOGGER		BasicLogger	CONCISE		DELAYEDWRITE
PROBES		100		2		1		50
```

This example runs Processor Sharing scheduling, with decoupled ("shadow") speed scaling driven by a virtual SRPT scheduler, an alpha=3 power function, an exponential-arrival/exponential-size workload of 100,000 jobs, and 100 rounds of probes spanning job sizes 1 to 50.

Each line's format is documented inline in the template file; the `SCHEDULER`, `SPEEDSCALER`, `POWERFUNCTION`, `WORKLOAD`, and `LOGGER` keys are dispatched through the corresponding factory method in `Configuration.cpp`, which is the best place to look to see exactly which parameters each option expects.

## Output

A run creates a timestamped folder under `data/` containing several log files, including: a progress/error/debug log, the simulation parameters used, per-job workload and probe reports (size, arrival, departure, energy, execution time, response time, slowdown), and time-series profiles of queue byte-count, job count, and speed. `testprojects/DEStest/DEStest.cpp` also contains post-processing helpers (`createProbeAvgFile`, `createBusyPeriodReport`) that reduce these raw logs into averaged summary tables.

## Extending the simulator

To add a new scheduling policy or speed-scaling strategy, start from the templates in `resources/header_template.h` and `resources/src_template.cpp`, implement the relevant abstract interface (`Scheduler` in `src/Scheduler.h` or `SpeedScaler` in `src/SpeedScaler.h`), and register the new type in the corresponding factory method in `src/Configuration.cpp` so it becomes selectable from the config file.

## Citing this work

If you use S3 in your own research, please see [`CITATION.cff`](CITATION.cff) for how to cite the software itself and the related publications describing the ideas it implements.

## Acknowledgments

See [`ACKNOWLEDGMENTS.md`](ACKNOWLEDGMENTS.md) for the students who contributed policy implementations to this codebase.

## License

Released under the [MIT License](LICENSE) — free to use, modify, and redistribute, provided the copyright notice is retained.
