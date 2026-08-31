#!/usr/bin/env python
"""Builds the self-playing S3 demo walkthrough (HTML, fully self-contained)."""
import os, json, base64, html

FIG = r"G:\My Drive\Research\Projects\S3Update\Claude Output\SimulatorDemo\figures"
OUT = r"G:\My Drive\Research\Projects\S3Update\Claude Output\SimulatorDemo\S3-simulator-walkthrough.html"

def img(name):
    p = os.path.join(FIG, name)
    if not os.path.isfile(p):
        return None
    with open(p, "rb") as f:
        return "data:image/png;base64," + base64.b64encode(f.read()).decode()

def code(t):    return {"kind": "code", "text": t}
def table(h, r):return {"kind": "table", "head": h, "rows": r}
def note(t):    return {"kind": "note", "text": t}
def diagram(svg, cap): return {"kind": "diagram", "svg": svg, "cap": cap}

# --- S3 class diagram -------------------------------------------------------
# Verified against src/: 14 Scheduler subclasses, 7 SpeedScaler, 8 WorkloadGenerator,
# 1 PowerFunction, 2 DESLogger. The Scheduler column is accented - it is the
# extension point Part 2 uses.
def _col(x, title, items, accent=False):
    stroke = "var(--accent)" if accent else "var(--line)"
    tw     = "var(--accent)" if accent else "var(--ink)"
    out = [f'<rect x="{x}" y="196" width="154" height="38" rx="6" fill="var(--panel)" '
           f'stroke="{stroke}" stroke-width="{2 if accent else 1}"/>',
           f'<text x="{x+77}" y="214" text-anchor="middle" font-size="12.5" font-weight="600" '
           f'fill="{tw}" font-family="IBM Plex Sans,sans-serif">{title}</text>',
           f'<text x="{x+77}" y="227" text-anchor="middle" font-size="9" fill="var(--dim)" '
           f'font-family="IBM Plex Mono,monospace">abstract</text>',
           f'<line x1="{x+77}" y1="234" x2="{x+77}" y2="252" stroke="{stroke}" stroke-width="1"/>']
    y = 268
    for it in items:
        out.append(f'<rect x="{x}" y="{y-13}" width="154" height="21" rx="4" fill="var(--codebg)" '
                   f'stroke="{stroke}" stroke-opacity="{0.55 if accent else 0.35}"/>')
        out.append(f'<text x="{x+77}" y="{y+1}" text-anchor="middle" font-size="10.5" '
                   f'fill="var(--ink)" font-family="IBM Plex Mono,monospace">{it}</text>')
        y += 26
    return "".join(out)

CLASS_SVG = (
 '<svg viewBox="0 0 900 370" width="100%" role="img" '
 'aria-label="S3 class diagram: ProbeSetting drives DES, which is composed of five '
 'pluggable abstract components — Scheduler, SpeedScaler, WorkloadGenerator, '
 'PowerFunction and DESLogger — each with concrete subclasses.">'
 # top row
 '<rect x="40" y="14" width="190" height="40" rx="6" fill="var(--panel)" stroke="var(--line)"/>'
 '<text x="135" y="33" text-anchor="middle" font-size="12.5" font-weight="600" fill="var(--ink)" font-family="IBM Plex Sans,sans-serif">ProbeSetting</text>'
 '<text x="135" y="46" text-anchor="middle" font-size="9.5" fill="var(--dim)" font-family="IBM Plex Mono,monospace">run() / runLoad()</text>'
 '<rect x="330" y="14" width="190" height="40" rx="6" fill="var(--panel)" stroke="var(--line)"/>'
 '<text x="425" y="33" text-anchor="middle" font-size="12.5" font-weight="600" fill="var(--ink)" font-family="IBM Plex Sans,sans-serif">Configuration</text>'
 '<text x="425" y="46" text-anchor="middle" font-size="9.5" fill="var(--dim)" font-family="IBM Plex Mono,monospace">factories, config.txt</text>'
 '<rect x="620" y="14" width="240" height="40" rx="6" fill="var(--panel)" stroke="var(--line)"/>'
 '<text x="740" y="33" text-anchor="middle" font-size="12.5" font-weight="600" fill="var(--ink)" font-family="IBM Plex Sans,sans-serif">Job&#160;&#183;&#160;Event</text>'
 '<text x="740" y="46" text-anchor="middle" font-size="9.5" fill="var(--dim)" font-family="IBM Plex Mono,monospace">Arrival / Departure / SpeedChange</text>'
 # connectors into DES
 '<line x1="135" y1="54" x2="135" y2="96" stroke="var(--line)"/>'
 '<line x1="425" y1="54" x2="425" y2="96" stroke="var(--line)"/>'
 '<line x1="740" y1="54" x2="740" y2="96" stroke="var(--line)"/>'
 # DES bar
 '<rect x="40" y="96" width="820" height="46" rx="7" fill="var(--accent-soft)" stroke="var(--accent)" stroke-width="1.5"/>'
 '<text x="450" y="115" text-anchor="middle" font-size="14" font-weight="600" fill="var(--ink)" font-family="IBM Plex Sans,sans-serif">DES &#8212; discrete-event core</text>'
 '<text x="450" y="131" text-anchor="middle" font-size="10" fill="var(--dim)" font-family="IBM Plex Mono,monospace">event loop &#183; owns the five components below</text>'
 # fan-out
 '<line x1="117" y1="142" x2="117" y2="196" stroke="var(--accent)" stroke-width="1.5"/>'
 '<line x1="283" y1="142" x2="283" y2="196" stroke="var(--line)"/>'
 '<line x1="449" y1="142" x2="449" y2="196" stroke="var(--line)"/>'
 '<line x1="615" y1="142" x2="615" y2="196" stroke="var(--line)"/>'
 '<line x1="781" y1="142" x2="781" y2="196" stroke="var(--line)"/>'
 + _col(40,  "Scheduler",   ["FCFS &#183; PS &#183; SRPT", "SJF &#183; PSJF", "FSP &#183; FEST &#183; SEH", "+ 6 more"], accent=True)
 + _col(206, "SpeedScaler", ["SingleSpeed", "CoupledSpeed", "ShadowSpeed", "+ 4 more"])
 + _col(372, "WorkloadGen", ["Exponential", "Pareto &#183; Weibull", "File &#183; Custom", "+ 3 more"])
 + _col(538, "PowerFunction", ["AlphaPowerFunction"])
 + _col(704, "DESLogger",  ["BasicLogger", "RoundZeroLogger"])
 + '</svg>')
def figure(n, c):
    d = img(n)
    return {"kind": "figure", "src": d, "cap": c} if d else note(
        "Figure pending: " + c)

