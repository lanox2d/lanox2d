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
 * @file        points.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "points.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_bitmap_renderer_stroke_points(lx_bitmap_device_t* device, lx_point_ref_t points, lx_size_t count) {
    lx_assert(device && points && count);
    lx_size_t i;
    for (i = 0; i < count; i++) {
        lx_bitmap_writer_draw_pixel(&device->writer, (lx_long_t)points[i].x, (lx_long_t)points[i].y);
    }
}
