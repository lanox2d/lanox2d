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
 * @file        allocator.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_ALLOCATOR_H
#define LX_CORE_DEVICE_VULKAN_ALLOCATOR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the vulkan buffer type
typedef struct lx_vk_buffer_t_ {
    VkBuffer        buffer;
    lx_size_t       offset;
    lx_size_t       size;
    lx_byte_t       privdata[16]; // the private data, only for internal allocator
}lx_vk_buffer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the vulkan buffer allocator
 *
 * @param device        the vulkan device
 * @param buffer_type   the buffer type, e.g. VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, ...
 *
 * @return              the vulkan allocator
 */
lx_vk_allocator_ref_t   lx_vk_allocator_init(lx_vulkan_device_t* device, VkBufferUsageFlagBits buffer_type);

/*! exit the vulkan buffer allocator
 *
 * @param allocator     the vulkan allocator
 */
lx_void_t               lx_vk_allocator_exit(lx_vk_allocator_ref_t allocator);

/*! allocate the vulkan buffer
 *
 * @param allocator     the vulkan allocator
 * @param size          the required buffer size
 * @param buffer        the allocated buffer
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_vk_allocator_alloc(lx_vk_allocator_ref_t allocator, lx_size_t size, lx_vk_buffer_t* buffer);

/*! free the vulkan buffer
 *
 * @param allocator     the vulkan allocator
 * @param buffer        the allocated buffer
 */
lx_void_t               lx_vk_allocator_free(lx_vk_allocator_ref_t allocator, lx_vk_buffer_t* buffer);

/*! copy data to the vulkan buffer
 *
 * @param allocator     the vulkan allocator
 * @param buffer        the allocated buffer
 * @param data          the copied data
 * @param size          the copied data size
 */
lx_void_t               lx_vk_allocator_copy(lx_vk_allocator_ref_t allocator, lx_vk_buffer_t* buffer, lx_pointer_t data, lx_size_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


