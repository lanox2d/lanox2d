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
 * @file        pal8.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

static lx_pixel_t lx_pixmap_pal8_pixel(lx_color_t color) {
    lx_trace_noimpl();
    return 0;
}

static lx_color_t lx_pixmap_pal8_color(lx_pixel_t pixel) {
    lx_trace_noimpl();
    return LX_COLOR_DEFAULT;
}

static lx_pixel_t lx_pixmap_pal8_pixel_get(lx_cpointer_t data) {
    return ((lx_byte_t*)data)[0];
}

static lx_void_t lx_pixmap_pal8_pixel_set_o(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    ((lx_byte_t*)data)[0] = (lx_byte_t)(pixel & 0xff);
}

static lx_void_t lx_pixmap_pal8_pixel_set_a(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha) {
    ((lx_byte_t*)data)[0] = (lx_byte_t)(pixel & 0xff);
}

static lx_void_t lx_pixmap_pal8_pixel_cpy_o(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    ((lx_byte_t*)data)[0] = ((lx_byte_t*)source)[0];
}

static lx_void_t lx_pixmap_pal8_pixel_cpy_a(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha) {
    ((lx_byte_t*)data)[0] = ((lx_byte_t*)source)[0];
}

static lx_color_t lx_pixmap_pal8_color_get(lx_cpointer_t data) {
    lx_trace_noimpl();
    return LX_COLOR_DEFAULT;
}

static lx_void_t lx_pixmap_pal8_color_set_o(lx_pointer_t data, lx_color_t color) {
    lx_trace_noimpl();
}

static lx_void_t lx_pixmap_pal8_color_set_a(lx_pointer_t data, lx_color_t color) {
    lx_trace_noimpl();
}

static lx_void_t lx_pixmap_pal8_pixels_fill_o(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_memset(data, (lx_byte_t)pixel, count);
}

static lx_void_t lx_pixmap_pal8_pixels_fill_a(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha) {
    lx_memset(data, (lx_byte_t)pixel, count);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

static lx_pixmap_t const g_pixmap_o_pal8 = {
    "pal8"
,   8
,   1
,   LX_PIXFMT_PAL8
,   lx_pixmap_pal8_pixel
,   lx_pixmap_pal8_color
,   lx_pixmap_pal8_pixel_get
,   lx_pixmap_pal8_pixel_set_o
,   lx_pixmap_pal8_pixel_cpy_o
,   lx_pixmap_pal8_color_get
,   lx_pixmap_pal8_color_set_o
,   lx_pixmap_pal8_pixels_fill_o
};

static lx_pixmap_t const g_pixmap_a_pal8 = {
    "pal8"
,   8
,   1
,   LX_PIXFMT_PAL8
,   lx_pixmap_pal8_pixel
,   lx_pixmap_pal8_color
,   lx_pixmap_pal8_pixel_get
,   lx_pixmap_pal8_pixel_set_a
,   lx_pixmap_pal8_pixel_cpy_a
,   lx_pixmap_pal8_color_get
,   lx_pixmap_pal8_color_set_a
,   lx_pixmap_pal8_pixels_fill_a
};


