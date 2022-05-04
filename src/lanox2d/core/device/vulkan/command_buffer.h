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
 * @file        command_buffer.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_COMMAND_BUFFER_H
#define LX_CORE_DEVICE_VULKAN_COMMAND_BUFFER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init command buffer
 *
 * @param device                the vulkan device
 * @param cmdbuffer             the native command buffer
 *
 * @return                      the command buffer
 */
lx_vk_command_buffer_ref_t      lx_vk_command_buffer_init(lx_vulkan_device_t* device, VkCommandBuffer cmdbuffer);

/* exit command buffer
 *
 * @param command_buffer        the command buffer
 */
lx_void_t                       lx_vk_command_buffer_exit(lx_vk_command_buffer_ref_t command_buffer);

/* get the vulkan command buffer
 *
 * @param command_buffer        the command buffer
 *
 * @return                      the native command buffer
 */
VkCommandBuffer                 lx_vk_command_buffer(lx_vk_command_buffer_ref_t command_buffer);

/* set the vulkan command buffer
 *
 * @param command_buffer        the command buffer
 * @param cmdbuffer             the native command buffer
 */
lx_void_t                       lx_vk_command_buffer_set(lx_vk_command_buffer_ref_t command_buffer, VkCommandBuffer cmdbuffer);

/* bind pipeline
 *
 * @param command_buffer        the command buffer
 * @param pipeline              the pipeline
 */
lx_void_t                       lx_vk_command_buffer_bind_pipeline(lx_vk_command_buffer_ref_t command_buffer, lx_vk_pipeline_ref_t pipeline);

/* bind descriptor sets
 *
 * @param command_buffer        the command buffer
 * @param pipeline              the pipeline
 * @param first_set             the first set
 * @param set_count             the set count
 * @param descriptor_sets       the descriptor sets
 * @param dynamic_offset_count  the dynamic offset count
 * @param dynamic_offsets       the dynamic offsets
 */
lx_void_t                       lx_vk_command_buffer_bind_descriptor_sets(lx_vk_command_buffer_ref_t command_buffer,
                                                                          lx_vk_pipeline_ref_t pipeline,
                                                                          lx_uint32_t first_set,
                                                                          lx_uint32_t set_count,
                                                                          VkDescriptorSet const* descriptor_sets,
                                                                          lx_uint32_t dynamic_offset_count,
                                                                          lx_uint32_t const* dynamic_offsets);
/* bind vertex buffers
 *
 * @param command_buffer        the command buffer
 * @param first_binding         the first binding
 * @param binding_count         the binding count
 * @param pbuffers              the buffers pointer
 * @param poffsets              the offsets pointer
 */
lx_void_t                       lx_vk_command_buffer_bind_vertex_buffers(lx_vk_command_buffer_ref_t command_buffer,
                                                                         lx_uint32_t first_binding, lx_uint32_t binding_count,
                                                                         VkBuffer const* pbuffers, VkDeviceSize const* poffsets);
/* push constants
 *
 * @param command_buffer        the command buffer
 * @param pipeline              the pipeline
 * @param stage                 the stage flags
 * @param offset                the values offset
 * @param size                  the values size
 * @param values                the values
 */
lx_void_t                       lx_vk_command_buffer_push_constants(lx_vk_command_buffer_ref_t command_buffer,
                                                                    lx_vk_pipeline_ref_t pipeline, VkShaderStageFlags stage_flags,
                                                                    lx_uint32_t offset, lx_uint32_t size, lx_cpointer_t values);

/* draw vertice
 *
 * @param command_buffer        the command buffer
 * @param vertex_count          the vertex count
 * @param instance_count        the instance count
 * @param first_vertex          the first vertex
 * @param first_instance        the first instance
 */
lx_void_t                       lx_vk_command_buffer_draw(lx_vk_command_buffer_ref_t command_buffer,
                                                          lx_uint32_t vertex_count, lx_uint32_t instance_count,
                                                          lx_uint32_t first_vertex, lx_uint32_t first_instance);

/* draw vertice with indirect mode
 *
 * @param command_buffer        the command buffer
 * @param buffer                the vertex buffer
 * @param offset                the vertex offset
 * @param draw_count            the draw count
 * @param stride                the buffer stride
 */
lx_void_t                       lx_vk_command_buffer_draw_indirect(lx_vk_command_buffer_ref_t command_buffer,
                                                                   VkBuffer buffer, VkDeviceSize offset,
                                                                   lx_uint32_t draw_count, lx_uint32_t stride);


/* end render pass
 *
 * @param command_buffer        the command buffer
 */
lx_void_t                       lx_vk_command_buffer_end_render_pass(lx_vk_command_buffer_ref_t command_buffer);

/* end command buffer
 *
 * @param command_buffer        the command buffer
 */
lx_void_t                       lx_vk_command_buffer_end(lx_vk_command_buffer_ref_t command_buffer);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


