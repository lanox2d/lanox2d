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
 * @file        xrgb8888.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "private/rgb32.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static lx_inline lx_pixel_t lx_pixmap_xrgb8888_pixel(lx_color_t color) {
    return lx_pixmap_rgb32_pixel(color) | 0xff000000;
}

static lx_inline lx_color_t lx_pixmap_xrgb8888_color(lx_pixel_t pixel) {
    return lx_pixmap_rgb32_color(pixel | 0xff000000);
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_pixel_set_la(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u32_le(data, lx_pixmap_rgb32_blend(lx_bits_get_u32_le(data), pixel, alpha));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_pixel_set_ba(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u32_be(data, lx_pixmap_rgb32_blend(lx_bits_get_u32_be(data), pixel, alpha));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_pixel_copy_la(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    lx_bits_set_u32_le(data, lx_pixmap_rgb32_blend(lx_bits_get_u32_le(data), lx_bits_get_u32_le(source), alpha));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_pixel_copy_ba(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    lx_bits_set_u32_be(data, lx_pixmap_rgb32_blend(lx_bits_get_u32_be(data), lx_bits_get_u32_be(source), alpha));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_color_set_lo(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u32_le(data, lx_pixmap_xrgb8888_pixel(color));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_color_set_bo(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u32_be(data, lx_pixmap_xrgb8888_pixel(color));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_color_set_la(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u32_le(data, lx_pixmap_rgb32_blend(lx_bits_get_u32_le(data), lx_pixmap_xrgb8888_pixel(color), color.a));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_color_set_ba(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u32_be(data, lx_pixmap_rgb32_blend(lx_bits_get_u32_be(data), lx_pixmap_xrgb8888_pixel(color), color.a));
}

static lx_inline lx_color_t lx_pixmap_xrgb8888_color_get_l(lx_cpointer_t data) {
    return lx_pixmap_xrgb8888_color(lx_bits_get_u32_le(data));
}

static lx_inline lx_color_t lx_pixmap_xrgb8888_color_get_b(lx_cpointer_t data) {
    return lx_pixmap_xrgb8888_color(lx_bits_get_u32_be(data));
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_pixels_fill_la(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_size_t    l = count & 0x3; count -= l;
    lx_uint32_t* p = (lx_uint32_t*)data;
    lx_uint32_t* e = p + count;
    lx_uint32_t  hs = (pixel >> 8) & 0x00ff00ff;
    lx_uint32_t  ls = pixel & 0x00ff00ff;
    while (p < e) {
        lx_bits_set_u32_le(&p[0], lx_pixmap_rgb32_blend2(lx_bits_get_u32_le(&p[0]), hs, ls, alpha));
        lx_bits_set_u32_le(&p[1], lx_pixmap_rgb32_blend2(lx_bits_get_u32_le(&p[1]), hs, ls, alpha));
        lx_bits_set_u32_le(&p[2], lx_pixmap_rgb32_blend2(lx_bits_get_u32_le(&p[2]), hs, ls, alpha));
        lx_bits_set_u32_le(&p[3], lx_pixmap_rgb32_blend2(lx_bits_get_u32_le(&p[3]), hs, ls, alpha));
        p += 4;
    }
    while (l--) {
        lx_bits_set_u32_le(&p[0], lx_pixmap_rgb32_blend2(lx_bits_get_u32_le(&p[0]), hs, ls, alpha));
        p++;
    }
}

static lx_inline lx_void_t lx_pixmap_xrgb8888_pixels_fill_ba(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_size_t    l = count & 0x3; count -= l;
    lx_uint32_t* p = (lx_uint32_t*)data;
    lx_uint32_t* e = p + count;
    lx_uint32_t  hs = (pixel >> 8) & 0x00ff00ff;
    lx_uint32_t  ls = pixel & 0x00ff00ff;
    while (p < e) {
        lx_bits_set_u32_be(&p[0], lx_pixmap_rgb32_blend2(lx_bits_get_u32_be(&p[0]), hs, ls, alpha));
        lx_bits_set_u32_be(&p[1], lx_pixmap_rgb32_blend2(lx_bits_get_u32_be(&p[1]), hs, ls, alpha));
        lx_bits_set_u32_be(&p[2], lx_pixmap_rgb32_blend2(lx_bits_get_u32_be(&p[2]), hs, ls, alpha));
        lx_bits_set_u32_be(&p[3], lx_pixmap_rgb32_blend2(lx_bits_get_u32_be(&p[3]), hs, ls, alpha));
        p += 4;
    }
    while (l--) {
        lx_bits_set_u32_be(&p[0], lx_pixmap_rgb32_blend2(lx_bits_get_u32_be(&p[0]), hs, ls, alpha));
        p++;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static lx_pixmap_t const g_pixmap_lo_xrgb8888 = {
    "xrgb8888"
,   32
,   4
,   LX_PIXFMT_XRGB8888
,   lx_pixmap_xrgb8888_pixel
,   lx_pixmap_xrgb8888_color
,   lx_pixmap_rgb32_pixel_get_l
,   lx_pixmap_rgb32_pixel_set_lo
,   lx_pixmap_rgb32_pixel_copy_o
,   lx_pixmap_xrgb8888_color_get_l
,   lx_pixmap_xrgb8888_color_set_lo
,   lx_pixmap_rgb32_pixels_fill_lo
};

static lx_pixmap_t const g_pixmap_bo_xrgb8888 = {
    "xrgb8888"
,   32
,   4
,   LX_PIXFMT_XRGB8888 | LX_PIXFMT_BENDIAN
,   lx_pixmap_xrgb8888_pixel
,   lx_pixmap_xrgb8888_color
,   lx_pixmap_rgb32_pixel_get_b
,   lx_pixmap_rgb32_pixel_set_bo
,   lx_pixmap_rgb32_pixel_copy_o
,   lx_pixmap_xrgb8888_color_get_b
,   lx_pixmap_xrgb8888_color_set_bo
,   lx_pixmap_rgb32_pixels_fill_bo
};

static lx_pixmap_t const g_pixmap_la_xrgb8888 = {
    "xrgb8888"
,   32
,   4
,   LX_PIXFMT_XRGB8888
,   lx_pixmap_xrgb8888_pixel
,   lx_pixmap_xrgb8888_color
,   lx_pixmap_rgb32_pixel_get_l
,   lx_pixmap_xrgb8888_pixel_set_la
,   lx_pixmap_xrgb8888_pixel_copy_la
,   lx_pixmap_xrgb8888_color_get_l
,   lx_pixmap_xrgb8888_color_set_la
,   lx_pixmap_xrgb8888_pixels_fill_la
};

static lx_pixmap_t const g_pixmap_ba_xrgb8888 = {
    "xrgb8888"
,   32
,   4
,   LX_PIXFMT_XRGB8888 | LX_PIXFMT_BENDIAN
,   lx_pixmap_xrgb8888_pixel
,   lx_pixmap_xrgb8888_color
,   lx_pixmap_rgb32_pixel_get_b
,   lx_pixmap_xrgb8888_pixel_set_ba
,   lx_pixmap_xrgb8888_pixel_copy_ba
,   lx_pixmap_xrgb8888_color_get_b
,   lx_pixmap_xrgb8888_color_set_ba
,   lx_pixmap_xrgb8888_pixels_fill_ba
};

