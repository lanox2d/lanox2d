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
 * @file        texture.h
 */
#ifndef LX_CORE_TEXTURE_H
#define LX_CORE_TEXTURE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "canvas.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the texture type enum
typedef enum lx_texture_type_e {
    LX_TEXTURE_TYPE_NONE     = 0
,   LX_TEXTURE_TYPE_LINEAR   = 1     //!< the linear gradient texture
,   LX_TEXTURE_TYPE_RADIAL   = 2     //!< the radial gradient texture
,   LX_TEXTURE_TYPE_BITMAP   = 3     //!< the bitmap texture
}lx_texture_type_e;

/// the texture mode enum
typedef enum lx_texture_mode_e {
    LX_TEXTURE_MODE_NONE     = 0
,   LX_TEXTURE_MODE_BORDER   = 1     //!< the border mode
,   LX_TEXTURE_MODE_CLAMP    = 2     //!< the clamp mode
,   LX_TEXTURE_MODE_REPEAT   = 3     //!< the repeat mode
,   LX_TEXTURE_MODE_MIRROR   = 4     //!< the mirror mode
}lx_texture_mode_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init linear gradient texture
 *
 * @param canvas     the canvas
 * @param mode       the mode
 * @param gradient   the gradient
 * @param line       the line
 *
 * @return           the texture
 */
lx_texture_ref_t     lx_texture_init_linear(lx_canvas_ref_t canvas, lx_size_t mode, lx_gradient_ref_t gradient, lx_line_ref_t line);

/*! init linear gradient texture with line: (xb, yb), (xe, ye)
 *
 * @param canvas     the canvas
 * @param mode       the mode
 * @param gradient   the gradient
 * @param xb         the begin x-coordinate
 * @param yb         the begin y-coordinate
 * @param xe         the end x-coordinate
 * @param ye         the end y-coordinate
 *
 * @return           the texture
 */
lx_texture_ref_t     lx_texture_init2_linear(lx_canvas_ref_t canvas, lx_size_t mode, lx_gradient_ref_t gradient, lx_float_t xb, lx_float_t yb, lx_float_t xe, lx_float_t ye);

/*! init linear gradient texture with integer line: (xb, yb), (xe, ye)
 *
 * @param canvas     the canvas
 * @param mode       the mode
 * @param gradient   the gradient
 * @param xb         the begin x-coordinate
 * @param yb         the begin y-coordinate
 * @param xe         the end x-coordinate
 * @param ye         the end y-coordinate
 *
 * @return           the texture
 */
lx_texture_ref_t     lx_texture_init2i_linear(lx_canvas_ref_t canvas, lx_size_t mode, lx_gradient_ref_t gradient, lx_long_t xb, lx_long_t yb, lx_long_t xe, lx_long_t ye);

/*! init radial gradient texture
 *
 * @param canvas     the canvas
 * @param mode       the mode
 * @param gradient   the gradient
 * @param circle     the circle
 *
 * @return           the texture
 */
lx_texture_ref_t     lx_texture_init_radial(lx_canvas_ref_t canvas, lx_size_t mode, lx_gradient_ref_t gradient, lx_circle_ref_t circle);

/*! init radial gradient texture with circle: (x0, y0, r)
 *
 * @param canvas     the canvas
 * @param mode       the mode
 * @param gradient   the gradient
 * @param x0         the x0-coordinate
 * @param y0         the y0-coordinate
 * @param r          the radius
 *
 * @return           the texture
 */
lx_texture_ref_t     lx_texture_init2_radial(lx_canvas_ref_t canvas, lx_size_t mode, lx_gradient_ref_t gradient, lx_float_t x0, lx_float_t y0, lx_float_t r);

/*! init radial gradient texture with integer circle: (x0, y0, r)
 *
 * @param canvas     the canvas
 * @param mode       the mode
 * @param gradient   the gradient
 * @param x0         the x0-coordinate
 * @param y0         the y0-coordinate
 * @param r          the radius
 *
 * @return           the texture
 */
lx_texture_ref_t     lx_texture_init2i_radial(lx_canvas_ref_t canvas, lx_size_t mode, lx_gradient_ref_t gradient, lx_long_t x0, lx_long_t y0, lx_size_t r);

/*! init bitmap texture
 *
 * @param canvas     the canvas
 * @param mode       the mode
 * @param bitmap     the bitmap
 *
 * @return           the texture
 */
lx_texture_ref_t     lx_texture_init_bitmap(lx_canvas_ref_t canvas, lx_size_t mode, lx_bitmap_ref_t bitmap);

/*! exit texture
 *
 * @param texture    the texture
 */
lx_void_t            lx_texture_exit(lx_texture_ref_t texture);

/*! the texture type
 *
 * @param texture    the texture
 *
 * @return           the type
 */
lx_size_t            lx_texture_type(lx_texture_ref_t texture);

/*! the texture mode
 *
 * @param texture    the texture
 *
 * @return           the mode
 */
lx_size_t            lx_texture_mode(lx_texture_ref_t texture);

/*! the texture matrix
 *
 * @param texture    the texture
 *
 * @return           the matrix
 */
lx_matrix_ref_t      lx_texture_matrix(lx_texture_ref_t texture);

/*! set the texture matrix
 *
 * @param texture    the texture
 * @param matrix     the matrix
 */
lx_void_t            lx_texture_matrix_set(lx_texture_ref_t texture, lx_matrix_ref_t matrix);

/*! the texture reference count
 *
 * @param texture   the texture
 *
 * @return          the texture reference count
 */
lx_size_t           lx_texture_ref(lx_texture_ref_t texture);

/*! increase the texture reference count
 *
 * @param texture   the texture
 */
lx_void_t           lx_texture_inc(lx_texture_ref_t texture);

/*! decrease the texture reference count, will free it if --refn == 0
 *
 * @param texture   the texture
 */
lx_void_t           lx_texture_dec(lx_texture_ref_t texture);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
