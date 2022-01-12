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

        // fragment.aColor as push-constant
        VkPushConstantRange push_constant_range = {};
        push_constant_range.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
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

        pipeline = lx_vk_pipeline_init(device, type,
            vshader, sizeof(vshader), fshader, sizeof(fshader), &vertex_inputinfo, &pipeline_layoutinfo);
        device->pipelines[type] = pipeline;
    }
    return pipeline;
}

