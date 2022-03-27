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
 * @file        descriptor_sets.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "descriptor_sets.h"
#include "descriptor_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef LX_CONFIG_SMALL
#   define LX_VK_DESCRIPTOR_FREE_SETS_GROW          (16)
#   define LX_VK_DESCRIPTOR_DESCRIPTOR_POOLS_GROW   (8)
#   define LX_VK_DESCRIPTOR_INIT                    (16)
#   define LX_VK_DESCRIPTOR_MAXN                    (1024)
#else
#   define LX_VK_DESCRIPTOR_FREE_SETS_GROW          (64)
#   define LX_VK_DESCRIPTOR_DESCRIPTOR_POOLS_GROW   (32)
#   define LX_VK_DESCRIPTOR_INIT                    (32)
#   define LX_VK_DESCRIPTOR_MAXN                    (2048)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the descriptor_sets type
typedef struct lx_vk_descriptor_sets_t {
    lx_vulkan_device_t*         device;
    VkDescriptorType            descriptor_type;
    VkDescriptorSetLayout       descriptor_set_layout;
    lx_uint32_t                 descriptor_count_per_set;
    lx_uint32_t                 descriptor_count_current;
    lx_uint32_t                 descriptor_maxn;
    lx_array_ref_t              free_sets;
    lx_array_ref_t              descriptor_pools;
}lx_vk_descriptor_sets_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

static lx_vk_descriptor_pool_ref_t lx_vk_descriptor_sets_get_new_descriptor_pool(lx_vk_descriptor_sets_t* descriptor_sets) {
    lx_vulkan_device_t* device = descriptor_sets->device;
    if (lx_array_size(descriptor_sets->descriptor_pools)) {
        lx_uint32_t pool_size = descriptor_sets->descriptor_maxn + ((descriptor_sets->descriptor_maxn + 1) >> 1);
        if (pool_size < LX_VK_DESCRIPTOR_MAXN) {
            descriptor_sets->descriptor_maxn = pool_size;
        } else {
            descriptor_sets->descriptor_maxn = LX_VK_DESCRIPTOR_MAXN;
        }

    }
    lx_vk_descriptor_pool_ref_t descriptor_pool = lx_vk_descriptor_pool_init(device, descriptor_sets->descriptor_type, descriptor_sets->descriptor_maxn);
    if (descriptor_pool) {
        lx_array_insert_tail(descriptor_sets->descriptor_pools, &descriptor_pool);
    }
    return descriptor_pool;
}

static VkDescriptorPool lx_vk_descriptor_sets_get_descriptor_pool(lx_vk_descriptor_sets_t* descriptor_sets) {
    if (lx_array_size(descriptor_sets->descriptor_pools)) {
        lx_vk_descriptor_pool_ref_t* pdescriptor_pool = (lx_vk_descriptor_pool_ref_t*)lx_array_last(descriptor_sets->descriptor_pools);
        if (pdescriptor_pool) {
            lx_vk_descriptor_pool_ref_t descriptor_pool = *pdescriptor_pool;
            if (descriptor_pool) {
                return lx_vk_descriptor_pool(descriptor_pool);
            }
        }
    }
    return VK_NULL_HANDLE;
}

static lx_void_t lx_vk_descriptor_sets_free_descriptor_pool(lx_pointer_t item, lx_pointer_t udata) {
    lx_vk_descriptor_pool_ref_t* pdescriptor_pool = (lx_vk_descriptor_pool_ref_t*)item;
    if (pdescriptor_pool && *pdescriptor_pool) {
        lx_vk_descriptor_pool_exit(*pdescriptor_pool);
    }
}

