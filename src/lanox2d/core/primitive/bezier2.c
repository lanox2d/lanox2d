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
 * @file        bezier2.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bezier2.h"
#include "point.h"
#include "utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_bezier2_make_line_impl(lx_point_t const points[3], lx_size_t count, lx_bezier2_line_cb_t callback, lx_cpointer_t udata) {
    /* divide it
     *
     *
     *                  p1
     *                  .
     *                .  .
     *              .     .
     *            .        .
     *       o1 . . . . . . . o3
     *        .      o2      .
     *      .                 .
     *    .                    .
     * p0, o0                p2, o4
     *
     */
    if (count) {
        // chop the quad at half
        lx_point_t output[5];
        lx_bezier2_chop_at_half(points, output);

        // make line for quad(o0, o1, o2)
        lx_bezier2_make_line_impl(output, count - 1, callback, udata);

        // make line to quad(o2, o3, o4)
        lx_bezier2_make_line_impl(output + 2, count - 1, callback, udata);
    } else {
        callback((lx_point_ref_t)&points[2], udata);
    }
}

static lx_void_t lx_bezier2_chop_xy_at(lx_float_t const* xy, lx_float_t* output, lx_float_t factor) {
    // compute the interpolation of p0 => p1
    lx_float_t xy01 = lx_interp(xy[0], xy[2], factor);

    // compute the interpolation of p1 => p2
    lx_float_t xy12 = lx_interp(xy[2], xy[4], factor);

    // make output
    output[0] = xy[0];
    output[2] = xy01;
    output[4] = lx_interp(xy01, xy12, factor);
    output[6] = xy12;
    output[8] = xy[4];
}

