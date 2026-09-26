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
3.  **Add or update tests** for your changes. All existing tests must continue to pass.
4.  **Lint and format** your code. C/C++/CUDA formatting is enforced via `.clang-format` (LLVM style), and static analysis runs through `.clang-tidy`. Install the [pre-commit](https://pre-commit.com) hooks once per clone so this happens automatically:

        pip install pre-commit
        pre-commit install

    This runs `clang-format` (and basic whitespace/large-file hygiene checks) on staged files at commit time. `clang-tidy` is not yet wired into pre-commit (it needs a generated `compile_commands.json`); run it manually against your changed files if your editor doesn't do it via `.clang-tidy` already:

        cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        clang-tidy -p build src/path/to/changed_file.c

    `.editorconfig` sets baseline indentation/whitespace per language (most editors pick this up automatically with no plugin needed for basic settings).
5.  Submit a **pull request** and clearly explain:
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
