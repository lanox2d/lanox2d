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
 * @file        buffer.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_BUFFER_H
#define LX_CORE_DEVICE_VULKAN_BUFFER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the vulkan buffer type
typedef struct lx_vk_buffer_t_ {
    VkBuffer*       buffer;
    lx_size_t       offset;
    lx_size_t       size;
}lx_vk_buffer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


