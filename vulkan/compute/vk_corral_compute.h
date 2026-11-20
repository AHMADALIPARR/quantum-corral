/* vulkan/compute/vk_corral_compute.h - Vulkan Compute Pipeline Host
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#ifndef VK_CORRAL_COMPUTE_H
#define VK_CORRAL_COMPUTE_H

#include "corral.h"

typedef struct {
    int initialized;
    int32_t nx;
    int32_t ny;
    float *staging_re;
    float *staging_im;
    float *staging_rho;
} VkCorralComputeCtx;

int vk_corral_compute_init(VkCorralComputeCtx *ctx, int32_t nx, int32_t ny);
int vk_corral_compute_dispatch(VkCorralComputeCtx *ctx, const CorralArray *re, const CorralArray *im, CorralArray *rho_out);
void vk_corral_compute_destroy(VkCorralComputeCtx *ctx);

#endif /* VK_CORRAL_COMPUTE_H */
