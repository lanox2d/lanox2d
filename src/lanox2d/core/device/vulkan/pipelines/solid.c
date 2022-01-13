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
 * @file        solid.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_vk_descriptor_sets_init(lx_vulkan_device_t* device, lx_vk_pipeline_t* pipeline) {

    // create descriptor pool
    VkDescriptorPoolSize pool_size = {};
    pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    pool_size.descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptor_poolinfo = {};
    descriptor_poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_poolinfo.pNext = lx_null;
    descriptor_poolinfo.maxSets = 1;
    descriptor_poolinfo.poolSizeCount = 1;
    descriptor_poolinfo.pPoolSizes = &pool_size;

    if (vkCreateDescriptorPool(device->device, &descriptor_poolinfo, lx_null, &pipeline->descriptor_pool) != VK_SUCCESS) {
        return lx_false;
    }

    // create descriptor sets
    VkDescriptorSetAllocateInfo descriptor_setsinfo = {};
    descriptor_setsinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_setsinfo.pNext = lx_null;
    descriptor_setsinfo.descriptorPool = pipeline->descriptor_pool;
    descriptor_setsinfo.descriptorSetCount = 1;
    descriptor_setsinfo.pSetLayouts = &pipeline->descriptor_set_layout;
    if (vkAllocateDescriptorSets(device->device, &descriptor_setsinfo, pipeline->descriptor_sets) != VK_SUCCESS) {
        return lx_false;
    }
    pipeline->descriptor_sets_count = 1;

    return lx_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_vk_pipeline_ref_t lx_vk_pipeline_solid(lx_vulkan_device_t* device) {
    const lx_size_t type = LX_VK_PIPELINE_TYPE_SOLID;
    lx_assert(type < lx_arrayn(device->pipelines));
    lx_vk_pipeline_ref_t pipeline = device->pipelines[type];
    if (!pipeline) {
        static lx_char_t const vshader[] = {
#include "solid.vert.spv.h"
        };
        static lx_char_t const fshader[] = {
#include "solid.frag.spv.h"
        };

        lx_bool_t ok = lx_false;
        lx_vk_pipeline_t* pipeline_solid = lx_null;
        do {
            pipeline_solid = lx_vk_pipeline_init(device, type);
            lx_assert_and_check_break(pipeline_solid);

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

            VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
            vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input_info.pNext = lx_null;
            vertex_input_info.vertexBindingDescriptionCount = lx_arrayn(vertex_input_bindings);
            vertex_input_info.pVertexBindingDescriptions = vertex_input_bindings;
            vertex_input_info.vertexAttributeDescriptionCount = lx_arrayn(vertex_input_attributes);
            vertex_input_info.pVertexAttributeDescriptions = vertex_input_attributes;

            // init push-constant
            VkPushConstantRange push_constant_range = {};
            push_constant_range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            push_constant_range.offset = 0;
            push_constant_range.size = 4 * sizeof(lx_float_t);

            // init descriptor set layout
            VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {};
            descriptor_set_layout_binding.binding = 0;
            descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            descriptor_set_layout_binding.descriptorCount = 1;
            descriptor_set_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            descriptor_set_layout_binding.pImmutableSamplers = lx_null;

            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = {};
            descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_info.pNext = lx_null;
            descriptor_set_layout_info.bindingCount = 1;
            descriptor_set_layout_info.pBindings = &descriptor_set_layout_binding;
            if (vkCreateDescriptorSetLayout(device->device, &descriptor_set_layout_info, lx_null, &pipeline_solid->descriptor_set_layout) != VK_SUCCESS) {
                break;
            }

            // init pipeline layout info
            VkPipelineLayoutCreateInfo pipeline_layout_info;
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.pNext = lx_null;
            pipeline_layout_info.setLayoutCount = 1;
            pipeline_layout_info.pSetLayouts = &pipeline_solid->descriptor_set_layout;
            pipeline_layout_info.pushConstantRangeCount = 1;
            pipeline_layout_info.pPushConstantRanges = &push_constant_range;

            // init descriptor sets
            if (!lx_vk_descriptor_sets_init(device, pipeline_solid)) {
                break;
            }

            // create pipeline
            if (!lx_vk_pipeline_create(pipeline_solid,
                vshader, sizeof(vshader), fshader, sizeof(fshader), &vertex_input_info, &pipeline_layout_info)) {
                break;
            }

            // ok
            pipeline = (lx_vk_pipeline_ref_t)pipeline_solid;
            device->pipelines[type] = pipeline;
            ok = lx_true;
        } while (0);

        if (!ok && pipeline_solid) {
            lx_vk_pipeline_exit((lx_vk_pipeline_ref_t)pipeline_solid);
            pipeline_solid = lx_null;
        }
    }
    return pipeline;
}

