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
 * @file        descriptor_pool.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_DESCRIPTOR_POOL_H
#define LX_CORE_DEVICE_VULKAN_DESCRIPTOR_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init descriptor pool
 *
 * @param device            the vulkan device
 * @param type              the descriptor type
 * @param count             the descriptor count
 *
 * @return                  the descriptor pool
 */
lx_vk_descriptor_pool_ref_t lx_vk_descriptor_pool_init(lx_vulkan_device_t* device, VkDescriptorType type, lx_uint32_t count);

/* exit descriptor pool
 *
 * @param descriptor_pool   the descriptor pool
 */
lx_void_t                   lx_vk_descriptor_pool_exit(lx_vk_descriptor_pool_ref_t descriptor_pool);

/* get the vulkan descriptor pool
 *
 * @param descriptor_pool   the descriptor pool
 *
 * @return                  the native descriptor pool
 */
VkDescriptorPool            lx_vk_descriptor_pool(lx_vk_descriptor_pool_ref_t descriptor_pool);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


