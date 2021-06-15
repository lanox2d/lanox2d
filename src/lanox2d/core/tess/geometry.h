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
 * @file        geometry.h
 *
 */
#ifndef LX_CORE_TESS_GEOMETRY_H
#define LX_CORE_TESS_GEOMETRY_H

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

/// a is in b's top?
#define lx_point_in_top(a, b)                   ((a)->y < (b)->y)

/// a is in b's top or horizontal?
#define lx_point_in_top_or_horizontal(a, b)     ((a)->y <= (b)->y)

/// a is in b's bottom?
#define lx_point_in_bottom(a, b)                ((a)->y > (b)->y)

/// a is in b's bottom or horizontal?
#define lx_point_in_bottom_or_horizontal(a, b)  ((a)->y >= (b)->y)

/// a is in b's left?
#define lx_point_in_left(a, b)                  ((a)->x < (b)->x)

/// a is in b's left or vertical?
#define lx_point_in_left_or_vertical(a, b)      ((a)->x <= (b)->x)

/// a is in b's right?
#define lx_point_in_right(a, b)                 ((a)->x > (b)->x)

/// a is in b's right or vertical?
#define lx_point_in_right_or_vertical(a, b)     ((a)->x >= (b)->x)

/*! v is in segment(u, l)'s left?
 *
 *                      u
 *                      .
 *                       .
 *                        .
 *          . ------------ .
 *          v               .
 *                           .
 *                            .
 *                            l
 *
 */
#define lx_point_in_segment_left(v, u, l)       (lx_point_to_segment_position_h(v, u, l) < 0)

/*! v is on segment(u, l) or it's left?
 *
 *                      u
 *                      .
 *                       .
 *                        .
 *          . ------------ . v'
 *          v               .
 *                           .
 *                            .
 *                            l
 *
 */
#define lx_point_on_segment_or_left(v, u, l)    (lx_point_to_segment_position_h(v, u, l) <= 0)

/*! v is in segment(u, l)'s right?
 *
 *       u
 *       .
 *      .
 *     .
 *    . -------- .
 *   .           v
 *  .
 * .
 * l
 *
 */
#define lx_point_in_segment_right(v, u, l)      (lx_point_to_segment_position_h(v, u, l) > 0)

/*! v is on segment(u, l) or it's right?
 *
 *       u
 *       .
 *      .
 *     .
 *  v'. -------- .
 *   .           v
 *  .
 * .
 * l
 *
 */
#define lx_point_on_segment_or_right(v, u, l)   (lx_point_to_segment_position_h(v, u, l) >= 0)

/*! v is in segment(u, l)'s top?
 *
 *                   . v
 *                   |
 *        l          |
 *         .         |
 *              .    |
 *                   .
 *                        .
 *                             . r
 *
 */
#define lx_point_in_segment_top(v, l, r)        (lx_point_to_segment_position_v(v, l, r) < 0)

/*! v is on segment(u, l) or it's top?
 *
 *                   . v
 *                   |
 *        l          |
 *         .         |
 *              .    |
 *                   .
 *                   v'   .
 *                             . r
 */
#define lx_point_on_segment_or_top(v, l, r)     (lx_point_to_segment_position_v(v, l, r) <= 0)

/*! v is in segment(u, l)'s bottom?
 *
 *                             . r
 *                        .
 *                   .
 *              .    |
 *         .         |
 *   l               |
 *                   |
 *                   . v
 *
 */
#define lx_point_in_segment_bottom(v, l, r)     (lx_point_to_segment_position_v(v, l, r) > 0)

/*! v is on segment(u, l) or it's bottom?
 *
 *                             . r
 *                        .
 *                   . v'
 *              .    |
 *         .         |
 *   l               |
 *                   |
 *                   . v
 *
 */
#define lx_point_on_segment_or_bottom(v, l, r)  (lx_point_to_segment_position_v(v, l, r) >= 0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the three points are counter-clockwise?
 *
 *                   p1
 *  . <-------------- .
 * p2                / \
 *                    |
 *                    |
 *                    |
 *                    .
 *                   p0
 *
 * @param p0            the first point
 * @param p1            the second point
 * @param p2            the last point
 *
 */
