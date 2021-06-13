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
 * @file        polygon.h
 *
 */
#ifndef LX_CORE_DEVICE_BITMAP_RENDERER_POLYGON_H
#define LX_CORE_DEVICE_BITMAP_RENDERER_POLYGON_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* fill polygon
 *
 * @param device    the device
 * @param polygon   the polygon
 * @param bounds    the bounds
 */
lx_void_t           lx_bitmap_renderer_fill_polygon(lx_bitmap_device_t* device, lx_polygon_ref_t polygon, lx_rect_ref_t bounds);

/* stroke polygon
 *
 * @param device    the device
 * @param polygon   the polygon
 */
lx_void_t           lx_bitmap_renderer_stroke_polygon(lx_bitmap_device_t* device, lx_polygon_ref_t polygon);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif