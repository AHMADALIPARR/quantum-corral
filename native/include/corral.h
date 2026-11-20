/* include/corral.h
 * Quantum Corral C ABI header
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under the Boost Software License 1.0 (BSL-1.0)
 * and Functional Source License 1.1 MIT Future (FSL-1.1-MIT)
 * with Competing Use restriction waived ("No competing clause").
 * See LICENSE and LICENSE.FSL in the repository root.
 *
 * C row-major FP64 fields, length nx * ny.
 * Declares the density kernel matching the Fortran and MATLAB interfaces.
 */

#ifndef CORRAL_H
#define CORRAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* C row-major FP64 fields, length nx * ny */
void corral_density(
    const double *restrict real_field,
    const double *restrict imag_field,
    double *restrict density,
    int nx,
    int ny
);

#ifdef __cplusplus
}
#endif
#endif /* CORRAL_H */
