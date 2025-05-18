#include <vulkan/vulkan.h>
#include <stdint.h>
#include "vulkan_context.h"
#include "core/vulkan_swapchain.h"
#include "../logger/logger.h"

void draw_frame(VulkanContext* ctx) {
    // Wait for the previous frame to finish
    vkWaitForFences(ctx->device, 1, &ctx->inFlightFences[ctx->currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(ctx->device, 1, &ctx->inFlightFences[ctx->currentFrame]);

    // Acquire next swapchain image
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(ctx->device, ctx->swapchain, UINT64_MAX, ctx->imageAvailableSemaphores[ctx->currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreate_swapchain(ctx);
        return;
    } else if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to acquire swapchain image: %d", result);
        return;
    }

    // Reset and record command buffer
    vkResetCommandBuffer(ctx->commandBuffers[ctx->currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0
    };

    if (vkBeginCommandBuffer(ctx->commandBuffers[ctx->currentFrame], &beginInfo) != VK_SUCCESS) {
        LOG_ERROR("Failed to begin recording command buffer");
        return;
    }

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = ctx->renderPass,
        .framebuffer = ctx->swapchainFramebuffers[imageIndex],
        .renderArea.offset = {0, 0},
        .renderArea.extent = ctx->swapchainExtent
    };

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(ctx->commandBuffers[ctx->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(ctx->commandBuffers[ctx->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->graphicsPipeline);
    vkCmdDraw(ctx->commandBuffers[ctx->currentFrame], 3, 1, 0, 0); // Draw a triangle
    vkCmdEndRenderPass(ctx->commandBuffers[ctx->currentFrame]);

    if (vkEndCommandBuffer(ctx->commandBuffers[ctx->currentFrame]) != VK_SUCCESS) {
        LOG_ERROR("Failed to record command buffer");
        return;
    }

    // Submit command buffer
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &ctx->imageAvailableSemaphores[ctx->currentFrame],
        .pWaitDstStageMask = (VkPipelineStageFlags[]) { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
        .commandBufferCount = 1,
        .pCommandBuffers = &ctx->commandBuffers[ctx->currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &ctx->renderFinishedSemaphores[ctx->currentFrame]
    };

    if (vkQueueSubmit(ctx->graphicsQueue, 1, &submitInfo, ctx->inFlightFences[ctx->currentFrame]) != VK_SUCCESS) {
        LOG_ERROR("Failed to submit draw command buffer");
        return;
    }

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &ctx->renderFinishedSemaphores[ctx->currentFrame],
        .swapchainCount = 1,
        .pSwapchains = &ctx->swapchain,
        .pImageIndices = &imageIndex
    };

    result = vkQueuePresentKHR(ctx->presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreate_swapchain(ctx);
        return;
    } else if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to acquire swapchain image: %d", result);
        return;
    }

    ctx->currentFrame = (ctx->currentFrame + 1) % ctx->maxFramesInFlight;
}
