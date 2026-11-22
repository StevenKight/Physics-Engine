# Profile

One CUDA `newtonian_gravity()` call at N = 4096 on the RTX 3070 (see [ENVIRONMENT.md](ENVIRONMENT.md)), from `./benchmarks/profile.sh` with Nsight Compute run as root. Raw data is in [profile/](profile/).

- **Where the time goes:** about 2.06 s in host/device copies against 0.09 s in kernels (Nsight Systems). `matrix_power_kernel` is 77% of the kernel time, 7 launches of about 10 ms each.
- **Achieved occupancy:** 82.1% of a theoretical 83.3% for `matrix_power_kernel`, limited by registers (44 per thread, 5 blocks per SM).
- **Compute or memory bound:** compute bound. SM throughput is 86% while DRAM throughput is 5.5%.
- **Top limiter:** the FP64 pipeline at 86% utilization. The kernel calls the general `pow()` to square values and take square roots, and the RTX 3070 has little FP64 throughput.
- **Optimization (not implemented):** the copies outweigh the kernels more than 20 to 1, so the largest gain is keeping every intermediate matrix on the GPU for the whole call: upload the positions and masses once, run all 34 matrix operations on device buffers, and download only the forces.
