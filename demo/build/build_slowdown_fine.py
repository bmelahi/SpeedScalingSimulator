import csv, glob, os
from collections import defaultdict
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt

BASE = r"C:\Users\melahi\AppData\Local\clauderepo\demo-s3\data\outputs"
OUT  = r"G:\My Drive\Research\Projects\S3Update\Claude Output\SimulatorDemo\figures"
COARSE = {"FCFS":"15H59M","PS":"15H59M","SRPT":"15H59M","SJF":"16H05M","PSJF":"16H05M"}
FINE   = "18H14M"   # sizes 0.05, 0.5

def folder(pol, stamp):
    g = sorted(glob.glob(os.path.join(BASE, f"{pol}-1--ALPHA-2--EXPONENTIAL-100000-0.80-1--2026-08-28--{stamp}*")))
    assert len(g) == 1, (pol, stamp, g)
    return g[0]

def slowdown(path):
    s, c = defaultdict(float), defaultdict(int)
    with open(os.path.join(path, "probereport.txt")) as f:
        next(f)
        for line in f:
            q = line.split()
            if len(q) < 8: continue
            s[float(q[1])] += float(q[7]); c[float(q[1])] += 1
    return {k: s[k]/c[k] for k in s}, {k: c[k] for k in c}

def merged(pol):
    a, ca = slowdown(folder(pol, COARSE[pol]))
    b, cb = slowdown(folder(pol, FINE))
    assert not (set(a) & set(b)), f"{pol}: overlapping sizes {set(a)&set(b)}"
    for k, v in list(ca.items()) + list(cb.items()):
        assert v == 100, f"{pol}: size {k} has {v} probes, expected 100"
    return {**a, **b}

def draw(pols, colors, markers, fname, title, ps_line=True):
    SL = {p: merged(p) for p in pols}
    sizes = sorted(SL[pols[0]])
    for p in pols:
        assert sorted(SL[p]) == sizes, p
    with open(os.path.join(OUT, fname + ".csv"), "w", newline="") as f:
        w = csv.writer(f); w.writerow(["JobSize"] + pols)
        for s in sizes: w.writerow([s] + [round(SL[p][s], 6) for p in pols])
    fig, ax = plt.subplots(figsize=(8, 5.4), dpi=150)
    ax.axhline(5.0, color="#7f7f7f", linestyle="--", linewidth=1.6,
               label="PS (theory) = 1/(1\u2212\u03c1) = 5")
    for p in pols:
        ax.plot(sizes, [SL[p][s] for s in sizes], label=p, color=colors[p],
                marker=markers[p], linewidth=2, markersize=6)
    ax.set_xscale("log"); ax.set_yscale("log")
    ax.set_xlabel("Job size  x   (log scale)", fontsize=12)
    ax.set_ylabel("Mean slowdown  E[T(x)] / x   (log scale)", fontsize=12)
    ax.set_title(title, fontsize=11)
    ax.legend(loc="best", frameon=True); ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout(); fig.savefig(os.path.join(OUT, fname + ".png"))
    print("wrote", fname + ".png")
    hdr = "  x      " + "".join(f"{p:>12}" for p in pols)
    print(hdr)
    for s in sizes:
        print(f"  {s:<7}" + "".join(f"{SL[p][s]:12.3f}" for p in pols))
    return SL

draw(["FCFS", "PS", "SRPT"],
     {"FCFS": "#d62728", "PS": "#7f7f7f", "SRPT": "#9467bd"},
     {"FCFS": "s", "PS": "o", "SRPT": "D"},
     "fig3_slowdown_vs_size",
     "Mean slowdown by job size, load 0.8\n"
     "probe sizes 0.05, 0.5, then 1, 3, ... 25   (100 probes each)")
print()
draw(["SJF", "PSJF", "SRPT"],
     {"SJF": "#ff7f0e", "PSJF": "#1f77b4", "SRPT": "#9467bd"},
     {"SJF": "v", "PSJF": "P", "SRPT": "D"},
     "fig4_part2_PSJF_comparison",
     "Non-preemptive SJF vs preemptive PSJF vs SRPT, load 0.8\n"
     "probe sizes 0.05, 0.5, then 1, 3, ... 25   (100 probes each)")
