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
 * @file        prefix.h
 *
 */
#ifndef LX_CORE_DEVICE_BITMAP_PREFIX_H
#define LX_CORE_DEVICE_BITMAP_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../path.h"
#include "../../paint.h"
#include "../../private/stroker.h"
#include "../../private/polygon_raster.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the bitmap writter solid type
typedef struct lx_bitmap_writter_solid_t_ {
    lx_pixel_t                pixel;
    lx_byte_t                 alpha;
}lx_bitmap_writter_solid_t;

// the bitmap writter type
typedef struct lx_bitmap_writter_t_ {
    union {
        lx_bitmap_writter_solid_t    solid;
    }u;
    lx_bitmap_ref_t          bitmap;
    lx_pixmap_ref_t          pixmap;
    lx_size_t                btp;
    lx_size_t                row_bytes;
    lx_void_t                (*exit)(struct lx_bitmap_writter_t_* writter);
    lx_void_t                (*draw_pixel)(struct lx_bitmap_writter_t_* writter, lx_long_t x, lx_long_t y);
    lx_void_t                (*draw_hline)(struct lx_bitmap_writter_t_* writter, lx_long_t x, lx_long_t y, lx_long_t w);
    lx_void_t                (*draw_vline)(struct lx_bitmap_writter_t_* writter, lx_long_t x, lx_long_t y, lx_long_t h);
    lx_void_t                (*draw_rect)(struct lx_bitmap_writter_t_* writter, lx_long_t x, lx_long_t y, lx_long_t w, lx_long_t h);
}lx_bitmap_writter_t;

// the bitmap device type
typedef struct lx_bitmap_device_t_ {
    lx_device_t             base;
    lx_bitmap_ref_t         bitmap;
    lx_pixmap_ref_t         pixmap;
    lx_rect_t               bounds;
    lx_array_ref_t          points;
    lx_array_ref_t          counts;
    lx_polygon_raster_ref_t raster;
    lx_stroker_ref_t        stroker;
    lx_bitmap_writter_t     writter;
}lx_bitmap_device_t;

#endif


