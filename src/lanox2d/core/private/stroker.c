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
 * @file        stroker.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stroker.h"
#include "../path.h"
#include "../paint.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// cos(179.55c): -0.9999691576f
#define LX_STROKER_TOO_SHARP_LIMIT       (-0.9999691576f)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/* the stroker capper type
 *
 * @param path                  the path
 * @param center                the center point
 * @param end                   the end point
 * @param normal                the normal vector of the outer contour
 * @param is_line_to            is line-to?
 */
typedef lx_void_t               (*lx_stroker_capper_t)(lx_path_ref_t path, lx_point_ref_t center, lx_point_ref_t end, lx_vector_ref_t normal, lx_bool_t is_line_to);

/* the stroker joiner type
 *
 * @param inner                 the inner path
 * @param outer                 the outer path
 * @param center                the center point
 * @param radius                the radius
 * @param normal_unit_before    the before unit normal of the outer contour
 * @param normal_unit_after     the after unit normal of the outer contour
 * @param miter_invert          the invert miter limit
 * @param is_line_to_prev       is previous line-to?
 * @param is_line_to            is line-to?
 */
typedef lx_void_t               (*lx_stroker_joiner_t)(lx_path_ref_t inner, lx_path_ref_t outer, lx_point_ref_t center, lx_float_t radius, lx_vector_ref_t normal_unit_before, lx_vector_ref_t normal_unit_after, lx_float_t miter_invert, lx_bool_t is_line_to_prev, lx_bool_t is_line_to);

// the stroker joiner angle type enum
typedef enum __lx_stroker_joiner_angle_type_e
{
    LX_STROKER_JOINER_ANGLE_NEAR0   = 0
,   LX_STROKER_JOINER_ANGLE_NEAR180 = 1
,   LX_STROKER_JOINER_ANGLE_OBTUSE  = 2
,   LX_STROKER_JOINER_ANGLE_SHARP   = 3

}lx_stroker_joiner_angle_type_e;

