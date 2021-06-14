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
 * @file        device.h
 *
 */
#ifndef LX_CORE_DEVICE_BITMAP_DEVICE_H
#define LX_CORE_DEVICE_BITMAP_DEVICE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "writter.h"
#include "polygon_raster.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

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


