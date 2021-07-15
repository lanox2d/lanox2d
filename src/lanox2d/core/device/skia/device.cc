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
#include "color.h"
#include "renderer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_device_skia_draw_clear(lx_device_ref_t self, lx_color_t color) {
    lx_skia_device_t* device = (lx_skia_device_t*)self;
    lx_assert(device && device->canvas);

    device->canvas->clear(lx_skia_color(color));
}

static lx_void_t lx_device_skia_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_skia_device_t* device = (lx_skia_device_t*)self;
    lx_assert(device && points && count);

    if (lx_skia_renderer_init(device)) {
        lx_skia_renderer_draw_lines(device, points, count, bounds);
        lx_skia_renderer_exit(device);
    }
}

static lx_void_t lx_device_skia_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_skia_device_t* device = (lx_skia_device_t*)self;
    lx_assert(device && points && count);

    if (lx_skia_renderer_init(device)) {
        lx_skia_renderer_draw_points(device, points, count, bounds);
        lx_skia_renderer_exit(device);
    }
}

static lx_void_t lx_device_skia_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_skia_device_t* device = (lx_skia_device_t*)self;
    lx_assert(device && polygon);

    if (lx_skia_renderer_init(device)) {
        lx_skia_renderer_draw_polygon(device, polygon, hint, bounds);
        lx_skia_renderer_exit(device);
    }
}

static lx_void_t lx_device_skia_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
    lx_skia_device_t* device = (lx_skia_device_t*)self;
    lx_assert(device && path);

    if (lx_skia_renderer_init(device)) {
        lx_skia_renderer_draw_path(device, path);
        lx_skia_renderer_exit(device);
    }
}

static lx_void_t lx_device_skia_exit(lx_device_ref_t self) {
    lx_skia_device_t* device = (lx_skia_device_t*)self;
    if (device) {
        if (device->path) {
            delete device->path;
            device->path = lx_null;
        }
        if (device->paint) {
            delete device->paint;
            device->paint = lx_null;
        }
        if (device->canvas) {
            delete device->canvas;
            device = lx_null;
        }
        if (device->surface) {
            delete device->surface;
            device->surface = lx_null;
        }
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_device_ref_t lx_device_init_from_skia(lx_window_ref_t window, lx_bitmap_ref_t bitmap) {
    lx_assert_and_check_return_val(window, lx_null);

    lx_bool_t         ok = lx_false;
    lx_skia_device_t* device = lx_null;
    do {

        // the width and height
        lx_size_t width  = lx_window_width(window);
        lx_size_t height = lx_window_height(window);
        lx_assert_and_check_break(width && height && width <= LX_WIDTH_MAX && height <= LX_HEIGHT_MAX);

        // init device
        device = lx_malloc0_type(lx_skia_device_t);
        lx_assert_and_check_break(device);

        device->base.draw_clear   = lx_device_skia_draw_clear;
        device->base.draw_lines   = lx_device_skia_draw_lines;
        device->base.draw_points  = lx_device_skia_draw_points;
        device->base.draw_polygon = lx_device_skia_draw_polygon;
        device->base.draw_path    = lx_device_skia_draw_path;
        device->base.exit         = lx_device_skia_exit;
        device->window            = window;

        // init bitmap surface
        if (bitmap) {
            lx_pointer_t data   = lx_bitmap_data(bitmap);
            lx_size_t width     = lx_bitmap_width(bitmap);
            lx_size_t height    = lx_bitmap_height(bitmap);
            lx_size_t row_bytes = lx_bitmap_row_bytes(bitmap);
            lx_size_t pixfmt    = lx_bitmap_pixfmt(bitmap);
            lx_assert_and_check_break(data && width && height && row_bytes);

            SkImageInfo const bitmap_info = SkImageInfo::Make((lx_int_t)width, (lx_int_t)height,
                lx_skia_color_type(pixfmt), kOpaque_SkAlphaType);
            device->surface = new SkBitmap();
            device->surface->setInfo(bitmap_info, (lx_int_t)row_bytes);
            device->surface->setPixels(data);
        } else {
            // TODO init skia surface
        }
        lx_assert_and_check_break(device->surface);

        // init canvas and paint
        device->canvas = new SkCanvas(*device->surface);
        device->paint = new SkPaint();
        device->path = new SkPath();
        lx_assert_and_check_break(device->canvas && device->paint && device->path);

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


