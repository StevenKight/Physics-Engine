"""
Python interface to the physics_sim shared library via ctypes.

Build the library first:
    cmake -B build && cmake --build build
    # produces build/src/logic/nbody_sim.so

Usage:
    from interface.nbody import Vec3, PhysicsObject, sim_run

    sun   = PhysicsObject(mass=1.989e30, x=0, y=0, z=0)
    earth = PhysicsObject(mass=5.972e24, x=1.496e11, y=0, z=0,
                          vx=0, vy=29780, vz=0)

    objects = [sun, earth]
    sim_run(objects, time_step=3600, num_steps=24)  # 24 hours, hourly steps

    print(earth.position.x, earth.position.y, earth.position.z)
"""

import ctypes
import ctypes.util
from pathlib import Path


def _preload_cuda() -> None:
    # libnbody_sim.so embeds CUDA device code whose __cudaRegisterFatBinary
    # constructors fire at dlopen time. Those constructors require the CUDA
    # runtime AND driver to already have an active context, otherwise they
    # crash before sim_run is ever called.
    #
    # Fix: load libcudart.so globally, then call cudaFree(0) — the standard
    # idiom that forces the runtime to initialise the driver and create a
    # default context on device 0. Once that context exists, dlopen of our
    # .so completes safely.
    name = ctypes.util.find_library("cudart")
    if not name:
        return
    cudart = ctypes.CDLL(name, ctypes.RTLD_GLOBAL)
    cudart.cudaFree.restype  = ctypes.c_int
    cudart.cudaFree.argtypes = [ctypes.c_void_p]
    cudart.cudaFree(None)   # cudaFree(0) — initialise CUDA, ignore error code


def _load_lib() -> ctypes.CDLL:
    _preload_cuda()
    search = [
        Path(__file__).parent.parent / "build" / "src" / "logic" / "libnbody_sim.so",
        Path(__file__).parent.parent / "build" / "libnbody_sim.so",
    ]
    for path in search:
        if path.exists():
            return ctypes.CDLL(str(path), ctypes.RTLD_GLOBAL)
    raise FileNotFoundError(
        "nbody_sim.so not found. Build the project first:\n"
        "  cmake -B build && cmake --build build"
    )


# ── C struct mirrors ──────────────────────────────────────────────────────────

class Vec3(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_double),
        ("y", ctypes.c_double),
        ("z", ctypes.c_double),
    ]

    def __repr__(self) -> str:
        return f"Vec3(x={self.x}, y={self.y}, z={self.z})"


class PhysicsObject(ctypes.Structure):
    _fields_ = [
        ("mass",         ctypes.c_double),
        ("position",     Vec3),
        ("velocity",     Vec3),
        ("acceleration", Vec3),
        ("force",        Vec3),
    ]

    def __init__(
        self,
        mass: float,
        x: float = 0.0, y: float = 0.0, z: float = 0.0,
        vx: float = 0.0, vy: float = 0.0, vz: float = 0.0,
    ):
        super().__init__()
        self.mass         = mass
        self.position     = Vec3(x, y, z)
        self.velocity     = Vec3(vx, vy, vz)
        self.acceleration = Vec3(0.0, 0.0, 0.0)
        self.force        = Vec3(0.0, 0.0, 0.0)

    def __repr__(self) -> str:
        return (
            f"PhysicsObject(mass={self.mass}, "
            f"pos={self.position}, vel={self.velocity})"
        )


# ── Library binding ───────────────────────────────────────────────────────────

_lib = _load_lib()

_lib.sim_run.restype  = None
_lib.sim_run.argtypes = [
    ctypes.POINTER(PhysicsObject),  # objects
    ctypes.c_int,                   # count
    ctypes.c_double,                # time_step
    ctypes.c_int,                   # num_steps
]


def sim_run(
    objects: list[PhysicsObject],
    time_step: float,
    num_steps: int,
) -> None:
    """
    Run the simulation for num_steps ticks of size time_step (seconds).

    Mutates each PhysicsObject in place — read back position/velocity
    after the call.

    Args:
        objects:    List of PhysicsObject instances.
        time_step:  Duration of each tick in seconds.
        num_steps:  Number of ticks to simulate.
    """
    n = len(objects)
    arr = (PhysicsObject * n)(*objects)
    _lib.sim_run(arr, ctypes.c_int(n), ctypes.c_double(time_step), ctypes.c_int(num_steps))
    for i in range(n):
        objects[i] = arr[i]


if __name__ == "__main__":
    sun   = PhysicsObject(mass=1.989e30, x=0, y=0, z=0)
    earth = PhysicsObject(mass=5.972e24, x=1.496e11, y=0, z=0,
                          vx=0, vy=29780, vz=0)

    time_step = 3600.0    # 1 hour in seconds
    num_steps = 24        # 1 simulated day

    objects = [sun, earth]
    sim_run(objects, time_step, num_steps)

    earth = objects[1]
    print(f"Earth position after {num_steps} steps",
          f"({earth.position.x}, {earth.position.y}, {earth.position.z})")
