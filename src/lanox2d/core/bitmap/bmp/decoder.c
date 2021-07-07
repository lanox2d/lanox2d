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
 * macros
 */

// the bpp offset
#define LX_BMP_OFFSET_BPP               (28)

// the bitmap size offset
#define LX_BMP_OFFSET_BITMAP_SIZE       (34)

// the palette offset
#define LX_BMP_OFFSET_PALETTE           (54)

// the bmp compression flag
#define LX_BMP_RGB                      (0)
#define LX_BMP_RLE8                     (1)
#define LX_BMP_RLE4                     (2)
#define LX_BMP_BITFIELDS                (3)

/* the bmp format
 *
 * @code
 *
 * typedef struct lx_bmp_t_ {
 *      lx_bmp_file_header_t    header;
 *      lx_bmp_info_t           info;
 * }lx_bitmap_t;
 *
 * typedef struct lx_bmp_file_header_t {
 *      lx_uint16_t             type;               //!< bmp_id == 0x4D42("BM")
 *      lx_uint32_t             size;
 *      lx_uint16_t             reserved1;
 *      lx_uint16_t             reserved2;
 *      lx_uint32_t             offset_bits;
 * }lx_bmp_file_header_t;
 *
 * typedef struct lx_bmp_info_t {
 *      lx_bmp_info_header_t    header;
 *      lx_bmp_rlx_t            colors[1];
 * }lx_bmp_info_t;
 *
 * typedef struct lx_bmp_info_header_t {
 *      lx_uint32_t             size;               // sizeof(lx_bmp_info_header_t)
 *      lx_int32_t              w;                  // width
 *      lx_int32_t              h;                  // height
 *      lx_uint16_t             planes;
 *      lx_uint16_t             bpp;                // bst per pixels
 *      lx_uint32_t             compression;        // LX_BMP_RGB, LX_BMP_RLE4, LX_BMP_RLE8, LX_BMP_BITFIELDS
 *      lx_uint32_t             bitmap_size;        // bitmap size for LX_BMP_RGB
 *      lx_int32_t              x_pels_per_meter;
 *      lx_int32_t              y_pels_per_meter;
 *      lx_uint32_t             clr_used;
 *      lx_uint32_t             clr_important;
 * }lx_bmp_info_header_t;
 *
 * typedef struct lx_bmp_rlx_t {
 *      lx_byte_t                b;
 *      lx_byte_t                g;
 *      lx_byte_t                r;
 *      lx_byte_t                reserved;
 * }lx_bmp_rlx_t;
 *
 * @endcode
 */

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

        // probe format first
        if (!lx_bitmap_bmp_probe(stream)) {
            break;
        }

        // read width and height
        if (!lx_stream_skip(stream, 18)) {
            break;
        }
        lx_uint32_t width  = lx_stream_peek_u4le(stream, 0);
        lx_uint32_t height = lx_stream_peek_u4le(stream, 4);
        lx_uint16_t bpp    = lx_stream_peek_u2le(stream, 10);
        lx_assert_and_check_break(width && height && width <= LX_WIDTH_MAX && height <= LX_HEIGHT_MAX);
        lx_assert_and_check_break(bpp && bpp <= 32);
        if (!lx_stream_skip(stream, 12)) {
            break;
        }

        // trace
        lx_trace_d("size: %ux%u, bpp: %u", width, height, bpp);

        // get compression
        lx_uint32_t bc = lx_stream_peek_u4le(stream, 0);
        lx_assert_and_check_break(bc != LX_BMP_RLE4 && bc != LX_BMP_RLE8);

        // read the data size
        lx_uint32_t datasize = lx_stream_peek_u4le(stream, 4);
        lx_size_t   linesize = (width * bpp) >> 3;
        lx_size_t   filesize = lx_stream_size(stream);
        if (!datasize) datasize = lx_align4(linesize) * height;
        lx_assert_and_check_break(datasize && datasize < filesize);
        if (!lx_stream_skip(stream, 8)) {
            break;
        }

        // trace
        lx_trace_d("data: %u bytes", datasize);

        // has palette?
        lx_color_t  pals[256];
        lx_size_t   paln = 1 << bpp;
        if (bpp <= 8) {

            // skip the palette count
            if (!lx_stream_skip(stream, 16)) {
                break;
            }

            // trace
            lx_trace_d("pal: %lu", paln);

            // read the palette entries
            lx_size_t i = 0;
            for (i = 0; i < paln; i++) {
                pals[i].b = lx_stream_peek_u1(stream, 0);
                pals[i].g = lx_stream_peek_u1(stream, 1);
                pals[i].r = lx_stream_peek_u1(stream, 2);
                if (!lx_stream_skip(stream, 4)) {
                    break;
                }
                pals[i].a = 0xff;
            }
            lx_assert_and_check_break(i == paln);
        }

        // bitfields?
        lx_pixmap_ref_t sp = lx_null;
        lx_pixmap_ref_t dp = lx_pixmap(pixfmt, 0xff);
        if (bc == LX_BMP_BITFIELDS) {

            // seek to the color mask position
            if (!lx_stream_skip(stream, 16)) {
                break;
            }

            // read r, g, b mask
            lx_uint32_t rm = lx_stream_peek_u4le(stream, 0);
            lx_uint32_t gm = lx_stream_peek_u4le(stream, 4);
            lx_uint32_t bm = lx_stream_peek_u4le(stream, 8);
            if (!lx_stream_skip(stream, 12)) {
                break;
            }

            if (bpp == 16) {
                if (rm == 0xf800 && gm == 0x07e0 && bm == 0x001f) {
                    sp = lx_pixmap(LX_PIXFMT_RGB565, 0xff);
                } else if (rm == 0x7c00 && gm == 0x03e0 && bm == 0x001f) {
                    sp = lx_pixmap(LX_PIXFMT_XRGB1555, 0xff);
                }
            } else if (bpp == 32) {
                if (rm == 0xff000000 && gm == 0xff0000 && bm == 0xff00) {
                    sp = lx_pixmap(LX_PIXFMT_RGBX8888, 0xff);
                } else if (rm == 0xff0000 && gm == 0xff00 && bm == 0xff) {
                    sp = lx_pixmap(LX_PIXFMT_XRGB8888, 0xff);
                }
            }
        } else if (bc == LX_BMP_RGB) { // rgb?
            switch (bpp) {
            case 32:
                sp = lx_pixmap(LX_PIXFMT_ARGB8888, 0xff);
                break;
            case 24:
                sp = lx_pixmap(LX_PIXFMT_RGB888, 0xff);
                break;
            case 16:
                sp = lx_pixmap(LX_PIXFMT_XRGB1555, 0xff);
                break;
            case 8:
            case 4:
            case 1:
                sp = lx_pixmap(LX_PIXFMT_PAL8, 0xff);
                break;
            default:
                lx_trace_e("the bpp: %lu is not supported", bpp);
                break;
            }
        }

        // check
        lx_assert_and_check_break(sp && dp);

        // trace
        lx_trace_d("pixfmt: %s => %s", sp->name, dp->name);

        // seek to the bmp data position
        if (!lx_stream_seek(stream, filesize - datasize)) {
            break;
        }

        // init bitmap, default: no alpha
        bitmap = lx_bitmap_init(lx_null, pixfmt, width, height, 0, lx_false);
        lx_assert_and_check_break(bitmap);

        // the bitmap data
        lx_byte_t* data = (lx_byte_t*)lx_bitmap_data(bitmap);
        lx_assert_and_check_break(data);

        // read bitmap data
        lx_color_t  c;
        lx_size_t   btp_dst = dp->btp;
        lx_size_t   btp_src = sp->btp;
        lx_size_t   has_alpha = 0;
        lx_size_t   row_bytes = lx_bitmap_row_bytes(bitmap);
        lx_size_t   row_bytes_align4 = lx_align4(linesize);
        lx_byte_t*  p = data + (height - 1) * row_bytes;
        if (bpp > 8) {
            while (height--) {
                lx_byte_t const* row_data = lx_null;
                if (row_bytes_align4 != lx_stream_peek(stream, &row_data, row_bytes_align4) && row_data) {
                    break;
                }
                if (!lx_stream_skip(stream, row_bytes_align4)) {
                    break;
                }
                lx_byte_t* d = p;
                lx_size_t  i = 0;
                for (i = 0; i < linesize; i += btp_src, d += btp_dst) {
                    c = sp->color_get(row_data + i);
                    dp->color_set(d, c);
                    if (!has_alpha) {
                        has_alpha = c.a <= LX_QUALITY_ALPHA_MAX;
                    }
                }
                p -= row_bytes;
            }
        } else if (bpp == 8) {
            while (height--) {
                lx_byte_t const* row_data = lx_null;
                if (row_bytes_align4 != lx_stream_peek(stream, &row_data, row_bytes_align4) && row_data) {
                    break;
                }
                if (!lx_stream_skip(stream, row_bytes_align4)) {
                    break;
                }
                lx_byte_t*  d = p;
                lx_size_t   i = 0;
                for (i = 0; i < linesize; i++, d += btp_dst) {
                    c = pals[row_data[i]];
                    dp->color_set(d, c);
                    if (!has_alpha) {
                        has_alpha = c.a <= LX_QUALITY_ALPHA_MAX;
                    }
                }
                p -= row_bytes;
            }
        } else { // bpp < 8?
            while (height--) {
                lx_byte_t const* row_data = lx_null;
                if (row_bytes_align4 != lx_stream_peek(stream, &row_data, row_bytes_align4) && row_data) {
                    break;
                }
                if (!lx_stream_skip(stream, row_bytes_align4)) {
                    break;
                }
                lx_byte_t*  d = p;
                lx_size_t   i = 0;
                lx_size_t   m = linesize << 3;
                for (i = 0; i < m; i += bpp, d += btp_dst) {
                    c = pals[lx_bits_get_ubits32(&row_data[i / 8], i & 7, bpp)];
                    dp->color_set(d, c);
                    if (!has_alpha) {
                        has_alpha = c.a <= LX_QUALITY_ALPHA_MAX;
                    }
                }
                p -= row_bytes;
            }
        }
        lx_assert_and_check_break(!(height + 1));

        // set alpha
        lx_bitmap_set_alpha(bitmap, (has_alpha && LX_PIXFMT_HAS_ALPHA(pixfmt))? lx_true : lx_false);

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

