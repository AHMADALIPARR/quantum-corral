/* vulkan/renderer/vk_corral_renderer.h - Vulkan Swapchain & Frame Renderer
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#ifndef VK_CORRAL_RENDERER_H
#define VK_CORRAL_RENDERER_H

#include "../compute/vk_corral_compute.h"

int vk_corral_render_ppm(const VkCorralComputeCtx *ctx, const char *ppm_path, double radius_norm);

#endif /* VK_CORRAL_RENDERER_H */
