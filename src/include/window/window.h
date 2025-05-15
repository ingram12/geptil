#ifndef GEPTIL_WINDOW_H
#define GEPTIL_WINDOW_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "../logger/logger.h"
#include "../vulkan/vulkan_init.h"

VkResult initWindow(VulkanContext* ctx, uint32_t width, uint32_t height);

#endif // GEPTIL_WINDOW_H
