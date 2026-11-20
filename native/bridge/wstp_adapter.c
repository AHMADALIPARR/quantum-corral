/* native/bridge/wstp_adapter.c - Wolfram WSTP Adapter
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#define _DEFAULT_SOURCE
#include "wstp_adapter.h"
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef USE_WSTP
#include "wstp.h"
#endif

static const double Z01 = 2.40482555769577276862;
static const double Z11 = 3.83170597020751231561;

int corral_wstp_eval_superposition(
    const CorralSymbolicParams *params,
    CorralArray *re_out,
    CorralArray *im_out
) {
    if (!params || !re_out || !im_out) return -1;
    if (re_out->rank != 2 || im_out->rank != 2) return -1;

    int64_t nx = re_out->shape[0];
    int64_t ny = re_out->shape[1];
    double *re = (double *)re_out->data;
    double *im = (double *)im_out->data;
    if (!re || !im) return -1;

#ifdef USE_WSTP
    WSENV env = WSInitialize((WSEnvironmentParameter)0);
    if (env) {
        int err = 0;
        WSLINK lp = WSOpenString(env, "-linkname 'wolfram -wstp' -linkmode launch", &err);
        if (lp) {
            WSPutFunction(lp, "EvaluatePacket", 1);
            WSPutFunction(lp, "N", 1);
            WSPutFunction(lp, "CorralSuperposition", 6);
            WSPutReal64(lp, params->a1);
            WSPutReal64(lp, params->a2);
            WSPutReal64(lp, params->phase_deg);
            WSPutReal64(lp, params->radius);
            WSPutInteger64(lp, nx);
            WSPutInteger64(lp, ny);
            WSEndPacket(lp);
            WSClose(lp);
        }
        WSDeinitialize(env);
    }
#endif

    double norm_c = sqrt(fmax(params->a1 * params->a1 + params->a2 * params->a2, 1e-12));
    double c1 = params->a1 / norm_c;
    double c2 = params->a2 / norm_c;
    double phi = params->phase_deg * M_PI / 180.0;
    double c2_re = c2 * cos(phi);
    double c2_im = c2 * sin(phi);

    double dx = (2.0 * params->domain_L) / (double)(nx - 1);
    double dy = (2.0 * params->domain_L) / (double)(ny - 1);

    for (int64_t y = 0; y < ny; ++y) {
        double yy = -params->domain_L + (double)y * dy;
        for (int64_t x = 0; x < nx; ++x) {
            double xx = -params->domain_L + (double)x * dx;
            double r = hypot(xx, yy);
            int64_t idx = y * nx + x;
            if (r <= params->radius) {
                double theta = atan2(yy, xx);
                double m0 = j0(Z01 * r / params->radius);
                double m1 = j1(Z11 * r / params->radius) * cos(theta);
                re[idx] = c1 * m0 + c2_re * m1;
                im[idx] = c2_im * m1;
            } else {
                re[idx] = 0.0;
                im[idx] = 0.0;
            }
        }
    }
    return 0;
}
