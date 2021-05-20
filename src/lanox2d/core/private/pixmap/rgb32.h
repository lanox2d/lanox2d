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
 * @file        rgb32.h
 *
 */

#ifndef LX_CORE_PRIVATE_PIXMAP_RGB32_H
#define LX_CORE_PRIVATE_PIXMAP_RGB32_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

static lx_inline lx_uint32_t lx_pixmap_rgb32_blend(lx_uint32_t d, lx_uint32_t s, lx_byte_t a) {
    lx_uint32_t hs = (s >> 8) & 0x00ff00ff;
    lx_uint32_t hd = (d >> 8) & 0x00ff00ff;
    lx_uint32_t ls = s & 0x00ff00ff;
    lx_uint32_t ld = d & 0x00ff00ff;
    hd = (((a * (hs - hd)) >> 8) + hd) & 0x00ff00ff;
    ld = (((a * (ls - ld)) >> 8) + ld) & 0x00ff00ff;
    return (hd << 8) | ld;
}

static lx_inline lx_uint32_t lx_pixmap_rgb32_blend2(lx_uint32_t d, lx_uint32_t hs, lx_uint32_t ls, lx_byte_t a) {
    lx_uint32_t hd = (d >> 8) & 0x00ff00ff;
    lx_uint32_t ld = d & 0x00ff00ff;
    hd = (((a * (hs - hd)) >> 8) + hd) & 0x00ff00ff;
    ld = (((a * (ls - ld)) >> 8) + ld) & 0x00ff00ff;
    return (hd << 8) | ld;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static lx_inline lx_pixel_t lx_pixmap_rgb32_pixel(lx_color_t color) {
    return lx_color_pixel(color);
}

static lx_inline lx_color_t lx_pixmap_rgb32_color(lx_pixel_t pixel) {
    return lx_pixel_color(pixel);
}

static lx_inline lx_pixel_t lx_pixmap_rgb32_pixel_get_l(lx_cpointer_t data) {
    return lx_bits_get_u32_le(data);
}

static lx_inline lx_pixel_t lx_pixmap_rgb32_pixel_get_b(lx_cpointer_t data) {
    return lx_bits_get_u32_be(data);
}

static lx_inline lx_void_t lx_pixmap_rgb32_pixel_set_lo(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u32_le(data, pixel);
}

static lx_inline lx_void_t lx_pixmap_rgb32_pixel_set_bo(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u32_be(data, pixel);
}

static lx_inline lx_void_t lx_pixmap_rgb32_pixel_copy_o(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    *((lx_uint32_t*)data) = *((lx_uint32_t*)source);
}

static lx_inline lx_void_t lx_pixmap_rgb32_pixels_fill_lo(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_memset32(data, lx_bits_ne_to_le_u32(pixel), count);
}

static lx_inline lx_void_t lx_pixmap_rgb32_pixels_fill_bo(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_memset32(data, lx_bits_ne_to_be_u32(pixel), count);
}

#endif
