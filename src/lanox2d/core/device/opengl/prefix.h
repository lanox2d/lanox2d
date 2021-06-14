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
#ifndef LX_CORE_DEVICE_OPENGL_PREFIX_H
#define LX_CORE_DEVICE_OPENGL_PREFIX_H

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

// the opengl device type
typedef struct lx_opengl_device_t_ {
    lx_device_t             base;
    lx_window_ref_t         window;
    lx_stroker_ref_t        stroker;
    lx_size_t               glversion;
}lx_opengl_device_t;

#endif


