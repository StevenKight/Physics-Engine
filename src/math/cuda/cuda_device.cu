/**
 * @file cuda_device.cu
 * @brief Runtime CUDA device availability check.
 *
 * @author Steven Kight
 * @date 2026-09-26
 */

#include "cuda_matrix.h"

extern "C" bool cuda_device_available(void) {
    int device_count = 0;
    cudaError_t err = cudaGetDeviceCount(&device_count);
    return err == cudaSuccess && device_count > 0;
}
