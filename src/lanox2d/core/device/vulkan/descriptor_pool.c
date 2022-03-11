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
 * @file        descriptor_pool.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "descriptor_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the descriptor_pool type
typedef struct lx_vk_descriptor_pool_t {
    lx_vulkan_device_t*     device;
    VkDescriptorType        type;
    lx_uint32_t             count;
    VkDescriptorPool        pool;
}lx_vk_descriptor_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_descriptor_pool_ref_t lx_vk_descriptor_pool_init(lx_vulkan_device_t* device, VkDescriptorType type, lx_uint32_t count) {
    lx_assert_and_check_return_val(device && type && count, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_descriptor_pool_t* descriptor_pool = lx_null;
    do {
        descriptor_pool = lx_malloc0_type(lx_vk_descriptor_pool_t);
        lx_assert_and_check_break(descriptor_pool);

        descriptor_pool->type  = type;
        descriptor_pool->count = count;
        descriptor_pool->device = device;

        VkDescriptorPoolSize pool_size = {};
        pool_size.type = type;
        pool_size.descriptorCount = count;

        VkDescriptorPoolCreateInfo descriptor_poolinfo = {};
        descriptor_poolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_poolinfo.pNext = lx_null;
        descriptor_poolinfo.maxSets = count; // just an estimate value
        descriptor_poolinfo.poolSizeCount = 1;
        descriptor_poolinfo.pPoolSizes = &pool_size;
        if (vkCreateDescriptorPool(device->device, &descriptor_poolinfo, lx_null, &descriptor_pool->pool) != VK_SUCCESS) {
            break;
        }

        ok = lx_true;
    } while (0);

    if (!ok && descriptor_pool) {
        lx_vk_descriptor_pool_exit((lx_vk_descriptor_pool_ref_t)descriptor_pool);
        descriptor_pool = lx_null;
    }
    return (lx_vk_descriptor_pool_ref_t)descriptor_pool;
}

lx_void_t lx_vk_descriptor_pool_exit(lx_vk_descriptor_pool_ref_t self) {
    lx_vk_descriptor_pool_t* descriptor_pool = (lx_vk_descriptor_pool_t*)self;
    if (descriptor_pool) {
        lx_assert(descriptor_pool->device && descriptor_pool->device->device);
        if (descriptor_pool->pool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(descriptor_pool->device->device, descriptor_pool->pool, lx_null);
            descriptor_pool->pool = VK_NULL_HANDLE;
        }
        lx_free(descriptor_pool);
    }
}

VkDescriptorPool lx_vk_descriptor_pool(lx_vk_descriptor_pool_ref_t self) {
    lx_vk_descriptor_pool_t* descriptor_pool = (lx_vk_descriptor_pool_t*)self;
    return descriptor_pool? descriptor_pool->pool : VK_NULL_HANDLE;
}

