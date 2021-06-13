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
 * @file        lines.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "lines.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_size_t lx_bitmap_renderer_stroke_line_generic(lx_bitmap_writter_t* writter, lx_fixed6_t xb, lx_fixed6_t yb, lx_fixed6_t xe, lx_fixed6_t ye) {
    // round coordinates
    lx_long_t ixb = lx_fixed6_round(xb);
    lx_long_t iyb = lx_fixed6_round(yb);
    lx_long_t ixe = lx_fixed6_round(xe);
    lx_long_t iye = lx_fixed6_round(ye);

    // horizontal?
    if (iyb == iye) return 'h';

    // vertical?
    if (ixb == ixe) return 'v';

    // the dx and dy
    lx_fixed6_t dx = xe - xb;
    lx_fixed6_t dy = ye - yb;

    /* more horizontal?
     *                       .
     *                  .
     *            .
     *      .
     * .
     */
    if (lx_fixed6_abs(dx) > lx_fixed6_abs(dy)) {
        // reverse it for xb => xe
        if (xb > xe) {
            lx_swap(lx_fixed6_t, xb, xe);
            lx_swap(lx_fixed6_t, yb, ye);
            lx_swap(lx_fixed6_t, ixb, ixe);
        }

        // too short? ignore it
        lx_check_return_val(ixb != ixe, 0);

        // compute the slope
        lx_fixed_t slope = lx_fixed6_div(dy, dx);

        /* compute the more accurate start y-coordinate
         *
         * yb + (ixb - xb + 0.5) * dy / dx
         * => yb + ((0.5 - xb) % 1) * dy / dx
         */
        lx_fixed_t start_y = lx_fixed6_to_fixed(yb) + ((slope * ((LX_FIXED6_HALF - xb) & 63)) >> 6);

        // check
        lx_assert(ixb < ixe);

        // draw
        do {
            lx_bitmap_writter_draw_pixel(writter, ixb, lx_fixed_round(start_y));
            start_y += slope;
        } while (++ixb < ixe);

    } else {
        /* more vertical?
         *
         *       .
         *      .
         *     .
         *    .
         *   .
         *  .
         * .
         */

        // reverse it for yb => ye
        if (yb > ye) {
            lx_swap(lx_fixed6_t, xb, xe);
            lx_swap(lx_fixed6_t, yb, ye);
            lx_swap(lx_fixed6_t, iyb, iye);
        }

        // too short? ignore it
        lx_check_return_val(iyb != iye, 0);

        // compute the slope
        lx_fixed_t slope = lx_fixed6_div(dx, dy);

        /* compute the more accurate start x-coordinate
         *
         * xb + (iyb - yb + 0.5) * dx / dy
         * => xb + ((0.5 - yb) % 1) * dx / dy
         */
        lx_fixed_t start_x = lx_fixed6_to_fixed(xb) + ((slope * ((LX_FIXED6_HALF - yb) & 63)) >> 6);

        // check
        lx_assert(iyb < iye);

        // draw
        do {
            lx_bitmap_writter_draw_pixel(writter, lx_fixed_round(start_x), iyb);
            start_x += slope;
        } while (++iyb < iye);
    }
    return 0;
}

static lx_void_t lx_bitmap_renderer_stroke_line_vertical(lx_bitmap_writter_t* writter, lx_fixed6_t xb, lx_fixed6_t yb, lx_fixed6_t xe, lx_fixed6_t ye) {
    // ensure the order
    if (yb > ye) {
        lx_swap(lx_long_t, yb, ye);
    }
    lx_bitmap_writter_draw_vline(writter, lx_fixed6_round(xb), lx_fixed6_round(yb), lx_fixed6_round(ye - yb + LX_FIXED6_ONE));
}

static lx_void_t lx_bitmap_renderer_stroke_line_horizontal(lx_bitmap_writter_t* writter, lx_fixed6_t xb, lx_fixed6_t yb, lx_fixed6_t xe, lx_fixed6_t ye) {
    // ensure the order
    if (xb > xe) {
        lx_swap(lx_long_t, xb, xe);
    }
    lx_bitmap_writter_draw_hline(writter, lx_fixed6_round(xb), lx_fixed6_round(yb), lx_fixed6_round(xe - xb + LX_FIXED6_ONE));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_bitmap_renderer_stroke_lines(lx_bitmap_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count && !(count & 0x1));

    lx_size_t       i   = 0;
    lx_size_t       ok  = 0;
    lx_point_ref_t  pb  = lx_null;
    lx_point_ref_t  pe  = lx_null;
    lx_fixed6_t     xb  = 0;
    lx_fixed6_t     yb  = 0;
    lx_fixed6_t     xe  = 0;
    lx_fixed6_t     ye  = 0;
    for (i = 0; i < count; i += 2) {
        // line: pb => pe
        pb = points + i;
        pe = points + i + 1;

        // (xb, yb) => (xe, ye)
        xb = lx_float_to_fixed6(pb->x);
        yb = lx_float_to_fixed6(pb->y);
        xe = lx_float_to_fixed6(pe->x);
        ye = lx_float_to_fixed6(pe->y);

        // draw line
        if ((ok = lx_bitmap_renderer_stroke_line_generic(&device->writter, xb, yb, xe, ye))) {
            lx_assert(ok == 'h' || ok == 'v');
            if (ok == 'h') {
                lx_bitmap_renderer_stroke_line_horizontal(&device->writter, xb, yb, xe, ye);
            } else {
                lx_bitmap_renderer_stroke_line_vertical(&device->writter, xb, yb, xe, ye);
            }
        }
    }
}
