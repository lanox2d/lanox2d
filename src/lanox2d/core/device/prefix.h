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
    lx_void_t           (*draw_clear)(lx_device_ref_t device, lx_color_t color);
    lx_void_t           (*draw_path)(lx_device_ref_t device, lx_path_ref_t path);
    lx_void_t           (*draw_lines)(lx_device_ref_t device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds);
    lx_void_t           (*draw_points)(lx_device_ref_t device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds);
    lx_void_t           (*draw_polygon)(lx_device_ref_t device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds);
    lx_texture_ref_t    (*create_linear_texture)(lx_device_ref_t device, lx_size_t mode, lx_gradient_ref_t gradient, lx_line_ref_t line);
    lx_texture_ref_t    (*create_radial_texture)(lx_device_ref_t device, lx_size_t mode, lx_gradient_ref_t gradient, lx_circle_ref_t circle);
    lx_texture_ref_t    (*create_bitmap_texture)(lx_device_ref_t device, lx_size_t mode, lx_bitmap_ref_t bitmap);
    lx_void_t           (*exit)(lx_device_ref_t device);
}lx_device_t;

// the texture type
typedef struct lx_texture_t_ {
    lx_uint8_t          type;
    lx_uint8_t          mode;
    lx_matrix_t         matrix;
    lx_void_t           (*exit)(lx_texture_ref_t texture);
}lx_texture_t;

#endif


