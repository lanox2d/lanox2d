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
 * private implementation
 */
static lx_bool_t lx_vk_descriptor_sets_init_texture(lx_vulkan_device_t* device, lx_vk_pipeline_t* pipeline,
    VkDescriptorSetLayoutBinding* bindings, lx_uint32_t bindings_count) {
    lx_assert(bindings_count < lx_arrayn(pipeline->descriptor_sets));

    // create descriptor set layout
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info = {};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_info.pNext = lx_null;
    descriptor_set_layout_info.bindingCount = bindings_count;
    descriptor_set_layout_info.pBindings = bindings;
    if (vkCreateDescriptorSetLayout(device->device, &descriptor_set_layout_info, lx_null, &pipeline->descriptor_set_layout) != VK_SUCCESS) {
        return lx_false;
    }

    // create descriptor pool
    lx_size_t i;
    VkDescriptorPoolSize pool_sizes[16] = {};
    lx_uint32_t pool_sizes_count = bindings_count;
    lx_assert(pool_sizes_count < lx_arrayn(pool_sizes));
    for (i = 0; i < bindings_count; i++) {
        pool_sizes[i].type = bindings[i].descriptorType;
        pool_sizes[i].descriptorCount = bindings[i].descriptorCount;
    }

    const lx_uint32_t descriptor_sets_count = 2;
    VkDescriptorPoolCreateInfo descriptor_poolinfo = {};
    descriptor_poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_poolinfo.pNext = lx_null;
    descriptor_poolinfo.maxSets = descriptor_sets_count;
    descriptor_poolinfo.poolSizeCount = pool_sizes_count;
    descriptor_poolinfo.pPoolSizes = pool_sizes;
    if (vkCreateDescriptorPool(device->device, &descriptor_poolinfo, lx_null, &pipeline->descriptor_pool) != VK_SUCCESS) {
        return lx_false;
    }

    // create descriptor sets
    VkDescriptorSetAllocateInfo descriptor_setsinfo = {};
    descriptor_setsinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_setsinfo.pNext = lx_null;
    descriptor_setsinfo.descriptorPool = pipeline->descriptor_pool;
    descriptor_setsinfo.descriptorSetCount = descriptor_sets_count;
    descriptor_setsinfo.pSetLayouts = &pipeline->descriptor_set_layout;
    if (vkAllocateDescriptorSets(device->device, &descriptor_setsinfo, pipeline->descriptor_sets) != VK_SUCCESS) {
        return lx_false;
    }
    pipeline->descriptor_sets_count = descriptor_sets_count;

    // configure descriptor sets: uniform buffer (matrix and image)
    for (i = 0; i < bindings_count; i++) {
        lx_uint32_t descriptor_type = bindings[i].descriptorType;
        VkDescriptorBufferInfo descriptor_buffer_info[1];
        VkDescriptorImageInfo descriptor_image_info[1];
        VkWriteDescriptorSet write_descriptor_set = {};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.pNext = lx_null;
        write_descriptor_set.dstSet = pipeline->descriptor_sets[i];
        write_descriptor_set.dstBinding = 0;
        write_descriptor_set.dstArrayElement = 0;
        write_descriptor_set.descriptorCount = bindings[i].descriptorCount;
        write_descriptor_set.descriptorType = descriptor_type;
        write_descriptor_set.pTexelBufferView = lx_null;
        if (descriptor_type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            lx_assert(lx_arrayn(descriptor_buffer_info) == bindings[i].descriptorCount);
            lx_memset(descriptor_buffer_info, 0, sizeof(VkDescriptorBufferInfo) * bindings[i].descriptorCount);
            descriptor_buffer_info[0].buffer = pipeline->ubo_matrix.buffer;
            descriptor_buffer_info[0].offset = 0;
            descriptor_buffer_info[0].range = sizeof(lx_vk_ubo_texture_matrix_t);
            write_descriptor_set.pBufferInfo = descriptor_buffer_info;
            write_descriptor_set.pImageInfo = lx_null;
        } else if (descriptor_type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
            lx_assert(lx_arrayn(descriptor_image_info) == bindings[1].descriptorCount);
            lx_memset(descriptor_image_info, 0, sizeof(VkDescriptorImageInfo) * bindings[1].descriptorCount);
#if 0
            // TODO
            descriptor_image_info[0].sampler = devdata->sampler;
            descriptor_image_info[0].imageView = devdata->imageview;
#endif
            descriptor_image_info[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            write_descriptor_set.pImageInfo = descriptor_image_info;
            write_descriptor_set.pBufferInfo = lx_null;
        }
        vkUpdateDescriptorSets(device->device, 1, &write_descriptor_set, 0, lx_null);
    }
    return lx_true;
}

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

            // init descriptor sets, uniform buffers: matrix, sampler
            VkDescriptorSetLayoutBinding descriptor_set_layout_bindings[2] = {};
            descriptor_set_layout_bindings[0].binding = 0;
            descriptor_set_layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_set_layout_bindings[0].descriptorCount = 1;
            descriptor_set_layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            descriptor_set_layout_bindings[0].pImmutableSamplers = lx_null;

            descriptor_set_layout_bindings[1].binding = 0;
            descriptor_set_layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_set_layout_bindings[1].descriptorCount = 1;
            descriptor_set_layout_bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            descriptor_set_layout_bindings[1].pImmutableSamplers = lx_null;

            // init descriptor sets
            if (!lx_vk_descriptor_sets_init_texture(device, pipeline_texture,
                    descriptor_set_layout_bindings, lx_arrayn(descriptor_set_layout_bindings))) {
                break;
            }

            // create pipeline
            VkPipelineLayoutCreateInfo pipeline_layout_info = {};
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.pNext = lx_null;
            pipeline_layout_info.setLayoutCount = 1;
            pipeline_layout_info.pSetLayouts = &pipeline_texture->descriptor_set_layout;
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


