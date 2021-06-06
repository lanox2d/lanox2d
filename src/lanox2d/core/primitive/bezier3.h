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
 * @file        bezier3.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_BEZIER3_H
#define LX_CORE_PRIMITIVE_BEZIER3_H

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

// the max cubic curve divided count
#define LX_BEZIER3_DIVIDED_MAXN          (6)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/* the cubic line callback type
 *
 * @param point     the point
 * @param udata     the user data
 */
typedef lx_void_t   (*lx_bezier3_line_cb_t)(lx_point_ref_t point, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* compute the approximate distance
 *
 *      p1                 p2
 *      . . . . . . . . . . .
 *     . .                 . .
 *    .   .               .   .
 *   .      .           .      .
 *  .     d1  .       .  d2     .
 * .            .   .            .
 * p0             .              p3
 *
 * distance = max(d1 + d2)
 *
 * @param points    the points
 *
 * @return          the approximate distance
 */
lx_float_t          lx_bezier3_near_distance(lx_point_t const points[4]);

/* compute the approximate divided count for approaching the line-to
 *
 * @param points    the points
 *
 * @return          the approximate divided count
 */
lx_size_t           lx_bezier3_divide_line_count(lx_point_t const points[4]);

/* chop the cubic curve at the given position
 *
 *               chop
 *                |
 *              . .
 *  factor .        .
 *      . p1      p2 . (1 - factor)
 *   .                .
 * .                   .
 * p0                   p3
 *
 * cubic(p0, p1, p2, p3) => cubic(o0, o1, o2, o3) + cubic(o3, o4, o5, o6)
 *
 * @param points    the points
 * @param output    the output points
 * @param factor    the length factor of the chopped curve, must be in range: (0, 1)
 */
lx_void_t           lx_bezier3_chop_at(lx_point_t const points[4], lx_point_t output[7], lx_float_t factor);

/* chop the cubic curve at the given positions
 *
 * @param points    the points
 * @param output    the output points
 * @param maxn      the max count of the output points
 * @param factors   the factors
 * @param count     the factor count
 */
lx_void_t           lx_bezier3_chop_at2(lx_point_t const points[4], lx_point_ref_t output, lx_size_t maxn, lx_float_t const* factors, lx_size_t count);

/* chop the cubic curve at the half position
 *
 *           chop
 *            |
 *          . . .
 *     p1.         .p2
 *   .               .
 * .                   .
 * p0                   p3
 *
 * cubic(p0, p1, p2, p3) => cubic(o0, o1, o2, o3) + cubic(o3, o4, o5, o6)
 *
 * @param points    the points
 * @param output    the output points
 */
lx_void_t           lx_bezier3_chop_at_half(lx_point_t const points[4], lx_point_t output[7]);

/* chop the cubic curve at the max curvature
 *
 *               chop
 *                |
 *              . . -> max curvature
 *       p1.      p2.
 *      .            .
 *   .                .
 * .                   .
 * p0                   p3
 *
 * cubic(p0, p1, p2, p3) => cubic(o0, o1, o2, o3) + cubic(o3, o4, o5, o6)
 *
 * @param points    the points
 * @param output    the output points, optional
 *
 * return           the chopped curve count, 1 or 2 or 3
 */
lx_size_t           lx_bezier3_chop_at_max_curvature(lx_point_t const points[4], lx_point_t output[13]);

/* make line-to points for the cubic curve
 *
 * @param points    the points
 * @param callback  the make callback
 * @param udata     the user data
 */
lx_void_t           lx_bezier3_make_line(lx_point_t const points[4], lx_bezier3_line_cb_t callback, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


