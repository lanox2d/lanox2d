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
#include "bitmap.h"
#include "pixmap.h"
#include "bitmap/decoder.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the bitmap type
typedef struct lx_bitmap_t_ {
    lx_pointer_t        data;
    lx_size_t           size;
    lx_uint8_t          is_owner    : 1;
    lx_uint8_t          has_alpha   : 1;
    lx_uint16_t         pixfmt;
    lx_uint16_t         width;
    lx_uint16_t         height;
    lx_uint16_t         row_bytes;
}lx_bitmap_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_ref_t lx_bitmap_init(lx_pointer_t data, lx_size_t pixfmt, lx_size_t width, lx_size_t height, lx_size_t row_bytes, lx_bool_t has_alpha) {
    lx_bool_t      ok = lx_false;
    lx_bitmap_t*   bitmap = lx_null;
    do {
        // check
        lx_assert_and_check_break(LX_PIXFMT_OK(pixfmt));
        lx_assert_and_check_break(width && width <= LX_WIDTH_MAX && height && height <= LX_HEIGHT_MAX);

        // make bitmap
        bitmap = lx_malloc0_type(lx_bitmap_t);
        lx_assert_and_check_break(bitmap);

        // the row bytes
        lx_byte_t btp = lx_pixmap_btp(pixfmt);
        if (!row_bytes) row_bytes = width * btp;
        lx_assert_and_check_break(row_bytes && row_bytes >= width * btp);

        // init bitmap
        bitmap->pixfmt        = (lx_uint16_t)pixfmt;
        bitmap->width         = (lx_uint16_t)width;
        bitmap->height        = (lx_uint16_t)height;
        bitmap->row_bytes     = (lx_uint16_t)row_bytes;
        bitmap->size          = row_bytes * height;
        bitmap->data          = data? data : lx_malloc0(bitmap->size);
        bitmap->has_alpha     = (lx_uint8_t)has_alpha;
        bitmap->is_owner      = !data;
        lx_assert_and_check_break(bitmap->data);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && bitmap) {
        lx_bitmap_exit((lx_bitmap_ref_t)bitmap);
        bitmap = lx_null;
    }
    return (lx_bitmap_ref_t)bitmap;
}

lx_bitmap_ref_t lx_bitmap_init_from_file(lx_char_t const* path, lx_size_t pixfmt) {
    lx_assert_and_check_return_val(path && LX_PIXFMT_OK(pixfmt), lx_null);
    lx_bitmap_ref_t bitmap = lx_null;
    lx_stream_ref_t stream = lx_stream_init_file(path, "r");
    if (stream) {
        bitmap = lx_bitmap_decode(pixfmt, stream);
        lx_stream_exit(stream);
    }
    return bitmap;
}

lx_void_t lx_bitmap_exit(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    if (bitmap) {
        if (bitmap->is_owner && bitmap->data) {
            lx_free(bitmap->data);
        }
        bitmap->data = lx_null;
        lx_free(bitmap);
    }
}

lx_size_t lx_bitmap_size(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    return bitmap? bitmap->size : 0;
}

lx_pointer_t lx_bitmap_data(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    return bitmap? bitmap->data : lx_null;
}

lx_bool_t lx_bitmap_attach(lx_bitmap_ref_t self, lx_pointer_t data, lx_size_t width, lx_size_t height, lx_size_t row_bytes) {

    // check
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    lx_assert_and_check_return_val(bitmap && !bitmap->is_owner && data, lx_false);

    lx_bool_t ok = lx_false;
    do {

        // get the row bytes
        if (!row_bytes) {
            lx_byte_t btp = lx_pixmap_btp(bitmap->pixfmt);
            row_bytes = width * btp;
            lx_assert_and_check_break(row_bytes && row_bytes >= width * btp);
        }

        // attach bitmap data
        bitmap->width         = (lx_uint16_t)width;
        bitmap->height        = (lx_uint16_t)height;
        bitmap->data          = data;
        bitmap->size          = row_bytes * height;
        bitmap->row_bytes     = (lx_uint16_t)row_bytes;

        // ok
        ok = lx_true;

    } while (0);
    return ok;
}

lx_bool_t lx_bitmap_resize(lx_bitmap_ref_t self, lx_size_t width, lx_size_t height) {

    // check
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    lx_assert_and_check_return_val(bitmap && bitmap->data, lx_false);

    // same?
    lx_check_return_val(bitmap->width != width || bitmap->height != height, lx_true);

    // space enough?
    lx_byte_t btp = lx_pixmap_btp(bitmap->pixfmt);
    if (height * width * btp <= bitmap->size) {
        bitmap->width     = (lx_uint16_t)width;
        bitmap->height    = (lx_uint16_t)height;
        if (bitmap->is_owner) bitmap->row_bytes = (lx_uint16_t)(width * btp);
        bitmap->size      = bitmap->row_bytes * height;
    } else {

        // must be owner
        lx_assert(bitmap->is_owner);
        lx_check_return_val(bitmap->is_owner, lx_false);

        // resize
        bitmap->width     = (lx_uint16_t)width;
        bitmap->height    = (lx_uint16_t)height;
        bitmap->row_bytes = (lx_uint16_t)(width * btp);
        bitmap->size      = bitmap->row_bytes * height;
        bitmap->data      = lx_ralloc(bitmap->data, bitmap->size);
        lx_assert_and_check_return_val(bitmap->data, lx_false);
    }
    return lx_true;
}

lx_size_t lx_bitmap_width(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    return bitmap? bitmap->width : 0;
}

lx_size_t lx_bitmap_height(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    return bitmap? bitmap->height : 0;
}

lx_size_t lx_bitmap_pixfmt(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    return bitmap? bitmap->pixfmt : LX_PIXFMT_NONE;
}

lx_bool_t lx_bitmap_has_alpha(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    return bitmap && bitmap->has_alpha? lx_true : lx_false;
}

lx_void_t lx_bitmap_set_alpha(lx_bitmap_ref_t self, lx_bool_t has_alpha) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    if (bitmap) {
        bitmap->has_alpha = has_alpha;
    }
}

lx_size_t lx_bitmap_row_bytes(lx_bitmap_ref_t self) {
    lx_bitmap_t* bitmap = (lx_bitmap_t*)self;
    return bitmap? bitmap->row_bytes : 0;
}
