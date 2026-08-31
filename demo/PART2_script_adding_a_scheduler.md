# Part 2 — Adding a New Scheduling Policy

> **DRAFT — under review, not for release.** Every number and figure below is real
> simulator output, but this script has not been reviewed and the wording and segment
> structure are still changing.

**Target length:** ~30 minutes
**What we build:** PSJF — Preemptive Shortest Job First
**Everything below was written, compiled and run before recording.**

---

## Segment 1 — The policies we already have  ⏱ 0:00–2:00

**Screen:** `src/SJF.h` and `src/SRPT.h` side by side.

**Narration:**
> Every scheduling policy answers one question: of the jobs in the system right now, which one
> should the server work on? The simulator already ships with two answers based on job size.
>
> **SJF — Shortest Job First.** Always start the smallest job in the queue, measured by its
> original size. And once a job starts, it runs to completion. A small job that arrives while
> a long one is in service simply has to wait. That's a non-preemptive policy.
>
> **SRPT — Shortest Remaining Processing Time.** Always run the job with the least work *left*,
> and re-make that decision whenever a job arrives — so a small newcomer can interrupt whatever
> is running. And because a job's remaining size shrinks as it is served, a job steadily climbs
> the priority order the longer it runs. That's preemptive.

---

## Segment 2 — The policy we're adding  ⏱ 2:00–3:45

**Screen:** a slide naming PSJF.

**Narration:**
> Today we add a third: **PSJF — Preemptive Shortest Job First.** Run the smallest job by
> *original* size, and re-decide at every arrival. So it preempts like SRPT, but sorts like SJF.
>
> And here is the consequence worth holding on to, because it's what the results at the end
> will show. Under PSJF a job's priority **never improves as it is served** — its original size
> doesn't change, no matter how much work you've already done on it. A long job can be
> preempted again and again and never work its way up the queue.

---

## Segment 3 — Two decisions, three policies  ⏱ 3:45–5:30

**Screen:** the table.

| Policy | Sorts on | Can switch | Preemptive? |
|---|---|---|---|
| `SJF` | original size | only at completion | no |
| **`PSJF`** | **original size** | **at every arrival** | **yes** |
| `SRPT` | remaining size | at every arrival | yes |

**Narration:**
> Side by side, the three differ along just two axes: what you sort on, and when you're allowed
> to switch.
>
> Read down the table. Going from SJF to PSJF changes only the switching rule — same sort key,
> but now preemption is allowed. Going from PSJF to SRPT changes only the sort key — still
> preemptive, but now on remaining size instead of original size.
>
> That's what makes this a real comparison rather than three unrelated curves: any difference
> we measure at the end is attributable to exactly one design decision at a time.

---

## Segment 4 — How the simulator is put together  ⏱ 5:30–8:00

**Screen:** the class diagram (rendered in the walkthrough artifact, Part 2 Segment 4 —
export it from there, or redraw from the structure below).

```
        ProbeSetting            Configuration           Job · Event
        run() / runLoad()       factories, config.txt   Arrival/Departure/SpeedChange
              |                       |                       |
        ┌─────┴───────────────────────┴───────────────────────┴─────┐
        │            DES  —  discrete-event core                     │
        │            event loop · owns the five components below     │
        └──┬──────────┬──────────────┬──────────────┬────────────┬───┘
           │          │              │              │            │
      Scheduler   SpeedScaler   WorkloadGen   PowerFunction  DESLogger
      (abstract)  (abstract)    (abstract)     (abstract)    (abstract)
       14 impls     7 impls       8 impls        1 impl        2 impls
      FCFS PS      SingleSpeed   Exponential   AlphaPower    BasicLogger
      SRPT SJF     CoupledSpeed  Pareto        Function      RoundZero
      PSJF FSP     ShadowSpeed   Weibull                     Logger
      FEST SEH …   …             File/Custom …
```

**Narration:**
> Before writing code, here's how the simulator is put together.
>
> `DES` runs the event loop, and owns five components — Scheduler, SpeedScaler,
> WorkloadGenerator, PowerFunction and DESLogger. Each is an abstract base class with
> interchangeable implementations, and `Configuration` builds them by factory from the config
> file. So an entire run is assembled at start-up from those seven lines of text we saw in
> part one.
>
> That's what makes the simulator extensible in two independent directions: the *policies* —
> Scheduler and SpeedScaler — and the *instrumentation* — the logger and the probe machinery.
> Adding to one doesn't touch the other.
>
> Today we're extending the left-hand column, Scheduler, which already has fourteen
> implementations. We're about to make it fifteen.

**Reference:** *Simulation Modeling for Speed Scaling Designs*, Elahi & Williamson,
VALUETOOLS 2016 — cited in Part 1.

---

## Segment 5 — The Scheduler interface  ⏱ 8:00–11:00

**Screen:** `src/Scheduler.h`, then `src/SRPT.h` beside it.

