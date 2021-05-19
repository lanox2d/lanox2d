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
 * @file        pixmap.c
 *
 */

#if 0
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pixmap.h"
#include "pixmap/pal8.h"
#include "pixmap/rgb565.h"
#include "pixmap/rgb888.h"
#include "pixmap/argb1555.h"
#include "pixmap/xrgb1555.h"
#include "pixmap/argb4444.h"
#include "pixmap/xrgb4444.h"
#include "pixmap/argb8888.h"
#include "pixmap/xrgb8888.h"
#include "pixmap/rgba5551.h"
#include "pixmap/rgbx5551.h"
#include "pixmap/rgba4444.h"
#include "pixmap/rgbx4444.h"
#include "pixmap/rgba8888.h"
#include "pixmap/rgbx8888.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the pixmaps for opaque and little endian
static lx_pixmap_ref_t g_pixmaps_lo[] = {
    &g_pixmap_o_pal8
,   &g_pixmap_lo_rgb565
,   &g_pixmap_lo_rgb888
,   &g_pixmap_lo_argb1555
,   &g_pixmap_lo_xrgb1555
,   &g_pixmap_lo_argb4444
,   &g_pixmap_lo_xrgb4444
,   &g_pixmap_lo_argb8888
,   &g_pixmap_lo_xrgb8888
,   &g_pixmap_lo_rgba5551
,   &g_pixmap_lo_rgbx5551
,   &g_pixmap_lo_rgba4444
,   &g_pixmap_lo_rgbx4444
,   &g_pixmap_lo_rgba8888
,   &g_pixmap_lo_rgbx8888
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
};

// the pixmaps for opaque and big endian
static lx_pixmap_ref_t g_pixmaps_bo[] = {
    &g_pixmap_o_pal8
,   &g_pixmap_bo_rgb565
,   &g_pixmap_bo_rgb888
,   &g_pixmap_bo_argb1555
,   &g_pixmap_bo_xrgb1555
,   &g_pixmap_bo_argb4444
,   &g_pixmap_bo_xrgb4444
,   &g_pixmap_bo_argb8888
,   &g_pixmap_bo_xrgb8888
,   &g_pixmap_bo_rgba5551
,   &g_pixmap_bo_rgbx5551
,   &g_pixmap_bo_rgba4444
,   &g_pixmap_bo_rgbx4444
,   &g_pixmap_bo_rgba8888
,   &g_pixmap_bo_rgbx8888
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
};

// the pixmaps for alpha and little endian
static lx_pixmap_ref_t g_pixmaps_la[] = {
    &g_pixmap_a_pal8
,   &g_pixmap_la_rgb565
,   &g_pixmap_la_rgb888
,   &g_pixmap_la_argb1555
,   &g_pixmap_la_xrgb1555
,   &g_pixmap_la_argb4444
,   &g_pixmap_la_xrgb4444
,   &g_pixmap_la_argb8888
,   &g_pixmap_la_xrgb8888
,   &g_pixmap_la_rgba5551
,   &g_pixmap_la_rgbx5551
,   &g_pixmap_la_rgba4444
,   &g_pixmap_la_rgbx4444
,   &g_pixmap_la_rgba8888
,   &g_pixmap_la_rgbx8888
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
};

// the pixmaps for alpha and big endian
static lx_pixmap_ref_t g_pixmaps_ba[] = {
    &g_pixmap_a_pal8
,   &g_pixmap_ba_rgb565
,   &g_pixmap_ba_rgb888
,   &g_pixmap_ba_argb1555
,   &g_pixmap_ba_xrgb1555
,   &g_pixmap_ba_argb4444
,   &g_pixmap_ba_xrgb4444
,   &g_pixmap_ba_argb8888
,   &g_pixmap_ba_xrgb8888
,   &g_pixmap_ba_rgba5551
,   &g_pixmap_ba_rgbx5551
,   &g_pixmap_ba_rgba4444
,   &g_pixmap_ba_rgbx4444
,   &g_pixmap_ba_rgba8888
,   &g_pixmap_ba_rgbx8888
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
,   lx_null
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementions
 */
lx_pixmap_ref_t lx_pixmap(lx_size_t pixfmt, lx_byte_t alpha) {

    // big endian?
    lx_size_t is_endian = LX_PIXFMT_BE(pixfmt);

    // the pixfmt
    pixfmt = LX_PIXFMT(pixfmt);
    lx_assert(pixfmt);

    // opaque?
    if (alpha > LX_ALPHA_MAXN) {
        lx_assert(pixfmt && (pixfmt - 1) < lx_arrayn(g_pixmaps_lo));
        return is_endian? g_pixmaps_bo[pixfmt - 1] : g_pixmaps_lo[pixfmt - 1];
    }
    // alpha?
    else if (alpha >= LX_ALPHA_MINN) {
        lx_assert(pixfmt && (pixfmt - 1) < lx_arrayn(g_pixmaps_la));
        return is_endian? g_pixmaps_ba[pixfmt - 1] : g_pixmaps_la[pixfmt - 1];
    }

    // transparent
    return lx_null;
}
#endif