P1 = [
 dict(t="What S3 is", body=[
   note("A discrete-event simulator for single-server queues, built to study "
        "scheduling and speed scaling <em>together</em>."),
   note('<p class="cite">The simulator and its design are described in '
        '<em>Simulation Modeling for Speed Scaling Designs</em>, '
        'Maryam Elahi and Carey Williamson, VALUETOOLS 2016.</p>'),
   note("S3 is written in <strong>C++</strong> with an extensible architecture. Five kinds of "
        "component are interchangeable, and you can implement your own of any of them:"),
   table(["Component", "Decides"], [
     ["Scheduler", "which job the server works on next"],
     ["SpeedScaler", "how fast the server runs"],
     ["PowerFunction", "what that speed costs in energy"],
     ["WorkloadGenerator", "how jobs arrive and how big they are"],
     ["DESLogger", "what gets measured and written out"]]),
   note("In this walkthrough we use the <strong>default implementations</strong> throughout — "
        "the classic scheduling policies, and the classic Exponential workload generator. "
        "Part 2 shows how to write a new component of your own, and includes the class diagram."),
   table(["You need", ""], [["OS", "Windows"], ["Compiler", "Visual Studio 2022"],
                            ["Dependencies", "none"]])],
   n="S3 is a discrete-event simulator for single-server queues. It was built to study two "
     "things that are usually studied apart: which job the server picks next, and how fast "
     "the server runs. The simulator and its design are described in Simulation Modeling for "
     "Speed Scaling Designs, by Elahi and Williamson, at VALUETOOLS 2016. "
     "It's written in C plus plus with an extensible architecture: schedulers, speed scalers, "
     "power functions, workload generators and loggers are each an interchangeable component, "
     "and you can implement your own of any of them. For this walkthrough we'll use the default "
     "implementations throughout — the classic scheduling policies, and the classic exponential "
     "workload generator. Part 2 shows how to write a new one. So: we install it, run three "
     "classic policies, and produce two kinds of graph."),

 dict(t="Getting the code", body=[
   code("git clone https://github.com/bmelahi/SpeedScalingSimulator.git\ncd SpeedScalingSimulator"),
   table(["Directory", "Contents"], [
     ["src/", "every scheduler, speed scaler, workload generator"],
     ["testprojects/DEStest/", "the program you actually run"],
     ["data/outputs/", "results — one self-describing folder per run"]])],
   n="Clone the repository. The layout is simple. src holds every scheduler, speed scaler and "
     "workload generator. testprojects slash DEStest is the program you run. And data slash "
     "outputs is where results land, one folder per run."),

 dict(t="Building", body=[
   code('MSBuild testprojects\\DEStest\\DEStest.vcxproj ^\n'
        '   /p:Configuration=Release /p:Platform=Win32'),
   note("Or open <code>S3Sim22.sln</code> in Visual Studio and build "
        "<strong>Release / Win32</strong>."),
   note("Build <strong>Release</strong>, not Debug — Debug is roughly three times slower.")],
   n="Build in Release, not Debug. Debug is about three times slower, and these runs are long "
     "enough already. The executable lands in testprojects, DEStest, Bin, Release."),

 dict(t="Structure of config.txt", body=[
   code("PATH          ..\\..\\data\\\n"
        "SCHEDULER     FCFS\n"
        "SPEEDSCALER   SingleSpeed   1     1\n"
        "POWERFUNCTION ALPHA         2\n"
        "WORKLOAD      EXPONENTIAL   100000   0.80   1\n"
        "LOGGER        BasicLogger   CONCISE  PROMTWRITE\n"
        "PROBES        300   2   1   50"),
   note("<strong>Warning: no blank lines allowed in this config.txt file.</strong> Make "
        "sure there is no blank line in the middle of the experiment description — the "
        "parser stops at the blank line.")],
   n="The whole experiment is described by seven lines. SCHEDULER picks the policy. "
     "SPEEDSCALER decides how fast the server runs — SingleSpeed one one means constant "
     "speed, an ordinary fixed-speed queue. With mean job size one, the arrival rate is the "
     "load, so zero point eight here means a load of zero point eight. And one warning: no "
     "blank lines allowed in this config dot txt file. Make sure there is no blank line in "
     "the middle of the experiment description — the parser stops at the blank line."),

 dict(t="Example: average response time for three policies at fixed speed", body=[
   code("SCHEDULER     FCFS          # then PS, then SRPT\n"
        "SPEEDSCALER   SingleSpeed   1     1\n"
        "WORKLOAD      EXPONENTIAL   100000   0.80   1\n"
        "PROBES        0     2     1     1"),
   note("<strong>For now we set the probe count to 0.</strong> We come back to what probes "
        "are, and what we use them for, later in this walkthrough."),
   note("With probes off, the run simulates the workload once and reports the "
        "<strong>averages and variances</strong> over that run:"),
   table(["", "Quantity", "Averaged over"], [
     ["E[N]", "average number of jobs in system", "time"],
     ["E[S]", "average execution speed", "time"],
     ["E[B]", "average bytes in system", "time"],
     ["E[T]", "average response time", "per job"],
     ["E[Busy]", "average busy-period length", "per busy period"]]),
   note("The three time averages come from the profile logs the run writes — "
        "<code>jobcountprofile.txt</code>, <code>speedprofile.txt</code> and "
        "<code>byteprofile.txt</code>, each a timestamped series. <code>E[T]</code> is "
        "summarised in <code>meanzlog.txt</code>."),
   table(["Policy", "E[T]"], [["FCFS", "5.264"], ["PS", "5.251"], ["SRPT", "2.432"]]),
   note("FCFS and PS agree closely; SRPT is roughly half. Theory predicts a specific number "
        "for FCFS and PS — the next segment checks it.")],
   n="For now we set the probe count to zero — we'll come back to what probes are and what we "
     "use them for later. With probes off, the run simulates the workload once and reports the "
     "averages and variances over that run. Three of them are time averages: E of N, the "
     "average number of jobs in the system; E of S, the average execution speed; and E of B, "
     "the average bytes in the system. Those come from the profile logs the run writes — job "
     "count, speed and bytes, each a timestamped series. Then E of T, the average response "
     "time, which is per job rather than per unit time; and E of Busy, the average busy period "
     "length, which is per busy period. "
     "FCFS and PS both land near five point two five. SRPT at two point four three, roughly "
     "half — the classic result that SRPT is optimal for mean response time."),

 dict(t="Theory moment: checking the measured E[T] against M/M/1 and M/G/1", body=[
   code("Assume  ρ = λ·E[X] < 1.\n\n"
        "Little's Law            E[N] = λ·E[T]\n"
        "M/M/1 FCFS, M/G/1 PS    E[N] = ρ / (1 - ρ)\n"
        "                        ――――――――――――――――――――\n"
        "                        E[T] = E[X] / (1 - ρ)\n\n"
        "Here  λ = 0.8,  E[X] = 1   →   E[T] = 1 / 0.2 = 5"),
   note("We measured <strong>5.264</strong> (FCFS) and <strong>5.251</strong> (PS). "
        "Where does the extra 0.25 come from?"),
   note("<strong>It is an artifact of the random number generator in this experiment</strong> — "
        "of the particular sample path this run happened to draw. It is not a defect in the "
        "simulator and not an error in the theory: run it again with a different seed and you "
        "get a different number, scattered around 5. Two things are going on."),
   note("<strong>One — the draw came out heavy.</strong> The realized mean job size wasn't "
        "exactly 1; it came out at <strong>1.00471</strong>, 0.47% high. Since "
        "ρ = λ·E[X], that lifts the realized load to <strong>0.8038</strong>, and "
        "E[X]/(1−ρ) to <strong>5.122</strong>. That closes about half the gap."),
   table(["Quantity", "Nominal", "Realized"], [
     ["E[X]", "1.00000", "1.00471"],
     ["λ", "0.80000", "0.80007"],
     ["ρ = λ·E[X]", "0.80000", "0.80384"],
     ["E[X]/(1−ρ)", "5.000", "5.122"]]),
   note("<strong>Two — the same draw scatters E[T] itself, far more than the job count "
        "suggests.</strong> Response times in a queue are strongly autocorrelated: consecutive "
        "jobs share the same backlog. Measured lag-1 autocorrelation here is <strong>0.966</strong>."),
   table(["Standard error of E[T]", "Value", ""], [
     ["assuming independence", "0.017", "wrong — ignores autocorrelation"],
     ["batch means (50 batches)", "0.149", "honest"]]),
   note("So the 95% interval is roughly <strong>[4.97, 5.56]</strong> — which contains both "
        "5.000 and 5.122. The discrepancy is <strong>not statistically significant</strong>. "
        "With ρ near 1 the curve is steep: dE[T]/dρ = E[X]/(1−ρ)² = 25 here, "
        "so a 0.004 error in ρ alone moves E[T] by 0.1.")],
   n="Let's check that against theory. Assume rho, the load, is lambda times mean job size, and "
     "less than one. Little's Law says expected number in system equals lambda times expected "
     "response time. And for M/M/1 under FCFS, and M/G/1 under processor sharing, expected "
     "number in system is rho over one minus rho. Put those together and expected response time "
     "is mean job size over one minus rho. Here lambda is zero point eight and mean job size is "
     "one, so theory says exactly five. We measured five point two six. "
     "That discrepancy is an artifact of the random number generator in this experiment — of the "
     "particular sample path this run happened to draw. It is not a defect in the simulator and "
     "not an error in the theory: run it again with a different seed and you get a different "
     "number, scattered around five. Two things are going on. First, the draw came out heavy: "
     "the realized mean job size was one point zero zero four seven, about half a percent high, "
     "which lifts realized load to zero point eight zero four and the prediction to five point "
     "one two. That closes about half the gap. Second, the same draw scatters the response time "
     "average itself, far more than a hundred thousand jobs suggests, because response times in "
     "a queue are autocorrelated at zero point nine seven. Batch means put the standard error at "
     "zero point one five, not the zero point zero one seven you'd get assuming independence. "
     "The ninety-five percent interval covers both five and five point one two, so the "
     "difference isn't significant."),

 dict(t="Sweeping the load at fixed speed", body=[
   note("Running one load at a time gets tedious. There is a built-in sweep, <code>runLoad()</code>, but it ships switched off. <strong>One edit turns it on</strong>, in <code>testprojects/DEStest/DEStest.cpp</code> — flip the comment toggle:"),
   code('/*        →     //*\n// run the probesimulator for load range\nif (probesim.runLoad()) {'),
   note("That is the only edit for now. The sweep range in <code>src/ProbeSetting.cpp</code> ships as:"),
   code('for (double i = 0.05; i <= 0.96; i += 0.05)\n//  19 loads: 0.05, 0.10, ... 0.95   -- the shipped range'),
   note("It stops below 1 on purpose: at fixed speed ρ = 1 is the stability boundary. Rebuild, then run the same three configs with <em>nothing else changed</em> — still <code>SingleSpeed 1 1</code>, still <code>PROBES 0</code>."),],
   n="Running one load at a time gets tedious. The simulator has a built-in load sweep, runLoad, but it ships switched off. Turning it on is one edit: a comment toggle in DEStest dot cpp decides whether runLoad or the ordinary single run gets compiled. Flip it and rebuild. The sweep range itself is hard-coded in ProbeSetting dot cpp, and the shipped range is nineteen loads from zero point zero five to zero point nine five. It stops below one on purpose — at fixed speed, load one is the stability boundary. We leave the range alone and rerun the same three configurations, still single speed, nothing else changed."),

 dict(t="The fixed-speed load graph", body=[
   figure("fig_loadsweep_singlespeed.png", "19 load points, 0.05 → 0.95, fixed speed"),
   table(["ρ", "FCFS", "PS", "SRPT", "E[X]/(1−ρ)"], [["0.50", "2.030", "2.031", "1.440", "2.000"], ["0.80", "5.264", "5.251", "2.432", "5.000"], ["0.90", "10.594", "10.548", "3.703", "10.000"], ["0.95", "22.502", "22.463", "6.035", "20.000"]])],
   n="Three things to read off this. First, the dotted line is the theory we just derived, E of X over one minus rho, and FCFS and PS sit right on it across the whole range — so the check we did at load zero point eight was not a coincidence. Second, the curve is a wall: E of T is about two at load zero point five, five at zero point eight, and twenty-two at zero point nine five. That blow-up as rho approaches one is the whole reason speed scaling exists. Third, FCFS and PS lie exactly on top of each other. That is not a plotting mistake — for exponentially distributed job sizes those two policies genuinely have the same mean response time. SRPT is well below both."),

 dict(t="Theory moment: stability, and how fast to run", body=[
   note("<strong>The speed is a free choice.</strong> S3 will run any speed function you can implement. But for a run to mean anything the system has to be <strong>stable</strong> — the speed must meet the workload demand."),
   code('fixed speed  s = 1        stable  ⇔  ρ = λ·E[X] < 1\nvariable     s(t)         you must ensure it keeps up with the load'),
   note("<strong>So how fast?</strong> Running faster lowers E[T] but costs energy. A robust policy adjusts dynamically and spends extra joules only in proportion to the response time they buy — one joule per unit of mean response time removed. That is the objective:"),
   code('minimize   z = E[T] + E[e]\n\n  E[T]   mean response time per job\n  E[e]   mean energy per job\n\nS3 reports all three: meanzlog.txt has columns  E[T]  E[e]  Z  N'),
   note("<strong>CoupledSpeed’s answer:</strong> set the speed to the inverse power function of the queue length."),
   code('P(s)  = s^α                     the power function, α = 2\ns(N)  = P⁻¹(N) · base  =  N^(1/α)      speed at queue length N\n      = √N                       here, with base = 1\ns(0)  = 0                       empty system, speed gated to zero'),
   note("A consequence worth knowing: the power drawn is then P(P⁻¹(N)) = N, so energy accrues at exactly the rate jobs accumulate — and by Little’s Law the energy per job equals the response time per job. The runs confirm it: <strong>E[e] = E[T] to six decimals</strong> for all three policies, so here z = 2·E[T]."),],
   n="Before we look at any speed-scaling results, one question: what speed should the server run at? The speed is a free choice — S3 will run any speed function you can implement. But for a run to mean anything the system has to be stable, which means the speed has to meet the workload demand. At a single fixed speed of one, that is just load less than one. If you make the speed variable, it is on you to make sure it keeps up with the load. Now, how fast? Running faster lowers response time but costs energy, so a robust policy adjusts dynamically and spends extra joules only in proportion to the response time they buy — one joule per unit of mean response time removed. That is the objective z, equal to E of T plus E of e, and the simulator reports all three in meanzlog dot txt. CoupledSpeed’s answer is to set the speed to the inverse power function of the queue length, so with alpha two the speed at any moment is the square root of the number of jobs in system. One consequence is worth knowing: the power drawn is then exactly N, so energy accrues at the rate jobs accumulate, and by Little’s Law energy per job comes out equal to response time per job. The measurements confirm it to six decimals, so here z is simply twice E of T."),

 dict(t="Turning on speed scaling", body=[
   code('SPEEDSCALER   CoupledSpeed  INV   1\nPOWERFUNCTION ALPHA         2\nWORKLOAD      EXPONENTIAL   100000   1.00   1'),
   note("Two lines change from the fixed-speed config: the speed scaler, and the load — now <strong>1.00</strong>, exactly the point where the fixed-speed system blew up."),
   table(["Policy", "E[T]", "E[e]", "z = E[T]+E[e]"], [["FCFS", "1.538", "1.538", "3.076"], ["PS", "1.537", "1.537", "3.074"], ["SRPT", "1.341", "1.341", "2.681"]]),
   figure("fig1_ET_two_settings.png", "Same three policies, two speed settings"),],
   n="Now we run it. Two lines change from the fixed-speed config — the speed scaler, and the load, which is now one point zero. That is exactly the point where the fixed-speed system blew up: at a single speed, load one is the stability boundary and mean response time is infinite. With speed scaling it stays perfectly stable, and response times are actually lower than the fixed-speed run at load zero point eight. You pay for that in energy — and there is the identity from the previous slide, E of e equal to E of T in every row."),

 dict(t="Sweeping past load 1", body=[
   note("The <code>runLoad()</code> toggle is already flipped from the fixed-speed sweep. This time <strong>one more edit</strong>: the range in <code>src/ProbeSetting.cpp</code>, so the sweep goes past 1:"),
   code('for (double i = 0.05; i <= 0.96; i += 0.05)   // shipped\nfor (double i = 0.1;  i <= 2.01; i += 0.1)    // ours'),
   note("Speed scaling removes the ρ = 1 stability boundary, so there is now something to see above it. Rebuild and rerun."),],
   n="Now we can ask the question the fixed-speed sweep could not. The runLoad toggle is already flipped, so this time there is just one more edit: the sweep range, which is hard-coded in ProbeSetting dot cpp. The shipped range stopped just below one because that was the stability limit. Speed scaling removes that limit, so we extend the range to two and rebuild."),

 dict(t="The speed-scaling load graph", body=[
   figure("fig2_loadsweep_speedscaling.png", "20 load points, 0.1 → 2.0, under α=2 speed scaling")],
   n="Here is the payoff, and it is worth putting side by side with the fixed-speed graph. "
     "Twenty load points from zero point one to two, three policies, under "
     "speed scaling. The dotted line is load one, where a fixed-speed system would have blown "
     "up. Nothing happens there at all — the curves continue smoothly. SRPT stays lowest "
     "throughout. And FCFS and PS lie exactly on top of each other. That's not a plotting "
     "mistake: for exponentially distributed job sizes those two policies genuinely have the "
     "same mean response time."),

 dict(t="Probes and slowdown", body=[
   code('PROBES        100    2      1        25      coarse sweep\nPROBES        100    0.45   0.05     0.5     fine, for small jobs\n#             rounds step   smallest largest'),
   note("Insert a probe job of a known size at 100 fixed moments; each probe goes into an otherwise <em>identical</em> run. Expensive: <strong>1,500 full simulations per policy</strong> ""for the 15 sizes here."),
   note("<strong>Two fine sizes, 0.05 and 0.5, are added on purpose.</strong> The unfairness of a non-preemptive policy to small jobs is a known theoretical result, and with the coarse sweep starting at x = 1 you simply cannot see it. These two sizes are the check that it shows up in measurement."),
   table(["x", "FCFS", "PS", "SRPT"], [["0.05", "105.9", "6.19", "1.01"], ["0.5", "11.5", "6.13", "1.21"], ["1", "6.25", "6.13", "1.47"], ["25", "1.21", "5.49", "5.91"]]),
   figure("fig3_slowdown_vs_size.png", "Mean slowdown E[T(x)]/x by job size, load 0.8"),],
   n="The last thing is a different question. Not how the system does on average, but how it treats a job of a particular size. Probes answer that: insert a job of a known size at a hundred fixed moments, into an otherwise identical run, and slowdown is its response time divided by its size. Notice the two fine sizes, zero point zero five and zero point five. They are there on purpose. The unfairness of a non-preemptive policy to small jobs is a known theoretical result, and with the coarse sweep starting at size one you simply cannot see it — so these two sizes are the check that it really shows up in measurement. And it does, dramatically. PS is the flat reference: every size gets the same slowdown, which is why it is the usual definition of fair. FCFS is non-preemptive, and a tiny job stuck behind a long one waits just as long as anyone else — at size zero point zero five its slowdown is a hundred and six, against PS at six. SRPT is the mirror image: excellent for small jobs, and the large ones pay for it."),
   
 dict(t="A caveat on the probe level", body=[
   note("PS should sit at exactly 1/(1−ρ) = 5. It measures <strong>5.4 to 6.2</strong>, and the curve is smooth rather than noisy."),
   note("That is not a simulator error. <strong>Every size shares the same 100 probe instants</strong> — and this particular sample of instants was busier than average. The offset is common to all sizes and all policies, which is why the curve is smooth, and why it <em>cancels</em> when you compare policies at the same x. Shared instants are the point of the design: FCFS at x = 0.05 and SRPT at x = 0.05 saw the identical system."),],
   n="One caveat, because it will be the first thing a careful viewer asks. PS should sit at exactly one over one minus rho, which is five. It measures between five point four and six point two, and the curve is smooth rather than noisy. That is not a simulator error. Every size shares the same hundred probe instants, and this particular sample of instants happened to be busier than average. The offset is common to every size and every policy, which is why the curve is smooth instead of scattered, and why it cancels when you compare two policies at the same size. That sharing is the point of the design: FCFS at size zero point zero five and SRPT at size zero point zero five saw the identical system."),
 dict(t="Part 1 wrap", body=[
   table(["What you saw", "How"], [["one experiment", "seven config lines"], ["the averages", "meanzlog.txt: E[T]  E[e]  Z  N"], ["a range of loads", "runLoad(), one comment toggle"], ["per-job-size behaviour", "PROBES, one run per probe"]]),
   note("Output folders are self-describing — policy, speed scaler, power function, workload and timestamp are all in the folder name, so results never get confused with each other."),],
   n="To recap: seven config lines describe an experiment. Meanzlog dot txt gives you the averages. runLoad sweeps a range of loads for one comment toggle. And probes give you per-job-size behaviour. Output folders are self-describing, so results never get confused with each other. In part two we add an entirely new scheduling policy — about sixty lines of real code — and evaluate it against the ones we just ran."),
]

