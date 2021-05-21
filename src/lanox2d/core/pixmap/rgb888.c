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
 * @file        rgb888.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "private/rgb24.h"
#include "private/rgb32.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

static lx_pixel_t lx_pixmap_rgb888_pixel(lx_color_t color);
static lx_color_t lx_pixmap_rgb888_color(lx_pixel_t pixel);
static lx_inline lx_uint32_t lx_pixmap_rgb888_blend(lx_uint32_t d, lx_uint32_t s, lx_byte_t a) {
    lx_color_t c;
    lx_color_t cs = lx_pixmap_rgb888_color(s & 0x00ffffff);
    lx_color_t cd = lx_pixmap_rgb888_color(d & 0x00ffffff);
    c.r = ((a * (cs.r - cd.r)) >> 8) + cd.r;
    c.g = ((a * (cs.g - cd.g)) >> 8) + cd.g;
    c.b = ((a * (cs.b - cd.b)) >> 8) + cd.b;
    return lx_pixmap_rgb888_pixel(c) & 0x00ffffff;
}

static lx_inline lx_uint32_t lx_pixmap_rgb888_blend2(lx_uint32_t d, lx_color_t s, lx_byte_t a) {
    lx_color_t c;
    lx_color_t cd = lx_pixmap_rgb888_color(d & 0x00ffffff);
    c.r = ((a * (s.r - cd.r)) >> 8) + cd.r;
    c.g = ((a * (s.g - cd.g)) >> 8) + cd.g;
    c.b = ((a * (s.b - cd.b)) >> 8) + cd.b;
    return lx_pixmap_rgb888_pixel(c) & 0x00ffffff;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
static lx_inline lx_pixel_t lx_pixmap_rgb888_pixel(lx_color_t color) {
    return lx_pixmap_rgb32_pixel(color) & 0x00ffffff;
}

static lx_inline lx_color_t lx_pixmap_rgb888_color(lx_pixel_t pixel) {
    return lx_pixmap_rgb32_color(pixel & 0x00ffffff);
}

static lx_inline lx_void_t lx_pixmap_rgb888_pixel_set_la(lx_pointer_t data, lx_uint32_t pixel, lx_byte_t alpha) {
    lx_bits_set_u24_le(data, lx_pixmap_rgb888_blend(lx_bits_get_u24_le(data), pixel, alpha));
}

static lx_inline lx_void_t lx_pixmap_rgb888_pixel_set_ba(lx_pointer_t data, lx_uint32_t pixel, lx_byte_t alpha) {
    lx_bits_set_u24_be(data, lx_pixmap_rgb888_blend(lx_bits_get_u24_be(data), pixel, alpha));
}

static lx_inline lx_void_t lx_pixmap_rgb888_pixel_copy_la(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    lx_bits_set_u24_le(data, lx_pixmap_rgb888_blend(lx_bits_get_u24_le(data), lx_bits_get_u24_le(source), alpha));
}

static lx_inline lx_void_t lx_pixmap_rgb888_pixel_copy_ba(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    lx_bits_set_u24_be(data, lx_pixmap_rgb888_blend(lx_bits_get_u24_be(data), lx_bits_get_u24_be(source), alpha));
}

static lx_inline lx_color_t lx_pixmap_rgb888_color_get_l(lx_cpointer_t data) {
    return lx_pixmap_rgb888_color(lx_bits_get_u24_le(data));
}

static lx_inline lx_color_t lx_pixmap_rgb888_color_get_b(lx_cpointer_t data) {
    return lx_pixmap_rgb888_color(lx_bits_get_u24_be(data));
}

static lx_inline lx_void_t lx_pixmap_rgb888_color_set_lo(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u24_le(data, lx_pixmap_rgb888_pixel(color));
}

static lx_inline lx_void_t lx_pixmap_rgb888_color_set_bo(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u24_be(data, lx_pixmap_rgb888_pixel(color));
}

static lx_inline lx_void_t lx_pixmap_rgb888_color_set_la(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u24_le(data, lx_pixmap_rgb888_blend(lx_bits_get_u24_le(data), lx_pixmap_rgb888_pixel(color), color.a));
}

static lx_inline lx_void_t lx_pixmap_rgb888_color_set_ba(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u24_be(data, lx_pixmap_rgb888_blend(lx_bits_get_u24_be(data), lx_pixmap_rgb888_pixel(color), color.a));
}

static lx_inline lx_void_t lx_pixmap_rgb888_pixels_fill_la(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_byte_t* p = (lx_byte_t*)data;
    lx_byte_t* e = p + count * 3;
    lx_color_t s = lx_pixmap_rgb888_color(pixel);
    while (p < e) {
        lx_bits_set_u24_le(p, lx_pixmap_rgb888_blend2(lx_bits_get_u24_le(p), s, alpha));
        p += 3;
    }
}

static lx_inline lx_void_t lx_pixmap_rgb888_pixels_fill_ba(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_byte_t* p = (lx_byte_t*)data;
    lx_byte_t* e = p + count * 3;
    lx_color_t s = lx_pixmap_rgb888_color(pixel);
    while (p < e) {
        lx_bits_set_u24_be(p, lx_pixmap_rgb888_blend2(lx_bits_get_u24_be(p), s, alpha));
        p += 3;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

static lx_pixmap_t const g_pixmap_lo_rgb888 = {
    "rgb888"
,   24
,   3
,   LX_PIXFMT_RGB888
,   lx_pixmap_rgb888_pixel
,   lx_pixmap_rgb888_color
,   lx_pixmap_rgb24_pixel_get_l
,   lx_pixmap_rgb24_pixel_set_lo
,   lx_pixmap_rgb24_pixel_copy_o
,   lx_pixmap_rgb888_color_get_l
,   lx_pixmap_rgb888_color_set_lo
,   lx_pixmap_rgb24_pixels_fill_lo
};

static lx_pixmap_t const g_pixmap_bo_rgb888 = {
    "rgb888"
,   24
,   3
,   LX_PIXFMT_RGB888 | LX_PIXFMT_BENDIAN
,   lx_pixmap_rgb888_pixel
,   lx_pixmap_rgb888_color
,   lx_pixmap_rgb24_pixel_get_b
,   lx_pixmap_rgb24_pixel_set_bo
,   lx_pixmap_rgb24_pixel_copy_o
,   lx_pixmap_rgb888_color_get_b
,   lx_pixmap_rgb888_color_set_bo
,   lx_pixmap_rgb24_pixels_fill_bo
};

static lx_pixmap_t const g_pixmap_la_rgb888 = {
    "rgb888"
,   24
,   3
,   LX_PIXFMT_RGB888
,   lx_pixmap_rgb888_pixel
,   lx_pixmap_rgb888_color
,   lx_pixmap_rgb24_pixel_get_l
,   lx_pixmap_rgb888_pixel_set_la
,   lx_pixmap_rgb888_pixel_copy_la
,   lx_pixmap_rgb888_color_get_l
,   lx_pixmap_rgb888_color_set_la
,   lx_pixmap_rgb888_pixels_fill_la
};

static lx_pixmap_t const g_pixmap_ba_rgb888 = {
    "rgb888"
,   24
,   3
,   LX_PIXFMT_RGB888 | LX_PIXFMT_BENDIAN
,   lx_pixmap_rgb888_pixel
,   lx_pixmap_rgb888_color
,   lx_pixmap_rgb24_pixel_get_b
,   lx_pixmap_rgb888_pixel_set_ba
,   lx_pixmap_rgb888_pixel_copy_ba
,   lx_pixmap_rgb888_color_get_b
,   lx_pixmap_rgb888_color_set_ba
,   lx_pixmap_rgb888_pixels_fill_ba
};

