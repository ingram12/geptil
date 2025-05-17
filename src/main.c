#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_GLCOREARB
#include "include/logger/logger.h"
#include "include/vulkan/vulkan_init.h"
#include "include/vulkan/core/vulkan_cleanup.h"
#include "include/window/window.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    VulkanContext ctx = {0};
    init_vulkan(&ctx, "geptil", "geptil engine", 800, 600);

    while (!glfwWindowShouldClose(ctx.window)) {
        glfwPollEvents();
    }
    
    cleanup_vulkan(&ctx);
    return 0;
}
