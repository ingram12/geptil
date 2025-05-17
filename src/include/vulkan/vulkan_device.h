#pragma once

#include "vulkan_context.h"

VkResult select_physical_device(VulkanContext* ctx);
VkResult create_logical_device(VulkanContext* ctx);
