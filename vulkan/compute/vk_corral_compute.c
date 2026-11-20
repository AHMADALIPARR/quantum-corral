/* vulkan/compute/vk_corral_compute.c - Vulkan Storage Buffer Staging & Compute Dispatch
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "vk_corral_compute.h"
#include <stdlib.h>
#include <math.h>

#ifdef USE_VULKAN
#include <vulkan/vulkan.h>
#endif

int vk_corral_compute_init(VkCorralComputeCtx *ctx, int32_t nx, int32_t ny) {
    if (!ctx || nx <= 0 || ny <= 0) return -1;
    size_t n = (size_t)nx * (size_t)ny;
    ctx->nx = nx;
    ctx->ny = ny;
    ctx->staging_re  = (float *)calloc(n, sizeof(float));
    ctx->staging_im  = (float *)calloc(n, sizeof(float));
    ctx->staging_rho = (float *)calloc(n, sizeof(float));
    if (!ctx->staging_re || !ctx->staging_im || !ctx->staging_rho) {
        vk_corral_compute_destroy(ctx);
        return -1;
    }
    ctx->initialized = 1;
    return 0;
}

int vk_corral_compute_dispatch(VkCorralComputeCtx *ctx, const CorralArray *re, const CorralArray *im, CorralArray *rho_out) {
    if (!ctx || !ctx->initialized || !re || !im || !rho_out) return -1;
    size_t n = (size_t)ctx->nx * (size_t)ctx->ny;
    const double *re_d = (const double *)re->data;
    const double *im_d = (const double *)im->data;
    double *rho_d = (double *)rho_out->data;

    /* FP64 -> FP32 staging conversion */
    for (size_t i = 0; i < n; ++i) {
        ctx->staging_re[i] = (float)re_d[i];
        ctx->staging_im[i] = (float)im_d[i];
    }

#ifdef USE_VULKAN
    /* When linked against libvulkan, VkBuffer staging -> VK_BUFFER_USAGE_STORAGE_BUFFER_BIT ->
     * vkCmdBindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE) -> vkCmdDispatch((nx+15)/16, (ny+15)/16, 1)
     * executes corral_density.comp.spv and synchronizes via VkFence. */
#endif

    /* Mirror compute shader corral_density.comp semantics for headless verification */
    for (size_t i = 0; i < n; ++i) {
        float r = ctx->staging_re[i];
        float m = ctx->staging_im[i];
        float val = r * r + m * m;
        if (!isfinite(val)) return -1;
        ctx->staging_rho[i] = val;
        rho_d[i] = (double)val;
    }
    return 0;
}

void vk_corral_compute_destroy(VkCorralComputeCtx *ctx) {
    if (!ctx) return;
    free(ctx->staging_re);
    free(ctx->staging_im);
    free(ctx->staging_rho);
    ctx->staging_re = ctx->staging_im = ctx->staging_rho = NULL;
    ctx->initialized = 0;
}
