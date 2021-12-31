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
 * macros
 */

// 64 * 4M = 256M
#define LX_VK_BUFFER_CHUNK_MAXN     (64)
#define LX_VK_BUFFER_CHUNK_SIZE     (1024 * 4096)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the buffer chunk type
typedef struct lx_vk_buffer_chunk_t_ {
    VkBuffer                buffer;
    lx_size_t               free_size;
    lx_size_t               total_size;
}lx_vk_buffer_chunk_t;

// the vulkan allocator type
typedef struct lx_vk_allocator_t {
    lx_vulkan_device_t*     device;
    lx_vk_buffer_chunk_t    chunks[LX_VK_BUFFER_CHUNK_MAXN];
    lx_size_t               chunks_count;
    lx_size_t               chunks_active_index;
    VkBufferUsageFlagBits   buffer_type;
}lx_vk_allocator_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_vk_buffer_chunk_t* lx_vk_buffer_chunk_select(lx_vk_allocator_t* allocator, lx_size_t size) {
    return lx_null;
}

static lx_bool_t lx_vk_buffer_chunk_alloc(lx_vk_allocator_t* allocator, lx_vk_buffer_chunk_t* chunk, lx_size_t size, lx_vk_buffer_t* buffer) {
    return lx_false;
}

static lx_void_t lx_vk_buffer_chunk_free(lx_vk_allocator_t* allocator, lx_vk_buffer_chunk_t* chunk, lx_vk_buffer_t* buffer) {
}

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

lx_bool_t lx_vk_allocator_alloc(lx_vk_allocator_ref_t self, lx_size_t size, lx_vk_buffer_t* buffer) {
    lx_vk_allocator_t* allocator = (lx_vk_allocator_t*)self;
    lx_assert(allocator && size && buffer);
//    lx_assert_and_check_return(allocator->chunks_active_index < allocator->chunks_count);

    lx_vk_buffer_chunk_t* chunk = lx_vk_buffer_chunk_select(allocator, size);
    if (chunk) {
        return lx_vk_buffer_chunk_alloc(allocator, chunk, size, buffer);
    }
    return lx_false;
}

lx_void_t lx_vk_allocator_free(lx_vk_allocator_ref_t self, lx_vk_buffer_t* buffer) {
    lx_vk_allocator_t* allocator = (lx_vk_allocator_t*)self;
    lx_assert(allocator && buffer);
    lx_assert_and_check_return(buffer->chunk_id < allocator->chunks_count);

    lx_vk_buffer_chunk_free(allocator, &allocator->chunks[buffer->chunk_id], buffer);
}
