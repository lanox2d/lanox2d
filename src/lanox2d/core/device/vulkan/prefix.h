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
 * @file        prefix.h
 *
 */
#ifndef LX_CORE_DEVICE_VULKAN_PREFIX_H
#define LX_CORE_DEVICE_VULKAN_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../path.h"
#include "../../paint.h"
#include "../../private/stroker.h"
#include "../../../platform/window.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the pipeline type enum
typedef enum lx_vk_pipeline_type_e_ {
    LX_VK_PIPELINE_TYPE_NONE    = 0
,   LX_VK_PIPELINE_TYPE_SOLID   = 1
,   LX_VK_PIPELINE_TYPE_TEXTURE = 2
,   LX_VK_PIPELINE_TYPE_MAXN    = 3
}lx_vk_pipeline_type_e;

// the pipeline ref type
typedef lx_typeref(vk_pipeline);

// the allocator ref type
typedef lx_typeref(vk_allocator);

// the matrix type
typedef lx_float_t         lx_vk_matrix_t[16];
typedef lx_vk_matrix_t*    lx_vk_matrix_ref_t;


#endif


