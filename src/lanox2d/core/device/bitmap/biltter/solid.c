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
 * @file        solid.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "solid.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_bitmap_biltter_solid_done_p(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y) {
    lx_assert(biltter && biltter->pixmap && biltter->pixmap->pixel_set);
    lx_assert(x >= 0 && y >= 0);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(biltter->bitmap);
    lx_assert(pixels);
    biltter->pixmap->pixel_set(pixels + y * biltter->row_bytes + x * biltter->btp, biltter->u.solid.pixel, biltter->u.solid.alpha);
}

static lx_void_t lx_bitmap_biltter_solid_done_h(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w) {
    lx_assert(biltter && biltter->pixmap && biltter->pixmap->pixels_fill);
    lx_assert(x >= 0 && y >= 0 && w >= 0);
    lx_check_return(w);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(biltter->bitmap);
    lx_assert(pixels);
    biltter->pixmap->pixels_fill(pixels + y * biltter->row_bytes + x * biltter->btp, biltter->u.solid.pixel, w, biltter->u.solid.alpha);
}

static lx_void_t lx_bitmap_biltter_solid_done_v(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t h) {
    lx_assert(biltter && biltter->pixmap && biltter->pixmap->pixels_fill);
    lx_assert(x >= 0 && y >= 0 && h >= 0);
    lx_check_return(h);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(biltter->bitmap);
    lx_assert(pixels);

    lx_size_t                       btp = biltter->btp;
    lx_size_t                       row_bytes = biltter->row_bytes;
    lx_pixel_t                      pixel = biltter->u.solid.pixel;
    lx_byte_t                       alpha = biltter->u.solid.alpha;
    lx_pixmap_func_pixel_set_t      pixel_set = biltter->pixmap->pixel_set;
    while (h--) {
        pixel_set(pixels + y++ * row_bytes + x * btp, pixel, alpha);
    }
}

static lx_void_t lx_bitmap_biltter_solid_done_r(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h) {
    lx_assert(biltter && biltter->pixmap && biltter->pixmap->pixels_fill);
    lx_assert(x >= 0 && y >= 0 && w >= 0 && h >= 0);
    lx_check_return(h && w);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(biltter->bitmap);
    lx_assert(pixels);

    lx_size_t                       btp = biltter->btp;
    lx_size_t                       row_bytes = biltter->row_bytes;
    lx_pixel_t                      pixel = biltter->u.solid.pixel;
    lx_byte_t                       alpha = biltter->u.solid.alpha;
    lx_pixmap_func_pixels_fill_t    pixels_fill = biltter->pixmap->pixels_fill;
    if (!x && (w * btp == row_bytes)) {
        pixels_fill(pixels, pixel, h * w, alpha);
    } else {
        pixels += y * row_bytes + x * btp;
        while (h--) {
            pixels_fill(pixels, pixel, w, alpha);
            pixels += row_bytes;
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_bitmap_biltter_solid_init(lx_bitmap_biltter_t* biltter, lx_bitmap_ref_t bitmap, lx_paint_ref_t paint) {
    lx_assert(biltter && bitmap && paint);

    biltter->bitmap        = bitmap;
    biltter->pixmap        = lx_pixmap(lx_bitmap_pixfmt(bitmap), lx_paint_alpha(paint));
    biltter->btp           = biltter->pixmap->btp;
    biltter->row_bytes     = lx_bitmap_row_bytes(biltter->bitmap);
    biltter->u.solid.pixel = biltter->pixmap->pixel(lx_paint_color(paint));
    biltter->u.solid.alpha = lx_paint_alpha(paint);
    biltter->done_p        = lx_bitmap_biltter_solid_done_p;
    biltter->done_h        = lx_bitmap_biltter_solid_done_h;
    biltter->done_v        = lx_bitmap_biltter_solid_done_v;
    biltter->done_r        = lx_bitmap_biltter_solid_done_r;
    biltter->exit          = lx_null;
    lx_check_return_val(biltter->pixmap, lx_false);
    return lx_true;
}

