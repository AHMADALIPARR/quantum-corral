/* include/corral.h - Shared C11 / Fortran / tinyAPL / Vulkan ABI
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#ifndef CORRAL_H
#define CORRAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CORRAL_MAX_RANK 8

typedef enum {
    CORRAL_F64 = 0,
    CORRAL_F32 = 1,
    CORRAL_C128 = 2,
    CORRAL_I64 = 3
} CorralDType;

typedef enum {
    CORRAL_OWNED = 0,
    CORRAL_BORROWED = 1
} CorralOwnership;

typedef enum {
    CORRAL_LAYOUT_ROW_MAJOR = 0,
    CORRAL_LAYOUT_COL_MAJOR = 1
} CorralLayout;

typedef struct {
    void *data;
    int32_t rank;
    int64_t shape[CORRAL_MAX_RANK];
    int64_t strides[CORRAL_MAX_RANK]; /* element strides */
    int64_t numel;
    CorralDType dtype;
    CorralOwnership ownership;
    CorralLayout layout;
} CorralArray;

/* Runtime array lifecycle & layout */
CorralArray corral_array_alloc(int32_t rank, const int64_t *shape, CorralDType dtype, CorralLayout layout);
CorralArray corral_array_wrap(void *ptr, int32_t rank, const int64_t *shape, const int64_t *strides,
                              CorralDType dtype, CorralLayout layout);
void corral_array_free(CorralArray *arr);
int corral_array_transpose_layout(const CorralArray *src, CorralArray *dst);

/* Native numerical kernel contracts (C11 / Fortran BIND(C) compatible) */
/* C row-major FP64 fields, length nx * ny */
void corral_density(
    const double *restrict real_field,
    const double *restrict imag_field,
    double *restrict density,
    int nx,
    int ny
);

void corral_laplacian_2d(
    const double *restrict re_in,
    const double *restrict im_in,
    double *restrict lap_re,
    double *restrict lap_im,
    int nx,
    int ny,
    double dx,
    double dy
);

void corral_rk4_step(
    double *restrict re,
    double *restrict im,
    const double *restrict pot_v,
    int nx,
    int ny,
    double dx,
    double dy,
    double dt
);

double corral_total_probability(
    const double *restrict density,
    int nx,
    int ny,
    double dx,
    double dy
);

#ifdef __cplusplus
}
#endif
#endif /* CORRAL_H */
