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
 * @file        geometry.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "geometry.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_double_t lx_point_to_segment_distance_h_cheap(lx_point_ref_t center, lx_point_ref_t upper, lx_point_ref_t lower) {
    lx_assert(center && upper && lower);

    // must be upper <= center <= lower
    lx_assertf(lx_point_in_top_or_horizontal(upper, center), "%{point} <=? %{point}", upper, center);
    lx_assertf(lx_point_in_top_or_horizontal(center, lower), "%{point} <=? %{point}", center, lower);

    // compute the upper and lower y-distances
    lx_float_t yu = center->y - upper->y;
    lx_float_t yl = lower->y - center->y;
    lx_assert(yu >= 0 && yl >= 0);

    // edge(upper, lower) is not horizontal?
    if (yu + yl > 0) {
        /* compute the position
         *
         * distance = (center.x - upper.x) + (upper.x - lower.x) * (yu / (yu + yl))
         * => distance * (yu + yl)  = (center.x - upper.x) * (yu + yl) + (upper.x - lower.x) * yu
         *                          = (center.x - upper.x) * yu + (center.x - upper.x) * yl + (upper.x - lower.x) * yu
         *
         *
         *
         * distance = (center.x - lower.x) + (lower.x - upper.x) * (yl / (yu + yl))
         * => distance * (yu + yl)  = (center.x - lower.x) * (yu + yl) + (lower.x - upper.x) * yl
         *                          = (center.x - lower.x) * yl + (center.x - lower.x) * yu + (lower.x - upper.x) * yl
         *
         * so
         *
         * 2 * distance * (yu + yl) = (center.x - upper.x) * yu + (center.x - upper.x) * yl + (upper.x - lower.x) * yu
         *                          + (center.x - lower.x) * yl + (center.x - lower.x) * yu + (lower.x - upper.x) * yl
         *                          = 2 * (center.x - lower.x) * yu + 2 * (center.x - upper.x) * yl
         *
         * => distance * (yu + yl)  = (center.x - lower.x) * yu + (center.x - upper.x) * yl
         * => sign(distance * (yu + yl))
         * => sign(distance)
         * => position
         *
         */
        return (lx_double_t)(center->x - lower->x) * yu + (lx_double_t)(center->x - upper->x) * yl;
    }

    // horizontal edge
    return 0;
}

static lx_double_t lx_point_to_segment_distance_v_cheap(lx_point_ref_t center, lx_point_ref_t left, lx_point_ref_t right) {
    lx_assert(center && left && right);

    // must be left <= center <= right
    lx_assertf(lx_point_in_left_or_vertical(left, center), "%{point} <=? %{point}", left, center);
    lx_assertf(lx_point_in_left_or_vertical(center, right), "%{point} <=? %{point}", center, right);

    // compute the left and right x-distances
    lx_float_t xl = center->x - left->x;
    lx_float_t xr = right->x - center->x;
    lx_assert(xl >= 0 && xr >= 0);

    // edge(left, right) is not vertical?
    if (xl + xr > 0) {
        // compute the position
        return (lx_double_t)(center->y - right->y) * xl + (lx_double_t)(center->y - left->y) * xr;
    }

    // vertical edge
    return 0;
}

// calculate interpolation: (x * a + y * b) / (a + b) for intersection
static lx_inline lx_float_t lx_segment_intersection_interpolate(lx_float_t x, lx_double_t a, lx_float_t y, lx_double_t b) {
    // a and b may be slightly negative
    if (a < 0) a = 0;
    if (b < 0) b = 0;

    // the results are very accurate even when a and b are very large
    if (a >= b) {
        // we only return x + y / 2 if a and b are zero
        if (0 == a) return lx_avg(x, y);

        // calculate the intersection
        return (lx_float_t)(x + (y - x) * (b / (a + b)));
    } else {
        // calculate the intersection
        return (lx_float_t)(y + (x - y) * (a / (a + b)));
    }
}

