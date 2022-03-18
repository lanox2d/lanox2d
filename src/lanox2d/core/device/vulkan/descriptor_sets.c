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

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the descriptor_sets type
typedef struct lx_vk_descriptor_sets_t {
    lx_vulkan_device_t*     device;
    VkDescriptorType        type;
    lx_uint32_t             count;
}lx_vk_descriptor_sets_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_descriptor_sets_ref_t lx_vk_descriptor_sets_init(lx_vulkan_device_t* device, VkDescriptorType type, lx_uint32_t count) {
    lx_assert_and_check_return_val(device && type && count, lx_null);

    lx_bool_t ok = lx_false;
    lx_vk_descriptor_sets_t* descriptor_sets = lx_null;
    do {
        descriptor_sets = lx_malloc0_type(lx_vk_descriptor_sets_t);
        lx_assert_and_check_break(descriptor_sets);


        ok = lx_true;
    } while (0);

    if (!ok && descriptor_sets) {
        lx_vk_descriptor_sets_exit((lx_vk_descriptor_sets_ref_t)descriptor_sets);
        descriptor_sets = lx_null;
    }
    return (lx_vk_descriptor_sets_ref_t)descriptor_sets;
}

lx_void_t lx_vk_descriptor_sets_exit(lx_vk_descriptor_sets_ref_t self) {
    lx_vk_descriptor_sets_t* descriptor_sets = (lx_vk_descriptor_sets_t*)self;
    if (descriptor_sets) {
        lx_free(descriptor_sets);
    }
}