**Narration:**
> Every policy is a class deriving from `Scheduler`, and the simulator only ever talks to it
> through a fixed set of methods — these nine.

| Method | What it does |
|---|---|
| `toString` | name and parameters of the object |
| `arrival_handler` | a job arrives; true if the running job changed |
| `departure_handler` | a job completes; returns the finished `Job` |
| `speedchange_handler` | the speed scaler changed speed |
| `bonusevent_handler` | a policy-defined event fires |
| `getExJob` | which job is executing right now |
| `nextDeparture` | when the running job finishes at this speed |
| `nextScheduler` | schedule a policy-defined future event |
| `updatePeriod` | apply the passage of time over [t1, t2] |

**Narration:**
> How much of this you end up writing yourself **depends on the policy**. For a policy that is
> just an ordering over a priority queue — SRPT, SJF, and the PSJF we're about to write — most of
> these come out the same and you copy them across.
>
> For a policy that serves several jobs at once they don't. PS shares the speed across every job
> in the system, so its `updatePeriod`, `nextDeparture` and `departure_handler` are genuinely
> different code — it doesn't even use a `priority_queue`, it uses a `set`.
>
> Ours is a priority-queue policy, so for us the interesting part is small.

---

## Segment 6 — Where the policy actually lives  ⏱ 11:00–13:30

**Screen:** `src/SRPT.h`, the `CompareJobSRPT` class.

**Narration:**
> The queue is a C++ priority queue, and the comparator is what makes one policy different
> from another. Here's SRPT's — it compares `remsize_m`, the remaining size.

```cpp
if (approximatelyEqual(j1->remsize_m, j2->remsize_m)) { ... }
else
    return definitelyGreaterThan(j1->remsize_m, j2->remsize_m);
```

**Narration:**
> Two things to notice, because both will bite you if you miss them.
>
> First, the comparison is backwards from what you'd expect. A C++ priority queue puts the
> *largest* element on top, so to get the *smallest* job on top we return "greater than".
>
> Second, never compare doubles with `==`. The codebase gives you `approximatelyEqual` and
> `definitelyGreaterThan` — use them. Floating-point job sizes will not compare exactly.

---

## Segment 7 — Writing PSJF.h  ⏱ 13:30–17:00

**Screen:** create `src/PSJF.h`, type the comparator live.

```cpp
class CompareJobPSJF {
public:
    bool operator()(const Job * j1, const Job * j2) const {
        if (approximatelyEqual(j1->getSize(), j2->getSize())) {
            if (j1->isBeingExecuted_m)      return false;
            else if (j2->isBeingExecuted_m) return true;
            else return definitelyGreaterThan(j1->getArrival(), j2->getArrival());
        }
        else
            return definitelyGreaterThan(j1->getSize(), j2->getSize());
    }
};
```

**Narration:**
> This is the entire policy. One character of real difference from SRPT: `getSize()` instead of
> `remsize_m`. Original size, not remaining size.
>
> The tie-break is worth explaining. If two jobs are the same size, prefer the one already
> running — otherwise the server can thrash, context-switching between equal jobs and making
> no progress. If neither is running, take the earlier arrival, so the policy is deterministic.

**Screen:** the rest of the header — the class declaration and the `priority_queue` member.

```cpp
std::priority_queue<Job*, std::vector<Job*>, CompareJobPSJF> jobs_q;
```

---

## Segment 8 — Writing PSJF.cpp  ⏱ 17:00–21:00

**Screen:** create `src/PSJF.cpp`. Focus on `arrival_handler`; scroll the rest.

```cpp
bool PSJF::arrival_handler(ArrivalEvent * e) {
    Job * top = NULL;
    if (!jobs_q.empty())  top = jobs_q.top();

    Job * newjob = new Job(e->job_m);
    jobs_q.push(newjob);

    // If the new job is now on top, it has preempted whatever was running.
    if (jobs_q.top()->getID() == newjob->getID()) {
        jobs_q.top()->isBeingExecuted_m = true;
        if (top != NULL) top->isBeingExecuted_m = false;
        return true;                    // true = "the running job changed"
    }
    else
        return false;
}
```

**Narration:**
> This is the second and last piece of real policy. Note we don't decide anything ourselves —
> we push the job and then *ask the queue* whether the top changed. The comparator already
> encodes the priority rule, so this stays correct no matter what ordering we chose.
>
> Returning true tells the simulator the running job changed, so it must recompute when the
> next departure will happen. Get this wrong and you'll get "illegal departure" errors in the
> log.
>
> Contrast this with the non-preemptive `SJF`, which instead checks `if (current_job_m == NULL)`
> and only starts a job when the server is idle. That single difference is the whole of
> preemptive versus non-preemptive.

**Screen:** scroll `updatePeriod`, `departure_handler`, `nextDeparture` quickly.

**Narration:**
> The remaining methods I've copied from SRPT unchanged — they consume work, log energy, and
> build departure events. Nothing there depends on the policy.

---

## Segment 9 — Registering it: three edits  ⏱ 21:00–24:00

