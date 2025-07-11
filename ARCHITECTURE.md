Architecture
============

This document outlines the architecture of the High-Performance Physics Engine, detailing the project structure and the roles of its various components.

Project Structure
-----------------

Below is the folder and file structure of the physics engine project.

```
physics-engine/
├── .vscode/
│   ├── extensions.json
│   └── settings.json
├── data/
│   ├── example.phys
│   └── test.phys
├── docs/
│   └── Wiki...
├── src/
│   ├── fortran/
│   │   ├── CMakeLists.txt
│   │   ├── dummy.f90
│   │   └── dummy.h
│   ├── io/
│   │   ├── CMakeLists.txt
│   │   ├── parser.cu
│   │   └── parser.h
│   ├── matrix/
│   │   ├── CMakeLists.txt
│   │   ├── matrix.cu
│   │   └── matrix.h
│   ├── state/
│   │   ├── CMakeLists.txt
│   │   └── object.h
│   ├── CMakeLists.txt
│   ├── constants.h
│   └── main.cpp
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

- `src/`: This is the main directory for all source code. It's organized into modules, each with a specific responsibility.
    - `fortran/`: This module contains Fortran code for performance-critical, small-scale numerical calculations. These routines are optimized for fast array manipulations and tight-loop operations that run efficiently on the CPU.
    - `io/`: This handles input/output operations, such as parsing simulation data from .phys files. The use of .cu files suggests that some parsing logic may be accelerated or interact directly with GPU memory.
    - `matrix/`: This directory holds the CUDA-based code for GPU-accelerated matrix operations. Offloading these large-scale parallel computations to the GPU is a key part of the engine's high-performance design.
    - `state/`: This module is responsible for managing the state of physical objects within the simulation, defined in object.h.
    - `main.cpp`: This is the central entry point of the application, orchestrating the simulation and coordinating the different modules.
- `data/`: This directory contains sample data files for running simulations. The .phys files likely define the initial conditions and objects for a physics scene.
- `docs/`: Contains project documentation. The Wiki subdirectory holds detailed mathematical derivations, notes on algorithms, and explanations of how the engine's technologies are used.
- `.vscode/`: This folder holds project-specific settings for Visual Studio Code, such as recommended extensions and workspace configurations, to ensure a consistent development environment.
- `CMakeLists.txt`: The main script for the CMake build system, used to configure and build the project across different platforms.
- `README.md`: Provides a general overview of the project, its goals, key technologies, and setup instructions.
- `ARCHITECTURE.md`: This file, which describes the project's structure and design.
- `LICENSE`: Contains the full text of the GNU General Public License v3.0 under which the project is distributed.
- `.gitignore`, .gitmodules, .clang-format: Standard configuration files for Git and code formatting.

Core Technology Integration
----------------------------

The engine's architecture is designed around a hybrid CPU/GPU computing model to maximize performance.

C++ serves as the primary language, providing the high-level structure and connecting the different specialized modules.

CUDA is used to offload computationally expensive and highly parallelizable tasks—such as matrix operations, collision detection, and force calculations—to the GPU for significant acceleration.

Fortran is employed for specific, performance-critical numerical calculations that are better suited for the CPU, such as tight-loop operations that do not benefit from massive parallelism.

This combination allows the engine to use the most effective hardware for each type of computation, achieving a high degree of optimization.