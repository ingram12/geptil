#include "../logger/logger.h"
#include "vulkan_context.h"
#include <stdlib.h>

void destroy_swapchain(VulkanContext* ctx) {
    for (uint32_t i = 0; i < ctx->imageCount; i++) {
        vkDestroyImageView(ctx->device, ctx->swapchainImageViews[i], NULL);
    }
    free(ctx->swapchainImageViews);
    free(ctx->swapchainImages);
    vkDestroySwapchainKHR(ctx->device, ctx->swapchain, NULL);
    LOG_INFO("Swapchain destroyed");
}

void cleanup_vulkan(VulkanContext* ctx) {
    destroy_swapchain(ctx);
    vkDestroyDevice(ctx->device, NULL);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    vkDestroyInstance(ctx->instance, NULL);
    LOG_INFO("Vulkan resources cleaned up");
}
