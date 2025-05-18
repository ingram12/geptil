#pragma once

#include "../vulkan_context.h"

VkResult create_sync_objects(VulkanContext* ctx);
void cleanup_sync_objects(VulkanContext* ctx);
