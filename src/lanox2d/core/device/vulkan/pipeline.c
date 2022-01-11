/*!A lightweight and fast 2D vector graphics engine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2021-present, Lanox2D Open Source Group.
 *
 * @author      ruki
 * @file        pipeline.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pipeline.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the pipeline type
typedef struct lx_vk_pipeline_t {
    lx_size_t           type;
    VkPipeline          pipeline;
    VkPipelineCache     pipeline_cache;
    VkPipelineLayout    pipeline_layout;
    lx_vulkan_device_t* device;
}lx_vk_pipeline_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_vk_pipeline_ref_t lx_vk_pipeline_init(lx_vulkan_device_t* device,
    lx_size_t type, lx_char_t const* vshader, lx_size_t vshader_size, lx_char_t const* fshader, lx_size_t fshader_size,
    VkPipelineVertexInputStateCreateInfo* vertex_inputinfo, VkPipelineLayoutCreateInfo* pipeline_layoutinfo) {
    lx_assert_and_check_return_val(device && device->device && vshader && fshader, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_pipeline_t* pipeline = lx_null;
    VkShaderModule vshader_module = 0;
    VkShaderModule fshader_module = 0;
    do {
        // init pipeline
        pipeline = lx_malloc0_type(lx_vk_pipeline_t);
        lx_assert_and_check_break(pipeline);

        pipeline->type   = type;
        pipeline->device = device;

        // create pipeline layout (empty)
        if (vkCreatePipelineLayout(device->device, pipeline_layoutinfo, lx_null, &pipeline->pipeline_layout) != VK_SUCCESS) {
            break;
        }

        // create vertex shader
        VkShaderModuleCreateInfo vshader_createinfo = {};
        vshader_createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vshader_createinfo.pNext = lx_null;
        vshader_createinfo.flags = 0;
        vshader_createinfo.codeSize = (lx_uint32_t)vshader_size;
        vshader_createinfo.pCode = (lx_uint32_t*)vshader;
        if (vkCreateShaderModule(device->device, &vshader_createinfo, lx_null, &vshader_module) != VK_SUCCESS) {
            break;
        }

        // create fragment shader
        VkShaderModuleCreateInfo fshader_createinfo = {};
        fshader_createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fshader_createinfo.pNext = lx_null;
        fshader_createinfo.flags = 0;
        fshader_createinfo.codeSize = (lx_uint32_t)fshader_size;
        fshader_createinfo.pCode = (lx_uint32_t*)fshader;
        if (vkCreateShaderModule(device->device, &fshader_createinfo, lx_null, &fshader_module) != VK_SUCCESS) {
            break;
        }

        // init vertex and fragment shader stages
        VkPipelineShaderStageCreateInfo shader_stages[2] = {};
        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].pNext = lx_null;
        shader_stages[0].flags = 0;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module = vshader_module;
        shader_stages[0].pName = "main";
        shader_stages[0].pSpecializationInfo = lx_null;

        shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[1].pNext = lx_null;
        shader_stages[1].flags = 0;
        shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stages[1].module = fshader_module;
        shader_stages[1].pName = "main";
        shader_stages[1].pSpecializationInfo = lx_null;

        // init viewport
        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = (lx_float_t)device->framesize.width;
        viewport.height = (lx_float_t)device->framesize.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = device->framesize;

        VkPipelineViewportStateCreateInfo viewport_info = {};
        viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_info.pNext = lx_null;
        viewport_info.viewportCount = 1;
        viewport_info.pViewports = &viewport;
        viewport_info.scissorCount = 1;
        viewport_info.pScissors = &scissor;

        // init multisample info
        VkSampleMask sample_mask = ~0u;
        VkPipelineMultisampleStateCreateInfo multisample_info = {};
        multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_info.pNext = lx_null;
        multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisample_info.sampleShadingEnable = VK_FALSE;
        multisample_info.minSampleShading = 0;
        multisample_info.pSampleMask = &sample_mask;
        multisample_info.alphaToCoverageEnable = VK_FALSE;
        multisample_info.alphaToOneEnable = VK_FALSE;

        // init color blend state
        VkPipelineColorBlendAttachmentState attachment_states = {};
        attachment_states.blendEnable = VK_FALSE;
        attachment_states.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo color_blendinfo = {};
        color_blendinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blendinfo.pNext = lx_null;
        color_blendinfo.flags = 0;
        color_blendinfo.logicOpEnable = VK_FALSE;
        color_blendinfo.logicOp = VK_LOGIC_OP_COPY;
        color_blendinfo.attachmentCount = 1;
        color_blendinfo.pAttachments = &attachment_states;

        // init rasterizer info
        VkPipelineRasterizationStateCreateInfo raster_info = {};
        raster_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        raster_info.pNext = lx_null;
        raster_info.depthClampEnable = VK_FALSE;
        raster_info.rasterizerDiscardEnable = VK_FALSE;
        raster_info.polygonMode = VK_POLYGON_MODE_FILL;
        raster_info.cullMode = VK_CULL_MODE_NONE;
        raster_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
        raster_info.depthBiasEnable = VK_FALSE;
        raster_info.lineWidth = 1;

        // init input assembler state
        VkPipelineInputAssemblyStateCreateInfo input_assemblyinfo = {};
        input_assemblyinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assemblyinfo.pNext = lx_null;
        input_assemblyinfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assemblyinfo.primitiveRestartEnable = VK_FALSE;

        // create the pipeline cache
        VkPipelineCacheCreateInfo pipeline_cacheinfo = {};
        pipeline_cacheinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        pipeline_cacheinfo.pNext = lx_null;
        pipeline_cacheinfo.flags = 0; // reserved, must be 0
        pipeline_cacheinfo.initialDataSize = 0;
        pipeline_cacheinfo.pInitialData = lx_null;
        if (vkCreatePipelineCache(device->device, &pipeline_cacheinfo, lx_null, &pipeline->pipeline_cache) != VK_SUCCESS) {
            break;
        }

        // create the pipeline
        VkGraphicsPipelineCreateInfo pipeline_createinfo = {};
        pipeline_createinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_createinfo.pNext = lx_null;
        pipeline_createinfo.flags = 0;
        pipeline_createinfo.stageCount = 2;
        pipeline_createinfo.pStages = shader_stages;
        pipeline_createinfo.pVertexInputState = vertex_inputinfo;
        pipeline_createinfo.pInputAssemblyState = &input_assemblyinfo;
        pipeline_createinfo.pTessellationState = lx_null;
        pipeline_createinfo.pViewportState = &viewport_info;
        pipeline_createinfo.pRasterizationState = &raster_info;
        pipeline_createinfo.pMultisampleState = &multisample_info;
        pipeline_createinfo.pDepthStencilState = lx_null;
        pipeline_createinfo.pColorBlendState = &color_blendinfo;
        pipeline_createinfo.pDynamicState = lx_null;
        pipeline_createinfo.layout = pipeline->pipeline_layout;
        pipeline_createinfo.renderPass = device->renderpass;
        pipeline_createinfo.subpass = 0;
        pipeline_createinfo.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_createinfo.basePipelineIndex = 0;
        if (vkCreateGraphicsPipelines(device->device, pipeline->pipeline_cache, 1, &pipeline_createinfo, lx_null, &pipeline->pipeline) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);

    // free shaders
    if (vshader_module) {
        vkDestroyShaderModule(device->device, vshader_module, lx_null);
        vshader_module = 0;
    }
    if (fshader_module) {
        vkDestroyShaderModule(device->device, fshader_module, lx_null);
        fshader_module = 0;
    }

    // free pipeline if failed
    if (!ok && pipeline) {
        lx_vk_pipeline_exit((lx_vk_pipeline_ref_t)pipeline);
        pipeline = lx_null;
    }
    return (lx_vk_pipeline_ref_t)pipeline;
}

static lx_vk_pipeline_ref_t lx_vk_pipeline_get(lx_vulkan_device_t* device,
    lx_size_t type, lx_char_t const* vshader, lx_size_t vshader_size, lx_char_t const* fshader, lx_size_t fshader_size,
    VkPipelineVertexInputStateCreateInfo* vertex_inputinfo, VkPipelineLayoutCreateInfo* pipeline_layoutinfo) {
    lx_assert_and_check_return_val(device && vshader && fshader, lx_null);
    lx_assert_and_check_return_val(type < lx_arrayn(device->pipelines), lx_null);

    lx_vk_pipeline_ref_t pipeline = device->pipelines[type];
    if (!pipeline) {
        pipeline = lx_vk_pipeline_init(device, type, vshader, vshader_size, fshader, fshader_size, vertex_inputinfo, pipeline_layoutinfo);
        device->pipelines[type] = pipeline;
    }
    return pipeline;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_vk_pipeline_ref_t lx_vk_pipeline_solid(lx_vulkan_device_t* device) {
    static lx_char_t const vshader[] = {
#include "solid.vert.spv.h"
    };
    static lx_char_t const fshader[] = {
#include "solid.frag.spv.h"
    };

    // init vertex input state
    VkVertexInputBindingDescription vertex_input_bindings[1];
    vertex_input_bindings[0].binding = 0; // for vertices buffer
    vertex_input_bindings[0].stride = 3 * sizeof(lx_float_t);
    vertex_input_bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_input_attributes[1];
    vertex_input_attributes[0].location = 0; // layout(location = 0) in vec4 aVertices;
    vertex_input_attributes[0].binding = 0;
    vertex_input_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_input_attributes[0].offset = 0;

    VkPipelineVertexInputStateCreateInfo vertex_inputinfo = {};
    vertex_inputinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_inputinfo.pNext = lx_null;
    vertex_inputinfo.vertexBindingDescriptionCount = lx_arrayn(vertex_input_bindings);
    vertex_inputinfo.pVertexBindingDescriptions = vertex_input_bindings;
    vertex_inputinfo.vertexAttributeDescriptionCount = lx_arrayn(vertex_input_attributes);
    vertex_inputinfo.pVertexAttributeDescriptions = vertex_input_attributes;

    // aColor as push-constant
    VkPushConstantRange push_constant_range = {};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = 4 * sizeof(lx_float_t);

    // init pipeline layout info
    VkPipelineLayoutCreateInfo pipeline_layoutinfo;
	pipeline_layoutinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layoutinfo.pNext = lx_null;
	pipeline_layoutinfo.setLayoutCount = 0;
	pipeline_layoutinfo.pSetLayouts = lx_null;
	pipeline_layoutinfo.pushConstantRangeCount = 1;
	pipeline_layoutinfo.pPushConstantRanges = &push_constant_range;

    // get pipeline
    return lx_vk_pipeline_get(device, LX_VK_PIPELINE_TYPE_SOLID,
        vshader, sizeof(vshader), fshader, sizeof(fshader), &vertex_inputinfo, &pipeline_layoutinfo);
}

lx_vk_pipeline_ref_t lx_vk_pipeline_texture(lx_vulkan_device_t* device) {
    static lx_char_t const vshader[] = {
#include "texture.vert.spv.h"
    };
    static lx_char_t const fshader[] = {
#include "texture.frag.spv.h"
    };

    // init vertex input state
    VkVertexInputBindingDescription vertex_input_bindings = {};
    vertex_input_bindings.binding = 0;
    vertex_input_bindings.stride = 3 * sizeof(lx_float_t);
    vertex_input_bindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertex_input_attributes[2];
    vertex_input_attributes[0].location = 0;
    vertex_input_attributes[0].binding = 0;
    vertex_input_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_input_attributes[0].offset = 0;

    vertex_input_attributes[1].location = 1;
    vertex_input_attributes[1].binding = 0;
    vertex_input_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertex_input_attributes[1].offset = 0;

    VkPipelineVertexInputStateCreateInfo vertex_inputinfo = {};
    vertex_inputinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_inputinfo.pNext = lx_null;
    vertex_inputinfo.vertexBindingDescriptionCount = 1;
    vertex_inputinfo.pVertexBindingDescriptions = &vertex_input_bindings;
    vertex_inputinfo.vertexAttributeDescriptionCount = 2;
    vertex_inputinfo.pVertexAttributeDescriptions = vertex_input_attributes;

    // init pipeline layout info
    VkPipelineLayoutCreateInfo pipeline_layoutinfo;
	pipeline_layoutinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layoutinfo.pNext = lx_null;
	pipeline_layoutinfo.setLayoutCount = 0;
	pipeline_layoutinfo.pSetLayouts = lx_null;
	pipeline_layoutinfo.pushConstantRangeCount = 0;
	pipeline_layoutinfo.pPushConstantRanges = lx_null;

    // get pipeline
    return lx_vk_pipeline_get(device, LX_VK_PIPELINE_TYPE_TEXTURE,
        vshader, sizeof(vshader), fshader, sizeof(fshader), &vertex_inputinfo, &pipeline_layoutinfo);
}

lx_void_t lx_vk_pipeline_exit(lx_vk_pipeline_ref_t self) {
    lx_vk_pipeline_t* pipeline = (lx_vk_pipeline_t*)self;
    if (pipeline) {
        lx_assert(pipeline->device && pipeline->device->device);
        if (pipeline->pipeline) {
            vkDestroyPipeline(pipeline->device->device, pipeline->pipeline, lx_null);
            pipeline->pipeline = 0;
        }
        if (pipeline->pipeline_cache) {
            vkDestroyPipelineCache(pipeline->device->device, pipeline->pipeline_cache, lx_null);
            pipeline->pipeline_cache = 0;
        }
        if (pipeline->pipeline_layout) {
            vkDestroyPipelineLayout(pipeline->device->device, pipeline->pipeline_layout, lx_null);
            pipeline->pipeline_layout = 0;
        }
        lx_free(pipeline);
    }
}

VkPipeline lx_vk_pipeline_native(lx_vk_pipeline_ref_t self) {
    lx_vk_pipeline_t* pipeline = (lx_vk_pipeline_t*)self;
    return pipeline? pipeline->pipeline : 0;
}

VkPipelineLayout lx_vk_pipeline_layout(lx_vk_pipeline_ref_t self) {
    lx_vk_pipeline_t* pipeline = (lx_vk_pipeline_t*)self;
    return pipeline? pipeline->pipeline_layout : 0;
}
