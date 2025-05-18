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
#include "core/vulkan_sync.h"
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

    VkResult result = initWindow(ctx, ctx->width, ctx->height);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create window: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }
    
    result = create_vulkan_instance(ctx, appName, engineName);
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

    result = create_sync_objects(ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create synchronization objects: %d", result);
        cleanup_vulkan(ctx);
        return result;
    }
    
    LOG_INFO("=== Vulkan Context State After Initialization ===");
    LOG_INFO("Basic Info:");
    LOG_INFO("- Window dimensions: %dx%d", ctx->width, ctx->height);
    LOG_INFO("- Current frame: %d", ctx->currentFrame);
    LOG_INFO("- Max frames in flight: %d", ctx->maxFramesInFlight);
    
    LOG_INFO("\nSwapchain Info:");
    LOG_INFO("- Image count: %d", ctx->imageCount);
    LOG_INFO("- Format: %d", ctx->swapchainImageFormat);
    LOG_INFO("- Extent: %dx%d", ctx->swapchainExtent.width, ctx->swapchainExtent.height);
    
    LOG_INFO("\nQueue Info:");
    LOG_INFO("- Graphics queue family index: %d", ctx->graphicsQueueFamily);
    LOG_INFO("- Queue family indices:");
    LOG_INFO("  - Graphics family: %d", ctx->queueFamilyIndices.graphicsFamily);
    
    LOG_INFO("\nHandle Status:");
    LOG_INFO("- Instance: %p", (void*)ctx->instance);
    LOG_INFO("- Physical Device: %p", (void*)ctx->physicalDevice);
    LOG_INFO("- Logical Device: %p", (void*)ctx->device);
    LOG_INFO("- Surface: %p", (void*)ctx->surface);
    LOG_INFO("- Swapchain: %p", (void*)ctx->swapchain);
    LOG_INFO("- Render Pass: %p", (void*)ctx->renderPass);
    LOG_INFO("- Pipeline Layout: %p", (void*)ctx->pipelineLayout);
    LOG_INFO("- Graphics Pipeline: %p", (void*)ctx->graphicsPipeline);
    LOG_INFO("- Command Pool: %p", (void*)ctx->commandPool);
    LOG_INFO("- Window Handle: %p", (void*)ctx->window);
    
    LOG_INFO("\nResource Arrays Status:");
    LOG_INFO("- Swapchain Images: %p", (void*)ctx->swapchainImages);
    LOG_INFO("- Swapchain Image Views: %p", (void*)ctx->swapchainImageViews);
    LOG_INFO("- Swapchain Framebuffers: %p", (void*)ctx->swapchainFramebuffers);
    LOG_INFO("- Command Buffers: %p", (void*)ctx->commandBuffers);
    LOG_INFO("- Image Available Semaphores: %p", (void*)ctx->imageAvailableSemaphores);
    LOG_INFO("- Render Finished Semaphores: %p", (void*)ctx->renderFinishedSemaphores);
    LOG_INFO("- In Flight Fences: %p", (void*)ctx->inFlightFences);
    LOG_INFO("=======================================");
    
    LOG_INFO("Vulkan initialization completed successfully");
    return VK_SUCCESS;
}
