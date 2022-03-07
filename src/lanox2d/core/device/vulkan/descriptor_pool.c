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
}lx_vk_descriptor_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_vk_descriptor_pool_ref_t lx_vk_descriptor_pool_init(lx_vulkan_device_t* device) {
    return lx_null;
}

lx_void_t lx_vk_descriptor_pool_exit(lx_vk_descriptor_pool_ref_t self) {
}

VkDescriptorPool lx_vk_descriptor_pool(lx_vk_descriptor_pool_ref_t self) {
    return lx_null;
}

