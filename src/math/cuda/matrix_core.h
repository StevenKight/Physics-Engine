/**
 * @file matrix_core.h
 * @brief Core matrix type used by CUDA implementations.
 *
 * This header defines the simple Matrix struct used across the CUDA helper
 * implementation files. It's intentionally minimal to keep the C ABI simple
 * for use from both host C/C++ code and device code wrappers.
 *
 * The Matrix struct contains a row and column dimension and a pointer to a
 * contiguous float array in host memory. Device-side functions in this
 * project expect host-side allocations to be copied to/from device buffers
 * before/after kernel execution.
 *
 * @author Steven Kight
 * @date 2025-10-09
 */

#pragma once
#include <cuda_runtime.h>
#include <stdlib.h>

/**
 * @brief Simple matrix container for host/device interop.
 *
 * rows: number of rows in the matrix.
 * cols: number of columns in the matrix.
 * data: pointer to contiguous row-major float data in host memory.
 */
typedef struct {
    int rows;
    int cols;
    float *data;
} Matrix;
