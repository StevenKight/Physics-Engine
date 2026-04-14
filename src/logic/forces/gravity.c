/**
 * @file gravity.c
 * @brief Implementation of gravitational force calculations.
 *
 * Implements pairwise Newtonian gravity across an array of physics objects.
 * The gravitational constant G is defined locally and applied to each
 * object pair to accumulate net force vectors.
 *
 * @author Steven Kight
 * @date 2026-04-09
 */

#include "gravity.h"
#include "../../math/matrix.h"
#include "../../models/object.h"

#include <stdbool.h>
#include <stdlib.h>

const double power = 2.0;
const double half = 0.5;
const double g = 6.67430e-11;

// Bodies at or below this count use the Fortran (CPU) backend. Above it, the
// CUDA (GPU) backend is used. Tune by benchmarking newtonian_gravity() around
// the boundary on target hardware — CUDA overhead (~26 ops × H↔D transfer per
// call) dominates for small N where each N×N matrix is tiny.
static const int GRAVITY_GPU_THRESHOLD = 64;

// Returns true if the CUDA backend should be used for an N-body gravity
// calculation. When USE_CUDA is not defined this always returns false.
static bool gravity_use_gpu(int count) {
#ifdef USE_CUDA
    return count > GRAVITY_GPU_THRESHOLD;
#else
    (void)count;
    return false;
#endif
}

/**
 * Computes pairwise displacement matrices ΔX, ΔY, ΔZ (see §Force Direction Vector
 * — Matrix Derivations) where each element is the signed difference:
 *
 *   ΔX[i,j] = x_j - x_i   (likewise for y and z)
 *
 * Computed via broadcasting outer products (see §Distance Calculation — Optimization):
 *
 *   xi = x * 1^T   →  xi[i,j] = x_i   (x broadcast across columns)
 *   xj = 1 * x^T   →  xj[i,j] = x_j   (x broadcast across rows)
 *   ΔX = xj − xi   →  ΔX[i,j] = x_j − x_i
 *
 * The sign convention here means ΔX[i,j] points from body i toward body j,
 * which is the correct direction for the attractive gravitational force on body i.
 *
 * The caller owns the returned data pointers and must free them.
 */
static void compute_displacements(const PhysicsObject *objects, int count, bool use_gpu,
                                   double **dx_out, double **dy_out, double **dz_out) {
    double *x_data    = malloc(count * sizeof(double));
    double *y_data    = malloc(count * sizeof(double));
    double *z_data    = malloc(count * sizeof(double));
    double *ones_data = malloc(count * sizeof(double));

    for (int i = 0; i < count; i++) {
        x_data[i]    = objects[i].position.x;
        y_data[i]    = objects[i].position.y;
        z_data[i]    = objects[i].position.z;
        ones_data[i] = 1.0;
    }

    Matrix x_col    = { .rows = count, .cols = 1,     .data = x_data    };
    Matrix x_row    = { .rows = 1,     .cols = count, .data = x_data    };
    Matrix y_col    = { .rows = count, .cols = 1,     .data = y_data    };
    Matrix y_row    = { .rows = 1,     .cols = count, .data = y_data    };
    Matrix z_col    = { .rows = count, .cols = 1,     .data = z_data    };
    Matrix z_row    = { .rows = 1,     .cols = count, .data = z_data    };
    Matrix ones_col = { .rows = count, .cols = 1,     .data = ones_data };
    Matrix ones_row = { .rows = 1,     .cols = count, .data = ones_data };

    // Allocate N×N broadcast and difference matrices for each axis
    double *xi_data = calloc(count * count, sizeof(double));
    double *xj_data = calloc(count * count, sizeof(double));
    double *dx_data = calloc(count * count, sizeof(double));
    double *yi_data = calloc(count * count, sizeof(double));
    double *yj_data = calloc(count * count, sizeof(double));
    double *dy_data = calloc(count * count, sizeof(double));
    double *zi_data = calloc(count * count, sizeof(double));
    double *zj_data = calloc(count * count, sizeof(double));
    double *dz_data = calloc(count * count, sizeof(double));

    Matrix xi = { count, count, xi_data }; // [i,j] = x_i  (x * 1^T)
    Matrix xj = { count, count, xj_data }; // [i,j] = x_j  (1 * x^T)
    Matrix dx = { count, count, dx_data }; // [i,j] = x_j - x_i  (ΔX)
    Matrix yi = { count, count, yi_data };
    Matrix yj = { count, count, yj_data };
    Matrix dy = { count, count, dy_data };
    Matrix zi = { count, count, zi_data };
    Matrix zj = { count, count, zj_data };
    Matrix dz = { count, count, dz_data };

    // Broadcast each coordinate into N×N matrices, then subtract to get ΔX/ΔY/ΔZ.
    // xi = x * 1^T  (each row is x^T),  xj = 1 * x^T  (each column is x).
    matrix_mul(&x_col, &ones_row, &xi, use_gpu);  // xi[i,j] = x_i
    matrix_mul(&ones_col, &x_row, &xj, use_gpu);  // xj[i,j] = x_j
    matrix_sub(&xj, &xi, &dx, use_gpu);           // ΔX[i,j] = x_j - x_i

    matrix_mul(&y_col, &ones_row, &yi, use_gpu);
    matrix_mul(&ones_col, &y_row, &yj, use_gpu);
    matrix_sub(&yj, &yi, &dy, use_gpu);           // ΔY[i,j] = y_j - y_i

    matrix_mul(&z_col, &ones_row, &zi, use_gpu);
    matrix_mul(&ones_col, &z_row, &zj, use_gpu);
    matrix_sub(&zj, &zi, &dz, use_gpu);           // ΔZ[i,j] = z_j - z_i

    free(x_data);  free(y_data);  free(z_data);  free(ones_data);
    free(xi_data); free(xj_data);
    free(yi_data); free(yj_data);
    free(zi_data); free(zj_data);

    *dx_out = dx_data;
    *dy_out = dy_data;
    *dz_out = dz_data;
}

