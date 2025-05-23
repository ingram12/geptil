#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

typedef struct {
    uint32_t graphicsFamily;
} QueueFamilyIndices;

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
    VkFramebuffer* swapchainFramebuffers;
    QueueFamilyIndices queueFamilyIndices;
    uint32_t graphicsQueueFamily;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffers;
    GLFWwindow* window;

    VkSemaphore* imageAvailableSemaphores;
    VkSemaphore* renderFinishedSemaphores;
    VkFence* inFlightFences;
    uint32_t maxFramesInFlight;
    uint32_t currentFrame;
} VulkanContext;
