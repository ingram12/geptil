#pragma once

#include <vulkan/vulkan.h>
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
    VkRenderPass renderPass;
    GLFWwindow* window;
} VulkanContext;
