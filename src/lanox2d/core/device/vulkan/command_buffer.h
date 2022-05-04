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
 * @param device            the vulkan device
 * @param cmdbuffer         the native command buffer
 *
 * @return                  the command buffer
 */
lx_vk_command_buffer_ref_t  lx_vk_command_buffer_init(lx_vulkan_device_t* device, VkCommandBuffer cmdbuffer);

/* exit command buffer
 *
 * @param command_buffer    the command buffer
 */
lx_void_t                   lx_vk_command_buffer_exit(lx_vk_command_buffer_ref_t command_buffer);

/* get the vulkan command buffer
 *
 * @param command_buffer    the command buffer
 *
 * @return                  the native command buffer
 */
VkCommandBuffer             lx_vk_command_buffer(lx_vk_command_buffer_ref_t command_buffer);

/* set the vulkan command buffer
 *
 * @param command_buffer    the command buffer
 * @param cmdbuffer         the native command buffer
 */
lx_void_t                   lx_vk_command_buffer_set(lx_vk_command_buffer_ref_t command_buffer, VkCommandBuffer cmdbuffer);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


