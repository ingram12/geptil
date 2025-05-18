#include "../logger/logger.h"
#include "core/vulkan_instance.h"
#include "core/vulkan_device.h"
#include "core/vulkan_surface.h"
#include "core/vulkan_swapchain.h"
#include "core/vulkan_cleanup.h"
#include "core/vulkan_render_pass.h"
#include "core/vulkan_pipeline.h"
#include "core/vulkan_framebuffer.h"
#include "core/vulkan_command_pool.h"
#include "core/vulkan_command_buffer.h"
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

    result = create_render_pass(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create render pass: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }

    result = create_framebuffers(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create framebuffers: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }

    result = create_pipeline(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create pipeline: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }

    result = create_command_pool(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create command pool: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }

    result = create_command_buffers(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create command buffers: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }
    
    LOG_INFO("Vulkan initialization completed successfully");
    return VK_SUCCESS;
}
