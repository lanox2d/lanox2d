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
#include "bmp/decoder.h"


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_ref_t lx_bitmap_decode(lx_size_t pixfmt, lx_stream_ref_t stream) {
    lx_assert(LX_PIXFMT_OK(pixfmt) && stream);
    static lx_bitmap_ref_t (*s_decode[])(lx_size_t, lx_stream_ref_t) = {
#ifdef LX_CONFIG_BITMAP_HAVE_BMP
        lx_bitmap_bmp_decode
#endif
    };
    lx_size_t i = 0;
    lx_bitmap_ref_t bitmap = lx_null;
    for (i = 0; i < lx_arrayn(s_decode); i++) {
        bitmap = s_decode[i](pixfmt, stream);
        if (bitmap) {
            break;
        }
    }
    return bitmap;
}

