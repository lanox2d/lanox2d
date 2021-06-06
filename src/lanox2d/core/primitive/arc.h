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
 * @file        arc.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_ARC_H
#define LX_CORE_PRIMITIVE_ARC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/* the arc quad callback type
 *
 * @param ctrl      the ctrl point, the point is first if be null
 * @param point     the point
 * @param udata     the user data
 */
typedef lx_void_t   (*lx_arc_quad_cb_t)(lx_point_ref_t ctrl, lx_point_ref_t point, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! make arc
 *
 * @param arc       the arc
 * @param x0        the x0
 * @param y0        the y0
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_arc_make(lx_arc_ref_t arc, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an);

/*! make arc with the integer value
 *
 * @param arc       the arc
 * @param x0        the x0
 * @param y0        the y0
 * @param rx        the x-radius
 * @param ry        the y-radius
 * @param ab        the start angle
 * @param an        the sweep angle, 0 - 360
 */
lx_void_t           lx_arc_imake(lx_arc_ref_t arc, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an);

/* make the quadratic curves for the arc
 *
 * @param arc       the arc
 * @param callback  the make callback
 * @param udata     the user data
 */
lx_void_t           lx_arc_make_quad(lx_arc_ref_t arc, lx_arc_quad_cb_t callback, lx_cpointer_t udata);

/* make the quadratic curves for the arc
 *
 * @param start     the start unit vector
 * @param stop      the stop unit vector
 * @param matrix    the user matrix
 * @param callback  the make callback
 * @param udata     the user data
 */
lx_void_t           lx_arc_make_quad2(lx_vector_ref_t start, lx_vector_ref_t stop, lx_matrix_ref_t matrix, lx_size_t direction, lx_arc_quad_cb_t callback, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


