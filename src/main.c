#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_GLCOREARB
#include "include/logger/logger.h"
#include "include/vulkan/vulkan_init.h"
#include "include/window/window.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    LOG_INFO("Application started");
    
    VulkanContext ctx = {0};  // Initialize all members to 0/NULL
    
    // Create Vulkan instance
    VkResult result = create_vulkan_instance(&ctx, "Geptil");
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create Vulkan instance: %d", result);
        return 1;
    }
    
    // Select physical device
    result = select_physical_device(&ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to select physical device: %d", result);
        cleanup_vulkan(&ctx);
        return 1;
    }
    
    // Create logical device
    result = create_logical_device(&ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create logical device: %d", result);
        cleanup_vulkan(&ctx);
        return 1;
    }

    result = initWindow(&ctx, 800, 600);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create window: %d", result);
        cleanup_vulkan(&ctx);
        return 1;
    }
    
    result = create_surface(&ctx);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create surface: %d", result);
        cleanup_vulkan(&ctx);
        return 1;
    }

    result = create_swapchain(&ctx, 800, 600);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create swapchain: %d", result);
        cleanup_vulkan(&ctx);
        return 1;
    }
    
    LOG_INFO("Vulkan initialization completed successfully");
    
    // Cleanup
    cleanup_vulkan(&ctx);
    return 0;
}