static lx_float_t lx_bezier2_find_max_curvature(lx_point_t const points[3]) {
    /* parameter: f = factor
     *
     * function of the x-coordinate:
     * X(f) = x0 * (1 - f) ^ 2 + 2 * x1 * f * (1 - f) + x2 * f ^ 2
     * => X'(f) = 2 * (x1 - x0) + 2 * (x0 - 2 * x1 + x2) * f
     * => X''(f) = 2 * (x0 - 2 * x1 + x2)
     *
     * Ax = 2 * (x1 - x0)
     * Bx = 2 * (x0 - 2 * x1 + x2)
     *
     * => X'(f) = Ax + Bx * f
     * => X''(f) = Bx
     *
     * function of the y-coordinate:
     * Y(f) = y0 * (1 - f) ^ 2 + 2 * y1 * f * (1 - f) + y2 * f ^ 2
     * => Y'(f) = 2 * (y1 - y0) + 2 * (y0 - 2 * y1 + y2) * f
     * => Y''(f) = 2 * (y0 - 2 * y1 + y2)
     *
     * Ay = 2 * (y1 - y0)
     * By = 2 * (y0 - 2 * y1 + y2)
     *
     * => Y'(f) = Ay + By * f
     * => Y''(f) = By
     *
     * curvature:
     *          |X'(f)Y''(f) - X''(f)Y'(f)|
     * K(f) = -------------------------------
     *           (X'(f)^2 + Y'(f)^2)^(3/2)
     *
     *          |(Ax + Bx * f) * By - Bx * (Ay + By * f)|
     *      = ---------------------------------------------
     *          ((Ax + Bx * f)^2 + (Ay + By * f)^2)^(3/2)
     *
     *          |Ax * By + Bx * By * f - Bx * Ay - Bx * By * f|
     *      = ------------------------------------------------------------------------------------
     *          (Ax^2 + 2 * Ax * Bx * f + Bx^2 * f^2 + Ay^2 + 2 * Ay * By * f + By^2 * f^2)^(3/2)
     *
     *          |Ax * By - Bx * Ay|
     *      = ------------------------------------------------------------------------------------
     *          (Ax^2 + Ay^2 + 2 * f * (Ax * Bx + Ay * By) + (Bx^2 + By^2) * f^2)^(3/2)
     *
     *          |Ax * By - Bx * Ay|
     *      = -----------------------
     *            (F(f))^(3/2)
     *
     * F(f) = Ax^2 + Ay^2 + 2 * f * (Ax * Bx + Ay * By) + (Bx^2 + By^2) * f^2
     * => F'(f) = 2 * (Ax * Bx + Ay * By) + 2 * f * (Bx^2 + By^2)
     *
     * max(K(f)) => min(F(f)) => F'(f) == 0
     * => 2 * (Ax * Bx + Ay * By) + 2 * f * (Bx^2 + By^2) = 0
     * => f = -(Ax * Bx + Ay * By) / (Bx^2 + By^2)
     *
     * so
     *
     * the curvature is maximum if f == -(Ax * Bx + Ay * By) / (Bx^2 + By^2)
     */
    lx_float_t ax = points[1].x - points[0].x;
    lx_float_t ay = points[1].y - points[0].y;
    lx_float_t bx = points[0].x - points[1].x - points[1].x + points[2].x;
    lx_float_t by = points[0].y - points[1].y - points[1].y + points[2].y;

    /* compute the factor of the max curvature
     *
     * -(Ax * Bx + Ay * By) / (Bx^2 + By^2)
     */
    lx_float_t factor = 0;
    lx_unitdiv(-(ax * bx + ay * by), bx * bx + by * by, &factor);

    // invalid factor?
    if (factor < 0 || factor >= 1.0f) {
        factor = 0;
        lx_assert(0);
    }
    return factor;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_float_t lx_bezier2_near_distance(lx_point_t const points[3]) {
    /* compute the delat x and y of the distance(p1, center(p0, p2))
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
     * p0         x            p2
     *
     */
    lx_float_t dx = lx_avg(points[0].x, points[2].x) - points[1].x;
    lx_float_t dy = lx_avg(points[0].y, points[2].y) - points[1].y;
    dx = lx_abs(dx);
    dy = lx_abs(dy);

    // compute the more approximate distance
    return (dx > dy)? (dx + lx_half(dy)) : (dy + lx_half(dx));
}

lx_size_t lx_bezier2_divide_line_count(lx_point_t const points[3]) {
    // check
    lx_assert(points);

    // compute the approximate distance
    lx_float_t distance = lx_bezier2_near_distance(points);
    lx_assert(distance >= 0);

    // get the integer distance
    lx_uint32_t idistance = (lx_uint32_t)lx_ceil(distance);

    // compute the divided count
    lx_uint32_t count = (lx_ilog2i(idistance) >> 1) + 1;
    if (count > LX_BEZIER2_DIVIDED_MAXN) {
        count = LX_BEZIER2_DIVIDED_MAXN;
    }
    return (lx_size_t)count;
}

lx_void_t lx_bezier2_chop_at(lx_point_t const points[3], lx_point_t output[5], lx_float_t factor) {
    lx_assert(points && output && lx_inunit(factor));
    lx_bezier2_chop_xy_at(&points[0].x, &output[0].x, factor);
    lx_bezier2_chop_xy_at(&points[0].y, &output[0].y, factor);
}

lx_void_t lx_bezier2_chop_at_half(lx_point_t const points[3], lx_point_t output[5]) {
    lx_assert(points && output);

    /* compute the chopped points
     *
     *                  p1
     *                  .
     *                .  .
     *              .     .
     *            .        .
     *       o1 . . . . . . . o3
     *        .      o2      .
     *      .                 .
     *    .                    .
     * p0, o0                p2, o4
     *
     * (p0, p1, p2) => (o0, o1, o2) + (o2, o3, o4)
     */
    lx_float_t x01 = lx_avg(points[0].x, points[1].x);
    lx_float_t y01 = lx_avg(points[0].y, points[1].y);
    lx_float_t x12 = lx_avg(points[1].x, points[2].x);
    lx_float_t y12 = lx_avg(points[1].y, points[2].y);

    // make output
    output[0] = points[0];
    lx_point_make(&output[1], x01, y01);
    lx_point_make(&output[2], lx_avg(x01, x12), lx_avg(y01, y12));
    lx_point_make(&output[3], x12, y12);
    output[4] = points[2];
}

lx_size_t lx_bezier2_chop_at_max_curvature(lx_point_t const points[3], lx_point_t output[5]) {

    // find the factor of the max curvature
    lx_float_t factor = lx_bezier2_find_max_curvature(points);
    lx_size_t count = 2;
    if (output) {
        // chop it
        if (factor != 0) lx_bezier2_chop_at(points, output, factor);
        // using the whole curve
        else {
            lx_memcpy(output, points, 3 * sizeof(lx_point_t));
            count = 1;
        }
    } else if (0 == factor) count = 1;

    // the sub-curve count
    return count;
}

lx_void_t lx_bezier2_make_line(lx_point_t const points[3], lx_bezier2_line_cb_t callback, lx_cpointer_t udata) {
    lx_assert(callback && points);
    lx_size_t count = lx_bezier2_divide_line_count(points);
    lx_bezier2_make_line_impl(points, count, callback, udata);
}
