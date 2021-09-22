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
 * @file        decoder_libpng.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
//#define LX_TRACE_DISABLED
#include "decoder.h"
#include "../../quality.h"
#include "../../pixmap.h"
#ifdef LX_CONFIG_OS_ANDROID
#   include "libpng_dynamic.h"
#else
#   define PNG_SKIP_SETJMP_CHECK
#   include <png.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_void_t lx_bitmap_png_error(png_structp png, png_const_charp msg) {
    lx_trace_e("error: %s", msg);
    longjmp(png_jmpbuf(png), 1);
}

static lx_void_t lx_bitmap_png_reader(png_structp png, png_bytep data, png_size_t size) {
    lx_assert(png && data);
    lx_stream_ref_t stream = (lx_stream_ref_t)png_get_io_ptr(png);
    if (stream) {
        lx_byte_t const* p = lx_null;
        if (size == (png_size_t)lx_stream_peek(stream, &p, (lx_size_t)size) && p) {
            lx_memcpy(data, p, size);
            lx_stream_skip(stream, (lx_size_t)size);
        }
    }
}

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
    png_structp     png = lx_null;
    png_infop       info = lx_null;
    lx_byte_t*      ldata = lx_null;
    do {

        // probe format first
        if (!lx_bitmap_png_probe(stream)) {
            break;
        }

        // init png
        png = png_create_read_struct(PNG_LIBPNG_VER_STRING, lx_null, lx_bitmap_png_error, lx_null);
        lx_assert_and_check_break(png);

        // init info
        info = png_create_info_struct(png);
        lx_assert_and_check_break(info);

        // init reader
        png_set_read_fn(png, (lx_pointer_t)stream, lx_bitmap_png_reader);

        // read info
        png_read_info(png, info);

        // the header info
        png_uint_32 width = 0;
        png_uint_32 height = 0;
        lx_int_t bpp = 0;
        lx_int_t color_type = 0;
        lx_int_t interlace_type = 0;
        png_get_IHDR(png, info, &width, &height, &bpp, &color_type, &interlace_type, lx_null, lx_null);
        lx_trace_d("size: %ux%u, bpp: %u", width, height, bpp);
        lx_assert_and_check_break(width && height && bpp);

        // TODO: not support now.
        lx_assert_and_check_break(color_type != PNG_COLOR_TYPE_PALETTE);

        // get pixmap
        lx_pixmap_ref_t dp = lx_pixmap(pixfmt, 0xff);
        lx_pixmap_ref_t sp = lx_pixmap(LX_PIXFMT_ARGB8888, 0xff);
        lx_assert_and_check_break(dp && sp);

        // init bitmap, default: no alpha
        bitmap = lx_bitmap_init(lx_null, pixfmt, width, height, 0, lx_false);
        lx_assert_and_check_break(bitmap);

        // the bitmap data
        lx_byte_t* data = (lx_byte_t*)lx_bitmap_data(bitmap);
        lx_assert_and_check_break(data);

        /* set error handling if you are using the setjmp/longjmp method (this is
         * the normal method of doing things with libpng).  required unless you
         * set up your own error handlers in the png_create_read_struct() earlier.
         */
        if (setjmp(png_jmpbuf(png))) {
            break;
        }

        // tell libpng to strip 16 bit/color files down to 8 bits/color
        if (bpp == 16) {
            png_set_strip_16(png);
        }

        /* extract multiple pixels with bit depths of 1, 2, and 4 from a single
         * byte into separate bytes (useful for paletted and grayscale images)
         */
        if (bpp < 8) {
            png_set_packing(png);
        }

        // expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
        if (color_type == PNG_COLOR_TYPE_GRAY && bpp < 8) {
            png_set_expand_gray_1_2_4_to_8(png);
        }

        // make a grayscale image into rgb.
        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            png_set_gray_to_rgb(png);
        }

        // get the transparent color
        png_color_16p   trans_c = lx_null;
        lx_int_t        trans_n = 0;
        png_get_tRNS(png, info, lx_null, &trans_n, &trans_c);
        lx_trace_d("transparent: #%x%x%x, count: %d", trans_c? trans_c->red : 0, trans_c? trans_c->green : 0, trans_c? trans_c->blue : 0, trans_n);

        /* expand paletted or rgb images with transparency to full alpha channels
         * so the data will be available as RGBA quartets.
         */
        if (png_get_valid(png, info, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png);
        }

        // flip the rgb pixels to bgr (or rgba to bgra)
        if (color_type & PNG_COLOR_MASK_COLOR) {
            png_set_bgr(png);
        }

        // swap the rgba or ga data to argb or ag (or bgra to abgr)
//        png_set_swap_alpha(png);

        // add filler (or alpha) byte (before/after each rgb triplet)
        if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) {
            png_set_filler(png, 0xff, PNG_FILLER_AFTER);
        }

        /* turn on interlace handling. required if you are not using
         * png_read_image(). to see how to handle interlacing passes,
         * see the png_read_row() method below:
         */
        lx_long_t number_passes = interlace_type != PNG_INTERLACE_NONE ? png_set_interlace_handling(png) : 1;
        lx_trace_d("number_passes: %ld", number_passes);

        /* optional call to gamma correct and add the background to the palette
         * and update info structure.  required if you are expecting libpng to
         * update the palette for you (ie you selected such a transform above).
         */
        png_read_update_info(png, info);

        // init line buffer
        lx_size_t lsize = png_get_rowbytes(png, info);
        ldata = (lx_byte_t*)lx_malloc0(lsize);
        lx_assert_and_check_break(ldata && lsize);

        // decode image data
        lx_size_t k = 0;
        lx_bool_t has_alpha = lx_false;
        for (k = 0; k < number_passes; k++) {
            lx_size_t   j;
            lx_size_t   b = dp->btp;
            lx_size_t   n = lx_bitmap_row_bytes(bitmap);
            lx_byte_t*  p = data;
            for (j = 0; j < height; j++) {
                png_read_rows(png, &ldata, lx_null, 1);
                if (k == number_passes - 1) {
                    lx_size_t   i = 0;
                    lx_byte_t*  d = p;
                    lx_byte_t*  e = p + n;
                    if (dp == sp) {
                        for (i = 0; i < lsize && d < e; i += 4, d += b) {
                            dp->pixel_copy(d, &ldata[i], 0xff);
                            if (!has_alpha) {
                                has_alpha = ldata[i + 3] <= LX_QUALITY_ALPHA_MAX;
                            }
                        }
                    } else {
                        for (i = 0; i < lsize && d < e; i += 4, d += b) {
                            dp->color_set(d, sp->color_get(&ldata[i]));
                            if (!has_alpha) {
                                has_alpha = ldata[i + 3] <= LX_QUALITY_ALPHA_MAX;
                            }
                        }
                    }
                    p += n;
                }
            }
        }

        // set alpha
        lx_bitmap_set_alpha(bitmap, (has_alpha && LX_PIXFMT_HAS_ALPHA(pixfmt))? lx_true : lx_false);

        // ok
        ok = lx_true;

    } while (0);

    // free line buffer
    if (ldata) {
        lx_free(ldata);
        ldata = lx_null;
    }

    // free png data
    if (png) {
        png_destroy_read_struct(&png, &info, lx_null);
    }

    // decode failed
    if (!ok) {
        if (bitmap) {
            lx_bitmap_exit(bitmap);
            bitmap = lx_null;
        }
    }
    return bitmap;
}

