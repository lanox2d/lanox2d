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
#include "descriptor_sets.h"
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
    VkDescriptorSet descriptor_set_uniform;
}lx_vk_vma_buffer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static VkDescriptorSet lx_vk_buffer_alloc_uniform_descriptor_set(lx_vulkan_device_t* device, VkBuffer buffer, lx_size_t size) {
    lx_assert(device && device->descriptor_sets_uniform);

    VkDescriptorSet descriptor_set = lx_vk_descriptor_sets_alloc(device->descriptor_sets_uniform);
    if (descriptor_set != VK_NULL_HANDLE) {
        VkDescriptorBufferInfo bufferinfo;
        lx_memset(&bufferinfo, 0, sizeof(VkDescriptorBufferInfo));
        bufferinfo.buffer = buffer;
        bufferinfo.offset = 0;
        bufferinfo.range = size;

        VkWriteDescriptorSet write_descriptor_set = {};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.pNext = lx_null;
        write_descriptor_set.dstSet = descriptor_set;
        write_descriptor_set.dstBinding = 0;
        write_descriptor_set.dstArrayElement = 0;
        write_descriptor_set.descriptorCount = 1;
        write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_descriptor_set.pImageInfo = lx_null,
        write_descriptor_set.pBufferInfo = &bufferinfo;
        write_descriptor_set.pTexelBufferView = lx_null;
        vkUpdateDescriptorSets(device->device, 1, &write_descriptor_set, 0, lx_null);
    }
    return descriptor_set;
}

static lx_void_t lx_vk_buffer_free_uniform_descriptor_set(lx_vulkan_device_t* device, VkDescriptorSet descriptor_set) {
    lx_assert(device && device->descriptor_sets_uniform);
    if (descriptor_set != VK_NULL_HANDLE) {
        lx_vk_descriptor_sets_free(device->descriptor_sets_uniform, descriptor_set);
    }
}

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
    if (vmaCreateBuffer(allocator->allocator, &buffer_info, &vma_alloc_createinfo, &vma_buffer->buffer, &vma_buffer->allocation, lx_null) != VK_SUCCESS) {
        return lx_false;
    }

    // if this is a uniform buffer we must setup a descriptor set
    if (allocator->buffer_type == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        vma_buffer->descriptor_set_uniform = lx_vk_buffer_alloc_uniform_descriptor_set(allocator->device, vma_buffer->buffer, size);
        if (vma_buffer->descriptor_set_uniform == VK_NULL_HANDLE) {
            vmaDestroyBuffer(allocator->allocator, vma_buffer->buffer, vma_buffer->allocation);
            return lx_false;
        }
    }
    return lx_true;
}

lx_void_t lx_vk_buffer_allocator_free(lx_vk_buffer_allocator_ref_t self, lx_vk_buffer_t* buffer) {
    lx_vk_buffer_allocator_t* allocator = (lx_vk_buffer_allocator_t*)self;
    lx_vk_vma_buffer_t* vma_buffer = (lx_vk_vma_buffer_t*)buffer;
    lx_assert(allocator && vma_buffer);

    vmaDestroyBuffer(allocator->allocator, vma_buffer->buffer, vma_buffer->allocation);

    if (allocator->buffer_type == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        lx_vk_buffer_free_uniform_descriptor_set(allocator->device, vma_buffer->descriptor_set_uniform);
    }
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
