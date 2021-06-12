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
 * @file        renderer.h
 *
 */
#ifndef LX_CORE_DEVICE_BITMAP_RENDERER_H
#define LX_CORE_DEVICE_BITMAP_RENDERER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface
 */

/* init render
 *
 * @param device    the device
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_bitmap_renderer_init(lx_bitmap_device_t* device);

/* exit render
 *
 * @param device    the device
 */
lx_void_t           lx_bitmap_renderer_exit(lx_bitmap_device_t* device);

/* draw path
 *
 * @param device    the device
 * @param path      the path
 */
lx_void_t           lx_bitmap_renderer_draw_path(lx_bitmap_device_t* device, lx_path_ref_t path);

/* draw lines
 *
 * @param device    the device
 * @param points    the points
 * @param count     the points count
 * @param bounds    the bounds
 */
lx_void_t           lx_bitmap_renderer_draw_lines(lx_bitmap_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds);

/* draw points
 *
 * @param device    the device
 * @param points    the points
 * @param count     the points count
 * @param bounds    the bounds
 */
lx_void_t           lx_bitmap_renderer_draw_points(lx_bitmap_device_t* device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds);

/* draw polygon
 *
 * @param device    the device
 * @param polygon   the polygon
 * @param hint      the hint shape
 * @param bounds    the bounds
 */
lx_void_t           lx_bitmap_renderer_draw_polygon(lx_bitmap_device_t* device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


