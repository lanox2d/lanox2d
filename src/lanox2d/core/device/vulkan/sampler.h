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
 * @file        sampler.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_SAMPLER_H
#define LX_CORE_DEVICE_VULKAN_SAMPLER_H

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

/* init sampler
 *
 * @param device        the vulkan device
 * @param filter        the sampler filter, e.g. VK_FILTER_NEAREST, VK_FILTER_LINEAR
 * @param addr_mode_x   the sampler address mode (x), e.g. VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE ..
 * @param addr_mode_y   the sampler address mode (y), e.g. VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE ..
 *
 * @return              the sampler
 */
lx_vk_sampler_ref_t     lx_vk_sampler_init(lx_vulkan_device_t* device, VkFilter filter, VkSamplerAddressMode addr_mode_x, VkSamplerAddressMode addr_mode_y);

/* exit sampler
 *
 * @param sampler       the sampler
 */
lx_void_t               lx_vk_sampler_exit(lx_vk_sampler_ref_t sampler);

/* get the vulkan sampler
 *
 * @param sampler       the sampler
 *
 * @return              the native sampler
 */
VkSampler               lx_vk_sampler(lx_vk_sampler_ref_t sampler);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