static VkDescriptorSet lx_vk_descriptor_sets_get_new_descriptor_set(lx_vk_descriptor_sets_t* descriptor_sets) {
    lx_vulkan_device_t* device = descriptor_sets->device;
    lx_assert(device && descriptor_sets->descriptor_maxn);

    // get the descriptor pool
    descriptor_sets->descriptor_count_current += descriptor_sets->descriptor_count_per_set;
    if (!lx_array_size(descriptor_sets->descriptor_pools) || descriptor_sets->descriptor_count_current > descriptor_sets->descriptor_maxn) {
        if (!lx_vk_descriptor_sets_get_new_descriptor_pool(descriptor_sets)) {
            return VK_NULL_HANDLE;
        }
        descriptor_sets->descriptor_count_current = descriptor_sets->descriptor_count_per_set;
    }
    VkDescriptorPool descriptor_pool = lx_vk_descriptor_sets_get_descriptor_pool(descriptor_sets);
    lx_assert(descriptor_pool != VK_NULL_HANDLE);

    // create a descriptor set
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    VkDescriptorSetAllocateInfo allocinfo = {};
    allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocinfo.pNext = lx_null;
    allocinfo.descriptorPool = descriptor_pool;
    allocinfo.descriptorSetCount = 1;
    allocinfo.pSetLayouts = &descriptor_sets->descriptor_set_layout;
    return vkAllocateDescriptorSets(device->device, &allocinfo, &descriptor_set) == VK_SUCCESS? descriptor_set : VK_NULL_HANDLE;
}

static lx_uint32_t lx_vk_descriptor_sets_get_layout_and_set_count_for_uniform(lx_vulkan_device_t* device,
    lx_uint32_t const* stages, lx_size_t stages_size, VkDescriptorSetLayout* pdescriptor_set_layout) {
    lx_assert_and_check_return_val(stages_size == 1, 0);

    // init descriptor set layout (uniform buffer)
    const lx_uint32_t descriptor_count = 1;
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = LX_VK_UNIFORM_BINDING;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//_DYNAMIC;
    layout_binding.descriptorCount = descriptor_count;
    layout_binding.stageFlags = stages[0];
    layout_binding.pImmutableSamplers = lx_null;

    VkDescriptorSetLayoutCreateInfo layout_createinfo = {};
    layout_createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_createinfo.pNext = lx_null;
    layout_createinfo.bindingCount = 1;
    layout_createinfo.pBindings = &layout_binding;
    return vkCreateDescriptorSetLayout(device->device, &layout_createinfo, lx_null, pdescriptor_set_layout) == VK_SUCCESS? descriptor_count : 0;
}

static lx_uint32_t lx_vk_descriptor_sets_get_layout_and_set_count_for_sampler(lx_vulkan_device_t* device,
    lx_uint32_t const* stages, lx_size_t stages_size, VkDescriptorSetLayout* pdescriptor_set_layout) {
    lx_assert_and_check_return_val(stages_size == 1, 0);

    // TODO
    return 0;
}

static lx_uint32_t lx_vk_descriptor_sets_get_layout_and_set_count(lx_vulkan_device_t* device, VkDescriptorType type,
    lx_uint32_t const* stages, lx_size_t stages_size, VkDescriptorSetLayout* pdescriptor_set_layout) {
    lx_uint32_t descriptor_count = 0;
    switch (type) {
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        descriptor_count = lx_vk_descriptor_sets_get_layout_and_set_count_for_uniform(device,
            stages, stages_size, pdescriptor_set_layout);
        break;
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        descriptor_count = lx_vk_descriptor_sets_get_layout_and_set_count_for_sampler(device,
            stages, stages_size, pdescriptor_set_layout);
        break;
    default:
        break;
    }
    return descriptor_count;
}

