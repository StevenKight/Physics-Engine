Contributing to the High-Performance Physics Engine
===================================================

Thank you for your interest in contributing! This project is a high-performance research engine written in C/C++ with CUDA and Fortran. It is currently in active development and contributions are welcome, especially in the following areas:

*   Optimized numerical methods and algorithms
*   Matrix operation kernels in CUDA or Fortran
*   Performance benchmarking tools
*   Documentation and derivation clarity
*   Test cases and validation strategies

Ground Rules
------------

*   All contributions must be **GPLv3-compatible**.
*   Contributions should aim to be **well-documented** and **reproducible**.
*   If you’re proposing major changes, **please open an issue** first to discuss the scope and impact.

How to Contribute
-----------------

1.  **Fork** the repository and create your branch from `main`:
    
        git checkout -b feature/my-feature

2.  Write clear, clean code with comments where appropriate. Keep performance in mind.
4.  **Lint and format** your code where relevant.
5.  Submit a **pull request** and clearly explain:
    *   What the contribution does
    *   Why it's useful
    *   How it was tested

Code Style Guidelines
---------------------

*   **C/C++**: Follow the [LLVM style guide](https://llvm.org/docs/CodingStandards.html) unless noted otherwise.
*   **Fortran**: Prefer modern Fortran (90+) with explicit interfaces.
*   **CUDA**: Prioritize memory access patterns and shared memory reuse.

Code of Conduct
---------------

Please be respectful and constructive in all interactions. Harassment or discriminatory behavior of any kind will not be tolerated.

Questions?
----------

Open an issue if you have a question or an issue with the code.
