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
 * @file        allocator.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "allocator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the vulkan allocator type
typedef struct lx_vk_allocator_t {
    lx_vulkan_device_t*     device;
    VkBufferUsageFlagBits   buffer_type;
}lx_vk_allocator_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_allocator_ref_t lx_vk_allocator_init(lx_vulkan_device_t* device, VkBufferUsageFlagBits buffer_type) {
    lx_bool_t ok = lx_false;
    lx_vk_allocator_t* allocator = lx_null;
    do {
        // init allocator
        allocator = lx_malloc0_type(lx_vk_allocator_t);
        lx_assert_and_check_break(allocator);

        allocator->device      = device;
        allocator->buffer_type = buffer_type;

        ok = lx_true;
    } while (0);
    if (!ok && allocator) {
        lx_vk_allocator_exit((lx_vk_allocator_ref_t)allocator);
        allocator = lx_null;
    }
    return (lx_vk_allocator_ref_t)allocator;
}

lx_void_t lx_vk_allocator_exit(lx_vk_allocator_ref_t self) {
    lx_vk_allocator_t* allocator = (lx_vk_allocator_t*)self;
    if (self) {
        lx_free(allocator);
    }
}

