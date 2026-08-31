# Part 1 — Installing S3 and Running a Policy Experiment

> **DRAFT — under review, not for release.** Every number and figure below is real
> simulator output, but this script has not been reviewed and the wording and segment
> structure are still changing.

**Target length:** ~37 minutes
**Repository:** https://github.com/bmelahi/SpeedScalingSimulator
**Everything below was executed and verified before recording.** All numbers shown are real output.

**Recording note:** the simulator runs take minutes to tens of minutes. Cut away at each
"RUN" marker and resume on the finished output — the script marks these explicitly.

---

## Segment 1 — What this is  ⏱ 0:00–1:30

**Screen:** title slide, then the GitHub repo page.

**Narration:**
> S3 is a discrete-event simulator for single-server queues. It was built to study two
> things together that are usually studied apart: which job the server picks next — the
> scheduling policy — and how fast the server runs — the speed scaling.
>
> The simulator and its design are described in *Simulation Modeling for Speed Scaling
> Designs*, by Elahi and Williamson, at VALUETOOLS 2016.
>
> It's written in C++ with an extensible architecture. Five kinds of component are
> interchangeable, and you can implement your own of any of them: the **scheduler**, which
> decides which job the server works on next; the **speed scaler**, which decides how fast
> the server runs; the **power function**, which decides what that speed costs in energy; the
> **workload generator**, which decides how jobs arrive and how big they are; and the
> **logger**, which decides what gets measured and written out.
>
> For this video we'll use the default implementations throughout — the classic scheduling
> policies, and the classic exponential workload generator. Part two shows how to write a new
> component of your own, and that's where we'll look at the class diagram.
>
> So: we install it, run three classic policies, and produce two kinds of graph — mean
> response time against load, and slowdown against job size. In part two we add a brand-new
> scheduling policy from scratch.
>
> You need Visual Studio 2022 on Windows, and nothing else. No external libraries.

**On-screen table:**

| Component | Decides |
|---|---|
| `Scheduler` | which job the server works on next |
| `SpeedScaler` | how fast the server runs |
| `PowerFunction` | what that speed costs in energy |
| `WorkloadGenerator` | how jobs arrive and how big they are |
| `DESLogger` | what gets measured and written out |

**On-screen citation card:**
> **Simulation Modeling for Speed Scaling Designs**
> Maryam Elahi and Carey Williamson — VALUETOOLS 2016

**Note for recording:** do *not* show the class diagram here. It belongs to Part 2,
Segment 4 — this is only the forward reference.

---

## Segment 2 — Getting the code  ⏱ 1:30–3:00

**Screen:** terminal.

```bash
git clone https://github.com/bmelahi/SpeedScalingSimulator.git
cd SpeedScalingSimulator
```

**Narration:**
> Clone the repository. The layout is simple: `src` has every scheduler, speed scaler and
> workload generator; `testprojects/DEStest` is the program you actually run; and `data/outputs`
> is where results land, one folder per run.

**Screen:** show the three directories, then `ls src` and point at `FCFS.cpp`, `PS.cpp`, `SRPT.cpp`.

---

## Segment 3 — Building  ⏱ 3:00–5:30

**Screen:** two options, show both briefly.

**Option A — Visual Studio:** open `S3Sim22.sln`, set configuration to **Release / Win32**, Build Solution.

**Option B — command line** (this is what the rest of the video uses):

```bat
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" ^
   testprojects\DEStest\DEStest.vcxproj /p:Configuration=Release /p:Platform=Win32
```

**Narration:**
> Build in Release, not Debug — Debug is roughly three times slower and these runs are long
> enough already. The executable lands in `testprojects\DEStest\Bin\Release\DEStest.exe`.
>
> One thing worth knowing: build the project file directly, as I'm doing here, rather than
> only through the solution. Both work.

---

## Segment 4 — Structure of config.txt  ⏱ 5:30–9:30

**Screen:** `testprojects\DEStest\config.txt`, scrolled to the seven active lines at the bottom.

```
PATH		..\\..\\data\\
SCHEDULER	FCFS
SPEEDSCALER	SingleSpeed	1		1
POWERFUNCTION	ALPHA		2
WORKLOAD	EXPONENTIAL	100000		0.80		1
LOGGER		BasicLogger	CONCISE		PROMTWRITE
PROBES		300		2		1		50
```

