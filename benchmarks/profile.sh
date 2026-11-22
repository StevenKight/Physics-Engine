#!/usr/bin/env bash
# Profiles one CUDA gravity call at N bodies (default 4096) and writes the
# summaries to benchmarks/profile/.
#
#   Nsight Systems: kernel, memcpy and CUDA API time    (always)
#   Nsight Compute: matrix_power_kernel details        (if counters allowed)
#
# Run from anywhere: ./benchmarks/profile.sh [N]
#
# Nsight Compute needs access to the GPU performance counters, which the
# driver only gives to root by default. To include it, pass a sudo'd ncu:
#   NCU="sudo /usr/local/cuda/bin/ncu" ./benchmarks/profile.sh
#
# NSYS can be set the same way to use a different nsys.

set -e
cd "$(dirname "$0")/.."

n=${1:-4096}
NSYS=${NSYS:-nsys}
NCU=${NCU:-/usr/local/cuda/bin/ncu}
bin=build/bench/benchmarks/profile_gravity
out=benchmarks/profile
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

cmake -B build/bench -DCMAKE_BUILD_TYPE=Release > /dev/null
cmake --build build/bench --target profile_gravity -j"$(nproc)"
mkdir -p "$out"

$NSYS profile --capture-range=cudaProfilerApi --capture-range-end=stop \
    -o "$tmp/gravity" "$bin" "$n"
for report in cuda_gpu_kern_sum cuda_gpu_mem_time_sum cuda_api_sum; do
    $NSYS stats -q --force-export true --format csv --report "$report" \
        "$tmp/gravity.nsys-rep" |
        grep -v '^$' > "$out/nsys_${report}_n$n.csv"
done

# matrix_power_kernel takes most of the kernel time (see the nsys summary).
if $NCU --profile-from-start off -k matrix_power_kernel -c 1 \
    --section SpeedOfLight --section Occupancy --section LaunchStats \
    --section ComputeWorkloadAnalysis --csv --page details \
    "$bin" "$n" > "$tmp/ncu.csv" 2> "$tmp/ncu.log" &&
    grep -q '"ID"' "$tmp/ncu.csv"; then
    grep -v '^==PROF==' "$tmp/ncu.csv" > "$out/ncu_matrix_power_kernel_n$n.csv"
else
    echo "Nsight Compute step skipped:"
    grep -h "==ERROR==" "$tmp/ncu.csv" "$tmp/ncu.log" || cat "$tmp/ncu.log"
fi

echo "Wrote $out/"
