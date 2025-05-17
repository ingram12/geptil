#include "../vulkan_context.h"
#include "../../logger/logger.h"
#include <stdlib.h>

VkResult create_framebuffers(VulkanContext* ctx) {
    ctx->swapchainFramebuffers = malloc(sizeof(VkFramebuffer) * ctx->imageCount);
    if (!ctx->swapchainFramebuffers) {
        LOG_ERROR("Failed to allocate memory for framebuffers");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < ctx->imageCount; i++) {
        VkImageView attachments[] = {
            ctx->swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = ctx->renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = ctx->swapchainExtent.width,
            .height = ctx->swapchainExtent.height,
            .layers = 1
        };

        VkResult result = vkCreateFramebuffer(ctx->device, &framebufferInfo, NULL, &ctx->swapchainFramebuffers[i]);
        if (result != VK_SUCCESS) {
            LOG_ERROR("Failed to create framebuffer %d", i);
            return result;
        }
    }

    LOG_INFO("Created %d framebuffers successfully", ctx->imageCount);
    return VK_SUCCESS;
}