static lx_vk_descriptor_sets_ref_t lx_vk_descriptor_sets_init(lx_vulkan_device_t* device,
    VkDescriptorType type, lx_uint32_t const* stages, lx_size_t stages_size) {
    lx_assert_and_check_return_val(device && stages && stages_size, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_descriptor_sets_t* descriptor_sets = lx_null;
    do {
        descriptor_sets = lx_malloc0_type(lx_vk_descriptor_sets_t);
        lx_assert_and_check_break(descriptor_sets);

        descriptor_sets->device = device;
        descriptor_sets->descriptor_type = type;
        descriptor_sets->descriptor_maxn = LX_VK_DESCRIPTOR_INIT;

        descriptor_sets->descriptor_count_per_set = lx_vk_descriptor_sets_get_layout_and_set_count(device,
            type, stages, stages_size, &descriptor_sets->descriptor_set_layout);
        lx_assert_and_check_break(descriptor_sets->descriptor_count_per_set);

        descriptor_sets->free_sets = lx_array_init(LX_VK_DESCRIPTOR_FREE_SETS_GROW, lx_element_mem(sizeof(VkDescriptorSet), lx_null, lx_null));
        lx_assert_and_check_break(descriptor_sets->free_sets);

        descriptor_sets->descriptor_pools = lx_array_init(LX_VK_DESCRIPTOR_DESCRIPTOR_POOLS_GROW,
            lx_element_mem(sizeof(lx_vk_descriptor_pool_ref_t), lx_vk_descriptor_sets_free_descriptor_pool, lx_null));
        lx_assert_and_check_break(descriptor_sets->descriptor_pools);

        ok = lx_true;
    } while (0);

    if (!ok && descriptor_sets) {
        lx_vk_descriptor_sets_exit((lx_vk_descriptor_sets_ref_t)descriptor_sets);
        descriptor_sets = lx_null;
    }
    return (lx_vk_descriptor_sets_ref_t)descriptor_sets;
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_descriptor_sets_ref_t lx_vk_descriptor_sets_init_uniform(lx_vulkan_device_t* device) {
    const lx_uint32_t stages[] = {VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT};
    return lx_vk_descriptor_sets_init(device, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stages, lx_arrayn(stages));
}

lx_vk_descriptor_sets_ref_t lx_vk_descriptor_sets_init_sampler(lx_vulkan_device_t* device) {
    const lx_uint32_t stages[] = {VK_SHADER_STAGE_FRAGMENT_BIT};
    return lx_vk_descriptor_sets_init(device, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stages, lx_arrayn(stages));
}

lx_void_t lx_vk_descriptor_sets_exit(lx_vk_descriptor_sets_ref_t self) {
    lx_vk_descriptor_sets_t* descriptor_sets = (lx_vk_descriptor_sets_t*)self;
    if (descriptor_sets) {
        lx_vulkan_device_t* device = descriptor_sets->device;
        lx_assert(device);

        if (descriptor_sets->free_sets) {
            lx_array_exit(descriptor_sets->free_sets);
            descriptor_sets->free_sets = lx_null;
        }

        if (descriptor_sets->descriptor_set_layout) {
            vkDestroyDescriptorSetLayout(device->device, descriptor_sets->descriptor_set_layout, lx_null);
            descriptor_sets->descriptor_set_layout = VK_NULL_HANDLE;
        }

        if (descriptor_sets->descriptor_pools) {
            lx_array_exit(descriptor_sets->descriptor_pools);
            descriptor_sets->descriptor_pools = lx_null;
        }
        lx_free(descriptor_sets);
    }
}

VkDescriptorSetLayout lx_vk_descriptor_sets_layout(lx_vk_descriptor_sets_ref_t self) {
    lx_vk_descriptor_sets_t* descriptor_sets = (lx_vk_descriptor_sets_t*)self;
    return descriptor_sets? descriptor_sets->descriptor_set_layout : VK_NULL_HANDLE;
}

VkDescriptorSet lx_vk_descriptor_sets_alloc(lx_vk_descriptor_sets_ref_t self) {
    lx_vk_descriptor_sets_t* descriptor_sets = (lx_vk_descriptor_sets_t*)self;
    lx_assert_and_check_return_val(descriptor_sets && descriptor_sets->free_sets, VK_NULL_HANDLE);

    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    if (lx_array_size(descriptor_sets->free_sets)) {
        VkDescriptorSet* pdescriptor_set = (VkDescriptorSet*)lx_array_last(descriptor_sets->free_sets);
        if (pdescriptor_set) {
            descriptor_set = *pdescriptor_set;
            lx_array_remove_last(descriptor_sets->free_sets);
        }
    } else {
        descriptor_set = lx_vk_descriptor_sets_get_new_descriptor_set(descriptor_sets);
    }
    lx_assert(descriptor_set != VK_NULL_HANDLE);
    return descriptor_set;
}

lx_void_t lx_vk_descriptor_sets_free(lx_vk_descriptor_sets_ref_t self, VkDescriptorSet descriptor_set) {
    lx_vk_descriptor_sets_t* descriptor_sets = (lx_vk_descriptor_sets_t*)self;
    if (descriptor_sets && descriptor_sets->free_sets) {
        lx_assert(descriptor_set != VK_NULL_HANDLE);
        lx_array_insert_tail(descriptor_sets->free_sets, &descriptor_set);
    }
}

