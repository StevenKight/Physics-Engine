Contributing to the High-Performance Physics Engine
===================================================

This project is not yet open for external contributions. If you want to discuss the project or have ideas, please open an issue or reach out directly.

The guidelines below describe what contributions will look like when the project opens up.

Ground Rules
------------

*   All contributions must be **GPLv3-compatible**.
*   Contributions should aim to be **well-documented** and **reproducible**.
*   If you're proposing major changes, **please open an issue** first to discuss the scope and impact.

How to Contribute
-----------------

1.  **Fork** the repository and create your branch from `main`:

        git checkout -b feature/my-feature

2.  Write clear, clean code with comments where appropriate. Keep performance in mind.
3.  **Add or update tests** for your changes. All existing tests must continue to pass. Note that the matrix test suite exercises both the CPU/Fortran and GPU/CUDA backends of each operation; on a machine with no CUDA device (including CI, see below), the GPU half of those tests reports `SKIP` rather than `PASS`/`FAIL` — this is expected and not a failure.
4.  **Lint and format** your code. C/C++/CUDA formatting is enforced via `.clang-format` (LLVM style), and static analysis runs through `.clang-tidy`. Install the [pre-commit](https://pre-commit.com) hooks once per clone so this happens automatically:

        pip install pre-commit
        pre-commit install

    This runs `clang-format` (and basic whitespace/large-file hygiene checks) on staged files at commit time. `clang-tidy` is not yet wired into pre-commit (it needs a generated `compile_commands.json`); run it manually against your changed files if your editor doesn't do it via `.clang-tidy` already:

        cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        clang-tidy -p build src/path/to/changed_file.c

    `.editorconfig` sets baseline indentation/whitespace per language (most editors pick this up automatically with no plugin needed for basic settings).

    Installing the hooks locally is not enforced by git itself — `.git/hooks/` isn't tracked, so a fresh clone won't have them until you run `pre-commit install`. As a backstop, `.github/workflows/pre-commit.yml` reruns the same checks in CI on every push to `main` and pull request, so an unformatted change will still be caught even if the local hook was skipped.
5.  Two more CI workflows run on pushes to `main` and on pull requests (not on every branch push, so day-to-day pushes to a feature branch stay fast — they're checked once a PR is opened):
    *   **`build-test.yml`** builds the full project (`cmake` + `ctest`) on a pinned `ubuntu-24.04` runner with `gcc-12`/`g++-12`/`gfortran-12` installed and passed to CMake explicitly. Free runners have no GPU, so this only exercises the CPU/Fortran matrix backend and non-CUDA logic at runtime — the CUDA sources still have to compile since `CMakeLists.txt` requires the CUDA language unconditionally, but no kernel actually runs.
    *   **`cuda-compile.yml`** runs the same configure+build inside the `nvidia/cuda:12.4.1-devel-ubuntu22.04` container, with `gcc-11`/`g++-11`/`gfortran-11` (this image's native default) passed explicitly, as a dedicated compile-only sanity check for the CUDA sources. It does not run `ctest` at all.

    Both pass an explicit `CMAKE_CUDA_ARCHITECTURES` list (rather than the local-dev default of `native`, which queries a physical GPU that doesn't exist on the runner, or the `all-major` keyword, whose resolution during CMake's compiler-ID probe turned out to depend on the CMake version) and an explicit `CMAKE_CUDA_HOST_COMPILER` pointing at the matching `g++`, not `gcc` — nvcc needs the C++ frontend since the `.cu` sources use `<iostream>`.

    Toolchain versions are pinned deliberately rather than left to float: without this, a CI run today can silently use different GCC/CUDA/CMake versions than one made months from now once Ubuntu's or the runner image's defaults move on, changing what "passes" means without anyone deciding that. `ubuntu-latest` in particular already moved once this year (22.04 → 24.04) underneath these jobs. Bumping any of these pinned versions is a deliberate, visible diff in the workflow file, not a silent one.
6.  Submit a **pull request** and clearly explain:
    *   What the contribution does
    *   Why it's useful
    *   How it was tested

Code Style Guidelines
---------------------

*   **C/C++**: Follow the [LLVM style guide](https://llvm.org/docs/CodingStandards.html). The `.clang-format` file in the repository root enforces this automatically, and `.clang-tidy` enables `bugprone-*`, `clang-analyzer-*`, `performance-*`, `portability-*`, and `readability-*` static-analysis checks.
*   **Fortran**: Prefer modern Fortran (90+) with explicit interfaces.
*   **CUDA**: Prioritize memory access patterns and shared memory reuse.

Code of Conduct
---------------

Please be respectful and constructive in all interactions. Harassment or discriminatory behavior of any kind will not be tolerated.

Questions?
----------

Open an issue if you have a question or an issue with the code.
