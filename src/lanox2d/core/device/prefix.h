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
#ifndef LX_CORE_DEVICE_PREFIX_H
#define LX_CORE_DEVICE_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../pixmap.h"
#include "../bitmap.h"
#include "../device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the device type
typedef struct lx_device_t_ {
    lx_uint16_t         width;
    lx_uint16_t         height;
    lx_path_ref_t       path;
    lx_paint_ref_t      paint;
    lx_matrix_ref_t     matrix;
    lx_clipper_ref_t    clipper;
    lx_void_t           (*resize)(lx_device_ref_t device, lx_size_t width, lx_size_t height);
    lx_void_t           (*draw_clear)(lx_device_ref_t device, lx_color_t color);
    lx_void_t           (*exit)(lx_device_ref_t device);
}lx_device_t;

#endif


