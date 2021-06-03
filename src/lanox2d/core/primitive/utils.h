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
 * @file        utils.h
 *
 */
#ifndef LX_CORE_PRIMITIVE_UTILS_H
#define LX_CORE_PRIMITIVE_UTILS_H

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

/* make bounds from the points
 *
 * @param bounds    the bounds
 * @param points    the points
 * @param count     the points count
 */
lx_void_t           lx_bounds_make(lx_rect_ref_t bounds, lx_point_ref_t points, lx_size_t count);

/*! apply matrix to the points
 *
 * @param matrix    the matrix
 * @param points    the points
 * @param count     the count
 */
lx_void_t           lx_matrix_apply_points(lx_matrix_ref_t matrix, lx_point_ref_t points, lx_size_t count);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

/*! apply matrix to the x-coordinate
 *
 * x' = x * sx + y * kx + tx
 *
 * @param matrix    the matrix
 * @param x         the x value
 * @param y         the y value
 *
 * @return          the new x value
 */
static lx_inline lx_float_t lx_matrix_apply_x(lx_matrix_ref_t matrix, lx_float_t x, lx_float_t y) {
    lx_assert(matrix);
    return (x * matrix->sx) + (y * matrix->kx) + matrix->tx;
}

/*! apply matrix to the y-coordinate
 *
 * y' = x * ky + y * sy + ty
 *
 * @param matrix    the matrix
 * @param x         the x value
 * @param y         the y value
 *
 * @return          the new y value
 */
static lx_inline lx_float_t lx_matrix_apply_y(lx_matrix_ref_t matrix, lx_float_t x, lx_float_t y) {
    lx_assert(matrix);
    return (x * matrix->ky) + (y * matrix->sy) + matrix->ty;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif


