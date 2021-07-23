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
//#define LX_TRACE_DISABLED
#include "decoder.h"
#include "../../quality.h"
#include "../../pixmap.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_bool_t lx_bitmap_png_probe(lx_stream_ref_t stream) {
    lx_byte_t const* p = lx_null;
    if (lx_stream_peek(stream, &p, 8) && p) {
        return (    p[0] == 0x89
                &&  p[1] == 'P'
                &&  p[2] == 'N'
                &&  p[3] == 'G'
                &&  p[4] == 0x0d
                &&  p[5] == 0x0a
                &&  p[6] == 0x1a
                &&  p[7] == 0x0a);
    }
    return lx_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_ref_t lx_bitmap_png_decode(lx_size_t pixfmt, lx_stream_ref_t stream) {
    lx_assert(stream);

    // decode image
    lx_bool_t       ok = lx_false;
    lx_bitmap_ref_t bitmap = lx_null;
    do {

        // probe format first
        if (!lx_bitmap_png_probe(stream)) {
            break;
        }

        lx_trace_i("decode png");

        // ok
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

