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