**Narration:**
> The whole experiment is described by these seven lines. Everything above them is a comment
> block listing the legal values — that block is your reference, keep it.
>
> `SCHEDULER` picks the policy. `SPEEDSCALER` decides how fast the server runs — `SingleSpeed 1 1`
> means constant speed 1, so this is an ordinary fixed-speed queue. `POWERFUNCTION ALPHA 2`
> says power is speed squared; it's ignored while the speed is constant, but it matters the
> moment we turn scaling on.
>
> `WORKLOAD EXPONENTIAL` takes a job count, an arrival rate, and a mean job size. With mean
> size 1, the arrival rate *is* the load — so `0.80` here means a load of 0.8.
>
> `PROBES` we'll come back to in segment 8.
>
> And one warning: no blank lines allowed in this config.txt file. Make sure there is no
> blank line in the middle of the experiment description — the parser stops at the blank line.

**⚠ On-screen callout:**
> **Warning: no blank lines allowed in this `config.txt` file.** Make sure there is no
> blank line in the middle of the experiment description — the parser stops at the
> blank line.

---

## Segment 5 — Example: average response time for three policies at fixed speed  ⏱ 9:30–14:00

**Screen:** edit `SCHEDULER` to `FCFS`, set `PROBES` to `0  2  1  1`, run.

```
SCHEDULER	FCFS
SPEEDSCALER	SingleSpeed	1		1
WORKLOAD	EXPONENTIAL	100000		0.80		1
PROBES		0		2		1		1
```

```bat
cd testprojects\DEStest
Bin\Release\DEStest.exe
```

**Narration:**
> For now we set the probe count to zero — we'll come back to what probes are, and what we use
> them for, later in this video.
>
> With probes off, the run simulates the workload once and reports the averages and variances
> over that run. Three of them are time averages: **E[N]**, the average number of jobs in the
> system; **E[S]**, the average execution speed; and **E[B]**, the average bytes in the system.
> Then **E[T]**, the average response time, which is per job rather than per unit time; and
> **E[Busy]**, the average busy-period length, which is per busy period.
>
> Then repeat with `SCHEDULER PS` and `SCHEDULER SRPT`. In practice, copy the folder three
> times so the runs don't overwrite each other and can run in parallel.

**On-screen table:**

| | Quantity | Averaged over |
|---|---|---|
| `E[N]` | average number of jobs in system | time |
| `E[S]` | average execution speed | time |
| `E[B]` | average bytes in system | time |
| `E[T]` | average response time | per job |
| `E[Busy]` | average busy-period length | per busy period |

**Where each comes from:** the three time averages come from the timestamped profile logs the
run writes — `jobcountprofile.txt`, `speedprofile.txt`, `byteprofile.txt`. `E[T]` is summarised
in `meanzlog.txt`. Busy-period statistics come from `createBusyPeriodReport()`, which is
**commented out** in `DEStest.cpp` (~line 101) — uncomment it if you want `E[Busy]`.

**▶ RUN — cut away here.** (~15 seconds per policy at N=100,000.)

**Screen:** `data\outputs\FCFS-1--ALPHA-2--EXPONENTIAL-100000-0.80-1--<timestamp>\meanzlog.txt`

**Real results:**

| Policy | E[T] |
|---|---|
| FCFS | 5.264 |
| PS | 5.251 |
| SRPT | 2.432 |

**Narration:**
> The output folder name encodes the whole configuration, so runs never collide and you can
> always tell what produced what. Inside, `meanzlog.txt` holds the summary — the first column
> is mean response time.
>
> FCFS and PS both land near 5.25, and SRPT at 2.43 — roughly half. That's the classic result:
> SRPT is optimal for mean response time.

**Screen:** show `figures/fig1_ET_two_settings.png` (left panel).

---

## Segment 6 — Theory moment: checking the measured E[T] against M/M/1 and M/G/1  ⏱ 14:00–17:30

**Screen:** the derivation, written out.

```
Assume  ρ = λ·E[X] < 1.

Little's Law            E[N] = λ·E[T]
M/M/1 FCFS, M/G/1 PS    E[N] = ρ / (1 - ρ)
                        ――――――――――――――――――――
                        E[T] = E[X] / (1 - ρ)

Here  λ = 0.8,  E[X] = 1   →   E[T] = 1 / 0.2 = 5
```

**Narration:**
> Let's check that against theory. Assume the load ρ — lambda times mean job size — is below
> one. Little's Law says the expected number in system is lambda times expected response time.
> And for M/M/1 under FCFS, and M/G/1 under processor sharing, the expected number in system
> is ρ over one minus ρ. Put those together and expected response time is mean job size over
> one minus ρ.
>
> Here lambda is 0.8 and mean job size is 1, so theory says exactly **5**. We measured 5.264.
> Where does the extra quarter come from?
>
> **It is an artifact of the random number generator in this experiment** — of the particular
> sample path this run happened to draw. It is not a defect in the simulator, and not an error
> in the theory. Run it again with a different seed and you get a different number, scattered
> around 5. Two things are going on.

