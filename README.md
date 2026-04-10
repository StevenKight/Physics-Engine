# High-Performance Physics Engine

A research project building a physics simulation engine using C/C++, Fortran, and CUDA. The goal is rigid body mechanics simulation with computation routed to whatever hardware makes the most sense for the job.

The short version: CUDA handles the big parallel workloads, Fortran handles small tight-loop numerical operations where it genuinely outperforms the alternatives, and C++ manages everything in between. Getting all three to compile together through CMake was its own adventure.

---

## Why This Stack

CUDA for GPU-accelerated parallel work is a straightforward choice. The Fortran decision gets more questions — the reasoning is that for small, tight numerical loops, Fortran's array handling and compiler optimizations are hard to beat and don't carry the overhead of GPU dispatch. Rather than forcing everything onto the GPU, the idea is to use the right tool for the scale of the problem.

C/C++ ties it all together and handles the core simulation logic.

---

## Current State

- CMake build system configured for all three languages compiling together
- Complete matrix operation suite (addition, subtraction, multiplication, scalar operations, element-wise division, Hadamard product, power, row/column summing) implemented in both CUDA and Fortran backends
- Newtonian N-body gravity with adaptive CPU/GPU routing (Fortran for ≤64 bodies, CUDA above that threshold)
- Unit test suite covering matrix operations and gravity calculations
- Rigid body mechanics in active development

The [Wiki](https://github.com/StevenKight/Physics-Engine/wiki) has derivations and math notes as they get worked out.

---

## Build

### Prerequisites

- NVIDIA GPU and the [CUDA Toolkit](https://developer.nvidia.com/cuda-toolkit)
- Fortran compiler (GNU Fortran or Intel Fortran)
- [CMake](https://cmake.org/download/) 3.18+
- CuBLAS (optional, for optimized matrix operations)

### Instructions

```bash
git clone https://github.com/StevenKight/Physics-Engine.git
cd Physics-Engine
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
cd build
ctest
```

---

## Roadmap

- Optimized matrix operations replacing naive implementations
- Core rigid body mechanics
- Benchmarking against existing engines
- Soft-body dynamics and constraint systems
- Mixed-precision arithmetic for GPU throughput
- Shared library interface for external use
- Rendering pipeline integration

---

## License

Licensed under the [GNU General Public License v3.0](./LICENSE). Derivative works must carry the same license.

---

## Contributing

Not open for contributions yet. If you want to discuss the project or have ideas, open an issue or reach out directly. See [CONTRIBUTING.md](./CONTRIBUTING.md) for more details.
