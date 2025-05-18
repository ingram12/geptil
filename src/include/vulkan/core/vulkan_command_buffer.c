#include "../vulkan_context.h"
#include "../../logger/logger.h"
#include <stdlib.h>

VkResult create_command_buffers(VulkanContext* ctx) {
    ctx->commandBuffers = malloc(sizeof(VkCommandBuffer) * ctx->imageCount);
    if (!ctx->commandBuffers) {
        LOG_ERROR("Failed to allocate memory for command buffers");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = ctx->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = ctx->imageCount
    };

    VkResult result = vkAllocateCommandBuffers(ctx->device, &allocInfo, ctx->commandBuffers);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to allocate command buffers");
        free(ctx->commandBuffers);
        ctx->commandBuffers = NULL;
        return result;
    }

    LOG_INFO("Command buffers created and recorded successfully");
    return VK_SUCCESS;
}

void cleanup_command_buffers(VulkanContext* ctx) {
    if (ctx->device && ctx->commandPool && ctx->commandBuffers) {
        vkFreeCommandBuffers(ctx->device, ctx->commandPool, ctx->imageCount, ctx->commandBuffers);
        free(ctx->commandBuffers);
        ctx->commandBuffers = NULL;
    }
}