**Screen:** the realized-parameters table.

| Quantity | Nominal | Realized |
|---|---|---|
| E[X] | 1.00000 | **1.00471** |
| λ | 0.80000 | 0.80007 |
| ρ = λ·E[X] | 0.80000 | **0.80384** |
| E[X]/(1−ρ) | 5.000 | **5.122** |

**Narration:**
> **One — the draw came out heavy.** The realized mean job size wasn't exactly 1; it came out
> at 1.00471, about half a percent high. Since ρ is lambda times E of X, that lifts the realized
> load to 0.8038, and the prediction to 5.122. That closes about half the gap.
>
> **Two — the same draw scatters E[T] itself**, far more than a hundred thousand jobs would
> suggest. Response times in a queue are strongly autocorrelated, because
> consecutive jobs share the same backlog. Here the lag-one autocorrelation is 0.966.

| Standard error of E[T] | Value | |
|---|---|---|
| assuming independence | 0.017 | wrong — ignores autocorrelation |
| batch means, 50 batches | **0.149** | honest |

**Narration:**
> So the honest 95% interval is about 4.97 to 5.56, which contains both 5.000 and 5.122. The
> discrepancy is **not statistically significant**.
>
> And note how steep this is near saturation: the derivative of E[T] with respect to ρ is
> E[X] over one-minus-ρ squared, which is 25 here. A 0.004 error in ρ alone moves E[T] by 0.1.
> That's the real lesson — don't read a single number off a queueing simulation without a
> confidence interval, and don't compute that interval as if the samples were independent.

---

## Segment 7 — Sweeping the load at fixed speed  ⏱ 17:30–20:30

**Narration:**
> Running one load at a time gets tedious. The simulator has a built-in load sweep — `runLoad`
> — but it ships switched off. Turning it on is **one edit**.

**Edit** — `testprojects/DEStest/DEStest.cpp`, around line 81. Flip the comment marker:

```cpp
/*                          →      //*
// run the probesimulator for load range
if (probesim.runLoad()) {
```

**Narration:**
> This is a comment-toggle. As shipped, the block calling `runLoad` is commented out and the
> normal `run` is active. Changing slash-star to slash-slash-star flips which half compiles.
> Rebuild.

**Screen:** `src/ProbeSetting.cpp`, in `runLoad()` — show the range, don't change it:

```cpp
for (double i = 0.05; i <= 0.96; i += 0.05) {
//  19 loads: 0.05, 0.10, ... 0.95  — the shipped range
```

**Narration:**
> The sweep range is hard-coded here. The shipped range is nineteen loads from 0.05 to 0.95,
> and it stops below one on purpose: at fixed speed, load one is the stability boundary. Leave
> it alone. We rerun the same three configs with nothing else changed — still
> `SingleSpeed 1 1`, still `PROBES 0`.

**▶ RUN — cut away.** (19 load points per policy.)

---

## Segment 8 — The fixed-speed load graph  ⏱ 20:30–22:30

**Screen:** `figures/fig_loadsweep_singlespeed.png`

**Real results** (raw E[T], 100,000 jobs per point; full table in
`figures/fig_loadsweep_singlespeed.csv`):

| ρ | FCFS | PS | SRPT | E[X]/(1−ρ) |
|---|---|---|---|---|
| 0.50 | 2.030 | 2.031 | 1.440 | 2.000 |
| 0.80 | 5.264 | 5.251 | 2.432 | 5.000 |
| 0.90 | 10.594 | 10.548 | 3.703 | 10.000 |
| 0.95 | 22.502 | 22.463 | 6.035 | 20.000 |

**Narration:**
> Three things to read off this.
>
> First, the dotted line is the theory we just derived, E[X] over one minus rho — and FCFS and
> PS sit right on it across the whole range. So the check we did at load 0.8 wasn't a
> coincidence of one load point.
>
> Second, the curve is a wall. E[T] is about 2 at load 0.5, 5 at 0.8, and 22 at 0.95. That
> blow-up as rho approaches one is the whole reason speed scaling exists — which is where we
> go next.
>
> Third, FCFS and PS lie exactly on top of each other. That's not a plotting mistake, it's a
> real property: for exponentially distributed job sizes those two policies have the same mean
> response time. SRPT is well below both.

---

