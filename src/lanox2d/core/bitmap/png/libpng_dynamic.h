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
 * @file        libpng_dynamic.h
 *
 */
#ifndef LX_CORE_BITMAP_PNG_LIBPNG_DYNAMIC_H
#define LX_CORE_BITMAP_PNG_LIBPNG_DYNAMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdlib.h>
#include <setjmp.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define PNG_LIBPNG_VER_STRING "1.6.37"
#define png_jmpbuf(png_ptr) (*png_set_longjmp_fn((png_ptr), longjmp, (sizeof (jmp_buf))))

/* color type masks */
#define PNG_COLOR_MASK_PALETTE    1
#define PNG_COLOR_MASK_COLOR      2
#define PNG_COLOR_MASK_ALPHA      4

/* color types.  Note that not all combinations are legal */
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE    (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)

#define PNG_INFO_tRNS 0x0010U
#define PNG_FILLER_AFTER 1
#define PNG_INTERLACE_NONE 0 /* Non-interlaced image */

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef lx_pointer_t png_structp;
typedef lx_pointer_t png_structrp;
typedef lx_pointer_t png_structpp;
typedef lx_pointer_t png_longjmp_ptr;
typedef lx_pointer_t png_const_charp;
typedef lx_pointer_t png_const_structrp;
typedef lx_pointer_t png_error_ptr;
typedef lx_pointer_t png_voidp;
typedef lx_pointer_t png_bytep;
typedef lx_pointer_t png_infop;
typedef lx_pointer_t png_infopp;
typedef lx_pointer_t png_inforp;
typedef lx_pointer_t png_const_inforp;
typedef lx_pointer_t png_rw_ptr;
typedef lx_pointer_t png_bytepp;
typedef lx_byte_t    png_byte;
typedef size_t       png_size_t;
typedef lx_uint16_t  png_uint_16;
typedef lx_uint32_t  png_uint_32;

typedef struct png_color_16_struct
{
    png_byte index;    /* used for palette files */
    png_uint_16 red;   /* for use in red green blue files */
    png_uint_16 green;
    png_uint_16 blue;
    png_uint_16 gray;  /* for use in grayscale files */
} png_color_16;
typedef png_color_16 * png_color_16p;

