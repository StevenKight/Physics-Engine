/**
 * @file matrix.cu
 * @brief Convenience aggregator for CUDA matrix operation implementations.
 *
 * This file includes the individual implementation units so the build can
 * compile all CUDA helpers together. The included files each contain their
 * own kernel and host-wrapper documentation.
 *
 * @author Steven Kight
 * @date 2025-10-09
 */

#include "matrix_mul.cu"
#include "matrix_add.cu"
#include "matrix_sub.cu"
#include "matrix_scalar.cu"
