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
 * @file        bezier2.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_BEZIER2_H
#define LX_CORE_PRIMITIVE_BEZIER2_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the max quadratic curve divided count
#define LX_BEZIER2_DIVIDED_MAXN          (5)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/* the quad line callback type
 *
 * @param point     the point
 * @param udata     the user private data
 */
typedef lx_void_t   (*lx_bezier2_line_cb_t)(lx_point_ref_t point, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* compute the approximate distance
 *
 *                  p1
 *                  .
 *                .  .
 *              .  .  .
 *            .   .    .
 *          .    .      .
 *        .     .        .
 *      .      .distance  .
 *    .       .            .
 * p0         x           p2
 *
 * distance(p1, center(p0, p2))
 *
 * @param points    the points
 *
 * @return          the approximate distance
 */
lx_float_t          lx_bezier2_near_distance(lx_point_t const points[3]);

/* compute the approximate divided count for approaching the line-to
 *
 * @param points    the points
 *
 * @return          the approximate divided count
 */
lx_size_t           lx_bezier2_divide_line_count(lx_point_t const points[3]);

/* chop the quad curve at the given position
 *
 *               chop
 *                |
 *              . .
 *  factor .      p1.
 *      .            . (1 - factor)
 *   .                .
 * .                   .
 * p0                   p2
 *
 * quad(p0, p1, p2) => quad(o0, o1, o2) + quad(o2, o3, o4)
 *
 * @param points    the points
 * @param output    the output points
 * @param factor    the length factor of the chopped curve, must be in range: (0, 1)
 */
lx_void_t           lx_bezier2_chop_at(lx_point_t const points[3], lx_point_t output[5], lx_float_t factor);

/* chop the quad curve at the half position
 *
 *           chop
 *            |
 *          . . .
 *      .     p1   .
 *   .               .
 * .                   .
 * p0                   p2
 *
 * quad(p0, p1, p2) => quad(o0, o1, o2) + quad(o2, o3, o4)
 *
 * @param points    the points
 * @param output    the output points
 */
lx_void_t           lx_bezier2_chop_at_half(lx_point_t const points[3], lx_point_t output[5]);

/* chop the quad curve at the max curvature
 *
 *               chop
 *                |
 *              . . -> max curvature
 *         .      p1.
 *      .            .
 *   .                .
 * .                   .
 * p0                   p2
 *
 * quad(p0, p1, p2) => quad(o0, o1, o2) + quad(o2, o3, o4)
 *
 * @param points    the points
 * @param output    the output points, optional
 *
 * return           the chopped curve count, 1 or 2
 */
lx_size_t           lx_bezier2_chop_at_max_curvature(lx_point_t const points[3], lx_point_t output[5]);

/* make line-to points for the quadratic curve
 *
 * @param points    the points
 * @param callback  the make callback
 * @param udata     the make callback private data for user
 */
lx_void_t           lx_bezier2_make_line(lx_point_t const points[3], lx_bezier2_line_cb_t callback, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


