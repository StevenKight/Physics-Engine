#!/usr/bin/env python3
"""
Package the Blender addon into physics_engine.zip.

Run from the project root:
    python3 scripts/package_blender.py
or via:
    make package
"""

import zipfile
from pathlib import Path

ROOT = Path(__file__).parent.parent
BLENDER_DIR = ROOT / "blender"
OUTPUT = ROOT / "physics_engine.zip"

with zipfile.ZipFile(OUTPUT, "w") as z:
    for path in sorted(BLENDER_DIR.rglob("*")):
        if path.is_file() and "__pycache__" not in path.parts:
            z.write(path, Path("physics_engine") / path.relative_to(BLENDER_DIR))

print(f"Created {OUTPUT}")
