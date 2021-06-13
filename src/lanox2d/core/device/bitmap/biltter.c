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
 * @file        writter.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "writter.h"
#include "writter/solid.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_bitmap_writter_init(lx_bitmap_writter_t* writter, lx_bitmap_ref_t bitmap, lx_paint_ref_t paint) {
    return lx_bitmap_writter_solid_init(writter, bitmap, paint);
}

lx_void_t lx_bitmap_writter_exit(lx_bitmap_writter_t* writter) {
    if (writter && writter->exit) {
        writter->exit(writter);
    }
}

lx_void_t lx_bitmap_writter_draw_pixel(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y) {
    lx_assert(writter && writter->draw_pixel);
    writter->draw_pixel(writter, x, y);
}

lx_void_t lx_bitmap_writter_draw_hline(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y, lx_long_t w) {
    lx_assert(writter && writter->draw_hline);
    writter->draw_hline(writter, x, y, w);
}

lx_void_t lx_bitmap_writter_draw_vline(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y, lx_long_t h) {
    lx_assert(writter && writter->draw_vline);
    writter->draw_vline(writter, x, y, h);
}

lx_void_t lx_bitmap_writter_draw_rect(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h) {
    lx_assert(writter);
    if (h == 1) {
        lx_assert(writter->draw_hline);
        writter->draw_hline(writter, x, y, w);
        return ;
    } else if (w == 1) {
        lx_assert(writter->draw_vline);
        writter->draw_vline(writter, x, y, h);
        return ;
    } else if (writter->draw_rect) {
        writter->draw_rect(writter, x, y, w, h);
    } else {
        lx_assert(writter->draw_hline);
        while (h--) writter->draw_hline(writter, x, y++, w);
    }
}
