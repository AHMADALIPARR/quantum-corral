/* verification/numerical/test_corral_e2e.c - End-to-End Cross-Stack Verification
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "corral.h"
#include "../../native/bridge/wstp_adapter.h"
#include "../../native/bridge/matlab_adapter.h"
#include "../../vulkan/compute/vk_corral_compute.h"
#include "../../vulkan/renderer/vk_corral_renderer.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Declared in generated_corral.c (lowered from test_tinyapl_lowering.apl by tinyaplc) */
int tinyapl_corral_eval(const double *psi_re, const double *psi_im,
                        double *rho_out, double *total_prob_out,
                        int64_t nx, int64_t ny, double dx, double dy);

int main(void) {
    const int64_t N = 128;
    const double L = 1.25;
    const double dx = (2.0 * L) / (double)(N - 1);
    const double dy = dx;
    int64_t shape[2] = { N, N };

    CorralArray re = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);
    CorralArray im = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);
    CorralArray rho_tinyapl = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);
    CorralArray rho_matlab  = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);
    CorralArray rho_vulkan  = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);

    /* 1. Evaluate symbolic superposition (a1=0.99, a2=0.81, phase=190 deg) */
    CorralSymbolicParams params = { .a1 = 0.99, .a2 = 0.81, .phase_deg = 190.0, .radius = 1.0, .domain_L = L };
    assert(corral_wstp_eval_superposition(&params, &re, &im) == 0);

    /* 2. Execute tinyAPL-lowered C11 kernel */
    double p_total_tinyapl = 0.0;
    assert(tinyapl_corral_eval((double *)re.data, (double *)im.data,
                               (double *)rho_tinyapl.data, &p_total_tinyapl,
                               N, N, dx, dy) == 0);

    /* 3. Execute MATLAB Engine adapter */
    assert(corral_matlab_eval_density(&re, &im, &rho_matlab) == 0);

    /* 4. Execute Vulkan compute staging & dispatch */
    VkCorralComputeCtx vk_ctx = {0};
    assert(vk_corral_compute_init(&vk_ctx, (int32_t)N, (int32_t)N) == 0);
    assert(vk_corral_compute_dispatch(&vk_ctx, &re, &im, &rho_vulkan) == 0);
    assert(vk_corral_render_ppm(&vk_ctx, "corral_frame.ppm", 1.0 / L) == 0);

    /* Cross-verify tinyAPL(FP64), MATLAB(FP64), and Vulkan(FP32) */
    double max_err_matlab = 0.0;
    double max_err_vulkan = 0.0;
    const double *ra = (const double *)rho_tinyapl.data;
    const double *rm = (const double *)rho_matlab.data;
    const double *rv = (const double *)rho_vulkan.data;

    for (int64_t i = 0; i < N * N; ++i) {
        double em = fabs(ra[i] - rm[i]);
        double ev = fabs(ra[i] - rv[i]);
        if (em > max_err_matlab) max_err_matlab = em;
        if (ev > max_err_vulkan) max_err_vulkan = ev;
    }
    assert(max_err_matlab < 1e-12);
    assert(max_err_vulkan < 1e-5);
    printf("[PASS] Cross-stack density match: max|tinyAPL-MATLAB|=%.3e, max|tinyAPL-Vulkan|=%.3e\n",
           max_err_matlab, max_err_vulkan);

    /* 5. Verify probability conservation under RK4 TDSE time evolution */
    CorralArray pot_v = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);
    double *v = (double *)pot_v.data;
    for (int64_t y = 0; y < N; ++y) {
        double yy = -L + (double)y * dy;
        for (int64_t x = 0; x < N; ++x) {
            double xx = -L + (double)x * dx;
            double r = hypot(xx, yy);
            v[y * N + x] = (r > 0.95) ? 50.0 * (r - 0.95) * (r - 0.95) : 0.0;
        }
    }

    double p0 = p_total_tinyapl;
    for (int step = 0; step < 50; ++step) {
        corral_rk4_step((double *)re.data, (double *)im.data, v, (int)N, (int)N, dx, dy, 0.0002);
    }
    corral_density((double *)re.data, (double *)im.data, (double *)rho_tinyapl.data, (int)N, (int)N);
    double p1 = corral_total_probability((double *)rho_tinyapl.data, (int)N, (int)N, dx, dy);
    double rel_drift = fabs(p1 - p0) / p0;
    assert(rel_drift < 1e-5);
    printf("[PASS] TDSE probability conservation: P0=%.6f, P1=%.6f, rel_drift=%.3e\n", p0, p1, rel_drift);

    vk_corral_compute_destroy(&vk_ctx);
    corral_array_free(&pot_v);
    corral_array_free(&re);
    corral_array_free(&im);
    corral_array_free(&rho_tinyapl);
    corral_array_free(&rho_matlab);
    corral_array_free(&rho_vulkan);
    return 0;
}