**Narration:**
> A new file isn't enough — the simulator has to be told the policy exists. Three edits, and
> forgetting any one of them fails in its own distinctive way.

**1.** `src/Configuration.h` — add the include:
```cpp
#include "PSJF.h"
```

**2.** `src/Configuration.cpp`, in `SchedulerFactory` — map the config keyword to the class:
```cpp
if (type == "PSJF") {
    obj = new PSJF();
    return obj;
}
```

**3.** `testprojects/DEStest/DEStest.vcxproj` — add the files to the build:
```xml
<ClCompile Include="..\..\src\PSJF.cpp" />
<ClInclude Include="..\..\src\PSJF.h" />
```

**⚠ On-screen callout — how each mistake shows up:**

| Forgotten | Symptom |
|---|---|
| the include | compile error in `Configuration.cpp` |
| the factory branch | **builds fine, then crashes at run time** — the factory returns NULL |
| the `.vcxproj` entry | linker error: unresolved `PSJF::PSJF` |

**Narration:**
> The middle one is the nasty one. The factory returns a null pointer for an unknown scheduler
> name, and you find out when the simulator dereferences it. If a brand-new policy crashes
> immediately, check the factory before you debug anything else.

---

## Segment 10 — Build and smoke test  ⏱ 24:00–25:30

```bat
MSBuild testprojects\DEStest\DEStest.vcxproj /p:Configuration=Release /p:Platform=Win32
```

**Screen:** `PSJF.cpp` compiling, then set `SCHEDULER PSJF` and run a short job.

**Narration:**
> Before trusting any numbers, check the error log in the output folder. The simulator
> self-checks aggressively — illegal departures, jobs finishing early, a job running while
> not marked as running. On a correct policy that file stays quiet.

---

## Segment 11 — Evaluating against existing policies  ⏱ 25:30–29:30

**Screen:** three config folders, `SJF` / `PSJF` / `SRPT`, all at load 0.8, with the same two
probe sweeps used in Part 1:

```
PROBES		100		2		1		25		coarse sweep
PROBES		100		0.45		0.05		0.5		fine, for small jobs
```

**Narration:**
> Same machinery as Part 1. And the fine sizes matter here more than anywhere — they're what make
> the preemptive/non-preemptive difference visible at all.
>
> Before trusting any of it, check `errorLog.txt`. The simulator self-checks aggressively; on a
> correct policy that file stays quiet. Ours did.

**▶ RUN — cut away.**

**Screen:** `figures/fig4_part2_PSJF_comparison.png`

**Real results** (mean slowdown E[T(x)]/x, load 0.8; full table in
`figures/fig4_part2_PSJF_comparison.csv`):

| x | SJF (non-preemptive) | PSJF (preemptive) | SRPT |
|---|---|---|---|
| 0.05 | **21.3** | **1.005** (best) | 1.01 |
| 1 | 2.49 | 1.36 (best) | 1.46 |
| 7 | 4.69 | 8.97 (worst) | 6.75 |
| 25 | 2.09 | 6.69 (worst) | 5.91 |

**Narration:**
> Here is why PSJF was worth building. Three policies, one design change between each pair.
>
> Look first at size 0.05. SJF, which is non-preemptive, gives it a slowdown of twenty-one: a
> tiny job that arrives while something is already running has to wait for that job to finish,
> however long it is. PSJF, which differs from SJF in **nothing but preemption**, gives the same
> job a slowdown of 1.005 — it essentially never waits. That gap is the non-preemptive penalty,
> isolated, because everything else about the two policies is identical.
>
> Now look at the large sizes. PSJF is the worst of the three, because a long job's *original*
> size never shrinks, so it gets preempted over and over and never climbs the queue. SRPT escapes
> exactly that trap: as it serves a job the remaining size falls, so the job eventually wins
> priority and finishes.
>
> So each gap in this figure is attributable to exactly one decision — which is precisely how you'd
> want to evaluate a policy you've just invented. And that is the whole workflow: one comparator,
> one arrival handler, three registration edits, and then the same evaluation machinery from part
> one applies to your policy for free.

---

## Segment 12 — Wrap  ⏱ 29:30–30:30

**Narration:**
> If you want to go further, the same recipe covers everything in `src`: FSP, FEST, the
> size-estimate policies. Read `SRPT.cpp` for a preemptive template and `SJF.cpp` for a
> non-preemptive one, and change the comparator.

---

## Appendix — the complete change set

| File | Change |
|---|---|
| `src/PSJF.h` | **new** — class declaration + `CompareJobPSJF` |
| `src/PSJF.cpp` | **new** — implementation |
| `src/Configuration.h` | +1 line: `#include "PSJF.h"` |
| `src/Configuration.cpp` | +5 lines: factory branch in `SchedulerFactory` |
| `testprojects/DEStest/DEStest.vcxproj` | +2 lines: `ClCompile` and `ClInclude` |

Working source for both new files is in `source/` beside this script.
