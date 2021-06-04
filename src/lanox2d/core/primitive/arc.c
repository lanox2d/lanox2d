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
 * @file        arc.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "arc.h"
#include "point.h"
#include "vector.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the quad points of the clockwise unit circle
static lx_point_t g_quad_points_of_unit_circle[] = {
#if 1
    // move-to
    { 1.0f,             0              }

    // part1
,   { 1.0f,             LX_TAN_PIOVER8 }
,   { LX_SQRT2_OVER2,   LX_SQRT2_OVER2 }
,   { LX_TAN_PIOVER8,   1.0f           }
,   { 0,                1.0f           }

    // part2
,   { -LX_TAN_PIOVER8,  1.0f           }
,   { -LX_SQRT2_OVER2,  LX_SQRT2_OVER2 }
,   { -1.0f,            LX_TAN_PIOVER8 }
,   { -1.0f,            0              }

    // part3
,   { -1.0f,           -LX_TAN_PIOVER8 }
,   { -LX_SQRT2_OVER2, -LX_SQRT2_OVER2 }
,   { -LX_TAN_PIOVER8, -1.0f           }
,   { 0,               -1.0f           }

    // part4
,   { LX_TAN_PIOVER8,  -1.0f           }
,   { LX_SQRT2_OVER2,  -LX_SQRT2_OVER2 }
,   { 1.0f,            -LX_TAN_PIOVER8 }
,   { 1.0f,            0               }

#else // counter-clockwise
    // move-to
    { 1.0f,             0              }

    // part1
,   { 1.0f,            -LX_TAN_PIOVER8 }
,   { LX_SQRT2_OVER2,  -LX_SQRT2_OVER2 }
,   { LX_TAN_PIOVER8,  -1.0f           }
,   { 0,               -1.0f           }

    // part2
,   { -LX_TAN_PIOVER8, -1.0f           }
,   { -LX_SQRT2_OVER2, -LX_SQRT2_OVER2 }
,   { -1.0f,           -LX_TAN_PIOVER8 }
,   { -1.0f,           0               }

    // part3
,   { -1.0f,           LX_TAN_PIOVER8  }
,   { -LX_SQRT2_OVER2, LX_SQRT2_OVER2  }
,   { -LX_TAN_PIOVER8, 1.0f            }
,   { 0,               1.0f            }

    // part4
,   { LX_TAN_PIOVER8,  1.0f            }
,   { LX_SQRT2_OVER2,  LX_SQRT2_OVER2  }
,   { 1.0f,            LX_TAN_PIOVER8  }
,   { 1.0f,            0               }
#endif
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_arc_make(lx_arc_ref_t arc, lx_float_t x0, lx_float_t y0, lx_float_t rx, lx_float_t ry, lx_float_t ab, lx_float_t an) {
    arc->c.x    = x0;
    arc->c.y    = y0;
    arc->rx     = rx;
    arc->ry     = ry;
    arc->ab     = ab;
    arc->an     = an;
}

lx_void_t lx_arc_imake(lx_arc_ref_t arc, lx_long_t x0, lx_long_t y0, lx_size_t rx, lx_size_t ry, lx_long_t ab, lx_long_t an) {
    lx_arc_make(arc, (lx_float_t)x0, (lx_float_t)y0, (lx_float_t)rx, (lx_float_t)ry, (lx_float_t)ab, (lx_float_t)an);
}

lx_void_t lx_arc_make_quad(lx_arc_ref_t arc, lx_arc_quad_cb_t callback, lx_cpointer_t udata) {
    // check
    lx_assert_and_check_return(arc && callback);

    // the degenerated arc
    if (lx_near0(arc->rx) && lx_near0(arc->ry)) {
        lx_point_t point;
        lx_point_make(&point, arc->c.x, arc->c.y);
        callback(lx_null, &point, udata);
        return ;
    }

    // the start and stop unit vector for arc
    lx_vector_t start;
    lx_vector_t stop;
    lx_sincosf(lx_degree_to_radian(arc->ab), &start.y, &start.x);
    lx_sincosf(lx_degree_to_radian(arc->ab + arc->an), &stop.y, &stop.x);

    // init matrix
    lx_matrix_t matrix;
    lx_matrix_init_scale(&matrix, arc->rx, arc->ry);
    lx_matrix_translate_lhs(&matrix, arc->c.x, arc->c.y);

    /* make quad curves
     *
     * arc = matrix * unit_arc
     */
    lx_arc_make_quad2(&start, &stop, &matrix, (arc->an > 0)? LX_ROTATE_DIRECTION_CW : LX_ROTATE_DIRECTION_CCW, callback, udata);
}

lx_void_t lx_arc_make_quad2(lx_vector_ref_t start, lx_vector_ref_t stop, lx_matrix_ref_t matrix, lx_size_t direction, lx_arc_quad_cb_t callback, lx_cpointer_t udata) {
    // check
    lx_assert(start && stop && callback);

    // the cos and sin value of the sweep angle
    lx_float_t sweep_x = lx_vector_dot(start, stop);
    lx_float_t sweep_y = lx_vector_cross(start, stop);

    // the absolute value of the sweep x and y
    lx_float_t sweep_abs_x = lx_abs(sweep_x);
    lx_float_t sweep_abs_y = lx_abs(sweep_y);

    // the points and count
    lx_size_t   count = 0;
    lx_point_t  points[lx_arrayn(g_quad_points_of_unit_circle)];

    // the sweep angle is nearly zero? only one start point
    if (    sweep_abs_y <= LX_NEAR0 && sweep_x > 0
        &&  (   (sweep_y >= 0 && direction == LX_ROTATE_DIRECTION_CW)
            ||  (sweep_y <= 0 && direction == LX_ROTATE_DIRECTION_CCW))) {
        lx_point_make(&points[count++], 1.0f, 0);
    } else {
        // counter-clockwise? reverse to the clockwise direction
        if (direction == LX_ROTATE_DIRECTION_CCW) sweep_y = -sweep_y;

        // init the point count for the move-to point first
        count++;

        /* compute the point count of the quadratic curve
         *
         *   .             .             .
         *     .           .           .
         *       .         .         .
         *         .       .       .
         *           .     .     .             sweep_y < 0
         *             .   .   .
         *               . . .
         *   . . . . . . . . . . . . . . .
         *               . . .                 |
         *             .   .   .               | direction: clockwise
         *           .     .     .            \|/
         *         .       .       .
         *       .         .         .
         *     .           .           .
         *   .             .             .     sweep_y > 0
         *
         */
        if (sweep_abs_y <= LX_NEAR0) {
            // cos(sweep_angle) must be -1
            lx_assert(lx_near0(sweep_x + 1.0f));
            // 180 degrees
            count += 8;
        } else if (sweep_abs_x <= LX_NEAR0) {
            // sin(sweep_angle) must be 1 or -1
            lx_assert(sweep_abs_y - 1.0f <= LX_NEAR0);
            // 90 or 270 degrees
            count += sweep_y > 0 ? 4 : 12;
        } else {
            // > 180 degrees
            if (sweep_y < 0) count += 8;

            // > 90 or 270 degrees
            lx_long_t same = 1;
            if ((sweep_x < 0) != (sweep_y < 0)) {
                count += 4;
                same = 0;
            }

            // > 45 or 135 or 225 or 315 degrees
            if ((sweep_abs_x < sweep_abs_y) == same)
                count += 2;
        }

        // check
        lx_assert((count & 0x1) && count <= lx_arrayn(g_quad_points_of_unit_circle));

        // make points
        lx_memcpy(points, g_quad_points_of_unit_circle, count * sizeof(lx_point_t));

        // patch the last quadratic curve
        if (    sweep_abs_x <= LX_NEAR0
            &&  sweep_abs_y <= LX_NEAR0
            &&  sweep_abs_x != LX_SQRT2_OVER2
            &&  sweep_abs_y != LX_SQRT2_OVER2) {

            // the patched start vector
            lx_vector_t patched_start;
            lx_vector_make_from_point(&patched_start, &points[count - 1]);

            // the patched stop vector
            lx_vector_t patched_stop;
            lx_vector_make(&patched_stop, sweep_x, sweep_y);

            // init the applied matrix
            lx_matrix_t applied_matrix;
            lx_matrix_init_sincos(&applied_matrix, patched_start.y, patched_start.x);

            /* compute the tan(a/2)
             *
             * tan(a/2) = sin(a) / (1 + cos(a))
             */
            lx_float_t cos_a = lx_vector_dot(&patched_start, &patched_stop);
            lx_float_t sin_a = lx_vector_cross(&patched_start, &patched_stop);
            lx_float_t tan_a = sin_a / (cos_a + 1.0f);

            /* compute the ctrl point, start => ctrl => stop
             *
             * .       start
             * .        .        ctrl
             * .       .       .
             * .      .      .
             * .     .     .
             * .    .    .         . stop
             * .   .   .       .
             * .  .  .     .
             * . . a   .        1
             * . . . . . . . . . . . . start^
             *       .    a/2        .
             *           .           .
             *               .       .  tan(a/2)
             *                   .   .
             *                       . ctrl^
             *
             *
             * ctrl^    = (1, tan(a/2))
             * matrix   = rotate_sincos(start.y, start.x)
             * ctrl     = ctrl^ * matrix
             */
            lx_point_t ctrl;
            lx_point_make(&ctrl, 1.0f, tan_a);
            lx_point_apply(&ctrl, &applied_matrix);

            // patch the last quadratic curve
            points[count++] = ctrl;
            lx_point_make(&points[count++], patched_stop.x, patched_stop.y);
        }
    }

    // init the applied matrix
    lx_matrix_t applied_matrix;
    lx_matrix_init_sincos(&applied_matrix, start->y, start->x);
    if (direction == LX_ROTATE_DIRECTION_CCW) lx_matrix_scale(&applied_matrix, 1.0f, -1.0f);
    if (matrix) lx_matrix_multiply_lhs(&applied_matrix, matrix);

    // apply matrix for the first point
    lx_point_apply(points, &applied_matrix);

    // walk points
    callback(lx_null, points, udata);
    lx_point_ref_t pb = points + 1;
    lx_point_ref_t pe = points + count;
    for (; pb < pe; pb += 2) {
        lx_point_apply(pb, &applied_matrix);
        lx_point_apply(pb + 1, &applied_matrix);
        callback(pb, pb + 1, udata);
    }
}
