#include "vulkan_sync.h"
#include "../../logger/logger.h"
#include <stdlib.h>

VkResult create_sync_objects(VulkanContext* ctx) {
    ctx->maxFramesInFlight = ctx->imageCount;
    ctx->currentFrame = 0;

    ctx->imageAvailableSemaphores = malloc(sizeof(VkSemaphore) * ctx->maxFramesInFlight);
    ctx->renderFinishedSemaphores = malloc(sizeof(VkSemaphore) * ctx->maxFramesInFlight);
    ctx->inFlightFences = malloc(sizeof(VkFence) * ctx->maxFramesInFlight);

    if (!ctx->imageAvailableSemaphores || !ctx->renderFinishedSemaphores || !ctx->inFlightFences) {
        LOG_ERROR("Failed to allocate memory for synchronization objects");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (size_t i = 0; i < ctx->maxFramesInFlight; i++) {
        if (vkCreateSemaphore(ctx->device, &semaphoreInfo, NULL, &ctx->imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(ctx->device, &semaphoreInfo, NULL, &ctx->renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(ctx->device, &fenceInfo, NULL, &ctx->inFlightFences[i]) != VK_SUCCESS) {
            LOG_ERROR("Failed to create synchronization objects for a frame");
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    }

    LOG_INFO("Synchronization objects created successfully");
    return VK_SUCCESS;
}

void cleanup_sync_objects(VulkanContext* ctx) {
    if (!ctx->device) return;

    for (size_t i = 0; i < ctx->maxFramesInFlight; i++) {
        if (ctx->imageAvailableSemaphores && ctx->imageAvailableSemaphores[i]) {
            vkDestroySemaphore(ctx->device, ctx->imageAvailableSemaphores[i], NULL);
        }
        if (ctx->renderFinishedSemaphores && ctx->renderFinishedSemaphores[i]) {
            vkDestroySemaphore(ctx->device, ctx->renderFinishedSemaphores[i], NULL);
        }
        if (ctx->inFlightFences && ctx->inFlightFences[i]) {
            vkDestroyFence(ctx->device, ctx->inFlightFences[i], NULL);
        }
    }

    free(ctx->imageAvailableSemaphores);
    free(ctx->renderFinishedSemaphores);
    free(ctx->inFlightFences);
    ctx->imageAvailableSemaphores = NULL;
    ctx->renderFinishedSemaphores = NULL;
    ctx->inFlightFences = NULL;
}
