/* native/runtime/corral_runtime.c - C11 Native Runtime / ABI
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "corral.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static size_t dtype_size(CorralDType dt) {
    switch (dt) {
        case CORRAL_F64:  return sizeof(double);
        case CORRAL_F32:  return sizeof(float);
        case CORRAL_C128: return 2 * sizeof(double);
        case CORRAL_I64:  return sizeof(int64_t);
        default:          return sizeof(double);
    }
}

static void compute_strides(int32_t rank, const int64_t *shape, CorralLayout layout, int64_t *strides) {
    if (rank <= 0) return;
    if (layout == CORRAL_LAYOUT_ROW_MAJOR) {
        int64_t acc = 1;
        for (int32_t i = rank - 1; i >= 0; --i) {
            strides[i] = acc;
            acc *= shape[i];
        }
    } else {
        int64_t acc = 1;
        for (int32_t i = 0; i < rank; ++i) {
            strides[i] = acc;
            acc *= shape[i];
        }
    }
}

CorralArray corral_array_alloc(int32_t rank, const int64_t *shape, CorralDType dtype, CorralLayout layout) {
    CorralArray arr;
    memset(&arr, 0, sizeof(arr));
    if (rank < 0 || rank > CORRAL_MAX_RANK) return arr;

    arr.rank = rank;
    arr.dtype = dtype;
    arr.ownership = CORRAL_OWNED;
    arr.layout = layout;
    arr.numel = 1;

    for (int32_t i = 0; i < rank; ++i) {
        arr.shape[i] = shape[i];
        arr.numel *= shape[i];
    }
    compute_strides(rank, arr.shape, layout, arr.strides);

    size_t nbytes = (size_t)arr.numel * dtype_size(dtype);
    arr.data = calloc(1, nbytes > 0 ? nbytes : 1);
    return arr;
}

CorralArray corral_array_wrap(void *ptr, int32_t rank, const int64_t *shape, const int64_t *strides,
                              CorralDType dtype, CorralLayout layout) {
    CorralArray arr;
    memset(&arr, 0, sizeof(arr));
    if (rank < 0 || rank > CORRAL_MAX_RANK) return arr;

    arr.data = ptr;
    arr.rank = rank;
    arr.dtype = dtype;
    arr.ownership = CORRAL_BORROWED;
    arr.layout = layout;
    arr.numel = 1;

    for (int32_t i = 0; i < rank; ++i) {
        arr.shape[i] = shape[i];
        arr.numel *= shape[i];
    }
    if (strides) {
        for (int32_t i = 0; i < rank; ++i) arr.strides[i] = strides[i];
    } else {
        compute_strides(rank, arr.shape, layout, arr.strides);
    }
    return arr;
}

void corral_array_free(CorralArray *arr) {
    if (!arr) return;
    if (arr->ownership == CORRAL_OWNED && arr->data) {
        free(arr->data);
    }
    arr->data = NULL;
    arr->numel = 0;
}

int corral_array_transpose_layout(const CorralArray *src, CorralArray *dst) {
    if (!src || !dst || src->rank != 2 || src->dtype != CORRAL_F64) return -1;
    int64_t nx = src->shape[0];
    int64_t ny = src->shape[1];
    const double *s = (const double *)src->data;
    double *d = (double *)dst->data;
    if (!s || !d) return -1;

    for (int64_t i = 0; i < nx; ++i) {
        for (int64_t j = 0; j < ny; ++j) {
            int64_t s_idx = i * src->strides[0] + j * src->strides[1];
            int64_t d_idx = i * dst->strides[0] + j * dst->strides[1];
            d[d_idx] = s[s_idx];
        }
    }
    return 0;
}

/* C11 fallback/reference implementations when Fortran kernels are not linked */
#ifndef USE_FORTRAN_KERNELS
void corral_density(
    const double *restrict real_field,
    const double *restrict imag_field,
    double *restrict density,
    int nx,
    int ny
) {
    const int n = nx * ny;
    for (int i = 0; i < n; ++i) {
        double r = real_field[i];
        double m = imag_field[i];
        density[i] = r * r + m * m;
    }
}