// the stroker impl type
typedef struct __lx_stroker_impl_t
{
    // the cap
    lx_size_t               cap;

    // the join
    lx_size_t               join;

    // the radius
    lx_float_t              radius;

    // the miter limit
    lx_float_t              miter;

    // the invert miter limit: 1 / miter
    lx_float_t              miter_invert;

    // the outer path and is the output path
    lx_path_ref_t           path_outer;

    // the inner path and is the temporary path, need reuse it
    lx_path_ref_t           path_inner;

    // the other complete contours path
    lx_path_ref_t           path_other;

    // the previous point of the contour
    lx_point_t              point_prev;

    // the first point of the contour
    lx_point_t              point_first;

    // the first point of the outer contour
    lx_point_t              outer_first;

    // the previous normal for the outer
    lx_vector_t             normal_prev;

    // the first normal_first for the outer
    lx_vector_t             normal_first;

    // the previous unit normal of the outer contour
    lx_vector_t             normal_unit_prev;

    // the first unit normal of the outer contour
    lx_vector_t             normal_unit_first;

    // the segment count
    lx_long_t               segment_count;

    // the previous operation of the contour is line-to?
    lx_bool_t               is_line_to_prev;

    // the first operation of the contour is line-to?
    lx_bool_t               is_line_to_first;

    // the capper
    lx_stroker_capper_t     capper;

    // the joiner
    lx_stroker_joiner_t     joiner;

}lx_stroker_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_stroker_add_hint(lx_stroker_ref_t stroker, lx_shape_ref_t hint, lx_bool_t* convex)
{
    // check
    lx_check_return_val(hint, lx_false);

    // done
    lx_bool_t ok = lx_false;
    switch (hint->type)
    {
    case LX_SHAPE_TYPE_RECT:
        {
            // add rect
            lx_stroker_add_rect(stroker, &hint->u.rect);

            // ok
            ok = lx_true;
        }
        break;
    case LX_SHAPE_TYPE_LINE:
        {
            // add lines
            lx_point_t points[2];
            points[0] = hint->u.line.p0;
            points[1] = hint->u.line.p1;
            lx_stroker_add_lines(stroker, points, lx_arrayn(points));

            // set convex
            if (convex) *convex = lx_true;

            // ok
            ok = lx_true;
        }
        break;
    case LX_SHAPE_TYPE_CIRCLE:
        {
            // add circle
            lx_stroker_add_circle(stroker, &hint->u.circle);

            // ok
            ok = lx_true;
        }
        break;
    case LX_SHAPE_TYPE_ELLIPSE:
        {
            // add ellipse
            lx_stroker_add_ellipse(stroker, &hint->u.ellipse);

            // ok
            ok = lx_true;
        }
        break;
    case LX_SHAPE_TYPE_POINT:
        {
            // add point
            lx_stroker_add_points(stroker, &hint->u.point, 1);

            // set convex
            if (convex) *convex = lx_true;

            // ok
            ok = lx_true;
        }
        break;
    default:
        break;
    }

    // ok?
    return ok;
}
static lx_void_t lx_stroker_capper_butt(lx_path_ref_t path, lx_point_ref_t center, lx_point_ref_t end, lx_vector_ref_t normal, lx_bool_t is_line_to)
{
    // check
    lx_assert(path && end);

    /* cap th butt
     *
     *                       normal
     *              ----------------------> first outer
     *             |  radius   |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     * reverse add |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     * last inner \|/         \|/         \|/
     *          inner        center       outer
     *             <------------------------
     *                        cap
     */
    lx_path_line_to(path, end);
}
static lx_void_t lx_stroker_capper_round(lx_path_ref_t path, lx_point_ref_t center, lx_point_ref_t end, lx_vector_ref_t normal, lx_bool_t is_line_to)
{
    // check
    lx_assert(path && center && end && normal);

    /* cap th round
     *
     *                        .
     *                     .   . L
     *                  .       .
     *               .           c1
     *            .
     *         .                   c2
     *      .                      .
     *   .   a                     . L
     * . . . . . . . . . . . . . . .
     *              1
     *
     * L = 4 * tan(a / 4) / 3
     *
     *      L
     * . . . . . . c1
     * .
     * .
     * .
     * .
     * .
     * .
     * .
     * .                           c2
     * .                           .
     * .                           .
     * .                           . L
     * . a = 90                    .
     * . . . . . . . . . . . . . . .
     *
     * L = 4 * tan(pi/8) / 3 if a == 90 degree
     *
     *
     *                       normal
     *              ----------------------> first outer
     *             |  radius   |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     * reverse add |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *         p3 \|/         \|/         \|/ p1
     *          inner        center       outer
     *             .           .           .
     *          L4 . .         .         . . L1
     *             .   .       .  arc  .   .
     *            c4       .   .   .       c1
     *                 c3. . . . . . .c2
     *                    L3   p2   L2
     *
     *                        cap
     *
     * factor = 4 * tan(pi/8) / 3
     *
     * L1 = (normal * factor).rotate(90, cw)
     *    = (lx, ly).rotate(90, cw)
     *    = (-ly, lx)
     *
     * L2 = (normal * factor)
     *    = (lx, ly)
     *
     * L3 = -L2
     *    = (-lx, -ly)
     *
     * L4 = L1
     *    = (-ly, lx)
     *
     * p1 = center + normal
     *    = (x0 + nx, y0 + ny)
     *
     * p2 = center + normal.rotate(90, cw)
     *    = (x0 - ny, y0 + nx)
     *
     * p3 = center - normal
     *    = (x0 - nx, y0 - ny)
     *    = end
     *
     * c1 = p1 + L1
     *    = (x0 + nx - ly, y0 + ny + lx)
     *
     * c2 = p2 + L2
     *    = (x0 - ny + lx, y0 + nx + ly)
     *
     * c3 = p2 + L3
     *    = (x0 - ny - lx, y0 + nx - ly)
     *
     * c4 = p3 + L4
     *    = (x0 - nx - ly, y0 - ny + lx)
     *
     * cap:
     * cubic_to(c1, c2, p2) = cubic_to(x0 + nx - ly, y0 + ny + lx, x0 - ny + lx, y0 + nx + ly, x0 - ny, y0 + nx)
     * cubic_to(c3, c4, p3) = cubic_to(x0 - ny - lx, y0 + nx - ly, x0 - nx - ly, y0 - ny + lx, x0 - nx, y0 - ny)
     */

    // the factors
    lx_float_t    x0 = center->x;
    lx_float_t    y0 = center->y;
    lx_float_t    nx = normal->x;
    lx_float_t    ny = normal->y;
    lx_float_t    lx = lx_mul(nx, LX_ARC_MAKE_CUBIC_FACTOR);
    lx_float_t    ly = lx_mul(ny, LX_ARC_MAKE_CUBIC_FACTOR);

    // cap the round
    lx_path_cubic2_to(path, x0 + nx - ly, y0 + ny + lx, x0 - ny + lx, y0 + nx + ly, x0 - ny, y0 + nx);
    lx_path_cubic2_to(path, x0 - ny - lx, y0 + nx - ly, x0 - nx - ly, y0 - ny + lx, end->x, end->y);
}
static lx_void_t lx_stroker_capper_square(lx_path_ref_t path, lx_point_ref_t center, lx_point_ref_t end, lx_vector_ref_t normal, lx_bool_t is_line_to)
{
    // check
    lx_assert(path && center && end && normal);

    // make the patched vector
    lx_vector_t patched;
    lx_vector_rotate2(normal, &patched, LX_ROTATE_DIRECTION_CW);

    /* cap the square
     *
     *                       normal
     *              ----------------------> first outer
     *             |  radius   |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     * reverse add |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     *             |           |           |
     * last inner \|/         \|/         \|/
     *    end   inner        center       outer
     *            /|\                      |
     *             |                       | patched
     *             |                       |
     *             |                      \|/
     *             <------------------------
     *                        cap
     *
     */
    if (is_line_to)
    {
        // ignore the two points for optimization if cap the line-to contour
        lx_point_t last;
        lx_point_make(&last, center->x + normal->x + patched.x, center->y + normal->y + patched.y);
        lx_path_last_set(path, &last);
        lx_path_line2_to(path, center->x - normal->x + patched.x, center->y - normal->y + patched.y);
    }
    else
    {
        lx_path_line2_to(path, center->x + normal->x + patched.x, center->y + normal->y + patched.y);
        lx_path_line2_to(path, center->x - normal->x + patched.x, center->y - normal->y + patched.y);
        lx_path_line_to(path, end);
    }
}
static lx_float_t lx_stroker_joiner_angle(lx_vector_ref_t normal_unit_before, lx_vector_ref_t normal_unit_after, lx_size_t* ptype)
{
    // check
    lx_assert(normal_unit_before && normal_unit_after);

    /* the cos(angle) value
     *
     *                   normal_before
     *                         |
     * . . . . . . . . . . . . .
     *                         .
     *                       R .
     *                         .
     *                         . angle
     *               . . . . . c . . . . . --> normal_after
     *               .         .    R    .
     *               .         .         .
     *               .         .         .
     *               .         .         .
     * . . . . . . . . . . . . .         .
     *               .                   .
     *               .                   .
     *               .                   .
     *               .                   .
     *               .                   .
     *               .                   .
     *
     */
    lx_float_t angle = lx_vector_dot(normal_unit_before, normal_unit_after);

    // compute the angle type
    if (ptype)
    {
        if (angle < 0) *ptype = (LX_ONE + angle) <= LX_NEAR0? LX_STROKER_JOINER_ANGLE_NEAR180 : LX_STROKER_JOINER_ANGLE_OBTUSE;
        else *ptype = (LX_ONE - angle) <= LX_NEAR0? LX_STROKER_JOINER_ANGLE_NEAR0 : LX_STROKER_JOINER_ANGLE_SHARP;
    }

    // the angle
    return angle;
}
static lx_void_t lx_stroker_joiner_outer(lx_point_ref_t ctrl, lx_point_ref_t point, lx_cpointer_t priv)
{
    // check
    lx_path_ref_t outer = (lx_path_ref_t)priv;
    lx_assert(outer && point);

    // add quadratic curve for the outer contour
    if (ctrl) lx_path_quad_to(outer, ctrl, point);
}
static lx_void_t lx_stroker_joiner_inner(lx_path_ref_t inner, lx_point_ref_t center, lx_vector_ref_t normal_after)
{
    /* join the inner contour
     *
     *               <-
     *               . . . . center
     *               .     .
     *               .     .
     * . . . . . . . . . . .
     * before        .    ->
     *               .
     *               .
     *               .
     *               .
     *             after
     *
     * @note need patch a center first for the radius is larger than segments
     *
     *          i2 . .
     *           .   .
     *           .   .
     * . . . . . . . . . . . . .
     * .         .   .   \|/   .
     *i1 . . . . . c . . . . . . - normal_before
     *           .   .      .
     *           .<- .    .
     *           .   .  .  outer
     *           .   .
     *           . . .
     *           |
     *      normal_after
     *
     * inner: i1 => c = > i2
     */
    lx_path_line2_to(inner, center->x, center->y);
    lx_path_line2_to(inner, center->x - normal_after->x, center->y - normal_after->y);
}
static lx_void_t lx_stroker_joiner_miter(lx_path_ref_t inner, lx_path_ref_t outer, lx_point_ref_t center, lx_float_t radius, lx_vector_ref_t normal_unit_before, lx_vector_ref_t normal_unit_after, lx_float_t miter_invert, lx_bool_t is_line_to_prev, lx_bool_t is_line_to)
{
    // check
    lx_assert(inner && outer && center && normal_unit_before && normal_unit_after);

    /*                   normal_before
     *                         |
     * . . . . . . . . . . . . . . . . . . miter
     *                         .    L  . .
     *                         .     .   .
     *                         .   .     .
     *                         . a       .
     *               . . . . . c . . . . . --> normal_after
     *               .         .    R    .
     *               .         .         .
     *               .         .         .
     *               .         .         .
     * . . . . . . . . . . . . .         .
     *               .                   .
     *               .                   .
     *               .                   .
     *               .                   .
     *               .                   .
     *               .                   .
     *
     * R: radius
     * M: miter limit
     * L: miter length
     * a: degree
     *
     * M = L / R
     * L = R / cos(a/2)
     */

    // compute the cos(a) value of the angle
    lx_size_t   type;
    lx_float_t  cos_angle = lx_stroker_joiner_angle(normal_unit_before, normal_unit_after, &type);

    // the join is nearly line? ignore this join directly
    if (type == LX_STROKER_JOINER_ANGLE_NEAR0) return ;

    // the unit normal vectors and direction
    lx_vector_t before       = *normal_unit_before;
    lx_vector_t after        = *normal_unit_after;

    // done miter
    lx_vector_t     miter;
    lx_bool_t       miter_join = lx_true;
    do
    {
        // the join is nearly 180 degrees? join the bevel
        if (type == LX_STROKER_JOINER_ANGLE_NEAR180)
        {
            // disable the line-to optimization
            is_line_to = lx_false;

            // switch to the bevel join
            miter_join = lx_false;
            break;
        }

        // counter-clockwise? reverse it
        lx_bool_t clockwise;
        if (!(clockwise = lx_vector_is_clockwise(normal_unit_before, normal_unit_after)))
        {
            // swap the inner and outer path
            lx_swap(lx_path_ref_t, inner, outer);

            // reverse the before normal
            lx_vector_negate(&before);

            // reverse the after normal
            lx_vector_negate(&after);
        }

        /* right angle(90 degrees)? done the more faster and accuracy miter
         *
         * .       .
         * .     .
         * .   . L
         * . .
         * . . . . .
         *     R
         *
         * if (M = L / R >= sqrt(2)) miter
         * if (1 / m <= 1 / sqrt(2)) miter
         */
        if (lx_near0(cos_angle) && miter_invert <= LX_ONEOVER_SQRT2)
        {
            lx_vector_make(&miter, lx_mul(before.x + after.x, radius), lx_mul(before.y + after.y, radius));
            break;
        }

        /* compute the cos(a/2)
         *
         * cos(a/2) = sqrt((1 + cos(a)) / 2)
         */
        lx_float_t cos_half_a = lx_sqrt(lx_avg(LX_ONE, cos_angle));

        /* limit the miter length
         *
         * if (L / R > M) strip
         * if (L > M * R) strip
         * if (R / cos(a/2) > M * R) strip
         * if (1 / cos(a/2) > M) strip
         * if (1 / M > cos(a/2)) strip
         */
        if (miter_invert > cos_half_a)
        {
            // disable the line-to optimization
            is_line_to = lx_false;

            // switch to the bevel join
            miter_join = lx_false;
            break;
        }

        /* compute the miter length
         *
         * L = R / cos(a/2)
         */
        lx_float_t length = lx_div(radius, cos_half_a);

        // compute the miter vector
        if (type == LX_STROKER_JOINER_ANGLE_OBTUSE)
        {
            /* compute the more accurate miter vector for the obtuse angle
             *
             *                              miter
             *                               .
             *            after.rotate(ccw) .
             *                          .  .  . before.rotate(cw)
             *                          . . .
             *                          . .
             *                          .
             *                        . .
             *                      . . .
             *          before    .  .  .
             *              \   .   .   .
             *                .    .    .
             *              .   .angle  .
             *            . . . . c . . . -> after
             *          .   .       .   .
             *        .     .         . .
             *      .       .       .   .
             *    .         .     .     .
             *  .           .   .       .
             *              . .         .
             *              .           .
             *            . .           .
             *          .   .           .
             *              .           .
             *
             * miter = before.rotate(cw) + after.rotate(ccw)
             *
             * @note:
             * the miter vector will be more accurate for setting length
             * because miter.length > (before + after).length
             */
            lx_vector_make(&miter, after.y - before.y, before.x - after.x);
            if (!clockwise) lx_vector_negate(&miter);
        }
        else lx_vector_make(&miter, before.x + after.x, before.y + after.y);
        lx_vector_length_set(&miter, length);

    } while (0);

    // scale the after normal
    lx_vector_scale(&after, radius);

    // join the outer contour
    if (miter_join)
    {
        if (is_line_to_prev)
        {
            // ignore one point for optimization if join the previous line-to contour
            lx_point_t last;
            lx_point_make(&last, center->x + miter.x, center->y + miter.y);
            lx_path_last_set(outer, &last);
        }
        else lx_path_line2_to(outer, center->x + miter.x, center->y + miter.y);
    }

    // ignore one point for optimization if join the line-to contour
    if (!is_line_to) lx_path_line2_to(outer, center->x + after.x, center->y + after.y);

    // join the inner contour
    lx_stroker_joiner_inner(inner, center, &after);
}
static lx_void_t lx_stroker_joiner_round(lx_path_ref_t inner, lx_path_ref_t outer, lx_point_ref_t center, lx_float_t radius, lx_vector_ref_t normal_unit_before, lx_vector_ref_t normal_unit_after, lx_float_t miter_invert, lx_bool_t is_line_to_prev, lx_bool_t is_line_to)
{
    // check
    lx_assert(inner && outer && center && normal_unit_before && normal_unit_after);

    // compute the angle type
    lx_size_t type;
    lx_stroker_joiner_angle(normal_unit_before, normal_unit_after, &type);

    // the join is nearly line? ignore this join directly
    if (type == LX_STROKER_JOINER_ANGLE_NEAR0) return ;

    // the unit normal vectors and direction
    lx_vector_t start       = *normal_unit_before;
    lx_vector_t stop        = *normal_unit_after;
    lx_size_t   direction   = LX_ROTATE_DIRECTION_CW;

    // counter-clockwise? reverse it
    if (!lx_vector_is_clockwise(normal_unit_before, normal_unit_after))
    {
        // swap the inner and outer path
        lx_swap(lx_path_ref_t, inner, outer);

        // reverse the start normal
        lx_vector_negate(&start);

        // reverse the stop normal
        lx_vector_negate(&stop);

        // reverse direction
        direction = LX_ROTATE_DIRECTION_CCW;
    }

    // init matrix
    lx_matrix_t matrix;
    lx_matrix_init_scale(&matrix, radius, radius);
    lx_matrix_translate_lhs(&matrix, center->x, center->y);

    // join the outer contour
    lx_arc_make_quad2(&start, &stop, &matrix, direction, lx_stroker_joiner_outer, outer);

    // join the inner contour
    lx_vector_scale(&stop, radius);
    lx_stroker_joiner_inner(inner, center, &stop);
}
static lx_void_t lx_stroker_joiner_bevel(lx_path_ref_t inner, lx_path_ref_t outer, lx_point_ref_t center, lx_float_t radius, lx_vector_ref_t normal_unit_before, lx_vector_ref_t normal_unit_after, lx_float_t miter_invert, lx_bool_t is_line_to_prev, lx_bool_t is_line_to)
{
    // check
    lx_assert(inner && outer && center && normal_unit_before && normal_unit_after);

    /* the after normal
     *
     *                      normal_before
     *            outer          |
     * . . . . . . . . . . . . . o1
     * .                         . .
     * .           -->      i2 . c . o2 -> normal_after
     * .                     .   .   .
     * . . . . . . . . . . . . . i1  .
     *            inner      .       .
     *                       .       .
     *                       .       .
     *                       .       .
     *                 inner .       . outer
     *                       .       .
     *                       .       .
     *                       .       .
     *                       .       .
     *
     *
     * outer: o1 => o2
     * inner: i1 => c = > i2
     */
    lx_vector_t normal_after;
    lx_vector_scale2(normal_unit_after, &normal_after, radius);

    // counter-clockwise? reverse it
    if (!lx_vector_is_clockwise(normal_unit_before, normal_unit_after))
    {
        // swap the inner and outer path
        lx_swap(lx_path_ref_t, inner, outer);

        // reverse the after normal
        lx_vector_negate(&normal_after);
    }

    // join the outer contour
    lx_path_line2_to(outer, center->x + normal_after.x, center->y + normal_after.y);

    // join the inner contour
    lx_stroker_joiner_inner(inner, center, &normal_after);
}
static lx_bool_t lx_stroker_normals_make(lx_point_ref_t before, lx_point_ref_t after, lx_float_t radius, lx_vector_ref_t normal, lx_vector_ref_t normal_unit)
{
    // check
    lx_assert(before && after && normal_unit);

    // the radius
    lx_assert_and_check_return_val(!normal || radius > 0, lx_false);

    /* compute the unit normal vector
     *
     *        ---------------------->  normal
     *       |  radius   |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *       |           |           |
     *      \|/         \|/         \|/
     *    inner         line        outer
     *
     */
    if (!lx_vector_make_unit(normal_unit, after->x - before->x, after->y - before->y)) return lx_false;
    lx_vector_rotate(normal_unit, LX_ROTATE_DIRECTION_CCW);

    // compute the normal vector
    if (normal) lx_vector_scale2(normal_unit, normal, radius);

    // ok
    return lx_true;
}
static __lx_inline__ lx_bool_t lx_stroker_normals_too_curvy(lx_float_t cos_angle)
{
    /*
     *              curve
     *               . .
     *             .     .
     *           .         .
     * before   .           .   after
     *      \  .             . /
     *        .      . .     .
     *           . .     . .
     *             .     .
     *               . .
     *              angle
     *
     * cos(angle) <= sqrt(2) / 2 + 0.1
     * angle >= 45 - 9 = 36 degrees
     *
     * curvy: angle(curve) = 180 - angle <= 135 + 9 = 144 degrees
     */
    return (cos_angle <= (LX_SQRT2_OVER2 + LX_ONE / 10));
}
#if 0
static __lx_inline__ lx_bool_t lx_stroker_normals_too_sharp(lx_vector_ref_t normal_unit_before, lx_vector_ref_t normal_unit_after)
{
    // check
    lx_assert(normal_unit_before && normal_unit_after);

    /*
     *    curve
     *      .
     *     . .
     *     . .
     *     . .
     *     . .
     *     . .
     *     . .
     *     . .
     *
     * cos(angle) <= -0.9999691576f
     * angle >= 179.55 degrees
     *
     * curvy: angle(curve) = 180 - angle < 0.45 degrees
     */
    return lx_vector_dot(normal_unit_before, normal_unit_after) <= LX_STROKER_TOO_SHARP_LIMIT;
}
#endif
static lx_void_t lx_stroker_make_line_to(lx_stroker_impl_t* impl, lx_point_ref_t point, lx_vector_ref_t normal)
{
    // check
    lx_assert(impl && impl->path_inner && impl->path_outer && point && normal);

    // line to the point for the inner and outer path
    lx_path_line2_to(impl->path_outer, point->x + normal->x, point->y + normal->y);
    lx_path_line2_to(impl->path_inner, point->x - normal->x, point->y - normal->y);
}
static lx_void_t lx_stroker_make_quad_to(lx_stroker_impl_t* impl, lx_point_ref_t points, lx_vector_ref_t normal_01, lx_vector_ref_t normal_unit_01, lx_vector_ref_t normal_12, lx_vector_ref_t normal_unit_12, lx_size_t divided_count)
{
    // check
    lx_assert(impl && points && normal_01 && normal_unit_01 && normal_12 && normal_unit_12);

    // compute the normal and unit normal vectors for b => c
    if (!lx_stroker_normals_make(&points[1], &points[2], impl->radius, normal_12, normal_unit_12))
    {
        // p1 nearly equals p2? make line-to
        lx_stroker_make_line_to(impl, &points[2], normal_01);

        // save the normal and unit normal for b => c
        *normal_12 = *normal_01;
        *normal_unit_12 = *normal_unit_01;
        return ;
    }

    // compute the cos(angle) of the normal_01 and normal_12
    lx_float_t cos_angle = lx_vector_dot(normal_unit_01, normal_unit_12);

    // this curve is too curvy? divide to the more flat curve
    if (divided_count && lx_stroker_normals_too_curvy(cos_angle))
    {
        // chop the quad at half
        lx_point_t output[5];
        lx_quad_chop_at_half(points, output);

        // make sub-quad-to curves for the inner and outer contour
        lx_vector_t normal;
        lx_vector_t normal_unit;
        lx_stroker_make_quad_to(impl, output, normal_01, normal_unit_01, &normal, &normal_unit, divided_count - 1);
        lx_stroker_make_quad_to(impl, output + 2, &normal, &normal_unit, normal_12, normal_unit_12, divided_count - 1);
    }
    /* too sharp and short?
     *  .
     * . .
     */
    else if (!divided_count && lx_stroker_normals_too_curvy(cos_angle))
    {
        // check
        lx_assert(impl->path_other);

        // line-to it
        lx_stroker_make_line_to(impl, &points[1], normal_01);
        lx_stroker_make_line_to(impl, &points[2], normal_12);

        // patch one circle at the sharp join
        lx_path_add_circle2(impl->path_other, points[1].x, points[1].y, impl->radius, LX_ROTATE_DIRECTION_CW);
    }
    // for flat curve
    else
    {
        // check
        lx_assert(impl->path_inner && impl->path_outer);

        /* compute the approximate normal of the vector(p1, p1^)
         *
         *                      normal_1(p1, p1^)
         *                            p1^
         *                            .
         *                        .   .   .
         *                    .       .  .   .
         * normal_01      .          .. . R     .       normal_12
         *       \    .          .    p1   .       .   /
         *        .          .        .       .       .
         *       R .     .             .         .   .
         *           .                 .         p2 . R
         *         p0  .               .           .
         *               .             .          .
         *                 .           .         .
         *                   .          .       .
         *                     .        .      .
         *                       .      .     .
         *                         .    .    .
         *                           .   .  .
         *                             . . .
         *                               .. O
         *                              angle
         *
         *
         * (O, p1) ~= (O, p1^) if be flat curve
         *
         * normal_1(p1, p1^) ~= center(normal_01, normal_12)
         */
        lx_vector_t normal_1;
        lx_vector_make(&normal_1, normal_unit_01->x + normal_unit_12->x, normal_unit_01->y + normal_unit_12->y);

        /* compute the approximate length of the normal_1 and set it
         *
         * length(p1, p1^) ~= R / cos(angle/2) = R / sqrt((1 + cos(angle)) / 2)
         */
        if (!lx_vector_length_set(&normal_1, lx_div(impl->radius, lx_sqrt(lx_avg(LX_ONE, cos_angle)))))
        {
            // failed
            lx_assert(0);
            return ;
        }

        // quad-to the inner and outer contour
        lx_path_quad2_to(impl->path_outer, points[1].x + normal_1.x, points[1].y + normal_1.y, points[2].x + normal_12->x, points[2].y + normal_12->y);
        lx_path_quad2_to(impl->path_inner, points[1].x - normal_1.x, points[1].y - normal_1.y, points[2].x - normal_12->x, points[2].y - normal_12->y);
    }
}
static lx_void_t lx_stroker_make_cubic_to(lx_stroker_impl_t* impl, lx_point_ref_t points, lx_vector_ref_t normal_01, lx_vector_ref_t normal_unit_01, lx_vector_ref_t normal_23, lx_vector_ref_t normal_unit_23, lx_bool_t normal_23_is_valid, lx_size_t divided_count)
{
    // check
    lx_assert(impl && points && normal_01 && normal_unit_01 && normal_23 && normal_unit_23);

    // compute the normal and unit normal vectors of the vector(p1, p2)
    lx_vector_t normal_12;
    lx_vector_t normal_unit_12;
    if (!lx_stroker_normals_make(&points[1], &points[2], impl->radius, &normal_12, &normal_unit_12))
    {
        // quad-to it
        lx_point_t quad_points[3];
        quad_points[0] = points[0];
        quad_points[1] = points[1];
        quad_points[2] = points[3];
        lx_stroker_make_quad_to(impl, quad_points, normal_01, normal_unit_01, normal_23, normal_unit_23, divided_count);
        return ;
    }

    // compute the normal and unit normal vectors of the vector(p2, p3) if be not valid
    if (!normal_23_is_valid && !lx_stroker_normals_make(&points[2], &points[3], impl->radius, normal_23, normal_unit_23))
    {
        // quad-to it
        lx_stroker_make_quad_to(impl, points, normal_01, normal_unit_01, normal_23, normal_unit_23, divided_count);
        return ;
    }

    // the vector(p0, p1) is degenerate?
    lx_vector_t vector_01;
    lx_vector_make_from_two_points(&vector_01, &points[0], &points[1]);
    if (!lx_vector_can_normalize(&vector_01))
    {
        // quad-to it
        lx_stroker_make_quad_to(impl, &points[1], &normal_12, &normal_unit_12, normal_23, normal_unit_23, divided_count);
        return ;
    }

    // compute the cos(angle) of the normal_01 and normal_12
    lx_float_t cos_angle_012 = lx_vector_dot(normal_unit_01, &normal_unit_12);

    // compute the cos(angle) of the normal_12 and normal_23
    lx_float_t cos_angle_123 = lx_vector_dot(&normal_unit_12, normal_unit_23);

    // this curve is too curvy? divide to the more flat curve
    if (divided_count && (lx_stroker_normals_too_curvy(cos_angle_012) || lx_stroker_normals_too_curvy(cos_angle_123)))
    {
        // chop the cubic at half
        lx_point_t output[7];
        lx_cubic_chop_at_half(points, output);

        /* make sub-cubic-to curves for the inner and outer contour
         *
         * we already have a valid normal_23 and normal_unit_23, so uses it repeatly now.
         */
        lx_vector_t normal;
        lx_vector_t normal_unit;
        lx_stroker_make_cubic_to(impl, output, normal_01, normal_unit_01, &normal, &normal_unit, lx_false, divided_count - 1);
        lx_stroker_make_cubic_to(impl, output + 3, &normal, &normal_unit, normal_23, normal_unit_23, lx_true, divided_count - 1);
    }
    /* too sharp and short?
     *
     *  .
     * . . .
     *    .
     */
    else if (!divided_count && (lx_stroker_normals_too_curvy(cos_angle_012) || lx_stroker_normals_too_curvy(cos_angle_123)))
    {
        // check
        lx_assert(impl->path_other);

        // line-to it
        lx_stroker_make_line_to(impl, &points[1], normal_01);
        lx_stroker_make_line_to(impl, &points[2], &normal_12);
        lx_stroker_make_line_to(impl, &points[3], normal_23);

        // patch circles at the join is too sharp
        if (lx_stroker_normals_too_curvy(cos_angle_012)) lx_path_add_circle2(impl->path_other, points[1].x, points[1].y, impl->radius, LX_ROTATE_DIRECTION_CW);
        if (lx_stroker_normals_too_curvy(cos_angle_123)) lx_path_add_circle2(impl->path_other, points[2].x, points[2].y, impl->radius, LX_ROTATE_DIRECTION_CW);
    }
    // for flat curve
    else
    {
        // check
        lx_assert(impl->path_inner && impl->path_outer);

        /* compute the approximate normal of the vector(p1, p1^) and vector(p2, p2^)
         *
         *                      normal_1(p1, p1^)
         *                            p1^
         *                            .
         *                        .   .   .     normal_12
         *       normal_01    .       .  .   . /
         *              \ .          .. . R     .       normal_2(p2, p2^)
         *            .          .    p1   .       .   /
         *        .          .        .       .    p2^.
         *       R .     .             .         .   .  .
         *           .                 .         p2 . .R  .     normal_23
         *         p0  .               .           .  .     . /
         *               .             .          .     .     .
         *                 .           .         .        .     .
         *                   .          .       .           . R
         *                     .        .      .        .   p3
         *                       .      .     .      .
         *                         .    .    .    .
         *                           .   .  .  .
         *                             . . . .
         *                               .. O
         *                              angle
         *
         * angle_012 = angle(p0, O, p2)
         * angle_123 = angle(p1, O, p3)
         *
         * (O, p1) ~= (O, p1^) if be flat curve
         * (O, p2) ~= (O, p2^) if be flat curve
         *
         * normal_1(p1, p1^) ~= center(normal_01, normal_12)
         * normal_1(p2, p2^) ~= center(normal_12, normal_23)
         */
        lx_vector_t normal_1;
        lx_vector_t normal_2;
        lx_vector_make(&normal_1, normal_unit_01->x + normal_unit_12.x, normal_unit_01->y + normal_unit_12.y);
        lx_vector_make(&normal_2, normal_unit_12.x + normal_unit_23->x, normal_unit_12.y + normal_unit_23->y);

         /* compute the approximate length of the normal_1 and set it
         *
         * length(p1, p1^) ~= R / cos(angle/2) = R / sqrt((1 + cos(angle)) / 2)
         */
        if (!lx_vector_length_set(&normal_1, lx_div(impl->radius, lx_sqrt(lx_avg(LX_ONE, cos_angle_012)))))
        {
            // failed
            lx_assert(0);
            return ;
        }

        /* compute the approximate length of the normal_2 and set it
         *
         * length(p1, p1^) ~= R / cos(angle/2) = R / sqrt((1 + cos(angle)) / 2)
         */
        if (!lx_vector_length_set(&normal_2, lx_div(impl->radius, lx_sqrt(lx_avg(LX_ONE, cos_angle_123)))))
        {
            // failed
            lx_assert(0);
            return ;
        }

        // cubic-to the inner and outer contour
        lx_path_cubic2_to(impl->path_outer, points[1].x + normal_1.x, points[1].y + normal_1.y, points[2].x + normal_2.x, points[2].y + normal_2.y, points[3].x + normal_23->x, points[3].y + normal_23->y);
        lx_path_cubic2_to(impl->path_inner, points[1].x - normal_1.x, points[1].y - normal_1.y, points[2].x - normal_2.x, points[2].y - normal_2.y, points[3].x - normal_23->x, points[3].y - normal_23->y);
    }
}
static lx_bool_t lx_stroker_enter_to(lx_stroker_impl_t* impl, lx_point_ref_t point, lx_vector_ref_t normal, lx_vector_ref_t normal_unit, lx_bool_t is_line_to)
{
    // check
    lx_assert(impl && point && normal && normal_unit);
    lx_assert(impl->segment_count >= 0);

    // the radius
    lx_float_t radius = impl->radius;
    lx_assert_and_check_return_val(radius > 0, lx_false);

    // compute the normal and unit normal vectors
    if (!lx_stroker_normals_make(&impl->point_prev, point, radius, normal, normal_unit))
    {
        lx_trace_i("%{point} %{point}", &impl->point_prev, point);
        // failed
        lx_assert(0);
        return lx_false;
    }

    // body?
    if (impl->segment_count > 0)
    {
        // check
        lx_assert(impl->joiner);

        // join it
        impl->joiner(impl->path_inner, impl->path_outer, &impl->point_prev, radius, &impl->normal_unit_prev, normal_unit, impl->miter_invert, impl->is_line_to_prev, is_line_to);
    }
    // start?
    else
    {
        // save the first point of the outer contour
        lx_point_make(&impl->outer_first, impl->point_prev.x + normal->x, impl->point_prev.y + normal->y);

        // save the first normal
        impl->normal_first = *normal;

        // save the first unit normal
        impl->normal_unit_first = *normal_unit;

        // save the first line-to state?
        impl->is_line_to_first = is_line_to;

        // move to the start point for the inner and outer path
        lx_path_move_to(impl->path_outer, &impl->outer_first);
        lx_path_move2_to(impl->path_inner, impl->point_prev.x - normal->x, impl->point_prev.y - normal->y);
    }

    // update the previous line-to state?
    impl->is_line_to_prev = is_line_to;

    // ok
    return lx_true;
}
static lx_void_t lx_stroker_leave_to(lx_stroker_impl_t* impl, lx_point_ref_t point, lx_vector_ref_t normal, lx_vector_ref_t normal_unit)
{
    // check
    lx_assert(impl && point && normal && normal_unit);

    // update the previous point
    impl->point_prev = *point;

    // update the previous normal
    impl->normal_prev = *normal;

    // update the previous unit normal
    impl->normal_unit_prev = *normal_unit;

    // update the segment count
    impl->segment_count++;
}
static lx_void_t lx_stroker_finish(lx_stroker_impl_t* impl, lx_bool_t closed)
{
    // check
    lx_assert(impl && impl->path_inner && impl->path_outer);
    lx_assert(impl->capper && impl->joiner);

    // exists contour now?
    if (impl->segment_count > 0)
    {
        // closed?
        if (closed)
        {
            // join it
            impl->joiner(impl->path_inner, impl->path_outer, &impl->point_prev, impl->radius, &impl->normal_unit_prev, &impl->normal_unit_first, impl->miter_invert, impl->is_line_to_prev, impl->is_line_to_first);

            // close the outer contour
            lx_path_clos(impl->path_outer);

            /* add the inner contour in reverse order to the outer path
             *
             *              -->
             * . . . . . . . . .
             * .               .
             * .   . . . . .   .
             * .   .       .   .
             * .   .       .   .
             * .   . inner .   .
             * .   . . . . x   .
             * .               .
             * . . . . . . . . x outer
             * <--
             */
            lx_point_t inner_last;
            lx_path_last(impl->path_inner, &inner_last);
            lx_path_move_to(impl->path_outer, &inner_last);
            lx_path_rpath_to(impl->path_outer, impl->path_inner);
            lx_path_clos(impl->path_outer);

        }
        /* add caps to the start and end point
         *
         *                    start cap
         *             ------------------------>
         *
         *                       normal
         *              ----------------------> first outer
         *             |  radius   |           |
         *             |           |           |
         *             |           |           |
         *             |           |           |
         *             |           |           |
         * reverse add |           |           |
         *             |           |           |
         *             |           |           |
         *             |           |           |
         *             |           |           |
         *             |           |           |
         * last inner \|/         \|/         \|/
         *          inner        center       outer
         *
         *             <------------------------
         *                     end cap
         */
        else
        {
            // cap the end point
            lx_point_t inner_last;
            lx_path_last(impl->path_inner, &inner_last);
            impl->capper(impl->path_outer, &impl->point_prev, &inner_last, &impl->normal_prev, impl->is_line_to_prev);

            // add the inner contour in reverse order to the outer path
            lx_path_rpath_to(impl->path_outer, impl->path_inner);

            // cap the start point
            lx_vector_t normal_first;
            lx_vector_negate2(&impl->normal_first, &normal_first);
            impl->capper(impl->path_outer, &impl->point_first, &impl->outer_first, &normal_first, impl->is_line_to_first);

            // close the outer contour
            lx_path_clos(impl->path_outer);
        }
    }

    // finish it
    impl->segment_count = -1;

    // clear the inner path for reusing it
    lx_path_clear(impl->path_inner);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_stroker_ref_t lx_stroker_init()
{
    // done
    lx_bool_t           ok = lx_false;
    lx_stroker_impl_t*  impl = lx_null;
    do
    {
        // make stroker
        impl = lx_malloc0_type(lx_stroker_impl_t);
        lx_assert_and_check_break(impl);

        // init stroker
        impl->cap               = LX_PAINT_STROKE_CAP_BUTT;
        impl->join              = LX_PAINT_STROKE_JOIN_MITER;
        impl->miter             = LX_STROKER_DEFAULT_MITER;
        impl->radius            = 0;
        impl->segment_count     = -1;
        impl->capper            = lx_stroker_capper_butt;
        impl->joiner            = lx_stroker_joiner_miter;
        impl->miter_invert      = lx_invert(LX_STROKER_DEFAULT_MITER);
        impl->is_line_to_prev   = lx_false;
        impl->is_line_to_first  = lx_false;

        // init the outer path
        impl->path_outer = lx_path_init();
        lx_assert_and_check_break(impl->path_outer);

        // init the inner path
        impl->path_inner = lx_path_init();
        lx_assert_and_check_break(impl->path_inner);

        // init the other path
        impl->path_other = lx_path_init();
        lx_assert_and_check_break(impl->path_other);

        // ok
        ok = lx_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) lx_stroker_exit((lx_stroker_ref_t)impl);
        impl = lx_null;
    }

    // ok?
    return (lx_stroker_ref_t)impl;
}
lx_void_t lx_stroker_exit(lx_stroker_ref_t stroker)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl);

    // exit the other path
    if (impl->path_other) lx_path_exit(impl->path_other);
    impl->path_other = lx_null;

    // exit the inner path
    if (impl->path_inner) lx_path_exit(impl->path_inner);
    impl->path_inner = lx_null;

    // exit the outer path
    if (impl->path_outer) lx_path_exit(impl->path_outer);
    impl->path_outer = lx_null;

    // exit it
    lx_free(impl);
}
lx_void_t lx_stroker_clear(lx_stroker_ref_t stroker)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl);

    // clear it
    impl->cap               = LX_PAINT_STROKE_CAP_BUTT;
    impl->join              = LX_PAINT_STROKE_JOIN_MITER;
    impl->miter             = LX_STROKER_DEFAULT_MITER;
    impl->radius            = 0;
    impl->segment_count     = -1;
    impl->capper            = lx_stroker_capper_butt;
    impl->joiner            = lx_stroker_joiner_miter;
    impl->miter_invert      = lx_invert(LX_STROKER_DEFAULT_MITER);
    impl->is_line_to_prev   = lx_false;
    impl->is_line_to_first  = lx_false;

    // clear the other path
    if (impl->path_other) lx_path_clear(impl->path_other);

    // clear the inner path
    if (impl->path_inner) lx_path_clear(impl->path_inner);

    // clear the outer path
    if (impl->path_outer) lx_path_clear(impl->path_outer);
}
lx_void_t lx_stroker_apply_paint(lx_stroker_ref_t stroker, lx_paint_ref_t paint)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && paint);

    // the width
    lx_float_t width = lx_paint_stroke_width(paint);
    lx_assert(width >= 0);

    // the miter
    lx_float_t miter = lx_paint_stroke_miter(paint);

    // set the cap
    impl->cap = lx_paint_stroke_cap(paint);

    // set the join
    impl->join = lx_paint_stroke_join(paint);

    // set the radius
    impl->radius = lx_half(width);

    // set the invert miter limit
    if (impl->miter != miter)
    {
        impl->miter_invert = 0;
        if (impl->join == LX_PAINT_STROKE_JOIN_MITER)
        {
            if (miter >= LX_ONE) impl->join = LX_PAINT_STROKE_JOIN_BEVEL;
            else impl->miter_invert = lx_invert(miter);
        }
    }

    // set the miter limit
    impl->miter = miter;

    // the cappers
    static lx_stroker_capper_t s_cappers[] =
    {
        lx_stroker_capper_butt
    ,   lx_stroker_capper_round
    ,   lx_stroker_capper_square
    };
    lx_assert(impl->cap < lx_arrayn(s_cappers));

    // the joiners
    static lx_stroker_joiner_t s_joiners[] =
    {
        lx_stroker_joiner_miter
    ,   lx_stroker_joiner_round
    ,   lx_stroker_joiner_bevel
    };
    lx_assert(impl->join < lx_arrayn(s_joiners));

    // set capper
    impl->capper = s_cappers[impl->cap];

    // set joiner
    impl->joiner = s_joiners[impl->join];
}
lx_void_t lx_stroker_clos(lx_stroker_ref_t stroker)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl);

    // close this contour
    lx_stroker_finish(impl, lx_true);
}
lx_void_t lx_stroker_move_to(lx_stroker_ref_t stroker, lx_point_ref_t point)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && impl->path_inner && impl->path_outer && point);

    // finish the current contour first
    if (impl->segment_count > 0) lx_stroker_finish(impl, lx_false);

    // start a new contour
    impl->segment_count = 0;

    // save the first point
    impl->point_first = *point;

    // save the previous point
    impl->point_prev = *point;
}
lx_void_t lx_stroker_line_to(lx_stroker_ref_t stroker, lx_point_ref_t point)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && point);

    // only be a point?
    if (lx_point_near_eq(&impl->point_prev, point)) return ;

    // enter-to
    lx_vector_t normal;
    lx_vector_t normal_unit;
    if (!lx_stroker_enter_to(impl, point, &normal, &normal_unit, lx_true)) return ;

    // make line-to for the inner and outer contour
    lx_stroker_make_line_to(impl, point, &normal);

    // leave-to
    lx_stroker_leave_to(impl, point, &normal, &normal_unit);
}
lx_void_t lx_stroker_quad_to(lx_stroker_ref_t stroker, lx_point_ref_t ctrl, lx_point_ref_t point)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && ctrl && point);

    // is point for p0 => p1 and p1 => p2?
    lx_bool_t is_point_for_01 = lx_point_near_eq(&impl->point_prev, ctrl);
    lx_bool_t is_point_for_12 = lx_point_near_eq(ctrl, point);

    // only be line?
    if (is_point_for_01 | is_point_for_12)
    {
        // line-to it
        lx_stroker_line_to(stroker, point);
        return ;
    }

    // enter-to
    lx_vector_t normal_01;
    lx_vector_t normal_12;
    lx_vector_t normal_unit_01;
    lx_vector_t normal_unit_12;
    if (!lx_stroker_enter_to(impl, ctrl, &normal_01, &normal_unit_01, lx_false)) return ;

    // init points
    lx_point_t points[3];
    points[0] = impl->point_prev;
    points[1] = *ctrl;
    points[2] = *point;