P2 = [
 dict(t="The policies we already have", body=[
   note("<strong>SJF — Shortest Job First.</strong> Always start the smallest job in the "
        "queue, measured by its <em>original</em> size. Once a job starts it runs to "
        "completion: a small job arriving mid-service must wait its turn. "
        "<em>Non-preemptive.</em>"),
   note("<strong>SRPT — Shortest Remaining Processing Time.</strong> Always run the job with "
        "the least work <em>left</em>. The decision is re-made whenever a job arrives, so a "
        "small newcomer can interrupt. As a job is served its remaining size shrinks, so it "
        "steadily climbs the order. <em>Preemptive.</em>"),
   note("Both ship with the simulator. Today we add a third.")],
   n="The simulator already has two size-based policies. SJF, shortest job first, always "
     "starts the smallest job in the queue measured by its original size — and once a job "
     "starts, it runs to completion, so a small job arriving mid-service has to wait. SRPT, "
     "shortest remaining processing time, always runs the job with the least work left, and "
     "re-decides whenever something arrives, so a small newcomer can interrupt. And because a "
     "job's remaining size shrinks as it's served, it steadily climbs the order."),

 dict(t="The policy we're adding", body=[
   note("<strong>PSJF — Preemptive Shortest Job First.</strong> Run the smallest job by "
        "<em>original</em> size, and re-decide at every arrival. It preempts like SRPT, but "
        "sorts like SJF."),
   note("Crucially, a job's priority under PSJF <strong>never improves as it is served</strong> "
        "— its original size doesn't change. A long job can be preempted again and again and "
        "never work its way up.")],
   n="The policy we're adding is preemptive shortest job first. Run the smallest job by "
     "original size, and re-decide at every arrival. So it preempts like SRPT, but sorts like "
     "SJF. And here's the consequence worth holding on to: under PSJF a job's priority never "
     "improves as it's served, because its original size doesn't change. A long job can be "
     "preempted again and again and never work its way up. Keep that in mind — you'll see it "
     "in the results at the end."),

 dict(t="Two decisions, three policies", body=[
   table(["Policy", "Sorts on", "Can switch", "Preemptive?"], [
     ["SJF", "original size", "only at completion", "no"],
     ["PSJF", "original size", "at every arrival", "yes"],
     ["SRPT", "remaining size", "at every arrival", "yes"]]),
   note("Every scheduling policy of this family answers two independent questions: "
        "<strong>what do you sort on</strong>, and <strong>when are you allowed to switch</strong>."),
   note("Read down the table: SJF → PSJF changes only the switching rule. PSJF → SRPT changes "
        "only the sort key. So any difference we measure is attributable to exactly one "
        "decision — which is what makes this a genuinely useful comparison rather than three "
        "unrelated curves.")],
   n="Laid out side by side, the three policies differ along just two axes: what you sort on, "
     "and when you're allowed to switch. Going from SJF to PSJF changes only the switching "
     "rule — same sort key, but now you can preempt. Going from PSJF to SRPT changes only the "
     "sort key — still preemptive, but now on remaining size instead of original size. That "
     "means any difference we measure is attributable to exactly one decision at a time, which "
     "is what makes this a real comparison rather than three unrelated curves."),

 dict(t="How the simulator is put together", body=[
   diagram(CLASS_SVG,
     "S3 architecture — DES owns five pluggable components. Scheduler (accented) is the one "
     "we extend today."),
   note("<code>DES</code> runs the event loop and owns five components, each an abstract base "
        "class with interchangeable implementations. <code>Configuration</code> reads "
        "<code>config.txt</code> and builds them through factory methods, so a run is assembled "
        "at start-up from text."),
   note("This is what makes the simulator extensible in <strong>two independent directions</strong>: "
        "the <em>policies</em> — Scheduler and SpeedScaler — and the <em>instrumentation</em> — "
        "DESLogger and the probe machinery. Adding to either does not touch the other."),
   note("Referenced in Part 1: <em>Simulation Modeling for Speed Scaling Designs</em>, "
        "Elahi &amp; Williamson, VALUETOOLS 2016.")],
   n="Before writing code, here's how the simulator is put together. DES runs the event loop, "
     "and owns five components — Scheduler, SpeedScaler, WorkloadGenerator, PowerFunction and "
     "DESLogger. Each is an abstract base class with interchangeable implementations, and "
     "Configuration builds them by factory from the config file, so an entire run is assembled "
     "at start-up from seven lines of text. That's what makes the simulator extensible in two "
     "independent directions: the policies, and the instrumentation. Adding to one doesn't "
     "touch the other. Today we're extending the accented column on the left — Scheduler — "
     "which already has fourteen implementations."),

 dict(t="The Scheduler interface", body=[
   note("Every policy derives from <code>Scheduler</code>, and the simulator talks to it only through these nine methods:"),
   table(["Method", "What it does"], [["toString", "name and parameters of the object"], ["arrival_handler", "a job arrives; true if the running job changed"], ["departure_handler", "a job completes; returns the finished Job"], ["speedchange_handler", "the speed scaler changed speed"], ["bonusevent_handler", "a policy-defined event fires"], ["getExJob", "which job is executing right now"], ["nextDeparture", "when the running job finishes at this speed"], ["nextScheduler", "schedule a policy-defined future event"], ["updatePeriod", "apply the passage of time over [t1, t2]"]]),
   note("<strong>How much of this you write yourself depends on the policy.</strong> For a policy that is just an ordering over a priority queue — SRPT, SJF, and the PSJF we are about to write — most of these come out the same and you copy them. For a policy that serves several jobs at once, they do not: PS shares the speed across every job in system, so its <code>updatePeriod</code>, <code>nextDeparture</code> and <code>departure_handler</code> are genuinely different code."),],
   n="Every policy derives from Scheduler, and the simulator only talks to it through a fixed set of methods — these nine. How much of this you end up writing yourself depends on the policy. For a policy that is just an ordering over a priority queue — SRPT, SJF, and the PSJF we are about to write — most of these come out the same and you copy them across. For a policy that serves several jobs at once they do not: PS shares the speed across every job in the system, so its updatePeriod, nextDeparture and departure handler are genuinely different code. Ours is a priority-queue policy, so for us the interesting part is small."),

 dict(t="Where the policy lives", body=[
   code("// SRPT — compares REMAINING size\n"
        "return definitelyGreaterThan(j1->remsize_m, j2->remsize_m);"),
   note("Two traps. A C++ priority queue puts the <em>largest</em> on top — so to get the "
        "smallest job first you return <strong>greater than</strong>."),
   note("Never compare doubles with <code>==</code>. Use the provided "
        "<code>approximatelyEqual</code> and <code>definitelyGreaterThan</code>.")],
   n="The queue is a C plus plus priority queue, and the comparator is what makes one policy "
     "different from another. Two things will bite you. First, the comparison looks backwards: "
     "a priority queue puts the largest element on top, so to get the smallest job first you "
     "return greater-than. Second, never compare doubles with double equals — the codebase "
     "gives you approximatelyEqual and definitelyGreaterThan."),

 dict(t="Writing PSJF.h", body=[
   code("class CompareJobPSJF {\npublic:\n"
        "  bool operator()(const Job * j1, const Job * j2) const {\n"
        "    if (approximatelyEqual(j1->getSize(), j2->getSize())) {\n"
        "      if (j1->isBeingExecuted_m)      return false;\n"
        "      else if (j2->isBeingExecuted_m) return true;\n"
        "      else return definitelyGreaterThan(j1->getArrival(), j2->getArrival());\n"
        "    }\n"
        "    else\n"
        "      return definitelyGreaterThan(j1->getSize(), j2->getSize());\n"
        "  }\n};"),
   note("One character of real difference from SRPT: <code>getSize()</code> instead of "
        "<code>remsize_m</code>.")],
   n="This is the entire policy. One meaningful difference from SRPT: getSize instead of "
     "remsize. Original size, not remaining size. The tie-break matters too — if two jobs are "
     "the same size, prefer the one already running, otherwise the server thrashes, context "
     "switching between equal jobs and making no progress."),

 dict(t="Writing PSJF.cpp", body=[
   code("bool PSJF::arrival_handler(ArrivalEvent * e) {\n"
        "  Job * top = NULL;\n"
        "  if (!jobs_q.empty())  top = jobs_q.top();\n\n"
        "  Job * newjob = new Job(e->job_m);\n"
        "  jobs_q.push(newjob);\n\n"
        "  if (jobs_q.top()->getID() == newjob->getID()) {\n"
        "    jobs_q.top()->isBeingExecuted_m = true;\n"
        "    if (top != NULL) top->isBeingExecuted_m = false;\n"
        "    return true;          // the running job changed\n"
        "  }\n"
        "  else return false;\n}"),
   note("We don't decide anything here — we push, then <em>ask the queue</em> whether the top "
        "changed. The comparator already holds the rule.")],
   n="This is the second and last piece of real policy. Notice we don't decide anything "
     "ourselves — we push the job and then ask the queue whether the top changed. The "
     "comparator already encodes the priority rule. Returning true tells the simulator the "
     "running job changed, so it must recompute the next departure. Get that wrong and you'll "
     "see illegal departure errors in the log."),

 dict(t="Registering it — three edits", body=[
   code('// 1. src/Configuration.h\n#include "PSJF.h"\n\n'
        '// 2. src/Configuration.cpp, in SchedulerFactory\n'
        'if (type == "PSJF") {\n    obj = new PSJF();\n    return obj;\n}\n\n'
        '// 3. testprojects/DEStest/DEStest.vcxproj\n'
        '<ClCompile Include="..\\..\\src\\PSJF.cpp" />\n'
        '<ClInclude Include="..\\..\\src\\PSJF.h" />'),
   table(["Forgotten", "How it fails"], [
     ["the include", "compile error in Configuration.cpp"],
     ["the factory branch", "builds fine, then CRASHES at run time"],
     ["the .vcxproj entry", "linker error: unresolved PSJF::PSJF"]])],
   n="A new file isn't enough — the simulator has to know the policy exists. Three edits, and "
     "each one fails differently if you forget it. The middle one is the nasty one: the "
     "factory returns a null pointer for an unknown scheduler name, so it builds perfectly and "
     "then crashes when the simulator dereferences it. If a brand-new policy crashes "
     "immediately, check the factory before debugging anything else."),

 dict(t="Evaluating the new policy", body=[
   note("Same machinery as Part 1 — three folders, <code>SJF</code> / <code>PSJF</code> / <code>SRPT</code>, load 0.8, and the same two probe sweeps, coarse (1 … 25) plus fine (0.05, 0.5). The fine sizes are what make the preemptive/non-preemptive difference visible at all."),
   note("Check <code>errorLog.txt</code> before trusting numbers. The simulator self-checks aggressively; on a correct policy that file stays quiet. Ours did."),
   table(["x", "SJF (non-pre.)", "PSJF (pre.)", "SRPT"], [["0.05", "21.3", "1.005  (best)", "1.01"], ["1", "2.49", "1.36  (best)", "1.46"], ["7", "4.69", "8.97  (worst)", "6.75"], ["25", "2.09", "6.69  (worst)", "5.91"]]),
   figure("fig4_part2_PSJF_comparison.png", "SJF → PSJF → SRPT: one design change per step"),],
   n="And there is the prediction, confirmed. Look first at size zero point zero five. SJF, which is non-preemptive, gives it a slowdown of twenty-one: a tiny job that arrives while something is already running has to wait for that job to finish, however long it is. PSJF, which differs from SJF in nothing but preemption, gives the same job a slowdown of one point zero zero five — it essentially never waits. That gap is the non-preemptive penalty, isolated, because everything else about the two policies is identical. Now look at the large sizes. PSJF is the worst of the three, because a long job’s original size never shrinks, so it gets preempted over and over and never climbs the queue. SRPT escapes exactly that trap: as it serves a job the remaining size falls, so the job eventually wins priority and finishes. That single difference in the sort key is the whole reason SRPT is the one people use — and we just demonstrated it with sixty lines of our own code."),
]

