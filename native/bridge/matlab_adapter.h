/* native/bridge/matlab_adapter.h - MATLAB Engine API Bridge
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#ifndef CORRAL_MATLAB_ADAPTER_H
#define CORRAL_MATLAB_ADAPTER_H

#include "corral.h"

/* Calls matlab/corral_field.m via MATLAB Engine API (converting row-major <-> col-major),
 * or invokes the native C11/Fortran fallback when MATLAB Engine is not linked. */
int corral_matlab_eval_density(
    const CorralArray *re_row_major,
    const CorralArray *im_row_major,
    CorralArray *rho_row_major
);

#endif /* CORRAL_MATLAB_ADAPTER_H */
