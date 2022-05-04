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
 * @file        command_buffer.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "command_buffer.h"
#include "pipeline.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the command buffer type
typedef struct lx_vk_command_buffer_t {
    lx_vulkan_device_t*     device;
    VkCommandBuffer         cmdbuffer;
}lx_vk_command_buffer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_command_buffer_ref_t lx_vk_command_buffer_init(lx_vulkan_device_t* device, VkCommandBuffer cmdbuffer) {
    lx_assert_and_check_return_val(device, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_command_buffer_t* command_buffer = lx_null;
    do {
        command_buffer = lx_malloc0_type(lx_vk_command_buffer_t);
        lx_assert_and_check_break(command_buffer);

        command_buffer->device = device;
        command_buffer->cmdbuffer = cmdbuffer;

        ok = lx_true;
    } while (0);

    if (!ok && command_buffer) {
        lx_vk_command_buffer_exit((lx_vk_command_buffer_ref_t)command_buffer);
        command_buffer = lx_null;
    }
    return (lx_vk_command_buffer_ref_t)command_buffer;
}

lx_void_t lx_vk_command_buffer_exit(lx_vk_command_buffer_ref_t self) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    if (command_buffer) {
        command_buffer->cmdbuffer = lx_null;
        lx_free(command_buffer);
    }
}

VkCommandBuffer lx_vk_command_buffer(lx_vk_command_buffer_ref_t self) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    return command_buffer? command_buffer->cmdbuffer : VK_NULL_HANDLE;
}

lx_void_t lx_vk_command_buffer_set(lx_vk_command_buffer_ref_t self, VkCommandBuffer cmdbuffer) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    if (command_buffer) {
        command_buffer->cmdbuffer = cmdbuffer;
    }
}

lx_void_t lx_vk_command_buffer_bind_pipeline(lx_vk_command_buffer_ref_t self, lx_vk_pipeline_ref_t pipeline) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer && pipeline);

    vkCmdBindPipeline(command_buffer->cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, lx_vk_pipeline_native(pipeline));
}

lx_void_t lx_vk_command_buffer_bind_descriptor_sets(lx_vk_command_buffer_ref_t self,
                                                    lx_vk_pipeline_ref_t pipeline,
                                                    lx_uint32_t first_set,
                                                    lx_uint32_t set_count,
                                                    VkDescriptorSet const* descriptor_sets,
                                                    lx_uint32_t dynamic_offset_count,
                                                    lx_uint32_t const* dynamic_offsets) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer && pipeline);

    vkCmdBindDescriptorSets(command_buffer->cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        lx_vk_pipeline_layout(pipeline), first_set, set_count,
        descriptor_sets, dynamic_offset_count, dynamic_offsets);
}

lx_void_t lx_vk_command_buffer_bind_vertex_buffers(lx_vk_command_buffer_ref_t self,
                                                   lx_uint32_t first_binding, lx_uint32_t binding_count,
                                                   VkBuffer const* pbuffers, VkDeviceSize const* poffsets) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer && pbuffers);

    vkCmdBindVertexBuffers(command_buffer->cmdbuffer, first_binding, binding_count, pbuffers, poffsets);
}

lx_void_t lx_vk_command_buffer_push_constants(lx_vk_command_buffer_ref_t self,
                                              lx_vk_pipeline_ref_t pipeline, VkShaderStageFlags stage_flags,
                                              lx_uint32_t offset, lx_uint32_t size, lx_cpointer_t values) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer && pipeline);

    vkCmdPushConstants(command_buffer->cmdbuffer, lx_vk_pipeline_layout(pipeline), stage_flags, offset, size, values);
}

lx_void_t lx_vk_command_buffer_draw(lx_vk_command_buffer_ref_t self,
                                    lx_uint32_t vertex_count, lx_uint32_t instance_count,
                                    lx_uint32_t first_vertex, lx_uint32_t first_instance) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer);

    vkCmdDraw(command_buffer->cmdbuffer, vertex_count, instance_count, first_vertex, first_instance);
}

lx_void_t lx_vk_command_buffer_draw_indirect(lx_vk_command_buffer_ref_t self,
                                             VkBuffer buffer, VkDeviceSize offset,
                                             lx_uint32_t draw_count, lx_uint32_t stride) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer);

    vkCmdDrawIndirect(command_buffer->cmdbuffer, buffer, offset, draw_count, stride);
}

lx_void_t lx_vk_command_buffer_end_render_pass(lx_vk_command_buffer_ref_t self) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer);

    vkCmdEndRenderPass(command_buffer->cmdbuffer);
}

lx_void_t lx_vk_command_buffer_end(lx_vk_command_buffer_ref_t self) {
    lx_vk_command_buffer_t* command_buffer = (lx_vk_command_buffer_t*)self;
    lx_assert_and_check_return(command_buffer && command_buffer->cmdbuffer);

    vkEndCommandBuffer(command_buffer->cmdbuffer);
}

