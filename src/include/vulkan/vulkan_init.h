#pragma once

#include "vulkan_context.h"

VkResult create_vulkan_instance(VulkanContext* ctx, const char* appName, const char* engineName);
VkResult select_physical_device(VulkanContext* ctx);
VkResult create_logical_device(VulkanContext* ctx);
VkResult create_surface(VulkanContext* ctx);
VkResult create_swapchain(VulkanContext* ctx, uint32_t width, uint32_t height);
void destroy_swapchain(VulkanContext* ctx);
void cleanup_vulkan(VulkanContext* ctx);
