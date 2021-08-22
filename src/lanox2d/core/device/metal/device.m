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

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_device_metal_draw_clear(lx_device_ref_t self, lx_color_t color) {
}

static lx_void_t lx_device_metal_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_metal_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_metal_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_metal_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
}

static lx_void_t lx_device_metal_exit(lx_device_ref_t self) {
    lx_metal_device_t* device = (lx_metal_device_t*)self;
    if (device) {
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_device_ref_t lx_device_init_from_metal(lx_size_t width, lx_size_t height, lx_cpointer_t devdata) {
    lx_assert_and_check_return_val(width && height, lx_null);

    lx_bool_t          ok = lx_false;
    lx_metal_device_t* device = lx_null;
    do {

        // init device
        device = lx_malloc0_type(lx_metal_device_t);
        lx_assert_and_check_break(device);

        device->base.draw_clear   = lx_device_metal_draw_clear;
        device->base.draw_lines   = lx_device_metal_draw_lines;
        device->base.draw_points  = lx_device_metal_draw_points;
        device->base.draw_polygon = lx_device_metal_draw_polygon;
        device->base.draw_path    = lx_device_metal_draw_path;
        device->base.exit         = lx_device_metal_exit;

        // ok
        ok = lx_true;

    } while (0);

    // failed?
    if (!ok && device) {
        lx_device_exit((lx_device_ref_t)device);
        device = lx_null;
    }
    return (lx_device_ref_t)device;
}


