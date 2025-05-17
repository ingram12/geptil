#include "vulkan_context.h"
#include "../logger/logger.h"
#include <stdlib.h>

VkResult create_swapchain(VulkanContext* ctx, uint32_t width, uint32_t height) {
    // Query surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->physicalDevice, ctx->surface, &capabilities);

    // Choose surface format
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physicalDevice, ctx->surface, &formatCount, NULL);
    VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physicalDevice, ctx->surface, &formatCount, formats);

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (uint32_t i = 0; i < formatCount; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = formats[i];
            break;
        }
    }
    free(formats);

    VkExtent2D extent = {
        .width = width,
        .height = height
    };

    if (extent.width > capabilities.maxImageExtent.width)
        extent.width = capabilities.maxImageExtent.width;
    if (extent.width < capabilities.minImageExtent.width)
        extent.width = capabilities.minImageExtent.width;
    if (extent.height > capabilities.maxImageExtent.height)
        extent.height = capabilities.maxImageExtent.height;
    if (extent.height < capabilities.minImageExtent.height)
        extent.height = capabilities.minImageExtent.height;

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = ctx->surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    VkResult result = vkCreateSwapchainKHR(ctx->device, &createInfo, NULL, &ctx->swapchain);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create swap chain");
        return result;
    }

    ctx->swapchainImageFormat = surfaceFormat.format;
    ctx->swapchainExtent = extent;

    // Get swapchain images
    vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->imageCount, NULL);
    ctx->swapchainImages = malloc(sizeof(VkImage) * ctx->imageCount);
    vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->imageCount, ctx->swapchainImages);

    // Create image views
    ctx->swapchainImageViews = malloc(sizeof(VkImageView) * ctx->imageCount);
    for (uint32_t i = 0; i < ctx->imageCount; i++) {
        VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = ctx->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = ctx->swapchainImageFormat,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        if (vkCreateImageView(ctx->device, &viewInfo, NULL, &ctx->swapchainImageViews[i]) != VK_SUCCESS) {
            LOG_ERROR("Failed to create image views");
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    }

    LOG_INFO("Swapchain created successfully");
    return VK_SUCCESS;
}