#if 0
    // attempt to chop the quadratic curve at the max curvature
    lx_point_t output[5];
    if (lx_quad_chop_at_max_curvature(points, output) == 2)
    {
        // make the unit normal of (p1, p2)
        if (lx_stroker_normals_make(&points[1], &points[2], 0, lx_null, &normal_unit_12))
        {
            /* too sharp? make the approximate curve using lines
             *
             *      .
             *     . .
             *     . .
             *     . .
             *     . .
             *     . .
             *     . .
             *     . .
             */
            if (lx_stroker_normals_too_sharp(&normal_unit_01, &normal_unit_12))
            {
                // check
                lx_assert(impl->path_inner && impl->path_outer && impl->path_other && impl->radius > 0);

                // compute the normal of (p1, p2)
                lx_vector_scale2(&normal_unit_12, &normal_12, impl->radius);

                // make the approximate curve for the outer contour using lines directly
                lx_path_line2_to(impl->path_outer, output[2].x + normal_01.x, output[2].y + normal_01.y);
                lx_path_line2_to(impl->path_outer, output[2].x + normal_12.x, output[2].y + normal_12.y);
                lx_path_line2_to(impl->path_outer, output[4].x + normal_12.x, output[4].y + normal_12.y);

                // make the approximate curve for the inner contour using lines directly
                lx_path_line2_to(impl->path_inner, output[2].x - normal_01.x, output[2].y - normal_01.y);
                lx_path_line2_to(impl->path_inner, output[2].x - normal_12.x, output[2].y - normal_12.y);
                lx_path_line2_to(impl->path_inner, output[4].x - normal_12.x, output[4].y - normal_12.y);

                // patch one circle for making the join of two sub-curves more like curve
                lx_path_add_circle2(impl->path_other, output[2].x, output[2].y, impl->radius, LX_ROTATE_DIRECTION_CW);
            }
            else
            {
                // make more flat quad-to curves for the first sub-curve
                lx_stroker_make_quad_to(impl, &output[0], &normal_01, &normal_unit_01, &normal_12, &normal_unit_12, LX_QUAD_DIVIDED_MAXN);

                // make more flat quad-to curves for the second sub-curve
                lx_vector_t normal2_01 = normal_12;
                lx_vector_t normal2_unit_01 = normal_unit_12;
                lx_stroker_make_quad_to(impl, &output[2], &normal2_01, &normal2_unit_01, &normal_12, &normal_unit_12, LX_QUAD_DIVIDED_MAXN);
            }
        }
        else
        {
            // trace
            lx_trace_e("failed to make unit normal for %{point} => %{point}", &points[1], &points[2]);

            // failed
            lx_assert(0);
        }
    }
    // only one curve?
    else
    {
        // make more flat quad-to curves for the whole curve
        lx_stroker_make_quad_to(impl, points, &normal_01, &normal_unit_01, &normal_12, &normal_unit_12, LX_QUAD_DIVIDED_MAXN);
    }
