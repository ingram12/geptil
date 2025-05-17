#include "../vulkan_context.h"
#include "../../logger/logger.h"
#include <stdlib.h>

VkResult select_physical_device(VulkanContext* ctx) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(ctx->instance, &deviceCount, NULL);
    
    if (deviceCount == 0) {
        LOG_ERROR("Failed to find GPUs with Vulkan support");
        return VK_ERROR_DEVICE_LOST;
    }

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    if (!devices) {
        LOG_ERROR("Failed to allocate memory for physical devices");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    vkEnumeratePhysicalDevices(ctx->instance, &deviceCount, devices);

    // Select first suitable device
    for (uint32_t i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
        
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            ctx->physicalDevice = devices[i];
            LOG_INFO("Selected discrete GPU: %s", deviceProperties.deviceName);
            free(devices);
            return VK_SUCCESS;
        }
    }

    // If no discrete GPU found, select first available device
    if (deviceCount > 0) {
        ctx->physicalDevice = devices[0];
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(devices[0], &deviceProperties);
        LOG_INFO("Selected GPU: %s", deviceProperties.deviceName);
        free(devices);
        return VK_SUCCESS;
    }

    free(devices);
    LOG_ERROR("Failed to find a suitable GPU");
    return VK_ERROR_DEVICE_LOST;
}

VkResult create_logical_device(VulkanContext* ctx) {
    // Find queue family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(ctx->physicalDevice, &queueFamilyCount, NULL);
    VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    if (!queueFamilies) {
        LOG_ERROR("Failed to allocate memory for queue families");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(ctx->physicalDevice, &queueFamilyCount, queueFamilies);

    // Find graphics queue family
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            ctx->graphicsQueueFamily = i;
            break;
        }
    }
    free(queueFamilies);

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = ctx->graphicsQueueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    const char* deviceExtensions[] = {"VK_KHR_swapchain", "VK_KHR_portability_subset"};
    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &queueCreateInfo,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &deviceFeatures,
        .ppEnabledExtensionNames = deviceExtensions,
        .enabledExtensionCount = 2,
    };

    VkResult result = vkCreateDevice(ctx->physicalDevice, &createInfo, NULL, &ctx->device);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create logical device");
        return result;
    }

    vkGetDeviceQueue(ctx->device, ctx->graphicsQueueFamily, 0, &ctx->graphicsQueue);
    ctx->presentQueue = ctx->graphicsQueue;

    LOG_INFO("Logical device created successfully");
    return VK_SUCCESS;
}
