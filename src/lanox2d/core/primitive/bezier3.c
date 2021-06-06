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
 * @file        bezier3.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bezier3.h"
#include "point.h"
#include "utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_bezier3_make_line_impl(lx_point_t const points[4], lx_size_t count, lx_bezier3_line_cb_t callback, lx_cpointer_t udata) {
    /* divide it
     *
     *          p1                 p2
     *          . . . . . . . . . . .
     *         .                     .
     *        .    o2 . . . . . o4    .
     *       .           o3            .
     *   o1 .             |             . o5
     *     .            chop             .
     *    .                               .
     *   .                                 .
     *  .                                   .
     * .                                     .
     * p0, o0                                p3, o6
     *
     */
    if (count) {
        // chop the cubic at half
        lx_point_t output[7];
        lx_bezier3_chop_at_half(points, output);

        // make line for cubic(o0, o1, o2, o3)
        lx_bezier3_make_line_impl(output, count - 1, callback, udata);

        // make line to cubic(o3, o4, o5, o6)
        lx_bezier3_make_line_impl(output + 3, count - 1, callback, udata);
    }
    else {
        callback((lx_point_ref_t)&points[3], udata);
    }
}

static lx_void_t lx_bezier3_chop_xy_at(lx_float_t const* xy, lx_float_t* output, lx_float_t factor) {
    // compute the interpolation of p0 => p1
    lx_float_t xy01 = lx_interp(xy[0], xy[2], factor);

    // compute the interpolation of p1 => p2
    lx_float_t xy12 = lx_interp(xy[2], xy[4], factor);

    // compute the interpolation of p2 => p3
    lx_float_t xy23 = lx_interp(xy[4], xy[6], factor);

    // compute the interpolation of (p0, p1) => (p1, p2)
    lx_float_t xy012 = lx_interp(xy01, xy12, factor);

    // compute the interpolation of (p1, p2) => (p2, p3)
    lx_float_t xy123 = lx_interp(xy12, xy23, factor);

    // make output
    output[0]   = xy[0];
    output[2]   = xy01;
    output[4]   = xy012;
    output[6]   = lx_interp(xy012, xy12, factor);
    output[8]   = xy123;
    output[10]  = xy23;
    output[12]  = xy[6];
}

