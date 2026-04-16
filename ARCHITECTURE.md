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
├── blender/
│   ├── __init__.py
│   ├── blender_manifest.toml
│   ├── operators.py
│   ├── panels.py
│   ├── preferences.py
│   └── properties.py
├── data/
├── docs/
│   └── Wiki...
├── interface/
│   └── nbody.py
├── scripts/
│   └── package_blender.py
├── src/
│   ├── logic/
│   │   ├── forces/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── gravity.c
│   │   │   └── gravity.h
│   │   ├── CMakeLists.txt
│   │   ├── sim.c
│   │   └── sim.h
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
│   │   ├── matrix.h
│   │   ├── vec3.c
│   │   └── vec3.h
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
│   ├── models/
│   │   └── test_object_step.c
│   └── CMakeLists.txt
├── .clang-format
├── .gitignore
├── .gitmodules
├── ARCHITECTURE.md
├── CMakeLists.txt
├── CONTRIBUTING.md
├── LICENSE
├── Makefile
├── README.md
└── Sample - Solar System.blend
```

Directory Overview
------------------

- `src/`: Main directory for all source code, organised into modules by responsibility.
    - `math/`: Backend-agnostic matrix operation API. `matrix.h` and `matrix.c` expose a unified interface; each operation accepts a `use_gpu` flag that routes the call to either the `cuda/` or `fortran/` backend at runtime. Also contains `vec3.h`/`vec3.c`, a lightweight 3D double-precision vector type used throughout the engine.
        - `math/cuda/`: CUDA kernels for GPU-accelerated matrix operations. Implements addition, subtraction, multiplication, scalar operations, element-wise division, Hadamard product, power, and row/column summing. Uses row-major double-precision storage.
        - `math/fortran/`: Fortran implementations of the same matrix operations for CPU execution. Uses column-major double-precision arrays; tight-loop structure lets the Fortran compiler apply aggressive optimisations without GPU dispatch overhead.
    - `logic/`: Physics calculations built on top of the math layer. Contains `sim.c`/`sim.h`, which drives the top-level N-body simulation loop (`sim_run`): each tick accumulates forces on all objects, then advances each object via Velocity Verlet integration.
        - `logic/forces/`: Force implementations. Currently contains Newtonian N-body gravity (`gravity.c`/`gravity.h`), which decomposes force computation into matrix operations and delegates to the appropriate backend based on problem size.
    - `models/`: Data structures for simulation objects. `object.h`/`object.c` define `PhysicsObject` (mass, position, velocity, acceleration, force — all using `Vec3`) and `object_step()`, which advances an object by one Velocity Verlet step and resets its accumulated force.
    - `main.cpp`: Entry point. Orchestrates the simulation and exercises the engine's subsystems.
- `blender/`: Blender addon that integrates the N-body simulation into Blender's physics system.
    - `__init__.py`: Addon entry point. Registers all classes, property groups, and UI extensions on load and cleans them up on unregister.
    - `operators.py`: Two operators — `PHYSICS_ENGINE_OT_run` bakes the simulation frame-by-frame and inserts location keyframes on every N-body object; `PHYSICS_ENGINE_OT_clear` removes those keyframes and restores each object to its pre-bake position.
    - `panels.py`: UI panels and draw callbacks that expose simulation controls in Blender's Physics Properties and Scene panels.
    - `preferences.py`: Addon preferences panel. Stores the path to the Physics Engine project root and provides `load_interface()`, which dynamically imports `PhysicsObject` and `sim_run` from `interface.nbody` at runtime.
    - `properties.py`: `PhysicsEngineSceneProperties` (time-step setting, stored on `bpy.types.Scene`) and `PhysicsEngineObjectProperties` (per-object mass, initial velocity, and N-body enable flag stored on `bpy.types.Object`). Enabling an object automatically strips conflicting Blender physics systems.
    - `blender_manifest.toml`: Blender Extension manifest declaring addon metadata.
- `interface/`: Language bindings for the compiled shared library.
    - `interface/nbody.py`: Python ctypes interface. Mirrors the `Vec3` and `PhysicsObject` C structs and exposes `sim_run()` so simulations can be driven from Python without recompiling.
- `scripts/`: Utility scripts for development and packaging.
    - `scripts/package_blender.py`: Packages the `blender/` directory into `physics_engine.zip` for Blender Extension installation. Invoked via `make package`.
- `test/`: Unit tests mirroring the `src/` module structure.
    - `test/framework/`: Minimal test utilities (`minunit.h`, `test_runner.h`) used across all tests.
    - `test/math/`: Tests for each matrix operation, verifying both CPU and GPU backends.
    - `test/logic/`: Tests for physics calculations, including multi-body gravity scenarios.
    - `test/models/`: Tests for simulation object behaviour, including Velocity Verlet integration correctness.
- `data/`: Directory for simulation data files (initial conditions, scene definitions).
- `docs/`: Project wiki submodule. Contains mathematical derivations, algorithm notes, and design rationale as they are worked out.
- `.vscode/`: VS Code workspace settings for a consistent development environment.
- `CMakeLists.txt`: Root CMake script configuring the multi-language build (C, C++, Fortran, CUDA).
- `Makefile`: Convenience wrapper exposing `build`, `test`, `package`, `clean`, and `help` targets so common workflows don't require remembering raw CMake or Python invocations.
- `Sample - Solar System.blend`: Pre-built Blender scene demonstrating the addon with a solar system setup. Planet models by FyorDev on SketchFab.
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