/* calculate the x-coordinate of intersection
 *
 *
 *        org2
 *          .
 *          |   .
 *      dy1 |       .
 * . . . . . . . . . . * . . . . . . . . . dst1
 * org1          dx    |    .       | dy2
 *                     |        .   |
 *                     |            .
 *                     |           dst2
 *                     |
 *                  result.x
 *
 * if dy1 + dy2 > 0:
 *    dx / (dst2.x - org2.x) = dy1 / (dy1 + dy2)
 *    dx = dy1 * (dst2.x - org2.x) / (dy1 + dy2)
 *
 * so:
 * result.x = org2.x + dx = org2.x + dy1 * (dst2.x - org2.x) / (dy1 + dy2)
 *          = (org2.x * (dy1 + dy2) + dy1 * (dst2.x - org2.x)) / (dy1 + dy2)
 *          = (org2.x * dy1 + org2.x * dy2 + dy1 * dst2.x - dy1 * org2.x) / (dy1 + dy2)
 *          = (org2.x * dy2 + dst2.x * dy1) / (dy1 + dy2)
 */
static lx_bool_t lx_segment_intersection_x(lx_point_ref_t org1, lx_point_ref_t dst1, lx_point_ref_t org2, lx_point_ref_t dst2, lx_point_ref_t result) {
    lx_assert(org1 && dst1 && org2 && dst2);

    /* sort edges in the order: org1.x <= org2.x <= (dst1/dst2).x
     *
     *        org2                                                    dst2
     *         .                                                       .
     *           .                                                   .
     *             .                                               .
     * . . . . . . . . . . . . . . .          or       . . . . . . . . . . . . .
     * org1            .           dst1              org1      .              dst1
     *                   .                                   .
     *                     .                               org2
     *                     dst2
     */
    if (lx_point_in_right(org1, dst1)) { lx_swap(lx_point_ref_t, org1, dst1); }
    if (lx_point_in_right(org2, dst2)) { lx_swap(lx_point_ref_t, org2, dst2); }
    if (lx_point_in_right(org1, org2)) { lx_swap(lx_point_ref_t, org1, org2);
                                         lx_swap(lx_point_ref_t, dst1, dst2); }

    // check
    lx_assert(org1 != org2);
    lx_assert(dst1 != dst2);

    /* no intersection?
     *                              org2
     *                                .
     *                                  .
     *                                    .
     * . . . . . . . . . . . . . . .        .
     * org1                       dst1        .
     *                                          .
     *                                            .
     *                                             dst2
     */
    if (lx_point_in_right(org2, dst1)) {
        return lx_false;
    } else if (lx_point_in_left_or_vertical(dst1, dst2)) {
        /* two points are projected onto the different line segment
         *
         * no intersection:
         *
         *                          org2
         *                           .
         *                           |  .
         *                       dy1 | |   .
         *                           | |dy2   .
         * . . . . . . . . . . . . . . .         .
         * org1                       dst1          .
         *                                             .
         *                                                .
         *                                                dst2
         *
         * or
         *
         *        org2
         *          .
         *          |   .
         *      dy1 |       .
         * . . . . . . . . . . * . . . . dst1
         * org1          dx    |    .  | dy2
         *                     |        .
         *                     |            .
         *                     |                .
         *                     |                    .
         *                  result.x               dst2
         */

        // calculate the vertical distance: dy1 and dy2
        lx_float_t dy1 = lx_point_to_segment_distance_v(org2, org1, dst1);
        lx_float_t dy2 = lx_point_to_segment_distance_v(dst1, org2, dst2);

        // ensure: (dy1 + dy2) > 0
        if (dy1 + dy2 < 0) {
            dy1 = -dy1;
            dy2 = -dy2;
        }

        /* no intersection? dy1 * dy2 < 0?
         *
         * we also attempt to calculate it if one segment is very close to another segment
         */
        if (dy1 < -LX_NEAR0 || dy2 < -LX_NEAR0) {
            return lx_false;
        }

        /* calculate the x-coordinate of the intersection
         *
         * result.x = (org2.x * dy2 + dst1.x * dy1) / (dy1 + dy2)
         */
        if (result) {
            result->x = lx_segment_intersection_interpolate(org2->x, dy2, dst1->x, dy1);
        }
    } else {
        /* two points are projected onto the same line segment
         *
         * no intersection:
         *
         *        org2
         *          .
         *          |   .
         *          |       .
         *      dy1 |           .  dst2
         *          |               .
         *          |               | dy2
         * . . . . . . . . . . . . . . . . . . . . dst1
         * org1
         *
         * or
         *
         *        org2
         *          .
         *          |   .
         *      dy1 |       .
         * . . . . . . . . . . * . . . . . . . . . dst1
         * org1          dx    |    .       | dy2
         *                     |        .   |
         *                     |            .
         *                     |           dst2
         *                     |
         *                  result.x
         */

        /* quickly calculate the vertical cheap distance: dy1 and dy2
         * and the result will be not changed
         *
         * dy1_cheap = dy1 * (dst1.x - org1.x) = dy1 * dt
         * dy2_cheap = dy2 * (dst1.x - org1.x) = dy2 * dt
         *
         * result.x = (org2.x * dy2_cheap + dst2.x * dy1_cheap) / (dy1_cheap + dy2_cheap)
         *          = (org2.x * dy2 * dt + dst2.x * dy1 * dt) / (dy1 * dt + dy2 * dt)
         *          = (org2.x * dy2 + dst2.x * dy1) / (dy1 + dy2)
         */
        lx_double_t dy1 = lx_point_to_segment_distance_v_cheap(org2, org1, dst1);
        lx_double_t dy2 = -lx_point_to_segment_distance_v_cheap(dst2, org1, dst1);

        // ensure: (dy1 + dy2) > 0
        if (dy1 + dy2 < 0) {
            dy1 = -dy1;
            dy2 = -dy2;
        }

        /* no intersection? dy1 * dy2 < 0?
         *
         * we also attempt to calculate it if one segment is very close to another segment
         */
        if (dy1 < -LX_NEAR0 || dy2 < -LX_NEAR0) {
            return lx_false;
        }

        /* calculate the x-coordinate of the intersection
         *
         * result.x = (org2.x * dy2 + dst2.x * dy1) / (dy1 + dy2)
         */
        if (result) {
            result->x = lx_segment_intersection_interpolate(org2->x, dy2, dst2->x, dy1);
        }
    }

    // exists intersection
    return lx_true;
}