void corral_laplacian_2d(
    const double *restrict re_in,
    const double *restrict im_in,
    double *restrict lap_re,
    double *restrict lap_im,
    int nx,
    int ny,
    double dx,
    double dy
) {
    const double idx2 = 1.0 / (dx * dx);
    const double idy2 = 1.0 / (dy * dy);
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            int idx = y * nx + x;
            if (x == 0 || x == nx - 1 || y == 0 || y == ny - 1) {
                lap_re[idx] = 0.0;
                lap_im[idx] = 0.0;
            } else {
                lap_re[idx] = (re_in[idx + 1] + re_in[idx - 1] - 2.0 * re_in[idx]) * idx2 +
                              (re_in[idx + nx] + re_in[idx - nx] - 2.0 * re_in[idx]) * idy2;
                lap_im[idx] = (im_in[idx + 1] + im_in[idx - 1] - 2.0 * im_in[idx]) * idx2 +
                              (im_in[idx + nx] + im_in[idx - nx] - 2.0 * im_in[idx]) * idy2;
            }
        }
    }
}

static void tdse_rhs(const double *re, const double *im, const double *pot_v,
                     double *dre, double *dim, int nx, int ny, double dx, double dy) {
    const double idx2 = 1.0 / (dx * dx);
    const double idy2 = 1.0 / (dy * dy);
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            int idx = y * nx + x;
            if (x == 0 || x == nx - 1 || y == 0 || y == ny - 1) {
                dre[idx] = 0.0;
                dim[idx] = 0.0;
                continue;
            }
            double lre = (re[idx + 1] + re[idx - 1] - 2.0 * re[idx]) * idx2 +
                         (re[idx + nx] + re[idx - nx] - 2.0 * re[idx]) * idy2;
            double lim = (im[idx + 1] + im[idx - 1] - 2.0 * im[idx]) * idx2 +
                         (im[idx + nx] + im[idx - nx] - 2.0 * im[idx]) * idy2;
            double h_re = -0.5 * lre + pot_v[idx] * re[idx];
            double h_im = -0.5 * lim + pot_v[idx] * im[idx];
            /* d(psi)/dt = -i * H * psi => d(re)/dt = h_im, d(im)/dt = -h_re */
            dre[idx] = h_im;
            dim[idx] = -h_re;
        }
    }
}

void corral_rk4_step(
    double *restrict re,
    double *restrict im,
    const double *restrict pot_v,
    int nx,
    int ny,
    double dx,
    double dy,
    double dt
) {
    size_t n = (size_t)nx * (size_t)ny;
    double *buf = (double *)malloc(10 * n * sizeof(double));
    if (!buf) return;
    double *k1r = buf, *k1i = buf + n;
    double *k2r = buf + 2*n, *k2i = buf + 3*n;
    double *k3r = buf + 4*n, *k3i = buf + 5*n;
    double *k4r = buf + 6*n, *k4i = buf + 7*n;
    double *tr  = buf + 8*n, *ti  = buf + 9*n;

    tdse_rhs(re, im, pot_v, k1r, k1i, nx, ny, dx, dy);
    for (size_t i = 0; i < n; ++i) { tr[i] = re[i] + 0.5*dt*k1r[i]; ti[i] = im[i] + 0.5*dt*k1i[i]; }
    tdse_rhs(tr, ti, pot_v, k2r, k2i, nx, ny, dx, dy);
    for (size_t i = 0; i < n; ++i) { tr[i] = re[i] + 0.5*dt*k2r[i]; ti[i] = im[i] + 0.5*dt*k2i[i]; }
    tdse_rhs(tr, ti, pot_v, k3r, k3i, nx, ny, dx, dy);
    for (size_t i = 0; i < n; ++i) { tr[i] = re[i] + dt*k3r[i]; ti[i] = im[i] + dt*k3i[i]; }
    tdse_rhs(tr, ti, pot_v, k4r, k4i, nx, ny, dx, dy);

    for (size_t i = 0; i < n; ++i) {
        re[i] += (dt / 6.0) * (k1r[i] + 2.0*k2r[i] + 2.0*k3r[i] + k4r[i]);
        im[i] += (dt / 6.0) * (k1i[i] + 2.0*k2i[i] + 2.0*k3i[i] + k4i[i]);
    }
    free(buf);
}
#endif

double corral_total_probability(
    const double *restrict density,
    int nx,
    int ny,
    double dx,
    double dy
) {
    double sum = 0.0;
    const int n = nx * ny;
    for (int i = 0; i < n; ++i) sum += density[i];
    return sum * dx * dy;
}