def dur(seg):  # ~14 chars/sec narration, floor 7s
    return max(7.0, round(len(seg["n"]) / 14.0, 1))

def prep(lst):
    return [dict(t=s["t"], n=s["n"], body=s["body"], d=dur(s)) for s in lst]

data = {"p1": prep(P1), "p2": prep(P2)}
missing = sum(1 for p in data.values() for s in p for b in s["body"]
              if b["kind"] == "note" and b["text"].startswith("Figure pending"))

TPL = """<title>S3 Simulator Walkthrough (Draft)</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:wght@400;500&family=IBM+Plex+Sans:wght@400;500;600&family=IBM+Plex+Serif:wght@500;600&display=swap">
<style>
:root{
  --ground:#eef1f5; --panel:#ffffff; --stage:#f7f9fb;
  --ink:#16202c; --dim:#5c7089; --line:#d3dce5;
  --accent:#b4701c; --accent-soft:#f0e0cb;
  --codebg:#eaeff4; --shadow:0 1px 2px rgba(22,32,44,.07),0 8px 24px rgba(22,32,44,.06);
}
@media (prefers-color-scheme:dark){
  :root:not([data-theme="light"]){
    --ground:#0b111a; --panel:#131c27; --stage:#0f1620;
    --ink:#e4ecf4; --dim:#8ea2b8; --line:#26323f;
    --accent:#e0932f; --accent-soft:#3a2c17;
    --codebg:#0b131c; --shadow:0 1px 2px rgba(0,0,0,.4),0 8px 28px rgba(0,0,0,.35);
  }
}
:root[data-theme="dark"]{
  --ground:#0b111a; --panel:#131c27; --stage:#0f1620;
  --ink:#e4ecf4; --dim:#8ea2b8; --line:#26323f;
  --accent:#e0932f; --accent-soft:#3a2c17;
  --codebg:#0b131c; --shadow:0 1px 2px rgba(0,0,0,.4),0 8px 28px rgba(0,0,0,.35);
}
*{box-sizing:border-box}
body{margin:0;background:var(--ground);color:var(--ink);
  font-family:"IBM Plex Sans",system-ui,-apple-system,Segoe UI,sans-serif;
  line-height:1.55;-webkit-font-smoothing:antialiased}
.wrap{max-width:1020px;margin:0 auto;padding:28px 20px 56px;display:flex;flex-direction:column;gap:18px}
header{display:flex;flex-wrap:wrap;align-items:baseline;gap:10px 16px}
h1{font-family:"IBM Plex Serif",Georgia,serif;font-weight:600;font-size:1.6rem;margin:0;
  letter-spacing:-.01em;text-wrap:balance}
.sub{color:var(--dim);font-size:.9rem}
.draft{font:600 .72rem/1 "IBM Plex Sans",sans-serif;letter-spacing:.09em;padding:5px 9px;border-radius:5px;border:1px solid var(--accent);color:var(--accent);background:transparent;text-transform:uppercase;align-self:center;white-space:nowrap}
.draftbar{padding:11px 16px;border:1px solid var(--line);border-left:3px solid var(--accent);border-radius:7px;background:var(--panel);color:var(--dim);font-size:.87rem;line-height:1.5}
.tabs{display:flex;gap:6px;margin-left:auto}
.tab{font:500 .85rem/1 "IBM Plex Sans",sans-serif;padding:9px 15px;border-radius:7px;
  border:1px solid var(--line);background:var(--panel);color:var(--dim);cursor:pointer}
.tab[aria-selected="true"]{background:var(--accent);border-color:var(--accent);color:#fff}
.tab:focus-visible,button:focus-visible{outline:2px solid var(--accent);outline-offset:2px}

.player{background:var(--panel);border:1px solid var(--line);border-radius:12px;
  box-shadow:var(--shadow);overflow:hidden}
.stage{background:var(--stage);padding:26px 28px;min-height:390px;
  display:flex;flex-direction:column;gap:16px;border-bottom:1px solid var(--line)}
.eyebrow{font:500 .7rem/1 "IBM Plex Mono",monospace;letter-spacing:.13em;
  text-transform:uppercase;color:var(--accent)}
.stitle{font-family:"IBM Plex Serif",Georgia,serif;font-weight:600;font-size:1.5rem;
  margin:0;letter-spacing:-.01em;text-wrap:balance}
.body{display:flex;flex-direction:column;gap:13px}
pre{margin:0;background:var(--codebg);border:1px solid var(--line);border-radius:8px;
  padding:14px 16px;overflow-x:auto;font-family:"IBM Plex Mono",monospace;
  font-size:.8rem;line-height:1.65;color:var(--ink)}
.note{margin:0;font-size:.94rem;color:var(--ink)}
.note code{font-family:"IBM Plex Mono",monospace;font-size:.85em;background:var(--codebg);
  padding:1px 5px;border-radius:4px}
.tblwrap{overflow-x:auto}
table{border-collapse:collapse;font-size:.87rem;width:100%;font-variant-numeric:tabular-nums}
th{text-align:left;font:500 .68rem/1.4 "IBM Plex Mono",monospace;letter-spacing:.08em;
  text-transform:uppercase;color:var(--dim);padding:7px 12px 7px 0;border-bottom:1px solid var(--line)}
td{padding:7px 12px 7px 0;border-bottom:1px solid var(--line)}
tr:last-child td{border-bottom:0}
figure{margin:0}
figure img{width:100%;border:1px solid var(--line);border-radius:8px;display:block;background:#fff}
figcaption{font-size:.79rem;color:var(--dim);margin-top:7px}
figure.diagram svg{width:100%;height:auto;display:block;border:1px solid var(--line);
  border-radius:8px;background:var(--stage);padding:6px 0}
.cite{font-size:.84rem;color:var(--dim);border-left:2px solid var(--accent);
  padding:2px 0 2px 12px;margin:0}
.cite em{color:var(--ink);font-style:normal;font-weight:500}

/* Must NOT be display:flex — that turns each word-span into a flex item on one
   non-wrapping line, and the narration overflows the box. Plain block text wraps. */
.caption{padding:16px 28px;font-size:.95rem;line-height:1.62;color:var(--ink);
  background:var(--panel);border-bottom:1px solid var(--line);
  display:block;min-height:96px;max-height:180px;overflow-y:auto;
  overflow-wrap:break-word;word-break:normal;hyphens:none}
.caption span{opacity:.4;transition:opacity .25s}
.caption span.on{opacity:1}
.controls{display:flex;align-items:center;gap:12px;padding:12px 20px}
button{font:500 .82rem/1 "IBM Plex Sans",sans-serif;border:1px solid var(--line);
  background:var(--panel);color:var(--ink);border-radius:7px;padding:8px 13px;cursor:pointer}
button:hover{border-color:var(--accent)}
#play{background:var(--accent);border-color:var(--accent);color:#fff;min-width:82px}
.time{font-family:"IBM Plex Mono",monospace;font-size:.78rem;color:var(--dim);
  margin-left:auto;font-variant-numeric:tabular-nums}
.track{display:flex;gap:3px;padding:0 20px 16px}
.seg{flex:1;height:5px;border-radius:3px;background:var(--line);cursor:pointer;
  position:relative;overflow:hidden;border:0;padding:0;min-width:0}
.seg i{position:absolute;inset:0;width:0;background:var(--accent);display:block}
.seg.done i{width:100%}
.legend{font-size:.78rem;color:var(--dim);display:flex;flex-wrap:wrap;gap:6px 14px}
@media(prefers-reduced-motion:reduce){.seg i{transition:none!important}.caption span{transition:none}}
@media(max-width:640px){.stage{padding:20px 18px;min-height:340px}.caption{padding:14px 18px}
  h1{font-size:1.3rem}.stitle{font-size:1.22rem}}
</style>

<div class="wrap">
  <header>
    <h1>S3 Simulator Walkthrough</h1>
    <span class="draft">Draft</span>
    <div class="sub">Speed-scaling &amp; scheduling — a two-part demo</div>
    <div class="tabs" role="tablist">
      <button class="tab" role="tab" aria-selected="true"  data-p="p1">Part 1 · Running experiments</button>
      <button class="tab" role="tab" aria-selected="false" data-p="p2">Part 2 · Adding a policy</button>
    </div>
  </header>

  <div class="draftbar"><strong>Draft — under review.</strong> Content, numbers and figures are
  real simulator output, but this walkthrough has not been reviewed for release and the wording
  and structure are still changing.</div>

  <div class="player">
    <div class="stage">
      <div class="eyebrow" id="eyebrow"></div>
      <h2 class="stitle" id="stitle"></h2>
      <div class="body" id="body"></div>
    </div>
    <div class="caption" id="caption"></div>
    <div class="controls">
      <button id="play">Play</button>
      <button id="prev">‹ Prev</button>
      <button id="next">Next ›</button>
      <button id="speed">1×</button>
      <div class="time" id="time"></div>
    </div>
    <div class="track" id="track"></div>
  </div>

  <div class="legend">
    <span>Click any bar to jump to that segment.</span>
    <span>Captions double as the narration script.</span>
  </div>
</div>

<script id="data" type="application/json">__DATA__</script>
<script>
const D=JSON.parse(document.getElementById('data').textContent);
let part='p1',i=0,playing=false,t0=0,elapsed=0,rate=1,raf=null;
const $=id=>document.getElementById(id);
const segs=()=>D[part];
const reduce=matchMedia('(prefers-reduced-motion: reduce)').matches;

function renderTrack(){
  const tr=$('track'); tr.innerHTML='';
  segs().forEach((s,k)=>{
    const b=document.createElement('button');
    b.className='seg'; b.title=s.t; b.setAttribute('aria-label','Segment '+(k+1)+': '+s.t);
    b.innerHTML='<i></i>';
    b.onclick=()=>{go(k);};
    tr.appendChild(b);
  });
}
function renderBody(s){
  const c=$('body'); c.innerHTML='';
  s.body.forEach(b=>{
    let el;
    if(b.kind==='code'){ el=document.createElement('pre'); el.textContent=b.text; }
    else if(b.kind==='note'){ el=document.createElement('p'); el.className='note'; el.innerHTML=b.text; }
    else if(b.kind==='table'){
      const w=document.createElement('div'); w.className='tblwrap';
      const t=document.createElement('table');
      t.innerHTML='<thead><tr>'+b.head.map(h=>'<th>'+h+'</th>').join('')+'</tr></thead><tbody>'+
        b.rows.map(r=>'<tr>'+r.map(x=>'<td>'+x+'</td>').join('')+'</tr>').join('')+'</tbody>';
      w.appendChild(t); el=w;
    }
    else if(b.kind==='figure'){
      el=document.createElement('figure');
      el.innerHTML='<img alt="'+b.cap+'" src="'+b.src+'"><figcaption>'+b.cap+'</figcaption>';
    }
    else if(b.kind==='diagram'){
      el=document.createElement('figure');
      el.className='diagram';
      el.innerHTML=b.svg+'<figcaption>'+b.cap+'</figcaption>';
    }
    if(el) c.appendChild(el);
  });
}
function words(s){
  // Real space TEXT NODES between spans, never a space inside a span - a trailing
  // space inside an inline box wraps awkwardly and detaches from its word.
  const c=$('caption'); c.innerHTML='';
  s.n.split(/\\s+/).filter(Boolean).forEach((w,idx)=>{
    if(idx) c.appendChild(document.createTextNode(' '));
    const sp=document.createElement('span'); sp.textContent=w; c.appendChild(sp);
  });
  c.scrollTop=0;
}
function go(k){
  i=Math.max(0,Math.min(segs().length-1,k)); elapsed=0; t0=performance.now();
  const s=segs()[i];
  $('eyebrow').textContent='Segment '+(i+1)+' of '+segs().length;
  $('stitle').textContent=s.t;
  renderBody(s); words(s);
  [...$('track').children].forEach((b,n)=>{
    b.classList.toggle('done',n<i); b.querySelector('i').style.width=n<i?'100%':'0';
  });
  tick(true);
}
function fmt(x){const m=Math.floor(x/60),sec=Math.floor(x%60);return m+':'+String(sec).padStart(2,'0');}
function tick(force){
  const s=segs()[i], tot=segs().reduce((a,b)=>a+b.d,0);
  const before=segs().slice(0,i).reduce((a,b)=>a+b.d,0);
  const p=Math.min(1,elapsed/s.d);
  const bar=$('track').children[i]; if(bar) bar.querySelector('i').style.width=(p*100)+'%';
  // While playing, reveal narration word by word. When paused, show it all so the
  // caption doubles as a readable script.
  const spans=$('caption').children, n=Math.floor(p*spans.length);
  for(let k=0;k<spans.length;k++) spans[k].classList.toggle('on', playing ? k<=n : true);
  $('time').textContent=fmt(before+elapsed)+' / '+fmt(tot);
}
function loop(now){
  if(!playing) return;
  elapsed+=(now-t0)/1000*rate; t0=now;
  const s=segs()[i];
  if(elapsed>=s.d){
    if(i<segs().length-1){ go(i+1); if(playing){t0=performance.now();} }
    else { playing=false; $('play').textContent='Replay'; elapsed=s.d; tick(); return; }
  }
  tick(); raf=requestAnimationFrame(loop);
}
$('play').onclick=()=>{
  if($('play').textContent==='Replay'){ go(0); }
  playing=!playing; $('play').textContent=playing?'Pause':'Play';
  if(playing){ t0=performance.now(); raf=requestAnimationFrame(loop); }
  else cancelAnimationFrame(raf);
};
$('prev').onclick=()=>{go(i-1); if(playing)t0=performance.now();};
$('next').onclick=()=>{go(i+1); if(playing)t0=performance.now();};
$('speed').onclick=()=>{ rate=rate===1?1.5:rate===1.5?2:rate===2?0.75:1; $('speed').textContent=rate+'\\u00d7'; };
document.querySelectorAll('.tab').forEach(b=>b.onclick=()=>{
  document.querySelectorAll('.tab').forEach(x=>x.setAttribute('aria-selected',String(x===b)));
  part=b.dataset.p; renderTrack(); go(0);
  if(playing){t0=performance.now();}
});
renderTrack(); go(0);
</script>
"""

open(OUT, "w", encoding="utf-8").write(TPL.replace("__DATA__", json.dumps(data)))
kb = os.path.getsize(OUT) / 1024
print(f"wrote {OUT}")
print(f"  size: {kb:.0f} KB   part1={len(data['p1'])} segments   part2={len(data['p2'])} segments")
print(f"  figures still pending: {missing}")
