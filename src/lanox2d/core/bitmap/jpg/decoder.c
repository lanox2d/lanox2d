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
#include <stdio.h>
#include <jpeglib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the jpeg source manger type
typedef struct lx_bitmap_jpg_jsrc_manger_t_ {
    struct jpeg_source_mgr          jsrc;
    lx_stream_ref_t                 stream;
}lx_bitmap_jpg_jsrc_manager_t;

// the jpeg error manager type
typedef struct lx_bitmap_jpg_jerr_manager_t {
    struct jpeg_error_mgr           jerr;
    lx_bool_t                       berr;
}lx_bitmap_jpg_jerr_manager_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static lx_void_t lx_bitmap_jpg_jsrc_init_source(j_decompress_ptr jdec) {
}

static boolean lx_bitmap_jpg_jsrc_fill_input_buffer(j_decompress_ptr jdec) {
    lx_bitmap_jpg_jsrc_manager_t* jsm = (lx_bitmap_jpg_jsrc_manager_t*)jdec->src;
    lx_assert_and_check_return_val(jsm && jsm->stream, lx_false);

    lx_hize_t left = lx_stream_left(jsm->stream);
    lx_size_t need = (lx_size_t)lx_min(left, LX_STREAM_BUFFER_MAXN);
    if (need == lx_stream_peek(jsm->stream, &jsm->jsrc.next_input_byte, need)) {
        lx_stream_skip(jsm->stream, need);
        jsm->jsrc.bytes_in_buffer = need;
        return lx_true;
    }
    return lx_false;
}

static lx_void_t lx_bitmap_jpg_jsrc_skip_input_data(j_decompress_ptr jdec, long num_bytes) {
    lx_bitmap_jpg_jsrc_manager_t* jsm = (lx_bitmap_jpg_jsrc_manager_t*)jdec->src;
    if (jsm && num_bytes > 0 && jsm->jsrc.next_input_byte && jsm->jsrc.bytes_in_buffer >= num_bytes) {
        jsm->jsrc.next_input_byte += num_bytes;
        jsm->jsrc.bytes_in_buffer -= num_bytes;
    }
}

static lx_void_t lx_bitmap_jpg_jsrc_term_source(j_decompress_ptr jdec) {
}

static lx_void_t lx_bitmap_jpg_jerr_exit(j_common_ptr jerr) {
    if (jerr) {
        lx_bitmap_jpg_jerr_manager_t* jem = (lx_bitmap_jpg_jerr_manager_t*)jerr->err;
        if (jem) jem->berr = lx_true;
    }
}

static lx_void_t lx_bitmap_jpg_jsrc_init(j_decompress_ptr jdec, lx_stream_ref_t stream) {
    if (!jdec->src) {
        jdec->src = (struct jpeg_source_mgr*)(*jdec->mem->alloc_small)((j_common_ptr)jdec, JPOOL_PERMANENT, sizeof(lx_bitmap_jpg_jsrc_manager_t));
    }

    lx_bitmap_jpg_jsrc_manager_t* jsm = (lx_bitmap_jpg_jsrc_manager_t*)jdec->src;
    jsm->jsrc.init_source             = lx_bitmap_jpg_jsrc_init_source;
    jsm->jsrc.fill_input_buffer       = lx_bitmap_jpg_jsrc_fill_input_buffer;
    jsm->jsrc.skip_input_data         = lx_bitmap_jpg_jsrc_skip_input_data;
    jsm->jsrc.resync_to_restart       = jpeg_resync_to_restart;
    jsm->jsrc.term_source             = lx_bitmap_jpg_jsrc_term_source;
    jsm->jsrc.next_input_byte         = lx_null;
    jsm->jsrc.bytes_in_buffer         = 0;
    jsm->stream                       = stream;
}

static lx_bool_t lx_bitmap_jpg_probe(lx_stream_ref_t stream) {
    lx_byte_t const* p = lx_null;
    if (lx_stream_peek(stream, &p, 4) && p) {
        return (    p[0] == 0xff
                &&  p[1] == 0xd8
                &&  p[2] == 0xff
                &&  (p[3] >= 0xe0 && p[3] <= 0xef));
    }
    return lx_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bitmap_ref_t lx_bitmap_jpg_decode(lx_size_t pixfmt, lx_stream_ref_t stream) {
    lx_assert(stream);

    // decode image
    lx_bool_t       ok = lx_false;
    lx_bitmap_ref_t bitmap = lx_null;
    lx_bitmap_jpg_jerr_manager_t jerr;
    struct jpeg_decompress_struct jdec;
    do {

        // probe format first
        if (!lx_bitmap_jpg_probe(stream)) {
            break;
        }

        // init jpeg manager
        jdec.err = jpeg_std_error((struct jpeg_error_mgr *)&jerr);
        jerr.jerr.error_exit = lx_bitmap_jpg_jerr_exit;
        jerr.berr = lx_false;

        // init jpeg decoder
        jpeg_create_decompress(&jdec);

        // init jpeg source
        lx_bitmap_jpg_jsrc_init(&jdec, stream);

        // read jpeg header
        jpeg_read_header(&jdec, lx_true);
        lx_assert_and_check_break(!jerr.berr);
        lx_assert_and_check_break(jdec.image_width && jdec.image_height);

        // init width & height
        lx_uint32_t width  = (lx_uint32_t)jdec.image_width;
        lx_uint32_t height = (lx_uint32_t)jdec.image_height;
        lx_trace_d("size: %ux%u", width, height);

        // get dp
        lx_pixmap_ref_t dp = lx_pixmap(pixfmt, 0xff);
        lx_assert_and_check_break(dp);

        // init bitmap, default: no alpha
        bitmap = lx_bitmap_init(lx_null, pixfmt, width, height, 0, lx_false);
        lx_assert_and_check_break(bitmap);

        // the bitmap data
        lx_byte_t* data = (lx_byte_t*)lx_bitmap_data(bitmap);
        lx_assert_and_check_break(data);

        // init pixfmt: rgb
        jdec.out_color_space = JCS_RGB;

        // decode it
        jpeg_start_decompress(&jdec);

        // init line buffer
        lx_size_t lsize = jdec.output_components * width;
        JSAMPROW  ldata = (JSAMPROW)jdec.mem->alloc_small((j_common_ptr)&jdec, JPOOL_IMAGE, lsize);
        lx_assert_and_check_break(ldata && lsize);

        // read lines
        lx_color_t  c;
        lx_size_t   b = dp->btp;
        lx_size_t   n = lx_bitmap_row_bytes(bitmap);
        lx_byte_t*  p = data;
        while (jdec.output_scanline < height) {
            jpeg_read_scanlines(&jdec, &ldata, 1);
            lx_size_t   i = 0;
            lx_byte_t*  d = p;
            for (i = 0; i < lsize; i += 3, d += b) {
                c.r = GETJSAMPLE(ldata[i + 0]);
                c.g = GETJSAMPLE(ldata[i + 1]);
                c.b = GETJSAMPLE(ldata[i + 2]);
                c.a = 0xff;
                dp->color_set(d, c);
            }
            p += n;
        }

        // finish it
        jpeg_finish_decompress(&jdec);

        // ok
        ok = lx_true;

    } while (0);

    // free decoder
    jpeg_destroy_decompress(&jdec);

    // decode failed
    if (!ok) {
        if (bitmap) {
            lx_bitmap_exit(bitmap);
            bitmap = lx_null;
        }
    }
    return bitmap;
}

