/* vulkan/renderer/vk_corral_renderer.c - Vulkan Frame Renderer & Headless PPM Export
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "vk_corral_renderer.h"
#include <stdio.h>
#include <math.h>

int vk_corral_render_ppm(const VkCorralComputeCtx *ctx, const char *ppm_path, double radius_norm) {
    if (!ctx || !ctx->initialized || !ppm_path) return -1;
    FILE *fp = fopen(ppm_path, "wb");
    if (!fp) return -1;

    float max_rho = 1e-6f;
    size_t n = (size_t)ctx->nx * (size_t)ctx->ny;
    for (size_t i = 0; i < n; ++i) {
        if (ctx->staging_rho[i] > max_rho) max_rho = ctx->staging_rho[i];
    }

    fprintf(fp, "P6\n%d %d\n255\n", ctx->nx, ctx->ny);
    for (int32_t y = 0; y < ctx->ny; ++y) {
        double v = ((double)y / (double)(ctx->ny - 1)) * 2.0 - 1.0;
        for (int32_t x = 0; x < ctx->nx; ++x) {
            double u = ((double)x / (double)(ctx->nx - 1)) * 2.0 - 1.0;
            float val = ctx->staging_rho[y * ctx->nx + x] / max_rho;
            if (val > 1.0f) val = 1.0f;
            float p = powf(val, 0.85f);

            double r = 0.03 + (0.25 - 0.03) * p;
            double g = 0.05 + (0.85 - 0.05) * p;
            double b = 0.10 + (1.00 - 0.10) * p;

            if (fabs(hypot(u, v) - radius_norm) < 0.015) {
                r = 0.92; g = 0.76; b = 0.45;
            }
            unsigned char rgb[3] = {
                (unsigned char)(r * 255.0),
                (unsigned char)(g * 255.0),
                (unsigned char)(b * 255.0)
            };
            fwrite(rgb, 1, 3, fp);
        }
    }
    fclose(fp);
    return 0;
}
