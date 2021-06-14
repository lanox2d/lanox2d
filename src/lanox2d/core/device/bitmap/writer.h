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
 * @file        writer.h
 */
#ifndef LX_CORE_DEVICE_BITMAP_WRITER_H
#define LX_CORE_DEVICE_BITMAP_WRITER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the bitmap writer solid type
typedef struct lx_bitmap_writer_solid_t_ {
    lx_pixel_t                pixel;
    lx_byte_t                 alpha;
}lx_bitmap_writer_solid_t;

// the bitmap writer type
typedef struct lx_bitmap_writer_t_ {
    union {
        lx_bitmap_writer_solid_t    solid;
    }u;
    lx_bitmap_ref_t          bitmap;
    lx_pixmap_ref_t          pixmap;
    lx_size_t                btp;
    lx_size_t                row_bytes;
    lx_void_t                (*exit)(struct lx_bitmap_writer_t_* writer);
    lx_void_t                (*draw_pixel)(struct lx_bitmap_writer_t_* writer, lx_long_t x, lx_long_t y);
    lx_void_t                (*draw_hline)(struct lx_bitmap_writer_t_* writer, lx_long_t x, lx_long_t y, lx_long_t w);
    lx_void_t                (*draw_vline)(struct lx_bitmap_writer_t_* writer, lx_long_t x, lx_long_t y, lx_long_t h);
    lx_void_t                (*draw_rect)(struct lx_bitmap_writer_t_* writer, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h);
}lx_bitmap_writer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interface
 */

/* init writer
 *
 * @param writer        the writer
 * @param bitmap        the bitmap
 * @param paint         the paint
 *
 * @return              lx_true or lx_false
 */
lx_bool_t               lx_bitmap_writer_init(lx_bitmap_writer_t* writer, lx_bitmap_ref_t bitmap, lx_paint_ref_t paint);

/* exit writer
 *
 * @param writer        the writer
 */
lx_void_t               lx_bitmap_writer_exit(lx_bitmap_writer_t* writer);

/* draw pixel
 *
 * @param writer        the writer
 * @param x             the x-coordinate
 * @param y             the y-coordinate
 */
lx_void_t               lx_bitmap_writer_draw_pixel(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y);

/* draw horizontal line
 *
 * @param writer        the writer
 * @param x             the start x-coordinate
 * @param y             the start y-coordinate
 * @param w             the width
 */
lx_void_t               lx_bitmap_writer_draw_hline(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y, lx_long_t w);

/* draw vertical line
 *
 * @param writer        the writer
 * @param x             the start x-coordinate
 * @param y             the start y-coordinate
 * @param h             the height
 */
lx_void_t               lx_bitmap_writer_draw_vline(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y, lx_long_t h);

/* draw rect
 *
 * @param writer        the writer
 * @param x             the start x-coordinate
 * @param y             the start y-coordinate
 * @param w             the width
 * @param h             the height
 */
lx_void_t               lx_bitmap_writer_draw_rect(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


