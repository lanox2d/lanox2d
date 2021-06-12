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

lx_void_t lx_bitmap_biltter_done_p(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y) {
    lx_assert(biltter && biltter->done_p);
    biltter->done_p(biltter, x, y);
}

lx_void_t lx_bitmap_biltter_done_h(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w) {
    lx_assert(biltter && biltter->done_h);
    biltter->done_h(biltter, x, y, w);
}

lx_void_t lx_bitmap_biltter_done_v(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t h) {
    lx_assert(biltter && biltter->done_v);
    biltter->done_v(biltter, x, y, h);
}

lx_void_t lx_bitmap_biltter_done_r(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h) {
    lx_assert(biltter);
    if (h == 1) {
        lx_assert(biltter->done_h);
        biltter->done_h(biltter, x, y, w);
        return ;
    } else if (w == 1) {
        lx_assert(biltter->done_v);
        biltter->done_v(biltter, x, y, h);
        return ;
    } else if (biltter->done_r) {
        biltter->done_r(biltter, x, y, w, h);
    } else {
        lx_assert(biltter->done_h);
        while (h--) biltter->done_h(biltter, x, y++, w);
    }
}
