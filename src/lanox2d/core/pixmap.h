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
 * @file        pixmap.h
 *
 */
#ifndef LX_CORE_PIXMAP_H
#define LX_CORE_PIXMAP_H

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

/*! color => pixel
 *
 * @param color         the color
 *
 * @return              the pixel
 */
typedef lx_pixel_t      (*lx_pixmap_func_pixel_t)(lx_color_t color);

/*! pixel => color
 *
 * @param pixel         the pixel
 *
 * @return              the color
 */
typedef lx_color_t      (*lx_pixmap_func_color_t)(lx_pixel_t pixel);

/*! get pixel from data
 *
 * @param data          the data
 *
 * @return              the pixel
 */
typedef lx_pixel_t      (*lx_pixmap_func_pixel_get_t)(lx_cpointer_t data);

/*! set pixel to data
 *
 * @param data          the data
 * @param pixel         the pixel
 * @param alpha         the alpha
 */
typedef lx_void_t       (*lx_pixmap_func_pixel_set_t)(lx_pointer_t data, lx_pixel_t pixel, lx_byte_t alpha);

/*! copy pixel to data from the source data
 *
 * @param data          the data
 * @param source        the source data
 * @param alpha         the alpha
 */
typedef lx_void_t       (*lx_pixmap_func_pixel_copy_t)(lx_pointer_t data, lx_cpointer_t source, lx_byte_t alpha);

/*! get color from data
 *
 * @param data          the data
 *
 * @return              the color
 */
typedef lx_color_t      (*lx_pixmap_func_color_get_t)(lx_cpointer_t data);

/*! set color to data
 *
 * @param data          the data
 * @param color         the color
 */
typedef lx_void_t       (*lx_pixmap_func_color_set_t)(lx_pointer_t data, lx_color_t color);

/*! fill pixels to data
 *
 * @param data          the data
 * @param pixel         the pixel
 * @param count         the count
 */
typedef lx_void_t       (*lx_pixmap_func_pixels_fill_t)(lx_pointer_t data, lx_pixel_t pixel, lx_size_t count, lx_byte_t alpha);

/// the pixmap type
typedef struct lx_pixmap_t_ {

    /// the pixel name
    lx_char_t const*              name;

    /// the bits per-pixel
    lx_byte_t                     bpp;

    /// the bytes per-pixel
    lx_byte_t                     btp;

    /// the pixel format with endian
    lx_uint16_t                   pixfmt;

    /// color => pixel
    lx_pixmap_func_pixel_t        pixel;

    /// pixel => color
    lx_pixmap_func_color_t        color;

    /// get pixel from data
    lx_pixmap_func_pixel_get_t    pixel_get;

    /// set pixel to data
    lx_pixmap_func_pixel_set_t    pixel_set;

    /// copy pixel to data from the source data
    lx_pixmap_func_pixel_copy_t   pixel_copy;

    /// get color from data
    lx_pixmap_func_color_get_t    color_get;

    /// set color to data
    lx_pixmap_func_color_set_t    color_set;

    /// fill pixels to data
    lx_pixmap_func_pixels_fill_t  pixels_fill;

}lx_pixmap_t;

/// the pixmap reference type
typedef lx_pixmap_t const*  lx_pixmap_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the pixmap from the pixel format
 *
 * @param pixfmt        the pixfmt with endian
 * @param alpha         the alpha value, do blend-alpha operation if (alpha >= LX_QUALITY_ALPHA_MIN && alpha <= LX_QUALITY_ALPHA_MAX)
 *
 * @return              the pixmap
 */
lx_pixmap_ref_t         lx_pixmap(lx_size_t pixfmt, lx_byte_t alpha);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


