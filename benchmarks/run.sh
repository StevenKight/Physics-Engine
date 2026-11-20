#!/usr/bin/env bash
# Builds the benchmarks in Release mode, records the environment and runs all
# three backends into a timestamped CSV under benchmarks/results/.
#
# Run from anywhere: ./benchmarks/run.sh
# Takes roughly an hour. Close other heavy programs first; the timings are
# only meaningful on an otherwise idle machine.

set -e
cd "$(dirname "$0")/.."

cmake -B build/bench -DCMAKE_BUILD_TYPE=Release
cmake --build build/bench -j"$(nproc)"

./benchmarks/record_env.sh

mkdir -p benchmarks/results
out="benchmarks/results/bench_$(date -u +%Y%m%dT%H%M%SZ).csv"
echo "scenario,backend,threads,n,ticks,runs,median_ms,min_ms,max_ms,note" > "$out"

for backend in fortran cuda router; do
    "./build/bench/benchmarks/bench_$backend" "$out"
done

echo "Wrote $out"