## Segment 9 — Theory moment: stability, and how fast to run  ⏱ 22:30–25:30

**Narration:**
> Before we look at any speed-scaling results, one question: what speed *should* the server run
> at? The speed is a free choice — S3 will run any speed function you can implement. But for a
> run to mean anything the system has to be **stable**: the speed has to meet the workload demand.

```
fixed speed  s = 1        stable  ⇔  ρ = λ·E[X] < 1
variable     s(t)         you must ensure it keeps up with the load
```

**Narration:**
> At a single fixed speed of 1, that's just load below 1 — which is exactly the wall we saw in the
> last graph. If you make the speed variable, it's on you to make sure it matches the load.
>
> Now, how fast? Running faster lowers response time but costs energy. A robust policy adjusts
> dynamically and spends extra joules only in proportion to the response time they buy — one
> joule per unit of mean response time removed. That's the objective:

```
minimize   z = E[T] + E[e]

  E[T]   mean response time per job
  E[e]   mean energy per job

S3 reports all three: meanzlog.txt has columns  E[T]  E[e]  Z  N
```

**Narration:**
> And `CoupledSpeed`'s answer is to set the speed to the inverse power function of the queue length:

```
P(s)  = s^α                     the power function, α = 2
s(N)  = P⁻¹(N) · base  =  N^(1/α)      speed at queue length N
      = √N                       here, with base = 1
s(0)  = 0                       empty system, speed gated to zero
```

**Narration:**
> So with alpha 2, the speed at any moment is the square root of the number of jobs in the system.
>
> One consequence is worth knowing. The power drawn is then P(P⁻¹(N)) = N, so energy accrues at
> exactly the rate jobs accumulate — and by Little's Law, energy per job comes out equal to
> response time per job. The runs confirm it: E[e] = E[T] to six decimals for all three policies,
> so here z is simply 2·E[T].

*(Verified in the source: `CoupledSpeed::getExSpeed` returns `P_m->inverseFunction(jobCount_m) *
baseSpeed_m`; `AlphaPowerFunction::inverseFunction(N)` returns `pow(N, 1/alpha)`.)*

---

## Segment 10 — Turning on speed scaling  ⏱ 25:30–27:30

**Screen:** change two lines only.

```
SPEEDSCALER	CoupledSpeed	INV	1
WORKLOAD	EXPONENTIAL	100000		1.00		1
```

**Narration:**
> Now we run it. Two lines change from the fixed-speed config — the speed scaler, and the load,
> which is now one-point-zero. That's exactly the point where the fixed-speed system blew up: at a
> single speed, load 1 is the stability boundary and mean response time is infinite. With speed
> scaling the server simply speeds up, so it stays perfectly stable.

**▶ RUN — cut away.**

**Real results at load 1.0, α=2:**

| Policy | E[T] | E[e] | z = E[T]+E[e] |
|---|---|---|---|
| FCFS | 1.538 | 1.538 | 3.076 |
| PS | 1.537 | 1.537 | 3.074 |
| SRPT | 1.341 | 1.341 | 2.681 |

**Narration:**
> All three are stable, and response times are *lower* than the fixed-speed run at load 0.8 —
> because the server is now allowed to run faster than 1. You pay for that in energy — and there's
> the identity from the previous slide, E[e] equal to E[T] in every row.

**Screen:** `figures/fig1_ET_two_settings.png` (both panels side by side).

---

## Segment 11 — Sweeping past load 1  ⏱ 27:30–30:30

**Narration:**
> Now we can ask the question the fixed-speed sweep couldn't. The `runLoad` toggle is already
> flipped from before, so this time there's just **one more edit**.

**Edit** — `src/ProbeSetting.cpp`, in `runLoad()`:

```cpp
for (double i = 0.05; i <= 0.96; i += 0.05) {    // shipped
for (double i = 0.1;  i <= 2.01; i += 0.1)  {    // ours — speed scaling is stable past 1
```

**Narration:**
> The shipped range stopped just below one because that was the stability limit at fixed speed.
> Speed scaling removes that limit, so there's now something to see above it. Extend the range
> to two and rebuild.

**▶ RUN — cut away.** (20 load points per policy, ~3 minutes each.)

**Screen:** `figures/fig2_loadsweep_speedscaling.png`

**Narration:**
> Here's the payoff — and it's worth putting side by side with the fixed-speed graph.
> Twenty load points from 0.1 to 2.0, three policies, under speed scaling.
> The dotted line is load one — where the fixed-speed system blew up. Nothing happens
> there at all; the curves continue smoothly.
>
> SRPT stays lowest throughout. FCFS and PS lie exactly on top of each other — same real
> property we saw at fixed speed.

