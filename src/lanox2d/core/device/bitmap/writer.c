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
 * @file        writer.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "writer.h"
#include "writer/solid.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_bitmap_writer_init(lx_bitmap_writer_t* writer, lx_bitmap_ref_t bitmap, lx_paint_ref_t paint) {
    return lx_bitmap_writer_solid_init(writer, bitmap, paint);
}

lx_void_t lx_bitmap_writer_exit(lx_bitmap_writer_t* writer) {
    if (writer && writer->exit) {
        writer->exit(writer);
    }
}

lx_void_t lx_bitmap_writer_draw_pixel(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y) {
    lx_assert(writer && writer->draw_pixel);
    writer->draw_pixel(writer, x, y);
}

lx_void_t lx_bitmap_writer_draw_hline(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y, lx_long_t w) {
    lx_assert(writer && writer->draw_hline);
    writer->draw_hline(writer, x, y, w);
}

lx_void_t lx_bitmap_writer_draw_vline(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y, lx_long_t h) {
    lx_assert(writer && writer->draw_vline);
    writer->draw_vline(writer, x, y, h);
}

lx_void_t lx_bitmap_writer_draw_rect(lx_bitmap_writer_t* writer, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h) {
    lx_assert(writer);
    if (h == 1) {
        lx_assert(writer->draw_hline);
        writer->draw_hline(writer, x, y, w);
        return ;
    } else if (w == 1) {
        lx_assert(writer->draw_vline);
        writer->draw_vline(writer, x, y, h);
        return ;
    } else if (writer->draw_rect) {
        writer->draw_rect(writer, x, y, w, h);
    } else {
        lx_assert(writer->draw_hline);
        while (h--) writer->draw_hline(writer, x, y++, w);
    }
}
