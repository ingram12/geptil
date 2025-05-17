#include "vulkan_init.h"
#include "../logger/logger.h"
#include <stdlib.h>
#include <string.h>

// Validation layers (for debugging)
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

VkResult select_physical_device(VulkanContext* ctx) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(ctx->instance, &deviceCount, NULL);
    
    if (deviceCount == 0) {
        LOG_ERROR("Failed to find GPUs with Vulkan support");
        return VK_ERROR_DEVICE_LOST;
    }

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
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

VkResult create_swapchain(VulkanContext* ctx, uint32_t width, uint32_t height) {
    // Query surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx->physicalDevice, ctx->surface, &capabilities);

    // Choose surface format
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physicalDevice, ctx->surface, &formatCount, NULL);
    VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(ctx->physicalDevice, ctx->surface, &formatCount, formats);

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (uint32_t i = 0; i < formatCount; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = formats[i];
            break;
        }
    }
    free(formats);

    // Choose present mode
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Choose swap extent
    VkExtent2D extent = {
        .width = width,
        .height = height
    };

    if (extent.width > capabilities.maxImageExtent.width)
        extent.width = capabilities.maxImageExtent.width;
    if (extent.width < capabilities.minImageExtent.width)
        extent.width = capabilities.minImageExtent.width;
    if (extent.height > capabilities.maxImageExtent.height)
        extent.height = capabilities.maxImageExtent.height;
    if (extent.height < capabilities.minImageExtent.height)
        extent.height = capabilities.minImageExtent.height;

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = ctx->surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    VkResult result = vkCreateSwapchainKHR(ctx->device, &createInfo, NULL, &ctx->swapchain);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create swap chain");
        return result;
    }

    ctx->swapchainImageFormat = surfaceFormat.format;
    ctx->swapchainExtent = extent;

    // Get swapchain images
    vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->imageCount, NULL);
    ctx->swapchainImages = malloc(sizeof(VkImage) * ctx->imageCount);
    vkGetSwapchainImagesKHR(ctx->device, ctx->swapchain, &ctx->imageCount, ctx->swapchainImages);

    // Create image views
    ctx->swapchainImageViews = malloc(sizeof(VkImageView) * ctx->imageCount);
    for (uint32_t i = 0; i < ctx->imageCount; i++) {
        VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = ctx->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = ctx->swapchainImageFormat,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        if (vkCreateImageView(ctx->device, &viewInfo, NULL, &ctx->swapchainImageViews[i]) != VK_SUCCESS) {
            LOG_ERROR("Failed to create image views");
            return VK_ERROR_INITIALIZATION_FAILED;
        }
    }

    LOG_INFO("Swapchain created successfully");
    return VK_SUCCESS;
}

VkResult create_surface(VulkanContext* ctx) {
    VkResult result = glfwCreateWindowSurface(ctx->instance, ctx->window, NULL, &ctx->surface);
    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create window surface");
        return result;
    }
    
    LOG_INFO("Surface created successfully");
    return VK_SUCCESS;
}

void destroy_swapchain(VulkanContext* ctx) {
    for (uint32_t i = 0; i < ctx->imageCount; i++) {
        vkDestroyImageView(ctx->device, ctx->swapchainImageViews[i], NULL);
    }
    free(ctx->swapchainImageViews);
    free(ctx->swapchainImages);
    vkDestroySwapchainKHR(ctx->device, ctx->swapchain, NULL);
    LOG_INFO("Swapchain destroyed");
}

void cleanup_vulkan(VulkanContext* ctx) {
    destroy_swapchain(ctx);
    vkDestroyDevice(ctx->device, NULL);
    vkDestroySurfaceKHR(ctx->instance, ctx->surface, NULL);
    vkDestroyInstance(ctx->instance, NULL);
    LOG_INFO("Vulkan resources cleaned up");
}
