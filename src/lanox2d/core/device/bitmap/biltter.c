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
 * @file        biltter.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "biltter.h"
#include "biltter/solid.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_bitmap_biltter_init(lx_bitmap_biltter_t* biltter, lx_bitmap_ref_t bitmap, lx_paint_ref_t paint) {
    return lx_bitmap_biltter_solid_init(biltter, bitmap, paint);
}

lx_void_t lx_bitmap_biltter_exit(lx_bitmap_biltter_t* biltter) {
    if (biltter && biltter->exit) {
        biltter->exit(biltter);
    }
}

lx_void_t lx_bitmap_biltter_draw_pixel(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y) {
    lx_assert(biltter && biltter->draw_pixel);
    biltter->draw_pixel(biltter, x, y);
}

lx_void_t lx_bitmap_biltter_draw_hline(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w) {
    lx_assert(biltter && biltter->draw_hline);
    biltter->draw_hline(biltter, x, y, w);
}

lx_void_t lx_bitmap_biltter_draw_vline(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t h) {
    lx_assert(biltter && biltter->draw_vline);
    biltter->draw_vline(biltter, x, y, h);
}

lx_void_t lx_bitmap_biltter_draw_rect(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h) {
    lx_assert(biltter);
    if (h == 1) {
        lx_assert(biltter->draw_hline);
        biltter->draw_hline(biltter, x, y, w);
        return ;
    } else if (w == 1) {
        lx_assert(biltter->draw_vline);
        biltter->draw_vline(biltter, x, y, h);
        return ;
    } else if (biltter->draw_rect) {
        biltter->draw_rect(biltter, x, y, w, h);
    } else {
        lx_assert(biltter->draw_hline);
        while (h--) biltter->draw_hline(biltter, x, y++, w);
    }
}
