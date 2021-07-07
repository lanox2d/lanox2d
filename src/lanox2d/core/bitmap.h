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
 * @file        bitmap.h
 *
 */
#ifndef LX_CORE_BITMAP_H
#define LX_CORE_BITMAP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init bitmap
 *
 * @param data      the data, will auto make data if be null
 * @param pixfmt    the pixfmt
 * @param width     the width
 * @param height    the height
 * @param row_bytes the row bytes, will auto calculate it using width if be zero
 * @param has_alpha has alpha?
 *
 * @return          the bitmap
 */
lx_bitmap_ref_t     lx_bitmap_init(lx_pointer_t data, lx_size_t pixfmt, lx_size_t width, lx_size_t height, lx_size_t row_bytes, lx_bool_t has_alpha);

/*! init bitmap from url
 *
 * @param url       the bitmap url
 * @param pixfmt    the pixfmt
 *
 * @return          the bitmap
 */
lx_bitmap_ref_t     lx_bitmap_init_from_url(lx_char_t const* url, lx_size_t pixfmt);

/*! exit bitmap
 *
 * @param bitmap    the bitmap
 */
lx_void_t           lx_bitmap_exit(lx_bitmap_ref_t bitmap);

/*! the bitmap data size
 *
 * @param bitmap    the bitmap
 *
 * @return          the bitmap data size
 */
lx_size_t           lx_bitmap_size(lx_bitmap_ref_t bitmap);

/*! the bitmap data
 *
 * @param bitmap    the bitmap
 *
 * @return          the bitmap data
 */
lx_pointer_t        lx_bitmap_data(lx_bitmap_ref_t bitmap);

/*! attach the bitmap data
 *
 * @param bitmap    the bitmap
 * @param data      the bitmap data
 * @param width     the width
 * @param height    the height
 * @param row_bytes the row bytes, will auto calculate it using width if be zero
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_bitmap_attach(lx_bitmap_ref_t bitmap, lx_pointer_t data, lx_size_t width, lx_size_t height, lx_size_t row_bytes);

/*! resize the bitmap
 *
 * @param bitmap    the bitmap
 * @param width     the width
 * @param height    the height
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_bitmap_resize(lx_bitmap_ref_t bitmap, lx_size_t width, lx_size_t height);

/*! the bitmap width
 *
 * @param bitmap    the bitmap
 *
 * @return          the bitmap width
 */
lx_size_t           lx_bitmap_width(lx_bitmap_ref_t bitmap);

/*! the bitmap height
 *
 * @param bitmap    the bitmap
 *
 * @return          the bitmap height
 */
lx_size_t           lx_bitmap_height(lx_bitmap_ref_t bitmap);

/*! the bitmap pixfmt
 *
 * @param bitmap    the bitmap
 *
 * @return          the bitmap pixfmt
 */
lx_size_t           lx_bitmap_pixfmt(lx_bitmap_ref_t bitmap);

/*! the bitmap has alpha data?
 *
 * @param bitmap    the bitmap
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_bitmap_has_alpha(lx_bitmap_ref_t bitmap);

/*! set the bitmap alpha: exists or not?
 *
 * @param bitmap    the bitmap
 * @param has_alpha has alpha value?
 */
lx_void_t           lx_bitmap_set_alpha(lx_bitmap_ref_t bitmap, lx_bool_t has_alpha);

/*! the bitmap row bytes
 *
 * @param bitmap    the bitmap
 *
 * @return          the bitmap row bytes
 */
lx_size_t           lx_bitmap_row_bytes(lx_bitmap_ref_t bitmap);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


