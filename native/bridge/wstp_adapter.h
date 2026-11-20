/* native/bridge/wstp_adapter.h - Wolfram WSTP Symbolic-to-Array Bridge
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#ifndef CORRAL_WSTP_ADAPTER_H
#define CORRAL_WSTP_ADAPTER_H

#include "corral.h"

typedef struct {
    double a1;
    double a2;
    double phase_deg;
    double radius;
    double domain_L;
} CorralSymbolicParams;

/* Evaluates two-mode circular corral wavefunction via WSTP (or C11 Bessel fallback)
 * into row-major C128/F64 buffers wrapped in CorralArray. */
int corral_wstp_eval_superposition(
    const CorralSymbolicParams *params,
    CorralArray *re_out,
    CorralArray *im_out
);

#endif /* CORRAL_WSTP_ADAPTER_H */
