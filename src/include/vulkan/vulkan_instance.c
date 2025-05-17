#include "vulkan_context.h"
#include "../logger/logger.h"
#include <stdbool.h>

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

VkResult create_vulkan_instance(VulkanContext* ctx, const char* appName, const char* engineName) {
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = appName,
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = engineName,
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };
    
    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_portability_enumeration",
        "VK_EXT_metal_surface",
        "VK_KHR_get_physical_device_properties2",
    };
    uint32_t extensionCount = sizeof(extensions) / sizeof(extensions[0]);

    const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation",
    };
    
    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = extensionCount,
        .ppEnabledExtensionNames = extensions,
        .enabledLayerCount = enableValidationLayers ? sizeof(validationLayers) / sizeof(validationLayers[0]) : 0,
        .ppEnabledLayerNames = enableValidationLayers ? validationLayers : NULL,
        .flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
    };

    VkResult result = vkCreateInstance(&createInfo, NULL, &ctx->instance);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create Vulkan instance");
        return result;
    }

    LOG_INFO("Vulkan instance created successfully");
    return VK_SUCCESS;
}
