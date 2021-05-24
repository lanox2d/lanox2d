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
 * @file        bitmap.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the bitmap device type
typedef struct lx_bitmap_device_t_ {
    lx_device_t     base;
    lx_bitmap_ref_t bitmap;
    lx_pixmap_ref_t pixmap;
}lx_bitmap_device_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static lx_void_t lx_device_bitmap_resize(lx_device_ref_t self, lx_size_t width, lx_size_t height) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    if (device && device->bitmap) {
        lx_bitmap_resize(device->bitmap, width, height);
    }
}

static lx_void_t lx_device_bitmap_draw_clear(lx_device_ref_t self, lx_color_t color) {

    // check
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    lx_assert_and_check_return(device && device->bitmap);

    // get the bitmap data
    lx_byte_t* data = lx_bitmap_data(device->bitmap);
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

static lx_void_t lx_device_bitmap_exit(lx_device_ref_t self) {
    lx_bitmap_device_t* device = (lx_bitmap_device_t*)self;
    if (device) {
        lx_free(device);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
lx_device_ref_t lx_device_init_from_bitmap(lx_bitmap_ref_t bitmap) {

    // check
    lx_assert_and_check_return_val(bitmap, lx_null);

    // done
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

        device->base.resize     = lx_device_bitmap_resize;
        device->base.draw_clear = lx_device_bitmap_draw_clear;
        device->base.exit       = lx_device_bitmap_exit;
        device->bitmap          = bitmap;

        // init pixmap
        device->pixmap = lx_pixmap(lx_bitmap_pixfmt(bitmap), 0xff);
        lx_assert_and_check_break(device->pixmap);

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