/**
 * Computes the N×N matrix of scalar gravitational force magnitudes.
 *
 * Implements the matrix final form (see §Matrix Final Form):
 *
 *   F_{N×N} = G · ((m_n × m_n^T) ⊘ r²_safe) ⊙ (J − I)
 *
 * where:
 *   m_n × m_n^T  — outer product giving m_i * m_j for every pair (i, j)
 *   r²_safe      — element-wise squared distances with identity added to
 *                  prevent division by zero on the diagonal (r²_safe = r² + I)
 *   ⊙ (J − I)   — Hadamard mask that zeros self-interaction entries
 *
 * @param objects    Input array of physics objects.
 * @param count      Number of objects (N).
 * @param force_data Pre-allocated N×N row-major buffer. Receives the scalar
 *                   force magnitude between every pair of bodies.
 * @param use_gpu    If true, use the CUDA backend; otherwise Fortran.
 */
void newtonian_gravity_forces(const PhysicsObject *objects, int count, double *force_data, bool use_gpu) {

    // ── Step 1: mass product  m_n × m_n^T  (N×N) ────────────────────────────
    // Produces mass_prod[i,j] = m_i * m_j for every body pair.
    double *col_data = malloc(count * sizeof(double));
    double *row_data = malloc(count * sizeof(double));

    for (int i = 0; i < count; i++) {
        col_data[i] = objects[i].mass;
        row_data[i] = objects[i].mass;
    }

    Matrix mass_col = { .rows = count, .cols = 1, .data = col_data }; // Nx1
    Matrix mass_row = { .rows = 1, .cols = count, .data = row_data }; // 1xN

    double *prod_data = calloc(count * count, sizeof(double));
    Matrix mass_prod = { .rows = count, .cols = count, .data = prod_data }; // NxN

    matrix_mul(&mass_col, &mass_row, &mass_prod, use_gpu);

    free(col_data);
    free(row_data);

    // ── Step 2: squared distances  r²[i,j] = Δx² + Δy² + Δz²  (N×N) ────────
    // Sign of ΔX/ΔY/ΔZ doesn't matter here since everything is squared.
    double *dx_data, *dy_data, *dz_data;
    compute_displacements(objects, count, use_gpu, &dx_data, &dy_data, &dz_data);

    double *dx_s_data = calloc(count * count, sizeof(double));
    double *dy_s_data = calloc(count * count, sizeof(double));
    double *dz_s_data = calloc(count * count, sizeof(double));

    Matrix dx = { count, count, dx_data };
    Matrix dy = { count, count, dy_data };
    Matrix dz = { count, count, dz_data };
    Matrix dx_s = { count, count, dx_s_data };
    Matrix dy_s = { count, count, dy_s_data };
    Matrix dz_s = { count, count, dz_s_data };

    matrix_power(&dx, &power, &dx_s, use_gpu);
    matrix_power(&dy, &power, &dy_s, use_gpu);
    matrix_power(&dz, &power, &dz_s, use_gpu);

    double *xy_dist_data = calloc(count * count, sizeof(double));
    double *dist_data    = calloc(count * count, sizeof(double));

    Matrix xy_dist  = { count, count, xy_dist_data };
    Matrix distances = { count, count, dist_data };

    matrix_add(&dx_s, &dy_s, &xy_dist, use_gpu);
    matrix_add(&xy_dist, &dz_s, &distances, use_gpu);

    free(dx_data);   free(dy_data);   free(dz_data);
    free(dx_s_data); free(dy_s_data); free(dz_s_data);
    free(xy_dist_data);

    // ── Step 3: numerical safety  r²_safe = r² + I ──────────────────────────
    // Adding I replaces each zero diagonal entry with 1, making division
    // well-defined. The ⊙(J−I) mask in step 5 zeroes those entries afterward.
    double *identity_data = calloc(count * count, sizeof(double));
    for (int i = 0; i < count; i++) {
        identity_data[i * count + i] = 1.0;
    }
    Matrix identity = { count, count, identity_data };

    double *safe_dist_data = calloc(count * count, sizeof(double));
    Matrix safe_distances = { count, count, safe_dist_data };

    matrix_add(&distances, &identity, &safe_distances, use_gpu);

    free(identity_data);
    free(dist_data);

    // ── Step 4: (m_n × m_n^T) ⊘ r²_safe, then scale by G ───────────────────
    double *mass_dist_data = calloc(count * count, sizeof(double));
    Matrix mass_distance = { count, count, mass_dist_data };

    matrix_div(&mass_prod, &safe_distances, &mass_distance, use_gpu);

    free(prod_data);
    free(safe_dist_data);

    Matrix forces = { count, count, force_data };
    matrix_scalar_mul(&mass_distance, &g, &forces, use_gpu);
    free(mass_dist_data);

    // ── Step 5: ⊙ (J − I) — zero diagonal (self-interaction) entries ─────────
    for (int i = 0; i < count; i++) {
        force_data[i * count + i] = 0.0;
    }
}