#else
    // make more flat quad-to curves for the whole curve
    lx_stroker_make_quad_to(impl, points, &normal_01, &normal_unit_01, &normal_12, &normal_unit_12, LX_QUAD_DIVIDED_MAXN);
#endif

    // leave-to
    lx_stroker_leave_to(impl, point, &normal_12, &normal_unit_12);
}
lx_void_t lx_stroker_cubic_to(lx_stroker_ref_t stroker, lx_point_ref_t ctrl0, lx_point_ref_t ctrl1, lx_point_ref_t point)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && ctrl0 && ctrl1 && point);

    // is point for p0 => p1 and p1 => p2 and p2 = > p3?
    lx_bool_t is_point_for_01 = lx_point_near_eq(&impl->point_prev, ctrl0);
    lx_bool_t is_point_for_12 = lx_point_near_eq(ctrl0, ctrl1);
    lx_bool_t is_point_for_23 = lx_point_near_eq(ctrl1, point);

    // only be quad?
    if (is_point_for_01 | is_point_for_12 | is_point_for_23)
    {
        // quad-to it
        lx_stroker_quad_to(stroker, is_point_for_01? ctrl1 : ctrl0, point);

        // only point
        return ;
    }

    // enter-to
    lx_vector_t normal_01;
    lx_vector_t normal_23;
    lx_vector_t normal_unit_01;
    lx_vector_t normal_unit_23;
    if (!lx_stroker_enter_to(impl, ctrl0, &normal_01, &normal_unit_01, lx_false)) return ;

    // init points
    lx_point_t points[4];
    points[0] = impl->point_prev;
    points[1] = *ctrl0;
    points[2] = *ctrl1;
    points[3] = *point;

