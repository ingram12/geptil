#include "../../logger/logger.h"
#include "../vulkan_context.h"
#include <stdlib.h>

void destroy_swapchain(VulkanContext* ctx) {
    if (!ctx || !ctx->device) return;

    if (ctx->swapchainImageViews) {
        for (uint32_t i = 0; i < ctx->imageCount; i++) {
            if (ctx->swapchainImageViews[i]) {
                vkDestroyImageView(ctx->device, ctx->swapchainImageViews[i], NULL);
            }
        }
        free(ctx->swapchainImageViews);
        ctx->swapchainImageViews = NULL;
    }

    if (ctx->swapchainImages) {
        free(ctx->swapchainImages);
        ctx->swapchainImages = NULL;
    }

    if (ctx->swapchain) {
        vkDestroySwapchainKHR(ctx->device, ctx->swapchain, NULL);
        ctx->swapchain = VK_NULL_HANDLE;
    }

    ctx->imageCount = 0;

    LOG_INFO("Swapchain destroyed");
}

void cleanup_vulkan(VulkanContext* ctx) {
    destroy_swapchain(ctx);

    if (ctx->device) {
        vkDestroyDevice(ctx->device, NULL);
    }

    if (ctx->surface) {
        vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    }

    if (ctx->instance) {
        vkDestroyInstance(ctx->instance, NULL);
    }

    LOG_INFO("Vulkan resources cleaned up");
}
