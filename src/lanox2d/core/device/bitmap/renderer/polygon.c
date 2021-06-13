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
 * @file        polygon.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "lines.h"
#include "polygon.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#if 0
static lx_void_t lx_bitmap_renderer_fill_raster(lx_long_t lx, lx_long_t rx, lx_long_t yb, lx_long_t ye, lx_cpointer_t udata) {
    lx_assert(udata && rx >= lx && ye > yb);
    lx_bitmap_biltter_draw_rect((lx_bitmap_biltter_t*)udata, lx, yb, rx - lx, ye - yb);
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_bitmap_renderer_fill_polygon(lx_bitmap_device_t* device, lx_polygon_ref_t polygon, lx_rect_ref_t bounds) {
    lx_assert(device && device->base.paint);
//    lx_polygon_raster_done(device->raster, polygon, bounds, lx_paint_fill_rule(device->base.paint), lx_bitmap_renderer_fill_raster, &device->biltter);
}

lx_void_t lx_bitmap_renderer_stroke_polygon(lx_bitmap_device_t* device, lx_polygon_ref_t polygon) {
    lx_assert(device && polygon && polygon->points && polygon->counts);
    lx_uint16_t     index = 0;
    lx_point_t      points_line[2];
    lx_point_ref_t  points = polygon->points;
    lx_uint16_t*    counts = polygon->counts;
    lx_uint16_t     count = *counts++;
    while (index < count) {
        points_line[1] = *points++;
        if (index) {
            lx_bitmap_renderer_stroke_lines(device, points_line, 2);
        }
        points_line[0] = points_line[1];
        index++;
        if (index == count) { // next polygon
            count = *counts++;
            index = 0;
        }
    }
}
