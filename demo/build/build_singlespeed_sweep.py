import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt, os, csv

base = r"C:\Users\melahi\AppData\Local\clauderepo\demo-s3\data\outputs"
stamp = "2026-08-28--18H04M37S"
pol = ["FCFS", "PS", "SRPT"]
loads = [round(0.05*(k+1), 2) for k in range(19)]

ET = {}
for p in pol:
    f = os.path.join(base, f"{p}-1--ALPHA-2--EXPONENTIAL-100000-0.80-1--{stamp}", "meanzlog.txt")
    rows = [l.split() for l in open(f).readlines()[1:] if l.strip()]
    assert len(rows) == 19, (p, len(rows))
    ET[p] = [float(r[0]) for r in rows]

out = r"G:\My Drive\Research\Projects\S3Update\Claude Output\SimulatorDemo\figures"
with open(os.path.join(out, "fig_loadsweep_singlespeed.csv"), "w", newline="") as f:
    w = csv.writer(f); w.writerow(["Load"] + pol + ["Theory E[X]/(1-rho)"])
    for i, L in enumerate(loads):
        w.writerow([L] + [round(ET[p][i], 6) for p in pol] + [round(1/(1-L), 6)])

colors = {"FCFS": "#d62728", "PS": "#7f7f7f", "SRPT": "#9467bd"}
markers = {"FCFS": "s", "PS": "o", "SRPT": "D"}

fig, ax = plt.subplots(figsize=(8, 5.2), dpi=150)
ax.plot(loads, [1/(1-L) for L in loads], color="black", linestyle=":", linewidth=1.6,
        label="theory:  E[X] / (1 − ρ)", zorder=1)
for p in pol:
    ax.plot(loads, ET[p], color=colors[p], marker=markers[p], linewidth=2, markersize=5.5,
            label=p, zorder=2)

ax.set_xlabel("Load  ρ", fontsize=12)
ax.set_ylabel("Mean response time  E[T]", fontsize=12)
ax.set_title("Fixed speed, no speed scaling: E[T] blows up as ρ → 1\n"
             "(Poisson arrivals, Exponential job sizes, E[X] = 1, 100,000 jobs)", fontsize=11)
ax.set_xlim(0, 1.0); ax.set_ylim(0, 24)
ax.legend(loc="upper left", frameon=True)
ax.grid(True, alpha=0.3)
fig.tight_layout()
fig.savefig(os.path.join(out, "fig_loadsweep_singlespeed.png"))
print("wrote fig_loadsweep_singlespeed.png")
for i, L in enumerate(loads):
    if L in (0.5, 0.8, 0.9, 0.95):
        print(f"  rho={L:<5} FCFS={ET['FCFS'][i]:7.3f}  PS={ET['PS'][i]:7.3f}  SRPT={ET['SRPT'][i]:7.3f}  theory={1/(1-L):7.3f}")