static lx_size_t lx_bezier3_find_max_curvature(lx_point_t const points[4], lx_float_t factors[3]) {
    // TODO
    lx_trace_noimpl();
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_float_t lx_bezier3_near_distance(lx_point_t const points[4]) {
    lx_assert(points);

    /* compute the delat x and y
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
     */
    lx_float_t x03 = lx_avg(points[0].x, points[3].x);
    lx_float_t y03 = lx_avg(points[0].y, points[3].y);
    lx_float_t dx1 = x03 - points[1].x;
    lx_float_t dy1 = y03 - points[1].y;
    lx_float_t dx2 = x03 - points[2].x;
    lx_float_t dy2 = y03 - points[2].y;

    dx1 = lx_abs(dx1);
    dy1 = lx_abs(dy1);
    dx2 = lx_abs(dx2);
    dy2 = lx_abs(dy2);

    // compute the more approximate distance
    lx_float_t d1 = (dx1 > dy1)? (dx1 + lx_half(dy1)) : (dy1 + lx_half(dx1));
    lx_float_t d2 = (dx2 > dy2)? (dx2 + lx_half(dy2)) : (dy2 + lx_half(dx2));

    // using the maximum value
    return lx_max(d1, d2);
}

lx_size_t lx_bezier3_divide_line_count(lx_point_t const points[4]) {
    lx_assert(points);

    // compute the approximate distance
    lx_float_t distance = lx_bezier3_near_distance(points);
    lx_assert(distance >= 0);

    // get the integer distance
    lx_size_t idistance = lx_ceil(distance);

    // compute the divided count
    lx_size_t count = (lx_ilog2i(idistance) >> 1) + 1;

    // limit the count
    if (count > LX_BEZIER3_DIVIDED_MAXN) {
        count = LX_BEZIER3_DIVIDED_MAXN;
    }
    return count;
}

lx_void_t lx_bezier3_chop_at(lx_point_t const points[4], lx_point_t output[7], lx_float_t factor) {
    lx_assert(points && output && lx_inunit(factor));

    // chop x-coordinates at the factor
    lx_bezier3_chop_xy_at(&points[0].x, &output[0].x, factor);

    // chop y-coordinates at the factor
    lx_bezier3_chop_xy_at(&points[0].y, &output[0].y, factor);
}

lx_void_t lx_bezier3_chop_at2(lx_point_t const points[4], lx_point_ref_t output, lx_size_t maxn, lx_float_t const* factors, lx_size_t count) {
    lx_assert(points && output && maxn >= 7 && factors && count);
    lx_size_t   index;
    lx_point_t  buffer[4];
    lx_float_t  factor = factors[0];
    for (index = 0; index < count; index++) {
        // check
        lx_assert(maxn >= 7);

        // chop it
        lx_bezier3_chop_at(points, output, factor);

        // end?
        lx_check_break(index != count - 1);

        // update to the next sub-cubic curve output
        output  += 3;
        maxn    -= 3;

        // update the new start points
        lx_memcpy(buffer, output, sizeof(buffer));
        points = buffer;

        // check
        lx_assert(lx_inunit(factors[index]));
        lx_assert(lx_inunit(factors[index + 1]));
        lx_assert(factors[index + 1] > factors[index]);

        // compute the next factor for the new range: (factor[index], 1) => (0, 1)
        if (!lx_unitdiv(factors[index + 1] - factors[index], 1.0f - factors[index], &factor)) {
            // failed? only make a degenerate cubic
            output[4] = output[5] = output[6] = points[3];
            break;
        }
    }
}

lx_void_t lx_bezier3_chop_at_half(lx_point_t const points[4], lx_point_t output[7]) {
    lx_assert(points && output);

    /* compute the chopped points
     *
     *          p1                 p2
     *          . . . . . . . . . . .
     *         .                     .
     *        .    o2 . . . . . o4    .
     *       .           o3            .
     *   o1 .             |             . o5
     *     .            chop             .
     *    .                               .
     *   .                                 .
     *  .                                   .
     * .                                     .
     * p0, o0                                p3, o6
     *
     * (p0, p1, p2, p3) => (o0, o1, o2, o3) + (o3, o4, o5, o6)
     */
    lx_float_t x01 = lx_avg(points[0].x, points[1].x);
    lx_float_t y01 = lx_avg(points[0].y, points[1].y);
    lx_float_t x12 = lx_avg(points[1].x, points[2].x);
    lx_float_t y12 = lx_avg(points[1].y, points[2].y);
    lx_float_t x23 = lx_avg(points[2].x, points[3].x);
    lx_float_t y23 = lx_avg(points[2].y, points[3].y);

    lx_float_t x012 = lx_avg(x01, x12);
    lx_float_t y012 = lx_avg(y01, y12);
    lx_float_t x123 = lx_avg(x12, x23);
    lx_float_t y123 = lx_avg(y12, y23);

    // make output
    output[0] = points[0];
    lx_point_make(&output[1], x01, y01);
    lx_point_make(&output[2], x012, y012);
    lx_point_make(&output[3], lx_avg(x012, x123), lx_avg(y012, y123));
    lx_point_make(&output[4], x123, y123);
    lx_point_make(&output[5], x23, y23);
    output[6] = points[3];
}

lx_size_t lx_bezier3_chop_at_max_curvature(lx_point_t const points[4], lx_point_t output[13]) {
    lx_assert(points);

    // find the factors of the max curvature
    lx_float_t  factors[3];
    lx_size_t   factors_count = lx_bezier3_find_max_curvature(points, factors);
    lx_assert(factors_count <= 3);

    if (output) {
        // chop it
        if (factors_count) lx_bezier3_chop_at2(points, output, 13, factors, factors_count);
        // using the whole curve
        else lx_memcpy(output, points, sizeof(lx_point_t) << 2);
    }

    // the sub-curve count
    return factors_count + 1;
}

lx_void_t lx_bezier3_make_line(lx_point_t const points[4], lx_bezier3_line_cb_t callback, lx_cpointer_t udata) {
    lx_assert(callback && points);
    lx_size_t count = lx_bezier3_divide_line_count(points);
    lx_bezier3_make_line_impl(points, count, callback, udata);
}

