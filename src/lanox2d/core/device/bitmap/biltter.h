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
 * @file        biltter.h
 */
#ifndef LX_CORE_DEVICE_BITMAP_BILLTER_H
#define LX_CORE_DEVICE_BITMAP_BILLTER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface
 */

/* init biltter
 *
 * @param biltter       the biltter
 * @param bitmap        the bitmap
 * @param paint         the paint
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_bitmap_biltter_init(lx_bitmap_biltter_t* biltter, lx_bitmap_ref_t bitmap, lx_paint_ref_t paint);

/* exit biltter
 *
 * @param biltter       the biltter
 */
lx_void_t               lx_bitmap_biltter_exit(lx_bitmap_biltter_t* biltter);

/* done biltter by pixel
 *
 * @param biltter       the biltter
 * @param x             the x-coordinate
 * @param y             the y-coordinate
 */
lx_void_t               lx_bitmap_biltter_draw_pixel(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y);

/* done biltter by horizontal
 *
 * @param biltter       the biltter
 * @param x             the start x-coordinate
 * @param y             the start y-coordinate
 * @param w             the width
 */
lx_void_t               lx_bitmap_biltter_draw_hline(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w);

/* done biltter by vertical
 *
 * @param biltter       the biltter
 * @param x             the start x-coordinate
 * @param y             the start y-coordinate
 * @param h             the height
 */
lx_void_t               lx_bitmap_biltter_draw_vline(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t h);

/* done biltter by rect
 *
 * @param biltter       the biltter
 * @param x             the start x-coordinate
 * @param y             the start y-coordinate
 * @param w             the width
 * @param h             the height
 */
lx_void_t               lx_bitmap_biltter_draw_rect(lx_bitmap_biltter_t* biltter, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


