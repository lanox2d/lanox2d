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
#include "renderer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef LX_CONFIG_SMALL
#   define LX_DEVICE_BITMAP_POINTS_GROW      (64)
#else
#   define LX_DEVICE_BITMAP_POINTS_GROW      (128)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static lx_void_t lx_device_bitmap_draw_clear(lx_device_ref_t self, lx_color_t color) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    lx_assert(device && device->bitmap);

    // get the bitmap data
    lx_byte_t* data = (lx_byte_t*)lx_bitmap_data(device->bitmap);
    lx_assert(data);

    // get pixmap
    lx_pixmap_ref_t pixmap = device->pixmap;
    lx_assert(pixmap && pixmap->pixel && pixmap->pixels_fill);

    // clear all pixels
    lx_size_t  width     = lx_bitmap_width(device->bitmap);
    lx_size_t  height    = lx_bitmap_height(device->bitmap);
    lx_size_t  row_bytes = lx_bitmap_row_bytes(device->bitmap);
    lx_pixel_t pixel     = pixmap->pixel(color);
    if (width * pixmap->btp == row_bytes) {
        pixmap->pixels_fill(data, pixel, width * height, 0xff);
    } else {
        lx_size_t y;
        for (y = 0; y < height; y++) {
            pixmap->pixels_fill(data + y * row_bytes, pixel, width, 0xff);
        }
    }
}

static lx_void_t lx_device_bitmap_draw_lines(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    lx_assert(device && points && count);

    if (lx_bitmap_renderer_init(device)) {
        lx_bitmap_renderer_draw_lines(device, points, count, bounds);
        lx_bitmap_renderer_exit(device);
    }
}

static lx_void_t lx_device_bitmap_draw_points(lx_device_ref_t self, lx_point_ref_t points, lx_size_t count, lx_rect_ref_t bounds) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    lx_assert(device && points && count);

    if (lx_bitmap_renderer_init(device)) {
        lx_bitmap_renderer_draw_points(device, points, count, bounds);
        lx_bitmap_renderer_exit(device);
    }
}

static lx_void_t lx_device_bitmap_draw_polygon(lx_device_ref_t self, lx_polygon_ref_t polygon, lx_shape_ref_t hint, lx_rect_ref_t bounds) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    lx_assert(device && polygon);

    if (lx_bitmap_renderer_init(device)) {
        lx_bitmap_renderer_draw_polygon(device, polygon, hint, bounds);
        lx_bitmap_renderer_exit(device);
    }
}

static lx_void_t lx_device_bitmap_draw_path(lx_device_ref_t self, lx_path_ref_t path) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    lx_assert(device && path);

    if (lx_bitmap_renderer_init(device)) {
        lx_bitmap_renderer_draw_path(device, path);
        lx_bitmap_renderer_exit(device);
    }
}

static lx_void_t lx_device_bitmap_exit(lx_device_ref_t self) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    if (device) {
        if (device->points) {
            lx_array_exit(device->points);
            device->points = lx_null;
        }
        if (device->counts) {
            lx_array_exit(device->counts);
            device->counts = lx_null;
        }
        if (device->stroker) {
            lx_stroker_exit(device->stroker);
            device->stroker = lx_null;
        }
        if (device->raster) {
            lx_polygon_raster_exit(device->raster);
            device->raster = lx_null;
        }
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
lx_device_ref_t lx_device_init_from_bitmap(lx_bitmap_ref_t bitmap) {
    lx_assert_and_check_return_val(bitmap, lx_null);

    lx_bool_t           ok = lx_false;
    lx_bitmap_device_t* device = lx_null;
    do {

        // the width and height
        lx_size_t width     = lx_bitmap_width(bitmap);
        lx_size_t height    = lx_bitmap_height(bitmap);
        lx_assert_and_check_break(width && height && width <= LX_WIDTH_MAX && height <= LX_HEIGHT_MAX);

        // init device
        device = lx_malloc0_type(lx_bitmap_device_t);
        lx_assert_and_check_break(device);

        device->base.draw_clear   = lx_device_bitmap_draw_clear;
        device->base.draw_lines   = lx_device_bitmap_draw_lines;
        device->base.draw_points  = lx_device_bitmap_draw_points;
        device->base.draw_polygon = lx_device_bitmap_draw_polygon;
        device->base.draw_path    = lx_device_bitmap_draw_path;
        device->base.exit         = lx_device_bitmap_exit;
        device->bitmap            = bitmap;

        // init pixmap
        device->pixmap = lx_pixmap(lx_bitmap_pixfmt(bitmap), 0xff);
        lx_assert_and_check_break(device->pixmap);

        // init raster
        device->raster = lx_polygon_raster_init();
        lx_assert_and_check_break(device->raster);

        // init stroker
        device->stroker = lx_stroker_init();
        lx_assert_and_check_break(device->stroker);

        // init points
        device->points = lx_array_init(LX_DEVICE_BITMAP_POINTS_GROW, lx_element_mem(sizeof(lx_point_t), lx_null));
        lx_assert_and_check_break(device->points);

        // init counts
        device->counts = lx_array_init(8, lx_element_mem(sizeof(lx_uint16_t), lx_null));
        lx_assert_and_check_break(device->counts);

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

