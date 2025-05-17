#pragma once

#include "vulkan_context.h"

VkResult init_vulkan(
    VulkanContext* ctx,
    const char* appName,
    const char* engineName,
    uint32_t width,
    uint32_t height
);
