#include "vulkan_command_pool.h"
#include "../../logger/logger.h"

VkResult create_command_pool(VulkanContext* ctx) {
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = ctx->graphicsQueueFamily
    };

    if (vkCreateCommandPool(ctx->device, &poolInfo, NULL, &ctx->commandPool) != VK_SUCCESS) {
        LOG_ERROR("Failed to create command pool");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    LOG_INFO("Command pool created successfully");
    return VK_SUCCESS;
}
