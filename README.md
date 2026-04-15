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
- `Vec3` 3D vector type and `PhysicsObject` model with Velocity Verlet integration (`object_step`)
- Newtonian N-body gravity with adaptive CPU/GPU routing (Fortran for ≤64 bodies, CUDA above that threshold)
- Top-level `sim_run` simulation loop wiring force accumulation and object integration together
- Python ctypes interface (`interface/nbody.py`) for driving simulations from Python without recompiling
- Unit test suite covering matrix operations, gravity calculations, and Velocity Verlet integration

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
make
```

### Running Tests

```bash
make test
```

---

## Blender Addon

The `blender/` directory contains a Blender addon that exposes the simulation through the Physics properties panel, allowing objects to be tagged as N-Body participants with their mass and initial velocity set directly in the UI.

### Prerequisites

- Blender 4.2 or later
- The project cloned and built locally (see [Build](#build) above)

### Installation

**1. Build the addon zip**

From the project root:

```bash
make package
```

**2. Install in Blender**

- Open **Edit > Preferences > Get Extensions**
- Click the dropdown arrow (top right) and select **Install from Disk**
- Select `physics_engine.zip` from the project root

**3. Configure the project root**

- Go to **Edit > Preferences > Add-ons** and find **Physics Engine**
- Set **Project Root** to the absolute path of this repository (e.g. `/home/user/Physics-Engine`)
- This allows the addon to locate `interface/nbody.py` and the compiled library at runtime

### Usage

With the addon enabled, select any object and open the **Physics** tab in the Properties panel. An **N-Body** button will appear alongside the built-in physics types. Enabling it marks the object as an N-Body participant and exposes its **Mass** and **Initial Velocity** for use in simulation.

---

## Roadmap

- Optimized matrix operations replacing naive implementations
- Core rigid body mechanics
- Benchmarking against existing engines
- Soft-body dynamics and constraint systems
- Additional Rendering pipeline integrations

---

## License

Licensed under the [GNU General Public License v3.0](./LICENSE). Derivative works must carry the same license.

---

## Contributing

Not open for contributions yet. If you want to discuss the project or have ideas, open an issue or reach out directly. See [CONTRIBUTING.md](./CONTRIBUTING.md) for more details.