/**
 * Computes the N×N unit direction tensor D_hat (see §Force Direction Vector
 * — Final Form) where each element is the unit vector pointing from body i
 * toward body j:
 *
 *   D[i,j]      = [ΔX[i,j], ΔY[i,j], ΔZ[i,j]]   (displacement vector)
 *   r[i,j]      = sqrt(ΔX² + ΔY² + ΔZ²)           (scalar distance)
 *   D_hat[i,j]  = D[i,j] / r[i,j]                  (unit direction)
 *
 * Numerical safety: r²_safe = r² + I prevents 0/0 on the diagonal.
 * Because ΔX[i,i] = 0, the diagonal of D_hat is 0/sqrt(1) = 0, which is
 * consistent with the force matrix's zeroed diagonal.
 *
 * @param objects    Input array of physics objects.
 * @param count      Number of objects (N).
 * @param directions Pre-allocated Matrix3 of N×N component matrices (x, y, z)
 *                   that will receive the unit direction vectors.
 * @param use_gpu    If true, use the CUDA backend; otherwise Fortran.
 */
void newtonian_gravity_directions(const PhysicsObject *objects, int count, Matrix3 *directions, bool use_gpu) {

    // ── Step 1: displacement components  ΔX, ΔY, ΔZ  (N×N) ─────────────────
    // ΔX[i,j] = x_j - x_i  (points from body i toward body j)
    double *dx_data, *dy_data, *dz_data;
    compute_displacements(objects, count, use_gpu, &dx_data, &dy_data, &dz_data);

    // ── Step 2: squared distances  r²[i,j] = ΔX² + ΔY² + ΔZ²  (N×N) ───────
    double *dx_s_data = calloc(count * count, sizeof(double));
    double *dy_s_data = calloc(count * count, sizeof(double));
    double *dz_s_data = calloc(count * count, sizeof(double));

    Matrix dx = { count, count, dx_data };
    Matrix dy = { count, count, dy_data };
    Matrix dz = { count, count, dz_data };
    Matrix dx_s = { count, count, dx_s_data };
    Matrix dy_s = { count, count, dy_s_data };
    Matrix dz_s = { count, count, dz_s_data };

    matrix_power(&dx, &power, &dx_s, use_gpu);
    matrix_power(&dy, &power, &dy_s, use_gpu);
    matrix_power(&dz, &power, &dz_s, use_gpu);

    double *xy_dist_data = calloc(count * count, sizeof(double));
    double *dist_data    = calloc(count * count, sizeof(double));

    Matrix xy_dist   = { count, count, xy_dist_data };
    Matrix distances = { count, count, dist_data };

    matrix_add(&dx_s, &dy_s, &xy_dist, use_gpu);
    matrix_add(&xy_dist, &dz_s, &distances, use_gpu);

    free(dx_s_data); free(dy_s_data); free(dz_s_data);
    free(xy_dist_data);

    // ── Step 3: numerical safety  r²_safe = r² + I ──────────────────────────
    double *identity_data = calloc(count * count, sizeof(double));
    for (int i = 0; i < count; i++) {
        identity_data[i * count + i] = 1.0;
    }
    Matrix identity = { count, count, identity_data };

    double *safe_dist_data = calloc(count * count, sizeof(double));
    Matrix safe_dist = { count, count, safe_dist_data };

    matrix_add(&distances, &identity, &safe_dist, use_gpu);
    free(identity_data);
    free(dist_data);

    // ── Step 4: r = sqrt(r²_safe),  then D_hat[i,j] = D[i,j] / r[i,j] ──────
    double *r_data = calloc(count * count, sizeof(double));
    Matrix r_mat = { count, count, r_data };

    matrix_power(&safe_dist, &half, &r_mat, use_gpu);
    free(safe_dist_data);

    // Divide each displacement component by r to obtain the unit direction.
    // Output matrices are pre-allocated N×N and owned by the caller.
    matrix_div(&dx, &r_mat, &directions->x, use_gpu);
    matrix_div(&dy, &r_mat, &directions->y, use_gpu);
    matrix_div(&dz, &r_mat, &directions->z, use_gpu);

    free(dx_data); free(dy_data); free(dz_data);
    free(r_data);
}

