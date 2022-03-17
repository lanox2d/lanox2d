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
 * @file        buffer_allocator.cc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "buffer_allocator.h"
#define VMA_IMPLEMENTATION
#define VMA_ASSERT lx_assert
#include <vk_mem_alloc.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// 64 * 4M = 256M
#define LX_VK_BUFFER_CHUNK_MAXN     (64)
#define LX_VK_BUFFER_CHUNK_SIZE     (1024 * 4096)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the vulkan buffer allocator type
typedef struct lx_vk_buffer_allocator_t {
    lx_vulkan_device_t*     device;
    VkBufferUsageFlagBits   buffer_type;
    VmaAllocator            allocator;
}lx_vk_buffer_allocator_t;

// the vulkan/vma buffer type
typedef struct lx_vk_vma_buffer_t_ {
    VkBuffer        buffer;
    lx_size_t       offset;
    lx_size_t       size;
    VmaAllocation   allocation;
}lx_vk_vma_buffer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_buffer_allocator_ref_t lx_vk_buffer_allocator_init(lx_vulkan_device_t* device, VkBufferUsageFlagBits buffer_type) {
    lx_assert_and_check_return_val(device, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_buffer_allocator_t* allocator = lx_null;
    do {
        // init allocator
        allocator = lx_malloc0_type(lx_vk_buffer_allocator_t);
        lx_assert_and_check_break(allocator);

        allocator->device      = device;
        allocator->buffer_type = buffer_type;

        // initialize the memory allocator
        VmaAllocatorCreateInfo allocator_info = {};
        allocator_info.physicalDevice = device->gpu_device;
        allocator_info.device = device->device;
        allocator_info.instance = device->instance;
#if defined(VMA_DYNAMIC_VULKAN_FUNCTIONS) && VMA_DYNAMIC_VULKAN_FUNCTIONS == 1
        // we need init it on android, @see https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/issues/56
        VmaVulkanFunctions fn = {};
        fn.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        fn.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        allocator_info.pVulkanFunctions = &fn;
#endif
        if (vmaCreateAllocator(&allocator_info, &allocator->allocator) != VK_SUCCESS) {
            break;
        }
        lx_assert_static(sizeof(lx_vk_vma_buffer_t) <= sizeof(lx_vk_buffer_t));

        ok = lx_true;
    } while (0);
    if (!ok && allocator) {
        lx_vk_buffer_allocator_exit((lx_vk_buffer_allocator_ref_t)allocator);
        allocator = lx_null;
    }
    return (lx_vk_buffer_allocator_ref_t)allocator;
}

lx_void_t lx_vk_buffer_allocator_exit(lx_vk_buffer_allocator_ref_t self) {
    lx_vk_buffer_allocator_t* allocator = (lx_vk_buffer_allocator_t*)self;
    if (allocator) {
        vmaDestroyAllocator(allocator->allocator);
        lx_free(allocator);
    }
}

lx_bool_t lx_vk_buffer_allocator_alloc(lx_vk_buffer_allocator_ref_t self, lx_size_t size, lx_vk_buffer_t* buffer) {
    lx_vk_buffer_allocator_t* allocator = (lx_vk_buffer_allocator_t*)self;
    lx_vk_vma_buffer_t* vma_buffer = (lx_vk_vma_buffer_t*)buffer;
    lx_assert(allocator && size && vma_buffer);

    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = allocator->buffer_type;

    VmaAllocationCreateInfo vma_alloc_createinfo = {};
    vma_alloc_createinfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    return vmaCreateBuffer(allocator->allocator, &buffer_info, &vma_alloc_createinfo, &vma_buffer->buffer, &vma_buffer->allocation, lx_null) == VK_SUCCESS;
}

lx_void_t lx_vk_buffer_allocator_free(lx_vk_buffer_allocator_ref_t self, lx_vk_buffer_t* buffer) {
    lx_vk_buffer_allocator_t* allocator = (lx_vk_buffer_allocator_t*)self;
    lx_vk_vma_buffer_t* vma_buffer = (lx_vk_vma_buffer_t*)buffer;
    lx_assert(allocator && vma_buffer);

    vmaDestroyBuffer(allocator->allocator, vma_buffer->buffer, vma_buffer->allocation);
}

lx_void_t lx_vk_buffer_allocator_copy(lx_vk_buffer_allocator_ref_t self, lx_vk_buffer_t* buffer, lx_size_t pos, lx_pointer_t data, lx_size_t size) {
    lx_vk_buffer_allocator_t* allocator = (lx_vk_buffer_allocator_t*)self;
    lx_vk_vma_buffer_t* vma_buffer = (lx_vk_vma_buffer_t*)buffer;
    lx_assert(allocator && vma_buffer && vma_buffer->allocation && data && size);

#ifdef LX_DEBUG
    VmaAllocationInfo vma_allocinfo;
    vmaGetAllocationInfo(allocator->allocator, vma_buffer->allocation, &vma_allocinfo);
    lx_assert(pos + size <= vma_allocinfo.size);
#endif

    lx_pointer_t buffer_data = lx_null;
    vmaMapMemory(allocator->allocator, vma_buffer->allocation, &buffer_data);
    if (buffer_data) {
        lx_memcpy((lx_byte_t*)buffer_data + pos, data, size);
        vmaUnmapMemory(allocator->allocator, vma_buffer->allocation);
    }
}
