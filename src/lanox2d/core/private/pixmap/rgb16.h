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
 * @file        rgb16.h
 *
 */

#ifndef LX_CORE_PRIVATE_PIXMAP_RGB16_H
#define LX_CORE_PRIVATE_PIXMAP_RGB16_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static lx_inline lx_pixel_t lx_pixmap_rgb16_pixel_get_l(lx_cpointer_t data) {
    return lx_bits_get_u16_le(data);
}

static lx_inline lx_pixel_t lx_pixmap_rgb16_pixel_get_b(lx_cpointer_t data) {
    return lx_bits_get_u16_be(data);
}

static lx_inline lx_void_t lx_pixmap_rgb16_pixel_set_lo(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u16_le(data, pixel);
}

static lx_inline lx_void_t lx_pixmap_rgb16_pixel_set_bo(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    lx_bits_set_u16_be(data, pixel);
}

static lx_inline lx_void_t lx_pixmap_rgb16_pixel_copy_o(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    *((lx_uint16_t*)data) = *((lx_uint16_t*)source);
}

static lx_inline lx_void_t lx_pixmap_rgb16_pixels_fill_lo(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_memset16(data, lx_bits_ne_to_le_u16(pixel), count);
}

static lx_inline lx_void_t lx_pixmap_rgb16_pixels_fill_bo(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_memset16(data, lx_bits_ne_to_be_u16(pixel), count);
}


#endif
