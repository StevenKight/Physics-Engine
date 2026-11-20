"""Plots the newest benchmark CSV into benchmarks/plots/.

Writes:
  gravity_scaling.png  time per gravity call vs N for each backend
  speedup.png          Fortran time / CUDA time vs N, crossover marked
  sim_scaling.png      time per sim_run tick vs N, 1 thread vs all cores

Usage: python3 benchmarks/plot.py
"""

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

from crossover import CURRENT_THRESHOLD, HERE, find_crossover, latest_csv, load

BLUE, ORANGE, AQUA = "#2a78d6", "#eb6834", "#1baf7a"
GRID = "#e4e3df"
TEXT = "#5f5e5a"

BACKENDS = [
    ("fortran", "Fortran", BLUE, "o"),
    ("cuda", "CUDA", ORANGE, "s"),
    ("router", "Router (default)", AQUA, "^"),
]


def style(ax, title, ylabel):
    ax.set_xscale("log", base=2)
    ax.set_yscale("log")
    ax.set_title(title, loc="left", fontsize=11)
    ax.set_xlabel("Bodies (N)", color=TEXT)
    ax.set_ylabel(ylabel, color=TEXT)
    ax.grid(True, which="major", color=GRID, linewidth=0.8)
    ax.tick_params(colors=TEXT)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color(GRID)


def plot_series(ax, rows, label, color, marker, linestyle="-"):
    ns = sorted(rows)
    med = [rows[n]["median_ms"] for n in ns]
    low = [rows[n]["median_ms"] - rows[n]["min_ms"] for n in ns]
    high = [rows[n]["max_ms"] - rows[n]["median_ms"] for n in ns]
    ax.errorbar(ns, med, yerr=[low, high], label=label, color=color,
                marker=marker, markersize=6, linewidth=2, linestyle=linestyle,
                capsize=3, markeredgecolor="white")


def gravity_scaling(series, out):
    fig, ax = plt.subplots(figsize=(7.5, 4.5))
    for backend, label, color, marker in BACKENDS:
        rows = series.get(("gravity", backend, 1))
        if rows:
            dashed = "--" if backend == "router" else "-"
            plot_series(ax, rows, label, color, marker, dashed)
    style(ax, "Gravity: time per newtonian_gravity() call", "Time (ms)")
    ax.legend(frameon=False)
    fig.tight_layout()
    fig.savefig(out / "gravity_scaling.png", dpi=150)
    plt.close(fig)


def speedup(series, out):
    fortran = series[("gravity", "fortran", 1)]
    cuda = series[("gravity", "cuda", 1)]
    ns = sorted(set(fortran) & set(cuda))
    ratio = [fortran[n]["median_ms"] / cuda[n]["median_ms"] for n in ns]

    fig, ax = plt.subplots(figsize=(7.5, 4.5))
    ax.plot(ns, ratio, color=ORANGE, marker="s", markersize=6, linewidth=2,
            markeredgecolor="white")
    ax.axhline(1.0, color=TEXT, linewidth=1)
    ax.annotate("equal speed", (ns[0], 1.0), xytext=(0, 4),
                textcoords="offset points", fontsize=9, color=TEXT)

    ax.axvline(CURRENT_THRESHOLD, color=TEXT, linewidth=1, linestyle=":")
    ax.annotate(f"current threshold ({CURRENT_THRESHOLD})",
                (CURRENT_THRESHOLD, max(ratio)), xytext=(4, 0),
                textcoords="offset points", fontsize=9, color=TEXT, va="top")

    result = find_crossover(fortran, cuda)
    if result and result[2]:
        estimate = result[2]
        ax.axvline(estimate, color=ORANGE, linewidth=1, linestyle="--")
        ax.annotate(f"measured crossover (N ≈ {estimate:.0f})",
                    (estimate, min(ratio)), xytext=(4, 0),
                    textcoords="offset points", fontsize=9, color=TEXT)

    style(ax, "CUDA speedup over Fortran (gravity)", "Fortran time / CUDA time")
    fig.tight_layout()
    fig.savefig(out / "speedup.png", dpi=150)
    plt.close(fig)


def sim_scaling(series, out):
    threads = sorted({t for (s, b, t) in series if s == "sim_sparse"})
    if not threads:
        return

    fig, axes = plt.subplots(1, 2, figsize=(10, 4.2), sharey=True)
    titles = {"sim_sparse": "Sparse boxes", "sim_dense": "Dense boxes"}
    for ax, scenario in zip(axes, ("sim_sparse", "sim_dense")):
        for t, color, marker in zip(threads, (BLUE, ORANGE), ("o", "s")):
            rows = series.get((scenario, "router", t))
            if rows:
                label = "1 thread" if t == 1 else f"{t} threads"
                plot_series(ax, rows, label, color, marker)
        style(ax, f"{titles[scenario]}: time per sim_run() tick", "Time (ms)")
    axes[0].legend(frameon=False)
    fig.tight_layout()
    fig.savefig(out / "sim_scaling.png", dpi=150)
    plt.close(fig)


def main():
    series = load(latest_csv())
    out = HERE / "plots"
    out.mkdir(exist_ok=True)
    gravity_scaling(series, out)
    speedup(series, out)
    sim_scaling(series, out)
    print(f"Wrote plots to {out.relative_to(HERE.parent)}")


if __name__ == "__main__":
    main()