void newtonian_gravity(const PhysicsObject *objects, int count,
                       Vec3 *forces_out) {

    const bool use_gpu = gravity_use_gpu(count);

    // ── Stage 1: scalar force magnitudes  F[i,j]  (N×N) ─────────────────────
    double *force_data = calloc(count * count, sizeof(double));
    newtonian_gravity_forces(objects, count, force_data, use_gpu);

    // ── Stage 2: unit direction tensor  D_hat[i,j]  (N×N×3) ─────────────────
    double *dir_x_data = calloc(count * count, sizeof(double));
    double *dir_y_data = calloc(count * count, sizeof(double));
    double *dir_z_data = calloc(count * count, sizeof(double));

    Matrix3 directions = {
        .x = { count, count, dir_x_data },
        .y = { count, count, dir_y_data },
        .z = { count, count, dir_z_data },
    };

    newtonian_gravity_directions(objects, count, &directions, use_gpu);

    // ── Stage 3: F_vec[i,j] = F[i,j] ⊙ D_hat[i,j]  (Hadamard per component) ─
    Matrix forces = { count, count, force_data };

    double *fvec_x_data = calloc(count * count, sizeof(double));
    double *fvec_y_data = calloc(count * count, sizeof(double));
    double *fvec_z_data = calloc(count * count, sizeof(double));

    Matrix fvec_x = { count, count, fvec_x_data };
    Matrix fvec_y = { count, count, fvec_y_data };
    Matrix fvec_z = { count, count, fvec_z_data };

    matrix_hadamard(&forces, &directions.x, &fvec_x, use_gpu);
    matrix_hadamard(&forces, &directions.y, &fvec_y, use_gpu);
    matrix_hadamard(&forces, &directions.z, &fvec_z, use_gpu);

    free(force_data);
    free(dir_x_data); free(dir_y_data); free(dir_z_data);

    // ── Stage 4: F(i) = Σ_j F_vec[i,j]  (row sum → net force per body) ──────
    double *sum_x_data = calloc(count, sizeof(double));
    double *sum_y_data = calloc(count, sizeof(double));
    double *sum_z_data = calloc(count, sizeof(double));

    Matrix sum_x = { count, 1, sum_x_data };
    Matrix sum_y = { count, 1, sum_y_data };
    Matrix sum_z = { count, 1, sum_z_data };

    matrix_row_sum(&fvec_x, &sum_x, use_gpu);
    matrix_row_sum(&fvec_y, &sum_y, use_gpu);
    matrix_row_sum(&fvec_z, &sum_z, use_gpu);

    free(fvec_x_data); free(fvec_y_data); free(fvec_z_data);

    for (int i = 0; i < count; i++) {
        forces_out[i].x = sum_x_data[i];
        forces_out[i].y = sum_y_data[i];
        forces_out[i].z = sum_z_data[i];
    }

    free(sum_x_data);
    free(sum_y_data);
    free(sum_z_data);
}
