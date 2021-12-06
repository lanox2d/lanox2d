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
 * @file        allocator.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_ALLOCATOR_H
#define LX_CORE_DEVICE_VULKAN_ALLOCATOR_H

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

/*! init the vulkan buffer allocator
 *
 * @param device        the vulkan device
 *
 * @return              the vulkan allocator
 */
lx_vk_allocator_ref_t   lx_vk_allocator_init(lx_vulkan_device_t* device);

/*! exit the vulkan buffer allocator
 *
 * @param               the vulkan allocator
 */
lx_void_t               lx_vk_allocator_exit(lx_vk_allocator_ref_t allocator);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


