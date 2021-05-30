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
 * @file        canvas.h
 *
 */
#ifndef LX_CORE_CANVAS_H
#define LX_CORE_CANVAS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas_matrix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init canvas from the given device
 *
 * @param device    the device
 *
 * @return          the canvas
 */
lx_canvas_ref_t     lx_canvas_init(lx_device_ref_t device);

/*! exit canvas
 *
 * @param canvas    the canvas
 */
lx_void_t           lx_canvas_exit(lx_canvas_ref_t canvas);

/*! clear draw and fill the given color
 *
 * @param canvas    the canvas
 * @param color     the color
 */
lx_void_t           lx_canvas_draw_clear(lx_canvas_ref_t canvas, lx_color_t color);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


