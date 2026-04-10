Architecture
============

This document outlines the architecture of the High-Performance Physics Engine, detailing the project structure and the roles of its various components.

Project Structure
-----------------

```
Physics-Engine/
├── .vscode/
│   ├── extensions.json
│   └── settings.json
├── data/
├── docs/
│   └── Wiki...
├── src/
│   ├── logic/
│   │   ├── CMakeLists.txt
│   │   ├── gravity.c
│   │   └── gravity.h
│   ├── math/
│   │   ├── cuda/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── cuda_matrix.h
│   │   │   ├── matrix_core.h
│   │   │   ├── matrix.cu
│   │   │   ├── matrix_add.cu
│   │   │   ├── matrix_div.cu
│   │   │   ├── matrix_hadamard.cu
│   │   │   ├── matrix_mul.cu
│   │   │   ├── matrix_power.cu
│   │   │   ├── matrix_scalar.cu
│   │   │   ├── matrix_sub.cu
│   │   │   └── matrix_sum.cu
│   │   ├── fortran/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── fortran_matrix.h
│   │   │   ├── matrix_add.f90
│   │   │   ├── matrix_div.f90
│   │   │   ├── matrix_hadamard.f90
│   │   │   ├── matrix_mul.f90
│   │   │   ├── matrix_power.f90
│   │   │   ├── matrix_scalar.f90
│   │   │   ├── matrix_sub.f90
│   │   │   └── matrix_sum.f90
│   │   ├── CMakeLists.txt
│   │   ├── matrix.c
│   │   └── matrix.h
│   ├── models/
│   │   ├── CMakeLists.txt
│   │   ├── object.c
│   │   └── object.h
│   ├── CMakeLists.txt
│   └── main.cpp
├── test/
│   ├── framework/
│   │   ├── minunit.h
│   │   └── test_runner.h
│   ├── logic/
│   │   └── test_newtonian_gravity.c
│   ├── math/
│   │   ├── test_matrix_add.c
│   │   ├── test_matrix_mul.c
│   │   ├── test_matrix_power.c
│   │   ├── test_matrix_scalar.c
│   │   └── test_matrix_sub.c
│   └── CMakeLists.txt
├── .clang-format
├── .gitignore
├── .gitmodules
├── ARCHITECTURE.md
├── CMakeLists.txt
├── CONTRIBUTING.md
├── LICENSE
└── README.md
```

Directory Overview
------------------

- `src/`: Main directory for all source code, organised into modules by responsibility.
    - `math/`: Backend-agnostic matrix operation API. `matrix.h` and `matrix.c` expose a unified interface; each operation accepts a `use_gpu` flag that routes the call to either the `cuda/` or `fortran/` backend at runtime.
        - `math/cuda/`: CUDA kernels for GPU-accelerated matrix operations. Implements addition, subtraction, multiplication, scalar operations, element-wise division, Hadamard product, power, and row/column summing. Uses row-major double-precision storage.
        - `math/fortran/`: Fortran implementations of the same matrix operations for CPU execution. Uses column-major double-precision arrays; tight-loop structure lets the Fortran compiler apply aggressive optimisations without GPU dispatch overhead.
    - `logic/`: Physics calculations built on top of the math layer. Currently contains Newtonian N-body gravity (`gravity.c`/`gravity.h`), which decomposes force computation into matrix operations and delegates to the appropriate backend based on problem size.
    - `models/`: Data structures for simulation objects. Defines `Vec3` (3D double-precision vector) and `PhysicsObject` (mass, position, velocity, acceleration, force).
    - `main.cpp`: Entry point. Orchestrates the simulation and exercises the engine's subsystems.
- `test/`: Unit tests mirroring the `src/` module structure.
    - `test/framework/`: Minimal test utilities (`minunit.h`, `test_runner.h`) used across all tests.
    - `test/math/`: Tests for each matrix operation, verifying both CPU and GPU backends.
    - `test/logic/`: Tests for physics calculations, including multi-body gravity scenarios.
- `data/`: Directory for simulation data files (initial conditions, scene definitions).
- `docs/`: Project wiki submodule. Contains mathematical derivations, algorithm notes, and design rationale as they are worked out.
- `.vscode/`: VS Code workspace settings for a consistent development environment.
- `CMakeLists.txt`: Root CMake script configuring the multi-language build (C, C++, Fortran, CUDA).
- `.clang-format`: LLVM-style formatting rules enforced across C/C++ code.

Core Technology Integration
----------------------------

The engine is designed around a hybrid CPU/GPU computing model — routing each computation to whichever hardware is most effective for the scale of work involved.

**C/C++** serves as the primary coordination layer, providing the high-level simulation structure and connecting the specialised modules.

**CUDA** is used to offload computationally expensive and highly parallelisable work — matrix operations, force calculations, and collision detection — to the GPU.

**Fortran** handles specific, performance-critical numerical calculations better suited to the CPU: tight-loop operations that do not justify GPU dispatch overhead at small scales.

### Computation Routing

A core design principle of the engine is adaptive backend selection. Each subsystem independently decides whether to execute on CPU (Fortran) or GPU (CUDA) based on the scale of the problem, avoiding unnecessary overhead in either direction.

The `math/` layer exposes this through a `use_gpu` flag on every operation. Higher-level subsystems evaluate their own threshold conditions and pass the appropriate flag down. For example, the gravity subsystem currently routes:

- **≤ 64 bodies** → Fortran (CPU): problem is small enough that Fortran's tight-loop optimisations outperform the cost of GPU dispatch and memory transfers.
- **> 64 bodies** → CUDA (GPU): parallelism benefits exceed dispatch overhead at this scale.

As additional subsystems are implemented (collision detection, constraint solving, etc.), they will follow the same pattern with thresholds tuned for their specific computational characteristics.
