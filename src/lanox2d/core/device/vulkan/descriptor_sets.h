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
 * @file        descriptor_sets.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_DESCRIPTOR_SETS_H
#define LX_CORE_DEVICE_VULKAN_DESCRIPTOR_SETS_H

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

/* init uniform descriptor sets
 *
 * @param device            the vulkan device
 *
 * @return                  the descriptor sets
 */
lx_vk_descriptor_sets_ref_t lx_vk_descriptor_sets_init_uniform(lx_vulkan_device_t* device);

/* exit descriptor sets
 *
 * @param descriptor_sets   the descriptor sets
 */
lx_void_t                   lx_vk_descriptor_sets_exit(lx_vk_descriptor_sets_ref_t descriptor_sets);

/* get the descriptor set layout
 *
 * @param descriptor_sets   the descriptor sets
 *
 * @return                  the descriptor set layout
 */
VkDescriptorSetLayout       lx_vk_descriptor_sets_layout(lx_vk_descriptor_sets_ref_t descriptor_sets);

/* allocate a new descriptor set
 *
 * @param descriptor_sets   the descriptor sets
 *
 * @return                  the descriptor set
 */
VkDescriptorSet             lx_vk_descriptor_sets_alloc(lx_vk_descriptor_sets_ref_t descriptor_sets);

/* free the given descriptor set
 *
 * @param descriptor_sets   the descriptor sets
 * @param descriptor_set    the descriptor set
 */
lx_void_t                   lx_vk_descriptor_sets_free(lx_vk_descriptor_sets_ref_t descriptor_sets, VkDescriptorSet descriptor_set);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


