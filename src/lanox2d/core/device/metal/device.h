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
 * @file        device.h
 *
 */
#ifndef LX_CORE_DEVICE_METAL_DEVICE_H
#define LX_CORE_DEVICE_METAL_DEVICE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../tess/tess.h"
#import "MetalRenderer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the metal device type
typedef struct lx_metal_device_t_ {
    lx_device_t             base;
    lx_window_ref_t         window;
    MetalRenderer*          renderer;
}lx_metal_device_t;

#endif


