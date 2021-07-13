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
 * @file        device.cc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "device.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_device_skia_draw_clear(lx_device_ref_t self, lx_color_t color) {
}

static lx_void_t lx_device_skia_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_skia_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_skia_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
}

static lx_void_t lx_device_skia_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
}

static lx_void_t lx_device_skia_exit(lx_device_ref_t self) {
    lx_skia_device_t* device = (lx_skia_device_t*)self;
    if (device) {
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_device_ref_t lx_device_init_from_skia(lx_window_ref_t window, lx_bitmap_ref_t bitmap) {
    lx_assert_and_check_return_val(window, lx_null);

    lx_bool_t           ok = lx_false;
    lx_skia_device_t* device = lx_null;
    do {

        // the width and height
        lx_size_t width  = lx_window_width(window);
        lx_size_t height = lx_window_height(window);
        lx_assert_and_check_break(width && height && width <= LX_WIDTH_MAX && height <= LX_HEIGHT_MAX);

        // init device
        device = lx_malloc0_type(lx_skia_device_t);
        lx_assert_and_check_break(device);

        device->base.draw_clear            = lx_device_skia_draw_clear;
        device->base.draw_lines            = lx_device_skia_draw_lines;
        device->base.draw_points           = lx_device_skia_draw_points;
        device->base.draw_polygon          = lx_device_skia_draw_polygon;
        device->base.draw_path             = lx_device_skia_draw_path;
        device->base.exit                  = lx_device_skia_exit;
        device->window                     = window;

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


