# How the walkthrough is regenerated

`build_demo_player.py` is the **single source of truth** for the artifact. The HTML is
generated, never hand-edited — edit the `P1` / `P2` segment lists in the script and re-run it.

```bash
python build_demo_player.py
```

It writes `../S3-simulator-walkthrough.html` (figures are inlined as base64 data URIs, so the
page is self-contained). Republishing that file to the same artifact URL updates the live page:

    https://claude.ai/code/artifact/32f9534a-36d7-4bb3-982a-d651dbd42b89

The two markdown scripts (`../PART1_*.md`, `../PART2_*.md`) are maintained by hand and must be
kept in step with the segment list — they are the recording scripts, the artifact is the visual.

## Figure generators

| Script | Produces | From |
|---|---|---|
| `build_singlespeed_sweep.py` | `fig_loadsweep_singlespeed.png` + `.csv` | `demoE_*` runs (19 loads, SingleSpeed) |
| `build_slowdown_fine.py` | `fig3_slowdown_vs_size.png`, `fig4_part2_PSJF_comparison.png` + `.csv`s | coarse `demoD_*` / `part2_*` runs merged with fine `demoF_*` / `part2b_*` runs |

Both read from `C:\Users\melahi\AppData\Local\clauderepo\demo-s3\data\outputs\` and hard-code the
run timestamps. If the runs are redone, update the stamp constants at the top of each script.

`fig1_ET_two_settings.png` and `fig2_loadsweep_speedscaling.png` were produced earlier in the
same way; their generators were not preserved separately.

## Merging probe sweeps is safe

Probe insertion is deterministic — `ProbeSetting::computeRandomProbeInsertionTime` returns
`startPeriod + (endPeriod - startPeriod)/rounds * thisround`, which depends only on the round
count and the background trace. So a coarse sweep and a fine sweep with the same `rounds` and the
same workload config share identical probe instants, and their results can be pooled by size.
`build_slowdown_fine.py` asserts there are no overlapping sizes and that every size has exactly
100 probes.