lx_long_t               lx_points_is_ccw(lx_point_ref_t p0, lx_point_ref_t p1, lx_point_ref_t p2);

/*! compute the point-to-segment horizontal distance
 *
 *     upper            upper'
 *       .               .
 *      .    distance     .
 *     .   > 0       < 0   .
 *    . -------- . -------- .
 *   .        center         .
 *  .                         .
 * .                           .
 * lower                       lower'
 *
 * distance = (center - segment(upper, lower)).x
 *
 * @param center        the center point
 * @param upper         the upper point of the segment
 * @param lower         the lower point of the segment
 *
 * @return              the horizontal distance
 */
lx_float_t              lx_point_to_segment_distance_h(lx_point_ref_t center, lx_point_ref_t upper, lx_point_ref_t lower);

/*! compute the point-to-segment vertical distance
 *
 *                             . right
 *                        .
 *                   .
 *              .    |
 *         .         | distance: > 0
 *   left            |
 *                   |
 *                   . center
 *                   |
 *   left'           |
 *         .         | distance': < 0
 *              .    |
 *                   .
 *                        .
 *                             . right'
 *
 * distance = (center - segment(left, right)).y
 *
 * @param center        the center point
 * @param left          the left point of the segment
 * @param right         the right point of the segment
 *
 * @return              the vertical distance
 */
lx_float_t              lx_point_to_segment_distance_v(lx_point_ref_t center, lx_point_ref_t left, lx_point_ref_t right);

/*! compute the point-to-segment horizontal position
 *
 * only evaluate the sign of the distance, faster than distance()
 *
 *     upper            upper'
 *       .               .
 *      .    position     .
 *     .   > 0       < 0   .
 *    . -------- . -------- .
 *   .        center         .
 *  .                         .
 * .                           .
 * lower                       lower'
 *
 * position = sign((center - segment(upper, lower)).x)
 *
 * @param center        the center point
 * @param upper         the upper point of the segment
 * @param lower         the lower point of the segment
 *
 * @return              the horizontal position
 */
lx_long_t               lx_point_to_segment_position_h(lx_point_ref_t center, lx_point_ref_t upper, lx_point_ref_t lower);

/*! compute the point-to-segment vertical position
 *
 * only evaluate the sign of the distance, faster than distance()
 *
 *                             . right
 *                        .
 *                   .
 *              .    |
 *         .         | position: > 0
 *   left            |
 *                   |
 *                   . center
 *                   |
 *   left'           |
 *         .         | position': < 0
 *              .    |
 *                   .
 *                        .
 *                             . right'
 *
 * position = sign((center - segment(left, right)).y)
 *
 * @param center        the center point
 * @param left          the left point of the segment
 * @param right         the right point of the segment
 *
 * @return              the vertical position
 */
lx_long_t               lx_point_to_segment_position_v(lx_point_ref_t center, lx_point_ref_t left, lx_point_ref_t right);

/*! calculate the intersection of two segments
 *
 * we would be best to reject it quickly first before calling this function
 *
 * <pre>
 *
 *    org2
 *       . intersection
 *         .   |
 *           . |
 * . . . . . . . . . . . . . . . . dst1
 * org1          .
 *                 .
 *                   .
 *                  dst2
 *
 * </pre>
 *
 * @param org1          the origin of the segment1
 * @param dst1          the destination of the segment1
 * @param org2          the origin of the segment2
 * @param dst2          the destination of the segment2
 * @param result        the intersection result, only get the return value if be null
 *
 * @return              no intersection: -1, parallel line: 0, exists intersection: 1
 */
lx_long_t               lx_segment_intersection(lx_point_ref_t org1, lx_point_ref_t dst1, lx_point_ref_t org2, lx_point_ref_t dst2, lx_point_ref_t result);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


