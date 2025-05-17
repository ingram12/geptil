#include "../vulkan_context.h"
#include "../shader/shader_utils.h"
#include "../../logger/logger.h"
#include <stdlib.h>

static VkShaderModule create_shader_module(VulkanContext* ctx, const uint32_t* code, size_t codeSize) {
    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = codeSize,
        .pCode = code
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(ctx->device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        LOG_ERROR("Failed to create shader module");
        return VK_NULL_HANDLE;
    }

    return shaderModule;
}

VkResult create_pipeline(VulkanContext* ctx) {
    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0
    };

    if (vkCreatePipelineLayout(ctx->device, &pipelineLayoutInfo, NULL, &ctx->pipelineLayout) != VK_SUCCESS) {
        LOG_ERROR("Failed to create pipeline layout");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // For now, we'll create a simple pipeline that just fills the screen with a color
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .vertexAttributeDescriptionCount = 0
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)ctx->swapchainExtent.width,
        .height = (float)ctx->swapchainExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = ctx->swapchainExtent
    };

    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                         VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE
    };

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    // Load shader code from files
    uint32_t* vertShaderCode = NULL;
    size_t vertShaderCodeSize = 0;
    VkResult result = load_shader_code("src/include/vulkan/shader/triangle.vert.spv", &vertShaderCode, &vertShaderCodeSize);
    if (result != VK_SUCCESS) {
        return result;
    }

    uint32_t* fragShaderCode = NULL;
    size_t fragShaderCodeSize = 0;
    result = load_shader_code("src/include/vulkan/shader/triangle.frag.spv", &fragShaderCode, &fragShaderCodeSize);
    if (result != VK_SUCCESS) {
        free(vertShaderCode);
        return result;
    }

    VkShaderModule vertShaderModule = create_shader_module(ctx, vertShaderCode, vertShaderCodeSize);
    free(vertShaderCode);
    if (vertShaderModule == VK_NULL_HANDLE) {
        free(fragShaderCode);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkShaderModule fragShaderModule = create_shader_module(ctx, fragShaderCode, fragShaderCodeSize);
    free(fragShaderCode);
    if (fragShaderModule == VK_NULL_HANDLE) {
        vkDestroyShaderModule(ctx->device, vertShaderModule, NULL);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertShaderModule,
            .pName = "main"
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragShaderModule,
            .pName = "main"
        }
    };

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &colorBlending,
        .layout = ctx->pipelineLayout,
        .renderPass = ctx->renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .pDepthStencilState = NULL,
        .pDynamicState = NULL
    };

    result = vkCreateGraphicsPipelines(ctx->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &ctx->graphicsPipeline);
    
    // Clean up shader modules
    vkDestroyShaderModule(ctx->device, fragShaderModule, NULL);
    vkDestroyShaderModule(ctx->device, vertShaderModule, NULL);

    if (result != VK_SUCCESS) {
        LOG_ERROR("Failed to create graphics pipeline");
        return result;
    }

    LOG_INFO("Graphics pipeline created successfully");
    return VK_SUCCESS;
}