---

## Segment 12 — Probes: the slowdown graph  ⏱ 30:30–34:30

**Narration:**
> The last thing is a different question. Not "how does the system do on average", but "how
> does the system treat a job of a *particular* size". That's what probes are for.

```
PROBES		100		2		1		25		coarse sweep
PROBES		100		0.45		0.05		0.5		fine, for small jobs
#		rounds		step		smallest	largest
```

**Narration:**
> Four numbers: rounds, step, smallest, largest. The first line says — insert a probe job of size 1
> at a hundred fixed moments in the simulated trace; then size 3, then 5, and so on in steps of 2
> up to 25. Each probe goes into an otherwise identical run, so we measure exactly how that one
> job would have been treated.
>
> The second line is the point of this segment. **The unfairness of a non-preemptive policy to
> small jobs is a known theoretical result, and with the coarse sweep starting at x = 1 you simply
> cannot see it.** So we add two fine sizes, 0.05 and 0.5, as a check that it really shows up in
> measurement.
>
> Be aware this is expensive: fifteen sizes at a hundred rounds is fifteen hundred full
> simulations per policy. Start it and go get coffee.

**▶ RUN — cut away.**

**Screen:** `probereport.txt`, then `figures/fig3_slowdown_vs_size.png`

**Real results** (mean slowdown E[T(x)]/x, load 0.8; full table in
`figures/fig3_slowdown_vs_size.csv`):

| x | FCFS | PS | SRPT |
|---|---|---|---|
| 0.05 | **105.9** | 6.19 | 1.01 |
| 0.5 | 11.5 | 6.13 | 1.21 |
| 1 | 6.25 | 6.13 | 1.47 |
| 25 | 1.21 | 5.49 | 5.91 |

**Narration:**
> `probereport.txt` has one row per probe — its size, when it arrived, its response time and its
> slowdown, which is response time divided by size.
>
> Averaging by size gives the fairness picture, and the two fine sizes earn their keep. PS is the
> flat reference: every size gets the same slowdown, which is why it's the usual definition of
> "fair". FCFS is non-preemptive, and a tiny job stuck behind a long one waits just as long as
> anyone else — at size 0.05 its slowdown is **106**, against PS at 6. You could not see that at
> all with the coarse sweep. SRPT is the mirror image: excellent for small jobs, and the price is
> paid by the large ones.

---

## Segment 13 — A caveat on the probe level  ⏱ 34:30–36:00

**Narration:**
> One caveat, because it will be the first thing a careful viewer asks. PS should sit at exactly
> 1/(1−ρ) = 5. It measures between 5.4 and 6.2 — and the curve is smooth rather than noisy.
>
> That is not a simulator error. **Every size shares the same 100 probe instants**, and this
> particular sample of instants happened to be busier than average. The offset is common to every
> size and every policy, which is why the curve is smooth instead of scattered, and why it
> *cancels* when you compare two policies at the same x.
>
> That sharing is the point of the design: FCFS at x = 0.05 and SRPT at x = 0.05 saw the identical
> system, down to the arrival instant. Probe insertion is deterministic —
> `startPeriod + (endPeriod − startPeriod)/rounds * thisround` — so it depends only on the round
> count and the background trace, not on the policy or the probe size.

---

## Segment 14 — Wrap  ⏱ 36:00–37:00

**Narration:**
> To recap: seven config lines describe an experiment; `meanzlog.txt` gives you averages;
> `runLoad` sweeps a range of loads; and probes give you per-job-size behaviour. Output folders
> are self-describing, so results never get confused with each other.
>
> In part two we add an entirely new scheduling policy — about sixty lines of real code — and
> evaluate it against the ones we just ran.

---

## Appendix — verified reference

| Item | Value |
|---|---|
| Repo | `github.com/bmelahi/SpeedScalingSimulator` |
| Build | Release / Win32, VS2022 (v143) |
| Executable | `testprojects\DEStest\Bin\Release\DEStest.exe` |
| Config | `testprojects\DEStest\config.txt` — **no blank lines** |
| Results | `data\outputs\<POLICY>-<SPEED>--<POWER>--<WORKLOAD>--<timestamp>\` |
| Summary file | `meanzlog.txt` — columns: `E[T]  E[e]  Z  N` |
| Per-probe file | `probereport.txt` — one row per probe |
| runLoad toggle | `testprojects/DEStest/DEStest.cpp` ~line 81 |
| Sweep range | `src/ProbeSetting.cpp`, `runLoad()`, the `for` loop |