static lx_long_t lx_segment_intersection_y(lx_point_ref_t org1, lx_point_ref_t dst1, lx_point_ref_t org2, lx_point_ref_t dst2, lx_point_ref_t result) {
    lx_assert(org1 && dst1 && org2 && dst2);

    // sort edges in the order: org1.y <= org2.y <= (dst1/dst2).y
    if (lx_point_in_bottom(org1, dst1)) { lx_swap(lx_point_ref_t, org1, dst1); }
    if (lx_point_in_bottom(org2, dst2)) { lx_swap(lx_point_ref_t, org2, dst2); }
    if (lx_point_in_bottom(org1, org2)) { lx_swap(lx_point_ref_t, org1, org2);
                                          lx_swap(lx_point_ref_t, dst1, dst2); }

    // check
    lx_assert(org1 != org2);
    lx_assert(dst1 != dst2);

    // no intersection?
    if (lx_point_in_bottom(org2, dst1)) {
        return lx_false;
    } else if (lx_point_in_top_or_horizontal(dst1, dst2)) { // two points are projected onto the different line segment

        // calculate the horizontal distance: dx1 and dx2
        lx_float_t dx1 = lx_point_to_segment_distance_h(org2, org1, dst1);
        lx_float_t dx2 = lx_point_to_segment_distance_h(dst1, org2, dst2);

        // ensure: (dx1 + dx2) > 0
        if (dx1 + dx2 < 0)
        {
            dx1 = -dx1;
            dx2 = -dx2;
        }

        /* no intersection? dx1 * dx2 < 0?
         *
         * we also attempt to calculate it if one segment is very close to another segment
         */
        if (dx1 < -LX_NEAR0 || dx2 < -LX_NEAR0) return lx_false;

        /* calculate the y-coordinate of the intersection
         *
         * result.y = (org2.y * dx2 + dst1.y * dx1) / (dx1 + dx2)
         */
        if (result) result->y = lx_segment_intersection_interpolate(org2->y, dx2, dst1->y, dx1);

    } else { // two points are projected onto the same line segment

        // quickly calculate the horizontal cheap distance: dx1 and dx2
        lx_double_t dx1 = lx_point_to_segment_distance_h_cheap(org2, org1, dst1);
        lx_double_t dx2 = -lx_point_to_segment_distance_h_cheap(dst2, org1, dst1);

        // ensure: (dx1 + dx2) > 0
        if (dx1 + dx2 < 0) {
            dx1 = -dx1;
            dx2 = -dx2;
        }

        /* no intersection? dx1 * dx2 < 0?
         *
         * we also attempt to calculate it if one segment is very close to another segment
         */
        if (dx1 < -LX_NEAR0 || dx2 < -LX_NEAR0) {
            return lx_false;
        }

        /* calculate the y-coordinate of the intersection
         *
         * result.y = (org2.y * dx2 + dst2.y * dx1) / (dx1 + dx2)
         */
        if (result) {
            result->y = lx_segment_intersection_interpolate(org2->y, dx2, dst2->y, dx1);
        }
    }

    // exists intersection
    return lx_true;
}