typedef jmp_buf*    (*png_set_longjmp_fn_t)(png_structrp png_ptr, png_longjmp_ptr longjmp_fn, size_t jmp_buf_size);
typedef png_voidp   (*png_get_io_ptr_t)(png_const_structrp png_ptr);
typedef png_structp (*png_create_read_struct_t)(png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
typedef png_infop   (*png_create_info_struct_t)(png_const_structrp png_ptr);
typedef void        (*png_set_read_fn_t)(png_structrp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
typedef void        (*png_read_info_t)(png_structrp png_ptr, png_inforp info_ptr);
typedef png_uint_32 (*png_get_IHDR_t)(png_const_structrp png_ptr, png_const_inforp info_ptr, png_uint_32 *width, png_uint_32 *height, int *bit_depth, int *color_type, int *interlace_method, int *compression_method, int *filter_method);
typedef void        (*png_set_strip_16_t)(png_structrp png_ptr);
typedef void        (*png_set_packing_t)(png_structrp png_ptr);
typedef void        (*png_set_expand_gray_1_2_4_to_8_t)(png_structrp png_ptr);
typedef void        (*png_set_gray_to_rgb_t)(png_structrp png_ptr);
typedef png_uint_32 (*png_get_tRNS_t)(png_const_structrp png_ptr, png_inforp info_ptr, png_bytep *trans_alpha, int *num_trans, png_color_16p *trans_color);
typedef png_uint_32 (*png_get_valid_t)(png_const_structrp png_ptr, png_const_inforp info_ptr, png_uint_32 flag);
typedef void        (*png_set_tRNS_to_alpha_t)(png_structrp png_ptr);
typedef void        (*png_set_bgr_t)(png_structrp png_ptr);
typedef void        (*png_set_filler_t)(png_structrp png_ptr, png_uint_32 filler, int flags);
typedef int         (*png_set_interlace_handling_t)(png_structrp png_ptr);
typedef void        (*png_read_update_info_t)(png_structrp png_ptr, png_inforp info_ptr);
typedef void        (*png_read_rows_t)(png_structrp png_ptr, png_bytepp row, png_bytepp display_row, png_uint_32 num_rows);
typedef size_t      (*png_get_rowbytes_t)(png_const_structrp png_ptr, png_const_inforp info_ptr);
typedef void        (*png_destroy_read_struct_t)(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static png_set_longjmp_fn_t             png_set_longjmp_fn = lx_null;
static png_get_io_ptr_t                 png_get_io_ptr = lx_null;
static png_create_read_struct_t         png_create_read_struct = lx_null;
static png_create_info_struct_t         png_create_info_struct = lx_null;
static png_set_read_fn_t                png_set_read_fn = lx_null;
static png_read_info_t                  png_read_info = lx_null;
static png_get_IHDR_t                   png_get_IHDR = lx_null;
static png_set_strip_16_t               png_set_strip_16 = lx_null;
static png_set_packing_t                png_set_packing = lx_null;
static png_set_expand_gray_1_2_4_to_8_t png_set_expand_gray_1_2_4_to_8 = lx_null;
static png_set_gray_to_rgb_t            png_set_gray_to_rgb = lx_null;
static png_get_tRNS_t                   png_get_tRNS = lx_null;
static png_get_valid_t                  png_get_valid = lx_null;
static png_set_tRNS_to_alpha_t          png_set_tRNS_to_alpha = lx_null;
static png_set_bgr_t                    png_set_bgr = lx_null;
static png_set_filler_t                 png_set_filler = lx_null;
static png_set_interlace_handling_t     png_set_interlace_handling = lx_null;
static png_read_update_info_t           png_read_update_info = lx_null;
static png_read_rows_t                  png_read_rows = lx_null;
static png_get_rowbytes_t               png_get_rowbytes = lx_null;
static png_destroy_read_struct_t        png_destroy_read_struct = lx_null;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
static lx_bool_t lx_libpng_init() {

    // load symbols from libpng.so
    static lx_dlimage_ref_t s_library = lx_null;
    if (!s_library) {
        s_library = lx_dlopen("libpng.so", LX_RTLD_LAZY);
        lx_assert_and_check_return_val(s_library, lx_false);

        png_set_longjmp_fn             = lx_dlsym(s_library, "png_set_longjmp_fn");
        png_get_io_ptr                 = lx_dlsym(s_library, "png_get_io_ptr");
        png_create_read_struct         = lx_dlsym(s_library, "png_create_read_struct");
        png_create_info_struct         = lx_dlsym(s_library, "png_create_info_struct");
        png_set_read_fn                = lx_dlsym(s_library, "png_set_read_fn");
        png_read_info                  = lx_dlsym(s_library, "png_read_info");
        png_get_IHDR                   = lx_dlsym(s_library, "png_get_IHDR");
        png_set_strip_16               = lx_dlsym(s_library, "png_set_strip_16");
        png_set_packing                = lx_dlsym(s_library, "png_set_packing");
        png_set_expand_gray_1_2_4_to_8 = lx_dlsym(s_library, "png_set_expand_gray_1_2_4_to_8");
        png_set_gray_to_rgb            = lx_dlsym(s_library, "png_set_gray_to_rgb");
        png_get_tRNS                   = lx_dlsym(s_library, "png_get_tRNS");
        png_get_valid                  = lx_dlsym(s_library, "png_get_valid");
        png_set_tRNS_to_alpha          = lx_dlsym(s_library, "png_set_tRNS_to_alpha");
        png_set_bgr                    = lx_dlsym(s_library, "png_set_bgr");
        png_set_filler                 = lx_dlsym(s_library, "png_set_filler");
        png_set_interlace_handling     = lx_dlsym(s_library, "png_set_interlace_handling");
        png_read_update_info           = lx_dlsym(s_library, "png_read_update_info");
        png_read_rows                  = lx_dlsym(s_library, "png_read_rows");
        png_get_rowbytes               = lx_dlsym(s_library, "png_get_rowbytes");
        png_destroy_read_struct        = lx_dlsym(s_library, "png_destroy_read_struct");
    }
    lx_assert_and_check_return_val(png_create_read_struct, lx_false);
    return lx_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

