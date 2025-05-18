#include "include/vulkan/vulkan_init.h"
#include "include/vulkan/vulkan_draw.h"
#include "include/vulkan/core/vulkan_cleanup.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    VulkanContext ctx = {0};
    init_vulkan(&ctx, "geptil", "geptil engine", 800, 600);

    while (!glfwWindowShouldClose(ctx.window)) {
        glfwPollEvents();
        draw_frame(&ctx);
    }
    
    cleanup_vulkan(&ctx);
    return 0;
}
