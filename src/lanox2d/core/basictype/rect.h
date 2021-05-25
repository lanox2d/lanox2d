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
 * @file        rect.h
 *
 */
#ifndef LX_CORE_BASICTYPE_RECT_H
#define LX_CORE_BASICTYPE_RECT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! make rect
 *
 * @param rect      the rect
 * @param x         the x
 * @param y         the y
 * @param w         the width
 * @param h         the height
 *
 */
lx_void_t           lx_rect_make(lx_rect_ref_t rect, lx_float_t x, lx_float_t y, lx_float_t w, lx_float_t h);

/*! make rect with the integer value
 *
 * @param rect      the rect
 * @param x         the x
 * @param y         the y
 * @param w         the width
 * @param h         the height
 *
 */
lx_void_t           lx_rect_imake(lx_rect_ref_t rect, lx_long_t x, lx_long_t y, lx_size_t w, lx_size_t h);

/* make bounds from the points
 *
 * @param rect      the rect
 * @param points    the points
 * @param count     the points count
 */
lx_void_t           lx_rect_make_bounds(lx_rect_ref_t rect, lx_point_ref_t points, lx_size_t count);

/*! apply matrix to rect
 *
 * @param rect      the rect
 * @param matrix    the matrix
 */
lx_void_t           lx_rect_apply(lx_rect_ref_t rect, lx_matrix_ref_t matrix);

/*! apply matrix to rect
 *
 * @param rect      the rect
 * @param applied   the applied rect
 * @param matrix    the matrix
 */
lx_void_t           lx_rect_apply2(lx_rect_ref_t rect, lx_rect_ref_t applied, lx_matrix_ref_t matrix);

/*! inflate rect
 *
 * @param rect      the rect
 * @param dx        the x-delta value and must be larger than zero
 * @param dy        the x-delta value and must be larger than zero
 */
lx_void_t           lx_rect_inflate(lx_rect_ref_t rect, lx_float_t dx, lx_float_t dy);

/*! deflate rect
 *
 * @param rect      the rect
 * @param dx        the x-delta value and must be smaller than zero
 * @param dy        the x-delta value and must be smaller than zero
 */
lx_void_t           lx_rect_deflate(lx_rect_ref_t rect, lx_float_t dx, lx_float_t dy);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