static lx_bool_t lx_segment_near_parallel(lx_point_ref_t org1, lx_point_ref_t dst1, lx_point_ref_t org2, lx_point_ref_t dst2) {

    // the slope factors
    lx_float_t dy1 = org1->y - dst1->y;
    lx_float_t dx1 = org1->x - dst1->x;
    lx_float_t dy2 = org2->y - dst2->y;
    lx_float_t dx2 = org2->x - dst2->x;

    // compute the slope errors
    lx_float_t dk = (lx_float_t)((lx_double_t)dy1 * dx2 - (lx_double_t)dy2 * dx1);

    // is parallel?
    return lx_near0(dk);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_long_t lx_points_is_ccw(lx_point_ref_t p0, lx_point_ref_t p1, lx_point_ref_t p2) {
    lx_assert(p0 && p1 && p2);

    // the coordinates
    lx_fixed_t x0 = lx_float_to_fixed(p0->x);
    lx_fixed_t y0 = lx_float_to_fixed(p0->y);
    lx_fixed_t x1 = lx_float_to_fixed(p1->x);
    lx_fixed_t y1 = lx_float_to_fixed(p1->y);
    lx_fixed_t x2 = lx_float_to_fixed(p2->x);
    lx_fixed_t y2 = lx_float_to_fixed(p2->y);

    /* compute the cross value of the vectors (p1, p0) and (p1, p2)
     *
     * cross[(p1, p0), (p1, p2)] > 0
     *
     *
     * the result is not reliable for almost-degenerate situations
     */
    return ((lx_hong_t)(x0 - x1) * (y2 - y1) - (lx_hong_t)(y0 - y1) * (x2 - x1)) > 0;
}

lx_float_t lx_point_to_segment_distance_h(lx_point_ref_t center, lx_point_ref_t upper, lx_point_ref_t lower) {
    lx_assert(center && upper && lower);

    // must be upper <= center <= lower
    lx_assertf(lx_point_in_top_or_horizontal(upper, center), "%{point} <=? %{point}", upper, center);
    lx_assertf(lx_point_in_top_or_horizontal(center, lower), "%{point} <=? %{point}", center, lower);

    // compute the upper and lower y-distances
    lx_float_t yu = center->y - upper->y;
    lx_float_t yl = lower->y - center->y;
    lx_assert(yu >= 0 && yl >= 0);

    // edge(upper, lower) is not horizontal?
    if (yu + yl > 0) {
        // center.y is close to upper.y?
        if (yu < yl) {
            /*
             *              upper
             *       .        .
             *    yu |       .
             *       |      .        xc > 0
             *       .     . -|------------------ . center
             *       |    . xu > 0
             *       |   .
             *    yl |  .
             *       | .
             *       |.
             *       .
             *       lower
             *
             *
             *                                       upper
             *                                .        .
             *                             yu |       .
             *                                |      .
             * center . ----------------------|---- . --
             *                                |    .  xu > 0
             *                                |   .
             *                             yl |  .
             *                                | .
             *                                |.
             *                                .
             *                                lower
             *
             *        |---------------------------------|
             *                       xc < 0
             *
             *
             * near horizontal? xu == 0
             *                               . upper                      . center
             *                 .             |----------------------------|
             *   .                                          xc
             * lower
             *
             * xc = center.x - upper.x
             * xu = (upper.x - lower.x) * (yu / (yu + yl))
             *
             * distance = xc + xu
             *          = (center.x - upper.x) + (upper.x - lower.x) * (yu / (yu + yl))
             */
            lx_double_t factor = (lx_double_t)(upper->x - lower->x) / (yu + yl);
            return (center->x - upper->x) + (lx_float_t)(yu * factor);

        } else { // center.y is close to lower.y?

            /*
             *              upper
             *       .        .
             *       |       .
             *       |      .
             *    yu |     .
             *       |    .
             *       |< 0.
             *       .xl. -------------------- .
             *       | .                     center
             *    yl |.
             *       .
             *       lower
             *
             *       |-------------------------|
             *                 xc > 0
             *
             *
             *
             *                                   upper
             *                            .        .
             *                            |       .
             *                            |      .
             *                         yu |     .
             *                            |    .
             *               xc < 0       |<0 .
             * center .------------------ .xl.
             *                            | .
             *                         yl |.
             *                            .
             *                            lower
             *
             *
             *
             * near horizontal? xl == 0
             *                               . upper
             *                 .
             *   .                               .
             * lower                           center
             *   |-------------------------------|
             *                  xc
             *
             * xc = center.x - lower.x
             * xl = (lower.x - upper.x) * (yl / (yu + yl))
             *
             * distance = xc + xl
             *          = (center.x - lower.x) + (lower.x - upper.x) * (yl / (yu + yl))
             */
            lx_double_t factor = (lx_double_t)(lower->x - upper->x) / (yu + yl);
            return (center->x - lower->x) + (lx_float_t)(yl * factor);
        }
    }

    // horizontal edge? no distance
    return 0;
}

lx_float_t lx_point_to_segment_distance_v(lx_point_ref_t center, lx_point_ref_t left, lx_point_ref_t right) {
    lx_assert(center && left && right);

    // must be left <= center <= right
    lx_assertf(lx_point_in_left_or_vertical(left, center), "%{point} <=? %{point}", left, center);
    lx_assertf(lx_point_in_left_or_vertical(center, right), "%{point} <=? %{point}", center, right);

    // compute the left and right x-distances
    lx_float_t xl = center->x - left->x;
    lx_float_t xr = right->x - center->x;
    lx_assert(xl >= 0 && xr >= 0);

    // edge(left, right) is not vertical?
    if (xl + xr > 0) {
        // center.x is close to left.x?
        if (xl < xr) {
            lx_double_t factor = (lx_double_t)(left->y - right->y) / (xl + xr);
            return (center->y - left->y) + (lx_float_t)(xl * factor);
        } else { // center.x is close to right.x?
            lx_double_t factor = (lx_double_t)(right->y - left->y) / (xl + xr);
            return (center->y - right->y) + (lx_float_t)(xr * factor);
        }
    }

    // vertical edge? no distance
    return 0;
}

lx_long_t lx_point_to_segment_position_h(lx_point_ref_t center, lx_point_ref_t upper, lx_point_ref_t lower) {
    lx_assert(center && upper && lower);

    // compute the cheap distance quickly
    lx_double_t distance = lx_point_to_segment_distance_h_cheap(center, upper, lower);

    // get the sign of the distance
    return distance < 0? -1 : distance > 0;
}

lx_long_t lx_point_to_segment_position_v(lx_point_ref_t center, lx_point_ref_t left, lx_point_ref_t right) {
    lx_assert(center && left && right);

    // compute the cheap distance quickly
    lx_double_t distance = lx_point_to_segment_distance_v_cheap(center, left, right);

    // get the sign of the distance
    return distance < 0? -1 : distance > 0;
}

lx_long_t lx_segment_intersection(lx_point_ref_t org1, lx_point_ref_t dst1, lx_point_ref_t org2, lx_point_ref_t dst2, lx_point_ref_t result) {
    lx_assert(org1 && dst1 && org2 && dst2);

    // near parallel? no intersection
    if (lx_segment_near_parallel(org1, dst1, org2, dst2)) {
        return 0;
    }

    /* calculate the intersection
     *
     * this is certainly not the most efficient way to find the intersection of two line segments,
     * but it is very numerically stable.
     */
    return (    lx_segment_intersection_x(org1, dst1, org2, dst2, result)
            &&  lx_segment_intersection_y(org1, dst1, org2, dst2, result))? 1 : -1;
}
