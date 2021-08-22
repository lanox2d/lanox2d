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
#ifndef LX_CORE_DEVICE_H
#define LX_CORE_DEVICE_H

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

/*! init device from bitmap
 *
 * @param bitmap        the bitmap
 *
 * @return              the device
 */
lx_device_ref_t         lx_device_init_from_bitmap(lx_bitmap_ref_t bitmap);

/*! init device from opengl
 *
 * @param width         the window width
 * @param height        the window height
 * @param framewidth    the frame width
 * @param frameheight   the frame height
 *
 * @return              the device
 */
lx_device_ref_t         lx_device_init_from_opengl(lx_size_t width, lx_size_t height, lx_size_t framewidth, lx_size_t frameheight);

/*! init device from metal
 *
 * @param width         the window width
 * @param height        the window height
 * @param devdata       the device data
 *
 * @return              the device
 */
lx_device_ref_t         lx_device_init_from_metal(lx_size_t width, lx_size_t height, lx_cpointer_t devdata);

/*! init device from skia
 *
 * @param width         the frame width
 * @param height        the frame height
 * @param bitmap        the bitmap, we will use opengl renderer if it is null
 *
 * @return              the device
 */
lx_device_ref_t         lx_device_init_from_skia(lx_size_t width, lx_size_t height, lx_bitmap_ref_t bitmap);

/*! exit device
 *
 * @param device        the device
 */
lx_void_t               lx_device_exit(lx_device_ref_t device);

/*! get the device width
 *
 * @param device        the device
 *
 * @return              the width
 */
lx_size_t               lx_device_width(lx_device_ref_t device);

/*! get the device height
 *
 * @param device        the device
 *
 * @return              the height
 */
lx_size_t               lx_device_height(lx_device_ref_t device);

/*! bind path
 *
 * @param device        the device
 * @param path          the path
 */
lx_void_t               lx_device_bind_path(lx_device_ref_t device, lx_path_ref_t path);

/*! bind paint
 *
 * @param device        the device
 * @param paint         the paint
 */
lx_void_t               lx_device_bind_paint(lx_device_ref_t device, lx_paint_ref_t paint);

/*! bind matrix
 *
 * @param device        the device
 * @param matrix        the matrix
 */
lx_void_t               lx_device_bind_matrix(lx_device_ref_t device, lx_matrix_ref_t matrix);

/*! bind clipper
 *
 * @param device        the device
 * @param clipper       the clipper
 */
lx_void_t               lx_device_bind_clipper(lx_device_ref_t device, lx_clipper_ref_t clipper);

/*! clear draw and fill the given color
 *
 * @param device        the device
 * @param color         the color
 */
lx_void_t               lx_device_draw_clear(lx_device_ref_t device, lx_color_t color);

/*! draw path
 *
 * @param device        the device
 * @param path          the path
 */
lx_void_t               lx_device_draw_path(lx_device_ref_t device, lx_path_ref_t path);

/*! draw lines
 *
 * @param device        the device
 * @param points        the points
 * @param count         the points count
 * @param bounds        the bounds
 */
lx_void_t               lx_device_draw_lines(lx_device_ref_t device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds);

/*! draw points
 *
 * @param device        the device
 * @param points        the points
 * @param count         the points count
 * @param bounds        the bounds
 */
lx_void_t               lx_device_draw_points(lx_device_ref_t device, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds);

/*! draw polygon
 *
 * @param device        the device
 * @param polygon       the polygon
 * @param hint          the hint shape
 * @param bounds        the bounds
 */
lx_void_t               lx_device_draw_polygon(lx_device_ref_t device, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds);


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


