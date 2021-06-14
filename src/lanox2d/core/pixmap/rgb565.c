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
 * @file        rgb565.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "rgb16.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

/* the alpha blend
 *
 * c:
 * 0000 0000 0000 0000 rrrr rggg gggb bbbb
 *
 * c | c << 16:
 * rrrr rggg gggb bbbb rrrr rggg gggb bbbb
 *
 * 0x7e0f81f:
 * 0000 0111 1110 0000 1111 1000 0001 1111
 *
 * d = (c | c << 16) & 0x7e0f81f:
 * 0000 0ggg ggg0 0000 rrrr r000 000b bbbb
 *
 * (d & 0xffff) | (d >> 16):
 * 0000 0000 0000 0000 rrrr rggg gggb bbbb
 *
 * (s * a + d * (32 - a)) >> 5 => ((s - d) * a) >> 5 + d
 */
static lx_inline lx_uint16_t lx_pixmap_rgb565_blend(lx_uint32_t d, lx_uint32_t s, lx_byte_t a) {
    // FIXME: s - d? overflow?
    s = (s | (s << 16)) & 0x7e0f81f;
    d = (d | (d << 16)) & 0x7e0f81f;
    d = ((((s - d) * a) >> 5) + d) & 0x7e0f81f;
    return (lx_uint16_t)((d & 0xffff) | (d >> 16));
}
static lx_inline lx_uint16_t lx_pixmap_rgb565_blend2(lx_uint32_t d, lx_uint32_t s, lx_byte_t a) {
    d = (d | (d << 16)) & 0x7e0f81f;
    d = ((((s - d) * a) >> 5) + d) & 0x7e0f81f;
    return (lx_uint16_t)((d & 0xffff) | (d >> 16));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static lx_inline lx_pixel_t lx_pixmap_rgb565_pixel(lx_color_t color) {
    return LX_RGB_565(color.r, color.g, color.b);
}

static lx_inline lx_color_t lx_pixmap_rgb565_color(lx_pixel_t pixel) {
    lx_color_t color;
    color.a = 0xff;
    color.r = LX_RGB_565_R(pixel);
    color.g = LX_RGB_565_G(pixel);
    color.b = LX_RGB_565_B(pixel);
    return color;
}

static lx_inline lx_void_t lx_pixmap_rgb565_pixel_set_la(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u16_le(data, lx_pixmap_rgb565_blend(lx_bits_get_u16_le(data), pixel, alpha >> 3));
}

static lx_inline lx_void_t lx_pixmap_rgb565_pixel_set_ba(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u16_be(data, lx_pixmap_rgb565_blend(lx_bits_get_u16_be(data), pixel, alpha >> 3));
}

static lx_inline lx_void_t lx_pixmap_rgb565_pixel_copy_la(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    lx_bits_set_u16_le(data, lx_pixmap_rgb565_blend(lx_bits_get_u16_le(data), lx_bits_get_u16_le(source), alpha >> 3));
}

static lx_inline lx_void_t lx_pixmap_rgb565_pixel_copy_ba(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    lx_bits_set_u16_be(data, lx_pixmap_rgb565_blend(lx_bits_get_u16_be(data), lx_bits_get_u16_be(source), alpha >> 3));
}

static lx_inline lx_color_t lx_pixmap_rgb565_color_get_l(lx_cpointer_t data) {
    return lx_pixmap_rgb565_color(lx_bits_get_u16_le(data));
}

static lx_inline lx_color_t lx_pixmap_rgb565_color_get_b(lx_cpointer_t data) {
    return lx_pixmap_rgb565_color(lx_bits_get_u16_be(data));
}

static lx_inline lx_void_t lx_pixmap_rgb565_color_set_lo(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u16_le(data, lx_pixmap_rgb565_pixel(color));
}

static lx_inline lx_void_t lx_pixmap_rgb565_color_set_bo(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u16_be(data, lx_pixmap_rgb565_pixel(color));
}

static lx_inline lx_void_t lx_pixmap_rgb565_color_set_la(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u16_le(data, lx_pixmap_rgb565_blend(lx_bits_get_u16_le(data), lx_pixmap_rgb565_pixel(color), color.a >> 3));
}

static lx_inline lx_void_t lx_pixmap_rgb565_color_set_ba(lx_pointer_t data, lx_color_t color) {
    lx_bits_set_u16_be(data, lx_pixmap_rgb565_blend(lx_bits_get_u16_be(data), lx_pixmap_rgb565_pixel(color), color.a >> 3));
}

static lx_inline lx_void_t lx_pixmap_rgb565_pixels_fill_la(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_size_t       l = count & 0x3; count -= l; alpha >>= 3;
    lx_uint16_t*    p = (lx_uint16_t*)data;
    lx_uint16_t*    e = p + count;
    lx_uint32_t     s = (pixel | (pixel << 16)) & 0x7e0f81f;
    while (p < e) {
        lx_bits_set_u16_le(&p[0], lx_pixmap_rgb565_blend2(lx_bits_get_u16_le(&p[0]), s, alpha));
        lx_bits_set_u16_le(&p[1], lx_pixmap_rgb565_blend2(lx_bits_get_u16_le(&p[1]), s, alpha));
        lx_bits_set_u16_le(&p[2], lx_pixmap_rgb565_blend2(lx_bits_get_u16_le(&p[2]), s, alpha));
        lx_bits_set_u16_le(&p[3], lx_pixmap_rgb565_blend2(lx_bits_get_u16_le(&p[3]), s, alpha));
        p += 4;
    }
    while (l--) {
        lx_bits_set_u16_le(&p[0], lx_pixmap_rgb565_blend2(lx_bits_get_u16_le(&p[0]), s, alpha));
        p++;
    }
}

static lx_inline lx_void_t lx_pixmap_rgb565_pixels_fill_ba(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_size_t       l = count & 0x3; count -= l; alpha >>= 3;
    lx_uint16_t*    p = (lx_uint16_t*)data;
    lx_uint16_t*    e = p + count;
    lx_uint32_t     s = (pixel | (pixel << 16)) & 0x7e0f81f;
    while (p < e) {
        lx_bits_set_u16_be(&p[0], lx_pixmap_rgb565_blend2(lx_bits_get_u16_be(&p[0]), s, alpha));
        lx_bits_set_u16_be(&p[1], lx_pixmap_rgb565_blend2(lx_bits_get_u16_be(&p[1]), s, alpha));
        lx_bits_set_u16_be(&p[2], lx_pixmap_rgb565_blend2(lx_bits_get_u16_be(&p[2]), s, alpha));
        lx_bits_set_u16_be(&p[3], lx_pixmap_rgb565_blend2(lx_bits_get_u16_be(&p[3]), s, alpha));
        p += 4;
    }
    while (l--) {
        lx_bits_set_u16_be(&p[0], lx_pixmap_rgb565_blend2(lx_bits_get_u16_be(&p[0]), s, alpha));
        p++;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

static lx_pixmap_t const g_pixmap_lo_rgb565 = {
    "rgb565"
,   16
,   2
,   LX_PIXFMT_RGB565
#ifdef LX_CONFIG_DEVICE_HAVE_BITMAP
,   lx_pixmap_rgb565_pixel
,   lx_pixmap_rgb565_color
,   lx_pixmap_rgb16_pixel_get_l
,   lx_pixmap_rgb16_pixel_set_lo
,   lx_pixmap_rgb16_pixel_copy_o
,   lx_pixmap_rgb565_color_get_l
,   lx_pixmap_rgb565_color_set_lo
,   lx_pixmap_rgb16_pixels_fill_lo
#endif
};

static lx_pixmap_t const g_pixmap_bo_rgb565 = {
    "rgb565"
,   16
,   2
,   LX_PIXFMT_RGB565 | LX_PIXFMT_BENDIAN
#ifdef LX_CONFIG_DEVICE_HAVE_BITMAP
,   lx_pixmap_rgb565_pixel
,   lx_pixmap_rgb565_color
,   lx_pixmap_rgb16_pixel_get_l
,   lx_pixmap_rgb16_pixel_set_lo
,   lx_pixmap_rgb16_pixel_copy_o
,   lx_pixmap_rgb565_color_get_b
,   lx_pixmap_rgb565_color_set_bo
,   lx_pixmap_rgb16_pixels_fill_bo
#endif
};

static lx_pixmap_t const g_pixmap_la_rgb565 = {
    "rgb565"
,   16
,   2
,   LX_PIXFMT_RGB565
#ifdef LX_CONFIG_DEVICE_HAVE_BITMAP
,   lx_pixmap_rgb565_pixel
,   lx_pixmap_rgb565_color
,   lx_pixmap_rgb16_pixel_get_l
,   lx_pixmap_rgb565_pixel_set_la
,   lx_pixmap_rgb565_pixel_copy_la
,   lx_pixmap_rgb565_color_get_l
,   lx_pixmap_rgb565_color_set_la
,   lx_pixmap_rgb565_pixels_fill_la
#endif
};

static lx_pixmap_t const g_pixmap_ba_rgb565 = {
    "rgb565"
,   16
,   2
,   LX_PIXFMT_RGB565 | LX_PIXFMT_BENDIAN
#ifdef LX_CONFIG_DEVICE_HAVE_BITMAP
,   lx_pixmap_rgb565_pixel
,   lx_pixmap_rgb565_color
,   lx_pixmap_rgb16_pixel_get_b
,   lx_pixmap_rgb565_pixel_set_ba
,   lx_pixmap_rgb565_pixel_copy_ba
,   lx_pixmap_rgb565_color_get_b
,   lx_pixmap_rgb565_color_set_ba
,   lx_pixmap_rgb565_pixels_fill_ba
#endif
};
