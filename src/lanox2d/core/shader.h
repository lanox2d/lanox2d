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
 * @file        shader.h
 */
#ifndef LX_CORE_SHADER_H
#define LX_CORE_SHADER_H

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

/// the shader type enum
typedef enum lx_shader_type_e_ {
    LX_SHADER_TYPE_NONE            = 0
,   LX_SHADER_TYPE_BITMAP          = 1   //!< the bitmap shader
,   LX_SHADER_TYPE_LINEAR_GRADIENT = 2   //!< the linear gradient shader
,   LX_SHADER_TYPE_RADIAL_GRADIENT = 3   //!< the radial gradient shader
}lx_shader_type_e;

/// the shader tile mode enum
typedef enum lx_shader_tile_mode_e_ {
    LX_SHADER_TILE_MODE_NONE     = 0
,   LX_SHADER_TILE_MODE_BORDER   = 1     //!< the border tile mode
,   LX_SHADER_TILE_MODE_CLAMP    = 2     //!< the clamp tile mode
,   LX_SHADER_TILE_MODE_REPEAT   = 3     //!< the repeat tile mode
,   LX_SHADER_TILE_MODE_MIRROR   = 4     //!< the mirror tile mode
}lx_shader_tile_mode_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init linear gradient shader
 *
 * @param tile_mode  the tile mode
 * @param gradient   the gradient
 * @param line       the line
 *
 * @return           the shader
 */
lx_shader_ref_t      lx_shader_init_linear_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_line_ref_t line);

/*! init linear gradient shader with line: (xb, yb), (xe, ye)
 *
 * @param tile_mode  the tile mode
 * @param gradient   the gradient
 * @param xb         the begin x-coordinate
 * @param yb         the begin y-coordinate
 * @param xe         the end x-coordinate
 * @param ye         the end y-coordinate
 *
 * @return           the shader
 */
lx_shader_ref_t      lx_shader_init2_linear_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_float_t xb, lx_float_t yb, lx_float_t xe, lx_float_t ye);

/*! init linear gradient shader with integer line: (xb, yb), (xe, ye)
 *
 * @param tile_mode  the tile mode
 * @param gradient   the gradient
 * @param xb         the begin x-coordinate
 * @param yb         the begin y-coordinate
 * @param xe         the end x-coordinate
 * @param ye         the end y-coordinate
 *
 * @return           the shader
 */
lx_shader_ref_t      lx_shader_init2i_linear_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_long_t xb, lx_long_t yb, lx_long_t xe, lx_long_t ye);

/*! init radial gradient shader
 *
 * @param tile_mode  the tile mode
 * @param gradient   the gradient
 * @param circle     the circle
 *
 * @return           the shader
 */
lx_shader_ref_t      lx_shader_init_radial_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_circle_ref_t circle);

/*! init radial gradient shader with circle: (x0, y0, r)
 *
 * @param tile_mode  the tile mode
 * @param gradient   the gradient
 * @param x0         the x0-coordinate
 * @param y0         the y0-coordinate
 * @param r          the radius
 *
 * @return           the shader
 */
lx_shader_ref_t      lx_shader_init2_radial_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_float_t x0, lx_float_t y0, lx_float_t r);

/*! init radial gradient shader with integer circle: (x0, y0, r)
 *
 * @param tile_mode  the tile mode
 * @param gradient   the gradient
 * @param x0         the x0-coordinate
 * @param y0         the y0-coordinate
 * @param r          the radius
 *
 * @return           the shader
 */
lx_shader_ref_t      lx_shader_init2i_radial_gradient(lx_size_t tile_mode, lx_gradient_ref_t gradient, lx_long_t x0, lx_long_t y0, lx_size_t r);

/*! init bitmap shader
 *
 * @param tile_mode  the tile mode
 * @param bitmap     the bitmap
 *
 * @return           the shader
 */
lx_shader_ref_t      lx_shader_init_bitmap(lx_size_t tile_mode, lx_bitmap_ref_t bitmap);

/*! exit shader
 *
 * @param shader     the shader
 */
lx_void_t            lx_shader_exit(lx_shader_ref_t shader);

/*! the shader type
 *
 * @param shader     the shader
 *
 * @return           the type
 */
lx_size_t            lx_shader_type(lx_shader_ref_t shader);

/*! the shader tile tile_mode
 *
 * @param shader     the shader
 *
 * @return           the tile tile_mode
 */
lx_size_t            lx_shader_tile_mode(lx_shader_ref_t shader);

/*! the shader matrix
 *
 * @param shader     the shader
 *
 * @return           the matrix
 */
lx_matrix_ref_t      lx_shader_matrix(lx_shader_ref_t shader);

/*! set the shader matrix in global coordinate
 *
 *   ----------> x
 *  |
 *  |
 * \|/
 *  y
 *
 * @param shader     the shader
 * @param matrix     the matrix
 */
lx_void_t            lx_shader_matrix_set(lx_shader_ref_t shader, lx_matrix_ref_t matrix);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
