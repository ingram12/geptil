#include "../logger/logger.h"
#include "vulkan_instance.h"
#include "vulkan_device.h"
#include "vulkan_surface.h"
#include "vulkan_swapchain.h"
#include "vulkan_cleanup.h"
#include "../window/window.h"

VkResult init_vulkan(
    VulkanContext* ctx,
    const char* appName,
    const char* engineName,
    uint32_t width,
    uint32_t height
) {
    LOG_INFO("Initializing Vulkan started");

    ctx->width = width;
    ctx->height = height;
    
    VkResult result = create_vulkan_instance(ctx, appName, engineName);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create Vulkan instance: %d", result);
        return result;
    }
    
    result = select_physical_device(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to select physical device: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }
    
    result = create_logical_device(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create logical device: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }

    result = initWindow(ctx, ctx->width, ctx->height);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create window: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }
    
    result = create_surface(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create surface: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }

    result = create_swapchain(ctx, ctx->width, ctx->height);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create swapchain: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }
    
    LOG_INFO("Vulkan initialization completed successfully");
    return VK_SUCCESS;
}
