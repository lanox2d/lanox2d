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
static lx_void_t lx_bitmap_writter_solid_draw_pixel(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y) {
    lx_assert(writter && writter->pixmap && writter->pixmap->pixel_set);
    lx_assert(x >= 0 && y >= 0);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(writter->bitmap);
    lx_assert(pixels);
    writter->pixmap->pixel_set(pixels + y * writter->row_bytes + x * writter->btp, writter->u.solid.pixel, writter->u.solid.alpha);
}

static lx_void_t lx_bitmap_writter_solid_draw_hline(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y, lx_long_t w) {
    lx_assert(writter && writter->pixmap && writter->pixmap->pixels_fill);
    lx_assert(x >= 0 && y >= 0 && w >= 0);
    lx_check_return(w);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(writter->bitmap);
    lx_assert(pixels);
    writter->pixmap->pixels_fill(pixels + y * writter->row_bytes + x * writter->btp, writter->u.solid.pixel, w, writter->u.solid.alpha);
}

static lx_void_t lx_bitmap_writter_solid_draw_vline(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y, lx_long_t h) {
    lx_assert(writter && writter->pixmap && writter->pixmap->pixels_fill);
    lx_assert(x >= 0 && y >= 0 && h >= 0);
    lx_check_return(h);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(writter->bitmap);
    lx_assert(pixels);

    lx_size_t                       btp = writter->btp;
    lx_size_t                       row_bytes = writter->row_bytes;
    lx_pixel_t                      pixel = writter->u.solid.pixel;
    lx_byte_t                       alpha = writter->u.solid.alpha;
    lx_pixmap_func_pixel_set_t      pixel_set = writter->pixmap->pixel_set;
    while (h--) {
        pixel_set(pixels + y++ * row_bytes + x * btp, pixel, alpha);
    }
}

static lx_void_t lx_bitmap_writter_solid_draw_rect(lx_bitmap_writter_t* writter, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h) {
    lx_assert(writter && writter->pixmap && writter->pixmap->pixels_fill);
    lx_assert(x >= 0 && y >= 0 && w >= 0 && h >= 0);
    lx_check_return(h && w);

    lx_byte_t* pixels = (lx_byte_t*)lx_bitmap_data(writter->bitmap);
    lx_assert(pixels);

    lx_size_t                       btp = writter->btp;
    lx_size_t                       row_bytes = writter->row_bytes;
    lx_pixel_t                      pixel = writter->u.solid.pixel;
    lx_byte_t                       alpha = writter->u.solid.alpha;
    lx_pixmap_func_pixels_fill_t    pixels_fill = writter->pixmap->pixels_fill;
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
lx_bool_t lx_bitmap_writter_solid_init(lx_bitmap_writter_t* writter, lx_bitmap_ref_t bitmap, lx_paint_ref_t paint) {
    lx_assert(writter && bitmap && paint);

    writter->bitmap        = bitmap;
    writter->pixmap        = lx_pixmap(lx_bitmap_pixfmt(bitmap), lx_paint_alpha(paint));
    writter->btp           = writter->pixmap->btp;
    writter->row_bytes     = lx_bitmap_row_bytes(writter->bitmap);
    writter->u.solid.pixel = writter->pixmap->pixel(lx_paint_color(paint));
    writter->u.solid.alpha = lx_paint_alpha(paint);
    writter->draw_pixel    = lx_bitmap_writter_solid_draw_pixel;
    writter->draw_hline    = lx_bitmap_writter_solid_draw_hline;
    writter->draw_vline    = lx_bitmap_writter_solid_draw_vline;
    writter->draw_rect     = lx_bitmap_writter_solid_draw_rect;
    writter->exit          = lx_null;
    lx_check_return_val(writter->pixmap, lx_false);
    return lx_true;
}