#if 0
    // chop the cubic curve at the max curvature
    lx_point_t  output[13];
    lx_size_t   count = lx_cubic_chop_at_max_curvature(points, output);
    lx_assert(count);

    // make every cubic sub-curves
    lx_size_t   index;
    lx_vector_t normal2_01 = normal_01;
    lx_vector_t normal2_unit_01 = normal_unit_01;
    for (index = 0; index < count; index++)
    {
        // make more flat cubic-to curves for the sub-curve
        lx_stroker_make_cubic_to(impl, &output[(index << 1) + index], &normal2_01, &normal2_unit_01, &normal_23, &normal_unit_23, lx_false, LX_CUBIC_DIVIDED_MAXN);

        // end?
        lx_check_break(index != count - 1);

        // update the start normal
        normal2_01 = normal_23;
        normal2_unit_01 = normal_unit_23;
    }
#else
    // make more flat cubic-to curves for the whole curve
    lx_stroker_make_cubic_to(impl, points, &normal_01, &normal_unit_01, &normal_23, &normal_unit_23, lx_false, LX_CUBIC_DIVIDED_MAXN);
#endif

    // leave-to
    lx_stroker_leave_to(impl, point, &normal_23, &normal_unit_23);
}
lx_void_t lx_stroker_add_path(lx_stroker_ref_t stroker, lx_path_ref_t path)
{
    // done
    lx_for_all_if (lx_path_item_ref_t, item, path, item)
    {
        switch (item->code)
        {
        case LX_PATH_CODE_MOVE:
            lx_stroker_move_to(stroker, &item->points[0]);
            break;
        case LX_PATH_CODE_LINE:
            lx_stroker_line_to(stroker, &item->points[1]);
            break;
        case LX_PATH_CODE_QUAD:
            lx_stroker_quad_to(stroker, &item->points[1], &item->points[2]);
            break;
        case LX_PATH_CODE_CUBIC:
            lx_stroker_cubic_to(stroker, &item->points[1], &item->points[2], &item->points[3]);
            break;
        case LX_PATH_CODE_CLOS:
            lx_stroker_clos(stroker);
            break;
        default:
            // trace
            lx_trace_e("invalid code: %lu", item->code);
            break;
        }
    }
}
lx_void_t lx_stroker_add_rect(lx_stroker_ref_t stroker, lx_rect_ref_t rect)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && impl->path_other && rect);

    // the radius
    lx_float_t radius = impl->radius;
    lx_check_return(radius > 0);

    // the width
    lx_float_t width = lx_lsh(radius, 1);

    // init the inner rect
    lx_rect_t rect_inner = *rect;
    if (rect_inner.w > width && rect_inner.h > width)
    {
        // make the inner rect
        lx_rect_deflate(&rect_inner, radius, radius);

        // add the inner rect to the other path
        lx_path_add_rect(impl->path_other, &rect_inner, LX_ROTATE_DIRECTION_CW);
    }

    // init the outer rect
    lx_rect_t rect_outer = *rect;

    // make the outer rect
    lx_rect_inflate(&rect_outer, radius, radius);

    // the join
    lx_size_t join = impl->join;
    if (join == LX_PAINT_STROKE_JOIN_MITER && impl->miter < LX_SQRT2)
        join = LX_PAINT_STROKE_JOIN_BEVEL;

    /* add the outer rect to the other path
     *
     * ------------------------------ miter join
     *                    .        . |
     *                      .    L   |
     *            bevel join  .      |
     *                        | .    |
     *                        |   .  |
     * -----------------|     |      |
     *                  |        R   |
     *                  |            |
     *                  |            |
     *                  |            |
     *                  |            |
     *                  | W = R * 2  |
     *
     * W: width
     * R: radius
     * miter_limit = L / R > 1
     */
    switch (join)
    {
    case LX_PAINT_STROKE_JOIN_MITER:
        {
            // add miter rect
            lx_path_add_rect(impl->path_other, &rect_outer, LX_ROTATE_DIRECTION_CCW);
        }
        break;
    case LX_PAINT_STROKE_JOIN_BEVEL:
        {
            // the bounds
            lx_float_t x = rect_outer.x;
            lx_float_t y = rect_outer.y;
            lx_float_t w = rect_outer.w;
            lx_float_t h = rect_outer.h;

            // add bevel rect by counter-clockwise
            lx_path_move2_to(impl->path_other, x,               y + radius);
            lx_path_line2_to(impl->path_other, x,               y + h - radius);
            lx_path_line2_to(impl->path_other, x + radius,      y + h);
            lx_path_line2_to(impl->path_other, x + w - radius,  y + h);
            lx_path_line2_to(impl->path_other, x + w,           y + h - radius);
            lx_path_line2_to(impl->path_other, x + w,           y + radius);
            lx_path_line2_to(impl->path_other, x + w - radius,  y);
            lx_path_line2_to(impl->path_other, x + radius,      y);
            lx_path_clos(impl->path_other);
        }
        break;
    case LX_PAINT_STROKE_JOIN_ROUND:
        {
            // add round rect
            lx_path_add_round_rect2(impl->path_other, &rect_outer, radius, radius, LX_ROTATE_DIRECTION_CCW);
        }
        break;
    default:
        lx_trace_e("unknown join: %lu", impl->join);
        break;
    }
}
lx_void_t lx_stroker_add_circle(lx_stroker_ref_t stroker, lx_circle_ref_t circle)
{
    // check
    lx_assert_and_check_return(circle);

    // make ellipse
    lx_ellipse_t ellipse;
    lx_ellipse_make(&ellipse, circle->c.x, circle->c.y, circle->r, circle->r);

    // add ellipse
    lx_stroker_add_ellipse(stroker, &ellipse);
}
lx_void_t lx_stroker_add_ellipse(lx_stroker_ref_t stroker, lx_ellipse_ref_t ellipse)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && impl->path_other && ellipse);

    // the radius
    lx_float_t radius = impl->radius;
    lx_check_return(radius > 0);

    // init the inner ellipse
    lx_ellipse_t ellipse_inner = *ellipse;
    if (ellipse_inner.rx > radius && ellipse_inner.ry > radius)
    {
        // make the inner ellipse
        ellipse_inner.rx -= radius;
        ellipse_inner.ry -= radius;

        // add the inner ellipse to the other path
        lx_path_add_ellipse(impl->path_other, &ellipse_inner, LX_ROTATE_DIRECTION_CW);
    }

    // init the outer ellipse
    lx_ellipse_t ellipse_outer = *ellipse;

    // make the outer ellipse
    ellipse_outer.rx += radius;
    ellipse_outer.ry += radius;

    // add the inner and outer ellipse to the other path
    lx_path_add_ellipse(impl->path_other, &ellipse_outer, LX_ROTATE_DIRECTION_CCW);
}
lx_void_t lx_stroker_add_lines(lx_stroker_ref_t stroker, lx_point_ref_t points, lx_size_t count)
{
    // check
    lx_assert_and_check_return(points && count && !(count & 0x1));

    // done
    lx_size_t index;
    for (index = 0; index < count; index += 2)
    {
        lx_stroker_move_to(stroker, points + index);
        lx_stroker_line_to(stroker, points + index + 1);
    }
}
lx_void_t lx_stroker_add_points(lx_stroker_ref_t stroker, lx_point_ref_t points, lx_size_t count)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return(impl && impl->path_other && points && count);

    // the radius
    lx_float_t radius = impl->radius;
    lx_check_return(radius > 0);

    // make the stroked path
    switch (impl->cap)
    {
    case LX_PAINT_STROKE_CAP_ROUND:
        {
            // done
            lx_size_t       index;
            lx_point_ref_t  point;
            lx_circle_t     circle;
            for (index = 0; index < count; index++)
            {
                // the point
                point = points + index;

                // make circle
                lx_circle_make(&circle, point->x, point->y, radius);

                // add circle to the other path
                lx_path_add_circle(impl->path_other, &circle, LX_ROTATE_DIRECTION_CW);
            }
        }
        break;
    case LX_PAINT_STROKE_CAP_BUTT:
    case LX_PAINT_STROKE_CAP_SQUARE:
    default:
        {
            // done
            lx_rect_t       rect;
            lx_size_t       index;
            lx_point_ref_t  point;
            lx_float_t      width = lx_lsh(radius, 1);
            for (index = 0; index < count; index++)
            {
                // the point
                point = points + index;

                // make rect
                lx_rect_make(&rect, point->x - radius, point->y - radius, width, width);

                // add rect to the other path
                lx_path_add_rect(impl->path_other, &rect, LX_ROTATE_DIRECTION_CW);
            }
        }
        break;
    }
}
lx_void_t lx_stroker_add_polygon(lx_stroker_ref_t stroker, lx_polygon_ref_t polygon)
{
    // check
    lx_assert_and_check_return(polygon && polygon->points && polygon->counts);

    // done
    lx_point_ref_t  first = lx_null;
    lx_point_ref_t  point = lx_null;
    lx_point_ref_t  points = polygon->points;
    lx_uint16_t*    counts = polygon->counts;
    lx_uint16_t     count = *counts++;
    lx_size_t       index = 0;
    while (index < count)
    {
        // the point
        point = points++;

        // first point?
        if (!index)
        {
            lx_stroker_move_to(stroker, point);
            first = point;
        }
        else lx_stroker_line_to(stroker, point);

        // next point
        index++;

        // next polygon
        if (index == count)
        {
            // close path
            if (first && first->x == point->x && first->y == point->y) lx_stroker_clos(stroker);

            // next
            count = *counts++;
            index = 0;
        }
    }
}
lx_path_ref_t lx_stroker_done(lx_stroker_ref_t stroker, lx_bool_t convex)
{
    // check
    lx_stroker_impl_t* impl = (lx_stroker_impl_t*)stroker;
    lx_assert_and_check_return_val(impl, lx_null);

    // finish the current contour first
    if (impl->segment_count > 0) lx_stroker_finish(impl, lx_false);

    // exists the other path? merge it
    if (impl->path_other && !lx_path_null(impl->path_other))
    {
        // add the other path
        lx_path_add_path(impl->path_outer, impl->path_other);

        // clear the other path
        lx_path_clear(impl->path_other);
    }

    // update convex and disable to analyze convex in the path automatically
    lx_path_convex_set(impl->path_outer, convex);

    // the stroked path
    return impl->path_outer;
}
lx_path_ref_t lx_stroker_done_path(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_path_ref_t path)
{
    // clear the stroker
    lx_stroker_clear(stroker);

    // apply paint to the stroker
    lx_stroker_apply_paint(stroker, paint);

    // attempt to add hint first
    lx_bool_t convex = lx_false;
    if (!lx_stroker_add_hint(stroker, lx_path_hint(path), &convex))
    {
        // add path to the stroker
        lx_stroker_add_path(stroker, path);
    }

    // done the stroker
    return lx_stroker_done(stroker, convex);
}
lx_path_ref_t lx_stroker_done_lines(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_point_ref_t points, lx_size_t count)
{
    // clear the stroker
    lx_stroker_clear(stroker);

    // apply paint to the stroker
    lx_stroker_apply_paint(stroker, paint);

    // add lines to the stroker
    lx_stroker_add_lines(stroker, points, count);

    // done the stroker
    return lx_stroker_done(stroker, lx_true);
}
lx_path_ref_t lx_stroker_done_points(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_point_ref_t points, lx_size_t count)
{
    // clear the stroker
    lx_stroker_clear(stroker);

    // apply paint to the stroker
    lx_stroker_apply_paint(stroker, paint);

    // add points to the stroker
    lx_stroker_add_points(stroker, points, count);

    // done the stroker
    return lx_stroker_done(stroker, lx_true);
}
lx_path_ref_t lx_stroker_done_polygon(lx_stroker_ref_t stroker, lx_paint_ref_t paint, lx_polygon_ref_t polygon, lx_shape_ref_t hint)
{
    // clear the stroker
    lx_stroker_clear(stroker);

    // apply paint to the stroker
    lx_stroker_apply_paint(stroker, paint);

    // attempt to add hint first
    lx_bool_t convex = lx_false;
    if (!lx_stroker_add_hint(stroker, hint, &convex))
    {
        // add polygon to the stroker
        lx_stroker_add_polygon(stroker, polygon);
    }

    // done the stroker
    return lx_stroker_done(stroker, convex);
}
