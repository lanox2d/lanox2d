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
 * @file        decoder.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "decoder.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_bitmap_bmp_probe(lx_stream_ref_t stream) {
    lx_byte_t const* p = lx_null;
    if (lx_stream_peek(stream, &p, 2) && p) {
        return p[0] == 'B' && p[1] == 'M';
    }
    return lx_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_ref_t lx_bitmap_bmp_decode(lx_size_t pixfmt, lx_stream_ref_t stream) {
    lx_assert(stream);

    // decode image
    lx_bool_t       ok = lx_false;
    lx_bitmap_ref_t bitmap = lx_null;
    do {
        if (!lx_bitmap_bmp_probe(stream)) {
            break;
        }
        lx_trace_i("TODO");
        ok = lx_true;
    } while (0);

    // decode failed
    if (!ok) {
        if (bitmap) {
            lx_bitmap_exit(bitmap);
            bitmap = lx_null;
        }
    }
    return bitmap;
}

