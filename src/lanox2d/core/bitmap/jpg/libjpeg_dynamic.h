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
 * @file        libjpeg_dynamic.h
 *
 */
#ifndef LX_CORE_BITMAP_JPEG_LIBJPEG_DYNAMIC_H
#define LX_CORE_BITMAP_JPEG_LIBJPEG_DYNAMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdio.h>
#include <jpeglib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#undef jpeg_CreateDecompress
#undef jpeg_destroy_decompress
#undef jpeg_finish_decompress
#undef jpeg_read_header
#undef jpeg_read_scanlines
#undef jpeg_resync_to_restart
#undef jpeg_start_decompress
#undef jpeg_std_error
#define jpeg_CreateDecompress   jpeg_CreateDecompress_
#define jpeg_destroy_decompress jpeg_destroy_decompress_
#define jpeg_finish_decompress  jpeg_finish_decompress_
#define jpeg_read_header        jpeg_read_header_
#define jpeg_read_scanlines     jpeg_read_scanlines_
#define jpeg_resync_to_restart  jpeg_resync_to_restart_
#define jpeg_start_decompress   jpeg_start_decompress_
#define jpeg_std_error          jpeg_std_error_

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef void                    (*jpeg_CreateDecompress_t)(j_decompress_ptr cinfo, int version, size_t structsize);
typedef void                    (*jpeg_destroy_decompress_t)(j_decompress_ptr cinfo);
typedef void                    (*jpeg_finish_decompress_t)(j_decompress_ptr cinfo);
typedef int                     (*jpeg_read_header_t)(j_decompress_ptr cinfo, boolean require_image);
typedef JDIMENSION              (*jpeg_read_scanlines_t)(j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines);
typedef boolean                 (*jpeg_resync_to_restart_t)(j_decompress_ptr cinfo, int desired);
typedef boolean                 (*jpeg_start_decompress_t)(j_decompress_ptr cinfo);
typedef struct jpeg_error_mgr*  (*jpeg_std_error_t)(struct jpeg_error_mgr* err);

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static jpeg_CreateDecompress_t      jpeg_CreateDecompress_ = lx_null;
static jpeg_destroy_decompress_t    jpeg_destroy_decompress_ = lx_null;
static jpeg_finish_decompress_t     jpeg_finish_decompress_ = lx_null;
static jpeg_read_header_t           jpeg_read_header_ = lx_null;
static jpeg_read_scanlines_t        jpeg_read_scanlines_ = lx_null;
static jpeg_resync_to_restart_t     jpeg_resync_to_restart_ = lx_null;
static jpeg_start_decompress_t      jpeg_start_decompress_ = lx_null;
static jpeg_std_error_t             jpeg_std_error_ = lx_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
static lx_bool_t lx_libjpeg_init() {
    // load symbols from libjpeg.so
    static lx_dlimage_ref_t s_library = lx_null;
    if (!s_library) {
        s_library = lx_dlopen("libjpeg.so", LX_RTLD_LAZY);
        lx_assert_and_check_return_val(s_library, lx_false);

        jpeg_CreateDecompress_   = lx_dlsym(s_library, "jpeg_CreateDecompress");
        jpeg_destroy_decompress_ = lx_dlsym(s_library, "jpeg_destroy_decompress");
        jpeg_finish_decompress_  = lx_dlsym(s_library, "jpeg_finish_decompress");
        jpeg_read_header_        = lx_dlsym(s_library, "jpeg_read_header");
        jpeg_read_scanlines_     = lx_dlsym(s_library, "jpeg_read_scanlines");
        jpeg_resync_to_restart_  = lx_dlsym(s_library, "jpeg_resync_to_restart");
        jpeg_start_decompress_   = lx_dlsym(s_library, "jpeg_start_decompress");
        jpeg_std_error_          = lx_dlsym(s_library, "jpeg_std_error");
    }
    lx_assert_and_check_return_val(jpeg_CreateDecompress_, lx_false);
    return lx_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

