#pragma once

#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    uint32_t imageCount;
    VkImage* swapchainImages;
    VkImageView* swapchainImageViews;
    uint32_t graphicsQueueFamily;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    GLFWwindow* window;
} VulkanContext;

VkResult create_vulkan_instance(VulkanContext* ctx, const char* appName, const char* engineName);
VkResult select_physical_device(VulkanContext* ctx);
VkResult create_logical_device(VulkanContext* ctx);
VkResult create_surface(VulkanContext* ctx);
VkResult create_swapchain(VulkanContext* ctx, uint32_t width, uint32_t height);
void destroy_swapchain(VulkanContext* ctx);
void cleanup_vulkan(VulkanContext* ctx);
