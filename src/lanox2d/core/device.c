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
 * @file        device.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"
#include "device/prefix.h"
#include "path.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

lx_void_t lx_device_exit(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    if (device && device->exit) {
        device->exit(self);
    }
}

lx_size_t lx_device_width(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    return device? device->width : 0;
}

lx_size_t lx_device_height(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    return device? device->height : 0;
}

lx_void_t lx_device_bind_paint(lx_device_ref_t self, lx_paint_ref_t paint) {
    lx_device_t* device = (lx_device_t*)self;
    if (device) {
        device->paint = paint;
    }
}

lx_void_t lx_device_bind_matrix(lx_device_ref_t self, lx_matrix_ref_t matrix) {
    lx_device_t* device = (lx_device_t*)self;
    if (device) {
        device->matrix = matrix;
    }
}

lx_void_t lx_device_bind_clipper(lx_device_ref_t self, lx_clipper_ref_t clipper) {
    lx_device_t* device = (lx_device_t*)self;
    if (device) {
        device->clipper = clipper;
    }
}

lx_bool_t lx_device_draw_lock(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    lx_assert(device && device->draw_lock);
    return device->draw_lock(self);
}

lx_void_t lx_device_draw_commit(lx_device_ref_t self) {
    lx_device_t* device = (lx_device_t*)self;
    lx_assert(device && device->draw_commit);
    device->draw_commit(self);
}

lx_void_t lx_device_draw_clear(lx_device_ref_t self, lx_color_t color) {
    lx_device_t* device = (lx_device_t*)self;
    lx_assert(device && device->draw_clear);
    device->draw_clear(self, color);
}

lx_void_t lx_device_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
    lx_device_t* device = (lx_device_t*)self;
    lx_assert_and_check_return(device);

    if (!lx_path_empty(path)) {
        if (device->draw_path) {
            device->draw_path(self, path);
        } else {
            /* draw the polygon for path
             *
             * @note the quality of drawing curve may be not higher and faster for stroking with the width > 1
             */
            lx_device_draw_polygon(self, lx_path_polygon(path), lx_path_hint(path), lx_path_bounds(path));
        }
    }
}

lx_void_t lx_device_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_device_t* device = (lx_device_t*)self;
    lx_assert(device && device->draw_lines);
    device->draw_lines(self, points, count, bounds);
}

lx_void_t lx_device_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_device_t* device = (lx_device_t*)self;
    lx_assert(device && device->draw_points);
    device->draw_points(self, points, count, bounds);
}

lx_void_t lx_device_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_device_t* device = (lx_device_t*)self;
    lx_assert(device && device->draw_polygon);
    device->draw_polygon(self, polygon, hint, bounds);
}
