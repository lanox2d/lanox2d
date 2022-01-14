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
#include "allocator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the pipeline type
typedef struct lx_vk_pipeline_t {
    lx_size_t               type;
    VkPipeline              pipeline;
    VkPipelineCache         pipeline_cache;
    VkPipelineLayout        pipeline_layout;
    VkDescriptorPool        descriptor_pool;
	lx_size_t               descriptor_count;
    VkDescriptorSetLayout   descriptor_set_layout;
    VkDescriptorSet         descriptor_sets[16];
    lx_size_t               descriptor_sets_count;
    lx_vulkan_device_t*     device;
    lx_vk_buffer_t          ubo_matrix[2];
}lx_vk_pipeline_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_vk_pipeline_t* lx_vk_pipeline_init(lx_vulkan_device_t* device, lx_size_t type) {
    lx_bool_t ok = lx_false;
    lx_vk_pipeline_t* pipeline = lx_null;
    do {
        pipeline = lx_malloc0_type(lx_vk_pipeline_t);
        lx_assert_and_check_break(pipeline);

        pipeline->type             = type;
        pipeline->device           = device;
        pipeline->descriptor_count = device->images_count;
        lx_assert_and_check_break(pipeline->descriptor_count <= lx_arrayn(pipeline->ubo_matrix));

        lx_size_t i;
        for (i = 0; i < pipeline->descriptor_count; i++) {
            if (!lx_vk_allocator_alloc(device->allocator_uniform, sizeof(lx_vk_ubo_matrix_t), &pipeline->ubo_matrix[i])) {
                break;
            }
        }
        lx_assert_and_check_break(i != pipeline->descriptor_count);

        ok = lx_true;
    } while (0);
    if (!ok && pipeline) {
        lx_vk_pipeline_exit((lx_vk_pipeline_ref_t)pipeline);
        pipeline = lx_null;
    }
    return pipeline;
}
static lx_bool_t lx_vk_pipeline_create(lx_vk_pipeline_t* pipeline,
    lx_char_t const* vshader, lx_size_t vshader_size, lx_char_t const* fshader, lx_size_t fshader_size,
    VkPipelineVertexInputStateCreateInfo* vertex_input_info, VkPipelineLayoutCreateInfo* pipeline_layout_info) {
    lx_assert_and_check_return_val(pipeline && pipeline->device, lx_false);

    lx_bool_t ok = lx_false;
    VkShaderModule vshader_module = 0;
    VkShaderModule fshader_module = 0;
    lx_vulkan_device_t* device = pipeline->device;
    do {
        // create pipeline layout
        if (vkCreatePipelineLayout(device->device, pipeline_layout_info, lx_null, &pipeline->pipeline_layout) != VK_SUCCESS) {
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
        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
        input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_info.pNext = lx_null;
        input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_info.primitiveRestartEnable = VK_FALSE;

        // create the pipeline cache
        VkPipelineCacheCreateInfo cache_info = {};
        cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        cache_info.pNext = lx_null;
        cache_info.flags = 0; // reserved, must be 0
        cache_info.initialDataSize = 0;
        cache_info.pInitialData = lx_null;
        if (vkCreatePipelineCache(device->device, &cache_info, lx_null, &pipeline->pipeline_cache) != VK_SUCCESS) {
            break;
        }

        // create the pipeline
        VkGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext = lx_null;
        pipeline_info.flags = 0;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly_info;
        pipeline_info.pTessellationState = lx_null;
        pipeline_info.pViewportState = &viewport_info;
        pipeline_info.pRasterizationState = &raster_info;
        pipeline_info.pMultisampleState = &multisample_info;
        pipeline_info.pDepthStencilState = lx_null;
        pipeline_info.pColorBlendState = &color_blendinfo;
        pipeline_info.pDynamicState = lx_null;
        pipeline_info.layout = pipeline->pipeline_layout;
        pipeline_info.renderPass = device->renderpass;
        pipeline_info.subpass = 0;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_info.basePipelineIndex = 0;
        if (vkCreateGraphicsPipelines(device->device, pipeline->pipeline_cache, 1, &pipeline_info, lx_null, &pipeline->pipeline) != VK_SUCCESS) {
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
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * pipelines
 */
#include "pipelines/solid.c"
#include "pipelines/texture.c"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_vk_pipeline_exit(lx_vk_pipeline_ref_t self) {
    lx_vk_pipeline_t* pipeline = (lx_vk_pipeline_t*)self;
    if (pipeline) {
        lx_vulkan_device_t* device = pipeline->device;
        lx_assert(device && device->device);

        // free ubo
        lx_size_t i;
        for (i = 0; i < pipeline->descriptor_count; i++) {
            lx_vk_allocator_free(device->allocator_uniform, &pipeline->ubo_matrix[i]);
        }

        // free descriptor set
        if (pipeline->descriptor_pool) {
            if (pipeline->descriptor_sets_count) {
                vkFreeDescriptorSets(device->device, pipeline->descriptor_pool, pipeline->descriptor_sets_count, pipeline->descriptor_sets);
                pipeline->descriptor_sets_count = 0;
            }
            if (pipeline->descriptor_set_layout) {
                vkDestroyDescriptorSetLayout(device->device, pipeline->descriptor_set_layout, lx_null);
                pipeline->descriptor_set_layout = 0;
            }
            vkDestroyDescriptorPool(device->device, pipeline->descriptor_pool, lx_null);
            pipeline->descriptor_pool = 0;
        }

        // free pipeline
        if (pipeline->pipeline) {
            vkDestroyPipeline(device->device, pipeline->pipeline, lx_null);
            pipeline->pipeline = 0;
        }
        if (pipeline->pipeline_cache) {
            vkDestroyPipelineCache(device->device, pipeline->pipeline_cache, lx_null);
            pipeline->pipeline_cache = 0;
        }
        if (pipeline->pipeline_layout) {
            vkDestroyPipelineLayout(device->device, pipeline->pipeline_layout, lx_null);
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
