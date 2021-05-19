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
 * @file        color.h
 *
 */
#ifndef LX_CORE_BASICTYPE_COLOR_H
#define LX_CORE_BASICTYPE_COLOR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the color contants
#define LX_COLOR_BLACK                  lx_color_from_index(0)
#define LX_COLOR_BLUE                   lx_color_from_index(1)
#define LX_COLOR_BROWN                  lx_color_from_index(2)
#define LX_COLOR_DARKBLUE               lx_color_from_index(3)
#define LX_COLOR_GOLD                   lx_color_from_index(4)
#define LX_COLOR_GRAY                   lx_color_from_index(5)
#define LX_COLOR_GREEN                  lx_color_from_index(6)
#define LX_COLOR_GREY                   lx_color_from_index(7)
#define LX_COLOR_LIGHTBLUE              lx_color_from_index(8)
#define LX_COLOR_LIGHTGRAY              lx_color_from_index(9)
#define LX_COLOR_LIGHTGREY              lx_color_from_index(10)
#define LX_COLOR_LIGHTPINK              lx_color_from_index(11)
#define LX_COLOR_LIGHTYELLOW            lx_color_from_index(12)
#define LX_COLOR_NAVY                   lx_color_from_index(13)
#define LX_COLOR_ORANGE                 lx_color_from_index(14)
#define LX_COLOR_PINK                   lx_color_from_index(15)
#define LX_COLOR_PURPLE                 lx_color_from_index(16)
#define LX_COLOR_RED                    lx_color_from_index(17)
#define LX_COLOR_SNOW                   lx_color_from_index(18)
#define LX_COLOR_TOMATO                 lx_color_from_index(19)
#define LX_COLOR_YELLOW                 lx_color_from_index(20)
#define LX_COLOR_WHEAT                  lx_color_from_index(21)
#define LX_COLOR_WHITE                  lx_color_from_index(22)
#define LX_COLOR_DEFAULT                LX_COLOR_LIGHTGRAY

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! make color
 *
 * @param a                         the alpha
 * @param r                         the red
 * @param g                         the green
 * @param b                         the blue
 *
 * @return                          the color
 */
static lx_inline lx_color_t lx_color_make(lx_byte_t a, lx_byte_t r, lx_byte_t g, lx_byte_t b) {
    lx_color_t c;
    c.a = a;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}

/*! convert color to the pixel
 *
 * @param color                     the color
 *
 * @return                          the pixel value
 */
static lx_inline lx_uint32_t lx_color_pixel(lx_color_t color) {
    union lx_c2p_t_ {
        lx_color_t c;
        lx_pixel_t p;
    }c2p;
    c2p.c = color;
    return c2p.p;
}

/*! convert pixel to the color
 *
 * @param pixel                     the pixel value
 *
 * @return                          the color
 */
static lx_inline lx_color_t lx_pixel_color(lx_pixel_t pixel) {
    union lx_p2c_t_ {
        lx_color_t c;
        lx_pixel_t p;
    }p2c;
    p2c.p = pixel;
    return p2c.c;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get color from the index
 *
 * @param index     the color index
 *
 * @return          the color
 */
lx_color_t const    lx_color_from_index(lx_size_t index);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


