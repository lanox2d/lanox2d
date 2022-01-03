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
    VkDeviceMemory          device_memory;
    lx_bool_t               inited;
    lx_pointer_t            mapped_data;
    lx_size_t               mapped_offset;
    lx_size_t               mapped_size;
}lx_vk_buffer_chunk_t;

// the vulkan allocator type
typedef struct lx_vk_allocator_t {
    lx_vulkan_device_t*     device;
    lx_vk_buffer_chunk_t    chunks[LX_VK_BUFFER_CHUNK_MAXN];
    lx_size_t               chunks_active_index;
    VkBufferUsageFlagBits   buffer_type;
}lx_vk_allocator_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_vk_map_memory_type_to_index(lx_vk_allocator_t* allocator, lx_uint32_t type_bits, VkFlags requirements_mask, lx_uint32_t* type_index) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(allocator->device->gpu_device, &memory_properties);
    // search memtypes to find first index with those properties
    for (lx_uint32_t i = 0; i < 32; i++) {
        if ((type_bits & 1) == 1) {
            // type is available, does it match user properties?
            if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *type_index = i;
                return lx_true;
            }
        }
        type_bits >>= 1;
    }
    return lx_false;
}

static lx_bool_t lx_vk_buffer_chunk_init(lx_vk_allocator_t* allocator, lx_vk_buffer_chunk_t* chunk) {
    lx_assert(allocator && allocator->device && chunk);

    lx_bool_t ok = lx_false;
    do {
        // create buffer
        lx_vulkan_device_t* device = allocator->device;
        VkBufferCreateInfo create_buffer_info = {};
        create_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        create_buffer_info.pNext = lx_null;
        create_buffer_info.flags = 0;
        create_buffer_info.size = LX_VK_BUFFER_CHUNK_SIZE;
        create_buffer_info.usage = allocator->buffer_type;
        create_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_buffer_info.queueFamilyIndexCount = 1;
        create_buffer_info.pQueueFamilyIndices = &device->gpu_familyidx;
        if (vkCreateBuffer(device->device, &create_buffer_info, lx_null, &chunk->buffer) != VK_SUCCESS) {
            lx_trace_e("create vulkan buffer failed!");
            break;
        }

        VkMemoryRequirements memory_req;
        vkGetBufferMemoryRequirements(device->device, chunk->buffer, &memory_req);

        // assign the proper memory type for that buffer
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.pNext = lx_null;
        alloc_info.allocationSize = memory_req.size;
        alloc_info.memoryTypeIndex = 0;
        if (!lx_vk_map_memory_type_to_index(allocator, memory_req.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &alloc_info.memoryTypeIndex)) {
            break;
        }

        // allocate memory for the buffer
        if (vkAllocateMemory(device->device, &alloc_info, lx_null, &chunk->device_memory) != VK_SUCCESS) {
            lx_trace_e("allocate vulkan device memory failed!");
            break;
        }

        // bind buffer memory
        if (vkBindBufferMemory(device->device, chunk->buffer, chunk->device_memory, 0) != VK_SUCCESS) {
            lx_trace_e("bind vulkan buffer memory failed!");
            break;
        }

        chunk->inited = lx_true;
        ok = lx_true;
    } while (0);
    return ok;
}

static lx_void_t lx_vk_buffer_chunk_exit(lx_vk_allocator_t* allocator, lx_vk_buffer_chunk_t* chunk) {
    if (chunk->inited) {
        lx_vulkan_device_t* device = allocator->device;
        lx_assert(device);
        if (chunk->mapped_data) {
            vkUnmapMemory(device->device, chunk->device_memory);
            chunk->mapped_data = lx_null;
        }
        vkFreeMemory(allocator->device->device, chunk->device_memory, lx_null);
        vkDestroyBuffer(allocator->device->device, chunk->buffer, lx_null);
        chunk->inited = lx_false;
    }
}

static lx_bool_t lx_vk_buffer_chunk_alloc(lx_vk_allocator_t* allocator, lx_vk_buffer_chunk_t* chunk, lx_size_t size, lx_vk_buffer_t* buffer) {
    if (!chunk->inited && !lx_vk_buffer_chunk_init(allocator, chunk)) {
        return lx_false;
    }
    buffer->chunk  = chunk;
    buffer->offset = 0;
    buffer->size   = size;
    buffer->buffer = &chunk->buffer;
    return lx_true;
}

static lx_void_t lx_vk_buffer_chunk_free(lx_vk_allocator_t* allocator, lx_vk_buffer_chunk_t* chunk, lx_vk_buffer_t* buffer) {
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_allocator_ref_t lx_vk_allocator_init(lx_vulkan_device_t* device, VkBufferUsageFlagBits buffer_type) {
    lx_assert_and_check_return_val(device, lx_null);

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
        lx_size_t i;
        for (i = 0; i < lx_arrayn(allocator->chunks); i++) {
            lx_vk_buffer_chunk_exit(allocator, &allocator->chunks[i]);
        }
        lx_free(allocator);
    }
}

lx_bool_t lx_vk_allocator_alloc(lx_vk_allocator_ref_t self, lx_size_t size, lx_vk_buffer_t* buffer) {
    lx_vk_allocator_t* allocator = (lx_vk_allocator_t*)self;
    lx_assert(allocator && size && buffer);
    lx_assert(allocator->chunks_active_index < lx_arrayn(allocator->chunks));

    lx_size_t i = allocator->chunks_active_index;
    lx_bool_t ok = lx_vk_buffer_chunk_alloc(allocator, &allocator->chunks[i], size, buffer);
    if (!ok) {
        for (i = 0; i < lx_arrayn(allocator->chunks); i++) {
            if (lx_vk_buffer_chunk_alloc(allocator, &allocator->chunks[i], size, buffer)) {
                ok = lx_true;
                break;
            }
        }
    }
    return ok;
}

lx_void_t lx_vk_allocator_free(lx_vk_allocator_ref_t self, lx_vk_buffer_t* buffer) {
    lx_vk_allocator_t* allocator = (lx_vk_allocator_t*)self;
    lx_assert(allocator && buffer && buffer->chunk);

    lx_vk_buffer_chunk_free(allocator, buffer->chunk, buffer);
}

lx_pointer_t lx_vk_allocator_data(lx_vk_allocator_ref_t self, lx_vk_buffer_t* buffer) {
    lx_vk_allocator_t* allocator = (lx_vk_allocator_t*)self;
    lx_assert(allocator && buffer && buffer->chunk);

    // this buffer has been mapped?
    lx_vk_buffer_chunk_t* chunk = (lx_vk_buffer_chunk_t*)buffer->chunk;
    if (chunk->mapped_data && chunk->mapped_offset == buffer->offset && chunk->mapped_size == buffer->size) {
        return chunk->mapped_data;
    }

    // unmap the previous buffer
    lx_vulkan_device_t* device = allocator->device;
    lx_assert(device);
    if (chunk->mapped_data) {
        vkUnmapMemory(device->device, chunk->device_memory);
        chunk->mapped_data = lx_null;
    }

    // map the buffer data
    if (vkMapMemory(device->device, chunk->device_memory, buffer->offset, buffer->size, 0, &chunk->mapped_data) != VK_SUCCESS) {
        return lx_null;
    }
    chunk->mapped_offset = buffer->offset;
    chunk->mapped_size = buffer->size;
    return chunk->mapped_data;
}
