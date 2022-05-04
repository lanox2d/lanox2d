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
 * @file        texture.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_pipeline_ref_t lx_vk_pipeline_texture(lx_vulkan_device_t* device) {
    const lx_size_t type = LX_VK_PIPELINE_TYPE_TEXTURE;
    lx_assert(type < lx_arrayn(device->pipelines));
    lx_vk_pipeline_ref_t pipeline = device->pipelines[type];
    if (!pipeline) {
        static lx_char_t const vshader[] = {
#include "texture.vert.spv.h"
        };
        static lx_char_t const fshader[] = {
#include "texture.frag.spv.h"
        };

        lx_bool_t ok = lx_false;
        lx_vk_pipeline_t* pipeline_texture = lx_null;
        do {
            pipeline_texture = lx_vk_pipeline_init(device, type);
            lx_assert_and_check_break(pipeline_texture);

            // init vertex input state
            VkVertexInputBindingDescription vertex_input_bindings[2];
            vertex_input_bindings[0].binding = 0; // for vertices buffer
            vertex_input_bindings[0].stride = 2 * sizeof(lx_float_t);
            vertex_input_bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            vertex_input_bindings[1].binding = 1; // for texcoord buffer
            vertex_input_bindings[1].stride = 2 * sizeof(lx_float_t);
            vertex_input_bindings[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            VkVertexInputAttributeDescription vertex_input_attributes[2];
            vertex_input_attributes[0].location = 0; // layout(location = 0) in vec4 aVertices;
            vertex_input_attributes[0].binding = 0;
            vertex_input_attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
            vertex_input_attributes[0].offset = 0;

            vertex_input_attributes[1].location = 1; // layout(location = 1) in vec4 aTexcoords;
            vertex_input_attributes[1].binding = 1;
            vertex_input_attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
            vertex_input_attributes[1].offset = 0;

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

            // create pipeline, TODO
            VkPipelineLayoutCreateInfo pipeline_layout_info = {};
            VkDescriptorSetLayout descriptor_set_layouts[2];
            descriptor_set_layouts[0] = lx_vk_descriptor_sets_layout(device->descriptor_sets_uniform);
            descriptor_set_layouts[1] = lx_vk_descriptor_sets_layout(device->descriptor_sets_sampler);
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.pNext = lx_null;
            pipeline_layout_info.setLayoutCount = lx_arrayn(descriptor_set_layouts);
            pipeline_layout_info.pSetLayouts = descriptor_set_layouts;
            pipeline_layout_info.pushConstantRangeCount = 1;
            pipeline_layout_info.pPushConstantRanges = &push_constant_range;
            if (!lx_vk_pipeline_create(pipeline_texture, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                vshader, sizeof(vshader), fshader, sizeof(fshader), &vertex_input_info, &pipeline_layout_info)) {
                break;
            }

            // ok
            pipeline = (lx_vk_pipeline_ref_t)pipeline_texture;
            device->pipelines[type] = pipeline;
            ok = lx_true;
        } while (0);

        if (!ok && pipeline_texture) {
            lx_vk_pipeline_exit((lx_vk_pipeline_ref_t)pipeline_texture);
            pipeline_texture = lx_null;
        }
    }
    return pipeline;
}


