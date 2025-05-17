#include "vulkan_context.h"
#include "../logger/logger.h"

VkResult create_surface(VulkanContext* ctx) {
    VkResult result = glfwCreateWindowSurface(ctx->instance, ctx->window, NULL, &ctx->surface);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create window surface");
        return result;
    }
    
    LOG_INFO("Surface created successfully");
    return VK_SUCCESS;
}
